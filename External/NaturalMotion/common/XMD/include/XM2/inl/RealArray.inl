#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_REALARRAY__INL__
#define XM2_REALARRAY__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arrayMul(XM2::XReal* a,const XM2::XReal b,const int num)
{
  arrayMul(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arrayMul(XM2::XReal* a,const XM2::XReal* b,const int num)
{
  arrayMul(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arrayDiv(XM2::XReal* a,const XM2::XReal b,const int num)
{
  arrayDiv(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arrayDiv(XM2::XReal* a,const XM2::XReal* b,const int num)
{
  arrayDiv(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arrayAdd(XM2::XReal* a,const XM2::XReal b,const int num)
{
  arrayAdd(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arrayAdd(XM2::XReal* a,const XM2::XReal* b,const int num)
{
  arrayAdd(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arraySub(XM2::XReal* a,const XM2::XReal b,const int num)
{
  arraySub(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::arraySub(XM2::XReal* a,const XM2::XReal* b,const int num)
{
  arraySub(a,a,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XRealArray::operator==(const XRealArray& other) const
{
  return equal(other);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XRealArray::operator!=(const XRealArray& other) const
{
  return notEqual(other);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator*=(const XRealArray& rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator/=(const XRealArray& rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator+=(const XRealArray& rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator-=(const XRealArray& rhs)
{
  sub(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::mul(const XRealArray& a,const XRealArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XRealArray::arrayMul(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::div(const XRealArray& a,const XRealArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XRealArray::arrayDiv(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::sub(const XRealArray& a,const XRealArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XRealArray::arraySub(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::add(const XRealArray& a,const XRealArray& b)
{
  XM2_ASSERT(a.size()==b.size());
  resize(a.size());
  XRealArray::arrayAdd(buffer(),a.buffer(),b.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::mul(const XRealArray& a)
{
  XM2_ASSERT(a.size()==size());
  XRealArray::arrayMul(buffer(),buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::div(const XRealArray& a)
{
  XM2_ASSERT(a.size()==size());
  XRealArray::arrayDiv(buffer(),buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::sub(const XRealArray& a)
{
  XM2_ASSERT(a.size()==size());
  XRealArray::arraySub(buffer(),buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::add(const XRealArray& a)
{
  XM2_ASSERT(a.size()==size());
  XRealArray::arrayAdd(buffer(),buffer(),a.buffer(),(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::mul(const XRealArray& a, const XReal b)
{
  resize(a.size());
  XRealArray::arrayMul(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::div(const XRealArray& a, const XReal b)
{
  resize(a.size());
  XRealArray::arrayDiv(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::sub(const XRealArray& a, const XReal b)
{
  resize(a.size());
  XRealArray::arraySub(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::add(const XRealArray& a, const XReal b)
{
  resize(a.size());
  XRealArray::arrayAdd(buffer(),a.buffer(),b,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::mul(const XReal& a)
{
  XRealArray::arrayMul(buffer(),buffer(),a,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::div(const XReal& a)
{
  XRealArray::arrayDiv(buffer(),buffer(),a,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::sub(const XReal& a)
{
  XRealArray::arraySub(buffer(),buffer(),a,(int)size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XRealArray::add(const XReal& a)
{
  XRealArray::arrayAdd(buffer(),buffer(),a,(int)size());
}
}
#endif
