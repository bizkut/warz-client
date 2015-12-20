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
// inline included by NMVector3.h
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMQuat.h"

#include "float.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34::Matrix34()
{
  // to init to something useful, use the ctor taking MatrixConstruction enum instead
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34::Matrix34(Matrix34::MatrixConstruction _mc)
{
  initialise(_mc);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34::Matrix34(const NMP::Matrix34& copy)
{
  initialise(copy);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34::Matrix34(const NMP::Vector3& xRow, const NMP::Vector3& yRow, const NMP::Vector3& zRow, const NMP::Vector3& trans)
{
  initialise(xRow, yRow, zRow, trans);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34::Matrix34(const NMP::Quat& quat)
{
  fromQuat(quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34::Matrix34(const Quat& quat, const NMP::Vector3& trans)
{
  fromQuat(quat);
  r[3] = trans;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34 Matrix34::operator * (const NMP::Matrix34& m) const
{
  Matrix34 result(*this);
  result.multiply(m);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::operator *= (const NMP::Matrix34& m)
{
  multiply(m);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34 Matrix34::operator + (const NMP::Matrix34& m) const
{
  Matrix34 result(*this);
  result += m;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::operator += (const NMP::Matrix34& m)
{
  r[0] += m.r[0];
  r[1] += m.r[1];
  r[2] += m.r[2];
  r[3] += m.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Matrix34 Matrix34::operator * (const float rhs) const
{
  Matrix34 result(*this);
  result *= rhs;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::operator *= (const float rhs)
{
  r[0] *= rhs;
  r[1] *= rhs;
  r[2] *= rhs;
  r[3] *= rhs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float& Matrix34::operator()(int32_t row, int32_t column)
{
  NMP_ASSERT(row >= 0 && row < 4);
  NMP_ASSERT(column >= 0 && column < 3);
  return r[row][column];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Matrix34::operator() (int32_t row, int32_t column) const
{
  NMP_ASSERT(row >= 0 && row < 4);
  NMP_ASSERT(column >= 0 && column < 3);
  return r[row][column];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const Matrix34& Matrix34::operator = (const NMP::Matrix34& m)
{
  r[0] = m.r[0];
  r[1] = m.r[1];
  r[2] = m.r[2];
  r[3] = m.r[3];

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::initialise(Matrix34::MatrixConstruction _mc)
{
  if (_mc == Matrix34::kIdentity)
    identity();
  else if (_mc == Matrix34::kZero)
    zero();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::initialise(const NMP::Matrix34& copy)
{
  r[0] = copy.r[0];
  r[1] = copy.r[1];
  r[2] = copy.r[2];
  r[3] = copy.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::initialise(const NMP::Vector3& xRow, const NMP::Vector3& yRow, const NMP::Vector3& zRow, const NMP::Vector3& trans)
{
  r[0] = xRow;
  r[1] = yRow;
  r[2] = zRow;
  r[3] = trans;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::initialise(const NMP::Quat& quat)
{
  fromQuat(quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::initialise(const Quat& quat, const NMP::Vector3& trans)
{
  fromQuat(quat);
  r[3] = trans;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::compare(const NMP::Matrix34& m, const float tolerance) const
{
  if (!r[0].compare(m.r[0], tolerance))
    return false;

  if (!r[1].compare(m.r[1], tolerance))
    return false;

  if (!r[2].compare(m.r[2], tolerance))
    return false;

  if (!r[3].compare(m.r[3], tolerance))
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::compareTranslation(const NMP::Matrix34& m, const float tolerance) const
{
  if (!r[3].compare(m.r[3], tolerance))
    return false;

  return true;
}

NM_INLINE bool Matrix34::compareRotation(const NMP::Matrix34& m, const float tolerance) const
{
  if (!r[0].compare(m.r[0], tolerance))
    return false;

  if (!r[1].compare(m.r[1], tolerance))
    return false;

  if (!r[2].compare(m.r[2], tolerance))
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::operator==(const NMP::Matrix34& rhs) const
{
  return compare(rhs, 0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Matrix34::getColumn(uint32_t index) const
{
  NMP_ASSERT(index < 3);
  Vector3 result(r[0][index], r[1][index], r[2][index]);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::setColumn(const NMP::Vector3& v, uint32_t index)
{
  NMP_ASSERT(index < 3);
  r[0][index] = v.x;
  r[1][index] = v.y;
  r[2][index] = v.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Matrix34::determinant() const
{
  float determinant;
  determinant = r[0].x * (r[1].y * r[2].z - r[2].y * r[1].z);
  determinant += r[1].x * (r[2].y * r[0].z - r[0].y * r[2].z);
  determinant += r[2].x * (r[0].y * r[1].z - r[1].y * r[0].z);

  return determinant;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::isIdentity3x3(float tolerance) const
{
  Vector3 test(0, 0, 0);
  for (int32_t i = 0; i < 3; ++i)
  {
    test[i] = 1.0f;
    if (test.summedAbsDiff(r[i]) > tolerance)
      return false;
    test[i] = 0.0f;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::isIdentity(float tolerance) const
{
  Vector3 test(0, 0, 0);
  for (int32_t i = 0; i < 3; ++i)
  {
    test[i] = 1.0f;
    if (test.summedAbsDiff(r[i]) > tolerance)
      return false;
    test[i] = 0.0f;
  }
  if (test.summedAbsDiff(r[3]) > tolerance)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::isValidTM3x3(float tolerance) const
{
  Matrix34 product, transposed;

  // orthonormal 3x3 component test; true if A x At = I
  transposed.transpose3x3(*this);
  product.multiply(*this, transposed);

  if (!product.isIdentity3x3(tolerance))
    return false;

  return (fabs(determinant() - 1) <= tolerance);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::isValidTM(float tolerance) const
{
  if (!isValidTM3x3(tolerance))
    return false;

  if (!translation().isValid())
    return false;
  else
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::isIsotropic() const
{
  if (!((r[0].x == r[1].y) && (r[1].y == r[2].z)))
    return false;

  if (!((r[0].y == 0) && (r[0].z == 0) && (r[1].z == 0)))
    return false;

  if (!((r[1].x == 0) && (r[2].x == 0) && (r[2].y == 0)))
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::identity3x3()
{
  r[0].set(1, 0, 0);
  r[1].set(0, 1, 0);
  r[2].set(0, 0, 1);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::identity()
{
  identity3x3();
  r[3].setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::zero3x3()
{
  r[0].setToZero();
  r[1].setToZero();
  r[2].setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::zero()
{
  zero3x3();
  r[3].setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::set3x3(const NMP::Matrix34& m)
{
  r[0].set(m.r[0]);
  r[1].set(m.r[1]);
  r[2].set(m.r[2]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::set(const NMP::Matrix34& m)
{
  r[0] = m.r[0];
  r[1] = m.r[1];
  r[2] = m.r[2];
  r[3] = m.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::set3x3ToXRotation(float angleInRadians)
{
  const float s = sinf(angleInRadians);
  const float c = cosf(angleInRadians);

  r[0].set(1, 0, 0);
  r[1].set(0, c, -s);
  r[2].set(0, s, c);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::set3x3ToYRotation(float angleInRadians)
{
  const float s = sinf(angleInRadians);
  const float c = cosf(angleInRadians);

  r[0].set(c, 0, -s);
  r[1].set(0, 1, 0);
  r[2].set(s, 0, c);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::set3x3ToZRotation(float angleInRadians)
{
  const float s = sinf(angleInRadians);
  const float c = cosf(angleInRadians);

  r[0].set(c, -s, 0);
  r[1].set(s, c, 0);
  r[2].set(0, 0, 1);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::add(const NMP::Matrix34& a, const NMP::Matrix34& b)
{
  r[0] = a.r[0] + b.r[0];
  r[1] = a.r[1] + b.r[1];
  r[2] = a.r[2] + b.r[2];
  r[3] = a.r[3] + b.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::add(const NMP::Matrix34& m)
{
  r[0] += m.r[0];
  r[1] += m.r[1];
  r[2] += m.r[2];
  r[3] += m.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::add3x3(const NMP::Matrix34& m)
{
  r[0] += m.r[0];
  r[1] += m.r[1];
  r[2] += m.r[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::subtract(const NMP::Matrix34& a, const NMP::Matrix34& b)
{
  r[0] = a.r[0] - b.r[0];
  r[1] = a.r[1] - b.r[1];
  r[2] = a.r[2] - b.r[2];
  r[3] = a.r[3] - b.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::subtract(const NMP::Matrix34& m)
{
  r[0] -= m.r[0];
  r[1] -= m.r[1];
  r[2] -= m.r[2];
  r[3] -= m.r[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::multiply(const NMP::Matrix34& a, const NMP::Matrix34& b)
{
  float xAxis_x = a.r[0].x * b.r[0].x + a.r[0].y * b.r[1].x + a.r[0].z * b.r[2].x;
  float xAxis_y = a.r[0].x * b.r[0].y + a.r[0].y * b.r[1].y + a.r[0].z * b.r[2].y;
  float xAxis_z = a.r[0].x * b.r[0].z + a.r[0].y * b.r[1].z + a.r[0].z * b.r[2].z;

  float yAxis_x = a.r[1].x * b.r[0].x + a.r[1].y * b.r[1].x + a.r[1].z * b.r[2].x;
  float yAxis_y = a.r[1].x * b.r[0].y + a.r[1].y * b.r[1].y + a.r[1].z * b.r[2].y;
  float yAxis_z = a.r[1].x * b.r[0].z + a.r[1].y * b.r[1].z + a.r[1].z * b.r[2].z;

  float zAxis_x = a.r[2].x * b.r[0].x + a.r[2].y * b.r[1].x + a.r[2].z * b.r[2].x;
  float zAxis_y = a.r[2].x * b.r[0].y + a.r[2].y * b.r[1].y + a.r[2].z * b.r[2].y;
  float zAxis_z = a.r[2].x * b.r[0].z + a.r[2].y * b.r[1].z + a.r[2].z * b.r[2].z;

  float trans_x = a.r[3].x * b.r[0].x + a.r[3].y * b.r[1].x + a.r[3].z * b.r[2].x + b.r[3].x;
  float trans_y = a.r[3].x * b.r[0].y + a.r[3].y * b.r[1].y + a.r[3].z * b.r[2].y + b.r[3].y;
  float trans_z = a.r[3].x * b.r[0].z + a.r[3].y * b.r[1].z + a.r[3].z * b.r[2].z + b.r[3].z;

  r[0].set(xAxis_x, xAxis_y, xAxis_z);
  r[1].set(yAxis_x, yAxis_y, yAxis_z);
  r[2].set(zAxis_x, zAxis_y, zAxis_z);
  r[3].set(trans_x, trans_y, trans_z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::multiply4x4(const NMP::Matrix34& a, const NMP::Matrix34& b)
{
  float xAxis_x = a.r[0].x * b.r[0].x + a.r[0].y * b.r[1].x + a.r[0].z * b.r[2].x + a.r[0].w * b.r[3].x;
  float xAxis_y = a.r[0].x * b.r[0].y + a.r[0].y * b.r[1].y + a.r[0].z * b.r[2].y + a.r[0].w * b.r[3].y;
  float xAxis_z = a.r[0].x * b.r[0].z + a.r[0].y * b.r[1].z + a.r[0].z * b.r[2].z + a.r[0].w * b.r[3].z;
  float xAxis_w = a.r[0].x * b.r[0].w + a.r[0].y * b.r[1].w + a.r[0].z * b.r[2].w + a.r[0].w * b.r[3].w;

  float yAxis_x = a.r[1].x * b.r[0].x + a.r[1].y * b.r[1].x + a.r[1].z * b.r[2].x + a.r[1].w * b.r[3].x;
  float yAxis_y = a.r[1].x * b.r[0].y + a.r[1].y * b.r[1].y + a.r[1].z * b.r[2].y + a.r[1].w * b.r[3].y;
  float yAxis_z = a.r[1].x * b.r[0].z + a.r[1].y * b.r[1].z + a.r[1].z * b.r[2].z + a.r[1].w * b.r[3].z;
  float yAxis_w = a.r[1].x * b.r[0].w + a.r[1].y * b.r[1].w + a.r[1].z * b.r[2].w + a.r[1].w * b.r[3].w;

  float zAxis_x = a.r[2].x * b.r[0].x + a.r[2].y * b.r[1].x + a.r[2].z * b.r[2].x + a.r[2].w * b.r[3].x;
  float zAxis_y = a.r[2].x * b.r[0].y + a.r[2].y * b.r[1].y + a.r[2].z * b.r[2].y + a.r[2].w * b.r[3].y;
  float zAxis_z = a.r[2].x * b.r[0].z + a.r[2].y * b.r[1].z + a.r[2].z * b.r[2].z + a.r[2].w * b.r[3].z;
  float zAxis_w = a.r[2].x * b.r[0].w + a.r[2].y * b.r[1].w + a.r[2].z * b.r[2].w + a.r[2].w * b.r[3].w;

  float trans_x = a.r[3].x * b.r[0].x + a.r[3].y * b.r[1].x + a.r[3].z * b.r[2].x + a.r[3].w * b.r[3].x;
  float trans_y = a.r[3].x * b.r[0].y + a.r[3].y * b.r[1].y + a.r[3].z * b.r[2].y + a.r[3].w * b.r[3].y;
  float trans_z = a.r[3].x * b.r[0].z + a.r[3].y * b.r[1].z + a.r[3].z * b.r[2].z + a.r[3].w * b.r[3].z;
  float trans_w = a.r[3].x * b.r[0].w + a.r[3].y * b.r[1].w + a.r[3].z * b.r[2].w + a.r[3].w * b.r[3].w;

  r[0].set(xAxis_x, xAxis_y, xAxis_z, xAxis_w);
  r[1].set(yAxis_x, yAxis_y, yAxis_z, yAxis_w);
  r[2].set(zAxis_x, zAxis_y, zAxis_z, zAxis_w);
  r[3].set(trans_x, trans_y, trans_z, trans_w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::multiply(const NMP::Matrix34& m)
{
  float xAxis_x = r[0].x * m.r[0].x + r[0].y * m.r[1].x + r[0].z * m.r[2].x;
  float xAxis_y = r[0].x * m.r[0].y + r[0].y * m.r[1].y + r[0].z * m.r[2].y;
  float xAxis_z = r[0].x * m.r[0].z + r[0].y * m.r[1].z + r[0].z * m.r[2].z;

  float yAxis_x = r[1].x * m.r[0].x + r[1].y * m.r[1].x + r[1].z * m.r[2].x;
  float yAxis_y = r[1].x * m.r[0].y + r[1].y * m.r[1].y + r[1].z * m.r[2].y;
  float yAxis_z = r[1].x * m.r[0].z + r[1].y * m.r[1].z + r[1].z * m.r[2].z;

  float zAxis_x = r[2].x * m.r[0].x + r[2].y * m.r[1].x + r[2].z * m.r[2].x;
  float zAxis_y = r[2].x * m.r[0].y + r[2].y * m.r[1].y + r[2].z * m.r[2].y;
  float zAxis_z = r[2].x * m.r[0].z + r[2].y * m.r[1].z + r[2].z * m.r[2].z;

  float trans_x = r[3].x * m.r[0].x + r[3].y * m.r[1].x + r[3].z * m.r[2].x + m.r[3].x;
  float trans_y = r[3].x * m.r[0].y + r[3].y * m.r[1].y + r[3].z * m.r[2].y + m.r[3].y;
  float trans_z = r[3].x * m.r[0].z + r[3].y * m.r[1].z + r[3].z * m.r[2].z + m.r[3].z;

  r[0].set(xAxis_x, xAxis_y, xAxis_z);
  r[1].set(yAxis_x, yAxis_y, yAxis_z);
  r[2].set(zAxis_x, zAxis_y, zAxis_z);
  r[3].set(trans_x, trans_y, trans_z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::multiply3x3(const NMP::Matrix34& a, const NMP::Matrix34& b)
{
  float xAxis_x = a.r[0].x * b.r[0].x + a.r[0].y * b.r[1].x + a.r[0].z * b.r[2].x;
  float xAxis_y = a.r[0].x * b.r[0].y + a.r[0].y * b.r[1].y + a.r[0].z * b.r[2].y;
  float xAxis_z = a.r[0].x * b.r[0].z + a.r[0].y * b.r[1].z + a.r[0].z * b.r[2].z;

  float yAxis_x = a.r[1].x * b.r[0].x + a.r[1].y * b.r[1].x + a.r[1].z * b.r[2].x;
  float yAxis_y = a.r[1].x * b.r[0].y + a.r[1].y * b.r[1].y + a.r[1].z * b.r[2].y;
  float yAxis_z = a.r[1].x * b.r[0].z + a.r[1].y * b.r[1].z + a.r[1].z * b.r[2].z;

  float zAxis_x = a.r[2].x * b.r[0].x + a.r[2].y * b.r[1].x + a.r[2].z * b.r[2].x;
  float zAxis_y = a.r[2].x * b.r[0].y + a.r[2].y * b.r[1].y + a.r[2].z * b.r[2].y;
  float zAxis_z = a.r[2].x * b.r[0].z + a.r[2].y * b.r[1].z + a.r[2].z * b.r[2].z;

  r[0].set(xAxis_x, xAxis_y, xAxis_z);
  r[1].set(yAxis_x, yAxis_y, yAxis_z);
  r[2].set(zAxis_x, zAxis_y, zAxis_z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::multiply3x3(const NMP::Matrix34& m)
{
  float xAxis_x = r[0].x * m.r[0].x + r[0].y * m.r[1].x + r[0].z * m.r[2].x;
  float xAxis_y = r[0].x * m.r[0].y + r[0].y * m.r[1].y + r[0].z * m.r[2].y;
  float xAxis_z = r[0].x * m.r[0].z + r[0].y * m.r[1].z + r[0].z * m.r[2].z;

  float yAxis_x = r[1].x * m.r[0].x + r[1].y * m.r[1].x + r[1].z * m.r[2].x;
  float yAxis_y = r[1].x * m.r[0].y + r[1].y * m.r[1].y + r[1].z * m.r[2].y;
  float yAxis_z = r[1].x * m.r[0].z + r[1].y * m.r[1].z + r[1].z * m.r[2].z;

  float zAxis_x = r[2].x * m.r[0].x + r[2].y * m.r[1].x + r[2].z * m.r[2].x;
  float zAxis_y = r[2].x * m.r[0].y + r[2].y * m.r[1].y + r[2].z * m.r[2].y;
  float zAxis_z = r[2].x * m.r[0].z + r[2].y * m.r[1].z + r[2].z * m.r[2].z;

  r[0].set(xAxis_x, xAxis_y, xAxis_z);
  r[1].set(yAxis_x, yAxis_y, yAxis_z);
  r[2].set(zAxis_x, zAxis_y, zAxis_z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::transpose3x3()
{
  NMP::nmSwap(r[0].y, r[1].x);
  NMP::nmSwap(r[0].z, r[2].x);
  NMP::nmSwap(r[1].z, r[2].y);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::transpose4x4()
{
  NMP::nmSwap(r[0].y, r[1].x);
  NMP::nmSwap(r[0].z, r[2].x);
  NMP::nmSwap(r[1].z, r[2].y);

  NMP::nmSwap(r[3].x, r[0].w);
  NMP::nmSwap(r[3].y, r[1].w);
  NMP::nmSwap(r[3].z, r[2].w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::transpose3x3(const NMP::Matrix34& m)
{
  float xAxis_y = m.r[1].x;
  float xAxis_z = m.r[2].x;

  float yAxis_x = m.r[0].y;
  float yAxis_z = m.r[2].y;

  float zAxis_x = m.r[0].z;
  float zAxis_y = m.r[1].z;

  r[0].set(m.r[0].x, xAxis_y, xAxis_z);
  r[1].set(yAxis_x, m.r[1].y, yAxis_z);
  r[2].set(zAxis_x, zAxis_y, m.r[2].z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::invertFast3x3()
{
  // transpose (i.e. invert) the rotation matrix
  NMP::nmSwap(r[0].y, r[1].x);
  NMP::nmSwap(r[0].z, r[2].x);
  NMP::nmSwap(r[1].z, r[2].y);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::invertFast()
{
  Vector3 copyTrans(r[3]);

  // multiply position vector by -1 * inv(rotation matrix)
  r[3].x = -(r[0].x * copyTrans.x + r[0].y * copyTrans.y + r[0].z * copyTrans.z);
  r[3].y = -(r[1].x * copyTrans.x + r[1].y * copyTrans.y + r[1].z * copyTrans.z);
  r[3].z = -(r[2].x * copyTrans.x + r[2].y * copyTrans.y + r[2].z * copyTrans.z);

  invertFast3x3();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::invertFast3x3(const NMP::Matrix34& m)
{
  // copy rotation component
  r[0].set(m.r[0]);
  r[1].set(m.r[1]);
  r[2].set(m.r[2]);

  invertFast3x3();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::invertFast(const NMP::Matrix34& m)
{
  // multiply position vector by -1 * inv(rotation matrix)
  r[3].x = -(m.r[0].x * m.r[3].x + m.r[0].y * m.r[3].y + m.r[0].z * m.r[3].z);
  r[3].y = -(m.r[1].x * m.r[3].x + m.r[1].y * m.r[3].y + m.r[1].z * m.r[3].z);
  r[3].z = -(m.r[2].x * m.r[3].x + m.r[2].y * m.r[3].y + m.r[2].z * m.r[3].z);

  invertFast3x3(m);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::invert3x3()
{
  float determinant;

  float d00 = (r[1].y * r[2].z - r[2].y * r[1].z);
  float d01 = (r[2].y * r[0].z - r[0].y * r[2].z);
  float d02 = (r[0].y * r[1].z - r[1].y * r[0].z);

  // early out if the matrix is singular
  determinant = (r[0].x * d00) + (r[1].x * d01) + (r[2].x * d02);
  if (determinant == 0.0f)
    return false;

  float d10 = (r[1].z * r[2].x - r[1].x * r[2].z);
  float d11 = (r[0].x * r[2].z - r[0].z * r[2].x);
  float d12 = (r[0].z * r[1].x - r[0].x * r[1].z);

  float d20 = (r[1].x * r[2].y - r[1].y * r[2].x);
  float d21 = (r[0].y * r[2].x - r[0].x * r[2].y);
  float d22 = (r[0].x * r[1].y - r[0].y * r[1].x);

  determinant = 1.0f / determinant;

  r[0].set(d00 * determinant, d01 * determinant, d02 * determinant);
  r[1].set(d10 * determinant, d11 * determinant, d12 * determinant);
  r[2].set(d20 * determinant, d21 * determinant, d22 * determinant);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::invert()
{
  if (invert3x3())
  {
    Vector3 copyTrans(r[3]);

    // multiply position vector by -1 * inv(rotation matrix)
    r[3].x = -(r[0].x * copyTrans.x + r[1].x * copyTrans.y + r[2].x * copyTrans.z);
    r[3].y = -(r[0].y * copyTrans.x + r[1].y * copyTrans.y + r[2].y * copyTrans.z);
    r[3].z = -(r[0].z * copyTrans.x + r[1].z * copyTrans.y + r[2].z * copyTrans.z);

    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::invert3x3(const NMP::Matrix34& m)
{
  Matrix34 temporary(m);

  if (temporary.invert3x3())
  {
    // copy rotation component
    r[0].set(temporary.r[0]);
    r[1].set(temporary.r[1]);
    r[2].set(temporary.r[2]);

    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Matrix34::invert(const NMP::Matrix34& m)
{
  Matrix34 temporary(m);

  if (temporary.invert3x3())
  {
    r[0].set(temporary.r[0]);
    r[1].set(temporary.r[1]);
    r[2].set(temporary.r[2]);

    // multiply position vector by -1 * inv(rotation matrix)
    r[3].x = -(r[0].x * m.r[3].x + r[1].x * m.r[3].y + r[2].x * m.r[3].z);
    r[3].y = -(r[0].y * m.r[3].x + r[1].y * m.r[3].y + r[2].y * m.r[3].z);
    r[3].z = -(r[0].z * m.r[3].x + r[1].z * m.r[3].y + r[2].z * m.r[3].z);

    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::scale(float f)
{
  r[0] *= f;
  r[1] *= f;
  r[2] *= f;
  r[3] *= f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::scale(const NMP::Vector3& v)
{
  r[0].multiplyElements(v);
  r[1].multiplyElements(v);
  r[2].multiplyElements(v);
  r[3].multiplyElements(v);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::scale3x3(float f)
{
  r[0] *= f;
  r[1] *= f;
  r[2] *= f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::scale3x3(const NMP::Vector3& v)
{
  r[0].multiplyElements(v);
  r[1].multiplyElements(v);
  r[2].multiplyElements(v);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::transformVector(NMP::Vector3& v) const
{
  float newX = v.x * r[0].x + v.y * r[1].x + v.z * r[2].x + r[3].x;
  float newY = v.x * r[0].y + v.y * r[1].y + v.z * r[2].y + r[3].y;
  float newZ = v.x * r[0].z + v.y * r[1].z + v.z * r[2].z + r[3].z;
  v.x = newX;
  v.y = newY;
  v.z = newZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Matrix34::getTransformedVector(const NMP::Vector3& v) const
{
  NMP::Vector3 result;
  transformVector(v, result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::rotateVector(NMP::Vector3& v) const
{
  float newX = v.x * r[0].x + v.y * r[1].x + v.z * r[2].x;
  float newY = v.x * r[0].y + v.y * r[1].y + v.z * r[2].y;
  float newZ = v.x * r[0].z + v.y * r[1].z + v.z * r[2].z;
  v.x = newX;
  v.y = newY;
  v.z = newZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Matrix34::getRotatedVector(const NMP::Vector3& v) const
{
  NMP::Vector3 result;
  rotateVector(v, result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::inverseTransformVector(NMP::Vector3& v) const
{
  Vector3 t;
  t = v - r[3];

  float newX = t.x * r[0].x + t.y * r[0].y + t.z * r[0].z;
  float newY = t.x * r[1].x + t.y * r[1].y + t.z * r[1].z;
  float newZ = t.x * r[2].x + t.y * r[2].y + t.z * r[2].z;
  v.x = newX;
  v.y = newY;
  v.z = newZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::inverseRotateVector(NMP::Vector3& v) const
{
  float newX = v.x * r[0].x + v.y * r[0].y + v.z * r[0].z;
  float newY = v.x * r[1].x + v.y * r[1].y + v.z * r[1].z;
  float newZ = v.x * r[2].x + v.y * r[2].y + v.z * r[2].z;
  v.x = newX;
  v.y = newY;
  v.z = newZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::transformVector(const NMP::Vector3& in, NMP::Vector3& out) const
{
  out.x = in.x * r[0].x + in.y * r[1].x + in.z * r[2].x + r[3].x;
  out.y = in.x * r[0].y + in.y * r[1].y + in.z * r[2].y + r[3].y;
  out.z = in.x * r[0].z + in.y * r[1].z + in.z * r[2].z + r[3].z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::transformVectorFull(NMP::Vector3& in, NMP::Vector3& out) const
{
  out.x = in.x * r[0].x + in.y * r[1].x + in.z * r[2].x + in.w * r[3].x;
  out.y = in.x * r[0].y + in.y * r[1].y + in.z * r[2].y + in.w * r[3].y;
  out.z = in.x * r[0].z + in.y * r[1].z + in.z * r[2].z + in.w * r[3].z;
  out.w = in.x * r[0].w + in.y * r[1].w + in.z * r[2].w + in.w * r[3].w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::rotateVector(const NMP::Vector3& in, NMP::Vector3& out) const
{
  out.x = in.x * r[0].x + in.y * r[1].x + in.z * r[2].x;
  out.y = in.x * r[0].y + in.y * r[1].y + in.z * r[2].y;
  out.z = in.x * r[0].z + in.y * r[1].z + in.z * r[2].z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::inverseTransformVector(const NMP::Vector3& in, NMP::Vector3& out) const
{
  Vector3 t;
  t = ((Vector3)in) - r[3];

  out.x = t.x * r[0].x + t.y * r[0].y + t.z * r[0].z;
  out.y = t.x * r[1].x + t.y * r[1].y + t.z * r[1].z;
  out.z = t.x * r[2].x + t.y * r[2].y + t.z * r[2].z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::inverseRotateVector(const NMP::Vector3& in, NMP::Vector3& out) const
{
  out.x = in.x * r[0].x + in.y * r[0].y + in.z * r[0].z;
  out.y = in.x * r[1].x + in.y * r[1].y + in.z * r[1].z;
  out.z = in.x * r[2].x + in.y * r[2].y + in.z * r[2].z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::orthonormalise()
{
  float dot;
  r[0].normaliseGetLength(NMP::Vector3::InitTypeOneX);
  dot = r[0].dot(r[1]);

  r[1] += (r[0] * -dot);
  r[1].normaliseGetLength(NMP::Vector3::InitTypeOneY);

  r[2].cross(r[0], r[1]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::interpolate(const NMP::Matrix34& a, const NMP::Matrix34& b, float t)
{
  // Short-cut blend
  if (t == 0.0f)
    set(a);
  else if (t == 1.0f)
    set(b);

  else
  {
    // create quats from the matrix positions, slerp them
    Quat aRot = a.toQuat();
    Quat bRot = b.toQuat();
    Quat result;
    result.slerp(aRot, bRot, t, aRot.dot(bRot));

    // set new rotation from slerp'd quat
    fromQuat(result);

    // lerp the r[3]
    r[3].lerp(a.r[3], b.r[3], t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::interpolate(const NMP::Matrix34& m, float t)
{
  // Short-cut blend
  if (t == 1.0f)
    set(m);
  else if (t != 0.0f)
  {
    // create quats from the matrix positions, slerp them
    Quat aRot = toQuat();
    Quat bRot = m.toQuat();
    Quat result;
    result.slerp(aRot, bRot, t, aRot.dot(bRot));

    // set new rotation from slerp'd quat
    fromQuat(result);

    // lerp the r[3]
    r[3].lerp(m.r[3], t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::interpolate6dof(
  const NMP::Matrix34& a, const NMP::Matrix34& b,
  float wtx, float wty, float wtz,
  float wrx, float wry, float wrz)
{
  // Short-cut blends
  float sum = wtx + wty + wtz + wrx + wry + wrz;
  if (sum == 0)
    set(a);
  else if (sum == 6.0f)
    set(b);
  else
  {
    // Get the rotation vector taking us between the TMs.
    // This takes a to b, which is inv(a)*b, but quats act as a premultiply
    // so it's b.toQuat() * a.toQuat().conjugate()
    Quat aQ = a.toQuat();
    Quat aQt = aQ;
    aQt.conjugate();
    Vector3 rv = (b.toQuat() * aQt).toRotationVector();
    Vector3 blendedr(rv.x * wrx, rv.y * wry, rv.z * wrz);
    // Now set the rotational component based on this result
    Quat q; q.fromRotationVector(blendedr);
    fromQuat(q * aQ);

    // Linear interpolate each translation component independently
    r[3].x = (1 - wtx) * a.r[3].x + wtx * b.r[3].x;
    r[3].y = (1 - wty) * a.r[3].y + wty * b.r[3].y;
    r[3].z = (1 - wtz) * a.r[3].z + wtz * b.r[3].z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::interpolate6dof(
  const NMP::Matrix34& m,
  float wtx, float wty, float wtz,
  float wrx, float wry, float wrz)
{
  // Short-cut blends
  float sum = wtx + wty + wtz + wrx + wry + wrz;
  if (sum == 6.0f)
    set(m);
  else if (sum != 0.0f)
  {
    // Get the rotation vector taking us between the TMs.
    // This takes a to b, which is inv(a)*b, but quats act as a premultiply
    // so it's m.toQuat() * a.toQuat().conjugate()
    Quat aQ = toQuat();
    Quat qt = aQ;
    qt.conjugate();
    Vector3 rv = (m.toQuat() * qt).toRotationVector();
    Vector3 blendedr(rv.x * wrx, rv.y * wry, rv.z * wrz);
    // Now set the rotational component based on this result
    Quat q; q.fromRotationVector(blendedr);
    fromQuat(q * aQ);

    // Linear interpolate each translation component independently
    r[3].x = (1 - wtx) * r[3].x + wtx * m.r[3].x;
    r[3].y = (1 - wty) * r[3].y + wty * m.r[3].y;
    r[3].z = (1 - wtz) * r[3].z + wtz * m.r[3].z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This method is a copy and paste of some older code from NMutils
// (NMMatrix3::setOrientationFromGeneralEulers + OEul_ToNMNMMatrix3 +
// TLC to remove the unneeded bits of code).
// The original method comes from Ken Shoemake and is described in
// Graphics Gems IV, Paul Heckbert (editor), Academic Press, 1994.
NM_INLINE void Matrix34::fromEulerXYZ(const NMP::Vector3& eulers)
{
  double ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
  int i = 0,
      j = 1,
      k = 2;
  ti = eulers.x;    tj = eulers.y;  th = eulers.z;
  ci = cos(ti); cj = cos(tj); ch = cos(th);
  si = sin(ti); sj = sin(tj); sh = sin(th);
  cc = ci * ch; cs = ci * sh; sc = si * ch; ss = si * sh;

  r[i][i] = (float)(cj * ch); r[i][j] = (float)(sj * sc - cs); r[i][k] = (float)(sj * cc + ss);
  r[j][i] = (float)(cj * sh); r[j][j] = (float)(sj * ss + cc); r[j][k] = (float)(sj * cs - sc);
  r[k][i] = (float)(-sj);    r[k][j] = (float)(cj * si);    r[k][k] = (float)(cj * ci);

  transpose3x3();
}

//----------------------------------------------------------------------------------------------------------------------
// This method is a copy and paste of some older code from NMutils
// (NMMatrix3::getGeneralEulersFromOrientation + OEul_FromNMNMMatrix3 +
// TLC to remove the unneeded bits of code).
// The original method comes from Ken Shoemake and is described in
// Graphics Gems IV, Paul Heckbert (editor), Academic Press, 1994.
NM_INLINE Vector3 Matrix34::toEulerXYZ() const
{
  Matrix34 matT(*this);
  matT.transpose3x3();

  Vector3 ea;
  int i = 0;
  int j = 1;
  int k = 2;

  double cy = sqrt(matT(i, i) * matT(i, i) + matT(j, i) * matT(j, i));

  if (cy > 16 * FLT_EPSILON)
  {
    ea.x = atan2((float)matT(k, j), (float)matT(k, k));
    ea.y = atan2((float) - matT(k, i), (float)cy);
    ea.z = atan2((float)matT(j, i), (float)matT(i, i));
  }
  else
  {
    ea.x = atan2((float) - matT(j, k), (float)matT(j, j));
    ea.y = atan2((float) - matT(k, i), (float)cy);
    ea.z = 0;
  }

  return ea;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::fromQuat(const Quat& quat)
{
  const float q0t2 = quat.w * 2;
  const float q1t2 = quat.x * 2;

  const float q0sq = quat.w * quat.w;
  const float q1sq = quat.x * quat.x;
  const float q2sq = quat.y * quat.y;
  const float q3sq = quat.z * quat.z;

  const float q0q1 = q0t2 * quat.x;
  const float q0q2 = q0t2 * quat.y;
  const float q0q3 = q0t2 * quat.z;

  const float q1q2 = q1t2 * quat.y;
  const float q1q3 = q1t2 * quat.z;
  const float q2q3 = quat.y * quat.z * 2;

  r[0].set(q0sq + q1sq - q2sq - q3sq, q1q2 + q0q3, -q0q2 + q1q3);
  r[1].set(q1q2 - q0q3, q0sq - q1sq + q2sq - q3sq, q0q1 + q2q3);
  r[2].set(q0q2 + q1q3, -q0q1 + q2q3, q0sq - q1sq - q2sq + q3sq);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Matrix34::toQuat() const
{
  const float* const t = (float*)&r[0].x;

  float tr, s;

  tr = 1.0f + t[0] + t[5] + t[10];

  NMP::Quat quat;

  // check the diagonals.
  if (tr > 1.0f)
  {
    s = sqrtf (tr) * 2.0f;

    quat.w = s * 0.25f;

    quat.x = (t[6] - t[9]) / s;
    quat.y = (t[8] - t[2]) / s;
    quat.z = (t[1] - t[4]) / s;
  }
  else
  {
    if ((t [0] > t [5]) && (t [0] > t [10]))
    { // Column 0:
      s  = sqrtf (1.0f + t [0] - t [5] - t [10]) * 2.0f;
      quat.x = 0.25f * s;
      quat.y = (t [1] + t [4]) / s;
      quat.z = (t [8] + t [2]) / s;
      quat.w = (t [6] - t [9]) / s;
    }
    else if (t [5] > t [10])
    {             // Column 1: 
      s  = sqrtf (1.0f + t [5] - t [0] - t [10]) * 2.0f;
      quat.x = (t [1] + t[4]) / s;
      quat.y = 0.25f * s;
      quat.z = (t[6] + t[9]) / s;
      quat.w = (t[8] - t[2]) / s;
    }
    else
    {                               // Column 2:
      s  = sqrtf (1.0f + t [10] - t [0] - t [5]) * 2.0f;
      quat.x = (t [8] + t [2]) / s;
      quat.y = (t [6] + t [9]) / s;
      quat.z = 0.25f * s;
      quat.w = (t [1] - t [4]) / s;
    }
  }

  return quat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::fromRotationVector(const NMP::Vector3& rot, bool tanQuarterAngle)
{
  fromQuat(NMP::Quat(rot, tanQuarterAngle));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Matrix34::toRotationVector(bool tanQuarterAngle) const
{
  return toQuat().toRotationVector(tanQuarterAngle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::setColumnMajorStride4(float* const M)
{
  // We are row major, so copy transposed
  // Skip last column
  r[0].set(M[0], M[4], M[8]);
  r[1].set(M[1], M[5], M[9]);
  r[2].set(M[2], M[6], M[10]);
  r[3].set(M[3], M[7], M[11]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::setRowMajorStride4(float* const M)
{
  // We are also row major, so copy directly
  // Skip last value on each row
  r[0].set(M[0], M[1], M[2]);
  r[1].set(M[4], M[5], M[6]);
  r[2].set(M[8], M[9], M[10]);
  r[3].set(M[12], M[13], M[14]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::getColumnMajorStride4(float* M) const
{
  // We are row major, so copy transpose
  M[0] = r[0].x; M[4] = r[0].y; M[8] = r[0].z; M[12] = 0;
  M[1] = r[1].x; M[5] = r[1].y; M[9] = r[1].z; M[13] = 0;
  M[2] = r[2].x; M[6] = r[2].y; M[10] = r[2].z; M[14] = 0;
  M[3] = r[3].x; M[7] = r[3].y; M[11] = r[3].z; M[15] = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Matrix34::getRowMajorStride4(float* M) const
{
  // We are also row major, so copy directly
  M[0] = r[0].x;  M[1] = r[0].y;  M[2] = r[0].z;  M[3] = 0;
  M[4] = r[1].x;  M[5] = r[1].y;  M[6] = r[1].z;  M[7] = 0;
  M[8] = r[2].x;  M[9] = r[2].y;  M[10] = r[2].z; M[11] = 0;
  M[12] = r[3].x; M[13] = r[3].y; M[14] = r[3].z; M[15] = 1.0f;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
