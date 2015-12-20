// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// BSplineCurve
//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::~BSplineCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(uint32_t order, uint32_t dims) :
  CurveBase(order, dims)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(uint32_t order, uint32_t dims, uint32_t number) :
  CurveBase(order, dims, number + order, dims, number)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(uint32_t order, uint32_t dims, const Vector<ScalarType>& knots) :
  CurveBase(order, dims, knots.numElements(), dims, knots.numElements() - order)
{
  m_knots = knots;
  m_coefs.set(static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(
  uint32_t order, uint32_t dims, const Vector<ScalarType>& knots, const Matrix<ScalarType>& coefs) :
  CurveBase(order, dims, knots, coefs)
{
  NMP_ASSERT(m_coefs.numRows() == m_dims);
  NMP_ASSERT(m_coefs.numColumns() + m_order == m_knots.numElements());
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(const BSplineCurve& curve) :
  CurveBase(curve.m_order, curve.m_dims, curve.m_knots, curve.m_coefs)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(uint32_t order, uint32_t dims, uint32_t spans, const Vector<ScalarType>& x) :
  CurveBase(order, dims, numKnotsFromSpans(order, spans), dims, numCtrlPtsFromSpans(order, spans))
{
  knotsOpen(x);
  m_coefs.set(static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>::BSplineCurve(
  uint32_t order, uint32_t dims, const Vector<ScalarType>& breaks, const Vector<int32_t>& mults) :
  CurveBase(order, dims)
{
  knots(breaks, mults);
  uint32_t numCtrlPts = numCtrlPtsFromKnots(order, m_knots.numElements());
  Matrix<ScalarType> coefs(dims, numCtrlPts);
  m_coefs.swap(coefs);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::endSpan() const
{
  uint32_t n = m_knots.numElements();
  NMP_ASSERT(n > m_order);

  uint32_t i = n - m_order;
  ScalarType last = m_knots[i];
  for (--i; i > 1; --i) if (m_knots[i] != last) break;
  return i;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::basis(uint32_t i, ScalarType x, Vector<ScalarType>& Nik) const
{
  Vector<ScalarType> left(m_order);
  Vector<ScalarType> right(m_order);
  ScalarType saved;

  // DEBUG: sanity check for t_i <= x < t_{i+1}
  NMP_ASSERT(x >= m_knots[i] && x <= m_knots[i+1]); // Make sure end knot is valid! (l.e.)

  // Compute the non-vanishing basis functions: N_{i-k+1,k}, ... , N_{i,k}
  Nik[0] = static_cast<ScalarType>(1);
  for (uint32_t j = 1; j < m_order; ++j)
  {
    left[j] = x - m_knots[i+1-j];
    right[j] = m_knots[i+j] - x;
    saved = static_cast<ScalarType>(0);

    for (uint32_t r = 0; r < j; ++r)
    {
      ScalarType temp = Nik[r] / (right[r+1] + left[j-r]);
      Nik[r] = saved + right[r+1] * temp;
      saved = left[j-r] * temp;
    }
    Nik[j] = saved;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::basisDerivatives(uint32_t i, ScalarType x, Matrix<ScalarType>& DNik) const
{
  NMP_ASSERT(DNik.numColumns() > 0 && DNik.numRows() == m_order);

  // Workspace memory for the basis function and knot differences
  Matrix<ScalarType> Ndt(m_order, m_order);

  // Workspace memory
  Vector<ScalarType> left(m_order);
  Vector<ScalarType> right(m_order);
  Vector<ScalarType> p(m_order); // columns of upper triangle
  Vector<ScalarType> v(m_order); // rows of lower triangle
  ScalarType saved;

  // Get the number of derivatives to compute: m+1
  uint32_t n = DNik.numColumns();

  // DEBUG: sanity check for t_i <= x < t_{i+1}
  NMP_ASSERT(x >= m_knots[i] && x <= m_knots[i+1]); // Make sure end knot is valid! (l.e.)

  // Build the basis function and knot difference table
  p[0] = static_cast<ScalarType>(1);
  Ndt.element(0, 0) = p[0];
  for (uint32_t j = 1; j < m_order; ++j)
  {
    // Compute the left and right vectors
    left[j] = x - m_knots[i+1-j];
    right[j] = m_knots[i+j] - x;
    saved = static_cast<ScalarType>(0);

    for (uint32_t r = 0; r < j; ++r)
    {
      // Compute the knot difference value (lower triangle)
      v[r] = (right[r+1] + left[j-r]);

      // At this point p[r] contains the value of the previous
      // table column Ndt[r][j-1]
      ScalarType temp = p[r] / v[r];

      // Compute the basis function value (upper triangle)
      p[r] = saved + right[r+1] * temp;
      saved = left[j-r] * temp;
    }
    p[j] = saved; // basis function on diagonal

    // Store the computed values within the table
    Ndt.setRowVector(j, 0, j, &v[0]);
    Ndt.setColumnVector(j, 0, j + 1, &p[0]);
  }

  // Set the zero^th derivative basis functions
  DNik.setColumnVector(0, p);

  // Set any trivial zero basis function derivatives
  if (n > m_order)
  {
    for (uint32_t m = m_order; m < n; ++m)
    {
      DNik.setColumnVector(m, static_cast<ScalarType>(0));
    }
    n = m_order; // valid number of derivatives
  }

  ///////////////////////////////////////////////////////////
  // Compute the remaining [1:m] basis function derivatives
  //                  (k-1)!
  // D^(m)N_{i,k} = ---------- * \sum_{j=0}^(m) a_{m,j} N_{i+j, k-m}
  //                (k-1 - m)!

  // Iterate over the basis function index N_{i-k+1,*}, ... , N_{i,*} to
  // build up the m^th derivatives corresponding to each basis function
  for (uint32_t r = 0; r < m_order; ++r)
  {
    // Set the old a_{0,0} vector element
    v[0] = static_cast<ScalarType>(1);

    // Loop to compute the m^th derivatives on basis function r
    for (uint32_t m = 1; m < n; ++m)
    {
      // Initialize the derivative and compute index offsets
      ScalarType d = static_cast<ScalarType>(0);
      int32_t rm = r - m;
      uint32_t km = m_order - m;

      // Get the km^th row vector and the (km-1)^th column vector
      // from the basis function and knot difference table
      Ndt.getRowVector(km, left); // Ndt[km][:]
      Ndt.getColumnVector(km - 1, right); // Ndt[:][km-1]

      // Leftmost case: a_{m,0} = a_{m-1,0} / (t_{i+k-m} - t_{i})
      if (r >= m)
      {
        p[0] = v[0] / left[rm]; // a_{m,0}
        d = p[0] * right[rm]; // a_{m,0} * N_{i,k-m}
      }

      // Find the valid index segment for the general case
      uint32_t j1, j2;
      if (rm >= -1) j1 = 1; else j1 = (uint32_t) - rm;
      if (r <= km) j2 = m; else j2 = m_order - r;

      // General case: a_{m,j} = (a_{m-1,j} - a_{m-1,j-1}) / (t_{i+j+k-m} - t_{i+j})
      for(uint32_t j = j1; j < j2; ++j)
      {
        uint32_t rmj = rm + j;
        p[j] = (v[j] - v[j - 1]) / left[rmj]; // a_{m,j}
        d += p[j] * right[rmj]; // a_{m,j} * N_{i+j,k-m}
      }

      // Rightmost case: a_{m,m} = -a_{m-1,m-1} / (t_{i+k} - t_{i+m})
      if (r < km)
      {
        p[m] = -v[m-1] / left[r];
        d += p[m] * right[r];
      }

      // Set the basis function derivative (without scaling factor)
      DNik.element(r, m) = d;

      // Move the computed a_{k,j} vector to the a_{k-1,j} vector
      p.swap(v);
    }
  }

  // Multiply the coefficients of the basis function derivatives by
  // the correct factors (k-1)! / (k-1 - m)!
  uint32_t c = m_order - 1;
  ScalarType fac = (ScalarType)c;
  for (uint32_t m = 1; m < n; ++m)
  {
    DNik.multiplyColumn(m, fac);
    c--;
    fac *= (ScalarType)c;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::spansNum() const
{
  if (m_coefs.isEmpty()) return 0;
  NMP_ASSERT(m_coefs.numColumns() >= m_order);
  return m_coefs.numColumns() - m_order + 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::multiplicityStart() const
{
  NMP_ASSERT(m_knots.numElements() > 0);
  uint32_t mc = 1;
  for (uint32_t i = 1; i < m_knots.numElements(); ++i, ++mc)
  {
    if (m_knots[i] != m_knots[0]) break;
  }
  return mc;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::multiplicityEnd() const
{
  NMP_ASSERT(m_knots.numElements() > 0);
  uint32_t mc = 1;
  uint32_t ext = m_knots.numElements() - 1;
  for (int32_t i = (int32_t)ext - 1; i >= 0; --i, ++mc)
  {
    if (m_knots[i] != m_knots[ext]) break;
  }
  return mc;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::
knotsOpenUniform(ScalarType start, ScalarType end)
{
  NMP_ASSERT(end >= start);

  uint32_t n = m_knots.numElements();
  NMP_ASSERT(n > 1);
  uint32_t ext = n - 1;
  for (uint32_t i = 0; i < m_order; ++i)
  {
    m_knots[i] = start;
    m_knots[ext-i] = end;
  }

  uint32_t d = m_order - 1;
  n -= 2 * d;
  NMP_ASSERT(n >= 2);
  n--; // adjust
  ScalarType m = (end - start) / n;
  for (uint32_t i = 1; i < n; ++i) m_knots[d+i] = m * i + start;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::knotsOpenArcLength(const Matrix<ScalarType>& X)
{
  uint32_t m = m_knots.numElements();
  uint32_t n = X.numColumns();
  uint32_t dims = X.numRows(); // may be different to m_dims
  uint32_t p = m_order - 1;
  NMP_ASSERT(n + 2 * p == m);
  NMP_ASSERT(m > 1);
  NMP_ASSERT(dims > 0);

  Vector<ScalarType> v(dims);
  Vector<ScalarType> dv(dims);

  // Set start knot multiplicity
  for (uint32_t i = 0; i < m_order; ++i) m_knots[i] = 0;

  // Compute cumulative arc lengths
  for (uint32_t i = 1; i < n; ++i)
  {
    X.getColumnVector(i - 1, v);
    X.getColumnVector(i, dv);
    dv -= v;
    uint32_t indx = i + p;
    m_knots[indx] = NMP::enorm<ScalarType, ScalarType>(dv) + m_knots[indx-1];
  }

  // Set end knot multiplicity
  for (uint32_t i = m - p; i < m; ++i) m_knots[i] = m_knots[i-1];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::knotsOpen(const Vector<ScalarType>& x)
{
  uint32_t nx = x.numElements();
  NMP_ASSERT(m_order <= nx);
  NMP_ASSERT(m_knots.numElements() >= m_order);

  uint32_t sp = spansNum();
  NMP_ASSERT(sp <= (nx - m_order + 1)); // Max spans

  // Check for single span and order 1
  if (m_order == 1 && sp == 1)
  {
    m_knots[0] = x[0];
    if (nx == 1)
      m_knots[1] = x[0] + 1;
    else
      m_knots[1] = x[nx-1];
  }
  else
  {
    uint32_t numC = m_coefs.numColumns();
    Vector<ScalarType> xu(numC);

    // Compute the sampled parameter vector x
    ScalarType fac = (nx - 1) / (ScalarType)(numC - 1);
    for (uint32_t i = 0; i < numC; ++i)
    {
      uint32_t indx = (uint32_t)(i * fac + static_cast<ScalarType>(0.5));
      xu[i] = x[indx];
    }

    // Set first and last knot multiplicities
    uint32_t ext = numC - 1;
    for (uint32_t i = 0; i < m_order; ++i)
    {
      m_knots[i] = xu[0];
      m_knots.end(i) = xu[ext];
    }

    // Use midpoints between data sites if order is one
    uint32_t n = numC - m_order;
    if (m_order == 1)
    {
      for (uint32_t i = 1; i <= n; ++i) m_knots[i] = (xu[i] + xu[i-1]) / 2;
    }
    else
    {
      // Check for second order
      if (m_order == 2)
      {
        for (uint32_t i = 1; i <= n; ++i) m_knots[i+1] = xu[i];
      }
      else
      {
        // Compute the averages of the successive order-1 knots
        uint32_t p = m_order - 1;
        for (uint32_t i = 1; i <= n; ++i)
        {
          ScalarType f = xu[i];
          for (uint32_t k = 1; k < p; ++k) f += xu[i+k];
          m_knots[i+p] = f / p;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::numKnotsFromSpans(uint32_t order, uint32_t n)
{
  NMP_ASSERT(order > 0);
  return n + 2 * order - 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::numCtrlPtsFromKnots(uint32_t order, uint32_t numKnots)
{
  NMP_ASSERT(numKnots > order + 2);
  return numKnots - order;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::numCtrlPtsFromSpans(uint32_t order, uint32_t n)
{
  NMP_ASSERT(order > 0);
  return n + order - 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::numKnotsFromSpans(uint32_t n) const
{
  NMP_ASSERT(m_order > 0);
  return n + 2 * m_order - 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::numCtrlPtsFromSpans(uint32_t n) const
{
  NMP_ASSERT(m_order > 0);
  return n + m_order - 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::insertKnot(ScalarType x, uint32_t mult)
{
  NMP_ASSERT(m_order > 0);
  if (mult == 0) return;
  uint32_t degree = m_order - 1;

  // Compute the curve interval for x
  uint32_t intv;
  if (interval(x, intv) != 0) return;

  // Compute the new knot vector
  uint32_t numKnots = m_knots.numElements();
  Vector<ScalarType> knots(numKnots + mult);
  for (uint32_t i = 0; i <= intv; ++i) knots[i] = m_knots[i];
  for (uint32_t i = 1; i <= mult; ++i) knots[intv+i] = x;
  for (uint32_t i = intv + 1; i < numKnots; ++i) knots[i+mult] = m_knots[i];

  // Allocate memory of the new control points
  uint32_t numDims = m_coefs.numRows();
  uint32_t numControlPts = m_coefs.numColumns();
  uint32_t numControlPts_ = numControlPts + mult;
  Matrix<ScalarType> coefs(numDims, numControlPts_);

  // vector for control points
  Vector<ScalarType> va(numDims);

  // Check if x is the end knot
  bool isEnd = isEndKnot(x);

  // Compute the knot multiplicity at x
  uint32_t s = 0;
  if (isEnd)
  {
    s = m_knots.numElements() - 1 - intv;
  }
  else
  {
    for (int32_t i = intv; i >= 0; --i)
    {
      if (m_knots[i] != x) break;
      ++s;
    }
  }

  // Check if we need to perform the knot insertion algorithm
  if (s >= degree)
  {
    if (isEnd)
    {
      // Special case for end knots where all alphas are 1
      coefs.setSubMatrix(0, 0, 0, 0, numDims, numControlPts, m_coefs);
      m_coefs.getColumnVector(numControlPts - 1, va);
      for (uint32_t i = 0; i < mult; ++i)
      {
        coefs.setColumnVector(i + numControlPts, va);
      }
    }
    else
    {
      // General case where all alphas are 0
      uint32_t indx = intv - degree;
      coefs.setSubMatrix(0, 0, 0, 0, numDims, indx, m_coefs);
      m_coefs.getColumnVector(indx, va);
      for (uint32_t i = 0; i < mult; ++i)
      {
        coefs.setColumnVector(i + indx, va);
      }
      coefs.setSubMatrix(0, indx, 0, indx + mult, numDims, numControlPts - indx, m_coefs);
    }

    // Set the new data vectors
    m_knots.swap(knots);
    m_coefs.swap(coefs);
    return;
  }

  uint32_t indxP = intv - degree + 1; // start index position for control points (to alter)
  uint32_t indxPe = intv - s; // start position of control points (unaltered)
  uint32_t indxPe_ = indxPe + mult; // start position of the new control points
  _ASSERT(indxPe >= indxP);

  // Store the unaltered control points
  coefs.setSubMatrix(0, 0, 0, 0, numDims, indxP, m_coefs);
  coefs.setSubMatrix(0, indxPe, 0, indxPe_, numDims, numControlPts - indxPe, m_coefs);

  // Temporaries for the control point coefficient table
  Vector<ScalarType> vb(numDims); // vector for control points
  Matrix<ScalarType> A(numDims, m_order); // control point coefficient table

  // Perform knot insertion
  uint32_t r = mult;
  uint32_t ds = degree - s; // number of knots we can possibly add in the algorithm
  if (r > ds) r = ds; // number of knots to actually add in algorithm
  A.setSubMatrix(0, indxP - 1, 0, 0, numDims, ds + 1, m_coefs); // Load initial control points into table
  --indxPe_; // index of rightmost new control point

  // Insert the knot r times
  int32_t colRem = ds - 1; // number of internal coefficients remaining in table column
  for (uint32_t j = 0; j < r; ++j)
  {
    uint32_t indx = indxP + j; // index for knot alphas

    // Compute coefficient table entries
    for (uint32_t i = 0; i < ds - j; ++i)
    {
      A.getColumnVector(i, va);
      A.getColumnVector(i + 1, vb);

      // Compute alpha coefficient
      ScalarType alpha = (x - m_knots[indx+i]) / (m_knots[i+intv+1] - m_knots[indx+i]);
      vb *= alpha;
      va *= (1 - alpha);
      va += vb;
      A.setColumnVector(i, va);
    }

    // Update the control points
    coefs.setSubMatrix(0, 0, 0, indx, numDims, 1, A);
    coefs.setSubMatrix(0, ds - j - 1, 0, indxPe_ - j, numDims, 1, A);
    colRem--;
  }

  // Store the remaining control points
  if (colRem > 0)
  {
    uint32_t cb = colRem >> 1;
    uint32_t ca = colRem - cb;
    coefs.setSubMatrix(0, 1, 0, indxP + r, numDims, ca, A);
    coefs.setSubMatrix(0, 1 + ca, 0, indxPe_ - r, numDims, cb, A);
  }

  // Insert any remaining knots
  for (uint32_t j = r; j < mult - 1; ++j)
  {
    coefs.setSubMatrix(0, 0, 0, indxP + j, numDims, 1, A);
  }

  // Set the new data vectors
  m_knots.swap(knots);
  m_coefs.swap(coefs);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::removeKnot(uint32_t NMP_UNUSED(intv), uint32_t NMP_UNUSED(mult))
{
  // This specialisation is not currently supported.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::getNumControlPoints() const
{
  return m_coefs.numColumns();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::getControlPointIndexFromKnotInterval(uint32_t intv) const
{
  return intv - (m_order - 1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BSplineCurve<ScalarType>::getKnotIntervalFromControlPointIndex(uint32_t i) const
{
  return i + (m_order - 1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::getControlPoint(uint32_t i, Vector<ScalarType>& x) const
{
  m_coefs.getColumnVector(i, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::setControlPoint(uint32_t i, const Vector<ScalarType>& x)
{
  m_coefs.setColumnVector(i, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::getControlPoint(uint32_t i, ScalarType* x) const
{
  m_coefs.getColumnVector(i, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::setControlPoint(uint32_t i, const ScalarType* x)
{
  m_coefs.setColumnVector(i, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::linearControlPoints(const Vector<ScalarType>& x0, const Vector<ScalarType>& x1)
{
  NMP_ASSERT(x0.numElements() == m_dims);
  NMP_ASSERT(x1.numElements() == m_dims);
  uint32_t n = m_coefs.numColumns();
  NMP_ASSERT(n >= 2);

  uint32_t numSpans = n - 1;
  ScalarType fac = static_cast<ScalarType>(1) / numSpans;

  Vector<ScalarType> a(m_dims);
  Vector<ScalarType> b(m_dims);
  m_coefs.setColumnVector(0, x0);
  for (uint32_t i = 1; i < numSpans; ++i)
  {
    ScalarType t = i * fac;
    a = x0; a *= (1 - t);
    b = x1; b *= t;
    a += b;
    m_coefs.setColumnVector(i, a);
  }
  m_coefs.setColumnVector(numSpans, x1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::transformControlPoints(
  const Vector<ScalarType>& scales, const Vector<ScalarType>& offsets)
{
  Vector<ScalarType> v(m_dims);
  uint32_t numCtrlPts = getNumControlPoints();
  for (uint32_t i = 0; i < numCtrlPts; ++i)
  {
    getControlPoint(i, v);
    for (uint32_t k = 0; k < m_dims; ++k)
    {
      v[k] = scales[k] * v[k] + offsets[k];
    }
    setControlPoint(i, v);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t BSplineCurve<ScalarType>::evaluate(ScalarType x, uint32_t& intv, Vector<ScalarType>& v, bool flag) const
{
  NMP_ASSERT(v.numElements() == m_dims);

  v.set((ScalarType)0);
  if (m_order == 0) return 0;

  // Find the interval
  int32_t rgn = interval(x, intv, flag);

  // Check if the parameter is within the basic interval
  if (rgn != 0) return rgn;

  // Compute the non vanishing basis functions on the interval
  Vector<ScalarType> Nik(m_order);
  basis(intv, x, Nik);

  // Adjust interval to control point range
  intv -= (m_order - 1);

  // Evaluate the point on the curve span
  Vector<ScalarType> Pi(m_dims);
  for (uint32_t i = 0; i < m_order; ++i)
  {
    m_coefs.getColumnVector(intv + i, Pi);
    Pi *= Nik[i];
    v += Pi;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t BSplineCurve<ScalarType>::derivative(
  uint32_t j, ScalarType x, uint32_t& intv, Vector<ScalarType>& v, bool flag) const
{
  NMP_ASSERT(v.numElements() == m_dims);

  // Check if we are evaluating the zero^th derivative
  if (j == 0) return evaluate(x, intv, v, flag);

  v.set((ScalarType)0);
  if (m_order == 0) return 0;

  // Find the interval
  int32_t rgn = interval(x, intv, flag);

  // Check if the parameter is within the basic interval
  if (rgn != 0) return rgn;

  // Compute the (j+1) non-vanishing derivatives of the basis
  // functions at x on the interval
  Matrix<ScalarType> DNik(m_order, j + 1);
  basisDerivatives(intv, x, DNik);

  // Get the basis function coefficients of the j^th derivative
  const ScalarType* coefs = &DNik.element(0, j);

  // Adjust interval to control point range
  intv -= (m_order - 1);

  // Evaluate the point on the curve span
  Vector<ScalarType> Pi(m_dims);
  for (uint32_t i = 0; i < m_order; ++i)
  {
    m_coefs.getColumnVector(intv + i, Pi);
    Pi *= coefs[i];
    v += Pi;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* BSplineCurve<ScalarType>::derivative(uint32_t j) const
{
  uint32_t number = m_coefs.numColumns();
  BSplineCurve<ScalarType>* pCurve;

  // Copy of curve
  if (j == 0)
  {
    pCurve = new BSplineCurve<ScalarType>(*this);
    NMP_ASSERT(pCurve != NULL);
    return pCurve;
  }

  // Require derivative of curve greater than its order
  if (j >= m_order)
  {
    number = m_knots.numElements() - 1;
    pCurve = new BSplineCurve<ScalarType>(1, m_dims, number);
    NMP_ASSERT(pCurve != NULL);
    pCurve->m_knots = m_knots;
    pCurve->m_coefs.set((ScalarType)0);
    return pCurve;
  }

  // Create a new curve with the required order
  uint32_t neworder = m_order - j;
  pCurve = new BSplineCurve<ScalarType>(neworder, m_dims, number - j);
  NMP_ASSERT(pCurve != NULL);

  // D^(j)C(x) = \sum_{ i=1 }^{ n-j } N_{ i, k-j }(x) * D^(j)P_i

  // D^(j)P_i = | P_i   if  j=0
  //            | ((k-j) / (t_{ i+k } - t_{ i+j })) * (D^(j-1)P_{ i+1 } - D^(j-1)P_i)  if  j>0
  Matrix<ScalarType> Cp(m_coefs);
  Vector<ScalarType> v(m_dims);
  Vector<ScalarType> p(m_dims);
  Vector<ScalarType> newKnots(m_knots);

  // Iterate over derivatives
  for (uint32_t m = 1; m <= j; ++m)
  {
    ScalarType kmj = (ScalarType)(m_order - m);
    for (uint32_t i = 0; i < number - m; ++i)
    {
      Cp.getColumnVector(i, v); // D^(j-1)P_i
      Cp.getColumnVector(i + 1, p); // D^(j-1)P_{i+1}
      p -= v; // D^(j-1)P_{i+1} - D^(j-1)P_i
      p *= (kmj / (m_knots[i+m_order] - m_knots[i+m]));
      Cp.setColumnVector(i, p);
    }
  }

  // Set the new control points
  Cp.getSubMatrix(0, 0, pCurve->m_coefs);

  // Set the new knot vector by dropping the j first and last elements
  // D^(j)knots = [a,...,a, t_{k+1}, ... , t_{z-k}, b,...,b]
  pCurve->m_knots.set(&m_knots[j]);

  return pCurve;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* BSplineCurve<ScalarType>::integral(const Vector<ScalarType>& va) const
{
  NMP_ASSERT(va.numElements() == m_dims);

  uint32_t number = m_coefs.numColumns();
  uint32_t kn = m_knots.numElements();
  NMP_ASSERT(number > 1 && kn > 1);

  // New order of the curve
  uint32_t neworder = m_order + 1;

  // Q_i = Q_{i-1} + P_i * (t_{i+k} - t_i) / k
  // where Q_i are the B-spline coefficients of the anti-derivative
  // P_i are the coefficients of the B-spline f(x)
  // t_i are the knot vector elements
  // k is the order of the B-spline f(x)
  BSplineCurve<ScalarType>* pCurve;

  // We need to ensure that the anti-derivative has finitely many knots
  // by adjoining suitable zero terms. i.e. ensure that t_{i+k} - t_i = 0
  // at both ends of the knot vector.
  uint32_t mcl = multiplicityStart();
  uint32_t mcr = multiplicityEnd();
  ScalarType kstart = m_knots[0];
  ScalarType kend = m_knots[m_knots.numElements()-1];

  // Adjust the number of required control points
  uint32_t addl = 0;
  if (mcl < m_order)
  {
    addl = m_order - mcl;
    number += addl;
  }
  uint32_t addr = 0;
  if (mcr < m_order)
  {
    addr = m_order - mcr;
    number += addr;
  }

  // Now add the additional Va control point to the start of
  // the sequence
  addl++; number++;
  pCurve = new BSplineCurve<ScalarType>(neworder, m_dims, number);
  NMP_ASSERT(pCurve != NULL);

  // Copy the knots from this curve + pad any added knots with end values
  pCurve->m_knots.setSubVector(0, addl, kstart);
  pCurve->m_knots.setSubVector(addl, m_knots);

  // This also pads the inserted knot required to increase the curve order
  pCurve->m_knots.setSubVector(addl + kn, kend);

  // Copy the control points from this curve + pad any added points with zero
  pCurve->m_coefs.setSubMatrix(0, 0, m_dims, addl, (ScalarType)0);
  pCurve->m_coefs.setSubMatrix(0, addl, m_coefs);
  pCurve->m_coefs.setSubMatrix(0, number - addr, m_dims, addr, (ScalarType)0);

  // Q_i = Q_{i-1} + P_i * (t_{i+k} - t_i) / k
  for (uint32_t i = 0; i < number; ++i)
  {
    pCurve->m_coefs.multiplyColumn(i, (pCurve->m_knots[i+m_order] - pCurve->m_knots[i]) / m_order);
  }
  // Set the required integral bound value
  pCurve->m_coefs.setColumnVector(0, va);
  pCurve->m_coefs.cumulativeColumnSum();

  return pCurve;
}

//----------------------------------------------------------------------------------------------------------------------
// These functions are defined because the compiler doesn't recognize the
// difference between the base class implementation of a function with the
// same name that has different arguments. i.e. the compile fails after
// searching only the function names in this class, reporting that the function
// takes the wrong number of arguments.
//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* BSplineCurve<ScalarType>::integral() const
{
  Vector<ScalarType> va(m_dims, (ScalarType)0);
  return integral(va);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::evaluate(const Vector<ScalarType>& x, Matrix<ScalarType>& v) const
{
  CurveBase<ScalarType>::evaluate(x, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::derivative(uint32_t j, const Vector<ScalarType>& x, Matrix<ScalarType>& v) const
{
  CurveBase<ScalarType>::derivative(j, x, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BSplineCurve<ScalarType>::convertToBezierForm()
{
  // Get the vector of breaksites and their multiplicities
  Vector<ScalarType> brks;
  Vector<int32_t> mults;
  breaks(brks, mults);
  uint32_t n = brks.numElements();
  NMP_ASSERT(n >= 2);

  // Ensure that first knot has a multiplicity of (order)
  if ((uint32_t)mults[0] < m_order)
  {
    insertKnot(brks[0], m_order - mults[0]);
  }

  // Insert knots to ensure that all interior knots have a multiplicity of (order-1)
  uint32_t ext = n - 1;
  uint32_t degree = m_order - 1;
  for (uint32_t i = 1; i < ext; ++i)
  {
    if ((uint32_t)mults[i] < degree)
    {
      insertKnot(brks[i], degree - mults[i]);
    }
  }

  // Ensure that last knot has a multiplicity of (order)
  if ((uint32_t)mults[ext] < m_order)
  {
    insertKnot(brks[ext], m_order - mults[ext]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* BSplineCurve<ScalarType>::curveSegment(ScalarType x0, ScalarType x1) const
{
  if (x0 >= x1) return 0;

  // Get the knot properties at x0 and x1
  KnotProperties prop0, prop1;
  knotProperties(x0, prop0);
  knotProperties(x1, prop1);
  if (prop0.getRegion() != 0 || prop1.getRegion() != 0) return 0;

  // Insert knots with multiplicity order at x0 and x1
  BSplineCurve<ScalarType>* pCurve = new BSplineCurve<ScalarType>(*this);
  uint32_t r = prop0.getMultiplicity();
  if (r < m_order) pCurve->insertKnot(x0, m_order - r);
  r = prop1.getMultiplicity();
  if (r < m_order) pCurve->insertKnot(x1, m_order - r);

  // Get the new knot properties
  pCurve->knotProperties(x0, prop0);
  pCurve->knotProperties(x1, prop1);

  // Adjust the interval if x1 is the end knot
  uint32_t i0 = prop0.getInterval();
  uint32_t i1 = prop1.getInterval();
  if (pCurve->isEndKnot(x1)) i1 += m_order;

  // Extract the new knot vector segment
  uint32_t numKnots = i1 - i0 + m_order;
  uint32_t indx = i0 - (m_order - 1);
  Vector<ScalarType> knots_(numKnots);
  Vector<ScalarType>& knots = pCurve->knots();
  knots.getSubVector(indx, knots_);

  // Extract the new control point sub-matrix
  Matrix<ScalarType> coefs_(m_dims, numKnots - m_order);
  Matrix<ScalarType>& coefs = pCurve->coefs();
  coefs.getSubMatrix(0, indx, coefs_);

  // Set the new curve data
  knots.swap(knots_);
  coefs.swap(coefs_);

  return pCurve;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
