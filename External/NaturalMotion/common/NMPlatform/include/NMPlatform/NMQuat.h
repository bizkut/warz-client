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
#ifndef NM_QUAT_H
#define NM_QUAT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

// nonstandard extension used : nameless struct/union
// used in anon union in vector/matrix classes
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4201)
  #pragma warning (disable : 4328)
#endif

namespace NMP
{
class Vector3;

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

/// \ingroup NaturalMotionPlatformMath
/// \brief
NMP_ALIGN_PREFIX(16) class Quat
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

  enum QuatConstruction {
    kIdentity
  };

  NM_INLINE Quat();
  NM_INLINE Quat(QuatConstruction qc);
  NM_INLINE Quat(const Quat& copy);
  NM_INLINE Quat(const Vector3& rotationVector, bool tanQuarterAngle);
  NM_INLINE Quat(const Vector3& axis, float angle);
  NM_INLINE Quat(const Vector3& tanHalfAngleVec);
  NM_INLINE Quat(float inX, float inY, float inZ, float inW);

  NM_INLINE float* getPtr() { return &f[0]; }
  NM_INLINE const float* getPtr() const { return &f[0]; }

  NM_INLINE float& operator [] (int32_t index);
  NM_INLINE float  operator [] (int32_t index) const;

  NM_INLINE Quat   operator - () const;
  NM_INLINE Quat   operator ~ () const; // conjugate
  NM_INLINE Quat   operator + (const NMP::Quat& q) const;
  NM_INLINE Quat   operator - (const NMP::Quat& q) const;
  NM_INLINE Quat   operator * (const NMP::Quat& q) const;
  NM_INLINE void   operator += (const NMP::Quat& q);
  NM_INLINE void   operator -= (const NMP::Quat& q);
  NM_INLINE void   operator *= (const NMP::Quat& q);
  NM_INLINE bool   operator == (const NMP::Quat& q) const;
  NM_INLINE bool   operator != (const NMP::Quat& q) const;

  NM_INLINE Quat   operator * (float scalar) const;
  NM_INLINE void   operator *= (float scalar);

  NM_INLINE const Quat& operator = (const NMP::Quat& copy);
#ifdef NM_COMPILER_GHS
  // GHS doesn't auto-magically deal with the use of pramga pack(4) on a 16-byte aligned
  //  struct, so we have to provide explicit assignement operators
  NM_INLINE const __packed Quat& operator = (const NMP::Quat& copy) __packed;
  NM_INLINE const __packed Quat& operator = (const __packed NMP::Quat& copy) __packed;
#endif // NM_COMPILER_GHS
  /// \ingroup Maths
  /// \brief Set to the identity, xyz being 0.0 and w being 1.0
  NM_INLINE void identity();

  /// \ingroup Maths
  /// \brief Copy the local values to the quaternion specified
  NM_INLINE void copyTo(NMP::Quat& q);

  /// \ingroup Maths
  /// \brief Copy the input quaternion into the local quaternion
  NM_INLINE void set(const NMP::Quat& q);

  /// \ingroup Maths
  /// \brief Set values directly, in the order X Y Z W
  NM_INLINE void setXYZW(float inX, float inY, float inZ, float inW);

  /// \ingroup Maths
  /// \brief Set values directly, in the order W X Y Z
  NM_INLINE void setWXYZ(float inW, float inX, float inY, float inZ);

  /// \ingroup Maths
  /// \brief Sets quaternion to -x, -y, -z, -w
  NM_INLINE void negate();

  /// \ingroup Maths
  /// \brief Invert the XYZ component of this quaternion, producing the opposite
  /// rotation
  NM_INLINE void conjugate();

  /// \ingroup Maths
  /// \brief Calculate the quaternion's angle of rotation, returning the angle
  /// in radians within the range 0 to 2pi.
  NM_INLINE float angle() const;

  /// \ingroup Maths
  /// \brief Compute the angle of rotation between this and toQuat (this is the distance between both quaternions).
  /// The returned angle is in radians within the range 0 to 2pi.
  NM_INLINE float angleTo(const NMP::Quat& toQuat) const;

  /// \ingroup Maths
  /// \brief Compare this quaternion to the input, with a given floating point tolerance value;
  /// returns true if the comparison passes.
  NM_INLINE bool compare(const NMP::Quat& q, const float tolerance) const;

  /// \ingroup Maths
  /// \brief compare two quaternions, with a given floating point tolerance value;
  /// returns true if the comparison passes.
  static bool compare(const NMP::Quat& a, const NMP::Quat& b, const float tolerance);

  /// \ingroup Maths
  /// \brief 4 element dot product of this quaternion with input quaternion.
  NM_INLINE float dot(const NMP::Quat& q) const;

  /// \ingroup Maths
  /// \brief Normalize this quaternion
  NM_INLINE void normalise();

  /// \ingroup Maths
  /// \brief Normalize the input quaternion, storing the results locally
  NM_INLINE void normalise(const NMP::Quat& q);

  /// \ingroup Maths
  /// \brief Normalize this quaternion, returning its original magnitude
  NM_INLINE float normaliseGetLength();

  /// \ingroup Maths
  /// \brief Normalize the input quaternion, storing the results locally and returning the magnitude of q
  NM_INLINE float normaliseGetLength(const NMP::Quat& q);

  /// \ingroup Maths
  /// \brief Normalize this quaternion
  NM_INLINE void fastNormalise();

  /// \ingroup Maths
  /// \brief returns the normalised version of this quaternion
  NM_INLINE Quat getNormalised() const;

  /// \ingroup Maths
  /// \brief Normalize the input quaternion, storing the results locally
  NM_INLINE void fastNormalise(const NMP::Quat& q);

  /// \ingroup Maths
  /// \brief Return the squared magnitude of the quaternion
  NM_INLINE float magnitudeSquared() const;

  /// \ingroup Maths
  /// \brief Find the magnitude of the quaternion.
  NM_INLINE float magnitude() const;

  /// \ingroup Maths
  /// \brief Multiply this quaternion with the input quaternion, result stored in local values
  NM_INLINE void multiply(const NMP::Quat& q);

  /// \ingroup Maths
  /// \brief Multiply the two input quaternions, result stored in local values
  NM_INLINE void multiply(const NMP::Quat& a, const NMP::Quat& b);

  /// \ingroup Maths
  /// \brief Rotate the Vector3 specified by this quaternion
  NM_INLINE Vector3 rotateVector(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief get the axis of the orientation
  NM_INLINE Vector3 getXAxis() const;

  /// \ingroup Maths
  /// \brief get the axis of the orientation
  NM_INLINE Vector3 getYAxis() const;

  /// \ingroup Maths
  /// \brief get the axis of the orientation
  NM_INLINE Vector3 getZAxis() const;

  /// \ingroup Maths
  /// \brief Rotate the Vector3 specified by this quaternion, place results into output vector
  NM_INLINE void rotateVector(const NMP::Vector3& vInput, NMP::Vector3& vOutput) const;

  /// \ingroup Maths
  /// \brief Inverse rotate the Vector3 specified by this quaternion
  NM_INLINE Vector3 inverseRotateVector(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Inverse rotate the Vector3 specified by this quaternion, place results into output vector
  NM_INLINE void inverseRotateVector(const NMP::Vector3& vInput, NMP::Vector3& vOutput) const;

  /// \ingroup Maths
  /// \brief Construct the quaternion from the euler rotation angles provided, applied in order X, Y, Z.
  NM_INLINE void fromEulerXYZ(const NMP::Vector3& eulers);

  /// \ingroup Maths
  /// \brief Convert the quaternion to a set of euler rotation angles, rotation order X, Y, Z.
  NM_INLINE Vector3 toEulerXYZ() const;

  /// \ingroup Maths
  /// \brief Set the quaternion from an axis and angle.
  NM_INLINE void fromAxisAngle(const Vector3& axis, float angle);

  /// \ingroup Maths
  /// \brief Convert the quaternion to an axis and an angle.
  /// The returned angle is in radians within the range -pi to pi
  NM_INLINE void toAxisAngle(Vector3& axis, float& angle) const;

  /// \ingroup Maths
  /// \brief Set the quaternion from an axis and a tan-half-angle.
  NM_INLINE void fromTanHalfAngleRotationVector(const Vector3& thaVec);

  /// \ingroup Maths
  /// \brief Set the quaternion from an axis and a tan-quarter-angle.
  NM_INLINE void fromTanQuarterAngleRotationVector(const Vector3& thaVec);

  /// \ingroup Maths
  /// \brief Convert the quaternion from a rotation vector
  NM_INLINE void fromRotationVector(const NMP::Vector3& r);
  NM_INLINE void fromRotationVector(const NMP::Vector3& r, bool tanQuarterAngle); // Legacy

  /// \ingroup Maths
  /// \brief Convert the quaternion to a tan-quarter-angle rotation vector
  NM_INLINE Vector3 toTanQuarterAngleRotationVector() const;  

  /// \ingroup Maths
  /// \brief Convert the quaternion to a rotation vector
  NM_INLINE Vector3 toRotationVector() const;
  NM_INLINE Vector3 toRotationVector(bool tanQuarterAngle) const; // Legacy

  /// \ingroup Maths
  /// \brief Calculates quaternion required to rotate v1 into v2; v1 and v2 need not be normalised
  NM_INLINE void forRotation(const NMP::Vector3& v1, const NMP::Vector3& v2);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If qFrom and qTo are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_INLINE void slerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If qFrom and qTo are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_INLINE void slerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If both input quaternions are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_INLINE void slerp(const NMP::Quat& qTo, float t);

  /// \ingroup Maths
  /// \brief Calculate spherical linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If both input quaternions are normalised then the result will be normalised (within error limits).
  ///        This function also uses fast trig functions and is hence not 100% accurate.
  NM_INLINE void slerp(const NMP::Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Fast spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If qFrom and qTo are normalised then the result will be normalised (within error limits).
  ///        This is a fast alternative to the slerp function.
  NM_INLINE void fastSlerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Fast spherical linear interpolation between the local quaternion and the given target,
  ///        by distance t, t being in the range 0.0f .. 1.0f; result is stored in the local quaternion directly.
  ///        If both input quaternions are normalised then the result will be normalised (within error limits).
  ///        This is a fast alternative to the slerp function.
  NM_INLINE void fastSlerp(const NMP::Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Fast spherical linear interpolation between the two quaternions given, by distance t,
  ///        t being in the range 0.0f .. 1.0f; This version performs the fast slerp approximation if
  ///        the two quats are within the same semi-arc, and the standard slerp approximation for longest
  ///        path arc-rotation when the quats are not in the same semi-arc.
  NM_INLINE void quickSlerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t);

  /// \ingroup Maths
  /// \brief Calculate linear interpolation between the two quaternions given, by distance t,
  ///        t normally being in the range 0.0f .. 1.0f (but values outside this range are 
  ///        accepted too); result is stored in the local quaternion directly.
  ///        To produce a sensible result both input vectors should be normalised and similar.
  ///        The result is normalised.
  ///        This is potentially a faster alternative to slerping closely spaced quaternions.
  NM_INLINE void lerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Calculate linear interpolation between the local quaternion and the given target,
  ///        by distance t, t normally being in the range 0.0f .. 1.0f (but values outside this 
  ///        range are accepted too); result is stored in the local quaternion directly.
  ///        To produce a sensible result both input vectors should be normalised and similar.
  ///        The result is normalised.
  ///        This is potentially a faster alternative to slerping closely spaced quaternions.
  NM_INLINE void lerp(const NMP::Quat& qTo, float t, float fromDotTo);

  /// \ingroup Maths
  /// \brief Checks that the vector does not contain any QNAN or INF values.
  bool isValid() const;

  /// \ingroup Maths
  /// \brief Is this quaternion normalised (within error limits).
  NM_INLINE bool isNormal() const;

  /// \ingroup Maths
  /// \brief Is this quaternion normalised (within error limits) - checks if the magnitude squared
  ///        is within tolerance of 1.
  NM_INLINE bool isNormal(float tolerance) const;

  /// \ingroup Maths
  /// \brief Set the quaternion from a quaternion log vector
  NM_INLINE void exp(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Compute an approximation to the quaternion exponential map by using
  ///        a polynomial function to compute the cos and sin trigonometric functions.
  ///        The error in the sin and cos functions is distributed over the entire [0:2*pi]
  ///        range and has a maximum error of the order 2e-4.
  NM_INLINE void fastExp(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Convert the quaternion to a quaternion log vector
  NM_INLINE Vector3 log() const;

  /// \ingroup Maths
  /// \brief Computes the quaternion log vector w = (theta/2) * v by using a
  ///        polynomial function to approximate the atan trigonometric function. The error
  ///        in the atan trigonometric function is distributed over the entire [0:pi] range
  ///        and has a maximum error of the order 2e-4.
  NM_INLINE Vector3 fastLog() const;

} NMP_ALIGN_SUFFIX(16);

NM_INLINE static Quat QuatIdentity() { return Quat(Quat::kIdentity); }

} // namespace NMP

#include "NMPlatform/generic/NMQuat.inl"

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_QUAT_H
//----------------------------------------------------------------------------------------------------------------------
