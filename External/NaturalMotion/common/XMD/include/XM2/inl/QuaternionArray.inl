#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_QUATERNION_ARRAY__INL__
#define XM2_QUATERNION_ARRAY__INL__
namespace XM2
{  
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionArray::XQuaternionArray()
  : pod_vector< XQuaternion >()
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionArray::XQuaternionArray(const XQuaternionArray& rhs)
  : pod_vector< XQuaternion >(rhs)
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionArray::XQuaternionArray(const XMatrixArray& rhs)
  : pod_vector< XQuaternion >()
{
  *this = rhs;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionArray::XQuaternionArray(const XTransformArray& rhs)
  : pod_vector< XQuaternion >()
{
  *this = rhs;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionArray::operator == (const XQuaternionArray& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionArray::operator != (const XQuaternionArray& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XQuaternionArray& a,const XQuaternionArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XQuaternion::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XQuaternionArray& a,const XQuaternion& b)
{
  resize(a.size());
  XQuaternion::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XQuaternionArray& a,const XReal& b)
{
  resize(a.size());
  XQuaternion::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XQuaternionArray& a)
{
  XM2_ASSERT(a.size()==size());
  XQuaternion::arrayMul(buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XQuaternion& a)
{
  XQuaternion::arrayMul(buffer(),a,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XQuaternionArray& a,const XRealArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XQuaternion::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XRealArray& b)
{
  XM2_ASSERT(b.size()==size());
  XQuaternion::arrayMul(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::mul(const XReal b)
{
  XQuaternion::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionArray::equal(const XQuaternionArray& b,const XReal eps) const
{
  return (size() == b.size()) ? XQuaternion::arrayEqual(buffer(),b.buffer(),(int)size(),eps) : false;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionArray::notEqual(const XQuaternionArray& b,const XReal eps) const
{
  return (size() == b.size()) ? XQuaternion::arrayNotEqual(buffer(),b.buffer(),(int)size(),eps) : false;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::fromMatrixArray(const XMatrixArray& mat)
{
  resize(mat.size());
  XQuaternion::arrayFromMatrixArray(buffer(),mat.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::toMatrixArray(XMatrixArray& mat) const
{
  mat.resize(size());
  XQuaternion::arrayToMatrixArray(mat.buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::fromTransformArray(const XTransformArray& mat)
{
  resize(mat.size());
  XQuaternion::arrayFromTransformArray(buffer(),mat.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::toTransformArray(XTransformArray& mat) const
{
  mat.resize(size());
  XQuaternion::arrayToTransformArray(mat.buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::dot(XRealArray& product,const XQuaternionArray& b) const
{
  XM2_ASSERT(size()==b.size());
  product.resize(size());
  XQuaternion::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator=(const XMatrixArray& rhs)
{
  fromMatrixArray(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator=(const XTransformArray& rhs)
{
  resize(rhs.size());
  iterator it = begin();
  XTransformArray::const_iterator itm = rhs.begin();
  for (;it != end();++it,++itm)
  {
    *it = itm->rotate;
  }
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XMatrixArray& rhs)
{
  XM2_ASSERT(rhs.size() == size());
  (void)rhs;
  //XQuaternion::arrayMul(buffer(),buffer(),rhs.buffer(),(int)size());
  /// \todo implement
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XTransformArray& rhs)
{
  XM2_ASSERT(rhs.size() == size());
  (void)rhs;
  //XQuaternion::arrayMul(buffer(),buffer(),rhs.buffer(),(int)size());
  /// \todo implement
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XQuaternionArray& rhs)
{
  XM2_ASSERT(rhs.size() == size());
  XQuaternion::arrayMul(buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XMatrix& rhs)
{
  XQuaternion q;
  rhs.toQuaternion(q);
  XQuaternion::arrayMul(buffer(),buffer(),q,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XTransform& rhs)
{
  XQuaternion::arrayMul(buffer(),rhs.rotate,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XQuaternion& rhs)
{
  XQuaternion::arrayMul(buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator+=(const XQuaternion& rhs)
{
  XQuaternion::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator+=(const XQuaternionArray& rhs)
{
  XM2_ASSERT(rhs.size() == size());
  XQuaternion::arrayAdd(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XReal rhs)
{
  XQuaternion::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionArray& XQuaternionArray::operator*=(const XRealArray& rhs)
{
  XM2_ASSERT(size()==rhs.size());
  XQuaternion::arrayMul(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::lerp(const XQuaternionArray& q0,const XQuaternionArray& q1,const XReal t)
{
  XM2_ASSERT(q0.size() == q1.size());
  resize(q0.size());
  XQuaternion::arrayLerp(buffer(),q0.buffer(),q1.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::slerp(const XQuaternionArray& q0,const XQuaternionArray& q1,const XReal t)
{
  XM2_ASSERT(q0.size() == q1.size());
  resize(q0.size());
  XQuaternion::arraySlerp(buffer(),q0.buffer(),q1.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::sum(const XQuaternionArray& q,const XReal w)
{
  XM2_ASSERT(size() ==  q.size());
  XQuaternion::arraySum(buffer(),q.buffer(),w,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::normalise()
{
  XQuaternion::arrayNormalise(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::identity()
{
  XQuaternion::arrayIdentity(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::zero()
{
  XQuaternion::arrayZero(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::invert()
{
  XQuaternion::arrayInvert(buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::invert(const XQuaternionArray& q)
{
  resize(q.size());
  XQuaternion::arrayInvert(buffer(),q.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::negate()
{
  XQuaternion::arrayNegate(buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionArray::negate(const XQuaternionArray& q)
{
  resize(q.size());
  XQuaternion::arrayNegate(buffer(),q.buffer(),(int)size());
}
#pragma warning(pop)
}
#endif
