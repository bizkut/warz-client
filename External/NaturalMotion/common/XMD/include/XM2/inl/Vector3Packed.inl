#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR3_PACKED__INL__
#define XM2_VECTOR3_PACKED__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed::XVector3Packed() 
{ 
  set(0,0,0); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed::XVector3Packed(const XVector3Packed& rhs) 
{ 
  x=rhs.x;
  y=rhs.y;
  z=rhs.z; 
}  
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed::XVector3Packed(const XVector3& rhs) 
{ 
  x=rhs.x;
  y=rhs.y;
  z=rhs.z; 
}  
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed::XVector3Packed(const XReal _x,const XReal _y,const XReal _z) 
{ 
  set(_x,_y,_z); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3Packed::equal(const XVector3Packed& b,const XReal eps) const
{
  return float_equal(x,b.x,eps) && 
         float_equal(y,b.y,eps) && 
         float_equal(z,b.z,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3Packed::notEqual(const XVector3Packed& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::add(const XVector3Packed& b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::sub(const XVector3Packed& b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::mul(const XVector3Packed& b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::div(const XVector3Packed& b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::add(const XReal b)
{
  add(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::sub(const XReal b)
{
  sub(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::div(const XReal b)
{
  div(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::mul(const XReal b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3Packed::lengthSquared() const
{
  return dot(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3Packed::length() const
{
  return XMsqrt(lengthSquared());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::normalise()
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::normalise(const XVector3Packed& v)
{
  const XReal l2 = v.lengthSquared();
  if (l2 > XM2_FLOAT_ZERO)
  {
    div(v,XMsqrt(l2));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator-() const
{
  return XVector3Packed(-x,-y,-z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3Packed::operator == (const XVector3Packed& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XVector3Packed::operator != (const XVector3Packed& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator = (const XVector3Packed& c)
{
  x=c.x;
  y=c.y;
  z=c.z;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator = (const XVector3& c)
{
  x=c.x;
  y=c.y;
  z=c.z;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator = (const XVector4& c)
{
  x=c.x;
  y=c.y;
  z=c.z;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator *= (const XVector3Packed& c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator /= (const XVector3Packed& c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator += (const XVector3Packed& c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator -= (const XVector3Packed& c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator += (const XReal c)
{
  add(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XVector3Packed& XVector3Packed::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator * (const XVector3Packed& c) const
{
  XVector3Packed temp;
  temp.mul(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator / (const XVector3Packed& c) const
{
  XVector3Packed temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator + (const XVector3Packed& c) const
{
  XVector3Packed temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator - (const XVector3Packed& c) const
{
  XVector3Packed temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator + (const XReal c) const
{
  XVector3Packed temp;
  temp.add(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator - (const XReal c) const
{
  XVector3Packed temp;
  temp.sub(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator / (const XReal c) const
{
  XVector3Packed temp;
  temp.div(*this,c);
  return temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3Packed XVector3Packed::operator * (const XReal c) const
{
  XVector3Packed temp;
  temp.mul(*this,c);
  return temp;
}

//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::set(const XReal _x,const XReal _y,const XReal _z)
{
  x=_x;
  y=_y;
  z=_z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::set(const XReal* ptr)
{
  XM2_ASSERT(ptr);
  x=ptr[0];
  y=ptr[1];
  z=ptr[2];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::zero()
{
  x=y=z=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::div(const XVector3Packed& a,const XReal b)
{
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b)>XM2_FLOAT_ZERO);
    const XReal f = 1.0f/b;
    x = a.x * f;
    y = a.y * f;
    z = a.z * f;
  #else
    const XReal f = (XMabs(b)>XM2_FLOAT_ZERO) ? 1.0f/b : 1.0f;
    x = a.x * f;
    y = a.y * f;
    z = a.z * f;
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::mul(const XVector3Packed& a,const XReal b)
{
  x = a.x*b;
  y = a.y*b;
  z = a.z*b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::add(const XVector3Packed& a,const XReal b)
{
  x = a.x+b;
  y = a.y+b;
  z = a.z+b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::sub(const XVector3Packed& a,const XReal b)
{
  x = a.x-b;
  y = a.y-b;
  z = a.z-b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::div(const XVector3Packed& a,const XVector3Packed& b)
{
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b.x)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.z)>XM2_FLOAT_ZERO);
    x = a.x / b.x;
    y = a.y / b.y;
    z = a.z / b.z;
  #else
    x = a.x / (XMabs(b.x)>XM2_FLOAT_ZERO?b.x:1.0f);
    y = a.y / (XMabs(b.y)>XM2_FLOAT_ZERO?b.y:1.0f);
    z = a.z / (XMabs(b.z)>XM2_FLOAT_ZERO?b.z:1.0f);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::mul(const XVector3Packed& a,const XVector3Packed& b)
{
  x = a.x*b.x;
  y = a.y*b.y;
  z = a.z*b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::add(const XVector3Packed& a,const XVector3Packed& b)
{
  x = a.x+b.x;
  y = a.y+b.y;
  z = a.z+b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::sub(const XVector3Packed& a,const XVector3Packed& b)
{
  x = a.x-b.x;
  y = a.y-b.y;
  z = a.z-b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3Packed::dot(const XVector3Packed& b) const
{
  return x*b.x + y*b.y + z*b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::cross(const XVector3Packed& a,const XVector3Packed& b)
{
  x = a.y * b.z  -  a.z * b.y;
  y = a.z * b.x  -  a.x * b.z;
  z = a.x * b.y  -  a.y * b.x;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::lerp(const XVector3Packed& a,const XVector3Packed& b,const XReal t)
{
  x = a.x + t*(b.x - a.x);
  y = a.y + t*(b.y - a.y);
  z = a.z + t*(b.z - a.z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::bezier(const XVector3Packed& _a,const XVector3Packed& _b,
    const XVector3Packed& _c,const XVector3Packed& _d,const XReal t)
{
  const XReal B0 = (1.0f-t) * (1.0f-t) * (1.0f-t);
  const XReal B1 = 3.0f * (1.0f-t) * (1.0f-t) * t;
  const XReal B2 = 3.0f * (1.0f-t) * t * t;
  const XReal B3 = t * t * t;

  x = B0 * _a.x  +  B1 * _b.x  +  B2 * _c.x +  B3 * _d.x;
  y = B0 * _a.y  +  B1 * _b.y  +  B2 * _c.y +  B3 * _d.y;
  z = B0 * _a.z  +  B1 * _b.z  +  B2 * _c.z +  B3 * _d.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::negate(const XVector3Packed& a)
{
  x = -a.x;
  y = -a.y;
  z = -a.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3Packed::negate()
{
  x = -x;
  y = -y;
  z = -z;
}
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XM2::XVector3Packed operator * (const XM2::XReal f,const XM2::XVector3Packed& p)
{
  XM2::XVector3Packed temp;
  temp.mul(p,f);
  return temp;
}
#endif
