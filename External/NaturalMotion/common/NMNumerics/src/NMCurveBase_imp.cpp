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
// CurveBase
//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>::~CurveBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>::CurveBase() :
  m_order(0), m_dims(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>::CurveBase(uint32_t order, uint32_t dims) :
  m_order(order), m_dims(dims)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>::CurveBase(uint32_t order, uint32_t dims, uint32_t knots, uint32_t Cr, uint32_t Cc) :
  m_order(order), m_dims(dims),
  m_knots(knots), m_coefs(Cr, Cc)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>::CurveBase(
  uint32_t                  order,
  uint32_t                  dims,
  const Vector<ScalarType>& knots,
  const Matrix<ScalarType>& coefs) :
  m_order(order), m_dims(dims), m_knots(knots), m_coefs(coefs)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool CurveBase<ScalarType>::isStartKnot(ScalarType x) const
{
  return m_knots[0] == x;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool CurveBase<ScalarType>::isEndKnot(ScalarType x) const
{
  return m_knots.end(0) == x;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t CurveBase<ScalarType>::interval(ScalarType x, uint32_t& intv, bool flag) const
{
  uint32_t m = m_knots.numElements();
  NMP_ASSERT(m >= 2);

  // Outside the leftmost or rightmost interval
  uint32_t ext = m - 1;
  if (x > m_knots[ext])
  {
    intv = endSpan();
    return 1;
  }
  if (x < m_knots[0])
  {
    intv = 0;
    return -1;
  }

  // Special case for the rightmost interval
  if (x == m_knots[ext])
  {
    intv = endSpan();
    return 0;
  }

  // Check if we need to perform a local search for the required span
  uint32_t low = 0;
  uint32_t high = ext;
  if (flag && intv < ext)
  {
    // Find the index of the rightmost current breakpoint site
    for (; intv < ext - 1; ++intv)
    {
      if (m_knots[intv] != m_knots[intv+1]) break;
    }
    // Check if x is in the current span
    if (x >= m_knots[intv] && x < m_knots[intv+1]) return 0;

    // Check locally to the right of the current interval
    if (x >= m_knots[intv])
    {
      // Find the next unique break site
      for (++intv; intv < ext - 1; ++intv)
      {
        if (m_knots[intv+1] != m_knots[intv]) break;
      }
      // Check if x is within the interval
      if (x >= m_knots[intv] && x < m_knots[intv+1]) return 0;
      low = intv;
    }
    else
    {
      // Check locally to the left of the current interval
      for (int32_t i = (int32_t)intv - 1; i > 0; --i)
      {
        if (m_knots[i] != m_knots[intv])
        { // Check if x is within the interval
          if (x >= m_knots[i] && x < m_knots[i+1])
          {
            intv = i;
            return 0;
          }
        }
      }
      high = intv;
    }
  }

  // Perform a binary search to find the required interval
  intv = (low + high) >> 1;
  while (x < m_knots[intv] || x >= m_knots[intv+1])
  {
    if (x < m_knots[intv]) high = intv; else low = intv;
    intv = (low + high) >> 1;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool CurveBase<ScalarType>::interval(const Vector<ScalarType>& x, Vector<int32_t>& intvs) const
{
  NMP_ASSERT(x.numElements() == intvs.numElements());
  bool ret = true;
  uint32_t n = x.numElements();
  int32_t rgn;
  uint32_t intv = 0;

  for (uint32_t i = 0; i < n; ++i)
  {
    rgn = interval(x[i], intv, true);
    if (rgn != 0) ret = false;
    intvs[i] = intv;
  }
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t CurveBase<ScalarType>::endSpan() const
{
  uint32_t n = m_knots.numElements();
  NMP_ASSERT(n > 1);

  uint32_t i = n - 1;
  ScalarType last = m_knots[i];
  for (--i; i > 1; --i) if (m_knots[i] != last) break;
  return i;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::knotProperties(ScalarType x, KnotProperties& properties) const
{
  uint32_t intv = 0;
  uint32_t mult = 0;

  // Get the interval the x lies on
  int32_t rgn = interval(x, intv);
  if (rgn == 0)
  {
    // Compute the knot multiplicity at x
    if (isEndKnot(x))
    {
      mult = m_knots.numElements() - 1 - intv;
    }
    else
    {
      for (int32_t i = intv; i >= 0; --i)
      {
        if (m_knots[i] != x) break;
        ++mult;
      }
    }
  }

  // Set the knot properties
  properties.setRegion(rgn);
  properties.setInterval(intv);
  properties.setMultiplicity(mult);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::normaliseKnots(ScalarType x0, ScalarType x1)
{
  uint32_t n = m_knots.numElements();
  NMP_ASSERT(n >= 2);
  NMP_ASSERT(x1 > x0);

  ScalarType a = m_knots[0];
  ScalarType b = m_knots[n-1];
  ScalarType m = (x1 - x0) / (b - a);
  ScalarType c = x0 - a * m;

  for (uint32_t i = 0; i < n; ++i)
  {
    m_knots[i] = m * m_knots[i] + c;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::transformKnots(ScalarType scale, ScalarType offset)
{
  uint32_t n = m_knots.numElements();
  for (uint32_t i = 0; i < n; ++i)
  {
    m_knots[i] = scale * m_knots[i] + offset;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::knots(Vector<ScalarType>& knots, const Vector<ScalarType>& breaks, const Vector<int32_t>& mults)
{
  uint32_t n = breaks.numElements();
  NMP_ASSERT(mults.numElements() == n);

  // Determine size of the knot vector
  uint32_t m = 0;
  for (uint32_t i = 0; i < n; ++i)
  {
    NMP_ASSERT(mults[i] > 0);
    m += mults[i];
  }

  // Recover the knots
  Vector<ScalarType> knots_(m);
  m = 0;
  for (uint32_t i = 0; i < n; ++i)
  {
    ScalarType t = breaks[i];
    for (int32_t j = 0; j < mults[i]; ++j, ++m)
    {
      knots_[m] = t;
    }
  }

  // Set the knot data
  knots.swap(knots_);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> CurveBase<ScalarType>::breaks() const
{
  uint32_t n = 0;
  uint32_t m = m_knots.numElements();
  Vector<ScalarType> t(m);

  // Generate the set of unique break sites
  for (uint32_t i = 0; i < m; )
  {
    t[n] = m_knots[i]; n++;
    for (++i; i < m; ++i) if (m_knots[i] != t[n-1]) break;
  }

  // Create a new vector of the correct size and set data
  Vector<ScalarType> result(n, t.data());
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::breaks(const Vector<ScalarType>& knots, Vector<ScalarType>& breaks, Vector<int32_t>& mults)
{
  uint32_t m = knots.numElements();
  Vector<ScalarType> breaks_(m);
  Vector<int32_t> mults_(m);

  // Generate the set of unique break sites
  uint32_t n = 0;
  for (uint32_t i = 0; i < m; )
  {
    // Store the break value
    ScalarType t = knots[i];
    breaks_[n] = t;
    mults_[n] = 1;

    // Compute the multiplicities
    for (++i; i < m; ++i)
    {
      if (knots[i] != t) break;
      mults_[n]++;
    }
    n++;
  }

  // Return the vectors
  Vector<ScalarType> breaks2(n, breaks_.data());
  Vector<int32_t> mults2(n, mults_.data());
  breaks.swap(breaks2);
  mults.swap(mults2);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t CurveBase<ScalarType>::numBreaks() const
{
  uint32_t n = 0;
  uint32_t m = m_knots.numElements();

  // Generate the set of unique break sites
  for (uint32_t i = 0; i < m; )
  {
    ScalarType t = m_knots[i]; n++;
    for (++i; i < m; ++i) if (m_knots[i] != t) break;
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::removeKnot(ScalarType x, uint32_t mult)
{
  uint32_t intv;
  int32_t rgn = interval(x, intv, false);
  if (rgn != 0) return;

  removeKnot(intv, mult);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::computeArcLengthMap(std::map<ScalarType, ScalarType>& mapSamples, ScalarType threshold) const
{
  mapSamples.clear();

  Vector<ScalarType> breaksVec = breaks();

  // To determine the arc length mapping, we sample the curve and compute the euclidean length
  // between the samples. In order to make sure not to have a bad approximation, we shorten the
  // distance between samples when the curvature is high. See below for a description of how to
  // get an estimation of the curvature.
  mapSamples.insert(std::pair<ScalarType, ScalarType>(breaksVec.element(0), static_cast<ScalarType>(0.0)));
  for (unsigned int i = 0; i < breaksVec.numElements() - 1; i++)
  {
    ScalarType startParam = breaksVec.element(i);
    ScalarType endParam   = breaksVec.element(i + 1);

    ScalarType j    = static_cast<ScalarType>(0.0);
    ScalarType inc  = static_cast<ScalarType>(0.1); // Default is 10 steps

    ScalarType j_km1 = j;
    j                += inc;

    uint32_t                    intv;
    NMP::Vector<ScalarType> pos(m_dims);
    NMP::Vector<ScalarType> pos_km1(pos);
    NMP::Vector<ScalarType> temp(m_dims);
    bool goOnLooping = true;
    while (goOnLooping == true)
    {
      NMP::Vector<ScalarType> pos_halfways(pos);
      evaluate(startParam + j * (endParam - startParam), intv, pos, false);
      evaluate(startParam + (j_km1 + (j - j_km1) / static_cast<ScalarType>(2.0)) * (endParam - startParam), intv, pos_halfways, false);
      evaluate(startParam + j_km1 * (endParam - startParam), intv, pos_km1, false);

      // Let C(j) be the point on curve evaluated at parameter j. Then let l1 = dist(C(j_km1),C(j_halfways)),
      // l2 = dist(C(j_halfways), C(j)) and l3 = dist(C(j_km1), C(j)). The following piece of code does
      // if (l1 + l2) - l3 > 'given threshold' then the curve linearization is judged too coarse and a new
      // subdivision is done. Otherwise the pair 'parameter/arc length' is stored in the map.
      temp.setFromDiff(pos_halfways, pos_km1);
      ScalarType l1 = NMP::enorm<ScalarType, ScalarType>(temp); // pos_halfways - pos_km1
      temp.setFromDiff(pos, pos_halfways);
      ScalarType l2 = NMP::enorm<ScalarType, ScalarType>(temp); // pos - pos_halfways
      temp.setFromDiff(pos, pos_km1);
      ScalarType l3 = NMP::enorm<ScalarType, ScalarType>(temp); // pos - pos_km1
      if ((l1 + l2) - l3 > threshold)
        j = j_km1 + (j - j_km1) / static_cast<ScalarType>(2.0);
      else
      {
        ScalarType arclength_soFar = 0;
        if (mapSamples.size() != 0)
          arclength_soFar = mapSamples.rbegin()->second;

        mapSamples.insert(std::pair<ScalarType, ScalarType>(startParam + j * (endParam - startParam), arclength_soFar + l1 + l2));

        if (j == static_cast<ScalarType>(1.0))
          goOnLooping = false;

        j_km1 = j;
        j + inc >= static_cast<ScalarType>(1.0) ? j = static_cast<ScalarType>(1.0) : j += inc;
      }
    }

    evaluate(startParam + j_km1                        * (endParam - startParam), intv, pos_km1, false);
    evaluate(startParam + static_cast<ScalarType>(1.0) * (endParam - startParam), intv, pos,     false);
    temp.setFromDiff(pos_km1, pos);
    ScalarType l = NMP::enorm<ScalarType, ScalarType>(temp); // pos_km1 - pos;
    mapSamples.insert(std::pair<ScalarType, ScalarType>(startParam + static_cast<ScalarType>(1.0) * (endParam - startParam), mapSamples.rbegin()->second + l));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CurveBase<ScalarType>::arcLengthMap(ScalarType parameter, std::map<ScalarType, ScalarType>& mapSamples)
{
  NMP_ASSERT_MSG(mapSamples.size() > 2, "The mapping should contain at least 2 samples!");

  if (parameter < mapSamples.begin()->first && parameter > mapSamples.rbegin()->first)
    return -1;

  if (parameter == mapSamples.begin()->first)
    return mapSamples.begin()->second;

  // Find the segment of the map in which 'parameter' lies
  bool goOnSearching = true;
  std::map<ScalarType, ScalarType>::iterator it = mapSamples.begin();
  while (it != mapSamples.end() && goOnSearching == true)
  {
    if (it->first < parameter)
      it++;
    else
      goOnSearching = false;
  }

  // Linear interpolation between two contiguous samples of the map
  std::map<ScalarType, ScalarType>::iterator higherSampleIt = it;
  std::map<ScalarType, ScalarType>::iterator lowerSampleIt  = --it;

  return lowerSampleIt->second + (parameter - lowerSampleIt->first) / (higherSampleIt->first - lowerSampleIt->first) * (higherSampleIt->second - lowerSampleIt->second);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CurveBase<ScalarType>::inverseArcLengthMap(ScalarType length, std::map<ScalarType, ScalarType>& mapSamples)
{
  NMP_ASSERT_MSG(mapSamples.size() > 2, "The mapping should contain at least 2 samples!");

  if (length < static_cast<ScalarType>(0.0) && length > mapSamples.rbegin()->second)
    return -1;

  if (length == 0)
    return mapSamples.begin()->first;

  // Find the segment of the map in which 'length' lies
  bool goOnSearching = true;
  std::map<ScalarType, ScalarType>::iterator it = mapSamples.begin();
  while (it != mapSamples.end() && goOnSearching == true)
  {
    if (it->second < length)
      it++;
    else
      goOnSearching = false;
  }

  // Linear interpolation between two contiguous samples of the map
  std::map<ScalarType, ScalarType>::iterator higherSampleIt = it;
  std::map<ScalarType, ScalarType>::iterator lowerSampleIt  = --it;

  return lowerSampleIt->first + (length - lowerSampleIt->second) / (higherSampleIt->second - lowerSampleIt->second) * (higherSampleIt->first - lowerSampleIt->first);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::evaluate(const Vector<ScalarType>& x, Matrix<ScalarType>& v) const
{
  uint32_t n = x.numElements();
  NMP_ASSERT(v.numRows() == n && v.numColumns() == m_dims);

  Vector<ScalarType> p(m_dims);
  uint32_t intv = 0;
  for (uint32_t i = 0; i < n; ++i)
  {
    evaluate(x[i], intv, p, true);
    v.setRowVector(i, p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::derivative(uint32_t j, const Vector<ScalarType>& x, Matrix<ScalarType>& v) const
{
  uint32_t n = x.numElements();
  NMP_ASSERT(v.numRows() == n && v.numColumns() == m_dims);

  Vector<ScalarType> p(m_dims);
  uint32_t intv = 0;
  for (uint32_t i = 0; i < n; ++i)
  {
    derivative(j, x[i], intv, p, true);
    v.setRowVector(i, p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t CurveBase<ScalarType>::order() const
{
  return m_order;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t CurveBase<ScalarType>::degree() const
{
  if (m_order == 0) return 0;
  return m_order - 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t CurveBase<ScalarType>::dims() const
{
  return m_dims;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& CurveBase<ScalarType>::coefs() const
{
  return m_coefs;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& CurveBase<ScalarType>::coefs()
{
  return m_coefs;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& CurveBase<ScalarType>::knots() const
{
  return m_knots;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& CurveBase<ScalarType>::knots()
{
  return m_knots;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CurveBase<ScalarType>::knots(uint32_t i) const
{
  return m_knots[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CurveBase<ScalarType>::swap(CurveBase<ScalarType>& curve)
{
  // Order and dims
  uint32_t k = m_order;
  uint32_t dims = m_dims;
  m_order = curve.m_order;
  m_dims = curve.m_dims;
  curve.m_order = k;
  curve.m_dims = dims;

  // Knot vector
  m_knots.swap(curve.m_knots);

  // Coefficient matrix
  m_coefs.swap(curve.m_coefs);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CurveBase<ScalarType>::projectPoint(const Vector<ScalarType>& P, Vector<ScalarType>& Cx, uint32_t n, ScalarType tolerance) const
{
  NMP_ASSERT(m_order > 0);
  NMP_ASSERT(P.numElements() == m_dims);
  NMP_ASSERT(Cx.numElements() == m_dims);
  NMP_ASSERT(m_knots.numElements() > 1);

  //
  // Closed form solution for curves with order=1, i.e. a set of
  // disjointed points in space
  //
  if (m_order == 1)
  {
    // Workspace memory
    Vector<ScalarType> v(m_dims);
    Vector<ScalarType> dx(m_dims);

    ScalarType d = std::numeric_limits<ScalarType>::infinity();
    ScalarType u = 0;

    for (uint32_t i = 0; i < m_knots.numElements(); )
    {
      // Get the current knot value
      ScalarType x = m_knots[i];

      // Evaluate the function C(x) at the knot value
      uint32_t intv = i;
      evaluate(x, intv, v, true);

      // Distance to space point P
      dx = P; dx -= v;
      ScalarType d_ = dx[0] * dx[0];
      for (uint32_t k = 1; k < m_dims; ++k) d_ += dx[k] * dx[k];

      // Update minimum distance
      if (d_ < d)
      {
        d = d_;
        u = x;
        Cx = v;
      }

      // Find the next unique knot value
      for (++i; i < m_knots.numElements(); ++i) if (m_knots[i] != x) break;
    }

    return u;
  }

  //
  // Closed form solution for curves with order=2, i.e. a polyline
  //
  if (m_order == 2)
  {
    // Workspace memory
    Vector<ScalarType> v(m_dims);
    Vector<ScalarType> v_(m_dims);
    Vector<ScalarType> dx(m_dims);
    Vector<ScalarType> dv(m_dims);

    ScalarType d = std::numeric_limits<ScalarType>::infinity();
    ScalarType u = 0;

    // Evaluate first data site
    uint32_t i = 0;
    uint32_t intv = i;
    evaluate(m_knots[i], intv, v, true);

    // Iterate over the knot vector
    while (true)
    {
      // Get the current knot value
      ScalarType x = m_knots[i];

      // Compute distance to curve at knot value
      dx = P; dx -= v;
      ScalarType d_ = dx[0] * dx[0];
      for (uint32_t k = 1; k < m_dims; ++k) d_ += dx[k] * dx[k];

      // Update minimum distance
      if (d_ < d)
      {
        d = d_;
        u = x;
        Cx = v;
      }

      // Find the next unique knot value
      i++; // next element
      while (true)
      {
        if (i == m_knots.numElements()) return u; // end of knot vector
        if (m_knots[i] != x) break; // found next unique knot
        i++;
      }

      // Evaluate the function C(x) at the new knot value
      intv = i;
      evaluate(m_knots[i], intv, v_, true);

      // Compute distance to line segment of curve
      dv = v_; dv -= v;
      ScalarType lambda = dv.dot(dx) / dv.dot(dv);
      if (lambda > 0 && lambda < 1)
      {
        // Compute projected point on line segment
        dv *= lambda;
        v += dv;

        // Compute distance to projected point
        dx = P; dx -= v;
        d_ = dx[0] * dx[0];
        for (uint32_t k = 1; k < m_dims; ++k) d_ += dx[k] * dx[k];

        // Update minimum distance
        if (d_ < d)
        {
          d = d_;
          u = (1 - lambda) * x + lambda * m_knots[i];
          Cx = v;
        }
      }

      // Update next function evaluation C(x)
      v = v_;
    }

    return u;
  }

  //
  // General solution for curves with order > 2, determined by finding
  // a suitable initial parameter value and using Newton iteration to
  // find a better estimate of the projected point on the curve
  //
  if (n < 1) n = 1;

  if (tolerance <= 0)
    return projectPoint_newton(P, Cx, n);
  else
    return projectPoint_bisection(P, Cx, n, tolerance);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool CurveBase<ScalarType>::projectPoint(const Vector<ScalarType>& P, Vector<ScalarType>& Cx, ScalarType& x) const
{
  NMP_ASSERT(P.numElements() == m_dims);
  NMP_ASSERT(Cx.numElements() == m_dims);
  NMP_ASSERT(x * 0 == 0); // Ensure is finite

  // Knot vector range
  const ScalarType ka = m_knots[0];
  const ScalarType kb = m_knots.end(0);

  // Set up tolerances
  const ScalarType tolC = NMCURVEBASE_CTOL;
  const ScalarType tolX = NMCURVEBASE_XTOL * (kb - ka);
  const ScalarType tolD = NMCURVEBASE_DTOL;
  const uint32_t maxIts = 5 * m_order;

  // Workspace memory
  Vector<ScalarType> dCx(m_dims);
  Vector<ScalarType> d2Cx(m_dims);
  Vector<ScalarType> CxP(m_dims);

  // Compute the derivative curves C'(x) and C''(x)
  const CurveBase<ScalarType>* pDC = derivative(1);
  NMP_ASSERT(pDC != NULL);
  const CurveBase<ScalarType>* pD2C = derivative(2);
  NMP_ASSERT(pD2C != NULL);

  // Make references to derivative curves
  const CurveBase<ScalarType>& DC = *pDC;
  const CurveBase<ScalarType>& D2C = *pD2C;

  // Set the old parameters
  ScalarType d_ = std::numeric_limits<ScalarType>::infinity();
  ScalarType x_ = x;

  // Initialise defaults
  uint32_t intv = 0, Dintv = 0, D2intv = 0;
  bool status = false;

  // Iterate until convergence
  for (uint32_t i = 0; i < maxIts; ++i)
  {
    // Evaluate the function C(x)
    int32_t rgn = evaluate(x, intv, Cx, true);
    if (rgn != 0)
    {
      x = ka; break;
    }

    // Evaluate the function C'(x)
    rgn = DC.evaluate(x, Dintv, dCx, true);
    NMP_ASSERT(rgn == 0);

    // Compute the functional: f(x) = C'(x) . (C(x) - P)
    CxP = Cx;
    CxP -= P;
    ScalarType f = dCx.dot(CxP);

    // Compute the distance and cosine angle measures
    ScalarType d = NMP::enorm<ScalarType, ScalarType>(CxP);
    ScalarType b = NMP::enorm<ScalarType, ScalarType>(dCx);
    ScalarType dd = d_ - d; // distance measure
    ScalarType c = fabs(f) / (b * d); // Cosine angle measure

    // Check for convergence of both measures
    if (dd < tolD && c < tolC)
    {
      // Check for divergence
      if (dd < 0) x = x_;
      status = true;
      break;
    }

    // Evaluate the function C''(x)
    rgn = D2C.evaluate(x, D2intv, d2Cx, true);
    NMP_ASSERT(rgn == 0);

    // Compute the functional: f'(x) = C''(x) . (C(x) - P) + (C'(x) . C'(x))
    ScalarType df = d2Cx.dot(CxP) + dCx.dot(dCx);
    NMP_ASSERT(df != 0);

    // Compute Newton iteration step
    x = x_ - f / df;

    // Clamp the computed parameter to the knot vector range
    if (x < ka)
      x = ka;
    else if (x > kb)
      x = kb;

    // Check for convergence of the parameter value
    dCx *= (x - x_);
    ScalarType dx = NMP::enorm<ScalarType, ScalarType>(dCx);
    if (dx < tolX)
    {
      status = true;
      break;
    }

    // Update the old parameter values
    x_ = x;
    d_ = d;
  }

  // Compute the point on the curve C(x)
  evaluate(x, intv, Cx, true);

  // Delete derivative curves
  delete pD2C;
  delete pDC;

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CurveBase<ScalarType>::projectPoint_newton(const Vector<ScalarType>& P, Vector<ScalarType>& Cx, uint32_t n) const
{
  // Workspace memory
  Vector<ScalarType> v(m_dims);
  Vector<ScalarType> v_(m_dims);
  Vector<ScalarType> dx(m_dims);
  Vector<ScalarType> dv(m_dims);

  ScalarType d = std::numeric_limits<ScalarType>::infinity();
  ScalarType u = 0;

  // Compute the set of point samples at n equally spaced parameter values
  // on each span of the curve
  uint32_t i = 0;
  while (true)
  {
    // Get the current knot value
    ScalarType x = m_knots[i];

    // Evaluate the function C(x) at the knot value
    uint32_t intv = i;
    evaluate(x, intv, v, true);

    // Compute distance to curve at knot value
    dx = P; dx -= v;
    ScalarType d_ = dx[0] * dx[0];
    for (uint32_t k = 1; k < m_dims; ++k) d_ += dx[k] * dx[k];

    // Update minimum distance
    if (d_ < d)
    {
      d = d_;
      u = x;
    }

    // Find the next unique knot value
    i++; // next element
    while (true)
    {
      if (i == m_knots.numElements()) // end of knot vector
      {
        projectPoint(P, Cx, u);
        return u;
      }
      if (m_knots[i] != x) break; // found next unique knot
      i++;
    }

    // Compute n equally spaced parameter values on the current curve span
    for (uint32_t j = 1; j < n; ++j)
    {
      ScalarType alpha = j / (ScalarType)n;
      ScalarType t = (1 - alpha) * x + alpha * m_knots[i];
      evaluate(t, intv, v, true);

      // Compute distance to curve at knot value
      dx = P; dx -= v;
      d_ = dx[0] * dx[0];
      for (uint32_t k = 1; k < m_dims; ++k) d_ += dx[k] * dx[k];

      // Update minimum distance
      if (d_ < d)
      {
        d = d_;
        u = t;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CurveBase<ScalarType>::projectPoint_bisection(const Vector<ScalarType>& P, Vector<ScalarType>& Cx, uint32_t n, ScalarType tolerance) const
{
  // Workspace memory
  Vector<ScalarType> v(m_dims);
  Vector<ScalarType> dx(m_dims);
  Vector<ScalarType> breakPts = breaks();

  ScalarType d = std::numeric_limits<ScalarType>::infinity();
  ScalarType x = 0;

  ScalarType lowX = 0;
  ScalarType upX = 0;

  uint32_t intv;

  // Sample each span of the curve n times and find the sample closest to P
  for (uint32_t i = 0; i < breakPts.numElements() - 1; i++)
  {
    // Compute n equally spaced parameter values on the current curve span
    for (uint32_t j = 0; j < n; j++)
    {
      ScalarType t = breakPts[i] + (ScalarType)j / (ScalarType)n * (breakPts[i + 1] - breakPts[i]);
      evaluate(t, intv, v, true);

      // Compute distance to curve at knot value
      dx = P - v;
      ScalarType d_ = 0;
      for (uint32_t k = 0; k < m_dims; k++)
        d_ += dx[k] * dx[k];

      // Update minimum distance
      if (d_ < d)
      {
        d = d_;
        x = t;
        Cx = v;

        if (j == 0)
        {
          if (i == 0)
            lowX = x;
          else
            lowX = x - (breakPts[i] - breakPts[i - 1]) / (ScalarType)n;

          // No need to check if i reached its upper boundary as max(i) = breakPts.numElements() - 2.
          upX = x + (breakPts[i + 1] - breakPts[i]) / (ScalarType)n;
        }
        else
        {
          lowX = x - (breakPts[i + 1] - breakPts[i]) / (ScalarType)n;
          upX = x + (breakPts[i + 1] - breakPts[i]) / (ScalarType)n;
        }
      }
    }

    ScalarType t = breakPts.end(0);
    evaluate(t, intv, v, true);

    // Compute distance to curve at knot value
    dx = P - v;
    ScalarType d_ = 0;
    for (uint32_t k = 0; k < m_dims; k++)
      d_ += dx[k] * dx[k];

    // Update minimum distance
    if (d_ < d)
    {
      d = d_;
      x = t;
      Cx = v;

      if (i != 0)
        lowX = x - (breakPts[i] - breakPts[i - 1]) / (ScalarType)n;
      else
        lowX = x;
      upX = x;
    }
  }

  // Refine the brute force result from above via dichotomy
  ScalarType derivStep = (ScalarType)100.0;
  ScalarType updateStep = (ScalarType)2.0;
  while (upX - lowX > tolerance)
  {
    // Test whether we need to increase or decrease x
    bool updateUp = false;
    bool updateDown = false;
    if (x != upX)
    {
      ScalarType t = x + (upX - x) / derivStep;
      evaluate(t, intv, v, true);

      // Compute distance to curve at knot value
      dx = P - v;
      ScalarType d_ = 0;
      for (uint32_t k = 0; k < m_dims; k++)
        d_ += dx[k] * dx[k];

      if (d_ < d)
        updateUp = true;
      else if (d_ > d)
        updateDown = true;
    }
    else if (x != lowX)
    {
      ScalarType t = x - (x - lowX) / derivStep;
      evaluate(t, intv, v, true);

      // Compute distance to curve at knot value
      dx = P - v;
      ScalarType d_ = 0;
      for (uint32_t k = 0; k < m_dims; k++)
        d_ += dx[k] * dx[k];

      if (d_ < d)
        updateDown = true;
      else if (d_ > d)
        updateUp = true;
    }

    // Increase or decrease x
    if (updateUp)
    {
      // Get out of the loop if:
      //    - we are at the end of the curve and need to increase x to decrease d (which of course we cannot do)
      //    - or if the distance between upX and x is lower than tolerance (which helps prevent situations where
      //      we need to increase x towards upX to decrease d but upX and x are nearly equal - which could otherwise
      //      potentially lead to infinite loops)
      if (fabs(upX - x) < tolerance)
        lowX = upX;
      else
      {
        ScalarType t = x + (upX - x) / updateStep;
        evaluate(t, intv, v, true);

        // Compute distance to curve at knot value
        dx = P - v;
        ScalarType d_ = 0;
        for (uint32_t k = 0; k < m_dims; k++)
          d_ += dx[k] * dx[k];

        if (d_ < d)
        {
          lowX = x;

          d = d_;
          x = t;
          Cx = v;
        }
        else
          upX = t;
      }
    }

    if (updateDown)
    {
      // Get out of the loop if:
      //    - we are at the beginning of the curve and need to decrease x to decrease d (which of course we cannot do)
      //    - or if the distance between lowX and x is lower than tolerance (which helps prevent situations where
      //      we need to decrease x towards lowX to decrease d but lowX and x are nearly equal - which could otherwise
      //      potentially lead to infinite loops)
      if (fabs(lowX - x) < tolerance)
        upX = lowX;
      else
      {
        ScalarType t = x - (x - lowX) / updateStep;
        evaluate(t, intv, v, true);

        // Compute distance to curve at knot value
        dx = P - v;
        ScalarType d_ = 0;
        for (uint32_t k = 0; k < m_dims; k++)
          d_ += dx[k] * dx[k];

        if (d_ < d)
        {
          upX = x;

          d = d_;
          x = t;
          Cx = v;
        }
        else
          lowX = t;
      }
    }
  }

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
std::ostream& operator<<(std::ostream& os, const CurveBase<ScalarType>& curve)
{
  os << "order : " << curve.order() << std::endl;
  os << "dims : " << curve.dims() << std::endl;
  os << std::endl;
  os << "knots : " << curve.knots() << std::endl;
  os << "coefs : " << curve.coefs() << std::endl;
  return os;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
