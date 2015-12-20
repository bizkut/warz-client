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
#ifndef NM_NORMAL_DISTRIBUTION_2D_H
#define NM_NORMAL_DISTRIBUTION_2D_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector.h"
#include "NMPlatform/NMMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// P( X | mu, E ) =
// 1 / ( 2.pi.sqrt( det(E) ) )
// exp( -1/2 . (X - mu)^{T} E^{-1} (X - mu) )
template <typename _T>
class NormalDistribution2D
{
public:
  // Construction / Destruction
  NormalDistribution2D();
  NormalDistribution2D(const NormalDistribution2D& rhs);
  NormalDistribution2D(const Vector<_T>& mu, const Matrix<_T>& E, bool pdf = true);
  NormalDistribution2D(_T mu_x, _T mu_y, _T var_x, _T covar_xy, _T var_y, bool pdf = true);
  ~NormalDistribution2D();

  // Assignment functions
  NormalDistribution2D& operator=(const NormalDistribution2D& rhs);
  NormalDistribution2D& operator*=(_T a);
  NormalDistribution2D& operator*=(const NormalDistribution2D& rhs);

  void clear();
  void get(Vector<_T>& mu, Matrix<_T>& E) const;
  void set(const Vector<_T>& mu, const Matrix<_T>& E);
  void get(_T& mu_x, _T& mu_y, _T& var_x, _T& covar_xy, _T& var_y) const;
  void set(_T mu_x, _T mu_y, _T var_x, _T covar_xy, _T var_y);

  // Data functions
  _T getA() const { return m_A; }
  void setA(_T a) { m_A = a; }
  const Vector<_T>& getMean() const { return m_mu; }
  const Matrix<_T>& getCovar() const { return m_E; }
  const Matrix<_T>& getInfoMatrix() const { return m_iE; }
  _T correlation() const; // -1 < p < 1

  // Likelihood: P( X | mu, E )
  _T P(_T x, _T y) const;
  _T P(const Vector<_T>& x) const { return P(x[0], x[1]); }

  // Ensure that area under Gaussian = 1 with
  // the current mean and covariance
  void makePDF();

protected:
  void computeInfoMatrix();

protected:
  _T m_A; // 1 / ( 2.pi.sqrt( det(E) ) )

  Vector<_T> m_mu; // Mean of distribution mu
  Matrix<_T> m_E; // Covariance matrix E

  Matrix<_T> m_iE; // Information matrix E^{-1}
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_NORMAL_DISTRIBUTION_2D_H
