#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_VECTOR2__INL__
#define XM2_VANILLA_VECTOR2__INL__
#ifdef XM2_USE_SSE
# if (!XM2_USE_FLOAT)
#  error FPU routines included when set to build with SSE
# endif
#endif
namespace XM2
{
  //----------------------------------------------------------------------------------------------------------------------  
  XM2_INLINE void XVector2::zero()
  {
    x=y=0;
  }
  //----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::set(const XReal _x,const XReal _y)
{
  x=_x;
  y=_y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::set(const XReal* ptr)
{
  XM2_ASSERT(ptr);
  x=ptr[0];
  y=ptr[1];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::add(const XVector2& a,const XVector2& b)
{
  x = a.x + b.x;
  y = a.y + b.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::sub(const XVector2& a,const XVector2& b)
{
  x = a.x - b.x;
  y = a.y - b.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::mul(const XVector2& a,const XVector2& b)
{
  x = a.x * b.x;
  y = a.y * b.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::div(const XVector2& a,const XVector2& b)
{
  x = a.x / b.x;
  y = a.y / b.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::add(const XVector2& a,const XReal b)
{
  x = a.x + b;
  y = a.y + b;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::sub(const XVector2& a,const XReal b)
{
  x = a.x - b;
  y = a.y - b;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::div(const XVector2& a,const XReal b)
{
  x = a.x / b;
  y = a.y / b;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::mul(const XVector2& a,const XReal b)
{
  x = a.x * b;
  y = a.y * b;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XReal XVector2::dot(const XVector2& b) const
{
  return x*b.x+y*b.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::lerp(const XVector2& a,const XVector2& b,const XReal t)
{
  x = a.x + t*(b.x-a.x);
  y = a.y + t*(b.y-a.y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::bezier(const XVector2& a,const XVector2& b,const XVector2& c,const XVector2& d,const XReal t)
{
  const XReal B0 = (1.0f-t)*(1.0f-t)*(1.0f-t);
  const XReal B1 = 3.0f*(1.0f-t)*(1.0f-t)*t;
  const XReal B2 = 3.0f*(1.0f-t)*t*t;
  const XReal B3 = t*t*t;
  x = a.x*B0 + b.x*B1 + c.x*B2 + d.x*B3;
  y = a.y*B0 + b.y*B1 + c.y*B2 + d.y*B3;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2::negate(const XVector2& v)
{
  x = -v.x;
  y = -v.y;
}
}
#endif
