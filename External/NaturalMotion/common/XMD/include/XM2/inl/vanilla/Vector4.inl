#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_VECTOR4__INL__
#define XM2_VANILLA_VECTOR4__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::zero()
{
  x=y=z=w=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::set(const XReal _x,const XReal _y,const XReal _z,const XReal _w)
{
  x = _x;
  y = _y;
  z = _z;
  w = _w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::set(const XReal* ptr)
{
  XM2_ASSERT(ptr);
  x = ptr[0];
  y = ptr[1];
  z = ptr[2];
  w = ptr[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XVector4& b)
{
  x = a.x + b.x;
  y = a.y + b.y;
  z = a.z + b.z;
  w = a.w + b.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XVector4& b)
{
  x = a.x - b.x;
  y = a.y - b.y;
  z = a.z - b.z;
  w = a.w - b.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XVector3& b)
{
  x = a.x - b.x;
  y = a.y - b.y;
  z = a.z - b.z;
  w = a.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XVector3& b)
{
  x = a.x + b.x;
  y = a.y + b.y;
  z = a.z + b.z;
  w = a.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XVector4& b)
{
  x = a.x * b.x;
  y = a.y * b.y;
  z = a.z * b.z;
  w = a.w * b.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XVector4& b)
{
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b.x)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.z)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.w)>XM2_FLOAT_ZERO);
    x = a.x / b.x;
    y = a.y / b.y;
    z = a.z / b.z;
    w = a.w / b.w;
  #else
  {
    const XReal fx = (XMabs(b.x)>XM2_FLOAT_ZERO) ? 1.0f/b.x : 0.0f;
    const XReal fy = (XMabs(b.y)>XM2_FLOAT_ZERO) ? 1.0f/b.y : 0.0f;
    const XReal fz = (XMabs(b.z)>XM2_FLOAT_ZERO) ? 1.0f/b.z : 0.0f;
    const XReal fw = (XMabs(b.w)>XM2_FLOAT_ZERO) ? 1.0f/b.w : 0.0f;
    x = a.x * fx;
    y = a.y * fy;
    z = a.z * fz;
    w = a.w * fw;
  }
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XVector3& b)
{
  x = a.x * b.x;
  y = a.y * b.y;
  z = a.z * b.z;
  w = a.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XVector3& b)
{    
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b.x)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.z)>XM2_FLOAT_ZERO);
    x = a.x / b.x;
    y = a.y / b.y;
    z = a.z / b.z;
  #else
  {
    const XReal fx = (XMabs(b.x)>XM2_FLOAT_ZERO) ? 1.0f/b.x : 0.0f;
    const XReal fy = (XMabs(b.y)>XM2_FLOAT_ZERO) ? 1.0f/b.y : 0.0f;
    const XReal fz = (XMabs(b.z)>XM2_FLOAT_ZERO) ? 1.0f/b.z : 0.0f;
    x = a.x * fx;
    y = a.y * fy;
    z = a.z * fz;
  }
  #endif
  w = a.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XVector4::dot(const XVector4& b) const
{
  return x * b.x +
         y * b.y +
         z * b.z + 
         w * b.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::cross(const XVector4& a,const XVector4& b)
{
  x = a.y * b.z  -  a.z * b.y;
  y = a.z * b.x  -  a.x * b.z;
  z = a.x * b.y  -  a.y * b.x;
  w = 0.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::add(const XVector4& a,const XReal b)
{
  x = a.x + b;
  y = a.y + b;
  z = a.z + b;
  w = a.w + b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::sub(const XVector4& a,const XReal b)
{
  x = a.x - b;
  y = a.y - b;
  z = a.z - b;
  w = a.w - b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::div(const XVector4& a,const XReal b)
{
  #if XM2_DIVIDE_BY_ZERO_MODE
  XM2_ASSERT(XMabs(b)>XM2_FLOAT_ZERO);
  {
    XReal f = 1.0f/b;
  #else
  {
    const XReal f = (XMabs(b)>XM2_FLOAT_ZERO) ? 1.0f/b : 0.0f;
  #endif
    x = a.x * f;
    y = a.y * f;
    z = a.z * f;
    w = a.w * f;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::mul(const XVector4& a,const XReal b)
{
  x = a.x * b;
  y = a.y * b;
  z = a.z * b;
  w = a.w * b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::lerp(const XVector4& a,const XVector4& b,const XReal t)
{
  x = a.x + t * (b.x - a.x);
  y = a.y + t * (b.y - a.y);
  z = a.z + t * (b.z - a.z);
  w = a.w + t * (b.w - a.w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::bezier(const XVector4& a,const XVector4& b,
                                 const XVector4& c,const XVector4& d,const XReal t)
{
  XReal B0 = (1.0f-t) * (1.0f-t) * (1.0f-t);
  XReal B1 = 3.0f * (1.0f-t) * (1.0f-t) * t;
  XReal B2 = 3.0f * (1.0f-t) * t * t;
  XReal B3 = t * t * t;

  x = B0 * a.x  +  B1 * b.x  +  B2 * c.x +  B3 * d.x;
  y = B0 * a.y  +  B1 * b.y  +  B2 * c.y +  B3 * d.y;
  z = B0 * a.z  +  B1 * b.z  +  B2 * c.z +  B3 * d.z;
  w = B0 * a.w  +  B1 * b.w  +  B2 * c.w +  B3 * d.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4::negate(const XVector4& a)
{
  x = -a.x;
  y = -a.y;
  z = -a.z;
  w = -a.w;
}
}
#endif
