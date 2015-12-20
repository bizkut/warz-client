#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR2SOA__INL__
#define XM2_VECTOR2SOA__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2SOA::XVector2SOA() 
{ 
  #if XM2_INIT_CLASSES
  zero();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2 XVector2SOA::get0() const
{
  XVector2 v;
  get0(v);
  return v;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2 XVector2SOA::get1() const
{
  XVector2 v;
  get1(v);
  return v;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2 XVector2SOA::get2() const
{
  XVector2 v;
  get2(v);
  return v;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2 XVector2SOA::get3() const
{
  XVector2 v;
  get3(v);
  return v;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2SOA::notEqual(const XVector2SOA& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::lengthSquared(XVector4& dp) const
{
  dot(dp,*this);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XReal b) 
{ 
  add(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XReal b) 
{ 
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XReal b) 
{ 
  div(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XReal b) 
{ 
  mul(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2& b) 
{ 
  add(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2& b) 
{ 
  sub(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2& b) 
{ 
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2& b) 
{ 
  mul(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::normalise() 
{ 
  normalise(*this); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::negate() 
{ 
  negate(*this); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& b) 
{ 
  add(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& b) 
{ 
  sub(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& b)
{
  mul(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& b)
{ 
  div(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2SOA XVector2SOA::operator-() const
{
  XVector2SOA v;
  v.negate(*this);
  return v;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2SOA::operator == (const XVector2SOA& c) const
{
  return equal(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector2SOA::operator != (const XVector2SOA& c) const
{
  return notEqual(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator = (const XVector2SOA& c)
{
  memcpy(this,&c,sizeof(XVector2SOA));
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator *= (const XVector2SOA& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator /= (const XVector2SOA& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator += (const XVector2SOA& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator -= (const XVector2SOA& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator += (const XVector2& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator -= (const XVector2& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator /= (const XVector2& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator *= (const XVector2& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator += (const XReal c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2SOA& XVector2SOA::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
#pragma warning(pop)
}
#endif
