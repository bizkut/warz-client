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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_LINEAR_REGRESSION_H
#define NM_LINEAR_REGRESSION_H

#include "NMPlatform/NMPlatform.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Fits the measurement data y_i, that has some known degree of uncertainty w_i,
/// at each of the known sample locations x_i to a straight line: y = m * x + c
template <typename _T>
class LinearRegression
{
public:
  // Construction / Destruction
  LinearRegression();
  ~LinearRegression();

  // Assignment functions
  void clear();
  void add(_T x, _T y);
  void add(_T x, _T y, _T w);

  void solve();

  void solve(
    uint32_t numSamples,
    const _T* x,
    const _T* y,
    const _T* w);

  void solve(
    uint32_t numSamples,
    const _T* x,
    const _T* y);

  // Return the solution vector [m, c]
  NM_INLINE const _T* getP() const { return m_P; }

  // Evaluate the function: y = m * x + c
  NM_INLINE _T evaluate(_T x) const { return m_P[0] * x + m_P[1]; }

protected:
  // Summations
  _T m_S;
  _T m_Sx;
  _T m_Sy;
  _T m_Sxx;
  _T m_Sxy;

  // Result data [m, c]
  _T  m_P[2];
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_LINEAR_REGRESSION_H
