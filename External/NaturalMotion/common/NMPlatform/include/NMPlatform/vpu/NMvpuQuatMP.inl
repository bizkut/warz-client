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

class Vector3MP;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \class NMP::vpu::QuatMP
///
/// \brief A class of four quats in soa format
//----------------------------------------------------------------------------------------------------------------------
class QuatMP
{
public:

  vector4_t x;
  vector4_t y;
  vector4_t z;
  vector4_t w;

  /// \brief  Default constructor
  ///
  NM_FORCEINLINE QuatMP() { };

  /// \brief  Copy a mp QuatMP
  ///
  NM_FORCEINLINE QuatMP(const QuatMP& quat);

  /// \brief  Construct a mp QuatMP from a Vector3MP and w elements
  ///
  NM_FORCEINLINE QuatMP(const Vector3MP& vec, const vector4_t& qw);

  /// \brief  Construct a mp QuatMP from x, y, z and w elements
  ///
  NM_FORCEINLINE QuatMP(const vector4_t& qx, const vector4_t& qy, const vector4_t& qz, const vector4_t& qw);

  /// \brief  Construct a mp QuatMP from x, y, z and w vectors in memory
  ///
  NM_FORCEINLINE QuatMP(const float* data);

  /// \brief  Duplicate one singular QuatMP
  ///
  NM_FORCEINLINE QuatMP(const vector4_t quat);

  /// \brief  Pack four singular Quats
  ///
  NM_FORCEINLINE void pack(const vector4_t& q0, const vector4_t& q1, const vector4_t& q2, const vector4_t& q3);

  /// \brief  Pack four singular Quats from memory
  ///
  NM_FORCEINLINE void pack(const float* data);

  /// \brief  Unpack four singular Quats
  ///
  NM_FORCEINLINE void unpack(vector4_t& q0, vector4_t& q1, vector4_t& q2, vector4_t& q3) const;

  /// \brief  Unpack four singular Quats to memory
  ///
  NM_FORCEINLINE void unpack(float* data) const;

  /// \brief  Overloads
  ///
  NM_FORCEINLINE QuatMP& operator =(const QuatMP& quat);
  NM_FORCEINLINE QuatMP operator -(const QuatMP& quat) const;
  NM_FORCEINLINE QuatMP operator -(const vector4_t& vec) const;
  NM_FORCEINLINE QuatMP operator -() const;
  NM_FORCEINLINE QuatMP operator +(const QuatMP& quat) const;
  NM_FORCEINLINE QuatMP operator +(const vector4_t& vec) const;
  NM_FORCEINLINE QuatMP operator *(const QuatMP& quat) const;
  NM_FORCEINLINE QuatMP operator *(const vector4_t& vec) const;

  NM_FORCEINLINE void operator -=(const QuatMP& quat);
  NM_FORCEINLINE void operator -=(const vector4_t& vec);
  NM_FORCEINLINE void operator +=(const QuatMP& quat);
  NM_FORCEINLINE void operator +=(const vector4_t& vec);
  NM_FORCEINLINE void operator *=(const vector4_t& quat);

  /// \brief  debug
  ///
  NM_FORCEINLINE vector4_t operator[] (int32_t i) const;

  /// \brief  Functions
  ///
  NM_FORCEINLINE void identity();
  NM_FORCEINLINE vector4_t dot(const QuatMP& quat) const;
  NM_FORCEINLINE vector4_t lengthSquared() const;
  NM_FORCEINLINE QuatMP negate(const vector4_t& mask) const;
  NM_FORCEINLINE QuatMP conjugate() const;
  NM_FORCEINLINE QuatMP merge(const QuatMP& quat, const vector4_t& mask) const;
  NM_FORCEINLINE vector4_t normalise();

  NM_FORCEINLINE Vector3MP rotateVector(vector4_t vec) const;
  NM_FORCEINLINE Vector3MP inverseRotateVector(vector4_t vec) const;

  /// \brief Rotate four vector3s by four quats
  NM_FORCEINLINE Vector3MP rotateVector(const Vector3MP& v) const;
  /// \brief Inverse-rotate four vector3s by four quats
  NM_FORCEINLINE Vector3MP inverseRotateVector(const Vector3MP& v) const;
  NM_FORCEINLINE void subtractiveBlend(const QuatMP& q0, const QuatMP& q1, const vector4_t& alpha);
  
  NM_FORCEINLINE QuatMP fastSlerp(const QuatMP &q0, const QuatMP &q1, const vector4_t &ta,  const vector4_t &fdt) const;
  NM_FORCEINLINE void interpBlend(const QuatMP &q0, const QuatMP &q1, const vector4_t &alpha);
  NM_FORCEINLINE void additiveBlend(const QuatMP &q0, const QuatMP &q1, const vector4_t &alpha);

  /// \brief  Conversions
  ///
  NM_FORCEINLINE void logAtan2(vector4_t& s, vector4_t& phi) const;
  NM_FORCEINLINE Vector3MP qLog(vector4_t& phi) const;
  NM_FORCEINLINE Vector3MP qLog() const; // rename the logs to show differences

};

//-----------------------------------------------------------------------------------------------------------------------

// copy
NM_FORCEINLINE QuatMP::QuatMP(const QuatMP& quat)
{
  x = quat.x;
  y = quat.y;
  z = quat.z;
  w = quat.w;
}

// construct
NM_FORCEINLINE QuatMP::QuatMP(const vector4_t& qx, const vector4_t& qy, const vector4_t& qz, const vector4_t& qw)
{
  x = qx;
  y = qy;
  z = qz;
  w = qw;
}

// mp dup
NM_FORCEINLINE QuatMP::QuatMP(const vector4_t quat)
{
  x = splatX(quat);
  y = splatY(quat);
  z = splatZ(quat);
  w = splatW(quat);
}

// load
NM_FORCEINLINE QuatMP::QuatMP(const float* data)
{
  load4v(data, x, y, z, w);
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void QuatMP::pack(const vector4_t& q0, const vector4_t& q1, const vector4_t& q2, const vector4_t& q3)
{
  x = q0;
  y = q1;
  z = q2;
  w = q3;
  transpose4v(x, y, z, w);
}

NM_FORCEINLINE void QuatMP::pack(const float* data)
{
  load4v(data, x, y, z, w);
  transpose4v(x, y, z, w);
}

NM_FORCEINLINE void QuatMP::unpack(vector4_t& q0, vector4_t& q1, vector4_t& q2, vector4_t& q3) const
{
  q0 = x;
  q1 = y;
  q2 = z;
  q3 = w;
  transpose4v(q0, q1, q2, q3);
}

NM_FORCEINLINE void QuatMP::unpack(float* data) const
{
  vector4_t v0, v1, v2, v3; unpack(v0, v1, v2, v3);
  store4v(data, v0, v1, v2, v3);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE QuatMP& QuatMP::operator =(const QuatMP& quat)
{
  x = quat.x;
  y = quat.y;
  z = quat.z;
  w = quat.w;
  return *this;
}

NM_FORCEINLINE QuatMP QuatMP::operator -(const QuatMP& quat) const
{
  return QuatMP(sub4f(x, quat.x), sub4f(y, quat.y), sub4f(z, quat.z), sub4f(w, quat.w));
}

NM_FORCEINLINE QuatMP QuatMP::operator -(const vector4_t& quat) const
{
  return QuatMP(sub4f(x, quat), sub4f(y, quat), sub4f(z, quat), sub4f(w, quat));
}

NM_FORCEINLINE QuatMP QuatMP::operator -() const
{
  return QuatMP(neg4f(x), neg4f(y), neg4f(z), neg4f(w));
}

NM_FORCEINLINE QuatMP QuatMP::operator +(const QuatMP& quat) const
{
  return QuatMP(add4f(x, quat.x), add4f(y, quat.y), add4f(z, quat.z), add4f(w, quat.w));
}

NM_FORCEINLINE QuatMP QuatMP::operator +(const vector4_t& quat) const
{
  return QuatMP(add4f(x, quat), add4f(y, quat), add4f(z, quat), add4f(w, quat));
}

NM_FORCEINLINE QuatMP QuatMP::operator *(const QuatMP& quat) const
{
  return QuatMP(add4f(add4f(mul4f(quat.w, x), mul4f(quat.x, w)), sub4f(mul4f(quat.z, y), mul4f(quat.y, z))),
                add4f(add4f(mul4f(quat.w, y), mul4f(quat.y, w)), sub4f(mul4f(quat.x, z), mul4f(quat.z, x))),
                add4f(add4f(mul4f(quat.w, z), mul4f(quat.z, w)), sub4f(mul4f(quat.y, x), mul4f(quat.x, y))),
                sub4f(sub4f(mul4f(quat.w, w), mul4f(quat.x, x)), add4f(mul4f(quat.y, y), mul4f(quat.z, z))));
}

NM_FORCEINLINE QuatMP QuatMP::operator *(const vector4_t& quat) const
{
  return QuatMP(mul4f(x, quat), mul4f(y, quat), mul4f(z, quat), mul4f(w, quat));
}

NM_FORCEINLINE void QuatMP::operator -=(const QuatMP& quat)
{
  x = sub4f(x, quat.x);
  y = sub4f(y, quat.y);
  z = sub4f(z, quat.z);
  w = sub4f(w, quat.w);
}

NM_FORCEINLINE void QuatMP::operator -=(const vector4_t& quat)
{
  x = sub4f(x, quat);
  y = sub4f(y, quat);
  z = sub4f(z, quat);
  w = sub4f(w, quat);
}

NM_FORCEINLINE void QuatMP::operator +=(const QuatMP& quat)
{
  x = add4f(x, quat.x);
  y = add4f(y, quat.y);
  z = add4f(z, quat.z);
  w = add4f(w, quat.w);
}

NM_FORCEINLINE void QuatMP::operator +=(const vector4_t& quat)
{
  x = add4f(x, quat);
  y = add4f(y, quat);
  z = add4f(z, quat);
  w = add4f(w, quat);
}

NM_FORCEINLINE void QuatMP::operator *=(const vector4_t& quat)
{
  x = mul4f(x, quat);
  y = mul4f(y, quat);
  z = mul4f(z, quat);
  w = mul4f(w, quat);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void QuatMP::identity()
{
  x = zero4f();
  y = x;
  z = x;
  w = one4f();
}

NM_FORCEINLINE vector4_t QuatMP::dot(const QuatMP& quat) const
{
  vector4_t t1, t2, t3, t4;
  t1 = mul4f(x, quat.x);
  t2 = mul4f(y, quat.y);
  t3 = mul4f(z, quat.z);
  t4 = mul4f(w, quat.w);
  return add4f(add4f(t1, t2), add4f(t3, t4));
}

NM_FORCEINLINE vector4_t QuatMP::lengthSquared() const
{
  return dot(*this);
}

NM_FORCEINLINE QuatMP QuatMP::negate(const vector4_t& mask) const
{
  return QuatMP(neg4f(x, mask), neg4f(y, mask), neg4f(z, mask), neg4f(w, mask));
}

NM_FORCEINLINE QuatMP QuatMP::conjugate() const
{
  return QuatMP(neg4f(x), neg4f(y), neg4f(z), w);
}

NM_FORCEINLINE QuatMP QuatMP::merge(const QuatMP& quat, const vector4_t& mask) const
{
  return QuatMP(sel4cmpMask(mask, x, quat.x), sel4cmpMask(mask, y, quat.y), sel4cmpMask(mask, z, quat.z), sel4cmpMask(mask, w, quat.w));
}

NM_FORCEINLINE vector4_t QuatMP::normalise()
{
  vector4_t lenSqr = lengthSquared();
  vector4_t mask = sel4cmpLT(lenSqr, fltEpsilon4f(), mask4i(), zero4f());

  vector4_t recipLen = rsqrt4f(sel4cmpMask(mask, one4f(), lenSqr));

  *this *= sel4cmpMask(mask, zero4f(), recipLen);

  w = sel4cmpMask(mask, one4f(), w);

  return mul4f(recipLen, lenSqr);
}

// debug
NM_FORCEINLINE vector4_t QuatMP::operator[] (int32_t i) const
{
  if (i == 0) return x;
  if (i == 1) return y;
  if (i == 2) return z;
  return w;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE QuatMP QuatMP::fastSlerp(const QuatMP& q0, const QuatMP& q1, const vector4_t& ta,  const vector4_t& fdt) const
{
  const vector4_t mOne  = one4f();
  vector4_t recipOnePlusFdt = rcp4f(add4f(mOne, fdt));

  vector4_t T  = sub4f(mOne, ta);
  vector4_t t2 = mul4f(ta, ta);
  vector4_t T2 = mul4f(T, T);

  // c1 =  1.570994357000f + (0.56429298590f + (-0.17836577170f + 0.043199493520f * fdt) * fdt) * fdt;
  // c3 = -0.646139638200f + (0.59456579360f + (0.08610323953f - 0.034651229280f * fdt) * fdt) * fdt;
  // c5 =  0.079498235210f + (-0.17304369310f + (0.10792795990f - 0.014393978010f * fdt) * fdt) * fdt;
  // c7 = -0.004354102836f + (0.01418962736f + (-0.01567189691f + 0.005848706227f * fdt) * fdt) * fdt;

  static const vector4_t Ca = set4f( 1.57099435700f, -0.64613963820f, 0.07949823521f, -0.004354102836f );
  static const vector4_t Cb = set4f( 0.56429298590f, 0.59456579360f, -0.17304369310f, 0.014189627360f );
  static const vector4_t Cc = set4f( -0.17836577170f, 0.08610323953f, 0.10792795990f, -0.015671896910f );
  static const vector4_t Cd = set4f( 0.04319949352f, -0.03465122928f, -0.01439397801f, 0.005848706227f );

  Vector4MP ff(fdt);
  Vector4MP cc = ff * (ff * (ff * Cd + Cc) + Cb) + Ca; cc.flip();
  // cc.x = all c1's, cc.y = all c3's, etc.

  vector4_t startWeights = mul4f(add4f(cc.x, mul4f(add4f(cc.y, mul4f(add4f(cc.z, mul4f(cc.w, T2)), T2)), T2)), mul4f(recipOnePlusFdt, T));
  vector4_t endWeights   = mul4f(add4f(cc.x, mul4f(add4f(cc.y, mul4f(add4f(cc.z, mul4f(cc.w, t2)), t2)), t2)), mul4f(recipOnePlusFdt, ta));

  return q0 * startWeights + q1 * endWeights;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void QuatMP::interpBlend(const QuatMP& q0, const QuatMP& q1, const vector4_t& alpha)
{
  vector4_t fromDotTo = q0.dot(q1);
  vector4_t fromDotToAbs = abs4f(fromDotTo);

  *this = fastSlerp(q0, q1.negate(fromDotTo), alpha, fromDotToAbs);
}

NM_FORCEINLINE void QuatMP::additiveBlend(const QuatMP& q0, const QuatMP& q1, const vector4_t& alpha)
{
  // Interpolate between identity and the adding quat.
  vector4_t mOne  = one4f();
  vector4_t mZero = zero4f();

  // fromDotTo = identityQuat.dot(q1);
  vector4_t fromDotTo = q1.w;
  vector4_t fromDotToAbs = abs4f(fromDotTo);

  QuatMP idQuat(mZero, mZero, mZero, mOne);
  *this = fastSlerp(idQuat, q1.negate(fromDotTo), alpha, fromDotToAbs) * q0;
}

NM_FORCEINLINE void QuatMP::subtractiveBlend(const QuatMP& q0, const QuatMP& q1, const vector4_t& alpha)
{
  // Interpolate between identity and the adding quat.
  vector4_t mOne  = one4f();
  vector4_t mZero = zero4f();

  // get the difference: source 1 - source 0
  QuatMP qdiff = q0 * q1.conjugate();

  // fromDotTo = identityQuat.dot(q1);
  vector4_t fromDotTo = qdiff.w;
  vector4_t fromDotToAbs = abs4f(fromDotTo);

  // identity
  QuatMP idQuat(mZero, mZero, mZero, mOne);

  // slerp between identity and (source 1 - source 0)
  *this = fastSlerp(idQuat, qdiff.negate(fromDotTo), alpha, fromDotToAbs);
}


//----------------------------------------------------------------------------------------------------------------------

/// \brief Helper function to compute the atan2 function approximation for the quat log map
NM_FORCEINLINE void QuatMP::logAtan2(vector4_t& s, vector4_t& phi) const
{
  static const vector4_t coefs = set4f( 0.999314747965817f, -0.322266683652863f, 0.14898078328198f, -0.040826575272239f );

  // Constants
  const vector4_t mPi_2 = set4f(1.57079632679489661923f);
  const vector4_t mZero = zero4f();
  const vector4_t mPi   = add4f(mPi_2, mPi_2);

  // Compute sin(phi) of the quat
  vector4_t s2 = add4f(add4f(mul4f(x, x), mul4f(y, y)), mul4f(z, z));
  s = sqrt4f(s2);

  // Compute an approximation to the trigonometric atan2 function by using
  // a polynomial function representation. The atan function is approximated by
  // using two curve pieces that divide the input value range between x = [-1:1].
  // This is possible because of the trig identity:
  ///
  /// atan(x) = x - x^3/3 + x^5/5 - x^7/7 ...  -1 < x < 1
  ///
  /// atan(x) = pi/2 - atan(1/x)              x < -1, x > 1
  ///
  /// The Taylors coefficients have been adjusted to distribute the error over the
  /// entire curve and has a maximum angle error of the order 2e-4. The returned
  /// angle is in the range phi = [0:pi]
  vector4_t c2  = mul4f(w, w);
  vector4_t kc  = sel4cmpLT(w, mZero, mPi, mZero);
  vector4_t msk = mask4cmpLT(c2, s2);

  vector4_t N = sel4cmpMask(msk, neg4f(w), s);
  vector4_t D = sel4cmpMask(msk, s, w);

  vector4_t offs = sel4cmpMask(msk, mPi_2, kc);

  // Compute the adjusted Taylor's polynomial for atan
  vector4_t localX = mul4f(N, rcp4f(D));
  vector4_t u = mul4f(localX, localX);
  vector4_t coefs0 = splatX(coefs);
  vector4_t coefs1 = splatY(coefs);
  vector4_t coefs2 = splatZ(coefs);
  vector4_t coefs3 = splatW(coefs);

  phi = madd4f(localX, madd4f(u, madd4f(u, madd4f(u, coefs3, coefs2), coefs1), coefs0), offs);
}

//----------------------------------------------------------------------------------------------------------------------
