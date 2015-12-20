// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
// Specializes template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// LinearRegression
//----------------------------------------------------------------------------------------------------------------------
template <>
LinearRegression<ScalarType>::~LinearRegression()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
LinearRegression<ScalarType>::LinearRegression()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void LinearRegression<ScalarType>::clear()
{
  m_S = m_Sx = m_Sy = m_Sxx = m_Sxy = static_cast<ScalarType>(0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void LinearRegression<ScalarType>::add(ScalarType x, ScalarType y)
{
  m_S += static_cast<ScalarType>(1);
  m_Sx += x;
  m_Sy += y;
  m_Sxx += x * x;
  m_Sxy += x * y;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void LinearRegression<ScalarType>::add(ScalarType x, ScalarType y, ScalarType w)
{
  NMP_ASSERT(w > static_cast<ScalarType>(0));
  ScalarType recipW = static_cast<ScalarType>(1) / (w * w);
  m_S += recipW;
  m_Sx += x * recipW;
  m_Sy += y * recipW;
  m_Sxx += x * x * recipW;
  m_Sxy += x * y * recipW;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void LinearRegression<ScalarType>::solve()
{
  ScalarType D = m_Sx * m_Sx - m_S * m_Sxx;
  if (D != static_cast<ScalarType>(0))
  {
    // Fit to a straight line
    m_P[0] = (m_Sx * m_Sy - m_S * m_Sxy) / D;
    m_P[1] = (m_Sx * m_Sxy - m_Sy * m_Sxx) / D;
  }
  else
  {
    // Fit to a constant value
    m_P[0] = static_cast<ScalarType>(0);
    m_P[1] = m_Sy / m_S;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void LinearRegression<ScalarType>::solve(
  uint32_t numSamples,
  const ScalarType* x,
  const ScalarType* y,
  const ScalarType* w)
{
  NMP_ASSERT(numSamples > 0);
  NMP_ASSERT(x);
  NMP_ASSERT(y);
  NMP_ASSERT(w);

  clear();
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    add(x[i], y[i], w[i]);
  }
  solve();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void LinearRegression<ScalarType>::solve(
  uint32_t numSamples,
  const ScalarType* x,
  const ScalarType* y)
{
  NMP_ASSERT(numSamples > 0);
  NMP_ASSERT(x);
  NMP_ASSERT(y);

  clear();
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    add(x[i], y[i]);
  }
  solve();
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
