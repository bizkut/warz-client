#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR2__INL__
#define XM2_VECTOR2__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2::equal(const XVector2& b,const XReal eps) const
{
  return float_equal(x,b.x,eps) &&
         float_equal(y,b.y,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2::notEqual(const XVector2& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XVector2::lengthSquared() const
{
  return dot(*this);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XVector2::length() const
{
  return XMsqrt(lengthSquared());
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::normalise(const XVector2& q)
{
  XReal l = q.lengthSquared();
  if (l>XM2_FLOAT_ZERO)
  {
    div(q,XMsqrt(l));
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::add(const XReal b) 
{ 
  add(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::sub(const XReal b) 
{
  sub(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::div(const XReal b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::mul(const XReal b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::normalise() 
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::negate() 
{
  negate(*this);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::add(const XVector2& b)
{
  add(*this,b); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::sub(const XVector2& b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::mul(const XVector2& b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::div(const XVector2& b) 
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2 XVector2::operator-() const
{
  XVector2 v;
  v.negate(*this);
  return v;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2::operator == (const XVector2& c) const
{
  return equal(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector2::operator != (const XVector2& c) const
{
  return notEqual(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator = (const XVector2& c)
{
  x = c.x;
  y = c.y;
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator *= (const XVector2& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator /= (const XVector2& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator += (const XVector2& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator -= (const XVector2& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator += (const XReal c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector2& XVector2::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator + (const XVector2& c) const
{
  XVector2 v;
  v.add(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator - (const XVector2& c) const
{
  XVector2 v;
  v.sub(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator * (const XVector2& c) const
{
  XVector2 v;
  v.mul(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator / (const XVector2& c) const
{
  XVector2 v;
  v.div(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator + (const XReal c) const
{
  XVector2 v;
  v.add(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator - (const XReal c) const
{
  XVector2 v;
  v.sub(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator / (const XReal c) const
{
  XVector2 v;
  v.div(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 XVector2::operator * (const XReal c) const
{
  XVector2 v;
  v.mul(*this,c);
  return v;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector2 operator * (const XReal f,const XVector2& p)
{
  XVector2 v;
  v.mul(p,f);
  return v;
}
#pragma warning(pop)
}
#endif
