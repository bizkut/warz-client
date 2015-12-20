#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR4__INL__
#define XM2_VECTOR4__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4::XVector4()
{
  #if XM2_INIT_CLASSES
    zero();
  #endif  
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4::XVector4(const XVector4& v)
{
  set(v.x,v.y,v.z,v.w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4::XVector4(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set(x_,y_,z_,w_);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector4::equal(const XVector4& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  return float_equal(x,b.x,eps) && 
         float_equal(y,b.y,eps) && 
         float_equal(z,b.z,eps) && 
         float_equal(w,b.w,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector4::notEqual(const XVector4& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector3& b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector3& b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector3& b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector3& b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XReal b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XReal b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XReal b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XReal b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector4::lengthSquared() const
{
  return dot(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector4::length() const
{
  return XMsqrt(lengthSquared());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::normalise()
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::normalise(const XVector4& v)
{
  XReal l = v.lengthSquared();
  if (l>XM2_FLOAT_ZERO)
  {
    div(v,XMsqrt(l));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::negate()
{
  negate(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector4::operator == (const XVector4& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector4::operator != (const XVector4& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator = (const XVector4& c)
{
  set(c.x,c.y,c.z,c.w);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator = (const XVector3& c)
{
  set(c.x,c.y,c.z,0);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator += (const XVector4& c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator -= (const XVector4& c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator *= (const XVector4& c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator /= (const XVector4& c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator += (const XVector3& c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator -= (const XVector3& c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator *= (const XVector3& c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator /= (const XVector3& c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator += (const XReal c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector4& XVector4::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator + (const XVector4& c) const
{
  XVector4 temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator - (const XVector4& c) const
{
  XVector4 temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator * (const XVector4& c) const
{
  XVector4 temp;
  temp.mul(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator / (const XVector4& c) const
{
  XVector4 temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator + (const XVector3& c) const
{
  XVector4 temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator - (const XVector3& c) const
{
  XVector4 temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator * (const XVector3& c) const
{
  XVector4 temp;
  temp.mul(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator / (const XVector3& c) const
{
  XVector4 temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator + (const XReal c) const
{
  XVector4 temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator - (const XReal c) const
{
  XVector4 temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator / (const XReal c) const
{
  XVector4 temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 XVector4::operator * (const XReal c) const
{
  XVector4 temp;
  temp.mul(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4 operator * (const XReal f,const XVector4& p)
{
  XVector4 temp;
  temp.mul(p,f);
  return temp;
} 
#pragma warning(pop)   
}
#endif
