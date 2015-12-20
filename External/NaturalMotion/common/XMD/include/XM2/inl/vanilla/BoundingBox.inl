#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_BOUNDING_BOX__INL__
#define XM2_VANILLA_BOUNDING_BOX__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3& p)
{
  minimum.x = XMmin(minimum.x,p.x);
  minimum.y = XMmin(minimum.y,p.y);
  minimum.z = XMmin(minimum.z,p.z);
  maximum.x = XMmax(maximum.x,p.x);
  maximum.y = XMmax(maximum.y,p.y);
  maximum.z = XMmax(maximum.z,p.z);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector4& p)
{
  minimum.x = XMmin(minimum.x,p.x);
  minimum.y = XMmin(minimum.y,p.y);
  minimum.z = XMmin(minimum.z,p.z);
  maximum.x = XMmax(maximum.x,p.x);
  maximum.y = XMmax(maximum.y,p.y);
  maximum.z = XMmax(maximum.z,p.z);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3Packed& p)
{
  minimum.x = XMmin(minimum.x,p.x);
  minimum.y = XMmin(minimum.y,p.y);
  minimum.z = XMmin(minimum.z,p.z);
  maximum.x = XMmax(maximum.x,p.x);
  maximum.y = XMmax(maximum.y,p.y);
  maximum.z = XMmax(maximum.z,p.z);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XBoundingBox& box)
{
  minimum.x = XMmin(minimum.x,box.minimum.x);
  minimum.y = XMmin(minimum.y,box.minimum.y);
  minimum.z = XMmin(minimum.z,box.minimum.z);
  maximum.x = XMmax(maximum.x,box.maximum.x);
  maximum.y = XMmax(maximum.y,box.maximum.y);
  maximum.z = XMmax(maximum.z,box.maximum.z);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector3& point) const
{
  return ((minimum.x<=point.x)&&(maximum.x>=point.x)) &&
         ((minimum.y<=point.y)&&(maximum.y>=point.y)) &&
         ((minimum.z<=point.z)&&(maximum.z>=point.z));
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector3Packed& point) const
{
  return ((minimum.x<=point.x)&&(maximum.x>=point.x)) &&
         ((minimum.y<=point.y)&&(maximum.y>=point.y)) &&
         ((minimum.z<=point.z)&&(maximum.z>=point.z));
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector4& point) const
{
  return ((minimum.x<=point.x)&&(maximum.x>=point.x)) &&
         ((minimum.y<=point.y)&&(maximum.y>=point.y)) &&
         ((minimum.z<=point.z)&&(maximum.z>=point.z));
}
}
#endif
