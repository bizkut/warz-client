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

class QuatMP;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \class NMP::vpu::Vector3MP
///
/// \brief A class of four 3d vectors in soa format
//----------------------------------------------------------------------------------------------------------------------
class Vector3MP
{
public:

  vector4_t x;
  vector4_t y;
  vector4_t z;

  /// \brief  Default constructor
  ///
  NM_FORCEINLINE Vector3MP() { };

  /// \brief  Copy a Vector3MP
  ///
  NM_FORCEINLINE Vector3MP(const Vector3MP& vec);

  /// \brief  Construct a Vector3MP from x, y, and z vectors
  ///
  NM_FORCEINLINE Vector3MP(const vector4_t& vx, const vector4_t& vy, const vector4_t& vz);

  /// \brief  Construct a Vector3MP from x, y, and z vectors in memory
  ///
  NM_FORCEINLINE Vector3MP(const float* data);

  /// \brief  Duplicate one singular Vector3MP
  ///
  NM_FORCEINLINE Vector3MP(const vector4_t vec);

  /// \brief  Pack four singular Vector3s
  ///
  NM_FORCEINLINE void pack(const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3);

  /// \brief  Pack four singular Vector3s from memory
  ///
  NM_FORCEINLINE void pack(const float* data);

  /// \brief  Unpack four singular Vector3s
  ///
  NM_FORCEINLINE void unpack(vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3) const;

  /// \brief  Unpack four singular Vector3s to memory
  ///
  NM_FORCEINLINE void unpack(float* data) const;

  /// \brief  overloads
  ///
  NM_FORCEINLINE Vector3MP& operator =(const Vector3MP& vec);
  NM_FORCEINLINE Vector3MP operator -(const Vector3MP& vec) const;
  NM_FORCEINLINE Vector3MP operator -(const vector4_t& vec) const;
  NM_FORCEINLINE Vector3MP operator -() const;
  NM_FORCEINLINE Vector3MP operator +(const Vector3MP& vec) const;
  NM_FORCEINLINE Vector3MP operator +(const vector4_t& vec) const;
  NM_FORCEINLINE Vector3MP operator *(const Vector3MP& vec) const;
  NM_FORCEINLINE Vector3MP operator *(const vector4_t& vec) const;

  NM_FORCEINLINE void operator -= (const Vector3MP& vec);
  NM_FORCEINLINE void operator -= (const vector4_t& vec);
  NM_FORCEINLINE void operator += (const Vector3MP& vec);
  NM_FORCEINLINE void operator += (const vector4_t& vec);
  NM_FORCEINLINE void operator *= (const Vector3MP& vec);
  NM_FORCEINLINE void operator *= (const vector4_t& vec);

  NM_FORCEINLINE vector4_t operator[] (int32_t i) const;

  /// \brief functions
  ///
  NM_FORCEINLINE void mulAdd(const Vector3MP& va, const Vector3MP& vb, const Vector3MP& vadd);
  NM_FORCEINLINE void lerp(const Vector3MP& vecSrcA, const Vector3MP& vecSrcB, const vector4_t& interpolant);
  NM_FORCEINLINE void interp(const Vector3MP& vecSrcA, const Vector3MP& vecSrcB, const vector4_t& interpolant);
  NM_FORCEINLINE void subtract(const Vector3MP& vecSrcA, const Vector3MP& vecSrcB, const vector4_t& interpolant);
  NM_FORCEINLINE void uniformQuantisation(const Vector3MP& qMin, const Vector3MP& qMax, const Vector3MP& prec);
  NM_FORCEINLINE void interpCubicBezier(const Vector3MP& p0, const Vector3MP& p1, const Vector3MP& p2, const Vector3MP& p3, const Vector4MP& basis4);

  NM_FORCEINLINE Vector3MP cross(const Vector3MP& v) const;
  NM_FORCEINLINE Vector3MP cross(const vector4_t& vec) const;
  NM_FORCEINLINE vector4_t dot(const Vector3MP& vec) const;
  NM_FORCEINLINE vector4_t dot(const vector4_t& vec) const;
  NM_FORCEINLINE Vector3MP merge(const Vector3MP& vec, const vector4_t& mask) const;
  NM_FORCEINLINE vector4_t lengthSquared() const;
  NM_FORCEINLINE vector4_t normalise();

  NM_FORCEINLINE Vector3MP applyQuatRotation(vector4_t quat) const;
  NM_FORCEINLINE Vector3MP applyInverseQuatRotation(vector4_t quat) const;

  NM_FORCEINLINE QuatMP convertTQARotVecToQuat() const;
  NM_FORCEINLINE QuatMP convertTQARotVecToQuat(const vector4_t& wNegMask) const;

  NM_FORCEINLINE vector4_t toAngleAxis();
  NM_FORCEINLINE QuatMP qExp(const vector4_t phi) const;
  NM_FORCEINLINE QuatMP qExp() const;

  NM_FORCEINLINE Vector3MP uniformQuantisation16(const Vector3MP& qMin, const Vector3MP& qMax) const;
  NM_FORCEINLINE Vector3MP uniformQuantisation8(const Vector3MP& qMin, const Vector3MP& qMax) const;

  /// \brief  Convert int elements to float elements
  ///
  NM_FORCEINLINE void convert2f();

};

//-----------------------------------------------------------------------------------------------------------------------

// copy
NM_FORCEINLINE Vector3MP::Vector3MP(const Vector3MP& vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
}

// construct
NM_FORCEINLINE Vector3MP::Vector3MP(const vector4_t& vx, const vector4_t& vy, const vector4_t& vz)
{
  x = vx;
  y = vy;
  z = vz;
}

// mp dup
NM_FORCEINLINE Vector3MP::Vector3MP(const vector4_t vec)
{
  x = splatX(vec);
  y = splatY(vec);
  z = splatZ(vec);
}

// load
NM_FORCEINLINE Vector3MP::Vector3MP(const float* data)
{
  x = load4f(data);
  y = load4f(data + 4);
  z = load4f(data + 8);
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector3MP::pack(const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3)
{
  x = v0;
  y = v1;
  z = v2;
  vector4_t v3c = v3;
  transpose4v(x, y, z, v3c);
}

NM_FORCEINLINE void Vector3MP::pack(const float* data)
{
  vector4_t vw;
  load4v(data, x, y, z, vw);
  transpose4v(x, y, z, vw);
}

NM_FORCEINLINE void Vector3MP::unpack(vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3) const
{
  v0 = x;
  v1 = y;
  v2 = z;
  v3 = zero4f();
  transpose4v(v0, v1, v2, v3);
}

NM_FORCEINLINE void Vector3MP::unpack(float* data) const
{
  vector4_t v0, v1, v2, v3; unpack(v0, v1, v2, v3);
  store4v(data, v0, v1, v2, v3);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Vector3MP& Vector3MP::operator =(const Vector3MP& vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  return *this;
}

NM_FORCEINLINE Vector3MP Vector3MP::operator -(const Vector3MP& vec) const
{
  return Vector3MP(sub4f(x, vec.x), sub4f(y, vec.y), sub4f(z, vec.z));
}

NM_FORCEINLINE Vector3MP Vector3MP::operator -(const vector4_t& vec) const
{
  return Vector3MP(sub4f(x, vec), sub4f(y, vec), sub4f(z, vec));
}

NM_FORCEINLINE Vector3MP Vector3MP::operator -() const
{
  return Vector3MP(neg4f(x), neg4f(y), neg4f(z));
}

NM_FORCEINLINE Vector3MP Vector3MP::operator +(const Vector3MP& vec) const
{
  return Vector3MP(add4f(x, vec.x), add4f(y, vec.y), add4f(z, vec.z));
}

NM_FORCEINLINE Vector3MP Vector3MP::operator +(const vector4_t& vec) const
{
  return Vector3MP(add4f(x, vec), add4f(y, vec), add4f(z, vec));
}

NM_FORCEINLINE Vector3MP Vector3MP::operator *(const Vector3MP& vec) const
{
  return Vector3MP(mul4f(x, vec.x), mul4f(y, vec.y), mul4f(z, vec.z));
}

NM_FORCEINLINE Vector3MP Vector3MP::operator *(const vector4_t& vec) const
{
  return Vector3MP(mul4f(x, vec), mul4f(y, vec), mul4f(z, vec));
}

NM_FORCEINLINE void Vector3MP::operator -=(const Vector3MP& vec)
{
  x = sub4f(x, vec.x);
  y = sub4f(y, vec.y);
  z = sub4f(z, vec.z);
}

NM_FORCEINLINE void Vector3MP::operator -=(const vector4_t& vec)
{
  x = sub4f(x, vec);
  y = sub4f(y, vec);
  z = sub4f(z, vec);
}

NM_FORCEINLINE void Vector3MP::operator +=(const Vector3MP& vec)
{
  x = add4f(x, vec.x);
  y = add4f(y, vec.y);
  z = add4f(z, vec.z);
}

NM_FORCEINLINE void Vector3MP::operator +=(const vector4_t& vec)
{
  x = add4f(x, vec);
  y = add4f(y, vec);
  z = add4f(z, vec);
}

NM_FORCEINLINE void Vector3MP::operator *=(const Vector3MP& vec)
{
  x = mul4f(x, vec.x);
  y = mul4f(y, vec.y);
  z = mul4f(z, vec.z);
}

NM_FORCEINLINE void Vector3MP::operator *=(const vector4_t& vec)
{
  x = mul4f(x, vec);
  y = mul4f(y, vec);
  z = mul4f(z, vec);
}

// debug
NM_FORCEINLINE vector4_t Vector3MP::operator[] (int32_t i) const
{
  if (i == 0) return x;
  if (i == 1) return y;
  return z;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector3MP::mulAdd(const Vector3MP& va, const Vector3MP& vb, const Vector3MP& vadd)
{
  *this = (va * vb) + vadd;
}

NM_FORCEINLINE void Vector3MP::lerp(const Vector3MP& vecSrcA, const Vector3MP& vecSrcB, const vector4_t& interpolant)
{
  *this = (vecSrcB - vecSrcA) * interpolant + vecSrcA;
}

NM_FORCEINLINE void Vector3MP::interp(const Vector3MP& vecSrcA, const Vector3MP& vecSrcB, const vector4_t& interpolant)
{
  *this = vecSrcA + (vecSrcB * interpolant);
}

NM_FORCEINLINE void Vector3MP::subtract(const Vector3MP& vecSrcA, const Vector3MP& vecSrcB, const vector4_t& interpolant)
{
  *this =  (vecSrcA - vecSrcB) * interpolant;
}

NM_FORCEINLINE Vector3MP Vector3MP::cross(const Vector3MP& v) const
{
  vector4_t tx = sub4f(mul4f(y, v.z), mul4f(z, v.y));
  vector4_t ty = sub4f(mul4f(z, v.x), mul4f(x, v.z));
  vector4_t tz = sub4f(mul4f(x, v.y), mul4f(y, v.x));

  return Vector3MP(tx, ty, tz);
}

NM_FORCEINLINE Vector3MP Vector3MP::cross(const vector4_t& vec) const
{
  vector4_t tx = sub4f(mul4f(y, splatZ(vec)), mul4f(z, splatY(vec)));
  vector4_t ty = sub4f(mul4f(z, splatX(vec)), mul4f(x, splatZ(vec)));
  vector4_t tz = sub4f(mul4f(x, splatY(vec)), mul4f(y, splatX(vec)));

  return Vector3MP(tx, ty, tz);
}

NM_FORCEINLINE vector4_t Vector3MP::dot(const Vector3MP& vec) const
{
  vector4_t t1, t2, t3;
  t1 = mul4f(x, vec.x);
  t2 = mul4f(y, vec.y);
  t3 = mul4f(z, vec.z);
  return add4f(add4f(t1, t2), t3);
}

NM_FORCEINLINE vector4_t Vector3MP::dot(const vector4_t& vec) const
{
  vector4_t t1, t2, t3;
  t1 = mul4f(x, splatX(vec));
  t2 = mul4f(y, splatY(vec));
  t3 = mul4f(z, splatZ(vec));
  return add4f(add4f(t1, t2), t3);
}

NM_FORCEINLINE Vector3MP Vector3MP::merge(const Vector3MP& vec, const vector4_t& mask) const
{
  return Vector3MP(sel4cmpMask(mask, x, vec.x), sel4cmpMask(mask, y, vec.y), sel4cmpMask(mask, z, vec.z));
}

NM_FORCEINLINE vector4_t Vector3MP::lengthSquared() const
{
  return dot(*this);
}

NM_FORCEINLINE vector4_t Vector3MP::normalise()
{
  const vector4_t eps = fltEpsilon4f();

  vector4_t lenSqr = lengthSquared();
  vector4_t mask = mask4cmpLT(lenSqr, eps);

  vector4_t recipLen = rsqrt4f(sel4cmpMask(mask, one4f(), lenSqr));

  *this *= sel4cmpMask(mask, zero4f(), recipLen);

  x = sel4cmpMask(mask, one4f(), x);

  return mul4f(recipLen, lenSqr);
}

NM_FORCEINLINE vector4_t Vector3MP::toAngleAxis()
{
  const vector4_t mOne = one4f();
  const vector4_t quatEps = fltEpsilon4f();

  // Compute the angle
  vector4_t mag2 = lengthSquared();
  vector4_t phi = sqrt4f(mag2);

  // Compute the axis. We don't care if the axis is
  // not normalised in the special case where phi = 0.
  vector4_t fac = sel4cmpLT(phi, quatEps, mOne, phi);
  *this *= rcp4f(fac);

  return phi;
}

// rethink this...
NM_FORCEINLINE void Vector3MP::interpCubicBezier(
  const Vector3MP& p0,
  const Vector3MP& p1,
  const Vector3MP& p2,
  const Vector3MP& p3,
  const Vector4MP& basis4) // switch to matrix44?
{
  *this = p0 * basis4.x + p1 * basis4.y + p2 * basis4.z + p3 * basis4.w;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector3MP::uniformQuantisation(const Vector3MP& qMin, const Vector3MP& qMax, const Vector3MP& prec)
{
  x = uniformQuantisationPrec(qMin.x, qMax.x, prec.x);
  y = uniformQuantisationPrec(qMin.y, qMax.y, prec.y);
  z = uniformQuantisationPrec(qMin.z, qMax.z, prec.z);
}

NM_FORCEINLINE Vector3MP Vector3MP::uniformQuantisation16(const Vector3MP& qMin, const Vector3MP& qMax) const
{
  vector4_t stepSize = set4f(1.0f / 65535); // 1 / (2^16 - 1)
  return (qMax - qMin) * stepSize;
}

NM_FORCEINLINE Vector3MP Vector3MP::uniformQuantisation8(const Vector3MP& qMin, const Vector3MP& qMax) const
{
  vector4_t stepSize = set4f(1.0f / 255); // 1 / (2^8 - 1)
  return (qMax - qMin) * stepSize;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector3MP::convert2f()
{
  x = c2float4i(x);
  y = c2float4i(y);
  z = c2float4i(z);
}

//----------------------------------------------------------------------------------------------------------------------

