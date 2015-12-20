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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_MOMENTS_2D_H
#define NM_MOMENTS_2D_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMNormalDistribution2D.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Moments in two dimensions
template <typename _T>
class Moments2D
{
public:
  // Construction / Destruction
  Moments2D();
  Moments2D(const Moments2D& rhs);
  ~Moments2D();

  // Assignment functions
  void clear();
  void add(_T x, _T y);
  void add(_T x, _T y, uint32_t count);
  void remove(_T x, _T y);
  void remove(_T x, _T y, uint32_t count);

  Moments2D& operator=(const Moments2D& rhs);
  Moments2D& operator+=(const Moments2D& rhs);
  Moments2D& operator-=(const Moments2D& rhs);

  // Moment data functions
  uint32_t size() const { return m_S00; }
  uint32_t S00() const { return m_S00; }
  _T S10() const { return m_S10; }
  _T S01() const { return m_S01; }
  _T S11() const { return m_S11; }
  _T S20() const { return m_S20; }
  _T S02() const { return m_S02; }

  // Distribution functions
  void mean(_T& x, _T& y) const;
  void mean(Vector<_T>& mu) const; // 2-vector
  Vector<_T> mean() const; // 2-vector
  void covar(_T& mu_x, _T& mu_y, _T& var_x, _T& covar_xy, _T& var_y) const;
  void covar(Matrix<_T>& E) const; // 2 by 2 matrix
  Matrix<_T> covar() const; // 2 by 2 matrix
  _T correlation() const; // -1 < p < 1

  // Principal components
  bool principalComponents(
    _T* xBar,   ///< OUT: Means (2-vector)
    _T* xVar,   ///< OUT: Variances (2-vector)
    _T* xV0,    ///< OUT: The most variant direction (2-vector)
    _T* xV1     ///< OUT: The least variant direction (2-vector)
    ) const;

  // Make a compact representation of the data distribution
  // with a bivariate normal distribution. Scaling factor is
  // set to one (value at the mean is one).
  void distribution(NormalDistribution2D<_T>& N) const;

  // Make a compact representation of the probability
  // density function with a bivariate normal
  // distribution. Area under Gaussian = 1
  void pdf(NormalDistribution2D<_T>& N) const;

protected:
  // Moment values: S(m,n) = \sum_{i}{ x_{i}^{m} y_{i}^{n} }
  uint32_t  m_S00;        // N
  _T        m_S10, m_S01; // x_{i}, y_{i}
  _T        m_S11;        // x_{i}*y_{i}
  _T        m_S20, m_S02; // x_{i}^{2}, y_{i}^{2}
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_MOMENTS_2D_H
