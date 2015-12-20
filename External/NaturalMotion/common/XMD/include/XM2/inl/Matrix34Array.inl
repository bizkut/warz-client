#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_MATRIX_ARRAY__INL__
#define XM2_MATRIX_ARRAY__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::identity()
{
  XMatrix::arrayIdentity(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::zero()
{
  XMatrix::arrayZero(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrixArray::equal(const XMatrixArray& b,const XReal eps) const
{
  return XMatrix::arrayEqual(buffer(),b.buffer(),(int)size(),eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrixArray::notEqual(const XMatrixArray& b,const XReal eps) const
{
  return XMatrix::arrayNotEqual(buffer(),b.buffer(),(int)size(),eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::toQuaternionArray(XQuaternionArray& output) const
{
  output.resize(size());
  XMatrix::arrayToQuaternionArray(output.buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::fromQuaternionArray(const XQuaternionArray& q)
{
  resize(q.size());
  XMatrix::arrayFromQuaternionArray(buffer(),q.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::toTransformArray(XTransformArray& output) const
{
  output.resize(size());
  XMatrix::arrayToTransformArray(output.buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::fromTransformArray(const XTransformArray& q)
{
  resize(q.size());
  XMatrix::arrayFromTransformArray(buffer(),q.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::invert(const XMatrixArray& a)
{
  resize(a.size());
  XMatrix::arrayInvert(buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::invert()
{
  XMatrix::arrayInvert(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::transpose(const XMatrixArray& a)
{
  resize(a.size());
  XMatrix::arrayTranspose(buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::transpose()
{
  XMatrix::arrayTranspose(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::mul(const XMatrixArray& a,const XMatrixArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XMatrix::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::quickMult(const XMatrixArray& a,const XMatrixArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XMatrix::arrayQuickMult(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::mul(const XMatrixArray& a,const XMatrix& b)
{
  resize(a.size());
  XMatrix::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrixArray::quickMult(const XMatrixArray& a,const XMatrix& b)
{
  resize(a.size());
  XMatrix::arrayQuickMult(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrixArray::XMatrixArray()
  : pod_vector< XMatrix >()
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrixArray::XMatrixArray(const XQuaternionArray& rhs)
  : pod_vector< XMatrix >()
{
  resize(rhs.size());
  XMatrix::arrayFromQuaternionArray(buffer(),rhs.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrixArray::XMatrixArray(const XTransformArray& rhs)
  : pod_vector< XMatrix >()
{
  resize(rhs.size());
  XMatrix::arrayFromTransformArray(buffer(),rhs.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrixArray::XMatrixArray(const XMatrixArray& rhs)
  : pod_vector< XMatrix >(rhs)
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrixArray& XMatrixArray::operator=(const XQuaternionArray& rhs)
{
  resize(rhs.size());
  XMatrix::arrayFromQuaternionArray(buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrixArray& XMatrixArray::operator=(const XTransformArray& rhs)
{
  resize(rhs.size());
  iterator it = begin();
  XTransformArray::const_iterator itt = rhs.begin();
  for (;it != end();++it,++itt)
  {
    itt->toMatrix(*it);
  }
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrixArray& XMatrixArray::operator*=(const XMatrixArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XMatrixArray temp = *this;
  XMatrix::arrayMul(buffer(),temp.buffer(),rhs.buffer(),(int)size());   
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrixArray& XMatrixArray::operator*=(const XMatrix& rhs)
{    
  XMatrixArray temp = *this;
  XMatrix::arrayMul(buffer(),&temp[0],rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrixArray& XMatrixArray::operator*=(const XReal rhs)
{
  XMatrix::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrixArray& XMatrixArray::operator/=(const XReal rhs)
{
  XMatrix::arrayMul(buffer(),buffer(),1.0f/rhs,(int)size());
  return *this;
}
}

#endif
