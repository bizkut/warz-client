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

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPU
/// \class NMP::vpu::Vector3
///
/// \brief
///
//----------------------------------------------------------------------------------------------------------------------

class Vector3
{

public:

  union
  {
    vector4_t vec;

    struct
    {
      float x, y, z, w;
    };

    float f[4];
  };

  enum InitZeroType { InitZero };
  enum InitOneType { InitOne };
  enum InitOneXType { InitOneX };
  enum InitOneYType { InitOneY };
  enum InitOneZType { InitOneZ };
  enum InitType { InitTypeZero, InitTypeOne, InitTypeOneX, InitTypeOneY, InitTypeOneZ };

  NM_FORCEINLINE Vector3() { };
  NM_FORCEINLINE Vector3(InitType init);

  NM_FORCEINLINE Vector3(InitZeroType);
  NM_FORCEINLINE Vector3(InitOneType);
  NM_FORCEINLINE Vector3(InitOneXType);
  NM_FORCEINLINE Vector3(InitOneYType);
  NM_FORCEINLINE Vector3(InitOneZType);

  NM_FORCEINLINE Vector3(const Vector3& copy);
  NM_FORCEINLINE Vector3(vector4_t copy);

  NM_FORCEINLINE Vector3(float inX, float inY, float inZ);
  NM_FORCEINLINE Vector3(float inX, float inY, float inZ, float inW);

  NM_FORCEINLINE const Vector3& operator = (const Vector3& copy);

  NM_FORCEINLINE bool  operator <  (const Vector3& v) const;
  NM_FORCEINLINE bool  operator <= (const Vector3& v) const;
  NM_FORCEINLINE bool  operator >= (const Vector3& v) const;
  NM_FORCEINLINE bool  operator >  (const Vector3& v) const;
  NM_FORCEINLINE bool  operator == (const Vector3& v) const;
  NM_FORCEINLINE bool  operator != (const Vector3& v) const;

  NM_FORCEINLINE Vector3  operator - () const;
  NM_FORCEINLINE Vector3  operator + (const Vector3& v) const;
  NM_FORCEINLINE Vector3  operator - (const Vector3& v) const;
  NM_FORCEINLINE Vector3  operator * (const Vector3& v) const;
  NM_FORCEINLINE Vector3  operator * (vector4_t v) const;
  NM_FORCEINLINE Vector3  operator * (float f) const;
  NM_FORCEINLINE Vector3  operator / (float f) const;
  NM_FORCEINLINE void     operator += (const Vector3& v);
  NM_FORCEINLINE void     operator -= (const Vector3& v);
  NM_FORCEINLINE void     operator *= (const Vector3& v);
  NM_FORCEINLINE void     operator *= (vector4_t v);
  NM_FORCEINLINE void     operator *= (float f);
  NM_FORCEINLINE void     operator /= (float f);

  /// \ingroup Maths
  /// \brief Copy the local values to the vector specified
  NM_FORCEINLINE void copyTo(Vector3& v) const;

  /// \ingroup Maths
  /// \brief Set all components of this vector to 0.0f, including the 'w' component
  NM_FORCEINLINE void setToZero();

  /// \ingroup Maths
  /// \brief Set all components of this vector to the given value, including the 'w' component
  NM_FORCEINLINE void set(float all);

  /// \ingroup Maths
  /// \brief Set the components of this vector in one go
  NM_FORCEINLINE void set(float inX, float inY, float inZ);

  /// \ingroup Maths
  /// \brief Copy the input vector into the local vector, including the 'w' component from 'v'
  NM_FORCEINLINE void set(const Vector3& v);

  /// \ingroup Maths
  /// \brief Copy the input vector into this vector, multiplying each
  ///        of the copied values by <tt>multiplier</tt>
  NM_FORCEINLINE void setScaledVector(const Vector3& v, float multiplier);

  /// \ingroup Maths
  /// \brief Copy the input vector into this vector, multiplying each
  ///        of the copied values by the associated values in <tt>multiplier</tt>
  NM_FORCEINLINE void setScaledVector(const Vector3& v, const Vector3& multiplier);

  /// \ingroup Maths
  /// \brief Multiply each element of this vector by the corresponding elements in the given vector,
  /// the 'w' component is ignored
  NM_FORCEINLINE void multiplyElements(const Vector3& v);

  /// \ingroup Maths
  /// \brief Divide each element of this vector by the corresponding elements in the given vector,
  /// the 'w' component is ignored
  NM_FORCEINLINE void divideElements(const Vector3& v);

  /// \ingroup Maths
  /// \brief Compare this vector to the input, with a given floating point tolerance value;
  /// returns true if the comparison passes.
  NM_FORCEINLINE bool compare(const Vector3& v, const float tolerance) const;

  /// \ingroup Maths
  /// \brief compare two vectors, with a given floating point tolerance value;
  /// returns true if the comparison passes.
  static bool compare(const Vector3& a, const Vector3& b, const float tolerance);

  /// \ingroup Maths
  /// \brief Set this vector to be the cross product of the two input vectors
  NM_FORCEINLINE void cross(const Vector3& a, const Vector3& b);

  /// \ingroup Maths
  /// \brief Set this vector to be the cross product of this vector and the given input
  NM_FORCEINLINE void cross(const Vector3& v);

  /// \ingroup Maths
  /// \brief Return the dot product of this vector with the input vector
  NM_FORCEINLINE float dot(const Vector3& v) const;

  /// \ingroup Maths
  /// \brief Store maximum values between this vector and given input vector -
  ///        eg; this.x = max(a.x, b.x), yzw
  NM_FORCEINLINE void maximum(const Vector3& a, const Vector3& b);

  /// \ingroup Maths
  /// \brief Store minimum values between this vector and given input vector -
  ///        eg; this.x = max(a.x, b.x), yzw
  NM_FORCEINLINE void minimum(const Vector3& a, const Vector3& b);

  /// \ingroup Maths
  /// \brief Clamp the values in this vector by the two limits
  NM_FORCEINLINE void clampComponents(float minV, float maxV);

  /// \ingroup Maths
  /// \brief Clamp the values in this vector by the two limits
  NM_FORCEINLINE void clampMagnitude(float maxV);

  /// \ingroup Maths
  /// \brief Calls fabs on each element in this vector
  NM_FORCEINLINE void abs();

  /// \ingroup Maths
  /// \brief return the sum of the absolute values of the coordinates
  ///        this is called the L-1 norm.
  NM_FORCEINLINE float summedAbs() const;

  /// \ingroup Maths
  /// \brief Return the sum of the absolute values of the coordinates of this vector - b
  NM_FORCEINLINE float summedAbsDiff(const Vector3& b);

  /// \ingroup Maths
  /// \brief Find the magnitude, or length, of the vector. eg. sqrt(magnitudeSquared())
  NM_FORCEINLINE float magnitude() const;

  /// \ingroup Maths
  /// \brief Return the squared magnitude, or length, of the vector
  NM_FORCEINLINE float magnitudeSquared() const;

  /// \ingroup Maths
  /// \brief Return the distance from this point to the point specified
  NM_FORCEINLINE float distanceTo(const Vector3& v) const;

  /// \ingroup Maths
  /// \brief Return the distance squared (no sqrtf) from this point to the point specified
  NM_FORCEINLINE float distanceSquaredTo(const Vector3& v) const;

  /// \ingroup Maths
  /// \brief Return a normalised copy of this vector - returns defaultValue if the magnitude was (effectively) 0
  NM_FORCEINLINE Vector3 getNormalised(InitType defaultValue = InitTypeOneX) const;

  /// \ingroup Maths
  /// \brief Normalize this vector, returning its original magnitude
  NM_FORCEINLINE float normaliseGetLength(InitType defaultValue = InitTypeOneX);

  NM_FORCEINLINE void normalise();
  NM_FORCEINLINE void normalise(InitType defaultValue);

  NM_FORCEINLINE void normaliseOrDef(const Vector3& defvec);

  /// \ingroup Maths
  /// \brief Normalize the input vector, storing the results locally and returning the magnitude of v
  NM_FORCEINLINE float normaliseDep(const Vector3& v, InitType defaultValue = InitTypeOneX);

  /// \ingroup Maths
  /// \brief Compute a Vector3 orthogonal to another Vector3
  NM_FORCEINLINE void makeOrthogonal(const Vector3& v);

  /// \ingroup Maths
  /// \brief Computes the result of 1.0f / the contents of this vector
  NM_FORCEINLINE void reciprocal();

  /// \ingroup Maths
  /// \brief Linearly interpolate between vectors a and b, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local vector directly.
  NM_FORCEINLINE void lerp(const Vector3& vFrom, const Vector3& vTo, float t);

  /// \ingroup Maths
  /// \brief Linearly interpolate between the local vector and the given target, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local vector directly.
  NM_FORCEINLINE void lerp(const Vector3& vTo, float t);
};
//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Vector3::Vector3(InitType init)
{
  switch (init)
  {
  case InitTypeZero:
    vec = zero4f();
    break;
  case InitTypeOne:
    vec = setwf(one4f(), zero4f());
    break;
  case InitTypeOneX:
    vec = setxf(zero4f(), one4f());
    break;
  case InitTypeOneY:
    vec = setyf(zero4f(), one4f());
    break;
  case InitTypeOneZ:
    vec = setzf(zero4f(), one4f());
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(InitZeroType)
{
  vec = zero4f();
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(InitOneType)
{
  vec = setwf(one4f(), zero4f());
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(InitOneXType)
{
  vec = setxf(zero4f(), one4f());
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(InitOneYType)
{
  vec = setyf(zero4f(), one4f());
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(InitOneZType)
{
  vec = setzf(zero4f(), one4f());
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(const Vector3& copy)
{
  vec = copy.vec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(vector4_t vect)
{
  vec = vect;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(float inX, float inY, float inZ)
{
  vec = set4f(inX, inY, inZ, 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3::Vector3(float inX, float inY, float inZ, float inW)
{
  vec = set4f(inX, inY, inZ, inW);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE const Vector3& Vector3::operator = (const Vector3& copy)
{
  NMP_ASSERT(NMP_IS_ALIGNED(this, 16));
  NMP_ASSERT(NMP_IS_ALIGNED(&copy, 16));
  vec = copy.vec;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::operator < (const Vector3& v) const
{
  // (x < v.x) && (y < v.y) && (z < v.z)
  return 0 != elemX(isBound3f(vec, v.vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::operator > (const Vector3& v) const
{
  // (x > v.x) && (y > v.y) && (z > v.z)
  return 0 != elemX(isBound3f(v.vec, vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::operator <= (const Vector3& v) const
{
  // (x <= v.x) && (y <= v.y) && (z <= v.z)
  return 0 == elemX(isBound3f(v.vec, vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::operator >= (const Vector3& v) const
{
  // (x >= v.x) && (y >= v.y) && (z >= v.z)
  return 0 == elemX(isBound3f(vec, v.vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::operator == (const Vector3& v) const
{
  return ((x == v.x) && (y == v.y) && (z == v.z));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::operator != (const Vector3& v) const
{
  return !(*this == v);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  Vector3::operator - () const
{
  return Vector3(neg4f(vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  Vector3::operator + (const Vector3& v) const
{
  return Vector3(add4f(vec, v.vec));
}
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  Vector3::operator - (const Vector3& v) const
{
  return Vector3(sub4f(vec, v.vec));
}
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  Vector3::operator * (float f) const
{
  return Vector3(mul4f(vec, set4f(f)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  Vector3::operator * (const Vector3& v) const
{
  return Vector3(mul4f(vec, v.vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  Vector3::operator / (float f) const
{
  return Vector3(mul4f(vec, rcp4f(set4f(f))));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void     Vector3::operator += (const Vector3& v)
{
  vec = add4f(vec, v.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void     Vector3::operator -= (const Vector3& v)
{
  vec = sub4f(vec, v.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void     Vector3::operator *= (float f)
{
  vec = mul4f(vec, set4f(f));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void     Vector3::operator *= (const Vector3& v)
{
  vec = mul4f(vec, v.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void     Vector3::operator *= (vector4_t v)
{
  vec = mul4f(vec, v);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void     Vector3::operator /= (float f)
{
  vec = mul4f(vec, rcp4f(set4f(f)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::copyTo(Vector3& v) const
{
  v.vec = vec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::setToZero()
{
  vec = zero4f();
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::set(float all)
{
  vec = set4f(all);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::set(float inX, float inY, float inZ)
{
  vec = set4f(inX, inY, inZ, 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::set(const Vector3& v)
{
  vec = setwf(v.vec, zero4f());
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::setScaledVector(const Vector3& v, float multiplier)
{
  vec = mul4f(v.vec, set4f(multiplier));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::setScaledVector(const Vector3& v, const Vector3& multiplier)
{
  vec = mul4f(v.vec, multiplier.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::multiplyElements(const Vector3& v)
{
  vec = mul4f(vec, v.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::divideElements(const Vector3& v)
{
  vec = mul4f(vec, rcp4f(v.vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Vector3::compare(const Vector3& v, const float tolerance) const
{
  // Equivalent to
  //   if (NMP::nmfabs(x - v.x) >= tolerance)
  //       return false;
  //   if (NMP::nmfabs(y - v.y) >= tolerance)
  //       return false;
  //   if (NMP::nmfabs(z - v.z) >= tolerance)
  //       return false;
  //   return true;
  
  return 0 != elemX(isBound3f(abs4f(sub4f(vec, v.vec)), set4f(tolerance)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::cross(const Vector3& a, const Vector3& b)
{
  vec = cross3f(a.vec, b.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::cross(const Vector3& v)
{
  vec = cross3f(vec, v.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::dot(const Vector3& v) const
{
  return floatX(dot3f(vec, v.vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::maximum(const Vector3& a, const Vector3& b)
{
  vec = sel4cmpLT(a.vec, b.vec, b.vec, a.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::minimum(const Vector3& a, const Vector3& b)
{
  vec = sel4cmpLT(b.vec, a.vec, b.vec, a.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::clampComponents(float minV, float maxV)
{
  vector4_t vmin = set4f(minV);
  vector4_t vmax = set4f(maxV);
  vec = sel4cmpLT(vec, vmin, vmin, vec);
  vec = sel4cmpLT(vmax, vec, vmax, vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::clampMagnitude(float maxV)
{
  NMP_ASSERT(maxV >= 0.0f);
  vector4_t mag2 = dot3f(vec, vec);
  float magSqr = floatX(mag2);
  if (magSqr > maxV * maxV)
    *this *= mul4f(set4f(maxV), rsqrt4f(mag2));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::abs()
{
  vec = abs4f(vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::summedAbs() const
{
  // fabs(x) + fabs(y) + fabs(z)
  return floatX(dot3f(abs4f(vec), one4f()));  
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::summedAbsDiff(const Vector3& b)
{
  // fabs(x - b.x) + fabs(y - b.y) + fabs(z - b.z)
  return floatX(dot3f(abs4f(sub4f(vec, b.vec)), one4f()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::magnitude() const
{
  return floatX(sqrt4f(dot3f(vec, vec)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::magnitudeSquared() const
{
  return floatX(dot3f(vec, vec));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::distanceTo(const Vector3& v) const
{
  vector4_t diff = sub4f(vec, v.vec);
  return floatX(sqrt4f(dot3f(diff, diff)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::distanceSquaredTo(const Vector3& v) const
{
  vector4_t diff = sub4f(vec, v.vec);
  return floatX(dot3f(diff, diff));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Vector3::getNormalised(InitType defaultValue) const
{
  Vector3 result(x, y, z, w);
  result.normalise(defaultValue);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float Vector3::normaliseGetLength(InitType defaultValue)
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
NM_FORCEINLINE void Vector3::normalise(InitType defaultValue)
{
  float length = magnitude();

  if (length == 0)
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
NM_FORCEINLINE float Vector3::normaliseDep(const Vector3& v, InitType defaultValue)
{
  float length = v.magnitude();
  if (length == 0)
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
NM_FORCEINLINE void Vector3::normaliseOrDef(const Vector3& defvec)
{
  vector4_t mag2 = dot3f(vec, vec);
  vec = sel4cmpEQ(mag2, zero4f(), defvec.vec, mul4f(vec, rsqrt4f(mag2)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::normalise()
{
  vector4_t defvec = setxf(zero4f(), one4f());
  vector4_t mag2 = dot3f(vec, vec);

  vec = sel4cmpEQ(mag2, zero4f(), defvec, mul4f(vec, rsqrt4f(mag2)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::makeOrthogonal(const Vector3& v)
{
  Vector3 vNorm(v);
  vNorm.normalise();

  if (vNorm.z < 0.5f && vNorm.z > -0.5f)
    set(-vNorm.y, vNorm.x, 0);
  else
    set(-vNorm.z, 0, vNorm.x);

  normalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::reciprocal()
{
  vec = setwf(vec, one4f());
  vec = rcp4f(vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::lerp(const Vector3& vFrom, const Vector3& vTo, float t)
{
  vec = madd4f(set4f(t), sub4f(vTo.vec, vFrom.vec), vFrom.vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Vector3::lerp(const Vector3& vTo, float t)
{
  vec = madd4f(set4f(t), sub4f(vTo.vec, vec), vec);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3  operator*(float f, const Vector3& b)
{
  return Vector3(f * b.x, f * b.y, f * b.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float vDot(const Vector3& a, const Vector3& b)
{
  return ((Vector3)a).dot(b);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float vDistanceBetween(const Vector3& vFrom, const Vector3& vTo)
{
  return ((Vector3)vFrom).distanceTo(vTo);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 vNormalise(const Vector3& a)
{
  Vector3 result(a);
  result.normalise();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 vCross(const Vector3& a, const Vector3& b)
{
  Vector3 result;
  result.cross(a, b);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 vMin(const Vector3& a, const Vector3& b)
{
  Vector3 result;
  result.minimum(a, b);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 vMax(const Vector3& a, const Vector3& b)
{
  Vector3 result;
  result.maximum(a, b);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 vLerp(const Vector3& vFrom, const Vector3& vTo, float t)
{
  Vector3 result;
  result.lerp(vFrom, vTo, t);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Vector3 Vector3XAxis() { return Vector3(1.0f, 0.0f, 0.0f); }
NM_FORCEINLINE Vector3 Vector3YAxis() { return Vector3(0.0f, 1.0f, 0.0f); }
NM_FORCEINLINE Vector3 Vector3ZAxis() { return Vector3(0.0f, 0.0f, 1.0f); }
NM_FORCEINLINE Vector3 Vector3Zero()  { return Vector3(0.0f, 0.0f, 0.0f); }
NM_FORCEINLINE Vector3 Vector3One()   { return Vector3(1.0f, 1.0f, 1.0f); }
NM_FORCEINLINE Vector3 Vector3Half()  { return Vector3(0.5f, 0.5f, 0.5f); }

//----------------------------------------------------------------------------------------------------------------------
