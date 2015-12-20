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
// inline included by NMQuat.h
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
//----------------------------------------------------------------------------------------------------------------------

// To allow quick testing without the use of our fast trig functions which could possibly introduce artifacts.
#define USE_FAST_TRIG_FUNCTIONS

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat()
{
  // doing stuff in default ctors can provoke hidden performance hits
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat(const Quat& copy)
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat(const Vector3& rotationVector, bool tanQuarterAngle)
{
  fromRotationVector(rotationVector, tanQuarterAngle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat(const Vector3& axis, float angle)
{
  fromAxisAngle(axis, angle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat(const Vector3& tanHalfAngleVec)
{
  fromTanHalfAngleRotationVector(tanHalfAngleVec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat(QuatConstruction NMP_UNUSED(qc))
{
  x = y = z = 0.0f;
  w = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat::Quat(float inX, float inY, float inZ, float inW)
{
  x = inX;
  y = inY;
  z = inZ;
  w = inW;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float& Quat::operator [] (int32_t index)
{
  NMP_ASSERT(index >= 0);
  NMP_ASSERT(index <= 3);
  return f[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::operator [] (int32_t index) const
{
  NMP_ASSERT(index >= 0);
  NMP_ASSERT(index <= 3);
  return f[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Quat::operator - () const
{
  Quat result(-x, -y, -z, -w);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Quat::operator ~ () const
{
  Quat result(-x, -y, -z, w);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Quat::operator + (const NMP::Quat& q) const
{
  Quat result(x + q.x, y + q.y, z + q.z, w + q.w);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Quat::operator - (const NMP::Quat& q) const
{
  Quat result(x - q.x, y - q.y, z - q.z, w - q.w);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Quat::operator * (const NMP::Quat& q) const
{
  Quat result(*this);
  result.multiply(q);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::operator += (const NMP::Quat& q)
{
  x += q.x;
  y += q.y;
  z += q.z;
  w += q.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::operator -= (const NMP::Quat& q)
{
  x -= q.x;
  y -= q.y;
  z -= q.z;
  w -= q.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::operator *= (const NMP::Quat& q)
{
  multiply(q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat    Quat::operator * (float scalar) const
{
  Quat result(x * scalar, y * scalar, z * scalar, w * scalar);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void   Quat::operator *= (float scalar)
{
  x *= scalar;
  y *= scalar;
  z *= scalar;
  w *= scalar;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Quat::operator == (const NMP::Quat& q) const
{
  return ((x == q.x) && (y == q.y) && (z == q.z) && (w == q.w));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Quat::operator != (const NMP::Quat& q) const
{
  return !(*this == q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const Quat& Quat::operator = (const NMP::Quat& copy)
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
NM_INLINE const __packed Quat& Quat::operator = (const NMP::Quat& copy) __packed
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const __packed Quat& Quat::operator = (const __packed NMP::Quat& copy) __packed
{
  x = copy.x;
  y = copy.y;
  z = copy.z;
  w = copy.w;
  return *this;
}
#endif // NM_COMPILER_GHS

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::identity()
{
  x = y = z = 0.0f;
  w = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::copyTo(NMP::Quat& q)
{
  q.x = x;
  q.y = y;
  q.z = z;
  q.w = w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::set(const NMP::Quat& q)
{
  x = q.x;
  y = q.y;
  z = q.z;
  w = q.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::setXYZW(float inX, float inY, float inZ, float inW)
{
  x = inX;
  y = inY;
  z = inZ;
  w = inW;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::setWXYZ(float inW, float inX, float inY, float inZ)
{
  x = inX;
  y = inY;
  z = inZ;
  w = inW;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::negate()
{
  x = -x;
  y = -y;
  z = -z;
  w = -w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::conjugate()
{
  x = -x;
  y = -y;
  z = -z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::angle() const
{
  float clampedW = NMP::clampValue(w, -1.0f, 1.0f);

#ifdef USE_FAST_TRIG_FUNCTIONS
  return 2.0f * fastArccos(clampedW);
#else
  return 2.0f * acosf(clampedW);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::angleTo(const NMP::Quat& toQuat) const
{
  // Compute the w component of the delta quat, and clamp to the valid range.
  float clampedW = NMP::clampValue(dot(toQuat), -1.0f, 1.0f);
  
#ifdef USE_FAST_TRIG_FUNCTIONS
  return 2.0f * fastArccos(clampedW);
#else
  return 2.0f * acosf(clampedW);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Quat::compare(const NMP::Quat& q, const float tolerance) const
{
  if (fabs(x - q.x) > tolerance)
    return false;

  if (fabs(y - q.y) > tolerance)
    return false;

  if (fabs(z - q.z) > tolerance)
    return false;

  if (fabs(w - q.w) > tolerance)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::dot(const NMP::Quat& q) const
{
  return (x * q.x + y * q.y + z * q.z + w * q.w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::normalise()
{
  float lengthSqr = magnitudeSquared();
  if (lengthSqr < FLT_EPSILON)
  {
    identity();
    return;
  }

  float invLength = 1.0f / sqrtf(lengthSqr);
  x *= invLength;
  y *= invLength;
  z *= invLength;
  w *= invLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat Quat::getNormalised() const
{
  Quat result(*this);
  result.normalise();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::normaliseGetLength()
{
  float lengthSqr = magnitudeSquared();
  if (lengthSqr < FLT_EPSILON)
  {
    identity();
    return 0.0f;
  }

  float invLength = 1.0f / sqrtf(lengthSqr);
  x *= invLength;
  y *= invLength;
  z *= invLength;
  w *= invLength;

  return invLength * lengthSqr;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fastNormalise()
{
  float lengthSqr = magnitudeSquared();
  if (lengthSqr < FLT_EPSILON)
  {
    identity();
    return;
  }

  float invLength = fastReciprocalSqrt(lengthSqr);
  x *= invLength;
  y *= invLength;
  z *= invLength;
  w *= invLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::normalise(const NMP::Quat& q)
{
  float lengthSqr = (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  if (lengthSqr < FLT_EPSILON)
  {
    identity();
    return;
  }

  float invLength = 1.0f / sqrtf(lengthSqr);
  x = q.x * invLength;
  y = q.y * invLength;
  z = q.z * invLength;
  w = q.w * invLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::normaliseGetLength(const NMP::Quat& q)
{
  float lengthSqr = (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  if (lengthSqr < FLT_EPSILON)
  {
    identity();
    return 0.0f;
  }

  float invLength = 1.0f / sqrtf(lengthSqr);
  x = q.x * invLength;
  y = q.y * invLength;
  z = q.z * invLength;
  w = q.w * invLength;

  return invLength * lengthSqr;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fastNormalise(const NMP::Quat& q)
{
  float lengthSqr = (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  if (lengthSqr < FLT_EPSILON)
  {
    identity();
    return;
  }

  float invLength = fastReciprocalSqrt(lengthSqr);
  x = q.x * invLength;
  y = q.y * invLength;
  z = q.z * invLength;
  w = q.w * invLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::magnitude() const
{
  return sqrtf(magnitudeSquared());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Quat::isNormal() const
{
  float magSq = magnitudeSquared();
  return NMP::nmfabs(magSq - 1.0f) < IS_NORMAL_ERROR_LIMIT;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Quat::isNormal(float tolerance) const
{
  float magSq = magnitudeSquared();
  return NMP::nmfabs(magSq - 1.0f) < tolerance;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Quat::magnitudeSquared() const
{
  return (x * x + y * y + z * z + w * w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::multiply(const NMP::Quat& q)
{
  float tempW = q.w * w - q.x * x - q.y * y - q.z * z;
  float tempX = q.w * x + q.x * w + y * q.z - z * q.y;
  float tempY = q.w * y + q.y * w + z * q.x - x * q.z;
  float tempZ = q.w * z + q.z * w + x * q.y - y * q.x;

  setWXYZ(tempW, tempX, tempY, tempZ);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::multiply(const NMP::Quat& a, const NMP::Quat& b)
{
  float tempW = b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z;
  float tempX = b.w * a.x + b.x * a.w + a.y * b.z - a.z * b.y;
  float tempY = b.w * a.y + b.y * a.w + a.z * b.x - a.x * b.z;
  float tempZ = b.w * a.z + b.z * a.w + a.x * b.y - a.y * b.x;

  setWXYZ(tempW, tempX, tempY, tempZ);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::rotateVector(const NMP::Vector3& v) const
{
  Vector3 result;
  Vector3 qv(x, y, z);

  result.cross(qv, v);
  result *= (w * 2.0f);

  result += (v * (2.0f * (w * w) - 1.0f));
  result += (qv * (qv.dot(v) * 2.0f));

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::getXAxis() const
{
  Vector3 result;
  float s = 2.0f * w;
  float x2 = 2.0f * x;
  result.x = x2 * x + s * w - 1.0f;
  result.y = x2 * y + s * z;
  result.z = x2 * z + s * -y;
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::getYAxis() const
{
  Vector3 result;
  float s = 2.0f * w;
  float y2 = 2.0f * y;
  result.x = y2 * x + s * -z;
  result.y = y2 * y + s * w - 1.0f;
  result.z = y2 * z + s * x;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::getZAxis() const
{
  Vector3 result;
  float s = 2.0f * w;
  float z2 = 2.0f * z;
  result.x = x * z2 + s * y;
  result.y = y * z2 + s * -x;
  result.z = z * z2 + s * w - 1.0f;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::rotateVector(const NMP::Vector3& vInput, NMP::Vector3& vOutput) const
{
  Vector3 result(NMP::Vector3::InitZero);
  Vector3 qv(x, y, z);

  result.cross(qv, vInput);
  result *= (w * 2.0f);

  result += (vInput * (2.0f * (w * w) - 1.0f));
  result += (qv * (qv.dot(vInput) * 2.0f));

  result.copyTo(vOutput);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::inverseRotateVector(const NMP::Vector3& v) const
{
  Vector3 result(NMP::Vector3::InitZero);
  Vector3 qv(x, y, z);

  result.cross(qv, v);
  result *= (-w * 2.0f);

  result += (v * (2.0f * (w * w) - 1.0f));
  result += (qv * (qv.dot(v) * 2.0f));

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::inverseRotateVector(const NMP::Vector3& vInput, NMP::Vector3& vOutput) const
{
  Vector3 result(NMP::Vector3::InitZero);
  Vector3 qv(x, y, z);

  result.cross(qv, vInput);
  result *= (-w * 2.0f);

  result += (vInput * (2.0f * (w * w) - 1.0f));
  result += (qv * (qv.dot(vInput) * 2.0f));

  result.copyTo(vOutput);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fromEulerXYZ(const NMP::Vector3& eulers)
{
  float chx = cosf(eulers.x / 2.0f);
  float chy = cosf(eulers.y / 2.0f);
  float chz = cosf(eulers.z / 2.0f);
  float shx = sinf(eulers.x / 2.0f);
  float shy = sinf(eulers.y / 2.0f);
  float shz = sinf(eulers.z / 2.0f);
  w = chx * chy * chz - shx * shy * shz;
  x = shy * shz * chx + chy * chz * shx;
  y = shy * chz * chx + chy * shz * shx;
  z = chy * shz * chx - shy * chz * shx;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::toEulerXYZ() const
{
  Vector3 result;

  // Code can handle non-unit quaternions
  float w2 = w * w;
  float x2 = x * x;
  float y2 = y * y;
  float z2 = z * z;
  float mag = w2 + x2 + y2 + z2;
  float xypluswz = x * y + z * w;
  const float singularityMargin = 0.99f;
  float singularityTest = 2.0f * (xypluswz) / (singularityMargin * mag);

  // Check for singularity straight up
  if (singularityTest > 1)
  {
    result.x = 0.0f;
    result.y = 2.0f * atan2f(x, w);
    result.z = NM_PI_OVER_TWO;
  }
  // Check for singularity straight down
  else if (singularityTest < -1)
  {
    result.x = 0.0f;
    result.y = -2.0f * atan2f(x, w);
    result.z = -NM_PI_OVER_TWO;
  }
  else
  {
    result.x = atan2f(2.0f * (x * w - y * z), w2 - x2 + y2 - z2);
    result.y = atan2f(2.0f * (y * w - x * z), w2 + x2 - y2 - z2);
    result.z = asinf(2.0f * xypluswz / mag);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fromAxisAngle(const Vector3& axis, float angle)
{
  const float half_a = angle * 0.5f;
  const float s = sinf(half_a);
  const float c = cosf(half_a);

  w = c;
  x = s * axis.x;
  y = s * axis.y;
  z = s * axis.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::toAxisAngle(Vector3& axis, float& angle) const
{
  // Check for a small rotation angle.
  Vector3 v(x, y, z);
  float sinHalfAngle = sqrtf(v.magnitudeSquared());
  if (sinHalfAngle < FLT_EPSILON)
  {
    angle = 0;
    axis.set(1, 0, 0); // Choose arbitrary axis
  }
  else
  {
    // Angle
    angle = 2 * atan2(sinHalfAngle, w);
    // Axis
    axis.setScaledVector(v, 1 / sinHalfAngle);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fromTanHalfAngleRotationVector(const Vector3& thaVec)
{
  *this = *(Quat*)(&thaVec);
  w = 1.0f;
  fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fromTanQuarterAngleRotationVector(const Vector3& thaVec)
{
  float mag2 = thaVec.magnitudeSquared();
  float opm2 = 1 + mag2;
  float fac = 2 / opm2;
  w = (1 - mag2) / opm2;
  x = thaVec.x * fac;
  y = thaVec.y * fac;
  z = thaVec.z * fac;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fromRotationVector(const NMP::Vector3& r)
{
  // Compute the rotation angle theta from the
  // magnitude of the vector v
  float theta = r.magnitude();
  if (theta < FLT_EPSILON)
  {
    identity();
  }
  else
  {
    // Compute the sin and cosine of the half angle
    float halfAngle = 0.5f * theta;
    float fac = sinf(halfAngle) / theta;
    w = cosf(halfAngle);

    // Set the vector components of the quat
    x = r.x * fac;
    y = r.y * fac;
    z = r.z * fac;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fromRotationVector(const NMP::Vector3& r, bool tanQuarterAngle)
{
  if (tanQuarterAngle)
  {
    fromTanQuarterAngleRotationVector(r);
  }
  else
  {
    fromRotationVector(r);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::toTanQuarterAngleRotationVector() const
{
  Vector3 v;

  // Check for a small angle. Also test w component since
  // numerical error might mean it is larger than one.
  float sinHalfAngle = sqrtf(x * x + y * y + z * z);
  if (sinHalfAngle < FLT_EPSILON || fabs(w) >= 1.0f)
  {
    v.setToZero();
  }
  else
  {
    // Ensure shortest path rotation (theta < 180) by selecting the quat that has
    // positive w component (i.e. q and -q represent the same rotation).
    // Note: the function f(w) = (1 - w) / (1 + w) is well behaved when w in 0 .. 1
    // However, this function quickly tends towards +infinity as w approaches -1.
    // Selecting the quat that has positive w component also avoids the singularity.
    float A;
    if (w >= 0.0f)
      A = sqrtf((1 - w) / (1 + w)); // w should not be > 1.0
    else
      A = -sqrtf((1 + w) / (1 - w));

    // Compute the rotation vector tan(theta/4) * a
    float fac = A / sinHalfAngle;
    v.set(fac * x, fac * y, fac * z);
  }

  return v;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::toRotationVector() const
{
  Vector3 v;

  // Check for a small rotation angle.
  float sinHalfAngle = sqrtf(x * x + y * y + z * z);
  if (sinHalfAngle < FLT_EPSILON)
  {
    v.setToZero();
  }
  else
  {
    // Ensure shortest path rotation (theta < 180) by selecting the quat that has
    // positive w component (i.e. q and -q represent the same rotation).
    float A;
    if (w >= 0.0f)
      A = 2 * atan2(sinHalfAngle, w);
    else
      A = -2 * atan2(sinHalfAngle, -w);

    // Compute the rotation vector theta * a
    float fac = A / sinHalfAngle;
    v.set(fac * x, fac * y, fac * z);
  }

  return v;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::toRotationVector(bool tanQuarterAngle) const
{
  if (tanQuarterAngle)
  {
    return toTanQuarterAngleRotationVector();
  }
  else
  {
    return toRotationVector();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::forRotation(const NMP::Vector3& v1, const NMP::Vector3& v2)
{
  NMP::Vector3 xyz;
  float dot = NMP::vDot(v1, v2);
  const float tol = FLT_EPSILON;

  float K = sqrtf(v1.magnitudeSquared() * v2.magnitudeSquared());

  dot = NMP::clampValue(dot, -K, K);

  // Test for zero-length input, to avoid infinite loop.  Return identity (not much else to do)
  if (K < tol)
    identity();
  // Test if v1 and v2 were antiparallel, to avoid singularity
  else if (fabs(dot + K) < tol * K)
  {
    NMP::Vector3 vMiddle;
    vMiddle.makeOrthogonal(v1);
    forRotation(vMiddle, v2);
    NMP::Quat q2;
    q2.forRotation(v1, vMiddle);
    multiply(q2);
  }
  else
  {
    // This means that xyz is Ka.sin(theta), where a is the unit axis of rotation,
    // which equals 2Ka.sin(theta/2)cos(theta/2).
    xyz.cross(v1, v2);

    // We then put 2Kcos^2(theta/2) = dot+K into the w part of the quaternion, and normalise.
    setWXYZ(dot + K, xyz.x, xyz.y, xyz.z);
    normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fastSlerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t, float fromDotTo)
{
  NMP_ASSERT(fromDotTo >= 0); // Quats must be in the same semi-arc - just negate one if they're not
  NMP_ASSERT(0.f <= t && t <= 1.f);

  float recipOnePlusFromdotTo;
  float c1, c3, c5, c7;
  float startWeight, endWeight;
  float T, t2, T2;

  recipOnePlusFromdotTo = 1.0f / (1.0f + fromDotTo);
  c1 =  1.570994357000f + (0.56429298590f + (-0.17836577170f + 0.043199493520f * fromDotTo) * fromDotTo) * fromDotTo;
  c3 = -0.646139638200f + (0.59456579360f + (0.08610323953f - 0.034651229280f * fromDotTo) * fromDotTo) * fromDotTo;
  c5 =  0.079498235210f + (-0.17304369310f + (0.10792795990f - 0.014393978010f * fromDotTo) * fromDotTo) * fromDotTo;
  c7 = -0.004354102836f + (0.01418962736f + (-0.01567189691f + 0.005848706227f * fromDotTo) * fromDotTo) * fromDotTo;

  T = 1 - t;
  t2 = t * t;
  T2 = T * T;
  startWeight = (c1 + (c3 + (c5 + c7 * T2) * T2) * T2) * T * recipOnePlusFromdotTo;
  endWeight   = (c1 + (c3 + (c5 + c7 * t2) * t2) * t2) * t * recipOnePlusFromdotTo;

  x = startWeight * qFrom.x + endWeight * qTo.x;
  y = startWeight * qFrom.y + endWeight * qTo.y;
  z = startWeight * qFrom.z + endWeight * qTo.z;
  w = startWeight * qFrom.w + endWeight * qTo.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fastSlerp(const NMP::Quat& qTo, float t, float fromDotTo)
{
  NMP_ASSERT(fromDotTo >= 0); // Quats must be in the same semi-arc - just negate one if they're not
  NMP_ASSERT(0.f <= t && t <= 1.f);

  float recipOnePlusFromdotTo;
  float c1, c3, c5, c7;
  float startWeight, endWeight;
  float T, t2, T2;

  recipOnePlusFromdotTo = 1.0f / (1.0f + fromDotTo);
  c1 =  1.570994357000f + (0.56429298590f + (-0.17836577170f + 0.043199493520f * fromDotTo) * fromDotTo) * fromDotTo;
  c3 = -0.646139638200f + (0.59456579360f + (0.08610323953f - 0.034651229280f * fromDotTo) * fromDotTo) * fromDotTo;
  c5 =  0.079498235210f + (-0.17304369310f + (0.10792795990f - 0.014393978010f * fromDotTo) * fromDotTo) * fromDotTo;
  c7 = -0.004354102836f + (0.01418962736f + (-0.01567189691f + 0.005848706227f * fromDotTo) * fromDotTo) * fromDotTo;

  T = 1 - t;
  t2 = t * t;
  T2 = T * T;
  startWeight = (c1 + (c3 + (c5 + c7 * T2) * T2) * T2) * T * recipOnePlusFromdotTo;
  endWeight   = (c1 + (c3 + (c5 + c7 * t2) * t2) * t2) * t * recipOnePlusFromdotTo;

  x = startWeight * x + endWeight * qTo.x;
  y = startWeight * y + endWeight * qTo.y;
  z = startWeight * z + endWeight * qTo.z;
  w = startWeight * w + endWeight * qTo.w;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::quickSlerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t)
{
  NMP_ASSERT(0.f <= t && t <= 1.f);

  float fromDotTo = qFrom.dot(qTo);
  if (fromDotTo >= 0)
  {
    fastSlerp(qFrom, qTo, t, fromDotTo);
  }
  else
  {
    float angle, s, startWeight, endWeight;

    if ((1.0f + fromDotTo) > QUATERNION_SLERP_LERP_ERROR_LIMIT)
    {
      // Source quaternions are different enough to perform a slerp
      angle = fastArccos(fromDotTo);
      s = fastSin(angle);
      startWeight = fastSin((1.0f - t) * angle) / s;
      endWeight = fastSin(t * angle) / s;
    }
    else
    {
      // Source quaternions are almost the same and slerping becomes inaccurate so do a lerp instead.
      startWeight = 1.0f - t;
      endWeight = t;
    }

    x = startWeight * qFrom.x + endWeight * qTo.x;
    y = startWeight * qFrom.y + endWeight * qTo.y;
    z = startWeight * qFrom.z + endWeight * qTo.z;
    w = startWeight * qFrom.w + endWeight * qTo.w;

    // Normalising is necessary because of numerical error and because the lerp path does not produce
    // normal results when sources are too different.
    fastNormalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::slerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t)
{
  NMP_ASSERT(0.f <= t && t <= 1.f);
  slerp(qFrom, qTo, t, NMP::Quat(qFrom).dot(qTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::slerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t, float fromDotTo)
{
  NMP_ASSERT(0.f <= t && t <= 1.f);

  float angle, s, startWeight, endWeight;

  // Correct for input quats in different semi-arcs.
  float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  Quat toQ = qTo * sgn;
  fromDotTo *= sgn;

  if ((1.0f - fabs(fromDotTo)) > QUATERNION_SLERP_LERP_ERROR_LIMIT)
  {
    // Source quaternions are different enough to perform a slerp.
    // Also fastArccos becomes inaccurate as c tends towards 1.
    angle = fastArccos(fromDotTo);
    s = fastSin(angle);
    startWeight = fastSin((1.0f - t) * angle) / s;
    endWeight = fastSin(t * angle) / s;
  }
  else
  {
    // Source quaternions are almost the same and slerping becomes inaccurate so do a lerp instead.
    startWeight = 1.0f - t;
    endWeight = t;
  }

  x = startWeight * qFrom.x + endWeight * toQ.x;
  y = startWeight * qFrom.y + endWeight * toQ.y;
  z = startWeight * qFrom.z + endWeight * toQ.z;
  w = startWeight * qFrom.w + endWeight * toQ.w;

  // Normalising is necessary as fastArccos is not accurate enough and because the lerp path does not produce
  // normal results when sources are too different.
  fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::slerp(const NMP::Quat& qTo, float t)
{
  NMP_ASSERT(0.f <= t && t <= 1.f);
  slerp(qTo, t, dot(qTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::slerp(const NMP::Quat& qTo, float t, float fromDotTo)
{
  NMP_ASSERT(0.f <= t && t <= 1.f);

  float angle, s, startWeight, endWeight;

  // Correct for input quats in different semi-arcs.
  float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  Quat toQ = qTo * sgn;
  fromDotTo *= sgn;

  if ((1.0f - fabs(fromDotTo)) > QUATERNION_SLERP_LERP_ERROR_LIMIT)
  {
    // Source quaternions are different enough to perform a slerp.
    // Also fastArccos becomes inaccurate as c tends towards 1.
    angle = fastArccos(fromDotTo);
    s = fastSin(angle);
    startWeight = fastSin((1.0f - t) * angle) / s;
    endWeight = fastSin(t * angle) / s;
  }
  else
  {
    // Source quaternions are almost the same and slerping becomes inaccurate so do a lerp instead.
    startWeight = 1.0f - t;
    endWeight = t;
  }

  x = startWeight * x + endWeight * toQ.x;
  y = startWeight * y + endWeight * toQ.y;
  z = startWeight * z + endWeight * toQ.z;
  w = startWeight * w + endWeight * toQ.w;

  // Normalising is necessary as fastArccos is not accurate enough and because the lerp path does not produce
  // normal results when sources are too different.
  fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::lerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t, float fromDotTo)
{
  // Correct for input quats in different semi-arcs.
  float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  Quat toQ = qTo * sgn;

  // Perform the interpolation
  float startWeight = 1.0f - t;
  float endWeight = t;

  x = startWeight * qFrom.x + endWeight * toQ.x;
  y = startWeight * qFrom.y + endWeight * toQ.y;
  z = startWeight * qFrom.z + endWeight * toQ.z;
  w = startWeight * qFrom.w + endWeight * toQ.w;

  // Result must always be normalised when lerping.
  fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::lerp(const NMP::Quat& qTo, float t, float fromDotTo)
{
  // Correct for input quats in different semi-arcs.
  float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  Quat toQ = qTo * sgn;

  // Perform the interpolation
  float startWeight = 1.0f - t;
  float endWeight = t;

  x = startWeight * x + endWeight * toQ.x;
  y = startWeight * y + endWeight * toQ.y;
  z = startWeight * z + endWeight * toQ.z;
  w = startWeight * w + endWeight * toQ.w;

  // Result must always be normalised when lerping.
  fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::exp(const NMP::Vector3& v)
{
  // Compute the half angle theta/2 from the
  // magnitude of the log vector v
  float halfAngle = sqrtf(v.magnitudeSquared());
  if (halfAngle < FLT_EPSILON)
  {
    identity();
  }
  else
  {
    // Compute the sin and cosine of the half angle
    float fac = sinf(halfAngle) / halfAngle;
    w = cosf(halfAngle);

    // Set the vector components of the quat
    x = fac * v.x;
    y = fac * v.y;
    z = fac * v.z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::fastExp(const NMP::Vector3& v)
{
  // Compute the half angle theta/2 from the
  // magnitude of the log vector v
  float halfAngle = fastSqrt(v.magnitudeSquared());
  if (halfAngle < FLT_EPSILON)
  {
    identity();
  }
  else
  {
    // Compute the sin and cosine of the half angle
    float fac;
    fastSinCos(halfAngle, fac, w);

    // Set the vector components of the quat
    fac /= halfAngle;
    x = fac * v.x;
    y = fac * v.y;
    z = fac * v.z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::log() const
{
  Vector3 v;

  // Check for a small rotation angle.
  float sinHalfAngle = sqrtf(x * x + y * y + z * z);
  if (sinHalfAngle < FLT_EPSILON)
  {
    v.setToZero();
  }
  else
  {
    // Compute the quat log vector theta/2 * a
    // Note: unlike toRotationVector() the log does not condition
    // the quat to be in the positive hemisphere, thus can encode
    // shortest or longest path rotation.
    float fac = atan2(sinHalfAngle, w) / sinHalfAngle;
    v.set(fac * x, fac * y, fac * z);
  }
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3 Quat::fastLog() const
{
  Vector3 v;

  // Check for a small rotation angle.
  float sinHalfAngle = fastSqrt(x * x + y * y + z * z);
  if (sinHalfAngle < FLT_EPSILON)
  {
    v.setToZero();
  }
  else
  {
    // Compute the quat log vector theta/2 * a
    // Note: unlike toRotationVector() the log does not condition
    // the quat to be in the positive hemisphere, thus can encode
    // shortest or longest path rotation.
    float fac = fastAtan2(sinHalfAngle, w) / sinHalfAngle;
    v.set(fac * x, fac * y, fac * z);
  }
  return v;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
