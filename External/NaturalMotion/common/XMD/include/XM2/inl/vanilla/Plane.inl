#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_PLANE__INL__
#define XM2_VANILLA_PLANE__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XPlane::zero()
{
  a=b=c=d=0;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XPlane::set(const XReal _a,const XReal _b,const XReal _c,const XReal _d)
{
  a=_a;
  b=_b;
  c=_c;
  d=_d;
}
};
#endif
