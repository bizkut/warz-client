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
// Moments2D
//----------------------------------------------------------------------------------------------------------------------
template <>
Moments2D<ScalarType>::~Moments2D()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Moments2D<ScalarType>::Moments2D()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Moments2D<ScalarType>::Moments2D(const Moments2D<ScalarType>& rhs)
{
  Moments2D<ScalarType>::operator=(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::clear()
{
  m_S00 = 0;
  m_S10 = m_S01 = m_S11 = m_S20 = m_S02 = static_cast<ScalarType>(0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::add(ScalarType x, ScalarType y)
{
  m_S00++;
  m_S10 += x;
  m_S01 += y;
  m_S11 += x * y;
  m_S20 += x * x;
  m_S02 += y * y;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::add(ScalarType x, ScalarType y, uint32_t count)
{
  ScalarType xc = x * count;
  ScalarType yc = y * count;

  // Add the point count times
  m_S00 += count;
  m_S10 += xc;
  m_S01 += yc;
  m_S11 += (x * yc);
  m_S20 += (x * xc);
  m_S02 += (y * yc);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::remove(ScalarType x, ScalarType y)
{
  m_S00--;
  m_S10 -= x;
  m_S01 -= y;
  m_S11 -= x * y;
  m_S20 -= x * x;
  m_S02 -= y * y;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::remove(ScalarType x, ScalarType y, uint32_t count)
{
  ScalarType xc = x * count;
  ScalarType yc = y * count;

  // Remove the point count times
  m_S00 -= count;
  m_S10 -= xc;
  m_S01 -= yc;
  m_S11 -= (x * yc);
  m_S20 -= (x * xc);
  m_S02 -= (y * yc);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Moments2D<ScalarType>& Moments2D<ScalarType>::operator=(const Moments2D& rhs)
{
  if (this == &rhs) return *this;
  m_S00 = rhs.m_S00;
  m_S10 = rhs.m_S10;
  m_S01 = rhs.m_S01;
  m_S11 = rhs.m_S11;
  m_S20 = rhs.m_S20;
  m_S02 = rhs.m_S02;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Moments2D<ScalarType>& Moments2D<ScalarType>::operator+=(const Moments2D& rhs)
{
  m_S00 += rhs.m_S00;
  m_S10 += rhs.m_S10;
  m_S01 += rhs.m_S01;
  m_S11 += rhs.m_S11;
  m_S20 += rhs.m_S20;
  m_S02 += rhs.m_S02;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Moments2D<ScalarType>& Moments2D<ScalarType>::operator-=(const Moments2D& rhs)
{
  m_S00 -= rhs.m_S00;
  m_S10 -= rhs.m_S10;
  m_S01 -= rhs.m_S01;
  m_S11 -= rhs.m_S11;
  m_S20 -= rhs.m_S20;
  m_S02 -= rhs.m_S02;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::mean(ScalarType& x, ScalarType& y) const
{
  x = m_S10 / m_S00;
  y = m_S01 / m_S00;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::mean(Vector<ScalarType>& mu) const
{
  NMP_ASSERT(mu.numElements() == 2);
  mu[0] = m_S10 / m_S00;
  mu[1] = m_S01 / m_S00;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Moments2D<ScalarType>::mean() const
{
  Vector<ScalarType> mu(2);
  mu[0] = m_S10 / m_S00;
  mu[1] = m_S01 / m_S00;
  return mu;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::covar(
  ScalarType& mu_x, ScalarType& mu_y, ScalarType& var_x, ScalarType& covar_xy, ScalarType& var_y) const
{
  // Mean
  mu_x = m_S10 / m_S00;
  mu_y = m_S01 / m_S00;

  // Covariance matrix
  var_x = (m_S20 / m_S00) - (mu_x * mu_x);
  covar_xy = (m_S11 / m_S00) - (mu_x * mu_y);
  var_y = (m_S02 / m_S00) - (mu_y * mu_y);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::covar(Matrix<ScalarType>& E) const
{
  NMP_ASSERT(E.isSize(2, 2));

  ScalarType mu_x, mu_y, var_x, covar_xy, var_y;
  covar(mu_x, mu_y, var_x, covar_xy, var_y);
  E.element(0, 0) = var_x;
  E.element(0, 1) = E.element(1, 0) = covar_xy;
  E.element(1, 1) = var_y;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Moments2D<ScalarType>::covar() const
{
  Matrix<ScalarType> E(2, 2);
  covar(E);
  return E;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Moments2D<ScalarType>::correlation() const
{
  // Compute the covariance matrix
  ScalarType mu_x, mu_y, var_x, covar_xy, var_y;
  covar(mu_x, mu_y, var_x, covar_xy, var_y);

  // Correlation (without destructive underflow)
  return (covar_xy / sqrt(var_x)) / sqrt(var_y);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::distribution(NormalDistribution2D<ScalarType>& N) const
{
  // Compute the covariance matrix
  ScalarType mu_x, mu_y, var_x, covar_xy, var_y;
  covar(mu_x, mu_y, var_x, covar_xy, var_y);

  // Set the normal distribution
  N.set(mu_x, mu_y, var_x, covar_xy, var_y);
  N.setA(static_cast<ScalarType>(1)); // Set scaling factor to unity
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Moments2D<ScalarType>::pdf(NormalDistribution2D<ScalarType>& N) const
{
  // Compute the covariance matrix
  ScalarType mu_x, mu_y, var_x, covar_xy, var_y;
  covar(mu_x, mu_y, var_x, covar_xy, var_y);

  // Set the normal distribution
  N.set(mu_x, mu_y, var_x, covar_xy, var_y);
  N.makePDF(); // area under distribution is unity
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Moments2D<ScalarType>::principalComponents(
  ScalarType* xBar,   // OUT: Means (2-vector)
  ScalarType* xVar,   // OUT: Variances (2-vector)
  ScalarType* xV0,    // OUT: The most variant direction (2-vector)
  ScalarType* xV1) const // OUT: The least variant direction (2-vector)
{
  NMP_ASSERT(xBar);
  NMP_ASSERT(xVar);
  NMP_ASSERT(xV0);
  NMP_ASSERT(xV1);

  if (m_S00 == 1)
  {
    // Mean
    xBar[0] = m_S10;
    xBar[1] = m_S01;

    // Variance
    xVar[0] = 0;
    xVar[1] = 0;

    // Principal directions
    xV0[0] = 1;
    xV0[1] = 0;
    xV1[0] = 0;
    xV1[1] = 1;

    return true;
  }

  if (m_S00 > 1)
  {
    // Mean
    xBar[0] = m_S10 / m_S00;
    xBar[1] = m_S01 / m_S00;

    // Covariance matrix
    ScalarType A[3];
    A[0] = (m_S20 / m_S00) - (xBar[0] * xBar[0]);
    A[1] = (m_S11 / m_S00) - (xBar[0] * xBar[1]);
    A[2] = (m_S02 / m_S00) - (xBar[1] * xBar[1]);

    // Compute the 2x2 symmetric Eigen-decomposition
    // D = V^T * A * V
    // V = [cos(t) sin(t); -sin(t) cos(t)]
    ScalarType c, s;
    Givens<ScalarType>::jacobi(A[0], A[1], A[2], c, s);

    // Diagonalise the 2x2 symmetric matrix A
    ScalarType cc = c * c;
    ScalarType ss = s * s;
    ScalarType cs_a12 = 2 * c * s * A[1];
    ScalarType D[2];
    D[0] = cc * A[0] - cs_a12 + ss * A[2];
    D[1] = ss * A[0] + cs_a12 + cc * A[2];

    // Set the output Eigen-vectors (principal directions)
    if (D[0] >= D[1])
    {
      xVar[0] = D[0];
      xVar[1] = D[1];
      xV0[0] = c;
      xV0[1] = -s;
      xV1[0] = s;
      xV1[1] = c;
    }
    else
    {
      xVar[0] = D[1];
      xVar[1] = D[0];
      xV0[0] = s;
      xV0[1] = c;
      xV1[0] = c;
      xV1[1] = -s;
    }

    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
