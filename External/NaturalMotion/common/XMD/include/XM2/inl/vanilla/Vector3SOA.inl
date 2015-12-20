#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_VECTOR3SOA__INL__
#define XM2_VANILLA_VECTOR3SOA__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA()
{
  #if XM2_INIT_CLASSES
  memset(this,0,sizeof(XVector3SOA));
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3& a)
{
  x[0]=x[1]=x[2]=x[3]=a.x;
  y[0]=y[1]=y[2]=y[3]=a.y;
  z[0]=z[1]=z[2]=z[3]=a.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector4& a)
{
  x[0]=x[1]=x[2]=x[3]=a.x;
  y[0]=y[1]=y[2]=y[3]=a.y;
  z[0]=z[1]=z[2]=z[3]=a.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3Packed& a)
{
  x[0]=x[1]=x[2]=x[3]=a.x;
  y[0]=y[1]=y[2]=y[3]=a.y;
  z[0]=z[1]=z[2]=z[3]=a.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3SOA& a)
{
  for (int i=0;i<4;++i)
  {
    x[i] = a.x[i];
    y[i] = a.y[i];
    z[i] = a.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector4SOA& a)
{
  for (int i=0;i<4;++i)
  {
    x[i] = a.x[i];
    y[i] = a.y[i];
    z[i] = a.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::zero() 
{
  memset(this,0,sizeof(XVector3SOA));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector3* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector4* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector4SOA& input)
{
  x[0] = input.x[0]; x[1] = input.x[1]; x[2] = input.x[2]; x[3] = input.x[3];
  y[0] = input.y[0]; y[1] = input.y[1]; y[2] = input.y[2]; y[3] = input.y[3];
  z[0] = input.z[0]; z[1] = input.z[1]; z[2] = input.z[2]; z[3] = input.z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector3Packed* input)
{
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector3* output) const
{
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector4* output) const
{
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector4SOA& output) const
{
  output.x[0] = x[0]; output.x[1] = x[1]; output.x[2] = x[2]; output.x[3] = x[3];
  output.y[0] = y[0]; output.y[1] = y[1]; output.y[2] = y[2]; output.y[3] = y[3];
  output.z[0] = z[0]; output.z[1] = z[1]; output.z[2] = z[2]; output.z[3] = z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector3Packed* output) const
{
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::lengthSquared(XVector4& _length_sqr) const
{
  dot(_length_sqr,*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::length(XVector4& _length) const
{
  lengthSquared(_length);
  _length.x = XMsqrt(_length.x);
  _length.y = XMsqrt(_length.y);
  _length.z = XMsqrt(_length.z);
  _length.w = XMsqrt(_length.w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::normalise(const XVector3SOA& a)
{
  XVector4 l;
  a.length(l);
  for(int i=0;i<4;++i)
  {
    if(float_equal(0,l.data[i]))
    {
      x[i] =0;
      y[i] =0;
      z[i] =0;
    }
    else
    {
      x[i] = a.x[i] / l.data[i];
      y[i] = a.y[i] / l.data[i];
      z[i] = a.z[i] / l.data[i];
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector3& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x;
    y[i] = a.y[i] + b.y;
    z[i] = a.z[i] + b.z;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector3SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x[i];
    y[i] = a.y[i] + b.y[i];
    z[i] = a.z[i] + b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector4SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b.x[i];
    y[i] = a.y[i] + b.y[i];
    z[i] = a.z[i] + b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XReal b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + b;
    y[i] = a.y[i] + b;
    z[i] = a.z[i] + b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector3& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x;
    y[i] = a.y[i] - b.y;
    z[i] = a.z[i] - b.z;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector3SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x[i];
    y[i] = a.y[i] - b.y[i];
    z[i] = a.z[i] - b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector4SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b.x[i];
    y[i] = a.y[i] - b.y[i];
    z[i] = a.z[i] - b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XReal b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] - b;
    y[i] = a.y[i] - b;
    z[i] = a.z[i] - b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector3& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x;
    y[i] = a.y[i] * b.y;
    z[i] = a.z[i] * b.z;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector3SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x[i];
    y[i] = a.y[i] * b.y[i];
    z[i] = a.z[i] * b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector4SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b.x[i];
    y[i] = a.y[i] * b.y[i];
    z[i] = a.z[i] * b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XReal b)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * b;
    y[i] = a.y[i] * b;
    z[i] = a.z[i] * b;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector3& b)
{
  for (int i=0;i!=4;++i)
  {
    #if XM2_DIVIDE_BY_ZERO_MODE
      XM2_ASSERT(XMabs(b.x)>XM2_FLOAT_ZERO);
      XM2_ASSERT(XMabs(b.y)>XM2_FLOAT_ZERO);
      XM2_ASSERT(XMabs(b.z)>XM2_FLOAT_ZERO);
      x[i] = a.x[i] / b.x;
      y[i] = a.y[i] / b.y;
      z[i] = a.z[i] / b.z;
    #else
      x[i] = a.x[i] * ((XMabs(b.x)>XM2_FLOAT_ZERO)?1.0f/b.x:0.0f);
      y[i] = a.y[i] * ((XMabs(b.y)>XM2_FLOAT_ZERO)?1.0f/b.y:0.0f);
      z[i] = a.z[i] * ((XMabs(b.z)>XM2_FLOAT_ZERO)?1.0f/b.z:0.0f);
    #endif
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector3SOA& b)
{
  for (int i=0;i!=4;++i)
  {    
    #if XM2_DIVIDE_BY_ZERO_MODE
      XM2_ASSERT(XMabs(b.x[i])>XM2_FLOAT_ZERO);
      XM2_ASSERT(XMabs(b.y[i])>XM2_FLOAT_ZERO);
      XM2_ASSERT(XMabs(b.z[i])>XM2_FLOAT_ZERO);
      x[i] = a.x[i] / b.x[i];
      y[i] = a.y[i] / b.y[i];
      z[i] = a.z[i] / b.z[i];
    #else
      x[i] = a.x[i] * ((XMabs(b.x[i])>XM2_FLOAT_ZERO)?1.0f/b.x[i]:0.0f);
      y[i] = a.y[i] * ((XMabs(b.y[i])>XM2_FLOAT_ZERO)?1.0f/b.y[i]:0.0f);
      z[i] = a.z[i] * ((XMabs(b.z[i])>XM2_FLOAT_ZERO)?1.0f/b.z[i]:0.0f);
    #endif
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector4SOA& b)
{
  for (int i=0;i!=4;++i)
  {
    #if XM2_DIVIDE_BY_ZERO_MODE
      XM2_ASSERT(XMabs(b.x[i])>XM2_FLOAT_ZERO);
      XM2_ASSERT(XMabs(b.y[i])>XM2_FLOAT_ZERO);
      XM2_ASSERT(XMabs(b.z[i])>XM2_FLOAT_ZERO);
      x[i] = a.x[i] / b.x[i];
      y[i] = a.y[i] / b.y[i];
      z[i] = a.z[i] / b.z[i];
    #else
      x[i] = a.x[i] * ((XMabs(b.x[i])>XM2_FLOAT_ZERO)?1.0f/b.x[i]:0.0f);
      y[i] = a.y[i] * ((XMabs(b.y[i])>XM2_FLOAT_ZERO)?1.0f/b.y[i]:0.0f);
      z[i] = a.z[i] * ((XMabs(b.z[i])>XM2_FLOAT_ZERO)?1.0f/b.z[i]:0.0f);
    #endif
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XReal b)
{
  XReal f;
  #if XM2_DIVIDE_BY_ZERO_MODE
  XM2_ASSERT(XMabs(b)>XM2_FLOAT_ZERO);
  f = 1.0f/b;
  #else
  f = (XMabs(b)>XM2_FLOAT_ZERO) ? 1.0f/b : 0.0f;
  #endif
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] * f;
    y[i] = a.y[i] * f;
    z[i] = a.z[i] * f;
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::dot(XVector4& _dot,const XVector3SOA& b) const
{
  for(int i=0;i!=4;++i)
  {
    _dot.data[i] = x[i]*b.x[i] + 
                   y[i]*b.y[i] + 
                   z[i]*b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::cross(const XVector3SOA& a,const XVector3SOA& b)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = a.y[i] * b.z[i]  -  a.z[i] * b.y[i];
    y[i] = a.z[i] * b.x[i]  -  a.x[i] * b.z[i];
    z[i] = a.x[i] * b.y[i]  -  a.y[i] * b.x[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::lerp(const XVector3SOA& a,const XVector3SOA& b,const XReal t)
{
  for (int i=0;i!=4;++i)
  {
    x[i] = a.x[i] + t * (b.x[i] - a.x[i]);
    y[i] = a.y[i] + t * (b.y[i] - a.y[i]);
    z[i] = a.z[i] + t * (b.z[i] - a.z[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::bezier(const XVector3SOA& a,const XVector3SOA& b,const XVector3SOA& c,const XVector3SOA& d,const XReal t)
{
  const XReal B0 = t*t*t;
  const XReal B1 = 3.0f*(1.0f-t)*t*t;
  const XReal B2 = 3.0f*(1.0f-t)*(1.0f-t)*t;
  const XReal B3 = (1.0f-t)*(1.0f-t)*(1.0f-t);
  for (int i=0;i!=4;++i)
  {
    x[i] = B0 * a.x[i] + B1 * b.x[i] + B2 * c.x[i] + B3 * d.x[i];
    y[i] = B0 * a.y[i] + B1 * b.y[i] + B2 * c.y[i] + B3 * d.y[i];
    z[i] = B0 * a.z[i] + B1 * b.z[i] + B2 * c.z[i] + B3 * d.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::negate(const XVector3SOA& a)
{
  for(int i=0;i!=4;++i)
  {
    x[i] = -a.x[i];
    y[i] = -a.y[i];
    z[i] = -a.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::negate()
{
  negate(*this);
}
}
#endif
