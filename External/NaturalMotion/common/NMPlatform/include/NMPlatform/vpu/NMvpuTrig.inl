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
/// \defgroup NaturalMotionVPUTrig Trigonometric Vector Intrinsic Functions.
/// \ingroup NaturalMotionVPU
///
/// \brief Trigonometric Vectorized Functions
//----------------------------------------------------------------------------------------------------------------------

#if defined(NM_HOST_X360)
//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t cos4f(const vector4_t d)
{
  static const vector4_t A = neg4f(half4f());
  static const vector4_t K = { 1.0f / 24.0f, -1.0f / 720.0f, 1.0f / 40320.0f, -1.0f / 3628800.0f };

  const vector4_t B = splatX(K);
  const vector4_t C = splatY(K);
  const vector4_t D = splatZ(K);
  const vector4_t E = splatW(K);

  const vector4_t d2 = mul4f(d, d);
  const vector4_t t1 = madd4f(d2, A, one4f());
  const vector4_t t3 = madd4f(d2, E, D);
  const vector4_t t2 = madd4f(d2, C, B);
  const vector4_t t4 = madd4f(d2, t3, t2);
  const vector4_t d4 = mul4f(d2, d2);

  return madd4f(d4, t4, t1);
}
#else
//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t cos4f(const vector4_t d)
{
  static const vector4_t A = set4f(-1.0f / 2.0f);
  static const vector4_t B = set4f(1.0f / 24.0f);
  static const vector4_t C = set4f(-1.0f / 720.0f);
  static const vector4_t D = set4f(1.0f / 40320.0f);
  static const vector4_t E = set4f(-1.0f / 3628800.0f);

  const vector4_t d2 = mul4f(d, d);
  const vector4_t t1 = madd4f(d2, A, one4f());
  const vector4_t t3 = madd4f(d2, E, D);
  const vector4_t t2 = madd4f(d2, C, B);
  const vector4_t t4 = madd4f(d2, t3, t2);
  const vector4_t d4 = mul4f(d2, d2);

  return madd4f(d4, t4, t1);
}
#endif

#if defined(NM_HOST_X360)
//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sin4f(const vector4_t d)
{
  static const vector4_t A = set4f(-1.0f / 6.0f);
  static const vector4_t B = set4f(+1.0f / 120.0f);
  static const vector4_t K = { -1.0f / 5040.0f, 1.0f / 362880.0f, -1.0f / 39916800.0f, 1.0f / 6227020800.0f };

  const vector4_t d2 = mul4f(d, d);

  const vector4_t C = splatX(K);
  const vector4_t D = splatY(K);
  const vector4_t E = splatZ(K);
  const vector4_t F = splatW(K);

  const vector4_t t1 = madd4f(d2, B, A);
  const vector4_t t2 = madd4f(d2, D, C);
  const vector4_t t3 = madd4f(d2, F, E);

  const vector4_t d4 = mul4f(d2, d2);
  const vector4_t d3 = mul4f(d2, d);

  const vector4_t t4 = madd4f(d4, t3, t2);
  const vector4_t t5 = madd4f(d4, t4, t1);

  return madd4f(d3, t5, d);
}
#else

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sin4f(const vector4_t d)
{
  static const vector4_t A = set4f(-1.0f / 6.0f);
  static const vector4_t B = set4f(+1.0f / 120.0f);
  static const vector4_t C = set4f(-1.0f / 5040.0f);
  static const vector4_t D = set4f(+1.0f / 362880.0f);
  static const vector4_t E = set4f(-1.0f / 39916800.0f);
  static const vector4_t F = set4f(+1.0f / 6227020800.0f);

  const vector4_t d2 = mul4f(d, d);

  const vector4_t t1 = madd4f(d2, B, A);
  const vector4_t t2 = madd4f(d2, D, C);
  const vector4_t t3 = madd4f(d2, F, E);

  const vector4_t d4 = mul4f(d2, d2);
  const vector4_t d3 = mul4f(d2, d);

  const vector4_t t4 = madd4f(d4, t3, t2);
  const vector4_t t5 = madd4f(d4, t4, t1);

  return madd4f(d3, t5, d);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void sinCos4f(vector4_t d, vector4_t& s, vector4_t& c)
{
#if !defined(NM_HOST_X360)
  const vector4_t d2 = mul4f(d, d);

  static const vector4_t A = set4f(-1.0f / 6.0f);
  static const vector4_t B = set4f(+1.0f / 120.0f);
  static const vector4_t C = set4f(-1.0f / 5040.0f);
  static const vector4_t D = set4f(+1.0f / 362880.0f);
  static const vector4_t E = set4f(-1.0f / 39916800.0f);
  static const vector4_t F = set4f(+1.0f / 6227020800.0f);

  const vector4_t d3 = mul4f(d2, d);

  static const vector4_t A2 = neg4f(half4f());
  static const vector4_t B2 = set4f(1.0f / 24.0f);
  static const vector4_t C2 = set4f(-1.0f / 720.0f);
  static const vector4_t D2 = set4f(1.0f / 40320.0f);
  static const vector4_t E2 = set4f(-1.0f / 3628800.0f);

  const vector4_t d4 = mul4f(d2, d2);

  const vector4_t r1 = madd4f(d2, B, A);
  const vector4_t r2 = madd4f(d2, D, C);
  const vector4_t r3 = madd4f(d2, F, E);

  const vector4_t t1 = madd4f(d2, A2, one4f());
  const vector4_t t3 = madd4f(d2, E2, D2);
  const vector4_t t2 = madd4f(d2, C2, B2);

  const vector4_t r4 = madd4f(d4, r3, r2);
  const vector4_t t4 = madd4f(d2, t3, t2);
  const vector4_t r5 = madd4f(d4, r4, r1);

  c = madd4f(d4, t4, t1);
  s = madd4f(d3, r5, d);
#else
  static const vector4_t A2 = neg4f(half4f());

  static const vector4_t A = set4f(-1.0f / 6.0f);
  static const vector4_t B = set4f(+1.0f / 120.0f);

  static const vector4_t K = { -1.0f / 5040.0f, 1.0f / 362880.0f, -1.0f / 39916800.0f, 1.0f / 6227020800.0f };
  static const vector4_t K2 = { 1.0f / 24.0f, -1.0f / 720.0f, 1.0f / 40320.0f, -1.0f / 3628800.0f };

  const vector4_t d2 = mul4f(d, d);

  const vector4_t C = splatX(K);
  const vector4_t D = splatY(K);
  const vector4_t E = splatZ(K);
  const vector4_t F = splatW(K);

  const vector4_t d3 = mul4f(d2, d);

  const vector4_t B2 = splatX(K2);
  const vector4_t C2 = splatY(K2);
  const vector4_t D2 = splatZ(K2);
  const vector4_t E2 = splatW(K2);

  const vector4_t d4 = mul4f(d2, d2);

  const vector4_t r1 = madd4f(d2, B, A);
  const vector4_t r2 = madd4f(d2, D, C);
  const vector4_t r3 = madd4f(d2, F, E);

  const vector4_t t1 = madd4f(d2, A2, one4f());
  const vector4_t t3 = madd4f(d2, E2, D2);
  const vector4_t t2 = madd4f(d2, C2, B2);

  const vector4_t r4 = madd4f(d4, r3, r2);
  const vector4_t t4 = madd4f(d2, t3, t2);
  const vector4_t r5 = madd4f(d4, r4, r1);

  c = madd4f(d4, t4, t1);
  s = madd4f(d3, r5, d);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t tan4f(vector4_t x)
{
#if defined(NM_HOST_X360)
  static const vector4_t K = { 0.999999986f, -0.0958010197f, -0.429135022f, 0.00971659383f };
  static const vector4_t top0 = splatX(K);
  static const vector4_t top1 = splatY(K);
  static const vector4_t btm0 = splatZ(K);
  static const vector4_t btm1 = splatW(K);
#else
  static const vector4_t top0 = set4f(0.999999986f);
  static const vector4_t top1 = set4f(-0.0958010197f);
  static const vector4_t btm0 = set4f(-0.429135022f);
  static const vector4_t btm1 = set4f(0.00971659383f);
#endif

  const vector4_t x2 = mul4f(x, x);
  const vector4_t t1 = madd4f(top1, x2, top0);
  const vector4_t b1 = madd4f(btm1, x2, btm0);
  const vector4_t top = mul4f(x, t1);
  const vector4_t bottom = madd4f(x2, b1, one4f());

  return  mul4f(top, rcp4f(bottom));
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
/// \param x assumed to be in the range [-1, 1]
//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_HOST_X360)
NM_FORCEINLINE vector4_t asin4f(vector4_t x)
{
  // save a sign mask
  vector4_t negmask = x;
  vector4_t absx = abs4f(x);

  // init coeffs
  static const vector4_t K1 = { 1.507095111f, 1.103007131f, -2.144008022f, 1.507171600f };
  static const vector4_t K2 = { -0.4089766186f, -0.9315200116f, 0.2836182315f, -0.1535779990f };

  const vector4_t top0 = splatX(K1);
  const vector4_t top1 = splatY(K1);
  const vector4_t top2 = splatZ(K1);
  const vector4_t btm0 = splatW(K1);

  const vector4_t btm1 = splatX(K2);
  const vector4_t btm2 = splatY(K2);
  const vector4_t btm3 = splatZ(K2);
  const vector4_t btm4 = splatW(K2);

  // perform approximation in all 4 elements
  const vector4_t x2 = mul4f(x, x);
  const vector4_t x4 = mul4f(x2, x2);

  const vector4_t b1 = madd4f(btm1, absx, btm0);
  const vector4_t b2 = madd4f(btm3, absx, btm2);
  const vector4_t b3 = madd4f(x2, b2, b1);
  const vector4_t bottom = madd4f(x4, btm4, b3);

  const vector4_t t1 = madd4f(top2, absx, top1);
  const vector4_t top = madd4f(absx, t1, top0);
  const vector4_t sq = sqrt4f(sub4f(one4f(), x2)); //__vrefp(__vrsqrtefp

  //const vector4_t result =  sub4f(mul4f(top, __vrefp(bottom)), sq);
  const vector4_t result =  sub4f(mul4f(top, rcp4f(bottom)), sq);

  // make sure we negate any results that were negative originally
  return neg4f(result, negmask);
}

#else

NM_FORCEINLINE vector4_t asin4f(vector4_t x)
{
  // going to use the approximation detailed at the tail end of this page:
  //  http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/arcsin/fastsqrt.html

  // save a sign mask
  vector4_t negmask = x;
  vector4_t absx = abs4f(x);

  // init coeffs
  static const vector4_t top0 = set4f(1.507095111f);
  static const vector4_t top1 = set4f(1.103007131f);
  static const vector4_t top2 = set4f(-2.144008022f);

  static const vector4_t btm0 = set4f(1.507171600f);
  static const vector4_t btm1 = set4f(-0.4089766186f);
  static const vector4_t btm2 = set4f(-0.9315200116f);
  static const vector4_t btm3 = set4f(0.2836182315f);
  static const vector4_t btm4 = set4f(-0.1535779990f);

  // perform approximation in all 4 elements
  const vector4_t x2 = mul4f(x, x);
  const vector4_t t1 = madd4f(top2, absx, top1);
  const vector4_t b1 = madd4f(btm1, absx, btm0);
  const vector4_t b2 = madd4f(btm3, absx, btm2);

  const vector4_t x4 = mul4f(x2, x2);
  const vector4_t top = madd4f(absx, t1, top0);

  const vector4_t b3 = madd4f(x2, b2, b1);
  const vector4_t sq = sqrt4f(sub4f(one4f(), x2));
  const vector4_t bottom = madd4f(x4, btm4, b3);

  const vector4_t result =  sub4f(mul4f(top, rcp4f(bottom)), sq);

  // make sure we negate any results that were negative originally
  return neg4f(result, negmask);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
/// \param x assumed to be in the range [-1, 1]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t acos4f(vector4_t x)
{
  // arcos(X) = PI/2 - arcsin(X)
  return sub4f(set4f(1.5707963267948966192313216916398f), asin4f(x));
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t atan4f(vector4_t x)
{
#if 1
  return asin4f(mul4f(x, rsqrt4f(madd4f(x, x, one4f()))));
#else
  // x(15+4x²)/(15+9x²), much faster, limited range
  static const vector4_t m4 = set4f(4.0f);  
  const vector4_t m9 = add4f(m4, add4f(m4, one4f()));
  const vector4_t m15 = sub4f(mul4f(m4, m4), one4f());

  const vector4_t x2 = mul4f(x, x);
  const vector4_t t1 = madd4f(m4, x2, m15);
  const vector4_t bot = madd4f(m9, x2, m15);
  const vector4_t top = mul4f(x, t1);

  return  mul4f(top, rcp4f(bot));
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
///
/// \brief Compute an approximation to the trigonometric atan2 function by using
/// a polynomial function representation. The atan function is approximated by
/// using two curve pieces that divide the input value range between x = [-1:1].
/// This is possible because of the trig identity:
///
/// atan(x) = x - x^3/3 + x^5/5 - x^7/7 ...  -1 < x < 1
///
/// atan(x) = pi/2 - atan(1/x)              x < -1, x > 1
///
/// The Taylors coefficients have been adjusted to distribute the error over the
/// entire curve and has a maximum angle error of the order 2e-4. The returned
/// angle is in the range [-pi:pi]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t atan24f(const vector4_t s, const vector4_t c)
{
  static const vector4_t K = set4f(0.999314747965817f, -0.322266683652863f, 0.14898078328198f, -0.040826575272239f);

  static const vector4_t mPi_2 = set4f(1.57079632679489661923f);

  const vector4_t coefs0 = splatX(K);
  const vector4_t coefs1 = splatY(K);
  const vector4_t coefs2 = splatZ(K);
  const vector4_t coefs3 = splatW(K);

  const vector4_t mZero = zero4f();
  const vector4_t mPi = add4f(mPi_2, mPi_2);

  vector4_t msk = mask4cmpLT(abs4f(s), abs4f(c));
  vector4_t kc  = sel4cmpLT(c, mZero, mPi, mZero);
  vector4_t ksc = sel4cmpMask(msk, kc, mPi_2);

  vector4_t theta = neg4f(ksc, s); // ?

  vector4_t N = sel4cmpMask(msk, s, c);
  vector4_t D = sel4cmpMask(msk, c, neg4f(s));
  vector4_t x = mul4f(N, rcp4f(D));
  vector4_t u = mul4f(x, x);

  return madd4f(x, madd4f(u, madd4f(u, madd4f(u, coefs3, coefs2), coefs1), coefs0), theta);
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUTrig
/// \brief Compute an approximation to the trigonometric sine and cosine functions by using a
/// polynomial function representation. This function uses two polynomial curves to represent the
/// canonical angle range (adjusted Taylors series about 0 and pi). The adjusted coefficients
/// distribute the error over the entire angle range and has a maximum error of the order 2e-4.
/// This function can represent all POSITIVE finite angles and computes four approximations at a
/// time.
///
/// \param theta - is a vector of four angles which must be positive but can have arbitrary magnitude.
/// \param s - is the output vector of four sine values
/// \param c - is the output vector of four cosine values
NM_FORCEINLINE void fastSinCos4f(vector4_t theta, vector4_t& s, vector4_t& c)
{
  // Constants
  const vector4_t two_pi = set4f(0.636619772367581f);
  const vector4_t pi_two = set4f(1.570796326794897f);
  static const vector4_t sin_coefs = set4f(1.0f, -0.16613412016702f, 0.007667296842758f, 0.0f);
  static const vector4_t cos_coefs = set4f(1.0f, -0.50005809815278f, 0.041657257163128f, -0.001318224442104f);
  const vector4_t bit2i = set4i(0x02);
  const vector4_t bit1i = set4i(0x01);
  vector4_t coefs0, coefs1, coefs2, coefs3;

  // A polynomial approximation to the sin() and cos() functions can be made with an adjusted
  // Taylors series:
  // x is the residual angle within the pi/2 quadrant
  // S_i are the modified Taylors coefficients for the sin() function
  // C_i are the modified Taylors coefficients for the cos() function
  //
  // | Func | quadrant |                        Polynomial                          | quad+1 (binary) |
  // +------------------------------------------------------------------------------------------------+
  // | sin  |    0     |  S_0*x        +  S_1*x^3        +  S_2*x^5                 |       01        |
  // | sin  |    1     | -S_0*(x-pi/2) + -S_1*(x-pi/2)^3 + -S_2*(x-pi/2)^5          |       10        |
  // | sin  |    2     | -S_0*x        + -S_1*x^3        + -S_2*x^5                 |       11        |
  // | sin  |    3     |  S_0*(x-pi/2) +  S_1*(x-pi/2)^3 +  S_2*(x-pi/2)^5          |       00        |
  // +------------------------------------------------------------------------------------------------+
  // | cos  |    0     |  C_0 +  C_1*x^2        +  C_2*x^4        +  C_3*x^6        |       01        |
  // | cos  |    1     | -C_0 + -C_1*(x-pi/2)^2 + -C_2*(x-pi/2)^4 + -C_3*(x-pi/2)^6 |       10        |
  // | cos  |    2     | -C_0 + -C_1*x^2        + -C_2*x^4        + -C_3*x^6        |       11        |
  // | cos  |    3     |  C_0 +  C_1*(x-pi/2)^2 +  C_2*(x-pi/2)^4 +  C_3*(x-pi/2)^6 |       00        |
  // +------------------------------------------------------------------------------------------------+

  // Select which pi/2 quadrant theta lies in.
  vector4_t p2segi = c2int4f(mul4f(theta, two_pi));

  // Compute the adjusted quadrant whose bit pattern encodes the sign of the coefficients
  // and the offset adjustment of the residual angle.
  vector4_t cmpsegi = add4i(p2segi, bit1i); // ** Integer add used to avoid numerical issues on SPU **
  // i.e. quad+1 is simply a quadrant shift x+pi/2

  // Get the appropriate sign mask for the coefficient sets:
  // Negative coefs sets occur in quadrants 1 & 2 (i.e. bit 2 is set in quad+1)
  vector4_t signmask = sel4cmpBits(cmpsegi, bit2i, negOne4f(), one4f());

  // The the adjusted residual angle in the range [0:pi/2] required for the modified
  // Taylors polynomial. The residual angle must be adjusted by pi/2 in quadrants 1 & 3
  // (i.e. bit 1 is not set in quad+1)
  vector4_t ang_res = sub4f(theta, mul4f(c2float4i(p2segi), pi_two));
  vector4_t ang_offset = sel4cmpBits(cmpsegi, bit1i, zero4f(), pi_two);
  vector4_t alpha = sub4f(ang_res, ang_offset);
  vector4_t u = mul4f(alpha, alpha);

  // Evaluate the Taylors series of the sine function
  coefs0 = mul4f(signmask, splatX(sin_coefs));
  coefs1 = mul4f(signmask, splatY(sin_coefs));
  coefs2 = mul4f(signmask, splatZ(sin_coefs));
  s = mul4f(alpha, madd4f(u, madd4f(u, coefs2, coefs1), coefs0));

  // Evaluate the Taylors series of the cosine function
  coefs0 = mul4f(signmask, splatX(cos_coefs));
  coefs1 = mul4f(signmask, splatY(cos_coefs));
  coefs2 = mul4f(signmask, splatZ(cos_coefs));
  coefs3 = mul4f(signmask, splatW(cos_coefs));
  c = madd4f(u, madd4f(u, madd4f(u, coefs3, coefs2), coefs1), coefs0);
}

//----------------------------------------------------------------------------------------------------------------------
