#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR3__INL__
#define XM2_VECTOR3__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3() 

{ set(0,0,0); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3(const XVector3Packed& rhs) 
{ 
  set(rhs.x,rhs.y,rhs.z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3(const XReal _x,const XReal _y,const XReal _z) 
{ 
  set(_x,_y,_z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3::equal(const XVector3& b,const XReal eps) const
{
  return float_equal(x,b.x,eps) && 
         float_equal(y,b.y,eps) && 
         float_equal(z,b.z,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3::notEqual(const XVector3& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::add(const XVector3& b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sub(const XVector3& b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::mul(const XVector3& b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::div(const XVector3& b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::add(const XReal b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sub(const XReal b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::div(const XReal b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::mul(const XReal b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3::lengthSquared() const
{
  return dot(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3::length() const
{
  return XMsqrt(lengthSquared());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::normalise()
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::normalise(const XVector3& v)
{
  const XReal l2 = v.lengthSquared();
  if (l2 > XM2_FLOAT_ZERO)
  {
    div(v,XMsqrt(l2));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator-() const
{
  return XVector3(-x,-y,-z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3::operator == (const XVector3& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3::operator != (const XVector3& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator = (const XVector3& c)
{
  memcpy(this,&c,sizeof(XVector3));
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator *= (const XVector3& c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator /= (const XVector3& c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator += (const XVector3& c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator -= (const XVector3& c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator += (const XReal c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3& XVector3::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator * (const XVector3& c) const
{
  XVector3 temp;
  temp.mul(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator / (const XVector3& c) const
{
  XVector3 temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator + (const XVector3& c) const
{
  XVector3 temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator - (const XVector3& c) const
{
  XVector3 temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator + (const XReal c) const
{
  XVector3 temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator - (const XReal c) const
{
  XVector3 temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator / (const XReal c) const
{
  XVector3 temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3 XVector3::operator * (const XReal c) const
{
  XVector3 temp;
  temp.mul(*this,c);
  return temp;
}
#pragma warning(pop)   
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XM2::XVector3 operator * (const XM2::XReal f,const XM2::XVector3& p)
{
XM2::XVector3 temp;
temp.mul(p,f);
return temp;
}
#endif
