#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_VECTOR2__INL__
#define XM2_SSE2_VECTOR2__INL__
#if (XM2_USE_FLOAT)
# error SSE2 double precision routines included when set to build using floats
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::zero()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::set(const XReal x,const XReal y)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_pd(y,x);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::set(const XReal* ptr)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_loadu_pd(ptr);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::add(const XVector2& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_pd(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::sub(const XVector2& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_pd(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::mul(const XVector2& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_pd(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::div(const XVector2& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_div_pd(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::add(const XVector2& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_pd(a.sse,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::sub(const XVector2& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_pd(a.sse,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::div(const XVector2& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_div_pd(a.sse,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::mul(const XVector2& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_pd(a.sse,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XVector2::dot(const XVector2& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  union 
  {
    __m128d sse;
    XReal data[2];
  } d;
  d.sse = _mm_mul_pd(sse,b.sse);
  __m128d a = _mm_shuffle_pd(d.sse,d.sse,_MM_SHUFFLE2(0,1));
  d.sse = _mm_add_pd(d.sse,a);
  return d.data[1];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::lerp(const XVector2& a,const XVector2& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_pd(a.sse,_mm_mul_pd(_mm_set1_pd(t),_mm_sub_pd(b.sse,a.sse)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::bezier(const XVector2& a,const XVector2& b,const XVector2& c,const XVector2& d,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);
  const __m128d ab = _mm_add_pd(a.sse,_mm_mul_pd(_t,_mm_sub_pd(b.sse,a.sse)));
  const __m128d bc = _mm_add_pd(b.sse,_mm_mul_pd(_t,_mm_sub_pd(c.sse,b.sse)));
  const __m128d cd = _mm_add_pd(c.sse,_mm_mul_pd(_t,_mm_sub_pd(d.sse,c.sse)));
  const __m128d abc = _mm_add_pd(ab,_mm_mul_pd(_t,_mm_sub_pd(bc,ab)));
  const __m128d bcd = _mm_add_pd(bc,_mm_mul_pd(_t,_mm_sub_pd(cd,bc)));
  sse = _mm_add_pd(abc,_mm_mul_pd(_t,_mm_sub_pd(bcd,abc)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::negate(const XVector2& v)
{
  XM2_ALIGN_ASSERT(&v);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_pd(_mm_setzero_pd(),v.sse);
}
}
#endif
