#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR2_ARRAY__INL__
#define XM2_VECTOR2_ARRAY__INL__
namespace XM2
{
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::add(const XVector2Array& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::add(const XVector2& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::add(const XReal b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::sub(const XVector2Array& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::sub(const XVector2& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::sub(const XReal b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::mul(const XVector2Array& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::mul(const XVector2& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::mul(const XReal b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::div(const XVector2Array& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::div(const XVector2& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::div(const XReal b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::add(const XVector2Array& a,const XVector2Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::add(const XVector2Array& a,const XVector2& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::add(const XVector2Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::sub(const XVector2Array& a,const XVector2Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::sub(const XVector2Array& a,const XVector2& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::sub(const XVector2Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::mul(const XVector2Array& a,const XVector2Array& b)
{
  XM2_ASSERT(a.size()==a.size());
  resize(a.size());
  XVector2::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::mul(const XVector2Array& a,const XVector2& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::mul(const XVector2Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::div(const XVector2Array& a,const XVector2Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::div(const XVector2Array& a,const XVector2& b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::div(const XVector2Array& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector2::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::zero()
{
  memset(buffer(),0,size()*sizeof(XVector2));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::dot(XRealArray& product,const XVector2Array& b) const
{
  XM2_ASSERT(size()==b.size());
  product.resize(size());
  XVector2::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::lerp(const XVector2Array& a,const XVector2Array& b,const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XVector2::arrayLerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::bezier(const XVector2Array& a,
                                      const XVector2Array& b,
                                      const XVector2Array& c,
                                      const XVector2Array& d,
                                      const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  XM2_ASSERT(c.size()==d.size());
  XM2_ASSERT(a.size()==c.size());
  resize(a.size());
  XVector2::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::negate()
{
  XVector2::arrayNegate(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::negate(const XVector2Array& v)
{
  resize(v.size());
  XVector2::arrayNegate(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::normalise()
{
  XVector2::arrayNormalise(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector2Array::normalise(const XVector2Array& v)
{
  resize(v.size());
  XVector2::arrayNormalise(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator*=(const XVector2Array& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator/=(const XVector2Array& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator+=(const XVector2Array& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator-=(const XVector2Array& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator*=(const XVector2& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator/=(const XVector2& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator+=(const XVector2& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator-=(const XVector2& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2Array& XVector2Array::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
}
#endif
