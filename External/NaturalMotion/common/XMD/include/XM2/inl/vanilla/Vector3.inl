#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_VECTOR3__INL__
#define XM2_VANILLA_VECTOR3__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3(const XVector3& rhs) 
{ 
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = 0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3::XVector3(const XVector4& rhs) 
{ 
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = 0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::set(const XReal _x,const XReal _y,const XReal _z)
{
  x=_x;
  y=_y;
  z=_z;
  w=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::set(const XReal* ptr)
{
  XM2_ASSERT(ptr);
  x=ptr[0];
  y=ptr[1];
  z=ptr[2];
  w=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::zero()
{
  x=y=z=w=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::div(const XVector3& a,const XReal b)
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
XM2_INLINE void XVector3::mul(const XVector3& a,const XReal b)
{
  x = a.x*b;
  y = a.y*b;
  z = a.z*b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::add(const XVector3& a,const XReal b)
{
  x = a.x+b;
  y = a.y+b;
  z = a.z+b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sub(const XVector3& a,const XReal b)
{
  x = a.x-b;
  y = a.y-b;
  z = a.z-b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::div(const XVector3& a,const XVector3& b)
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
XM2_INLINE void XVector3::mul(const XVector3& a,const XVector3& b)
{
  x = a.x*b.x;
  y = a.y*b.y;
  z = a.z*b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::add(const XVector3& a,const XVector3& b)
{
  x = a.x+b.x;
  y = a.y+b.y;
  z = a.z+b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sub(const XVector3& a,const XVector3& b)
{
  x = a.x-b.x;
  y = a.y-b.y;
  z = a.z-b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector3::dot(const XVector3& b) const
{
  return x*b.x + y*b.y + z*b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::cross(const XVector3& a,const XVector3& b)
{
  x = a.y * b.z  -  a.z * b.y;
  y = a.z * b.x  -  a.x * b.z;
  z = a.x * b.y  -  a.y * b.x;
  w = 0.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::lerp(const XVector3& a,const XVector3& b,const XReal t)
{
  x = a.x + t*(b.x - a.x);
  y = a.y + t*(b.y - a.y);
  z = a.z + t*(b.z - a.z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::bezier(const XVector3& _a,const XVector3& _b,const XVector3& _c,const XVector3& _d,const XReal t)
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
XM2_INLINE void XVector3::negate(const XVector3& a)
{
  x = -a.x;
  y = -a.y;
  z = -a.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::negate()
{
  x = -x;
  y = -y;
  z = -z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3::sum(const XVector3& a,const XReal weight)
{
  x += (a.x * weight);
  y += (a.y * weight);
  z += (a.z * weight);
}
}
#endif
