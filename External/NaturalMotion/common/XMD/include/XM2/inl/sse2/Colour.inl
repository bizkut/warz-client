#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_COLOUR__INL__
#define XM2_SSE2_COLOUR__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal v)
{
  XM2_ALIGN_ASSERT(this);
  rg = ba = _mm_set1_pd(v);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal* col)
{
  XM2_ASSERT(col);
  XM2_ALIGN_ASSERT(this);
  rg = _mm_loadu_pd(col);
  ba = _mm_loadu_pd(col+2);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColour::equal(const XColour& _b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_b);
  XColour temp = *this;
  temp -= _b;
  temp.rg = _mm_mul_pd(temp.rg,temp.rg);
  temp.ba = _mm_mul_pd(temp.ba,temp.ba);
  const __m128d _eps = _mm_set1_pd(eps*eps);
  return (3 == _mm_movemask_pd(_mm_cmplt_pd(temp.rg,_eps))) && 
         (3 == _mm_movemask_pd(_mm_cmplt_pd(temp.ba,_eps)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator = (const XColour& c)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&c);
  rg = c.rg;
  ba = c.ba;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::zero()
{
  XM2_ALIGN_ASSERT(this);
  rg = _mm_setzero_pd();
  ba = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::white()
{
  XM2_ALIGN_ASSERT(this);
  rg = _mm_set1_pd(1.0);
  ba = _mm_set1_pd(1.0);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::set(const XReal _r,const XReal _g,const XReal _b,const XReal _a)
{
  XM2_ALIGN_ASSERT(this);
  rg = _mm_set_pd(_g,_r);
  ba = _mm_set_pd(_a,_b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::mul(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  rg = _mm_mul_pd(_a.rg,_b.rg);
  ba = _mm_mul_pd(_a.ba,_b.ba);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::div(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);

  /* perform division. either use assert or runtime check to prevent division by zero */
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(_b.r)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(_b.g)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(_b.b)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(_b.a)>XM2_FLOAT_ZERO);
    rg = _mm_div_pd(_a.rg,_b.rg);
    ba = _mm_div_pd(_a.ba,_b.ba);
  #else
    r = (XMabs(_b.r)<XM2_FLOAT_ZERO) ? 0 : _a.r / _b.r;
    g = (XMabs(_b.g)<XM2_FLOAT_ZERO) ? 0 : _a.g / _b.g;
    b = (XMabs(_b.b)<XM2_FLOAT_ZERO) ? 0 : _a.b / _b.b;
    a = (XMabs(_b.a)<XM2_FLOAT_ZERO) ? 0 : _a.a / _b.a;
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::sub(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);
  rg = _mm_sub_pd(_a.rg,_b.rg);
  ba = _mm_sub_pd(_a.ba,_b.ba);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::add(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);
  rg = _mm_add_pd(_a.rg,_b.rg);
  ba = _mm_add_pd(_a.ba,_b.ba);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::mul(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  __m128d temp = _mm_set1_pd(_b);
  rg = _mm_mul_pd(_a.rg,temp);
  ba = _mm_mul_pd(_a.ba,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::div(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  /* perform division. either use assert or runtime check to prevent division by zero */
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(_b)>XM2_FLOAT_ZERO);
    const __m128d temp = _mm_set1_pd(_b);
    rg = _mm_div_pd(_a.rg,temp);
    ba = _mm_div_pd(_a.ba,temp);
  #else
    r = (XMabs(_b)<XM2_FLOAT_ZERO) ? 0 : _a.r / _b;
    g = (XMabs(_b)<XM2_FLOAT_ZERO) ? 0 : _a.g / _b;
    b = (XMabs(_b)<XM2_FLOAT_ZERO) ? 0 : _a.b / _b;
    a = (XMabs(_b)<XM2_FLOAT_ZERO) ? 0 : _a.a / _b;
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::sub(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(_b);
  rg = _mm_sub_pd(_a.rg,temp);
  ba = _mm_sub_pd(_a.ba,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::add(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(_b);
  rg = _mm_add_pd(_a.rg,temp);
  ba = _mm_add_pd(_a.ba,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::invert(const XColour& _a)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(1.0);
  rg = _mm_sub_pd(temp,_a.rg);
  ba = _mm_sub_pd(temp,_a.ba);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::negate(const XColour& _a)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(-1.0);
  rg = _mm_mul_pd(_a.rg,temp);
  ba = _mm_mul_pd(_a.ba,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::lerp(const XColour& a,const XColour& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(t);
  rg = _mm_add_pd( a.rg, _mm_mul_pd( temp, _mm_sub_pd( b.rg, a.rg ) ) );
  ba = _mm_add_pd( a.ba, _mm_mul_pd( temp, _mm_sub_pd( b.ba, a.ba ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::bezier(const XColour& _a,const XColour& _b,const XColour& _c,const XColour& _d,const XReal t)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(&_c);
  XM2_ALIGN_ASSERT(&_d);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);
  const __m128d _ab0 = _mm_add_pd( _a.rg, _mm_mul_pd( _t, _mm_sub_pd( _b.rg, _a.rg ) ) );
  const __m128d _ab1 = _mm_add_pd( _a.ba, _mm_mul_pd( _t, _mm_sub_pd( _b.ba, _a.ba ) ) );
  const __m128d _bc0 = _mm_add_pd( _b.rg, _mm_mul_pd( _t, _mm_sub_pd( _c.rg, _b.rg ) ) );
  const __m128d _bc1 = _mm_add_pd( _b.ba, _mm_mul_pd( _t, _mm_sub_pd( _c.ba, _b.ba ) ) );
  const __m128d _cd0 = _mm_add_pd( _c.rg, _mm_mul_pd( _t, _mm_sub_pd( _d.rg, _c.rg ) ) );
  const __m128d _cd1 = _mm_add_pd( _c.ba, _mm_mul_pd( _t, _mm_sub_pd( _d.ba, _c.ba ) ) );

  const __m128d _abc0 = _mm_add_pd( _ab0, _mm_mul_pd( _t, _mm_sub_pd( _bc0, _ab0 ) ) );
  const __m128d _abc1 = _mm_add_pd( _ab1, _mm_mul_pd( _t, _mm_sub_pd( _bc1, _ab1 ) ) );
  const __m128d _bcd0 = _mm_add_pd( _bc0, _mm_mul_pd( _t, _mm_sub_pd( _cd0, _bc0 ) ) );
  const __m128d _bcd1 = _mm_add_pd( _bc1, _mm_mul_pd( _t, _mm_sub_pd( _cd1, _bc1 ) ) );

  rg = _mm_add_pd( _abc0, _mm_mul_pd( _t, _mm_sub_pd( _bcd0, _abc0 ) ) );
  ba = _mm_add_pd( _abc1, _mm_mul_pd( _t, _mm_sub_pd( _bcd1, _abc1 ) ) );
}
}
#endif
