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
#ifndef NM_QUAT_UTILS_H
#define NM_QUAT_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
/// \file NMQuatUtils.h
/// \ingroup NaturalMotionPlatform
/// \brief Defines a set of maths utility functions for manipulating quaternions.
//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the mean of the set of quaternion rotations
void quatMean(uint32_t numSamples, const Quat* quats, Quat& qbar);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the angular variance of the set of quaternion rotations
float quatVar(uint32_t numSamples, const Quat* quats, bool zeroMean = false);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the matrix representation of the quaternion product
/// q_ = qqmul(p, q), i.e. the matrix-vector computation: q_ = P * q
/// \param p - The input quaternion p = [sin(theta/2)*n; cos(theta/2)]
/// \param P - The matrix representation of the quaternion product which must be a
///            (4 by 4) matrix
template <typename _T> NM_FORCEINLINE void quatMultMatrixL(const Quat& p, Matrix<_T>& P)
{
  NMP_ASSERT(P.isSize(4, 4));
  _T* ptr = P.data();

  // P = [ [w*eye(3)+vec_to_matrix(v); -v'], [v; w] ];
  ptr[0] = (_T)p.w;
  ptr[1] = (_T)p.z;
  ptr[2] = (_T) - p.y;
  ptr[3] = (_T) - p.x;
  ptr[4] = (_T) - p.z;
  ptr[5] = (_T)p.w;
  ptr[6] = (_T)p.x;
  ptr[7] = (_T) - p.y;
  ptr[8] = (_T)p.y;
  ptr[9] = (_T) - p.x;
  ptr[10] = (_T)p.w;
  ptr[11] = (_T) - p.z;
  ptr[12] = (_T)p.x;
  ptr[13] = (_T)p.y;
  ptr[14] = (_T)p.z;
  ptr[15] = (_T)p.w;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the matrix representation of the quaternion product
/// q_ = qqmul(p, q), i.e. the matrix-vector computation: q_ = Q * p
/// \param q - The input quaternion q = [sin(theta/2)*n; cos(theta/2)]
/// \param Q - The matrix representation of the quaternion product which must be a
///            (4 by 4) matrix
template <typename _T> NM_FORCEINLINE void quatMultMatrixR(const Quat& q, Matrix<_T>& Q)
{
  NMP_ASSERT(Q.isSize(4, 4));
  _T* ptr = Q.data();

  // P = [ [w*eye(3)-vec_to_matrix(v); -v'], [v;w] ];
  ptr[0] = (_T)q.w;
  ptr[1] = (_T) - q.z;
  ptr[2] = (_T)q.y;
  ptr[3] = (_T) - q.x;
  ptr[4] = (_T)q.z;
  ptr[5] = (_T)q.w;
  ptr[6] = (_T) - q.x;
  ptr[7] = (_T) - q.y;
  ptr[8] = (_T) - q.y;
  ptr[9] = (_T)q.x;
  ptr[10] = (_T)q.w;
  ptr[11] = (_T) - q.z;
  ptr[12] = (_T)q.x;
  ptr[13] = (_T)q.y;
  ptr[14] = (_T)q.z;
  ptr[15] = (_T)q.w;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the quaternion exponential map and the corresponding Jacobian Dq/Dv
/// \param v - The input quaternion log vector v = (theta/2)*n
/// \param J - The Jacobian Dq/Dv which must be a (4 by 3) matrix
/// \param q - The computed quaternion q = [sin(theta/2)*n; cos(theta/2)]
template <typename _T> NM_FORCEINLINE void quatExpJac(const Vector3& v, Matrix<_T>& J, Quat& q)
{
  NMP_ASSERT(J.isSize(4, 3));
  _T* ptr = J.data();

  float phi2 = v.x * v.x + v.y * v.y + v.z * v.z;
  float phi = sqrtf(phi2);
  if (phi < FLT_EPSILON)
  {
    // qexp
    q.identity();

    // Compute the Jacobian: J = [eye(3); -v']
    // First column
    ptr[0] = 1;
    ptr[1] = 0;
    ptr[2] = 0;
    ptr[3] = (_T) - v.x;

    // Second column
    ptr[4] = 0;
    ptr[5] = 1;
    ptr[6] = 0;
    ptr[7] = (_T) - v.y;

    // Third column
    ptr[8] = 0;
    ptr[9] = 0;
    ptr[10] = 1;
    ptr[11] = (_T) - v.z;

  }
  else
  {
    // qexp
    q.w = cosf(phi);
    float s_phi = sinf(phi) / phi;
    q.x = s_phi * v.x;
    q.y = s_phi * v.y;
    q.z = s_phi * v.z;

    // Compute the Jacobian: J = [s_phi*eye(3) + ((c-s_phi)/phi2) * v*v'; -s_phi*v'];
    float fac = (q.w - s_phi) / phi2;

    // Upper triangle of J(1:3, 1:3)
    ptr[0] = (_T)(s_phi + fac * v.x * v.x);
    ptr[4] = (_T)(fac * v.x * v.y);
    ptr[8] = (_T)(fac * v.x * v.z);
    ptr[5] = (_T)(s_phi + fac * v.y * v.y);
    ptr[9] = (_T)(fac * v.y * v.z);
    ptr[10] = (_T)(s_phi + fac * v.z * v.z);

    // Lower triangle of J(1:3, 1:3)
    ptr[1] = ptr[4];
    ptr[2] = ptr[8];
    ptr[6] = ptr[9];

    // Fourth row
    ptr[3] = (_T) - q.x;
    ptr[7] = (_T) - q.y;
    ptr[11] = (_T) - q.z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the quaternion log map and the corresponding Jacobian Dv/Dq
/// \param q - The input quaternion q = [sin(theta/2)*n; cos(theta/2)]
/// \param J - The Jacobian Dv/Dq which must be a (3 by 4) matrix
/// \param v - The computed quaternion log vector v = (theta/2)*n
template <typename _T> NM_FORCEINLINE void quatLogJac(const Quat& q, Matrix<_T>& J, Vector3& v)
{
  NMP_ASSERT(J.isSize(3, 4));
  _T* ptr = J.data();

  float s = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z);
  if (s < FLT_EPSILON)
  {
    // log
    v.set(0.0f);

    // Compute the Jacobian: J = [eye(3), -v]
    ptr[0] = 1;
    ptr[1] = 0;
    ptr[2] = 0;

    ptr[3] = 0;
    ptr[4] = 1;
    ptr[5] = 0;

    ptr[6] = 0;
    ptr[7] = 0;
    ptr[8] = 1;

  }
  else
  {
    // normalised axis vector n
    float phi = atan2(s, q.w);
    float oos = 1 / s;
    v.x = oos * q.x;
    v.y = oos * q.y;
    v.z = oos * q.z;

    // Compute the Jacobian: J = [c*A + (phi/s)*(eye(3) - A), -v]
    float alpha = phi * oos;
    float beta = q.w - alpha;

    // Upper triangle of J(1:3, 1:3)
    float temp = beta * v.x;
    ptr[0] = (_T)(temp * v.x + alpha);
    ptr[3] = (_T)(temp * v.y);
    ptr[6] = (_T)(temp * v.z);
    temp = beta * v.y;
    ptr[4] = (_T)(temp * v.y + alpha);
    ptr[7] = (_T)(temp * v.z);
    ptr[8] = (_T)(beta * v.z * v.z + alpha);

    // Lower triangle of J(1:3, 1:3)
    ptr[1] = ptr[3];
    ptr[2] = ptr[6];
    ptr[5] = ptr[7];

    // log
    v *= phi;
  }

  // Fourth column
  ptr[9] = (_T) - q.x;
  ptr[10] = (_T) - q.y;
  ptr[11] = (_T) - q.z;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the conversion of a tan quarter angle rotation vector
/// to a quaternion and its corresponding Jacobian Dq/Dv
/// \param q - The output quaternion q = [sin(theta/2)*n; cos(theta/2)]
/// \param J - The Jacobian Dq/Dv which must be a (4 by 3) matrix
/// \param v - The input tan quarter angle rotation vector v = tan(theta/4)*n
template <typename _T> NM_FORCEINLINE void fromRotationVectorTQAJac(Quat& q, Matrix<_T>& J, const Vector3& v)
{
  NMP_ASSERT(J.isSize(4, 3));
  _T* ptr = J.data();

  // Convert the tan quarter angle rotation vector to a quaternion
  float m2 = v.magnitudeSquared();
  float b = 1.0f / (1.0f + m2);
  float c = 2.0f * b;
  Vector3 u = v * c;
  q.setXYZW(u.x, u.y, u.z, (1.0f - m2) * b);

  // Compute the Jacobian
  float c2 = c * c;
  Vector3 dw_by_dv = v * -c2;

  // Dqv / Dv
  ptr[0] = (_T)(c + v.x * dw_by_dv.x);
  ptr[4] = (_T)(v.x * dw_by_dv.y);
  ptr[8] = (_T)(v.x * dw_by_dv.z);

  ptr[1] = (_T)(v.y * dw_by_dv.x);
  ptr[5] = (_T)(c + v.y * dw_by_dv.y);
  ptr[9] = (_T)(v.y * dw_by_dv.z);

  ptr[2] = (_T)(v.z * dw_by_dv.x);
  ptr[6] = (_T)(v.z * dw_by_dv.y);
  ptr[10] = (_T)(c + v.z * dw_by_dv.z);

  // Dqw / Dv
  ptr[3] = (_T)dw_by_dv.x;
  ptr[7] = (_T)dw_by_dv.y;
  ptr[11] = (_T)dw_by_dv.z;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the quaternion vector rotation: y = q * v * q^-1 and its corresponding Jacobian Dy/Dq
/// \param q - The input quaternion q = [sin(theta/2)*n; cos(theta/2)]
/// \param v - The input 3-vector to rotate
/// \param J - The Jacobian Dy/Dq which must be a (3 by 4) matrix
/// \param y - The output rotated 3-vector
template <typename _T> NM_FORCEINLINE void qvmulJac(const Quat& q, const Vector3& v, Matrix<_T>& J, Vector3& y)
{
  NMP_ASSERT(J.isSize(3, 4));
  _T* ptr = J.data();

  // y = q * v * q^-1
  y = q.rotateVector(v);

  // Jacobian
  Vector3 u, xv, yv, zv, wv;
  u = v * 2;
  xv = u * q.x;
  yv = u * q.y;
  zv = u * q.z;
  wv = u * q.w;

  // Dy / Dqv
  ptr[0] = (_T)(yv.y + zv.z);
  ptr[1] = (_T)(yv.x - 2 * xv.y - wv.z);
  ptr[2] = (_T)(zv.x + wv.y - 2 * xv.z);

  ptr[3] = (_T)(-2 * yv.x + xv.y + wv.z);
  ptr[4] = (_T)(xv.x + zv.z);
  ptr[5] = (_T)(-wv.x + zv.y - 2 * yv.z);

  ptr[6] = (_T)(-2 * zv.x - wv.y + xv.z);
  ptr[7] = (_T)(wv.x - 2 * zv.y + yv.z);
  ptr[8] = (_T)(xv.x + yv.y);

  // Dy / Dqw
  ptr[9] = (_T)(-zv.y + yv.z);
  ptr[10] = (_T)(zv.x - xv.z);
  ptr[11] = (_T)(-yv.x + xv.y);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the quaternion vector rotation: y = q * v * q^-1
/// and its corresponding Jacobian Dy/Dq
/// \param q - The input quaternion q = [sin(theta/2)*n; cos(theta/2)]
/// \param v - The input 3-vector to rotate
/// \param J - The Jacobian Dy/Dq which must be a (3 by 4) matrix
template <typename _T> NM_FORCEINLINE void qvmulJac(const Quat& q, const Vector3& v, Matrix<_T>& J)
{
  NMP_ASSERT(J.isSize(3, 4));
  _T* ptr = J.data();

  // Jacobian
  Vector3 u, xv, yv, zv, wv;
  u = v * 2;
  xv = u * q.x;
  yv = u * q.y;
  zv = u * q.z;
  wv = u * q.w;

  // Dy / Dqv
  ptr[0] = (_T)(yv.y + zv.z);
  ptr[1] = (_T)(yv.x - 2 * xv.y - wv.z);
  ptr[2] = (_T)(zv.x + wv.y - 2 * xv.z);

  ptr[3] = (_T)(-2 * yv.x + xv.y + wv.z);
  ptr[4] = (_T)(xv.x + zv.z);
  ptr[5] = (_T)(-wv.x + zv.y - 2 * yv.z);

  ptr[6] = (_T)(-2 * zv.x - wv.y + xv.z);
  ptr[7] = (_T)(wv.x - 2 * zv.y + yv.z);
  ptr[8] = (_T)(xv.x + yv.y);

  // Dy / Dqw
  ptr[9] = (_T)(-zv.y + yv.z);
  ptr[10] = (_T)(zv.x - xv.z);
  ptr[11] = (_T)(-yv.x + xv.y);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_UTILS_H
