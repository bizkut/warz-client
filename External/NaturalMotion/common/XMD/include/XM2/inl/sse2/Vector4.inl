#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_VECTOR4__INL__
#define XM2_SSE2_VECTOR4__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::set(const XReal _x,const XReal _y,const XReal _z,const XReal _w)
{
  XM2_ALIGN_ASSERT(this);
  xy = _mm_set_pd(_y,_x);
  zw = _mm_set_pd(_w,_z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::set(const XReal* ptr)
{
  XM2_ASSERT(ptr);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_loadu_pd(ptr);
  zw = _mm_loadu_pd(ptr+2);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::zero()
{
  XM2_ALIGN_ASSERT(this);
  xy = _mm_setzero_pd();
  zw = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_add_pd(a.xy,b.xy);
  zw = _mm_add_sd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_sub_pd(a.xy,b.xy);
  zw = _mm_sub_sd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_add_pd(a.xy,b.xy);
  zw = _mm_add_pd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_sub_pd(a.xy,b.xy);
  zw = _mm_sub_pd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_mul_pd(a.xy,b.xy);
  zw = _mm_mul_sd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_div_pd(a.xy,b.xy);
  zw = _mm_div_sd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_mul_pd(a.xy,b.xy);
  zw = _mm_mul_pd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_div_pd(a.xy,b.xy);
  zw = _mm_div_pd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector4::dot(const XVector4& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XVector4 dp;
  dp.xy = _mm_mul_pd(xy,b.xy);
  dp.zw = _mm_mul_pd(zw,b.zw);
  dp.xy = _mm_add_pd(dp.xy,dp.zw);
  dp.xy = _mm_add_sd(dp.xy,_mm_shuffle_pd(dp.xy,dp.xy,_MM_SHUFFLE2(1,1)));
  return dp.x;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::cross(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d temp_a0 = _mm_set_pd(a.z,a.y);
  __m128d temp_a1 = _mm_set_pd(0,a.x);
  __m128d temp_b0 = _mm_set_pd(b.x,b.z);
  __m128d temp_b1 = _mm_set_pd(0,b.y);
  __m128d temp_c0 = _mm_set_pd(a.x,a.z);
  __m128d temp_c1 = _mm_set_pd(0,a.y);
  __m128d temp_d0 = _mm_set_pd(b.z,b.y);
  __m128d temp_d1 = _mm_set_pd(0,b.x);
  xy = _mm_mul_pd(temp_a0,temp_b0);
  zw = _mm_mul_pd(temp_a1,temp_b1);
  xy = _mm_sub_pd(xy,_mm_mul_pd(temp_c0,temp_d0));
  zw = _mm_sub_pd(zw,_mm_mul_pd(temp_c1,temp_d1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(b);
  xy = _mm_add_pd( a.xy, temp );
  zw = _mm_add_pd( a.zw, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(b);
  xy = _mm_sub_pd( a.xy, temp );
  zw = _mm_sub_pd( a.zw, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT( XMabs(b) > XM2_FLOAT_ZERO );
    const __m128d temp = _mm_set1_pd( b );
    xy = _mm_div_pd( a.xy, temp );
    zw = _mm_div_pd( a.zw, temp );
  #else
    const __m128d temp = _mm_set1_pd( (XMabs(b) < XM2_FLOAT_ZERO) ? 0.0f : 1.0f/b );
    xy = _mm_mul_pd( a.xy, temp );
    zw = _mm_mul_pd( a.zw, temp );
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(b);
  xy = _mm_mul_pd( a.xy, temp );
  zw = _mm_mul_pd( a.zw, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::lerp(const XVector4& a,const XVector4& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);
  xy = _mm_add_pd( a.xy, _mm_mul_pd( _t, _mm_sub_pd( b.xy, a.xy ) ) );
  zw = _mm_add_pd( a.zw, _mm_mul_pd( _t, _mm_sub_pd( b.zw, a.zw ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::bezier(const XVector4& _a,const XVector4& _b,const XVector4& _c,const XVector4& _d,const XReal t)
{
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(&_c);
  XM2_ALIGN_ASSERT(&_d);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);
  const __m128d _ab0 = _mm_add_pd( _a.xy, _mm_mul_pd( _t, _mm_sub_pd( _b.xy, _a.xy ) ) );
  const __m128d _bc0 = _mm_add_pd( _b.xy, _mm_mul_pd( _t, _mm_sub_pd( _c.xy, _b.xy ) ) );
  const __m128d _cd0 = _mm_add_pd( _c.xy, _mm_mul_pd( _t, _mm_sub_pd( _d.xy, _c.xy ) ) );
  const __m128d _ab1 = _mm_add_pd( _a.zw, _mm_mul_pd( _t, _mm_sub_pd( _b.zw, _a.zw ) ) );
  const __m128d _bc1 = _mm_add_pd( _b.zw, _mm_mul_pd( _t, _mm_sub_pd( _c.zw, _b.zw ) ) );
  const __m128d _cd1 = _mm_add_pd( _c.zw, _mm_mul_pd( _t, _mm_sub_pd( _d.zw, _c.zw ) ) );
  const __m128d _abc0 = _mm_add_pd( _ab0, _mm_mul_pd( _t, _mm_sub_pd( _bc0, _ab0 ) ) );
  const __m128d _bcd0 = _mm_add_pd( _bc0, _mm_mul_pd( _t, _mm_sub_pd( _cd0, _bc0 ) ) );
  const __m128d _abc1 = _mm_add_pd( _ab1, _mm_mul_pd( _t, _mm_sub_pd( _bc1, _ab1 ) ) );
  const __m128d _bcd1 = _mm_add_pd( _bc1, _mm_mul_pd( _t, _mm_sub_pd( _cd1, _bc1 ) ) );
  xy = _mm_add_pd( _abc0, _mm_mul_pd( _t, _mm_sub_pd( _bcd0, _abc0 ) ) );
  zw = _mm_add_pd( _abc1, _mm_mul_pd( _t, _mm_sub_pd( _bcd1, _abc1 ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::negate(const XVector4& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_sub_pd(_mm_setzero_pd(),a.xy);
  zw = _mm_sub_pd(_mm_setzero_pd(),a.zw);
}

}
#endif
