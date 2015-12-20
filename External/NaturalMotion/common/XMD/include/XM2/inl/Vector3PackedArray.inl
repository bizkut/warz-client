#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR3_PACKED_ARRAY__INL__
#define XM2_VECTOR3_PACKED_ARRAY__INL__
namespace XM2
{
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::zero()
{
  XVector3Packed::arrayZero(buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::dot(XRealArray& product,const XVector3PackedArray& b) const
{
  XM2_ASSERT(size()==b.size());
  product.resize(b.size());
  XVector3Packed::arrayDot(product.buffer(),buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::cross(const XVector3PackedArray& a,const XVector3PackedArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(b.size());
  XVector3Packed::arrayCross(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::dot(XRealArray& product,const XVector3Packed& b) const
{
  product.resize(size());
  XVector3Packed::arrayDot(product.buffer(),buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::cross(const XVector3PackedArray& a,const XVector3Packed& b)
{
  resize(a.size());
  XVector3Packed::arrayCross(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::lerp(const XVector3PackedArray& a,const XVector3PackedArray& b,const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XVector3Packed::arrayLerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::bezier(const XVector3PackedArray& a,
                                            const XVector3PackedArray& b,
                                            const XVector3PackedArray& c,
                                            const XVector3PackedArray& d,
                                            const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  XM2_ASSERT(c.size()==d.size());
  XM2_ASSERT(a.size()==c.size());
  resize(a.size());
  XVector3Packed::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::negate()
{
  XVector3Packed::arrayNegate(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::negate(const XVector3PackedArray& v)
{
  resize(v.size());
  XVector3Packed::arrayNegate(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::normalise()
{
  XVector3Packed::arrayNormalise(buffer(),buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::normalise(const XVector3PackedArray& v)
{
  resize(v.size());
  XVector3Packed::arrayNormalise(buffer(),v.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3PackedArray& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3Array& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3Packed& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XReal b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3PackedArray& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3Array& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3Packed& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XReal b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3PackedArray& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3Array& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3Packed& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XReal b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3PackedArray& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3Array& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3Packed& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XReal b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3PackedArray& a,const XVector3PackedArray& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3PackedArray& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3PackedArray& a,const XVector3Packed& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3PackedArray& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::add(const XVector3PackedArray& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3PackedArray& a,const XVector3PackedArray& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3PackedArray& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3PackedArray& a,const XVector3Packed& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3PackedArray& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::sub(const XVector3PackedArray& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arraySub(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3PackedArray& a,const XVector3PackedArray& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3PackedArray& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3PackedArray& a,const XVector3Packed& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3PackedArray& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::mul(const XVector3PackedArray& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3PackedArray& a,const XVector3PackedArray& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3PackedArray& a,const XVector3Array& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3PackedArray& a,const XVector3Packed& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3PackedArray& a,const XVector3& b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3PackedArray::div(const XVector3PackedArray& a,const XReal b)
{
  XM2_ASSERT(size()==a.size());
  XVector3Packed::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator*=(const XVector3PackedArray& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator/=(const XVector3PackedArray& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator+=(const XVector3PackedArray& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator-=(const XVector3PackedArray& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator*=(const XVector3Array& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator/=(const XVector3Array& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator+=(const XVector3Array& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator-=(const XVector3Array& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator*=(const XVector3Packed& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator/=(const XVector3Packed& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator+=(const XVector3Packed& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator-=(const XVector3Packed& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator*=(const XVector3& rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator/=(const XVector3& rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator+=(const XVector3& rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator-=(const XVector3& rhs)
{
  sub(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3PackedArray& XVector3PackedArray::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
}
#endif
