#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_TRANSFORM__INL__
#define XM2_SSE2_TRANSFORM__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::lerp(const XTransform& a,const XTransform& b,const XReal t)
{
  __m128d _t = _mm_set1_pd(t);
  XQuaternion q(b.rotate);
  q.negate();
  q = a.rotate.dot(b.rotate)<0 ? q : b.rotate;
  translate.xy = _mm_add_pd(a.translate.xy,_mm_mul_pd(_t,_mm_sub_pd(b.translate.xy,a.translate.xy)));
  translate.zw = _mm_add_pd(a.translate.zw,_mm_mul_pd(_t,_mm_sub_pd(b.translate.zw,a.translate.zw)));
  rotate.xy = _mm_add_pd(a.rotate.xy,_mm_mul_pd(_t,_mm_sub_pd(b.rotate.xy,a.rotate.xy)));
  rotate.zw = _mm_add_pd(a.rotate.zw,_mm_mul_pd(_t,_mm_sub_pd(b.rotate.zw,a.rotate.zw)));
  rotate.normalise();
  scale.xy = _mm_add_pd(a.scale.xy,_mm_mul_pd(_t,_mm_sub_pd(b.scale.xy,a.scale.xy)));
  scale.zw = _mm_add_pd(a.scale.zw,_mm_mul_pd(_t,_mm_sub_pd(b.scale.zw,a.scale.zw)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::slerp(const XTransform& a,const XTransform& b,const XReal t)
{
  __m128d _t = _mm_set1_pd(t);
  translate.xy = _mm_add_pd(a.translate.xy,_mm_mul_pd(_t,_mm_sub_pd(b.translate.xy,a.translate.xy)));
  translate.zw = _mm_add_pd(a.translate.zw,_mm_mul_pd(_t,_mm_sub_pd(b.translate.zw,a.translate.zw)));
  rotate.slerp(a.rotate,b.rotate,t);
  scale.xy = _mm_add_pd(a.scale.xy,_mm_mul_pd(_t,_mm_sub_pd(b.scale.xy,a.scale.xy)));
  scale.zw = _mm_add_pd(a.scale.zw,_mm_mul_pd(_t,_mm_sub_pd(b.scale.zw,a.scale.zw)));
}
}
#endif
