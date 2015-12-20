#pragma once
namespace XM2
{
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::add(const XVector3Array& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::add(const XVector3& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::add(const XReal b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::sub(const XVector3Array& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::sub(const XVector3& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::sub(const XReal b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::mul(const XVector3Array& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::mul(const XVector3& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::mul(const XReal b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::div(const XVector3Array& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::div(const XVector3& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::div(const XReal b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::add(const XVector3Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::add(const XVector3Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::add(const XVector3Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::sub(const XVector3Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::sub(const XVector3Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::sub(const XVector3Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::mul(const XVector3Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::mul(const XVector3Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::mul(const XVector3Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::div(const XVector3Array& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::div(const XVector3Array& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::div(const XVector3Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::zero()
{
  memset(buffer(),0,size()*sizeof(XVector3));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::dot(XRealArray& product,const XVector3Array& b) const
{
  XM2_ASSERT(size()==b.size());
  product.resize(size());
  XVector3::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::cross(const XVector3Array& a,const XVector3Array& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XVector3::arrayCross(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::dot(XRealArray& product,const XVector3& b) const
{
  product.resize(size());
  XVector3::arrayDot(product.buffer(),buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::cross(const XVector3Array& a,const XVector3& b)
{
  resize(a.size());
  XVector3::arrayCross(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::lerp(const XVector3Array& a,const XVector3Array& b,const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XVector3::arrayLerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::bezier(const XVector3Array& a,
                                      const XVector3Array& b,
                                      const XVector3Array& c,
                                      const XVector3Array& d,
                                      const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  XM2_ASSERT(c.size()==d.size());
  XM2_ASSERT(a.size()==c.size());
  resize(a.size());
  XVector3::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::negate()
{
  XVector3::arrayNegate(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::negate(const XVector3Array& v)
{
  resize(v.size());
  XVector3::arrayNegate(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::normalise()
{
  XVector3::arrayNormalise(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3Array::normalise(const XVector3Array& v)
{
  resize(v.size());
  XVector3::arrayNormalise(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator*=(const XVector3Array& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator/=(const XVector3Array& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator+=(const XVector3Array& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator-=(const XVector3Array& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator*=(const XVector3& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator/=(const XVector3& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator+=(const XVector3& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator-=(const XVector3& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3Array& XVector3Array::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
}
