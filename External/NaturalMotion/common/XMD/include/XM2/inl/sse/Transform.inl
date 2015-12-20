#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_TRANSFORM__INL__
#define XM2_SSE_TRANSFORM__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::lerp(const XTransform& a,const XTransform& b,const XReal t)
{
  __m128 _t = _mm_set_ps1(t);
  XQuaternion q(b.rotate);
  q.negate();
  q = a.rotate.dot(b.rotate)<0 ? q : b.rotate;
  translate.sse = _mm_add_ps(a.translate.sse,_mm_mul_ps(_t,_mm_sub_ps(b.translate.sse,a.translate.sse)));
  rotate.sse = _mm_add_ps(a.rotate.sse,_mm_mul_ps(_t,_mm_sub_ps(b.rotate.sse,a.rotate.sse)));
  rotate.normalise();
  scale.sse = _mm_add_ps(a.scale.sse,_mm_mul_ps(_t,_mm_sub_ps(b.scale.sse,a.scale.sse)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::slerp(const XTransform& a,const XTransform& b,const XReal t)
{
  __m128 _t = _mm_set_ps1(t);
  translate.sse = _mm_add_ps(a.translate.sse,_mm_mul_ps(_t,_mm_sub_ps(b.translate.sse,a.translate.sse)));
  rotate.slerp(a.rotate,b.rotate,t);
  scale.sse = _mm_add_ps(a.scale.sse,_mm_mul_ps(_t,_mm_sub_ps(b.scale.sse,a.scale.sse)));
}
}
#endif
