#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_RAY__INL__
#define XM2_RAY__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XRay::XRay()
{
  #if XM2_INIT_CLASSES
  zero();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XRay::XRay(const XRay& p)
{
  memcpy(this,&p,sizeof(XRay));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XRay::XRay(const XVector3& _origin,const XVector3& _dir)
{
  set(_origin,_dir);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XRay::XRay(const XReal ox,const XReal oy,const XReal oz,
                  const XReal dx,const XReal dy,const XReal dz)
{
  set(XVector3(ox,oy,oz),XVector3(dx,dy,dz));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XRay::zero()
{
  origin.zero();
  direction.zero();
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XRay::transform(const XMatrix& m)
{
  m.transformPoint(origin);
  m.transformVector(direction);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XRay::negate()
{
  /* check pointers are valid */
  XM2_ALIGN_ASSERT(this);

  direction.negate();
}
//----------------------------------------------------------------------------------------------------------------------  
bool XRay::equal(const XRay& b,const XReal eps) const
{
  return origin.equal(b.origin,eps) &&
         direction.equal(b.direction,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XRay::notEqual(const XRay& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
void XRay::set(const XVector3& _origin,const XVector3& _dir)
{
  origin = _origin;
  direction = _dir;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XRay& XRay::operator = (const XRay& p)
{
  memcpy(this,&p,sizeof(XRay));
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XRay::operator == (const XRay& p) const
{
  return equal(p);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XRay::operator != (const XRay& p) const
{
  return notEqual(p);
}
}
#endif
