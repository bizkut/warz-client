#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_BOUNDINGBOX__INL__
#define XM2_BOUNDINGBOX__INL__
#include <float.h>
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XBoundingBox::XBoundingBox()
{
  #if XM2_INIT_CLASSES
  clear();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XBoundingBox::XBoundingBox(const XBoundingBox& src)
  : minimum(src.minimum),maximum(src.maximum)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&src);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XBoundingBox::XBoundingBox(const XVector3& _min,const XVector3& _max)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_min);
  XM2_ALIGN_ASSERT(&_max);
  minimum = _min;
  maximum = _max;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XBoundingBox::equal(const XBoundingBox& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  return maximum.equal(b.maximum,eps) &&  
         minimum.equal(b.minimum,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::notEqual(const XBoundingBox& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::clear()
{
  XM2_ALIGN_ASSERT(this);
  minimum.set(FLT_MAX,FLT_MAX,FLT_MAX);
  maximum.set(FLT_MIN,FLT_MIN,FLT_MIN);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XBoundingBox::width() const
{
  XM2_ALIGN_ASSERT(this);

  return maximum.x-minimum.x;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XBoundingBox::height() const
{
  XM2_ALIGN_ASSERT(this);

  return maximum.y-minimum.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XBoundingBox::depth() const
{
  XM2_ALIGN_ASSERT(this);

  return maximum.z-minimum.z;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::center(XVector3& out_center) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&out_center);

  out_center.add(maximum,minimum);
  out_center.mul(0.5f);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XBoundingBox& XBoundingBox::operator=(const XBoundingBox& other)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&other);
  minimum = other.minimum;
  maximum = other.maximum;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XBoundingBox::operator==(const XBoundingBox& other) const
{
  return equal(other);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XBoundingBox::operator!=(const XBoundingBox& other) const
{
  return notEqual(other);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3SOA& p)
{
  expand(p.get0());
  expand(p.get1());
  expand(p.get2());
  expand(p.get3());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector4SOA& p)
{
  expand(p.get0());
  expand(p.get1());
  expand(p.get2());
  expand(p.get3());
}
}
#endif
