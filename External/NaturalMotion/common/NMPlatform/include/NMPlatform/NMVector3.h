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
#ifndef NM_VECTOR3_H
#define NM_VECTOR3_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

// nonstandard extension used : nameless struct/union
// used in anon union in vector/matrix classes
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4201)
#endif

namespace NMP
{
class Matrix34;

// Compared against the magnitude of a vector to check if it is actually normalised.
#define IS_NORMAL_ERROR_LIMIT 0.000002f

/// \ingroup NaturalMotionPlatformMath
/// \brief A %Vector3 class, offering the usual set of functionality. For alignment
///        and packing reasons, it exists as a 4D vector, having a w component that
///        is not used in any of the class' functions or operators.
NMP_ALIGN_PREFIX(16) class Vector3
{
public:

  union
  {
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

  NM_INLINE Vector3();
  NM_INLINE Vector3(InitZeroType);
  NM_INLINE Vector3(InitOneType);
  NM_INLINE Vector3(InitOneXType);
  NM_INLINE Vector3(InitOneYType);
  NM_INLINE Vector3(InitOneZType);
  NM_INLINE Vector3(InitType init);
  NM_INLINE Vector3(const Vector3& copy);
  NM_INLINE Vector3(float inX, float inY);
  NM_INLINE Vector3(float inX, float inY, float inZ);
  NM_INLINE Vector3(float inX, float inY, float inZ, float inW);

  NM_INLINE float* getPtr() { return &f[0]; }
  NM_INLINE const float* getPtr() const { return &f[0]; }

  NM_INLINE const Vector3& operator = (const NMP::Vector3& copy);
#ifdef NM_COMPILER_GHS
  // GHS doesn't auto-magically deal with the use of pramga pack(4) on a 16-byte aligned
  //  struct, so we have to provide explicit assignement operators
  NM_INLINE const __packed Vector3& operator = (const NMP::Vector3& copy) __packed;
  NM_INLINE const __packed Vector3& operator = (const __packed NMP::Vector3& copy) __packed;
#endif // NM_COMPILER_GHS
  NM_INLINE float& operator [] (int32_t index);
  NM_INLINE float  operator [] (int32_t index) const;

  NM_INLINE bool  operator <  (const NMP::Vector3& v) const;
  NM_INLINE bool  operator <= (const NMP::Vector3& v) const;
  NM_INLINE bool  operator >= (const NMP::Vector3& v) const;
  NM_INLINE bool  operator >  (const NMP::Vector3& v) const;
  NM_INLINE bool  operator == (const NMP::Vector3& v) const;
  NM_INLINE bool  operator != (const NMP::Vector3& v) const;

  NM_INLINE Vector3  operator - () const;
  NM_INLINE Vector3  operator + (const NMP::Vector3& v) const;
  NM_INLINE Vector3  operator - (const NMP::Vector3& v) const;
  NM_INLINE Vector3  operator * (const NMP::Vector3& v) const;
  NM_INLINE Vector3  operator * (float f) const;
  NM_INLINE Vector3  operator / (float f) const;
  NM_INLINE void     operator += (const NMP::Vector3& v);
  NM_INLINE void     operator -= (const NMP::Vector3& v);
  NM_INLINE void     operator *= (const NMP::Vector3& v);
  NM_INLINE void     operator *= (float f);
  NM_INLINE void     operator /= (float f);

  /// \ingroup Maths
  /// \brief Copy the local values to the vector specified
  NM_INLINE void copyTo(NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Set all components of this vector to 0.0f, including the 'w' component
  NM_INLINE void setToZero();

  /// \ingroup Maths
  /// \brief Set all components of this vector to the given value, including the 'w' component
  NM_INLINE void set(float all);

  /// \ingroup Maths
  /// \brief Set the components of this vector in one go
  NM_INLINE void set(float inX, float inY, float inZ);

  /// \ingroup Maths
  /// \brief Set the components of this vector in one go.
  /// Be aware that the w component may be ignored and overwritten in other vector class functions.
  NM_INLINE void set(float inX, float inY, float inZ, float inW);

  /// \ingroup Maths
  /// \brief Copy the input vector into the local vector, including the 'w' component from 'v'
  NM_INLINE void set(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Copy the input vector into this vector, multiplying each
  ///        of the copied values by <tt>multiplier</tt>
  NM_INLINE void setScaledVector(const NMP::Vector3& v, float multiplier);

  /// \ingroup Maths
  /// \brief Copy the input vector into this vector, multiplying each
  ///        of the copied values by the associated values in <tt>multiplier</tt>
  NM_INLINE void setScaledVector(const NMP::Vector3& v, const NMP::Vector3& multiplier);

  /// \ingroup Maths
  /// \brief Multiply each element of this vector by the corresponding elements in the given vector,
  ///        the 'w' component is ignored
  NM_INLINE void multiplyElements(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Divide each element of this vector by the corresponding elements in the given vector,
  ///        the 'w' component is ignored
  NM_INLINE void divideElements(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Compare this vector to the input, with a given floating point tolerance value;
  ///        returns true if the comparison passes.
  NM_INLINE bool compare(const NMP::Vector3& v, const float tolerance) const;

  /// \ingroup Maths
  /// \brief compare two vectors, with a given floating point tolerance value;
  ///        returns true if the comparison passes.
  static bool compare(const NMP::Vector3& a, const NMP::Vector3& b, const float tolerance);

  /// \ingroup Maths
  /// \brief Set this vector to be the cross product of the two input vectors
  NM_INLINE void cross(const NMP::Vector3& a, const NMP::Vector3& b);

  /// \ingroup Maths
  /// \brief Set this vector to be the cross product of this vector and the given input
  NM_INLINE void cross(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Return the dot product of this vector with the input vector
  NM_INLINE float dot(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Returns the vector with the component in dir (which does not need to be normalised) removed
  NM_INLINE Vector3 getComponentOrthogonalToDir(const NMP::Vector3& dir) const;

  /// \ingroup Maths
  /// \brief Store maximum values between this vector and given input vector -
  ///        eg; this.x = max(a.x, b.x), yzw
  NM_INLINE void maximum(const NMP::Vector3& a, const NMP::Vector3& b);

  /// \ingroup Maths
  /// \brief Store minimum values between this vector and given input vector -
  ///        eg; this.x = max(a.x, b.x), yzw
  NM_INLINE void minimum(const NMP::Vector3& a, const NMP::Vector3& b);

  /// \ingroup Maths
  /// \brief Clamp the values in this vector by the two limits
  NM_INLINE void clampComponents(float minV, float maxV);

  /// \ingroup Maths
  /// \brief Clamp the values in this vector by the two limits. Returns true if clamping occurred, false otherwise
  NM_INLINE bool clampMagnitude(float maxV);

  /// \ingroup Maths
  /// \brief Calls fabs on each element in this vector
  NM_INLINE void abs();

  /// \ingroup Maths
  /// \brief return the sum of the absolute values of the coordinates
  ///        this is called the L-1 norm.
  NM_INLINE float summedAbs() const;

  /// \ingroup Maths
  /// \brief Return the sum of the absolute values of the coordinates of this vector - b
  NM_INLINE float summedAbsDiff(const NMP::Vector3& b);

  /// \ingroup Maths
  /// \brief Return the maximum of the absolute values of the coordinates.
  ///        This is called the L-infinity norm.
  NM_INLINE float maxAbs() const;

  /// \ingroup Maths
  /// \brief Find the magnitude, or length, of the vector. eg. sqrt(magnitudeSquared())
  NM_INLINE float magnitude() const;

  /// \ingroup Maths
  /// \brief Return the squared magnitude, or length, of the vector
  NM_INLINE float magnitudeSquared() const;

  /// \ingroup Maths
  /// \brief Return the distance from this point to the point specified
  NM_INLINE float distanceTo(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Return the distance squared (no sqrtf) from this point to the point specified
  NM_INLINE float distanceSquaredTo(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Return a normalised copy of this vector - returns defaultValue if the magnitude was (effectively) 0
  NM_INLINE Vector3 getNormalised(InitType defaultValue = InitTypeOneX) const;

  /// \ingroup Maths
  /// \brief Normalize this vector, returning its original magnitude
  NM_INLINE float normaliseGetLength(InitType defaultValue = InitTypeOneX);

  /// \ingroup Maths
  /// \brief Normalize this vector, defaults to InitTypeOneX for a NULL vector
  NM_INLINE void normalise();

  /// \ingroup Maths
  /// \brief Normalize this vector, takes a replacement for a NULL vector
  NM_INLINE void normalise(InitType defaultValue);

  /// \ingroup Maths
  /// \brief Normalize this vector, takes a literal replacement for a NULL vector
  NM_INLINE void normaliseOrDef(const NMP::Vector3& defvec);

  /// \ingroup Maths
  /// \brief Normalize the input vector, storing the results locally and returning the magnitude of v
  NM_INLINE float normaliseDep(const NMP::Vector3& v, InitType defaultValue = InitTypeOneX);

  /// \ingroup Maths
  /// \brief Normalize this vector (fast approximate), defaults to InitTypeOneX for a NULL vector
  NM_INLINE void fastNormalise();

  /// \ingroup Maths
  /// \brief Normalize this vector (fast approximate), takes a replacement for a NULL vector
  NM_INLINE void fastNormalise(InitType defaultValue);

  /// \ingroup Maths
  /// \brief Normalize this vector (fast approximate), takes a literal replacement for a NULL vector
  NM_INLINE void fastNormaliseOrDef(const NMP::Vector3& defvec);

  /// \ingroup Maths
  /// \brief Compute a vector3 orthogonal to another vector3
  NM_INLINE void makeOrthogonal(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Computes the result of 1.0f / the contents of this vector
  NM_INLINE void reciprocal();

  /// \ingroup Maths
  /// \brief Linearly interpolate between vectors a and b, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local vector directly.
  NM_INLINE void lerp(const NMP::Vector3& vFrom, const NMP::Vector3& vTo, float t);

  /// \ingroup Maths
  /// \brief Linearly interpolate between the local vector and the given target, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local vector directly.
  NM_INLINE void lerp(const NMP::Vector3& vTo, float t);

  /// \ingroup Maths
  /// \brief Slerp interpolate between the 2 input vectors and produce a vector result.
  ///        Linearly interpolates the angle between the inputs and the length of the inputs.
  ///        NOTE: Result becomes inaccurate as either input vector length tends to zero or if vectors lie parallel and in
  ///        diametrically opposite directions.
  void slerp(const NMP::Vector3& sourceVec0, const NMP::Vector3& sourceVec1, float alpha);

  /// \ingroup Maths
  /// \brief Transform this vector by the specified matrix
  void transform(const Matrix34& m);

  /// \ingroup Maths
  /// \brief Transform this vector by the 3x3 rotation component of the specified matrix
  void rotate(const Matrix34& m);

  /// \ingroup Maths
  /// \brief Transform this vector by the inverse of the specified matrix
  void inverseTransform(const Matrix34& m);

  /// \ingroup Maths
  /// \brief Transform this vector by the transpose of the 3x3 rotation component of the specified matrix
  void inverseRotate(const Matrix34& m);

  /// \ingroup Maths
  /// \brief Checks that the vector does not contain any QNAN or INF values.
  bool isValid() const;

  /// \ingroup Maths
  /// \brief Is this vector normalised (within error limits).
  NM_INLINE bool isNormal() const;

  /// \ingroup Maths
  /// \brief Is this vector normalised (within error limits) - checks if the magnitude squared
  ///        is within tolerance of 1.
  NM_INLINE bool isNormal(float tolerance) const;

} NMP_ALIGN_SUFFIX(16);

// a bank of useful constants
// done in this slightly curious way to avoid static member variables,
// which, in the past, caused problems during dynamic module relocation
// on one of our clients' configurations
NM_INLINE static Vector3 Vector3XAxis();
NM_INLINE static Vector3 Vector3YAxis();
NM_INLINE static Vector3 Vector3ZAxis();
NM_INLINE static Vector3 Vector3Zero();
NM_INLINE static Vector3 Vector3One();
NM_INLINE static Vector3 Vector3Half();

// commonly used vector functions that create new vector results directly,
// placed here to decrease typing required to use them, eg:
// x = NMP::vCross(a, b)
// x = NMP::Vector3::vCross(a, b)
NM_INLINE float    vDot(const NMP::Vector3& a, const NMP::Vector3& b);
NM_INLINE float    vDistanceBetween(const NMP::Vector3& vFrom, const NMP::Vector3& vTo);
NM_INLINE Vector3  vNormalise(const NMP::Vector3& a);
NM_INLINE Vector3  vCross(const NMP::Vector3& a, const NMP::Vector3& b);
NM_INLINE Vector3  vMin(const NMP::Vector3& a, const NMP::Vector3& b);
NM_INLINE Vector3  vMax(const NMP::Vector3& a, const NMP::Vector3& b);
NM_INLINE Vector3  vLerp(const NMP::Vector3& vFrom, const NMP::Vector3& vTo, float t);
Vector3  vTransform(const NMP::Vector3& v, const Matrix34& m);
Vector3  vRotate(const NMP::Vector3& v, const Matrix34& m);
Vector3  vInverseTransform(const NMP::Vector3& v, const Matrix34& m);
Vector3  vInverseRotate(const NMP::Vector3& v, const Matrix34& m);

// Useful utility functions
void vBounds(uint32_t n, const Vector3* v, Vector3& minV, Vector3& maxV);

} // namespace NMP

#include "NMPlatform/generic/NMVector3.inl"

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_VECTOR3_H
//----------------------------------------------------------------------------------------------------------------------
