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

// Compared against the magnitude of a quaternion to check if it is actually normalised.
#define IS_NORMAL_ERROR_LIMIT 0.000002f

// Determines the point below which slerping 2 quaternions becomes inaccurate because they are too similar,
// and the point at which the fastArccos becomes significantly inaccurate.
// (compared against the dot product of the two vectors to lerp).
// This value needs to be small in order to avoid the necessity for a normalise function in our slerp functions,
// however too small and we get closer to a divide by zero in the slerp.
#define QUATERNION_SLERP_LERP_ERROR_LIMIT 0.05f

// Determines the point above which lerping 2 quaternions is not likely to produce a reasonable result.
// (compared against the dot product of the two vectors to lerp).
#define QUATERNION_LERP_ERROR_LIMIT 0.1f

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPU
/// \class NMP::vpu::Quat
///
/// \brief
//----------------------------------------------------------------------------------------------------------------------

class Quat
{
public:

  union
  {
    vector4_t quat;

    struct
    {
      float x, y, z, w;
    };

    float f[4];
  };

  NM_FORCEINLINE Quat() { };

  NM_FORCEINLINE Quat(const Quat& copy);
  NM_FORCEINLINE Quat(vector4_t vec);
  NM_FORCEINLINE Quat(const Vector3& rotationVector, bool tanQuarterAngle);
  NM_FORCEINLINE Quat(const Vector3& axis, float angle);
  NM_FORCEINLINE Quat(float inX, float inY, float inZ, float inW);

  NM_FORCEINLINE Quat   operator - () const;
  NM_FORCEINLINE Quat   operator ~ () const; // conjugate
  NM_FORCEINLINE Quat   operator + (const Quat& q) const;
  NM_FORCEINLINE Quat   operator - (const Quat& q) const;
  NM_FORCEINLINE Quat   operator * (const Quat& q) const;
  NM_FORCEINLINE void   operator += (const Quat& q);
  NM_FORCEINLINE void   operator -= (const Quat& q);
  NM_FORCEINLINE void   operator *= (const Quat& q);
  NM_FORCEINLINE bool   operator == (const Quat& q) const;
  NM_FORCEINLINE bool   operator != (const Quat& q) const;

  NM_FORCEINLINE Quat   operator * (float scalar) const;
  NM_FORCEINLINE void   operator *= (float scalar);

  NM_FORCEINLINE const Quat& operator = (const Quat& copy);

  /// \ingroup Maths
  /// \brief Set to the identity, xyz being 0.0 and w being 1.0
  NM_FORCEINLINE void identity();

  /// \ingroup Maths
  /// \brief Copy the local values to the quaternion specified
  NM_FORCEINLINE void copyTo(Quat& q);

  /// \ingroup Maths
  /// \brief Copy the input quaternion into the local quaternion
  NM_FORCEINLINE void set(const Quat& q);

  /// \ingroup Maths
  /// \brief Set values directly, in the order X Y Z W
  NM_FORCEINLINE void setXYZW(float inX, float inY, float inZ, float inW);

  /// \ingroup Maths
  /// \brief Set values directly, in the order W X Y Z
  NM_FORCEINLINE void setWXYZ(float inW, float inX, float inY, float inZ);

  /// \ingroup Maths
  /// \brief Sets quaternion to -x, -y, -z, -w
  NM_FORCEINLINE void negate();

  /// \ingroup Maths
  /// \brief Invert the XYZ component of this quaternion, producing the opposite
  /// rotation
  NM_FORCEINLINE void conjugate();

  /// \ingroup Maths
  /// \brief Calculate the quaternions angle of rotation
  NM_FORCEINLINE float angle() const;

  /// \ingroup Maths
  /// \brief Compute the angle of rotation between this and toQuat (this is the distance between both quaternions)
  NM_FORCEINLINE float angleTo(const Quat& toQuat) const;

  /// \ingroup Maths
  /// \brief Compare this quaternion to the input, with a given floating point tolerance value;
  ///        returns true if the comparison passes.
  NM_FORCEINLINE bool compare(const Quat& q, const float tolerance) const;

  /// \ingroup Maths
  /// \brief compare two quaternions, with a given floating point tolerance value;
  ///        returns true if the comparison passes.
  static bool compare(const Quat& a, const Quat& b, const float tolerance);

  /// \ingroup Maths
  /// \brief 4 element dot product of this quaternion with input quaternion.
  NM_FORCEINLINE float dot(const Quat& q) const;

  /// \ingroup Maths
  /// \brief Normalize this quaternion, returning its original magnitude
  NM_FORCEINLINE void normalise();

  /// \ingroup Maths
  /// \brief Normalize the input quaternion, storing the results locally and returning the magnitude of q
  NM_FORCEINLINE void normalise(const Quat& q);

  /// \ingroup Maths
  /// \brief Normalize this quaternion, returning its original magnitude (uses fast square root function).
  NM_FORCEINLINE void fastNormalise();

  /// \ingroup Maths
  /// \brief Normalize the input quaternion, storing the results locally and returning the magnitude of q (uses fast square root function).
  NM_FORCEINLINE void fastNormalise(const Quat& q);

  /// \ingroup Maths
  /// \brief Return the squared magnitude of the quaternion
  NM_FORCEINLINE float magnitudeSquared() const;

  /// \ingroup Maths
  /// \brief Find the magnitude of the quaternion.
  NM_FORCEINLINE float magnitude() const;

  /// \ingroup Maths
  /// \brief Multiply this quaternion with the input quaternion, result stored in local values
  NM_FORCEINLINE void multiply(const Quat& q);

  /// \ingroup Maths
  /// \brief Multiply the two input quaternions, result stored in local values
  NM_FORCEINLINE void multiply(const Quat& a, const Quat& b);

  /// \ingroup Maths
  /// \brief Rotate the Vector3 specified by this quaternion
  NM_FORCEINLINE Vector3 rotateVector(const Vector3& v) const;

  /// \ingroup Maths
  /// \brief get the axis of the orientation
  NM_FORCEINLINE Vector3 getXAxis() const;

  /// \ingroup Maths
  /// \brief get the axis of the orientation
  NM_FORCEINLINE Vector3 getYAxis() const;

  /// \ingroup Maths
  /// \brief get the axis of the orientation
  NM_FORCEINLINE Vector3 getZAxis() const;

  /// \ingroup Maths
  /// \brief Rotate the Vector3 specified by this quaternion, place results into output vector
  NM_FORCEINLINE void rotateVector(const Vector3& vInput, Vector3& vOutput) const;

  /// \ingroup Maths
  /// \brief Inverse rotate the Vector3 specified by this quaternion
  NM_FORCEINLINE Vector3 inverseRotateVector(const Vector3& v) const;

  /// \ingroup Maths
  /// \brief Inverse rotate the Vector3 specified by this quaternion, place results into output vector
  NM_FORCEINLINE void inverseRotateVector(const Vector3& vInput, Vector3& vOutput) const;

  /// \ingroup Maths
  /// \brief Construct the quaternion from the euler rotation angles provided, applied in order X, Y, Z.
  NM_FORCEINLINE void fromEulerXYZ(const Vector3& eulers);

  /// \ingroup Maths
  /// \brief Convert the quaternion to a set of euler rotation angles, rotation order X, Y, Z.
  NM_FORCEINLINE Vector3 toEulerXYZ() const;

  /// \ingroup Maths
  /// \brief Set the quaternion from an axis and angle.
  NM_FORCEINLINE void fromAxisAngle(const Vector3& axis, float angle);

  /// \ingroup Maths
  /// \brief Convert the quaternion from a rotation vector
  NM_FORCEINLINE void fromRotationVector(const Vector3& r, bool tanQuarterAngle = false);

  /// \ingroup Maths
  /// \brief Convert the quaternion to a rotation vector
  NM_FORCEINLINE Vector3 toRotationVector(bool tanQuarterAngle = false) const;
  NM_FORCEINLINE Vector3 toRotationVectorFPU(bool tanQuarterAngle = false) const;

  /// \ingroup Maths
  /// \brief Calculates quaternion required to rotate v1 into v2; v1 and v2 need not be normalised
  NM_INLINE void forRotation(const Vector3& v1, const Vector3& v2);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If qFrom and qTo are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_FORCEINLINE void slerp(const Quat& qFrom, const Quat& qTo, float t);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If qFrom and qTo are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_FORCEINLINE void slerp(const Quat& qFrom, const Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If both input quaternions are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_FORCEINLINE void slerp(const Quat& qTo, float t);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If both input quaternions are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_FORCEINLINE void slerp(const Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Fast spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If qFrom and qTo are normalised then the result will be normalised (within error limits).
  ///       This is a fast alternative to the slerp function.
  NM_FORCEINLINE void fastSlerp(const Quat& qFrom, const Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Fast spherical linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If both input quaternions are normalised then the result will be normalised (within error limits).
  ///        This is a fast alternative to the slerp function.
  NM_FORCEINLINE void fastSlerp(const Quat& qTo, float t, float fromDotTo);  

  /// \ingroup Maths
  /// \brief Calculate linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        To produce a sensible result both input vectors should be normalised and similar.
  ///        The result is normalised.
  ///        This is potentially a faster alternative to slerping closely spaced quaternions.
  NM_FORCEINLINE void lerp(const Quat& qFrom, const Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Calculate linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        To produce a sensible result both input vectors should be normalised and similar.
  ///        The result is normalised.
  ///        This is potentially a faster alternative to slerping closely spaced quaternions.
  NM_FORCEINLINE void lerp(const Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Is this quaternion normalised (within error limits).
  NM_FORCEINLINE bool isNormal() const;

  /// \ingroup Maths
  /// \brief Set the quaternion from a quaternion log vector
  NM_FORCEINLINE void exp(const Vector3& v);

  /// \ingroup Maths
  /// \brief Compute an approximation to the quaternion exponential map by using
  ///        a polynomial function to compute the cos and sin trigonometric functions.
  ///        The error in the sin and cos functions is distributed over the entire [0:2*pi]
  ///        range and has a maximum error of the order 2e-4.
  NM_FORCEINLINE void fastExp(const Vector3& v);

  /// \ingroup Maths
  /// \brief Convert the quaternion to a quaternion log vector
  NM_FORCEINLINE Vector3 log() const;

  /// \ingroup Maths
  /// \brief Computes the quaternion log vector w = (theta/2) * v by using a
  ///        polynomial function to approximate the atan trigonometric function. The error
  ///        in the atan trigonometric function is distributed over the entire [0:pi] range
  ///        and has a maximum error of the order 2e-4.
  NM_FORCEINLINE Vector3 fastLog() const;

};

//-----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat::Quat(const Quat& copy)
{
  quat = copy.quat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat::Quat(vector4_t vec)
{
  quat = vec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat::Quat(const Vector3& rotationVector, bool tanQuarterAngle)
{
  fromRotationVector(rotationVector, tanQuarterAngle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat::Quat(const Vector3& axis, float angle)
{
  fromAxisAngle(axis, angle);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat::Quat(float inX, float inY, float inZ, float inW)
{
  quat = set4f(inX, inY, inZ, inW);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat Quat::operator - () const
{
  return Quat(neg4f(quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat Quat::operator ~ () const
{
  return Quat(setwf(neg4f(quat), quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat Quat::operator + (const Quat& q) const
{
  return Quat(add4f(quat, q.quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat Quat::operator - (const Quat& q) const
{
  return Quat(sub4f(quat, q.quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat Quat::operator * (const Quat& q) const
{
  return Quat(qqMul(quat, q.quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::operator += (const Quat& q)
{
  quat = add4f(quat, q.quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::operator -= (const Quat& q)
{
  quat = sub4f(quat, q.quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::operator *= (const Quat& q)
{
  quat = qqMul(quat, q.quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Quat Quat::operator * (float scalar) const
{
  return Quat(mul4f(quat, set4f(scalar)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::operator *= (float scalar)
{
  quat = mul4f(quat, set4f(scalar));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Quat::operator == (const Quat& q) const
{
  return ((x == q.x) && (y == q.y) && (z == q.z) && (w == q.w));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Quat::operator != (const Quat& q) const
{
  return !(*this == q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE const Quat& Quat::operator = (const Quat& copy)
{
  quat = copy.quat;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::identity()
{
  quat = quat4f();
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::copyTo(Quat& q)
{
  q.quat = quat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::set(const Quat& q)
{
  quat = q.quat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::setXYZW(float inX, float inY, float inZ, float inW)
{
  quat = set4f(inX, inY, inZ, inW);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::setWXYZ(float inW, float inX, float inY, float inZ)
{
  quat = set4f(inX, inY, inZ, inW);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::negate()
{
  quat = neg4f(quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::conjugate()
{
  quat = setwf(neg4f(quat), quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Quat::angle() const
{
  // Clamp w between -1 and 1
  vector4_t quatw = splatW(quat);
  quatw = sel4cmpLT(quatw, negOne4f(), negOne4f(), quatw);
  quatw = sel4cmpLT(quatw, one4f(), quatw, one4f());

  // Compute the angle from the w component
  vector4_t halfAngle = acos4f(quatw);
  return floatX(add4f(halfAngle, halfAngle));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Quat::angleTo(const Quat& toQuat) const
{
  // Compute the w component of the delta quat
  vector4_t quatw = dot4f(quat, toQuat.quat);

  // Clamp w between -1 and 1
  quatw = sel4cmpLT(quatw, negOne4f(), negOne4f(), quatw);
  quatw = sel4cmpLT(quatw, one4f(), quatw, one4f());

  // Compute the angle from the w component
  vector4_t halfAngle = acos4f(quatw);
  return floatX(add4f(halfAngle, halfAngle));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Quat::compare(const Quat& q, const float tolerance) const
{
  // Equivalent to
  //   if (fabs(x - q.x) >= tolerance)
  //     return false;
  //   if (fabs(y - q.y) >= tolerance)
  //     return false;
  //   if (fabs(z - q.z) >= tolerance)
  //     return false;
  //   if (fabs(w - q.w) >= tolerance)
  //     return false;
  //   return true;

  return 0 != elemX(isBound4f(abs4f(sub4f(quat, q.quat)), set4f(tolerance)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Quat::dot(const Quat& q) const
{
  return floatX(dot4f(quat, q.quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::normalise()
{
  normalise(*this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fastNormalise()
{
  fastNormalise(*this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::normalise(const Quat& q)
{
  vector4_t id = quat4f();
  vector4_t lenSqr = dot4f(q.quat, q.quat);
  quat = sel4cmpLT(lenSqr, fltEpsilon4f(), id, mul4f(q.quat, rsqrt4f(lenSqr)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fastNormalise(const Quat& q)
{
  vector4_t id = quat4f();
  vector4_t lenSqr = dot4f(q.quat, q.quat);
  quat = sel4cmpLT(lenSqr, fltEpsilon4f(), id, mul4f(q.quat, rsqrt4f(lenSqr)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Quat::magnitude() const
{
  return floatX(sqrt4f(dot4f(quat, quat)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Quat::isNormal() const
{
  // nmfabs(magnitudeSquared() - 1.0f) < IS_NORMAL_ERROR_LIMIT
  return 0 != elemX(mask4cmpLT(abs4f(sub4f(dot4f(quat, quat), one4f())), set4f(IS_NORMAL_ERROR_LIMIT)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Quat::magnitudeSquared() const
{
  return floatX(dot4f(quat, quat));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::multiply(const Quat& q)
{
  quat = qqMul(quat, q.quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::multiply(const Quat& a, const Quat& b)
{
  quat = qqMul(a.quat, b.quat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::rotateVector(const Vector3& v) const
{
  Vector3 result; result.vec = quatRotVec(quat, v.vec);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::getXAxis() const
{
  Vector3 result;

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float s = 2.0f * w;
  float x2 = 2.0f * x;

  result.x = x2 * x + s * w - 1.0f;
  result.y = x2 * y + s * z;
  result.z = x2 * z + s * -y;
#else
  vector4_t t = add4f(quat, quat);
  vector4_t us = setzf(one4f(), negOne4f());
  vector4_t un = setxf(zero4f(), negOne4f());

  vector4_t x2 = splatX(t);
  vector4_t s = mul4f(splatW(t), us);

  result.vec = madd4f(quat, x2, madd4f(reverse(quat), s, un));
  result.vec = setwf(result.vec, zero4f());
#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::getYAxis() const
{
  Vector3 result;

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float s = 2.0f * w;
  float y2 = 2.0f * y;

  result.x = y2 * x + s * -z;
  result.y = y2 * y + s * w - 1.0f;
  result.z = y2 * z + s * x;
#else
  vector4_t t = add4f(quat, quat);
  vector4_t us = setxf(one4f(), negOne4f());
  vector4_t un = setyf(zero4f(), negOne4f());

  vector4_t y2 = splatY(t);
  vector4_t s = mul4f(splatW(t), us);

  result.vec = madd4f(quat, y2, madd4f(swapLoHi(quat), s, un));
  result.vec = setwf(result.vec, zero4f());
#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::getZAxis() const
{
  Vector3 result;

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float s = 2.0f * w;
  float z2 = 2.0f * z;

  result.x = x * z2 + s * y;
  result.y = y * z2 + s * -x;
  result.z = z * z2 + s * w - 1.0f;
#else
  vector4_t t = add4f(quat, quat);
  vector4_t us = setyf(one4f(), negOne4f());
  vector4_t un = setzf(zero4f(), negOne4f());

  vector4_t z2 = splatZ(t);
  vector4_t s = mul4f(splatW(t), us);

  result.vec = madd4f(quat, z2, madd4f(swapYW(rotL4(quat)), s, un));
  result.vec = setwf(result.vec, zero4f());
#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::rotateVector(const Vector3& vInput, Vector3& vOutput) const
{
  vOutput.vec = quatRotVec(quat, vInput.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::inverseRotateVector(const Vector3& v) const
{
  Vector3 result; result.vec = quatInvRotVec(quat, v.vec);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::inverseRotateVector(const Vector3& vInput, Vector3& vOutput) const
{
  vOutput.vec = quatInvRotVec(quat, vInput.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fromEulerXYZ(const Vector3& eulers)
{
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

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

#else

  vector4_t eu2 = mul4f(eulers.vec, half4f());

  vector4_t vs, vc; sinCos4f(eu2, vs, vc);

  // x = shy*shz *chx + chy*chz *shx
  // y = shy*chz *chx + chy*shz *shx
  // z = chy*shz *chx - shy*chz *shx
  // w = chy*chz *chx - shy*shz *shx

  vector4_t vsub = mixLoHi(zero4f(), negOne4f());

  vector4_t shx = splatX(vs);
  vector4_t chx = splatX(vc);

  vector4_t scy = mixLoHi(splatY(vs), splatY(vc));
  vector4_t scz = mixLoHi(splatZ(vs), splatZ(vc));
  scz = swapYZ(scz);

  vector4_t m1  = mul4f(scy, scz);
  vector4_t m2s = mul4f(reverse(m1), shx);

  quat = add4f(mul4f(m1, chx), neg4f(m2s, vsub));

#endif
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::toEulerXYZ() const
{
  Vector3 result;

  // Code can handle non-unit quaternions

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

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
    result.y = 2.0f * fastAtan2(x, w);
    result.z = NM_PI_OVER_TWO;
  }
  // Check for singularity straight down
  else if (singularityTest < -1)
  {
    result.x = 0.0f;
    result.y = -2.0f * fastAtan2(x, w);
    result.z = -NM_PI_OVER_TWO;
  }
  else
  {
    result.x = fastAtan2(2.0f * (x * w - y * z), w2 - x2 + y2 - z2);
    result.y = fastAtan2(2.0f * (y * w - x * z), w2 + x2 - y2 - z2);
    result.z = fastArcsin(2.0f * xypluswz / mag);
  }

#else

  // set up
  vector4_t q2 = mul4f(quat, quat);
  vector4_t mag2 = dot4f(quat, quat);

  vector4_t xy = mul4f(quat, rotL4(quat));
  vector4_t zw = swapLoHi(xy);
  vector4_t xypluswz = splatX(add4f(xy, zw));

  vector4_t singm = set4f(0.99f);
  vector4_t singt = mul4f(add4f(xypluswz, xypluswz), rcp4f(mag2));
  vector4_t asingt = abs4f(singt);

  // do 4 atan2fs together, as
  // atan2f(2.0f * (x * w - y * z), w2 - x2 + y2 - z2)
  // atan2f(2.0f * (y * w - x * z), w2 + x2 - y2 - z2)
  // atan2f(x, w)
  // atan2f(1, 1)  ( note atan(1) = pi/4 trick for singularity result )

  vector4_t np = sub4f(mul4f(quat, splatW(quat)), mul4f(swapXY(quat), splatZ(quat)));
  vector4_t n = mixLoHi(add4f(np, np), splatX(quat));
  vector4_t dp = sub4f(add4f(splatW(q2), swapXY(q2)), add4f(splatZ(q2), q2));
  vector4_t d = mixLoHi(dp, splatW(quat));

  //n = setwf(n, one4f()); d = setwf(d, one4f());
  vector4_t at = atan24f(n, d);
  // or (as 'trick' not as accurate) just
  at = setwf(at, set4f(0.7853981634f));

  // singularity result, for when singularityTest > 1 or singularityTest < -1
  vector4_t svp = rotL4(mixLoHi(zero4f(), at));
  vector4_t sv = neg4f(add4f(svp, svp), xypluswz);

  // standard result
  vector4_t sz = asin4f(singt);
  vector4_t nsv = setwf(mixLoHi(at, sz), zero4f());

  // select
  result.vec = sel4cmpLT(singm, asingt, sv, nsv);

#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fromAxisAngle(const Vector3& axis, float angle)
{
  vector4_t ha = mul4f(half4f(), set4f(angle));

  vector4_t vs, vc; fastSinCos4f(ha, vs, vc);

  quat = setwf(mul4f(axis.vec, vs), vc);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fromRotationVector(const Vector3& r, bool tanQuarterAngle)
{
  if (tanQuarterAngle)
  {
    quat = convertTQARV2Quat(r.vec);
  }
  else
  {
    vector4_t mag2 = dot3f(r.vec, r.vec);

    if (floatX(mag2) < FLT_EPSILON * FLT_EPSILON)
    {
      identity();
    }
    else
    {
      vector4_t mag = sqrt4f(mag2);
      vector4_t phi = mul4f(half4f(), mag);

      vector4_t vs, vc; sinCos4f(phi, vs, vc);

      vs = mul4f(vs, rcp4f(mag));

      quat = setwf(mul4f(r.vec, vs), vc);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::toRotationVector(bool tanQuarterAngle) const
{
  Vector3 result;

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

  float mag = fastSqrt(x * x + y * y + z * z);

  // Check for a small angle. Also test w component since
  // numerical error might mean it is larger than one.
  if (mag < FLT_EPSILON || fabs(w) >= 1.0f)
  {
    result.x = 0;
    result.y = 0;
    result.z = 0;
  }
  else
  {
    float A, fac;

    if (tanQuarterAngle)
    {
      // w must be conditioned to avoid negative square root
      if (w >= 0.0f)
        A = fastSqrt((1 - w) / (1 + w));
      else
        A = -fastSqrt((1 + w) / (1 - w));
    }
    else
    {
      if (w >= 0.0f)
        A = 2 * fastAtan2(mag, w);
      else
        A = -2 * fastAtan2(mag, -w);
    }

    fac = A / mag;
    result.x = fac * x;
    result.y = fac * y;
    result.z = fac * z;
  }
#else

  vector4_t mag = sqrt4f(dot3f(quat, quat));

  // Check for a small angle. Also test w component since
  // numerical error might mean it is larger than one.
  if (floatX(mag) < FLT_EPSILON || fabs(w) >= 1.0f)
  {
    result.vec = zero4f();
  }
  else
  {
    vector4_t A;

    vector4_t wsign = set4f(w);
    vector4_t aw = abs4f(wsign);

    if (tanQuarterAngle)
    {
      // A = sqrtf((1-aw) / (1+aw));
      vector4_t vn = sub4f(one4f(), aw);
      vector4_t vd = add4f(one4f(), aw);
      A = sqrt4f(mul4f(vn, rcp4f(vd)));
    }
    else
    {
      // A = 2 * atan2(mag, aw);
      A = atan24f(mag, aw);
      A = add4f(A, A);
    }

    vector4_t fac = mul4f(A, rcp4f(mag));
    result.vec = mul4f(quat, neg4f(fac, wsign)); // w?
  }

#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Quat::forRotation(const Vector3& v1, const Vector3& v2)
{
  Vector3 xyz;
  float dot = v1.dot(v2);
  const float tol = FLT_EPSILON;
  float K2 = v1.magnitudeSquared() * v2.magnitudeSquared();

  // Test for zero-length input, to avoid infinite loop.  Return identity (not much else to do)
  if (K2 < tol * tol)
    identity();
  // Test if v1 and v2 were antiparallel, to avoid singularity
  else if (fabs(dot * dot - K2) < tol * tol)
  {
    Vector3 vMiddle;
    vMiddle.makeOrthogonal(v1);
    forRotation(vMiddle, v2);
    Quat q2;
    q2.forRotation(v1, vMiddle);
    multiply(q2);
  }
  else
  {
    float K = fastSqrt(K2);

    // This means that xyz is Ka.sin(theta), where a is the unit axis of rotation,
    // which equals 2Ka.sin(theta/2)cos(theta/2).
    xyz.cross(v1, v2);

    // We then put 2Kcos^2(theta/2) = dot+K into the w part of the quaternion, and normalise.
    setWXYZ(dot + K, xyz.x, xyz.y, xyz.z);
    fastNormalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fastSlerp(const Quat& qFrom, const Quat& qTo, float t, float fromDotTo)
{
  NMP_ASSERT(fromDotTo >= 0);
  // Quats must be in the same semi-arc - just negate one if they're not

  quat = fastSlerp2(qFrom.quat, qTo.quat, set4f(t), set4f(fromDotTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fastSlerp(const Quat& qTo, float t, float fromDotTo)
{
  NMP_ASSERT(fromDotTo >= 0); // Quats must be in the same semi-arc - just negate one if they're not

  quat = fastSlerp2(quat, qTo.quat, set4f(t), set4f(fromDotTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::slerp(const Quat& qFrom, const Quat& qTo, float t)
{
  *this = qFrom;
  slerp(qTo, t, Quat(qFrom).dot(qTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::slerp(const Quat& qFrom, const Quat& qTo, float t, float fromDotTo)
{
  *this = qFrom;
  slerp(qTo, t, fromDotTo);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::slerp(const Quat& qTo, float t)
{
  slerp(qTo, t, dot(qTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::slerp(const Quat& qTo, float t, float fromDotTo)
{
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

  float angle, s, startWeight, endWeight;

  // Correct for input quats in different semi-arcs.
  Quat toQ = qTo;
  if (fromDotTo < 0)
  {
    toQ.setWXYZ(-qTo.w, -qTo.x, -qTo.y, -qTo.z);
    fromDotTo = -fromDotTo;
  }

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

#else

  vector4_t weight = set4f(t);
  vector4_t fdt = set4f(fromDotTo);

  // Correct for input quats in different semi-arcs.
  vector4_t qt = qTo.quat;
  qt = neg4f(qt, fdt);
  fdt = abs4f(fdt);

  if ((1.0f - fabs(fromDotTo)) > QUATERNION_SLERP_LERP_ERROR_LIMIT)
  {
    // Source quaternions are different enough to perform a slerp.
    // Also fastArccos becomes inaccurate as c tends towards 1.
    //
    // Here, we replace sin(angle) with sqrt(1-fdt^2)
    vector4_t angle = acos4f(fdt);
    vector4_t s = rsqrt4f(sub4f(one4f(), mul4f(fdt, fdt)));

    //startWeight = fastSin((1.0f - t) * angle) / s;
    //endWeight   = fastSin(t * angle) / s;

    vector4_t fac = mixLoHi(sub4f(one4f(), weight), weight);
    vector4_t fsc = mul4f(sin4f(mul4f(fac, angle)), s);

    quat = madd4f(splatX(fsc), quat, mul4f(splatW(fsc), qt));
  }
  else
  {
    // Source quaternions are almost the same and slerping becomes inaccurate so do a lerp instead.
    quat = lerpMP4(quat, qt, weight);
  }

#endif

  // Normalizing is necessary as fastArccos is not accurate enough and because the lerp path does not produce
  // normal results when sources are too different.
  fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::lerp(const Quat& qFrom, const Quat& qTo, float t, float NMP_USED_FOR_ASSERTS(fromDotTo))
{
  vector4_t weight = set4f(t);
  vector4_t qt = qTo.quat;

#ifdef NMP_ENABLE_ASSERTS
  vector4_t fdt = set4f(fromDotTo);
#else
  vector4_t fdt = dot4f(qFrom.quat, qt);
#endif
  qt = neg4f(qt, fdt);

  quat = lerpMP4(qFrom.quat, qt, weight);
  fastNormalise();  // Result must always be normalised when lerping.
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::lerp(const Quat& qTo, float t, float NMP_USED_FOR_ASSERTS(fromDotTo))
{
  vector4_t weight = set4f(t);
  vector4_t qt = qTo.quat;
#ifdef NMP_ENABLE_ASSERTS
  vector4_t fdt = set4f(fromDotTo);
#else
  vector4_t fdt = dot4f(quat, qt);
#endif
  qt = neg4f(qt, fdt);

  quat = lerpMP4(quat, qt, weight);
  fastNormalise();  // Result must always be normalised when lerping.
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::exp(const Vector3& v)
{
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

  float phi = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  if (phi < FLT_EPSILON)
  {
    identity();
  }
  else
  {
    float fac = sinf(phi) / phi;
    w = cosf(phi);
    x = fac * v.x;
    y = fac * v.y;
    z = fac * v.z;
  }

#else

  static const vector4_t quatEps = fltEpsilon4f();

  vector4_t phi = sqrt4f(dot3f(v.vec, v.vec));
  vector4_t fac = sel4cmpLT(phi, quatEps, one4f(), phi);

  vector4_t s, qw; sinCos4f(phi, s, qw);
  quat = mul4f(v.vec, mul4f(s, rcp4f(fac)));
  quat = setwf(quat, qw);

#endif
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Quat::fastExp(const Vector3& v)
{
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

  float phi = fastSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (phi < FLT_EPSILON)
  {
    identity();
  }
  else
  {
    float fac;
    fastSinCos(phi, fac, w);
    fac /= phi;
    x = fac * v.x;
    y = fac * v.y;
    z = fac * v.z;
  }

#else

  vector4_t phi = sqrt4f(dot3f(v.vec, v.vec));
  vector4_t fac = sel4cmpLT(phi, fltEpsilon4f(), one4f(), phi);

  vector4_t s, qw; sinCos4f(phi, s, qw);
  quat = mul4f(v.vec, mul4f(s, rcp4f(fac)));
  quat = setwf(quat, qw);

#endif
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::log() const
{
  Vector3 v;

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

  float s = sqrtf(x * x + y * y + z * z);
  if (s < FLT_EPSILON)
  {
    v.set(0.0f);
  }
  else
  {
    float fac = atan2f(s, w) / s;
    v.set(fac * x, fac * y, fac * z);
  }

#else

  vector4_t s = sqrt4f(dot3f(quat, quat));
  vector4_t msk = mask4cmpLT(s, fltEpsilon4f());

  vector4_t fac = sel4cmpMask(msk, one4f(), s);
  vector4_t phi = atan24f(s, splatW(quat));

  fac = mul4f(phi, rcp4f(fac));
  fac = sel4cmpMask(msk, zero4f(), fac);

  v.vec = mul4f(quat, fac);
  v.vec = setwf(v.vec, zero4f());

#endif

  return v;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Quat::fastLog() const
{
  Vector3 v;

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)

  float s = fastSqrt(x * x + y * y + z * z);
  if (s < FLT_EPSILON)
  {
    v.set(0.0f);
  }
  else
  {
    float fac = fastAtan2(s, w) / s;
    v.set(fac * x, fac * y, fac * z);
  }

#else

  vector4_t s = sqrt4f(dot3f(quat, quat));
  vector4_t msk = mask4cmpLT(s, fltEpsilon4f());

  vector4_t fac = sel4cmpMask(msk, one4f(), s);
  vector4_t phi = atan24f(s, splatW(quat));

  fac = mul4f(phi, rcp4f(fac));
  fac = sel4cmpMask(msk, zero4f(), fac);

  v.vec = mul4f(quat, fac);
  v.vec = setwf(v.vec, zero4f());

#endif

  return v;
}

//----------------------------------------------------------------------------------------------------------------------
