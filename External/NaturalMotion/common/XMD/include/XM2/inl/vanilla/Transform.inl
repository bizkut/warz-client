#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_TRANSFORM__INL__
#define XM2_VANILLA_TRANSFORM__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::lerp(const XTransform& a,const XTransform& b,const XReal t)
{
  scale.lerp(a.scale,b.scale,t);
  translate.lerp(a.translate,b.translate,t);
  rotate.lerp(a.rotate,b.rotate,t);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::slerp(const XTransform& a,const XTransform& b,const XReal t)
{
  scale.lerp(a.scale,b.scale,t);
  translate.lerp(a.translate,b.translate,t);
  rotate.slerp(a.rotate,b.rotate,t);
}
}
#endif
