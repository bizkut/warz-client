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

//-----------------------------------------------------------------------------------------------------------------------
// vpu multiplex functions
//-----------------------------------------------------------------------------------------------------------------------

// Vector3MP --> QuatMP

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief Construct a QuatMP from a Vector3MP and a vector of 4 w components
///
///

NM_FORCEINLINE QuatMP::QuatMP(const Vector3MP& vec, const vector4_t& qw)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = qw;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief Convert 4 tan quarter angle rotation vectors into quaternions
///
///
NM_FORCEINLINE QuatMP Vector3MP::convertTQARotVecToQuat() const
{
  const vector4_t mOne  = one4f();

  vector4_t mag2 = lengthSquared();
  vector4_t fact = rcp4f(add4f(mOne, mag2)); fact = add4f(fact, fact);

  vector4_t qw = mul4f(sub4f(mOne, mag2), half4f());

  return QuatMP(x, y, z, qw) * fact;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief Convert four loads of tan quarter angle rotation vectors back to quaternions
/// in the packed format that lie in the w < 0 or w >= 0 hemisphere specified by the wNegs flags.
///
/// A tan quarter angle rotation vector can safely encode the set of quaternions that lie
/// in the positive hemisphere. If we need to recover the original value of the quaternion
/// without ambiguity, we need to keep track of the sign of the hemisphere: sign(w).
///
/// \param wNegMask - A vector of four integer masks corresponding to the sign of w
///                   All zeros - if sign(w) >= 0
///                   All ones - if sign(w) < 0
NM_FORCEINLINE QuatMP Vector3MP::convertTQARotVecToQuat(const vector4_t& wNegMask) const
{
  const vector4_t mOne  = one4f();
  vector4_t mag2 = lengthSquared();
  vector4_t fact = rcp4f(add4f(mOne, mag2)); fact = neg4f(add4f(fact, fact), wNegMask);
  vector4_t qw = mul4f(sub4f(mOne, mag2), half4f());

  return QuatMP(*this, qw) * fact;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four loads of the quaternion exponential map:
/// q = qExp(phi, a),
/// where (phi, a) is the angle and axis representation of the rotation vector.
///       q = [sin(phi) * a, cos(phi)]
///
/// \param phi  - On input is the angle components phi = theta/2 of the log vector
NM_FORCEINLINE QuatMP Vector3MP::qExp(const vector4_t phi) const
{
  // Compute an approximation to sin and cos
  vector4_t s, qw; fastSinCos4f(phi, s, qw);

  // Compute the quaternion
  return QuatMP(*this * s, qw);
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four loads of the quaternion exponential map:
/// q = qExp(v),
/// where v = (theta/2) * a, and a is the unit axis 3-vector, theta is the rotation angle scalar
///       q = [sin(theta/2) * a, cos(theta/2)]
///
NM_FORCEINLINE QuatMP Vector3MP::qExp() const
{
  // Compute the angle and axis representation of the quat log vector
  Vector3MP qvec(*this); vector4_t phi = qvec.toAngleAxis();

  // Compute a approximation to sin and cos
  vector4_t s, qw; fastSinCos4f(phi, s, qw);

  // Compute the quaternion
  return QuatMP(qvec * s, qw);
}

//-----------------------------------------------------------------------------------------------------------------------

// QuatMP --> Vector3MP
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute the inverse rotations of four vectors by a given quaternion
///
NM_FORCEINLINE Vector3MP Vector3MP::applyInverseQuatRotation(vector4_t quat) const
{
  Vector3MP result;
  Vector3MP qv(quat);
  vector4_t sw = neg4f(splatW(quat));

  result  = qv.cross(*this);
  result *= add4f(sw, sw); // *= (w * -2.0f);

  // += vec * (2.0f * (w*w) - 1.0f)
  vector4_t tv = mul4f(sw, sw);
  tv = add4f(tv, tv);
  tv = sub4f(tv, one4f());
  result += *this * tv;

  // += qv * (qv.dot(v) * 2.0f)
  tv = qv.dot(*this);
  tv = add4f(tv, tv);
  result += (qv * tv);

  return result;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four vector rotations by a given quaternion
///
NM_FORCEINLINE Vector3MP Vector3MP::applyQuatRotation(vector4_t quat) const
{
  Vector3MP result;
  Vector3MP qv(quat);
  vector4_t sw = splatW(quat);

  result  = qv.cross(*this);
  result *= add4f(sw, sw); // *= (w * 2.0f);

  // += vec * (2.0f * (w*w) - 1.0f)
  vector4_t tv = mul4f(sw, sw);
  tv = add4f(tv, tv);
  tv = sub4f(tv, one4f());
  result += *this * tv;

  // += qv * (qv.dot(v) * 2.0f)
  tv = qv.dot(*this);
  tv = add4f(tv, tv);
  result += (qv * tv);

  return result;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four rotations of a vector by the given four quaternions
///
NM_FORCEINLINE Vector3MP QuatMP::rotateVector(vector4_t vec) const
{
  Vector3MP result;
  Vector3MP qv(x, y, z);

  result  = qv.cross(vec);
  result *= add4f(w, w); // *= (w * 2.0f);

  // += vec * (2.0f * (w*w) - 1.0f)
  vector4_t tv = mul4f(w, w);
  tv = add4f(tv, tv);
  tv = sub4f(tv, one4f());
  result += mul4f(vec, tv);

  // += qv * (qv.dot(v) * 2.0f)
  tv = qv.dot(vec);
  tv = add4f(tv, tv);
  result += (qv * tv);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// A function to compute four rotations of four vectors by the given four quaternions
///
NM_FORCEINLINE Vector3MP QuatMP::rotateVector(const Vector3MP& vec) const
{
  Vector3MP result;
  const Vector3MP& qv = *(const Vector3MP*)((const void*)this);

  result  = qv.cross(vec);
  result *= add4f(w, w); // *= (w * 2.0f);

  // += vec * (2.0f * (w*w) - 1.0f)
  vector4_t tv = mul4f(w, w);
  tv = add4f(tv, tv);
  tv = sub4f(tv, one4f());
  result += vec * tv; 

  // += qv * (qv.dot(v) * 2.0f)
  tv = qv.dot(vec);
  tv = add4f(tv, tv);
  result += qv * tv;

  return result;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four inverse rotations of a vector by the given four quaternions
/// 
NM_FORCEINLINE Vector3MP QuatMP::inverseRotateVector(vector4_t vec) const
{
  Vector3MP result;
  Vector3MP qv(x, y, z);
  vector4_t nw = neg4f(w);

  result  = qv.cross(vec);
  result *= add4f(nw, nw); // *= (w * -2.0f);

  // += vec * (2.0f * (w*w) - 1.0f)
  vector4_t tv = mul4f(w, w);
  tv = add4f(tv, tv);
  tv = sub4f(tv, one4f());
  result += mul4f(vec, tv);

  // += qv * (qv.dot(v) * 2.0f)
  tv = qv.dot(vec);
  tv = add4f(tv, tv);
  result += (qv * tv);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// A function to compute four inverse rotations of four vectors by the given four quaternions
///
NM_FORCEINLINE Vector3MP QuatMP::inverseRotateVector(const Vector3MP& vec) const
{
  Vector3MP result;
  const Vector3MP& qv = *(const Vector3MP*)((const void*)this);
  vector4_t nw = neg4f(w);

  result = qv.cross(vec);
  result *= add4f(nw, nw); // *= (w * -2.0f);

  // += vec * (2.0f * (w*w) - 1.0f)
  vector4_t tv = mul4f(w, w);
  tv = add4f(tv, tv);
  tv = sub4f(tv, one4f());
  result += vec * tv; 

  // += qv * (qv.dot(v) * 2.0f)
  tv = qv.dot(vec);
  tv = add4f(tv, tv);
  result += qv * tv;

  return result;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four loads of the quaternion log map in the packed format:
/// v = qLog(q), where
///       v = (theta/2) * a, and a is the unit axis 3-vector, theta is the rotation angle scalar
///       q = [sin(theta/2) * a, cos(theta/2)]
///
NM_FORCEINLINE Vector3MP QuatMP::qLog() const
{
  // Constants
  const vector4_t mZero = zero4f();
  const vector4_t mOne = one4f();
  const vector4_t quatEps = fltEpsilon4f();

  // Compute the quat log atan2 function approximation
  vector4_t s, phi; logAtan2(s, phi);

  // Compute the multiplication factor: fastAtan2(s, w) / s;
  vector4_t fac;
  fac = sel4cmpLT(s, quatEps, mOne, s);
  fac = mul4f(phi, rcp4f(fac));
  fac = sel4cmpLT(s, quatEps, mZero, fac);

  // Compute the quat log vec
  return Vector3MP(x, y, z) * fac;
}

//-----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \brief A function to compute four loads of the quaternion log map in the packed format:
/// v = qLog(q), where
///       v = (theta/2) * a, and a is the unit axis 3-vector, theta is the rotation angle scalar
///       q = [sin(theta/2) * a, cos(theta/2)]
///
NM_FORCEINLINE Vector3MP QuatMP::qLog(vector4_t& phi) const
{
  // Constants
  const vector4_t mOne = one4f();
  const vector4_t quatEps = fltEpsilon4f();

  // Compute the quat log atan2 function approximation
  vector4_t s; logAtan2(s, phi);

  // Normalize the axis vector. We don't care if the axis is not normalised in the
  // special case where phi = 0.
  vector4_t fac = sel4cmpLT(s, quatEps, mOne, s);
  fac = rcp4f(fac);

  return Vector3MP(x, y, z) * fac;
}

//-----------------------------------------------------------------------------------------------------------------------
