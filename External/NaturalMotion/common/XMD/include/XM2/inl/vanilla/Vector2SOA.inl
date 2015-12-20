#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_VECTOR2SOA__INL__
#define XM2_VANILLA_VECTOR2SOA__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2SOA::XVector2SOA(const XVector2SOA& rhs) 
{ 
  memcpy(this,&rhs,sizeof(XVector2SOA)); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set0(const XReal x_,const XReal y_)
{
  x[0] = x_;
  y[0] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set1(const XReal x_,const XReal y_)
{
  x[1] = x_;
  y[1] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set2(const XReal x_,const XReal y_)
{
  x[2] = x_;
  y[2] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set3(const XReal x_,const XReal y_)
{
  x[3] = x_;
  y[3] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set0(const XVector2& v)
{
  x[0] = v.x;
  y[0] = v.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set1(const XVector2& v)
{
  x[1] = v.x;
  y[1] = v.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set2(const XVector2& v)
{
  x[2] = v.x;
  y[2] = v.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set3(const XVector2& v)
{
  x[3] = v.x;
  y[3] = v.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get0(XVector2& vec) const
{
  vec.x = x[0];
  vec.y = y[0];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get1(XVector2& vec) const
{
  vec.x = x[1];
  vec.y = y[1];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get2(XVector2& vec) const
{
  vec.x = x[2];
  vec.y = y[2];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get3(XVector2& vec) const
{
  vec.x = x[3];
  vec.y = y[3];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2SOA::equal(const XVector2SOA& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  for(int i=0;i!=4;++i)
  {
    if(!float_equal(x[i],b.x[i],eps) ||
      !float_equal(y[i],b.y[i],eps))
      return false;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::streamIn(const XVector2* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::streamOut(XVector2* input) const
{
  input[0].x = x[0]; input[1].x = x[1]; input[2].x = x[2]; input[3].x = x[3];
  input[0].y = y[0]; input[1].y = y[1]; input[2].y = y[2]; input[3].y = y[3];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::length(XVector4& dp) const
{
  lengthSquared(dp);
  dp.x = XMsqrt(dp.x);
  dp.y = XMsqrt(dp.y);
  dp.z = XMsqrt(dp.z);
  dp.w = XMsqrt(dp.w);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::normalise(const XVector2SOA& q)
{
  XVector4 l;
  q.length(l);
  //if (l>XM2_FLOAT_ZERO)
  {
    for (int i=0;i!=4;++i)
    {
      if(float_equal(0,l.data[i]))
      {
        x[i] =0;
        y[i] =0;
      }
      else
      {
        x[i] = q.x[i] / l.data[i];
        y[i] = q.y[i] / l.data[i];
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::zero()
{
  memset(this,0,sizeof(XVector2SOA));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XVector2SOA& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x[i];
    y[i] = a.y[i] + b.y[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XVector2SOA& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x[i];
    y[i] = a.y[i] - b.y[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XVector2SOA& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x[i];
    y[i] = a.y[i] * b.y[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XVector2SOA& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] / b.x[i];
    y[i] = a.y[i] / b.y[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XVector2& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x;
    y[i] = a.y[i] + b.y;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XVector2& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x;
    y[i] = a.y[i] - b.y;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XVector2& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] / b.x;
    y[i] = a.y[i] / b.y;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XVector2& b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x;
    y[i] = a.y[i] * b.y;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XReal b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b;
    y[i] = a.y[i] + b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XReal b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b;
    y[i] = a.y[i] - b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XReal b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] / b;
    y[i] = a.y[i] / b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XReal b)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b;
    y[i] = a.y[i] * b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::dot(XVector4& dp,const XVector2SOA& b) const
{
  dp.x = x[0]*b.x[0] + y[0]*b.y[0];
  dp.y = x[1]*b.x[1] + y[1]*b.y[1];
  dp.z = x[2]*b.x[2] + y[2]*b.y[2];
  dp.w = x[3]*b.x[3] + y[3]*b.y[3];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::lerp(const XVector2SOA& a,const XVector2SOA& b,const XReal t)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i] + t * (b.x[i] - a.x[i]);
    y[i] = a.y[i] + t * (b.y[i] - a.y[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::bezier(const XVector2SOA& a,const XVector2SOA& b,const XVector2SOA& c,const XVector2SOA& d,const XReal t)
{
  const XReal B0 = (1-t)*(1-t)*(1-t);
  const XReal B1 = 3.0f*(1-t)*(1-t)*t;
  const XReal B2 = 3.0f*(1-t)*t*t;
  const XReal B3 = t*t*t;

  for (unsigned i=0;i!=4;++i)
  {
    x[i] = a.x[i]*B0 + b.x[i]*B1 + c.x[i]*B2 + d.x[i]*B3;
    y[i] = a.y[i]*B0 + b.y[i]*B1 + c.y[i]*B2 + d.y[i]*B3;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::negate(const XVector2SOA& v)
{
  for (unsigned i=0;i!=4;++i)
  {
    x[i] = -v.x[i];
    y[i] = -v.y[i];
  }
}
}
#endif
