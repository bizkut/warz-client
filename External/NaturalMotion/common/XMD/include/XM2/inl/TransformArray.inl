#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_TRANSFORM_ARRAY__INL__
#define XM2_TRANSFORM_ARRAY__INL__
namespace XM2
{  
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransformArray::XTransformArray() 
  : pod_vector<XTransform>() 
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransformArray::XTransformArray(const XTransformArray& ta)
  : pod_vector<XTransform>(ta) 
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransformArray::XTransformArray(const XQuaternionArray& ta)
  : pod_vector<XTransform>() 
{
  fromQuaternionArray(ta);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XTransformArray::XTransformArray(const XMatrixArray& ta)
  : pod_vector<XTransform>() 
{
  fromMatrixArray(ta);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XTransformArray::equal(const XTransformArray& b,const XReal eps) const
{
  return (size() == b.size()) ? XTransform::arrayEqual(buffer(),b.buffer(),(int)size(),eps) : false;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XTransformArray::notEqual(const XTransformArray& b,const XReal eps) const
{
  return (size() == b.size()) ? XTransform::arrayNotEqual(buffer(),b.buffer(),(int)size(),eps) : false;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::toMatrixArray(XMatrixArray& m) const
{
  m.resize(size());
  XTransform::arrayToMatrixArray(m.buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::fromMatrixArray(const XMatrixArray& m)
{
  resize(m.size());
  XTransform::arrayFromMatrixArray(buffer(),m.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::toQuaternionArray(XQuaternionArray& m) const
{
  m.resize(size());
  XTransform::arrayToQuaternionArray(m.buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::fromQuaternionArray(const XQuaternionArray& m)
{
  resize(m.size());
  XTransform::arrayFromQuaternionArray(buffer(),m.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::lerp(const XTransformArray& a,const XTransformArray& b,const XReal t)
{
  XM2_ASSERT( (a.size() == b.size()) && (size() == a.size()) );
  XTransform::arrayLerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::slerp(const XTransformArray& a,const XTransformArray& b,const XReal t)
{
  XM2_ASSERT( (a.size() == b.size()) && (size() == a.size()) );
  XTransform::arraySlerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::sum(const XTransformArray& a,const XReal t)
{
  XM2_ASSERT(size() == a.size());
  XTransform::arraySum(buffer(),a.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::normalise()
{
  XTransform::arrayNormalise(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::zero()
{
  XTransform::arrayZero(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XTransformArray::identity()
{
  XTransform::arrayIdentity(buffer(),(int)size());
}
}
#endif
