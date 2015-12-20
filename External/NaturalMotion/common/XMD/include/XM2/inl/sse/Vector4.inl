#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_VECTOR4__INL__
#define XM2_SSE_VECTOR4__INL__
#if (!XM2_USE_FLOAT)
# error SSE single precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::set(const XReal _x,const XReal _y,const XReal _z,const XReal _w)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps(_w,_z,_y,_x);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::set(const XReal* ptr)
{
  XM2_ASSERT(ptr);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_loadu_ps(ptr);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::zero()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_div_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_div_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector4::dot(const XVector4& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XVector4 dp;
  dp.sse = _mm_mul_ps(sse,b.sse);
  return (dp.x + dp.y + dp.z + dp.w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::cross(const XVector4& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128 temp_a = _mm_set_ps(0.0f,a.x,a.z,a.y);
  __m128 temp_b = _mm_set_ps(0.0f,b.y,b.x,b.z);
  __m128 temp_c = _mm_set_ps(0.0f,a.y,a.x,a.z);
  __m128 temp_d = _mm_set_ps(0.0f,b.x,b.z,b.y);
  sse = _mm_mul_ps(temp_a,temp_b);
  sse = _mm_sub_ps(sse,_mm_mul_ps(temp_c,temp_d));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps( a.sse, _mm_set_ps1(b) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps( a.sse, _mm_set_ps1(b) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT( XMabs(b) > XM2_FLOAT_ZERO );
    sse = _mm_div_ps( a.sse, _mm_set_ps1(b) );
  #else
    sse = _mm_mul_ps( a.sse, _mm_set_ps1( (XMabs(b) < XM2_FLOAT_ZERO) ? 0.0f : 1.0f/b) );
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps( a.sse, _mm_set_ps1(b) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::lerp(const XVector4& a,const XVector4& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps( a.sse, _mm_mul_ps( _mm_set_ps1(t), _mm_sub_ps( b.sse, a.sse ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::bezier(const XVector4& _a,const XVector4& _b,const XVector4& _c,const XVector4& _d,const XReal t)
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
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::negate(const XVector4& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_mm_setzero_ps(),a.sse);
}
}
#endif
