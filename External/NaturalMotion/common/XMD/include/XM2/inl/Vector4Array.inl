#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR4_ARRAY__INL__
#define XM2_VECTOR4_ARRAY__INL__
namespace XM2
{
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4Array& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector3Array& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector3& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XReal b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4Array& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector3Array& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector3& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XReal b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4Array& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector3Array& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector3& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XReal b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4Array& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector3Array& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector3& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XReal b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4Array& a,const XVector4Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4Array& a,const XVector4& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::add(const XVector4Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4Array& a,const XVector4Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4Array& a,const XVector4& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::sub(const XVector4Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4Array& a,const XVector4Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4Array& a,const XVector4& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::mul(const XVector4Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4Array& a,const XVector4Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4Array& a,const XVector4& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::div(const XVector4Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector4::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::zero()
{
  memset(buffer(),0,size()*sizeof(XVector4));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::dot(XRealArray& product,const XVector4Array& b) const
{
  XM2_ASSERT(size()==b.size());
  product.resize(size());
  XVector4::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::cross(const XVector4Array& a,const XVector4Array& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XVector4::arrayCross(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::lerp(const XVector4Array& a,const XVector4Array& b,const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XVector4::arrayLerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::bezier(const XVector4Array& a,
                                      const XVector4Array& b,
                                      const XVector4Array& c,
                                      const XVector4Array& d,
                                      const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  XM2_ASSERT(c.size()==d.size());
  XM2_ASSERT(a.size()==c.size());
  resize(a.size());
  XVector4::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::negate()
{
  XVector4::arrayNegate(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::negate(const XVector4Array& v)
{
  resize(v.size());
  XVector4::arrayNegate(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::normalise()
{
  XVector4::arrayNormalise(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4Array::normalise(const XVector4Array& v)
{
  resize(v.size());
  XVector4::arrayNormalise(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator*=(const XVector4Array& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator/=(const XVector4Array& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator+=(const XVector4Array& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator-=(const XVector4Array& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator*=(const XVector3Array& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator/=(const XVector3Array& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator+=(const XVector3Array& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator-=(const XVector3Array& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator*=(const XVector4& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator/=(const XVector4& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator+=(const XVector4& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator-=(const XVector4& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator*=(const XVector3& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator/=(const XVector3& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator+=(const XVector3& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator-=(const XVector3& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4Array& XVector4Array::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
}
#endif
