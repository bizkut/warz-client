#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_PLANE__INL__
#define XM2_SSE_PLANE__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XPlane::zero()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XPlane::set(const XReal a,const XReal b,const XReal c,const XReal d)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps(d,c,b,a);
}
};
#endif
