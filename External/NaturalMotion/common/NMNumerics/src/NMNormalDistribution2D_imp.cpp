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
// Specializes template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// NormalDistribution2D
//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>::~NormalDistribution2D()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>::NormalDistribution2D() :
  m_mu(2), m_E(2, 2), m_iE(2, 2)
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>::NormalDistribution2D(const NormalDistribution2D<ScalarType>& rhs) :
  m_mu(2), m_E(2, 2), m_iE(2, 2)
{
  NormalDistribution2D<ScalarType>::operator=(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>::NormalDistribution2D(
  const Vector<ScalarType>& mu, const Matrix<ScalarType>& E, bool pdf) :
  m_mu(2), m_E(2, 2), m_iE(2, 2)
{
  set(mu, E);
  if (pdf) makePDF();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>::NormalDistribution2D(
  ScalarType mu_x, ScalarType mu_y, ScalarType var_x,
  ScalarType covar_xy, ScalarType var_y, bool pdf) :
  m_mu(2), m_E(2, 2), m_iE(2, 2)
{
  set(mu_x, mu_y, var_x, covar_xy, var_y);
  if (pdf) makePDF();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>& NormalDistribution2D<ScalarType>::operator=(const NormalDistribution2D<ScalarType>& rhs)
{
  if (this == &rhs) return *this;

  m_A = rhs.m_A;
  m_mu = rhs.m_mu;
  m_E = rhs.m_E;
  m_iE = rhs.m_iE;

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>& NormalDistribution2D<ScalarType>::operator*=(ScalarType a)
{
  m_A *= a;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NormalDistribution2D<ScalarType>& NormalDistribution2D<ScalarType>::operator*=(
  const NormalDistribution2D<ScalarType>& NMP_UNUSED(rhs))
{
  // This specialisation is not currently supported.
  NMP_ASSERT_FAIL();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::clear()
{
  m_A = static_cast<ScalarType>(0);
  m_mu.set(static_cast<ScalarType>(0));
  m_E.set(static_cast<ScalarType>(0));
  m_iE.set(static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::get(Vector<ScalarType>& mu, Matrix<ScalarType>& E) const
{
  mu = m_mu;
  E = m_E;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::set(const Vector<ScalarType>& mu, const Matrix<ScalarType>& E)
{
  m_mu = mu;
  m_E = E;
  computeInfoMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::get(
  ScalarType& mu_x, ScalarType& mu_y, ScalarType& var_x, ScalarType& covar_xy, ScalarType& var_y) const
{
  mu_x = m_mu[0];
  mu_y = m_mu[1];
  var_x = m_E.element(0, 0);
  covar_xy = m_E.element(0, 1);
  var_y = m_E.element(1, 1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::set(
  ScalarType mu_x, ScalarType mu_y, ScalarType var_x, ScalarType covar_xy, ScalarType var_y)
{
  m_mu[0] = mu_x;
  m_mu[1] = mu_y;
  m_E.element(0, 0) = var_x;
  m_E.element(0, 1) = m_E.element(1, 0) = covar_xy;
  m_E.element(1, 1) = var_y;
  computeInfoMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType NormalDistribution2D<ScalarType>::correlation() const
{
  // p(xy) = cov(xy) / (sqrt(var(x)) . sqrt(var(x)))
  // without destructive underflow
  return (m_E.element(0, 1) / sqrt(m_E.element(0, 0))) / sqrt(m_E.element(1, 1));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType NormalDistribution2D<ScalarType>::P(ScalarType x, ScalarType y) const
{
  ScalarType d, v[2], Ev[2];

  // (X - mu)
  v[0] = x - m_mu[0];
  v[1] = y - m_mu[1];

  // (X - mu)^{T} E^{-1} (X - mu)
  m_iE.preMultiply(Ev, v);
  d = v[0] * Ev[0] + v[1] * Ev[1];

  // P( X | mu, E ) =
  // 1 / ( 2.pi.sqrt( det(E) ) )
  // exp{ -1/2 . (X - mu)^{T} E^{-1} (X - mu) }
  return m_A * exp(static_cast<ScalarType>(-0.5) * d);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::makePDF()
{
  // Compute the scaling factor A:
  // 1 / (2.pi.sqrt(det(E)))
  const ScalarType pi2 = static_cast<ScalarType>(6.283185307179586476925286766559);
  ScalarType det = m_E.element(0, 0) * m_E.element(1, 1) - m_E.element(1, 0) * m_E.element(0, 1);
  m_A = 1 / (pi2 * sqrt(det));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NormalDistribution2D<ScalarType>::computeInfoMatrix()
{
  // Compute the pseudo-inverse for stability via symmetric eigen-decomposition
  // E = V D V^T  therefore E^+ = V D^+ V^T
  EigenSystemSym<ScalarType> eig(m_E);
  eig.pseudoInverse(m_iE);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
