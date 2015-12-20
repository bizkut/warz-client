#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR4SOA_ARRAY__INL__
#define XM2_VECTOR4SOA_ARRAY__INL__
namespace XM2
{  
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4SOAArray& a,const XVector4SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector4SOA::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4SOAArray& a,const XVector4SOA& b)
{
  resize(a.size());
  XVector4SOA::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4SOAArray& a,const XVector4& b)
{
  resize(a.size());
  XVector4SOA::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector4SOA::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4SOAArray& a,const XVector4SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector4SOA::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4SOAArray& a,const XVector4SOA& b)
{
  resize(a.size());
  XVector4SOA::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4SOAArray& a,const XVector4& b)
{
  resize(a.size());
  XVector4SOA::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector4SOA::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4SOAArray& a,const XVector4SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector4SOA::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4SOAArray& a,const XVector4SOA& b)
{
  resize(a.size());
  XVector4SOA::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4SOAArray& a,const XVector4& b)
{
  resize(a.size());
  XVector4SOA::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector4SOA::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4SOAArray& a,const XVector4SOAArray& b)
{
  XM2_ASSERT(a.size() == b.size());
  resize(a.size());
  XVector4SOA::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4SOAArray& a,const XVector4SOA& b)
{
  resize(a.size());
  XVector4SOA::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4SOAArray& a,const XVector4& b)
{
  resize(a.size());
  XVector4SOA::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4SOAArray& a,const XReal b)
{
  resize(a.size());
  XVector4SOA::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector4SOA::arrayAdd(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4SOA& b)
{
  XVector4SOA::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XVector4& b)
{
  XVector4SOA::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::add(const XReal b)
{
  XVector4SOA::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector4SOA::arraySub(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4SOA& b)
{
  XVector4SOA::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XVector4& b)
{
  XVector4SOA::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::sub(const XReal b)
{
  XVector4SOA::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector4SOA::arrayMul(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4SOA& b)
{
  XVector4SOA::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XVector4& b)
{
  XVector4SOA::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::mul(const XReal b)
{
  XVector4SOA::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4SOAArray& b)
{
  XM2_ASSERT(size() == b.size());
  XVector4SOA::arrayDiv(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4SOA& b)
{
  XVector4SOA::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XVector4& b)
{
  XVector4SOA::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::div(const XReal b)
{
  XVector4SOA::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::normalise()
{
  XVector4SOA::arrayNormalise(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::normalise(const XVector4SOAArray& v)
{
  resize(v.size());
  XVector4SOA::arrayNormalise(buffer(),v.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::zero()
{
  XVector4SOA::arrayZero(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::dot(XVector4Array& product,const XVector4SOAArray& b) const
{
  XM2_ASSERT( size() == b.size() );
  product.resize(size());
  XVector4SOA::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::dot(XVector4Array& product,const XVector4SOA& b) const
{
  product.resize(size());
  XVector4SOA::arrayDot(product.buffer(),buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::dot(XVector4Array& product,const XVector4& b) const
{
  product.resize(size());
  XVector4SOA::arrayDot(product.buffer(),buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::cross(const XVector4SOAArray& a,const XVector4SOAArray& b)
{
  XM2_ASSERT( a.size() == b.size() );
  resize(b.size());
  XVector4SOA::arrayCross(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::cross(const XVector4SOAArray& a,const XVector4SOA& b)
{
  resize(a.size());
  XVector4SOA::arrayCross(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::cross(const XVector4SOAArray& a,const XVector4& b)
{
  resize(a.size());
  XVector4SOA::arrayCross(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::lerp(const XVector4SOAArray& a,const XVector4SOAArray& b,const XReal t)
{
  XM2_ASSERT( a.size() == b.size() );
  resize(a.size());
  XVector4SOA::arrayLerp(buffer(), a.buffer(),b.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::bezier(const XVector4SOAArray& a,
                                         const XVector4SOAArray& b,
                                         const XVector4SOAArray& c,
                                         const XVector4SOAArray& d,
                                         const XReal t)
{
  XM2_ASSERT( a.size() == b.size() );
  XM2_ASSERT( c.size() == d.size() );
  XM2_ASSERT( a.size() == c.size() );
  resize(a.size());
  XVector4SOA::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::negate()
{
  XVector4SOA::arrayNegate(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOAArray::negate(const XVector4SOAArray& v)
{
  XVector4SOA::arrayNegate(buffer(),v.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator*=(const XVector4SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector4SOA::arrayMul(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator/=(const XVector4SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector4SOA::arrayDiv(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator+=(const XVector4SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector4SOA::arrayAdd(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator-=(const XVector4SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector4SOA::arraySub(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator*=(const XVector4SOA& rhs)
{
  XVector4SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator/=(const XVector4SOA& rhs)
{
  XVector4SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator+=(const XVector4SOA& rhs)
{
  XVector4SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator-=(const XVector4SOA& rhs)
{
  XVector4SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator*=(const XVector4& rhs)
{
  XVector4SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator/=(const XVector4& rhs)
{
  XVector4SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator+=(const XVector4& rhs)
{
  XVector4SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator-=(const XVector4& rhs)
{
  XVector4SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator*=(const XReal rhs)
{
  XVector4SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator/=(const XReal rhs)
{
  XVector4SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator+=(const XReal rhs)
{
  XVector4SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector4SOAArray& XVector4SOAArray::operator-=(const XReal rhs)
{
  XVector4SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
}
#endif
