#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_SPHERE__INL__
#define XM2_VANILLA_SPHERE__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector3SOA& p)
{
  XVector3SOA diff(center);
  diff.sub(p,diff);
  XVector4 dist;
  diff.lengthSquared(dist);
  setRadiusSquared( XMmax(XMmax(XMmax(getRadiusSquared(),dist.x),XMmax(dist.y,dist.z)),dist.w) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector4SOA& p)
{
  XVector4SOA diff(center);
  diff.sub(p,diff);
  diff.w[0]=diff.w[1]=diff.w[2]=diff.w[3]=0;
  XVector4 dist;
  diff.lengthSquared(dist);
  setRadiusSquared( XMmax(XMmax(XMmax(getRadiusSquared(),dist.x),XMmax(dist.y,dist.z)),dist.w) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::setRadiusSquared(const XReal _radius)
{
 radius = _radius;
}
}
#endif
