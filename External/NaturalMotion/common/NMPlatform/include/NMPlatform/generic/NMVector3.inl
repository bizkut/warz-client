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
#include "NMPlatform/NMMatrix34.h"
#include <float.h>
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

#define V3_OPR_VEC(op) \
  return Vector3(x op v.x, y op v.y, z op v.z);

#define V3_OPR_FLOAT(op) \
  return Vector3(x op f, y op f, z op f);

#define V3LOCAL_OPR_VEC(op) \
  x op v.x; y op v.y; z op v.z;

#define V3LOCAL_OPR_FLOAT(op) \
  x op f; y op f; z op f;

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3()
{
  // doing stuff in default ctors can provoke hidden performance hits
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(InitType init)
{
  switch (init)
  {
  case InitTypeZero: x = y = z = w = 0.0f; break;
  case InitTypeOne: x = y = z = w = 1.0f; break;
  case InitTypeOneX: x = 1.0f; y = z = w = 0.0f; break;
  case InitTypeOneY: y = 1.0f; z = x = w = 0.0f; break;
  case InitTypeOneZ: z = 1.0f; x = y = w = 0.0f; break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(InitZeroType)
{
  x = y = z = w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(InitOneType)
{
  x = y = z = w = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(InitOneXType)
{
  y = z = w = 0.0f;
  x = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(InitOneYType)
{
  x = z = w = 0.0f;
  y = 1.0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(InitOneZType)
{
  x = y = w = 0.0f;
  z = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(const Vector3& copy)
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(float inX, float inY)
{
  x = inX;
  y = inY;
  z = 0.0f;
  w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(float inX, float inY, float inZ)
{
  x = inX;
  y = inY;
  z = inZ;
  w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3::Vector3(float inX, float inY, float inZ, float inW)
{
  x = inX;
  y = inY;
  z = inZ;
  w = inW;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const Vector3& Vector3::operator = (const NMP::Vector3& copy)
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_GHS
// GHS doesn't auto-magically deal with the use of pramga pack(4) on a 16-byte aligned
//  struct, so we have to provide explicit assignement operators
NM_INLINE const __packed Vector3& Vector3::operator = (const NMP::Vector3& copy) __packed
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const __packed Vector3& Vector3::operator = (const __packed NMP::Vector3& copy) __packed
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
  return *this;
}
#endif // NM_COMPILER_GHS

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float& Vector3::operator [] (int32_t index)
{
  NMP_ASSERT(index >= 0);
  NMP_ASSERT(index <= 3);
  return f[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::operator [] (int32_t index) const
{
  NMP_ASSERT(index >= 0);
  NMP_ASSERT(index <= 3);
  return f[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::operator < (const NMP::Vector3& v) const
{
  return ((x < v.x) && (y < v.y) && (z < v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::operator > (const NMP::Vector3& v) const
{
  return ((x > v.x) && (y > v.y) && (z > v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::operator <= (const NMP::Vector3& v) const
{
  return ((x <= v.x) && (y <= v.y) && (z <= v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::operator >= (const NMP::Vector3& v) const
{
  return ((x >= v.x) && (y >= v.y) && (z >= v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::operator == (const NMP::Vector3& v) const
{
  return ((x == v.x) && (y == v.y) && (z == v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::operator != (const NMP::Vector3& v) const
{
  return ((x != v.x) || (y != v.y) || (z != v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  Vector3::operator - () const
{
  return Vector3(-x, -y, -z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  Vector3::operator + (const NMP::Vector3& v) const
{
  V3_OPR_VEC(+);
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  Vector3::operator - (const NMP::Vector3& v) const
{
  V3_OPR_VEC(-);
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  Vector3::operator * (float f) const
{
  V3_OPR_FLOAT(*);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  Vector3::operator * (const NMP::Vector3& v) const
{
  return Vector3(x * v.x, y * v.y, z * v.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  Vector3::operator / (float f) const
{
  float iF = (1.0f / f);
  return Vector3(x * iF, y * iF, z * iF);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void     Vector3::operator += (const NMP::Vector3& v)
{
  V3LOCAL_OPR_VEC(+=)
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void     Vector3::operator -= (const NMP::Vector3& v)
{
  V3LOCAL_OPR_VEC(-=)
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void     Vector3::operator *= (float f)
{
  V3LOCAL_OPR_FLOAT(*=)
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void     Vector3::operator *= (const NMP::Vector3& v)
{
  x *= v.x;
  y *= v.y;
  z *= v.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void     Vector3::operator /= (float f)
{
  float iF = (1.0f / f);
  x *= iF;
  y *= iF;
  z *= iF;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::copyTo(NMP::Vector3& v) const
{
  v.x = x;
  v.y = y;
  v.z = z;
  v.w = w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::setToZero()
{
  x = y = z = w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::set(float all)
{
  x = y = z = w = all;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::set(float inX, float inY, float inZ)
{
  x = inX;
  y = inY;
  z = inZ;
  w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::set(float inX, float inY, float inZ, float inW)
{
  x = inX;
  y = inY;
  z = inZ;
  w = inW;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::set(const NMP::Vector3& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::setScaledVector(const NMP::Vector3& v, float multiplier)
{
  x = v.x * multiplier;
  y = v.y * multiplier;
  z = v.z * multiplier;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::setScaledVector(const NMP::Vector3& v, const NMP::Vector3& multiplier)
{
  x = v.x * multiplier.x;
  y = v.y * multiplier.y;
  z = v.z * multiplier.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::multiplyElements(const NMP::Vector3& v)
{
  x *= v.x;
  y *= v.y;
  z *= v.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::divideElements(const NMP::Vector3& v)
{
  x /= v.x;
  y /= v.y;
  z /= v.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::compare(const NMP::Vector3& v, const float tolerance) const
{
  if (NMP::nmfabs(x - v.x) > tolerance)
    return false;

  if (NMP::nmfabs(y - v.y) > tolerance)
    return false;

  if (NMP::nmfabs(z - v.z) > tolerance)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::cross(const NMP::Vector3& a, const NMP::Vector3& b)
{
  float tempX = (a.y * b.z) - (a.z * b.y);
  float tempY = (a.z * b.x) - (a.x * b.z);
  float tempZ = (a.x * b.y) - (a.y * b.x);

  x = tempX;
  y = tempY;
  z = tempZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::cross(const NMP::Vector3& v)
{
  float tempX = (y * v.z) - (z * v.y);
  float tempY = (z * v.x) - (x * v.z);
  float tempZ = (x * v.y) - (y * v.x);

  x = tempX;
  y = tempY;
  z = tempZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::dot(const NMP::Vector3& v) const
{
  return (x * v.x + y * v.y + z * v.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Vector3::getComponentOrthogonalToDir(const NMP::Vector3& dir) const
{
  float dirLenSq = dir.magnitudeSquared();
  return dirLenSq > 0.0f ? (*this) - dir * (dot(dir) / dirLenSq) : *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::maximum(const NMP::Vector3& a, const NMP::Vector3& b)
{
  x = NMP::maximum(a.x, b.x);
  y = NMP::maximum(a.y, b.y);
  z = NMP::maximum(a.z, b.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::minimum(const NMP::Vector3& a, const NMP::Vector3& b)
{
  x = NMP::minimum(a.x, b.x);
  y = NMP::minimum(a.y, b.y);
  z = NMP::minimum(a.z, b.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::clampComponents(float minV, float maxV)
{
  x = NMP::clampValue(x, minV, maxV);
  y = NMP::clampValue(y, minV, maxV);
  z = NMP::clampValue(z, minV, maxV);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::clampMagnitude(float maxV)
{
  NMP_ASSERT(maxV >= 0.0f);
  float magSqr = magnitudeSquared();
  if (magSqr > maxV * maxV)
  {
    *this *= maxV / sqrtf(magSqr);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::abs()
{
  x = fabs(x);
  y = fabs(y);
  z = fabs(z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::summedAbs() const
{
  return (fabs(x) + fabs(y) + fabs(z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::summedAbsDiff(const NMP::Vector3& b)
{
  return fabs(x - b.x) + fabs(y - b.y) + fabs(z - b.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::maxAbs() const
{
  float absx = fabs(x);
  float absy = fabs(y);
  float absz = fabs(z);
  return absx < absy ? NMP::maximum(absy, absz) : NMP::maximum(absx, absz);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::magnitude() const
{
  return sqrtf(magnitudeSquared());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::magnitudeSquared() const
{
  return (x * x + y * y + z * z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::distanceTo(const NMP::Vector3& v) const
{
  float dX = (v.x - x);
  float dY = (v.y - y);
  float dZ = (v.z - z);
  return sqrtf((dX * dX) + (dY * dY) + (dZ * dZ));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::distanceSquaredTo(const NMP::Vector3& v) const
{
  float dX = (v.x - x);
  float dY = (v.y - y);
  float dZ = (v.z - z);
  return (dX * dX) + (dY * dY) + (dZ * dZ);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Vector3::getNormalised(InitType defaultValue) const
{
  NMP::Vector3 result(x, y, z, w);
  result.normalise(defaultValue);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::normaliseGetLength(InitType defaultValue)
{
  float length = magnitude();

  if (length == 0)
  {
    *this = Vector3(defaultValue);
    return 0.0f;
  }

  float invLength = 1.0f / length;
  x *= invLength;
  y *= invLength;
  z *= invLength;

  return length;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::normalise(InitType defaultValue)
{
  float length = magnitude();

  if (length < FLT_MIN)
  {
    *this = Vector3(defaultValue);
    return;
  }

  float invLength = 1.0f / length;
  x *= invLength;
  y *= invLength;
  z *= invLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::normalise()
{
  float length = magnitude();

  if (length < FLT_MIN)
  {
    *this = Vector3(InitTypeOneX);
  }
  else
  {
    float invLength = 1.0f / length;
    x *= invLength;
    y *= invLength;
    z *= invLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------

NM_INLINE void Vector3::normaliseOrDef(const NMP::Vector3& defvec)
{
  float length = magnitude();

  if (length < FLT_MIN)
  {
    *this = defvec;
  }
  else
  {
    float invLength = 1.0f / length;
    x *= invLength;
    y *= invLength;
    z *= invLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Vector3::normaliseDep(const NMP::Vector3& v, InitType defaultValue)
{
  float length = v.magnitude();
  if (length < FLT_MIN)
  {
    *this = Vector3(defaultValue);
    return 0.0f;
  }

  float invLength = 1.0f / length;
  x = v.x * invLength;
  y = v.y * invLength;
  z = v.z * invLength;

  return length;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::fastNormalise(InitType defaultValue)
{
  float length2 = magnitudeSquared();

  if (length2 < FLT_MIN)
  {
    *this = Vector3(defaultValue);
    return;
  }

  float invLength = fastReciprocalSqrt(length2);
  x *= invLength;
  y *= invLength;
  z *= invLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::fastNormalise()
{
  float length2 = magnitudeSquared();

  if (length2 < FLT_MIN)
  {
    *this = Vector3(InitTypeOneX);
  }
  else
  {
    float invLength = fastReciprocalSqrt(length2);
    x *= invLength;
    y *= invLength;
    z *= invLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------

NM_INLINE void Vector3::fastNormaliseOrDef(const NMP::Vector3& defvec)
{
  float length2 = magnitudeSquared();

  if (length2 < FLT_MIN)
  {
    *this = defvec;
  }
  else
  {
    float invLength = fastReciprocalSqrt(length2);
    x *= invLength;
    y *= invLength;
    z *= invLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::makeOrthogonal(const NMP::Vector3& v)
{
  NMP::Vector3 vNorm(v);
  vNorm.normalise();

  if (vNorm.z < 0.5f && vNorm.z > -0.5f)
    set(-vNorm.y, vNorm.x, 0);
  else
    set(-vNorm.z, 0, vNorm.x);

  normalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::reciprocal()
{
  x = 1.0f / x;
  y = 1.0f / y;
  z = 1.0f / z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::lerp(const NMP::Vector3& vFrom, const NMP::Vector3& vTo, float t)
{
  x = vFrom.x + t * (vTo.x - vFrom.x);
  y = vFrom.y + t * (vTo.y - vFrom.y);
  z = vFrom.z + t * (vTo.z - vFrom.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3::lerp(const NMP::Vector3& vTo, float t)
{
  x = x + t * (vTo.x - x);
  y = y + t * (vTo.y - y);
  z = z + t * (vTo.z - z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3  operator*(float f, const NMP::Vector3& b)
{
  return Vector3(f * b.x, f * b.y, f * b.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float vDot(const NMP::Vector3& a, const NMP::Vector3& b)
{
  return ((Vector3)a).dot(b);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float vDistanceBetween(const NMP::Vector3& vFrom, const NMP::Vector3& vTo)
{
  return ((Vector3)vFrom).distanceTo(vTo);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 vNormalise(const NMP::Vector3& a)
{
  Vector3 result(a);
  result.normalise();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 vCross(const NMP::Vector3& a, const NMP::Vector3& b)
{
  Vector3 result;
  result.cross(a, b);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 vMin(const NMP::Vector3& a, const NMP::Vector3& b)
{
  Vector3 result;
  result.minimum(a, b);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 vMax(const NMP::Vector3& a, const NMP::Vector3& b)
{
  Vector3 result;
  result.maximum(a, b);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 vLerp(const NMP::Vector3& vFrom, const NMP::Vector3& vTo, float t)
{
  Vector3 result;
  result.lerp(vFrom, vTo, t);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Vector3XAxis() { return Vector3(1.0f, 0.0f, 0.0f); }
NM_INLINE Vector3 Vector3YAxis() { return Vector3(0.0f, 1.0f, 0.0f); }
NM_INLINE Vector3 Vector3ZAxis() { return Vector3(0.0f, 0.0f, 1.0f); }
NM_INLINE Vector3 Vector3Zero()  { return Vector3(0.0f, 0.0f, 0.0f); }
NM_INLINE Vector3 Vector3One()   { return Vector3(1.0f, 1.0f, 1.0f); }
NM_INLINE Vector3 Vector3Half()  { return Vector3(0.5f, 0.5f, 0.5f); }


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::isNormal() const
{
  float magSq = magnitudeSquared();
  return NMP::nmfabs(magSq - 1.0f) < IS_NORMAL_ERROR_LIMIT;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Vector3::isNormal(float tolerance) const
{
  float magSq = magnitudeSquared();
  return NMP::nmfabs(magSq - 1.0f) < tolerance;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
