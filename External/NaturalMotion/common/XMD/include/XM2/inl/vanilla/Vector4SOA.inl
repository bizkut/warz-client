#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_VECTOR4SOA__INL__
#define XM2_VANILLA_VECTOR4SOA__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA()
{
  #if XM2_INIT_CLASSES
  memset(this,0,sizeof(XVector4SOA));
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3& a)
{
  x[0]=x[1]=x[2]=x[3]=a.x;
  y[0]=y[1]=y[2]=y[3]=a.y;
  z[0]=z[1]=z[2]=z[3]=a.z;
  w[0]=w[1]=w[2]=w[3]=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector4& a)
{
  x[0]=x[1]=x[2]=x[3]=a.x;
  y[0]=y[1]=y[2]=y[3]=a.y;
  z[0]=z[1]=z[2]=z[3]=a.z;
  w[0]=w[1]=w[2]=w[3]=a.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3Packed& a)
{
  x[0]=x[1]=x[2]=x[3]=a.x;
  y[0]=y[1]=y[2]=y[3]=a.y;
  z[0]=z[1]=z[2]=z[3]=a.z;
  w[0]=w[1]=w[2]=w[3]=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3SOA& a)
{
  for (int i=0;i<4;++i)
  {
    x[i] = a.x[i];
    y[i] = a.y[i];
    z[i] = a.z[i];
    w[i] = 0.0f;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector4SOA& a)
{
  for (int i=0;i<4;++i)
  {
    x[i] = a.x[i];
    y[i] = a.y[i];
    z[i] = a.z[i];
    w[i] = a.w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::lengthSquared(XVector4& _length_sqr) const
{
  dot(_length_sqr,*this);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::zero()
{
  memset(this,0,sizeof(XVector4SOA));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set0(const XVector4& vec)
{
  x[0] = vec.x;
  y[0] = vec.y;
  z[0] = vec.z;
  w[0] = vec.w;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set1(const XVector4& vec)
{
  x[1] = vec.x;
  y[1] = vec.y;
  z[1] = vec.z;
  w[1] = vec.w;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set2(const XVector4& vec)
{
  x[2] = vec.x;
  y[2] = vec.y;
  z[2] = vec.z;
  w[2] = vec.w;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set3(const XVector4& vec)
{
  x[3] = vec.x;
  y[3] = vec.y;
  z[3] = vec.z;
  w[3] = vec.w;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get0(XVector4& vec) const 
{
  vec.x = x[0];
  vec.y = y[0];
  vec.z = z[0];
  vec.w = w[0];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get1(XVector4& vec) const
{
  vec.x = x[1];
  vec.y = y[1];
  vec.z = z[1];
  vec.w = w[1];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get2(XVector4& vec) const
{
  vec.x = x[2];
  vec.y = y[2];
  vec.z = z[2];
  vec.w = w[2];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get3(XVector4& vec) const
{
  vec.x = x[3];
  vec.y = y[3];
  vec.z = z[3];
  vec.w = w[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
  w[0]=w[1]=w[2]=w[3]=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector4* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
  w[0] = input[0].w; w[1] = input[1].w; w[2] = input[2].w; w[3] = input[3].w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3SOA& input)
{
  x[0] = input.x[0]; x[1] = input.x[1]; x[2] = input.x[2]; x[3] = input.x[3];
  y[0] = input.y[0]; y[1] = input.y[1]; y[2] = input.y[2]; y[3] = input.y[3];
  z[0] = input.z[0]; z[1] = input.z[1]; z[2] = input.z[2]; z[3] = input.z[3];
  w[0]=w[1]=w[2]=w[3]=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3Packed* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
  w[0]=w[1]=w[2]=w[3]=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3* output) const
{
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector4* output) const
{
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
  output[0].w = w[0]; output[1].w = w[1]; output[2].w = w[2]; output[3].w = w[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3SOA& output) const
{
  output.x[0] = x[0]; output.x[1] = x[1]; output.x[2] = x[2]; output.x[3] = x[3];
  output.y[0] = y[0]; output.y[1] = y[1]; output.y[2] = y[2]; output.y[3] = y[3];
  output.z[0] = z[0]; output.z[1] = z[1]; output.z[2] = z[2]; output.z[3] = z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3Packed* output) const
{
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XVector4SOA& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x[i];
    y[i] = a.y[i] + b.y[i];
    z[i] = a.z[i] + b.z[i];
    w[i] = a.w[i] + b.w[i];
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XVector4SOA& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x[i];
    y[i] = a.y[i] - b.y[i];
    z[i] = a.z[i] - b.z[i];
    w[i] = a.w[i] - b.w[i];
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XVector4SOA& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x[i];
    y[i] = a.y[i] * b.y[i];
    z[i] = a.z[i] * b.z[i];
    w[i] = a.w[i] * b.w[i];
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XVector4SOA& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] / b.x[i];
    y[i] = a.y[i] / b.y[i];
    z[i] = a.z[i] / b.z[i];
    w[i] = a.w[i] / b.w[i];
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XVector4& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x;
    y[i] = a.y[i] + b.y;
    z[i] = a.z[i] + b.z;
    w[i] = a.w[i] + b.w;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XVector4& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x;
    y[i] = a.y[i] - b.y;
    z[i] = a.z[i] - b.z;
    w[i] = a.w[i] - b.w;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XVector4& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x;
    y[i] = a.y[i] * b.y;
    z[i] = a.z[i] * b.z;
    w[i] = a.w[i] * b.w;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XVector4& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] / b.x;
    y[i] = a.y[i] / b.y;
    z[i] = a.z[i] / b.z;
    w[i] = a.w[i] / b.w;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XReal b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b;
    y[i] = a.y[i] + b;
    z[i] = a.z[i] + b;
    w[i] = a.w[i] + b;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XReal b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b;
    y[i] = a.y[i] - b;
    z[i] = a.z[i] - b;
    w[i] = a.w[i] - b;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XReal b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] / b;
    y[i] = a.y[i] / b;
    z[i] = a.z[i] / b;
    w[i] = a.w[i] / b;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XReal b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b;
    y[i] = a.y[i] * b;
    z[i] = a.z[i] * b;
    w[i] = a.w[i] * b;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::dot(XVector4& _dot,const XVector4SOA& b) const
{
  _dot.x = x[0]*b.x[0] + y[0]*b.y[0] + z[0]*b.z[0] + w[0]*b.w[0];
  _dot.y = x[1]*b.x[1] + y[1]*b.y[1] + z[1]*b.z[1] + w[1]*b.w[1];
  _dot.z = x[2]*b.x[2] + y[2]*b.y[2] + z[2]*b.z[2] + w[2]*b.w[2];
  _dot.w = x[3]*b.x[3] + y[3]*b.y[3] + z[3]*b.z[3] + w[3]*b.w[3];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::cross(const XVector4SOA& a,const XVector4SOA& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.y[i] * b.z[i]  -  a.z[i] * b.y[i];
    y[i] = a.z[i] * b.x[i]  -  a.x[i] * b.z[i];
    z[i] = a.x[i] * b.y[i]  -  a.y[i] * b.x[i];
    w[i] = 0.0f;
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::length(XVector4& out_length) const
{
  dot(out_length,*this);
  out_length.x = XMsqrt(out_length.x);
  out_length.y = XMsqrt(out_length.y);
  out_length.z = XMsqrt(out_length.z);
  out_length.w = XMsqrt(out_length.w);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::normalise(const XVector4SOA& a)
{
  XVector4 l;
  a.length(l);
  for(int i=0;i<4;++i)
  {
    if(XMabs(l.data[i])>XM2_FLOAT_ZERO) 
    { 
      l.data[i] = 1.0f/l.data[i]; 
      x[i] = l.data[i] * a.x[i];
      y[i] = l.data[i] * a.y[i];
      z[i] = l.data[i] * a.z[i];
      w[i] = l.data[i] * a.w[i];
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::normalise()
{
  normalise(*this);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::lerp(const XVector4SOA& a,const XVector4SOA& b,const XReal t)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + t*(b.x[i]-a.x[i]);
    y[i] = a.y[i] + t*(b.y[i]-a.y[i]);
    z[i] = a.z[i] + t*(b.z[i]-a.z[i]);
    w[i] = a.w[i] + t*(b.w[i]-a.w[i]);
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::bezier(const XVector4SOA& a,const XVector4SOA& b,const XVector4SOA& c,const XVector4SOA& d,const XReal t)
{
  const XReal B0 = t*t*t;
  const XReal B1 = 3.0f*(1.0f-t)*t*t;
  const XReal B2 = 3.0f*(1.0f-t)*(1.0f-t)*t;
  const XReal B3 = (1.0f-t)*(1.0f-t)*(1.0f-t);
  for (int i=0;i!=4;++i)
  {
    x[i] = B0*a.x[i] + B1*b.x[i] + B2*c.x[i] + B3*d.x[i];
    y[i] = B0*a.y[i] + B1*b.y[i] + B2*c.y[i] + B3*d.y[i];
    z[i] = B0*a.z[i] + B1*b.z[i] + B2*c.z[i] + B3*d.z[i];
    w[i] = B0*a.w[i] + B1*b.w[i] + B2*c.w[i] + B3*d.w[i];
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::negate(const XVector4SOA& a)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = -a.x[i];
    y[i] = -a.y[i];
    z[i] = -a.z[i];
    w[i] = -a.w[i];
  }
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::negate()
{
  negate(*this);
}
}
#endif
