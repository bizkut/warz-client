#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_PLANE__INL__
#define XM2_SSE2_PLANE__INL__
#if XM2_USE_FLOAT
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XPlane::zero()
{
  XM2_ALIGN_ASSERT(this);
  ab = _mm_setzero_pd();
  cd = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XPlane::set(const XReal a,const XReal b,const XReal c,const XReal d)
{
  XM2_ALIGN_ASSERT(this);
  ab = _mm_set_pd(b,a);
  cd = _mm_set_pd(d,c);
}
};
#endif
