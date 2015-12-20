#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_TRANSFORM__INL__
#define XM2_TRANSFORM__INL__
#include "XM2/Vector4.h"
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransform::XTransform()
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_INIT_CLASSES
    identity();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransform::XTransform(const XTransform& rhs)
  : translate(rhs.translate), rotate(rhs.rotate),scale(rhs.scale)
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransform::XTransform(const XMatrix& rhs)
{
  fromMatrix(rhs);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XTransform::equal(const XTransform& b,const XReal eps) const
{
  return translate.equal(b.translate,eps) && 
         rotate.equal(b.rotate,eps) && 
         scale.equal(b.scale,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XTransform::notEqual(const XTransform& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::toMatrix(XMatrix& output) const
{
  // generate rotation matrix
  output.fromQuaternion(rotate);
  // set scaling
  output.xAxis().mul(scale.x);
  output.yAxis().mul(scale.y);
  output.zAxis().mul(scale.z);
  // set translate
  output.wAxis().set(translate.x,translate.y,translate.z,1.0f);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::fromMatrix(const XMatrix& input)
{
  input.decompose(translate,rotate,scale);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::identity()
{
  scale.set(1.0f,1.0f,1.0f);
  translate.zero();
  rotate.identity();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XTransform::operator == (const XTransform& p) const
{
  return equal(p);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XTransform::operator != (const XTransform& p) const
{
  return notEqual(p);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::zero()
{
  translate.zero();
  scale.zero();
  rotate.zero();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransform::sum(const XTransform& a,const XReal weight)
{
  translate.sum(a.translate,weight);
  scale.sum(a.scale,weight);
  rotate.sum(a.rotate,weight);
}
}
#endif
