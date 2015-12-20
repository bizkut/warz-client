#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SPHERE__INL__
#define XM2_SPHERE__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XSphere::XSphere()
{
  #if XM2_INIT_CLASSES
  zero();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XSphere::XSphere(const XVector3& c,const XReal r)
{
  center = c;
  setRadius(r);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XSphere::XSphere(const XSphere& src)
{
  center = src.center;
  setRadius(src.getRadius());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::equal(const XSphere& a,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  return center.equal(a.center,eps) && 
         float_equal(getRadius(),a.getRadius(),eps);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::notEqual(const XSphere& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::zero()
{
  XM2_ALIGN_ASSERT(this);
  memset(this,0,sizeof(XSphere));
}
//----------------------------------------------------------------------------------------------------------------------  
void XSphere::set(const XVector3& _center,const XReal _radius)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_center);
  setRadius(_radius);
  center = _center;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::setRadius(const XReal _radius)
{
  XM2_ALIGN_ASSERT(this);
  setRadiusSquared(_radius*_radius);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XSphere::getRadius() const
{
  XM2_ALIGN_ASSERT(this);
  const XReal* ptr = (const XReal*)((const void*)&radius);
  return XMsqrt(*ptr);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XSphere::getRadiusSquared() const
{
  XM2_ALIGN_ASSERT(this);
  const XReal* ptr = (const XReal*)((const void*)&radius);
  return (*ptr);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::intersects(const XRay& ray,XVector3& intersect) const
{ 
  XReal ip;
  XM2_ALIGN_ASSERT(&ray);
  XM2_ALIGN_ASSERT(&intersect);
  XM2_ALIGN_ASSERT(this);
  if (intersects(ray,ip))
  {
    XVector3 dist;
    dist.mul(ray.direction,ip);
    intersect.add(ray.origin,dist);
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::copy(const XSphere& src)
{
  XM2_ALIGN_ASSERT(&src);
  XM2_ALIGN_ASSERT(this);
  memcpy(this,&src,sizeof(XSphere));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::contains(const XVector3& point) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&point);
  XVector3 diff;
  diff.sub(point,center);
  XReal dist = diff.lengthSquared();
  return (dist>getRadiusSquared()) ? false : true;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::contains(const XVector4& point) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&point);
  XVector4 diff;
  diff.sub(point,center);
  XReal dist = diff.lengthSquared();
  return (dist>getRadiusSquared()) ? false : true;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::contains(const XVector3Packed& point) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&point);
  XVector3Packed diff;
  diff.sub(point,center);
  XReal dist = diff.lengthSquared();
  return (dist>getRadiusSquared()) ? false : true;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::transform(const XMatrix& mat)
{
  XM2_ALIGN_ASSERT(&mat);
  XM2_ALIGN_ASSERT(this);
  mat.transformPoint(center);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector3& p)
{
  XM2_ALIGN_ASSERT(&p);
  XM2_ALIGN_ASSERT(this);
  XVector3 diff;
  diff.sub(p,center);
  XReal dist = diff.lengthSquared();
  XReal r = getRadiusSquared();
  setRadiusSquared((dist>(r)) ? dist : r);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector4& p)
{
  XM2_ALIGN_ASSERT(&p);
  XM2_ALIGN_ASSERT(this);
  XVector3 diff;
  diff.sub(XVector3(p.x,p.y,p.z),center);
  XReal dist = diff.lengthSquared();
  XReal r = getRadiusSquared();
  setRadiusSquared((dist>(r)) ? dist : r);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector3Packed& p)
{
  XM2_ALIGN_ASSERT(&p);
  XM2_ALIGN_ASSERT(this);
  XVector3 diff;
  diff.sub(XVector3(p),center);
  XReal dist = diff.lengthSquared();
  XReal r = getRadiusSquared();
  setRadiusSquared((dist>(r)) ? dist : r);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XSphere& test)
{
  XM2_ALIGN_ASSERT(&test);
  XM2_ALIGN_ASSERT(this);
  XVector3 v;
  v.sub(center,test.center);
  XReal r2 = v.length();
  r2 += test.getRadius();
  setRadius((r2>getRadius()) ? r2 : getRadius() );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XSphere& XSphere::operator=(const XSphere& other)
{
  XM2_ALIGN_ASSERT(&other);
  XM2_ALIGN_ASSERT(this);
  center = other.center;
  radius = other.radius;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::operator==(const XSphere& other) const
{
  XM2_ALIGN_ASSERT(&other);
  XM2_ALIGN_ASSERT(this);
  return equal(other);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XSphere::operator!=(const XSphere& other) const
{
  XM2_ALIGN_ASSERT(&other);
  XM2_ALIGN_ASSERT(this);
  return notEqual(other);
}
}
#endif
