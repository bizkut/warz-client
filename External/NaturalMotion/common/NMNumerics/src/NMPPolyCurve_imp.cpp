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
// PPolyCurve
//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>::~PPolyCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>::PPolyCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>::PPolyCurve(uint32_t order, uint32_t dims) :
  CurveBase(order, dims)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>::PPolyCurve(uint32_t order, uint32_t dims, uint32_t number) :
  CurveBase(order, dims, number, order, dims*(number - 1))
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>::PPolyCurve(
  uint32_t order, uint32_t dims, const Vector<ScalarType>& breaks, const Matrix<ScalarType>& coefs) :
  CurveBase(order, dims, breaks, coefs)
{
  NMP_ASSERT(m_coefs.numRows() == order);
  NMP_ASSERT(m_coefs.numColumns() == dims*(m_knots.numElements() - 1));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>::PPolyCurve(const PPolyCurve& curve) :
  CurveBase(curve.m_order, curve.m_dims, curve.m_knots, curve.m_coefs)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void PPolyCurve<ScalarType>::getCoef(uint32_t j, uint32_t i, Vector<ScalarType>& pj) const
{
  uint32_t n = pj.numElements();
  uint32_t stride = spansNum();
  for (uint32_t k = 0; k < n; ++k) pj[k] = m_coefs.element(j, stride * k + i);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void PPolyCurve<ScalarType>::setCoef(uint32_t j, uint32_t i, const Vector<ScalarType>& pj)
{
  uint32_t n = pj.numElements();
  uint32_t stride = spansNum();
  for (uint32_t k = 0; k < n; ++k) m_coefs.element(j, stride * k + i) = pj[k];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t PPolyCurve<ScalarType>::spansNum() const
{
  if (m_knots.isEmpty()) return 0;
  return m_knots.numElements() - 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void PPolyCurve<ScalarType>::insertKnot(ScalarType NMP_UNUSED(x), uint32_t NMP_UNUSED(mult))
{
  // This specialisation is not currently supported.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void PPolyCurve<ScalarType>::removeKnot(uint32_t NMP_UNUSED(intv), uint32_t NMP_UNUSED(mult))
{
  // This specialisation is not currently supported.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t PPolyCurve<ScalarType>::evaluate(ScalarType x, uint32_t& intv, Vector<ScalarType>& v, bool flag) const
{
  NMP_ASSERT(v.numElements() == m_dims);
  if (m_order == 0)
  {
    v.set((ScalarType)0);
    return 0;
  }

  // Find the interval
  uint32_t stride = spansNum();
  int32_t rgn = interval(x, intv, flag);

  // local space: x - t_i
  x -= m_knots[intv];

  // Evaluate the function of order k:
  // C_i(x) = p_{1,i} + p_{2,i}*(x-t_i) + ... + p_{k,i}*(x-t_i)^(k-1)
  for (uint32_t ii = 0; ii < m_dims; ++ii)
  {
    const ScalarType* P = &m_coefs.element(0, ii * stride + intv);
    v[ii] = P[m_order-1];
    for (int32_t k = m_order - 2; k >= 0; --k) v[ii] = x * v[ii] + P[k];
  }

  return rgn;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t PPolyCurve<ScalarType>::derivative(
  uint32_t j, ScalarType x, uint32_t& intv, Vector<ScalarType>& v, bool flag) const
{
  NMP_ASSERT(v.numElements() == m_dims);

  // Zero^th derivative: just evaluate the function
  if (j == 0) return evaluate(x, intv, v, flag);

  // Check if the derivative is greater than the curve degree
  if (j >= m_order)
  {
    v.set((ScalarType)0);
    return 0;
  }

  // Find the interval
  uint32_t stride = spansNum();
  int32_t rgn = interval(x, intv, flag);

  // local space: x - t_i
  x -= m_knots[intv];

  // Evaluate the derivative of the function with order k
  // D^(j)C_i(x) = \sum_{m=j}^{k-1} p_{m+1,i}*(x-t_i)^(m-j) * (m! / (m-j)!)
  for (uint32_t ii = 0; ii < m_dims; ++ii)
  {
    // Get polynomial coefficients P_{:,i}
    const ScalarType* P = &m_coefs.element(0, ii * stride + intv);

    // Accumulate factors containing the shifted powers (x-t_i)^(m-j)
    // i.e factors over the sum: m = j+1:k-1
    v[ii] = 0;
    uint32_t m = m_order - 1;
    uint32_t mmj = m - j;
    for (; m > j; --m, --mmj)
    {
      v[ii] = (v[ii] + P[m]) * x * ((ScalarType)m / (ScalarType)mmj);
    }

    // Accumulate the zeroth shifted power, where m=j
    v[ii] += P[j];

    // Finish computing the top factorial calculations m!
    for (; m >= 2; --m) v[ii] *= (ScalarType)m;

  }

  return rgn;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* PPolyCurve<ScalarType>::derivative(uint32_t j) const
{
  uint32_t number = m_knots.numElements();
  PPolyCurve<ScalarType>* pCurve;

  // Copy of curve
  if (j == 0)
  {
    pCurve = new PPolyCurve<ScalarType>(*this);
    NMP_ASSERT(pCurve != NULL);
    return pCurve;
  }

  // Require derivative of curve greater than its order
  if (j >= m_order)
  {
    pCurve = new PPolyCurve<ScalarType>(1, m_dims, number);
    NMP_ASSERT(pCurve != NULL);
    pCurve->m_knots = m_knots;
    pCurve->m_coefs.set((ScalarType)0);
    return pCurve;
  }

  // Create a new curve with the required order
  uint32_t neworder = m_order - j;
  pCurve = new PPolyCurve<ScalarType>(neworder, m_dims, number);
  NMP_ASSERT(pCurve != NULL);

  // Copy break sites and the high order coefficients
  pCurve->m_knots = m_knots;
  m_coefs.getSubMatrix(j, 0, pCurve->m_coefs);

  // Iteration over j factors
  for (uint32_t k = m_order - 1; k >= neworder; --k)
  {
    int32_t ii = neworder - 1;
    for (uint32_t i = k; i > 1 && ii >= 0; --i, --ii)
    {
      pCurve->m_coefs.multiplyRow(ii, (ScalarType)i);
    }
  }
  return pCurve;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* PPolyCurve<ScalarType>::integral(const Vector<ScalarType>& va) const
{
  NMP_ASSERT(va.numElements() == m_dims);

  uint32_t number = m_knots.numElements();
  PPolyCurve<ScalarType>* pCurve;

  // Create a new curve with the required order
  uint32_t neworder = m_order + 1;
  pCurve = new PPolyCurve<ScalarType>(neworder, m_dims, number);
  NMP_ASSERT(pCurve != NULL);

  // Get the number of polynomial pieces
  uint32_t stride = spansNum();

  // Copy break sites and the high order coefficients
  pCurve->m_knots = m_knots;
  pCurve->m_coefs.setSubMatrix(1, 0, m_coefs);

  // Adjust the coefficients
  for (uint32_t k = 2; k <= m_order; ++k)
  {
    pCurve->m_coefs.multiplyRow(k, 1 / (ScalarType)k);
  }

  // Check if we have a simple integral
  if (stride < 2)
  {
    pCurve->m_coefs.setRowVector(0, va);
    return pCurve;
  }

  // Evaluate each integrated polynomial piece at the right of each interval.
  // This gives the integral over each polynomial piece, which we then cumulatively
  // sum together to compute the full integral
  for (uint32_t i = 0; i < stride - 1; ++i)
  {
    // interval between break sites
    ScalarType x = m_knots[i+1] - m_knots[i];

    // Iterate over dims
    for (uint32_t ii = 0; ii < m_dims; ++ii)
    {
      // Evaluate at the right of the interval x = t_i+1
      // i.e. the local space value x = t_i+1 - t_i
      uint32_t indx = ii * stride + i;
      const ScalarType* P = &pCurve->m_coefs.element(0, indx);
      ScalarType* Pn = &pCurve->m_coefs.element(0, indx + 1);
      Pn[0] = x * P[m_order];
      for (uint32_t k = m_order - 1; k >= 1; --k) Pn[0] = x * (Pn[0] + P[k]);
    }
  }

  // Set the integral left boundary value
  for (uint32_t ii = 0; ii < m_dims; ++ii) pCurve->m_coefs.element(0, ii * stride) = va[ii];

  // Compute cumulative sum
  for (uint32_t i = 1; i < stride; ++i)
  {
    for (uint32_t ii = 0; ii < m_dims; ++ii)
    {
      uint32_t indx = ii * stride + i;
      pCurve->m_coefs.element(0, indx) += pCurve->m_coefs.element(0, indx - 1);
    }
  }

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
CurveBase<ScalarType>* PPolyCurve<ScalarType>::integral() const
{
  Vector<ScalarType> va(m_dims, (ScalarType)0);
  return integral(va);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void PPolyCurve<ScalarType>::evaluate(const Vector<ScalarType>& x, Matrix<ScalarType>& v) const
{
  CurveBase<ScalarType>::evaluate(x, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void PPolyCurve<ScalarType>::derivative(uint32_t j, const Vector<ScalarType>& x, Matrix<ScalarType>& v) const
{
  CurveBase<ScalarType>::derivative(j, x, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CurveBase<ScalarType>* PPolyCurve<ScalarType>::curveSegment(ScalarType x0, ScalarType x1) const
{
  NMP_ASSERT(x1 >= x0);

  // This specialisation is not currently supported.
  (void)x0;
  (void)x1;
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
