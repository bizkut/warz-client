#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_COLOUR__INL__
#define XM2_SSE_COLOUR__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal v)
{
  sse = _mm_set_ps1(v);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal* col)
{
  XM2_ASSERT(col);
  sse = _mm_loadu_ps(col);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColour::equal(const XColour& _b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_b);
  XColour temp = *this;
  temp.sse = _mm_sub_ps(temp.sse,_b.sse);
  temp.sse = _mm_mul_ps(temp.sse,temp.sse);
  return 15 == _mm_movemask_ps(_mm_cmplt_ps(temp.sse,_mm_set_ps1(eps*eps)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator = (const XColour& c)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&c);
  sse = c.sse;;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::zero()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::white()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps1(1.0f);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::set(const XReal _r,const XReal _g,const XReal _b,const XReal _a)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps(_a,_b,_g,_r);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::mul(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  sse = _mm_mul_ps(_a.sse,_b.sse);
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
    sse = _mm_div_ps(_a.sse,_b.sse);
  #else
    // test if any of the lengths are zero
    const __m128 test = _mm_and_ps(_mm_cmpgt_ps(_b.sse,_mm_sub_ps(_mm_setzero_ps(),_mm_set_ps1(XM2_FLOAT_ZERO))),
                                   _mm_cmplt_ps(_b.sse,_mm_set_ps1(XM2_FLOAT_ZERO)));

    // replace any zero's with 1.0f
    __m128 temp = _mm_rcp_ps(_mm_or_ps(_mm_and_ps(test,_mm_set_ps1(1.0f)),
                                       _mm_andnot_ps(test,_b.sse)));
    sse = _mm_div_ps(_a.sse,temp);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::sub(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_a.sse,_b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::add(const XColour& _a,const XColour& _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(_a.sse,_b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::mul(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps(_a.sse,_mm_set_ps1(_b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::div(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(_b)>XM2_FLOAT_ZERO);
    sse = _mm_div_ps(_a.sse,_mm_set_ps1(_b));
  #else
    __m128 _d = _mm_set_ps1((XMabs(_b)<XM2_FLOAT_ZERO) ? 1.0f : _b);
    sse = _mm_div_ps(_a.sse,_d);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::sub(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_a.sse,_mm_set_ps1(_b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::add(const XColour& _a,const XReal _b)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(_a.sse,_mm_set_ps1(_b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::invert(const XColour& _a)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_mm_set_ps1(1.0f),_a.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::negate(const XColour& _a)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_mm_setzero_ps(),_a.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::lerp(const XColour& a,const XColour& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps( a.sse, _mm_mul_ps( _mm_set_ps1(t), _mm_sub_ps( b.sse, a.sse ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::bezier(const XColour& _a,const XColour& _b,const XColour& _c,const XColour& _d,const XReal t)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(&_c);
  XM2_ALIGN_ASSERT(&_d);
  XM2_ALIGN_ASSERT(this);
  const __m128 _t = _mm_set_ps1(t);
  const __m128 _ab = _mm_add_ps( _a.sse, _mm_mul_ps( _t, _mm_sub_ps( _b.sse, _a.sse ) ) );
  const __m128 _bc = _mm_add_ps( _b.sse, _mm_mul_ps( _t, _mm_sub_ps( _c.sse, _b.sse ) ) );
  const __m128 _cd = _mm_add_ps( _c.sse, _mm_mul_ps( _t, _mm_sub_ps( _d.sse, _c.sse ) ) );
  const __m128 _abc = _mm_add_ps( _ab, _mm_mul_ps( _t, _mm_sub_ps( _bc, _ab ) ) );
  const __m128 _bcd = _mm_add_ps( _bc, _mm_mul_ps( _t, _mm_sub_ps( _cd, _bc ) ) );
  sse = _mm_add_ps( _abc, _mm_mul_ps( _t, _mm_sub_ps( _bcd, _abc ) ) );
}
}
#endif
