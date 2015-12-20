#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_PLANE__INL__
#define XM2_PLANE__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XPlane::XPlane()
{
  zero();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XPlane::XPlane(const XReal _a,const XReal _b,const XReal _c,const XReal _d)
{
  set(_a,_b,_c,_d);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XPlane::XPlane(const XPlane& p)
{
  set(p.a,p.b,p.c,p.d);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XPlane::XPlane(const XVector3& p0,const XVector3& p1,const XVector3& p2)
{
  constructFromPoints(p0,p1,p2);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XPlane::XPlane(const XVector3& point,const XVector3& norm)
{
  constructFromPointAndNormal(point,norm);
}
//----------------------------------------------------------------------------------------------------------------------
bool XPlane::equal(const XPlane& p,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p);
  return float_equal(a,p.a,eps) && 
         float_equal(b,p.b,eps) && 
         float_equal(c,p.c,eps) && 
         float_equal(d,p.d,eps);
}
//----------------------------------------------------------------------------------------------------------------------
bool XPlane::notEqual(const XPlane& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
void XPlane::constructFromPoints(const XVector3& p0,const XVector3& p1,const XVector3& p2)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p0);
  XM2_ALIGN_ASSERT(&p1);
  XM2_ALIGN_ASSERT(&p2);
  XVector3 n;
  XVector3 side0;
  XVector3 side1;
  side0.sub(p2,p1);
  side1.sub(p0,p1);
  n.cross(side1,side0);
  n.normalise();
  set(n.x,n.y,n.z,-n.dot(p1));
}
//----------------------------------------------------------------------------------------------------------------------
void XPlane::constructFromPointAndNormal(const XVector3& point,const XVector3& normal)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&point);
  XM2_ALIGN_ASSERT(&normal);
  XVector3 n = normal;
  n.normalise();
  set(n.x,n.y,n.z,-n.dot(point));
}
//----------------------------------------------------------------------------------------------------------------------
void XPlane::normal(XVector3& _normal)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_normal);
  _normal.set(a,b,c);
}
//----------------------------------------------------------------------------------------------------------------------
XReal XPlane::distance(const XVector3& point)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&point);
  XVector3 n;
  normal(n);
  return n.dot(point) + d;
}
//----------------------------------------------------------------------------------------------------------------------
bool XPlane::lineIntersect(const XRay& ray,XReal& intersect)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&ray);

  XVector3 n;
  XReal dp0,dp1;
  bool ret=0;
  normal(n);
  dp0 = -(n.dot(ray.origin)+d);
  dp1 =   n.dot(ray.direction);
  ret = (XMabs(dp1)>XM2_FLOAT_ZERO) ? true : false;
  dp1 = ret ? dp1 : 1.0f;
  dp0 = ret ? dp0 : 0.0f;
  intersect = dp0/dp1;
  return ret;
}
//----------------------------------------------------------------------------------------------------------------------
bool XPlane::lineIntersectPoint(const XRay& ray,XVector3& outpoint)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&ray);
  XM2_ALIGN_ASSERT(&outpoint);
  XReal dist;
  bool ret = lineIntersect(ray,dist);
  outpoint.mul(ray.direction,dist);
  outpoint.add(ray.origin);
  return ret;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XPlane& XPlane::operator = (const XPlane& p)
{
  set(p.a,p.b,p.c,p.d);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XPlane::operator == (const XPlane& p) const
{
  return equal(p);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XPlane::operator != (const XPlane& p) const
{
  return notEqual(p);
}
};
#endif
