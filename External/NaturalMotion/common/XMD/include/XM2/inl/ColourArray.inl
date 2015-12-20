#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_COLOUR_ARRAY__INL__
#define XM2_COLOUR_ARRAY__INL__
namespace XM2
{  
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::zero()
{
  XColour::arrayZero(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::white()
{
  XColour::arrayWhite(buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColourArray::equal(const XColourArray& b,const XReal eps) const
{
  return XColour::arrayEqual(buffer(),b.buffer(),(int)b.size(),eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColourArray::notEqual(const XColourArray& b,const XReal eps) const
{
  return XColour::arrayNotEqual(buffer(),b.buffer(),(int)b.size(),eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::mul(const XColourArray& a,const XColourArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XColour::arrayMul(buffer(),a.buffer(),b.buffer(),(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::div(const XColourArray& a,const XColourArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XColour::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sub(const XColourArray& a,const XColourArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XColour::arraySub(buffer(),a.buffer(),b.buffer(),(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::add(const XColourArray& a,const XColourArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XColour::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::mul(const XColourArray& a,const XColour& b)
{
  resize(a.size());
  XColour::arrayMul(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::div(const XColourArray& a,const XColour& b)
{
  resize(a.size());
  XColour::arrayDiv(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sub(const XColourArray& a,const XColour& b)
{
  resize(a.size());
  XColour::arraySub(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::add(const XColourArray& a,const XColour& b)
{
  resize(a.size());
  XColour::arrayAdd(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::mul(const XColourArray& a,const XReal b)
{
  resize(a.size());
  XColour::arrayMul(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::div(const XColourArray& a,const XReal b)
{
  resize(a.size());
  XColour::arrayDiv(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sub(const XColourArray& a,const XReal b)
{
  resize(a.size());
  XColour::arraySub(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::add(const XColourArray& a,const XReal b)
{
  resize(a.size());
  XColour::arrayAdd(buffer(),a.buffer(),b,(int)a.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::mul(const XColourArray& b)
{
  XM2_ASSERT( b.size() == size() );
  XColour::arrayMul(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::div(const XColourArray& b)
{
  XM2_ASSERT( b.size() == size() );
  XColour::arrayDiv(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sub(const XColourArray& b)
{
  XM2_ASSERT( b.size() == size() );
  XColour::arraySub(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::add(const XColourArray& b)
{
  XM2_ASSERT( b.size() == size() );
  XColour::arrayAdd(buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::mul(const XColour& b)
{
  XColour::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::div(const XColour& b)
{
  XColour::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sub(const XColour& b)
{
  XColour::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::add(const XColour& b)
{
  XColour::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::mul(const XReal b)
{
  XColour::arrayMul(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::div(const XReal b)
{
  XColour::arrayDiv(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sub(const XReal b)
{
  XColour::arraySub(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::add(const XReal b)
{
  XColour::arrayAdd(buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::invert(const XColourArray& a)
{
  resize(a.size());
  XColour::arrayInvert(buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::invert()
{
  XColour::arrayInvert(buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::negate(const XColourArray& a)
{
  resize(a.size());
  XColour::arrayNegate(buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::negate()
{
  XColour::arrayNegate(buffer(),buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::lerp(const XColourArray& a,const XColourArray& b,const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XColour::arrayLerp(buffer(),a.buffer(),b.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::bezier(const XColourArray& a,
                                     const XColourArray& b,
                                     const XColourArray& c,
                                     const XColourArray& d,
                                     const XReal t)
{
  XM2_ASSERT(a.size()==b.size());
  XM2_ASSERT(c.size()==d.size());
  XM2_ASSERT(b.size()==c.size());
  resize(a.size());
  XColour::arrayBezier(buffer(),a.buffer(),b.buffer(),c.buffer(),d.buffer(),t,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sum(const XColourArray& a,const XReal weight)
{
  XM2_ASSERT(a.size()==size());
  XColour::arraySum(buffer(),a.buffer(),weight,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColourArray::sum(const XColour& a,const XReal weight)
{
  XColour::arraySum(buffer(),a,weight,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColourArray::operator==(const XColourArray& ca) const
{
  return equal(ca);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColourArray::operator!=(const XColourArray& ca) const
{
  return !equal(ca);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator*=(const XColourArray& rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator/=(const XColourArray& rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator+=(const XColourArray& rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator-=(const XColourArray& rhs)
{
  sub(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator*=(const XColour& rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator/=(const XColour& rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator+=(const XColour& rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator-=(const XColour& rhs)
{
  sub(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColourArray& XColourArray::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
}
#endif
