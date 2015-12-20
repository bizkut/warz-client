#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR3SOA_ARRAY__INL__
#define XM2_VECTOR3SOA_ARRAY__INL__
namespace XM2
{  
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3SOAArray& a,const XVector3SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector3SOA::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3SOAArray& a,const XVector3SOA& b)
{
  resize(a.size());
  XVector3SOA::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3SOAArray& a,const XVector3& b)
{
  resize(a.size());
  XVector3SOA::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector3SOA::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3SOAArray& a,const XVector3SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector3SOA::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3SOAArray& a,const XVector3SOA& b)
{
  resize(a.size());
  XVector3SOA::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3SOAArray& a,const XVector3& b)
{
  resize(a.size());
  XVector3SOA::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector3SOA::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3SOAArray& a,const XVector3SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector3SOA::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3SOAArray& a,const XVector3SOA& b)
{
  resize(a.size());
  XVector3SOA::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3SOAArray& a,const XVector3& b)
{
  resize(a.size());
  XVector3SOA::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector3SOA::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3SOAArray& a,const XVector3SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector3SOA::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3SOAArray& a,const XVector3SOA& b)
{
  resize(a.size());
  XVector3SOA::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3SOAArray& a,const XVector3& b)
{
  resize(a.size());
  XVector3SOA::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector3SOA::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector3SOA::arrayAdd(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3SOA& b)
{
  XVector3SOA::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XVector3& b)
{
  XVector3SOA::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::add(const XReal b)
{
  XVector3SOA::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector3SOA::arraySub(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3SOA& b)
{
  XVector3SOA::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XVector3& b)
{
  XVector3SOA::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::sub(const XReal b)
{
  XVector3SOA::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector3SOA::arrayMul(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3SOA& b)
{
  XVector3SOA::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XVector3& b)
{
  XVector3SOA::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::mul(const XReal b)
{
  XVector3SOA::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector3SOA::arrayDiv(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3SOA& b)
{
  XVector3SOA::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XVector3& b)
{
  XVector3SOA::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::div(const XReal b)
{
  XVector3SOA::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::normalise()
{
  XVector3SOA::arrayNormalise(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::normalise(const XVector3SOAArray& v)
{
  resize(v.size());
  XVector3SOA::arrayNormalise(buffer(),v.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::zero()
{
  XVector3SOA::arrayZero(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::dot(XVector4Array& product,const XVector3SOAArray& b) const
{
  XM2_ASSERT( size() == b.size() );
  product.resize(size());
  XVector3SOA::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::cross(const XVector3SOAArray& a,const XVector3SOAArray& b)
{
  XM2_ASSERT( a.size() == b.size() );
  resize(b.size());
  XVector3SOA::arrayCross(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::lerp(const XVector3SOAArray& a,const XVector3SOAArray& b,const XReal t)
{
  XM2_ASSERT( a.size() == b.size() );
  resize(a.size());
  XVector3SOA::arrayLerp(buffer(), a.buffer(),b.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::bezier(const XVector3SOAArray& a,
                                         const XVector3SOAArray& b,
                                         const XVector3SOAArray& c,
                                         const XVector3SOAArray& d,
                                         const XReal t)
{
  XM2_ASSERT( a.size() == b.size() );
  XM2_ASSERT( c.size() == d.size() );
  XM2_ASSERT( a.size() == c.size() );
  resize(a.size());
  XVector3SOA::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::negate()
{
  XVector3SOA::arrayNegate(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOAArray::negate(const XVector3SOAArray& v)
{
  XVector3SOA::arrayNegate(buffer(),v.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator*=(const XVector3SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector3SOA::arrayMul(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator/=(const XVector3SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector3SOA::arrayDiv(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator+=(const XVector3SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector3SOA::arrayAdd(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator-=(const XVector3SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector3SOA::arraySub(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator*=(const XVector3SOA& rhs)
{
  XVector3SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator/=(const XVector3SOA& rhs)
{
  XVector3SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator+=(const XVector3SOA& rhs)
{
  XVector3SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator-=(const XVector3SOA& rhs)
{
  XVector3SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator*=(const XVector3& rhs)
{
  XVector3SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator/=(const XVector3& rhs)
{
  XVector3SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator+=(const XVector3& rhs)
{
  XVector3SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator-=(const XVector3& rhs)
{
  XVector3SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator*=(const XReal rhs)
{
  XVector3SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator/=(const XReal rhs)
{
  XVector3SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator+=(const XReal rhs)
{
  XVector3SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector3SOAArray& XVector3SOAArray::operator-=(const XReal rhs)
{
  XVector3SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
}
#endif
