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
/// \ingroup NaturalMotionVPUMP
/// \class NMP::vpu::Vector4MP
///
/// \brief A class of four 4d vectors in soa format
//----------------------------------------------------------------------------------------------------------------------
class Vector4MP
{
public:

  vector4_t x;
  vector4_t y;
  vector4_t z;
  vector4_t w;

  /// \brief  Default constructor
  ///
  NM_FORCEINLINE Vector4MP() { };

  /// \brief  Copy a Vector4MP
  ///
  NM_FORCEINLINE Vector4MP(const Vector4MP& vec);

  /// \brief  Construct a Vector4MP from x, y, z and w vectors
  ///
  NM_FORCEINLINE Vector4MP(const vector4_t& vx, const vector4_t& vy, const vector4_t& vz, const vector4_t& vw);

  /// \brief  Duplicate one singular Vector4MP
  ///
  NM_FORCEINLINE Vector4MP(const vector4_t vec);

  /// \brief  Pack four singular Vector4s
  ///
  NM_FORCEINLINE void pack(const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3);

  /// \brief  Unpack four singular Vector4s
  ///
  NM_FORCEINLINE void unpack(vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3) const;

  /// \brief  Transpose as matrix
  ///
  NM_FORCEINLINE void flip();

  /// \brief  overloads
  ///
  NM_FORCEINLINE Vector4MP& operator =(const Vector4MP& vec);
  NM_FORCEINLINE Vector4MP operator -(const Vector4MP& vec) const;
  NM_FORCEINLINE Vector4MP operator -(const vector4_t& vec) const;
  NM_FORCEINLINE Vector4MP operator -() const;
  NM_FORCEINLINE Vector4MP operator +(const Vector4MP& vec) const;
  NM_FORCEINLINE Vector4MP operator +(const vector4_t& vec) const;
  NM_FORCEINLINE Vector4MP operator *(const Vector4MP& vec) const;
  NM_FORCEINLINE Vector4MP operator *(const vector4_t& vec) const;

  NM_FORCEINLINE void operator -= (const Vector4MP& vec);
  NM_FORCEINLINE void operator -= (const vector4_t& vec);
  NM_FORCEINLINE void operator += (const Vector4MP& vec);
  NM_FORCEINLINE void operator += (const vector4_t& vec);
  NM_FORCEINLINE void operator *= (const Vector4MP& vec);
  NM_FORCEINLINE void operator *= (const vector4_t& vec);

  /// \brief  functions
  ///
  NM_FORCEINLINE void mulAdd(const Vector4MP& va, const Vector4MP& vb, const Vector4MP& vadd);
  NM_FORCEINLINE void lerp(const Vector4MP& vecSrcA, const Vector4MP& vecSrcB, const vector4_t& interpolant);
  NM_FORCEINLINE void interp(const Vector4MP& vecSrcA, const Vector4MP& vecSrcB, const vector4_t& interpolant);
  NM_FORCEINLINE void subtract(const Vector4MP& vecSrcA, const Vector4MP& vecSrcB, const vector4_t& interpolant);
  NM_FORCEINLINE Vector4MP merge(const Vector4MP& vec, const vector4_t& mask) const;
  NM_FORCEINLINE vector4_t lengthSquared() const;

  /// \brief  Convert int elements to float elements
  ///
  NM_FORCEINLINE void convert2f();

};

//-----------------------------------------------------------------------------------------------------------------------

// copy
NM_FORCEINLINE Vector4MP::Vector4MP(const Vector4MP& vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = vec.w;
}

// construct
NM_FORCEINLINE Vector4MP::Vector4MP(const vector4_t& vx, const vector4_t& vy, const vector4_t& vz, const vector4_t& vw)
{
  x = vx;
  y = vy;
  z = vz;
  w = vw;
}

// mp dup
NM_FORCEINLINE Vector4MP::Vector4MP(const vector4_t vec)
{
  x = splatX(vec);
  y = splatY(vec);
  z = splatZ(vec);
  w = splatW(vec);
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector4MP::pack(const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3)
{
  x = v0;
  y = v1;
  z = v2;
  w = v3;
  transpose4v(x, y, z, w);
}

NM_FORCEINLINE void Vector4MP::unpack(vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3) const
{
  v0 = x;
  v1 = y;
  v2 = z;
  v3 = w;
  transpose4v(v0, v1, v2, v3);
}

NM_FORCEINLINE void Vector4MP::flip()
{
  transpose4v(x, y, z, w);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Vector4MP& Vector4MP::operator =(const Vector4MP& vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = vec.w;
  return *this;
}

NM_FORCEINLINE Vector4MP Vector4MP::operator -(const Vector4MP& vec) const
{
  return Vector4MP(sub4f(x, vec.x), sub4f(y, vec.y), sub4f(z, vec.z), sub4f(w, vec.w));
}

NM_FORCEINLINE Vector4MP Vector4MP::operator -(const vector4_t& vec) const
{
  return Vector4MP(sub4f(x, vec), sub4f(y, vec), sub4f(z, vec), sub4f(w, vec));
}

NM_FORCEINLINE Vector4MP Vector4MP::operator -() const
{
  return Vector4MP(neg4f(x), neg4f(y), neg4f(z), neg4f(w));
}

NM_FORCEINLINE Vector4MP Vector4MP::operator +(const Vector4MP& vec) const
{
  return Vector4MP(add4f(x, vec.x), add4f(y, vec.y), add4f(z, vec.z), add4f(w, vec.w));
}

NM_FORCEINLINE Vector4MP Vector4MP::operator +(const vector4_t& vec) const
{
  return Vector4MP(add4f(x, vec), add4f(y, vec), add4f(z, vec), add4f(w, vec));
}

NM_FORCEINLINE Vector4MP Vector4MP::operator *(const Vector4MP& vec) const
{
  return Vector4MP(mul4f(x, vec.x), mul4f(y, vec.y), mul4f(z, vec.z), mul4f(w, vec.w));
}

NM_FORCEINLINE Vector4MP Vector4MP::operator *(const vector4_t& vec) const
{
  return Vector4MP(mul4f(x, vec), mul4f(y, vec), mul4f(z, vec), mul4f(w, vec));
}

NM_FORCEINLINE void Vector4MP::operator -=(const Vector4MP& vec)
{
  x = sub4f(x, vec.x);
  y = sub4f(y, vec.y);
  z = sub4f(z, vec.z);
  w = sub4f(w, vec.w);
}

NM_FORCEINLINE void Vector4MP::operator -=(const vector4_t& vec)
{
  x = sub4f(x, vec);
  y = sub4f(y, vec);
  z = sub4f(z, vec);
  w = sub4f(w, vec);
}

NM_FORCEINLINE void Vector4MP::operator +=(const Vector4MP& vec)
{
  x = add4f(x, vec.x);
  y = add4f(y, vec.y);
  z = add4f(z, vec.z);
  w = add4f(w, vec.w);
}

NM_FORCEINLINE void Vector4MP::operator +=(const vector4_t& vec)
{
  x = add4f(x, vec);
  y = add4f(y, vec);
  z = add4f(z, vec);
  w = add4f(w, vec);
}

NM_FORCEINLINE void Vector4MP::operator *=(const Vector4MP& vec)
{
  x = mul4f(x, vec.x);
  y = mul4f(y, vec.y);
  z = mul4f(z, vec.z);
  w = mul4f(w, vec.w);
}

NM_FORCEINLINE void Vector4MP::operator *=(const vector4_t& vec)
{
  x = mul4f(x, vec);
  y = mul4f(y, vec);
  z = mul4f(z, vec);
  w = mul4f(w, vec);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector4MP::mulAdd(const Vector4MP& va, const Vector4MP& vb, const Vector4MP& vadd)
{
  *this = (va * vb) + vadd;
}

NM_FORCEINLINE void Vector4MP::lerp(const Vector4MP& vecSrcA, const Vector4MP& vecSrcB, const vector4_t& interpolant)
{
  *this = (vecSrcB - vecSrcA) * interpolant + vecSrcA;
}

NM_FORCEINLINE void Vector4MP::interp(const Vector4MP& vecSrcA, const Vector4MP& vecSrcB, const vector4_t& interpolant)
{
  *this = vecSrcA + (vecSrcB * interpolant);
}

NM_FORCEINLINE void Vector4MP::subtract(const Vector4MP& vecSrcA, const Vector4MP& vecSrcB, const vector4_t& interpolant)
{
  *this = vecSrcA - (vecSrcB * interpolant);
}

NM_FORCEINLINE Vector4MP Vector4MP::merge(const Vector4MP& vec, const vector4_t& mask) const
{
  return Vector4MP(sel4cmpMask(mask, x, vec.x), sel4cmpMask(mask, y, vec.y), sel4cmpMask(mask, z, vec.z), sel4cmpMask(mask, w, vec.w));
}

NM_FORCEINLINE vector4_t Vector4MP::lengthSquared() const
{
  Vector4MP tmp(*this * *this);
  return add4f(add4f(add4f(tmp.x, tmp.y), tmp.z), tmp.w);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Vector4MP::convert2f()
{
  x = c2float4i(x);
  y = c2float4i(y);
  z = c2float4i(z);
  w = c2float4i(w);
}

//----------------------------------------------------------------------------------------------------------------------
