#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_VECTOR3__INL__
#define XM2_SSE_VECTOR3__INL__
#if (!XM2_USE_FLOAT)
# error SSE single precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3(const XVector3& rhs) 
{ 
  XM2_ALIGN_ASSERT(&rhs);
  XM2_ALIGN_ASSERT(this);
  sse = rhs.sse;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3(const XVector4& rhs) 
{ 
  XM2_ALIGN_ASSERT(&rhs);
  XM2_ALIGN_ASSERT(this);
  sse = rhs.sse;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::set(const XReal x,const XReal y,const XReal z)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps(0.0f,z,y,x);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::set(const XReal* ptr)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(ptr);
  sse = _mm_loadu_ps(ptr);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::zero()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::div(const XVector3& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  // division by zero check
  XM2_ASSERT( XMabs(b) > XM2_FLOAT_ZERO );
  sse = _mm_div_ps(a.sse,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::mul(const XVector3& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps(a.sse,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::add(const XVector3& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(a.sse,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sub(const XVector3& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(a.sse,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::div(const XVector3& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_div_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::mul(const XVector3& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::add(const XVector3& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sub(const XVector3& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3::dot(const XVector3& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XVector3 dp;
  dp.sse = _mm_mul_ps(sse,b.sse);
  return (dp.x + dp.y + dp.z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::cross(const XVector3& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  {
    __m128 temp_a = _mm_set_ps(0.0f,a.x,a.z,a.y);
    __m128 temp_b = _mm_set_ps(0.0f,b.y,b.x,b.z);

    __m128 temp_c = _mm_set_ps(0.0f,a.y,a.x,a.z);
    __m128 temp_d = _mm_set_ps(0.0f,b.x,b.z,b.y);

    sse = _mm_mul_ps(temp_a,temp_b);
    sse = _mm_sub_ps(sse,_mm_mul_ps(temp_c,temp_d));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::lerp(const XVector3& a,const XVector3& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps( a.sse, _mm_mul_ps( _mm_set_ps1(t), _mm_sub_ps( b.sse, a.sse ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::bezier(const XVector3& _a,const XVector3& _b,const XVector3& _c,const XVector3& _d,const XReal t)
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
XM2_INLINE void XVector3::negate(const XVector3& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_mm_setzero_ps(),a.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::negate()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_mm_setzero_ps(),sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sum(const XVector3& a,const XReal weight)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  sse = _mm_add_ps(_mm_mul_ps(a.sse,_mm_set_ps1(weight)),sse);
}
}
#endif
