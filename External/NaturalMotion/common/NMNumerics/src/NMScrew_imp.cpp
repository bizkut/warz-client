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
// Screw
//----------------------------------------------------------------------------------------------------------------------
template <>
Screw<ScalarType>::~Screw()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Screw<ScalarType>::Screw() :
  SVD(3, 3), m_v(3), m_x(3)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Screw<ScalarType>::Screw(const Matrix<ScalarType>& R, const Vector<ScalarType>& t) :
  SVD(3, 3), m_v(3), m_x(3)
{
  // Compute the screw decomposition
  compute(R, t);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::compute(const Matrix<ScalarType>& R, const Vector<ScalarType>& t)
{
  NMP_ASSERT(R.isSize(3, 3));
  NMP_ASSERT(t.numElements() == 3);

  // We need to find the set of points that remain fixed under the
  // transformation: R x + t = x  i.e.  (R - I) x = -t
  Matrix<ScalarType>& A = SVD::getA();
  A = R;
  A.addDiagVector(0, static_cast<ScalarType>(-1));

  // Since R = I + sin(theta)*[v]_x + (1-cos(theta))*[v]_x^{2}
  // then (R - I) is rank 2 if theta != 0, or rank 0 if theta = 0
  SVD::compute(A);

  // Ensure that matrix A corresponds to a rotation by setting the
  // first two singular values to have the same value. Note the magnitude
  // of the singular value is a factor of the angle of rotation.
  Vector<ScalarType>& D = SVD::getD();
  ScalarType sigma = (D[0] + D[1]) / 2;
  D[0] = sigma;
  D[1] = sigma;
  D[2] = 0;

  // Check if R = I within the specified rank tolerance
  if (sigma < m_rTol)
  {
    // Normalize the axis unit direction vector
    m_v = t;
    ScalarType mag = NMP::enorm<ScalarType, ScalarType>(m_v);
    if (mag > 0) m_v /= mag; else m_v[0] = static_cast<ScalarType>(1);

    m_theta = static_cast<ScalarType>(0);
    m_x.set(static_cast<ScalarType>(0));
    m_lambda = m_v.dot(t);
    return;
  }

  // Solve for the least squares point on the axis that does not move
  // (U D V^T) x = -t
  for (uint32_t i = 0; i < 3; ++i) m_v[i] = -t[i];
  SVD::solve(m_x, m_v);

  // Recompose the matrix (R-I) = U * D_ * V^T
  SVD::recompose(A);

  // Compute the rotation axis direction and angle
  SVD::nullVector(m_v);
  ScalarType sinv[3];
  sinv[0] = A.element(2, 1) - A.element(1, 2);
  sinv[1] = A.element(0, 2) - A.element(2, 0);
  sinv[2] = A.element(1, 0) - A.element(0, 1);

  ScalarType sintheta = (m_v[0] * sinv[0] + m_v[1] * sinv[1] + m_v[2] * sinv[2]) / 2;
  ScalarType costheta = (A.trace() + 2) / 2;
  m_theta = atan2(sintheta, costheta);

  // Compute the parallel translation along the screw axis
  ScalarType r[3];
  A.preMultiply(r, &m_x[0]);
  m_lambda = static_cast<ScalarType>(0);
  for (uint32_t i = 0; i < 3; ++i) m_lambda += (r[i] + t[i]) * m_v[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::recompose(Matrix<ScalarType>& R, Vector<ScalarType>& t)
{
  // The original transformation can be recomposed by computing:
  // H = T_lambda * T_x * R_theta * T_x^-1

  // Get a temporary 3 by 3 matrix
  Matrix<ScalarType>& A = m_Bidiag.getQ();

  if (m_theta == 0)
  {
    R.setIdentity();
    for (uint32_t i = 0; i < 3; ++i) t[i] = m_lambda * m_v[i];
  }
  else
  {
    // Get the cross product matrix [v]_x
    R.setDiagVector(0, static_cast<ScalarType>(0));
    R.element(0, 1) = -m_v[2]; R.element(1, 0) = m_v[2];
    R.element(0, 2) = m_v[1]; R.element(2, 0) = -m_v[1];
    R.element(1, 2) = -m_v[0]; R.element(2, 1) = m_v[0];

    // R = I + sin(theta)*[v]_x + (1-cos(theta))*[v]_x^{ 2 }
    A.setFromMultiplication(R, R);
    A *= (1 - cos(m_theta));
    R *= sin(m_theta);
    R += A;
    R.addDiagVector(0, static_cast<ScalarType>(1));

    // t = x - R*x + lambda*v
    ScalarType w[3];
    R.preMultiply(w, &m_x[0]);
    for (uint32_t i = 0; i < 3; ++i)
    {
      t[i] = m_x[i] - w[i] + m_lambda * m_v[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::closest(Vector<ScalarType>& x, const Vector<ScalarType>& u) const
{
  NMP_ASSERT(x.numElements() == 3 && u.numElements() == 3);
  closest(&x[0], &u[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::closest(ScalarType* x, const ScalarType* u) const
{
  // p = x + alpha * v
  // (p - u)^T * v = 0

  // Compute alpha = (u - x)^T * v
  ScalarType alpha = static_cast<ScalarType>(0);
  for (uint32_t i = 0; i < 3; ++i) alpha += ((u[i] - m_x[i]) * m_v[i]);

  // Compute p = x + alpha * v
  for (uint32_t i = 0; i < 3; ++i) x[i] = m_x[i] + alpha * m_v[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& Screw<ScalarType>::getAxisV() const
{
  return m_v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Screw<ScalarType>::getAxisV()
{
  return m_v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::getAxisV(ScalarType* v) const
{
  for (uint32_t i = 0; i < 3; ++i) v[i] = m_v[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::getAxisV(Vector3& v) const
{
  v.set((float)m_v[0], (float)m_v[1], (float)m_v[2]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& Screw<ScalarType>::getAxisX() const
{
  return m_x;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Screw<ScalarType>::getAxisX()
{
  return m_x;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::getAxisX(ScalarType* x) const
{
  for (uint32_t i = 0; i < 3; ++i) x[i] = m_x[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Screw<ScalarType>::getAxisX(Vector3& x) const
{
  x.set((float)m_x[0], (float)m_x[1], (float)m_x[2]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Screw<ScalarType>::getTheta() const
{
  return m_theta;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Screw<ScalarType>::getLambda() const
{
  return m_lambda;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
