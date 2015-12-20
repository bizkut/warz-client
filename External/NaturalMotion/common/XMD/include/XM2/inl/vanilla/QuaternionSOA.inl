#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_QUATERNION_SOA__INL__
#define XM2_VANILLA_QUATERNION_SOA__INL__
#if (!XM2_USE_FLOAT)
# error float precision routines included when set to build using doubles
#endif
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA()
{
  #if XM2_INIT_CLASSES
  identity();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const XQuaternionSOA& q)
{
  copy(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const XQuaternion quats[4])
{
  set0(quats[0]);
  set1(quats[1]);
  set2(quats[2]);
  set3(quats[3]);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::streamIn(const XQuaternion* input)
{
  for (int i=0;i<4;++i)
  {
    x[i] = input[i].x;
    y[i] = input[i].y;
    z[i] = input[i].z;
    w[i] = input[i].w;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::streamOut(XQuaternion* output) const
{
  for (int i=0;i<4;++i)
  {
    output[i].x = x[i];
    output[i].y = y[i];
    output[i].z = z[i];
    output[i].w = w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set0(const XQuaternion& vec)
{
  x[0] = vec.x;
  y[0] = vec.y;
  z[0] = vec.z;
  w[0] = vec.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set1(const XQuaternion& vec)
{
  x[1] = vec.x;
  y[1] = vec.y;
  z[1] = vec.z;
  w[1] = vec.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set2(const XQuaternion& vec)
{
  x[2] = vec.x;
  y[2] = vec.y;
  z[2] = vec.z;
  w[2] = vec.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set3(const XQuaternion& vec)
{
  x[3] = vec.x;
  y[3] = vec.y;
  z[3] = vec.z;
  w[3] = vec.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get0(XQuaternion& vec) const
{
  vec.x = x[0];
  vec.y = y[0];
  vec.z = z[0];
  vec.w = w[0];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get1(XQuaternion& vec) const
{
  vec.x = x[1];
  vec.y = y[1];
  vec.z = z[1];
  vec.w = w[1];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get2(XQuaternion& vec) const
{
  vec.x = x[2];
  vec.y = y[2];
  vec.z = z[2];
  vec.w = w[2];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get3(XQuaternion& vec) const
{
  vec.x = x[3];
  vec.y = y[3];
  vec.z = z[3];
  vec.w = w[3];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromAxisAngleD(const XQuaternionSOA& axis)
{
  XQuaternion q;
  q.fromAxisAngleD( XVector3(axis.x[0],axis.y[0],axis.z[0]), axis.w[0] );
  set0(q);
  q.fromAxisAngleD( XVector3(axis.x[1],axis.y[1],axis.z[1]), axis.w[1] );
  set1(q);
  q.fromAxisAngleD( XVector3(axis.x[2],axis.y[2],axis.z[2]), axis.w[2] );
  set2(q);
  q.fromAxisAngleD( XVector3(axis.x[3],axis.y[3],axis.z[3]), axis.w[3] );
  set3(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromAxisAngleR(const XQuaternionSOA& axis)
{
  XQuaternion q;
  q.fromAxisAngleR( XVector3(axis.x[0],axis.y[0],axis.z[0]), axis.w[0] );
  set0(q);
  q.fromAxisAngleR( XVector3(axis.x[1],axis.y[1],axis.z[1]), axis.w[1] );
  set1(q);
  q.fromAxisAngleR( XVector3(axis.x[2],axis.y[2],axis.z[2]), axis.w[2] );
  set2(q);
  q.fromAxisAngleR( XVector3(axis.x[3],axis.y[3],axis.z[3]), axis.w[3] );
  set3(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toAxisAngleD(XQuaternionSOA& axis) const
{
  XVector3 _axis;
  get0().toAxisAngleD( _axis, axis.w[0] );
  axis.x[0] = _axis.x; axis.y[0] = _axis.y; axis.z[0] = _axis.z;
  get1().toAxisAngleD( _axis, axis.w[1] );
  axis.x[1] = _axis.x; axis.y[1] = _axis.y; axis.z[1] = _axis.z;
  get2().toAxisAngleD( _axis, axis.w[2] );
  axis.x[2] = _axis.x; axis.y[2] = _axis.y; axis.z[2] = _axis.z;
  get3().toAxisAngleD( _axis, axis.w[3] );
  axis.x[3] = _axis.x; axis.y[3] = _axis.y; axis.z[3] = _axis.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toAxisAngleR(XQuaternionSOA& axis) const
{
  XVector3 _axis;
  get0().toAxisAngleR( _axis, axis.w[0] );
  axis.x[0] = _axis.x; axis.y[0] = _axis.y; axis.z[0] = _axis.z;
  get1().toAxisAngleR( _axis, axis.w[1] );
  axis.x[1] = _axis.x; axis.y[1] = _axis.y; axis.z[1] = _axis.z;
  get2().toAxisAngleR( _axis, axis.w[2] );
  axis.x[2] = _axis.x; axis.y[2] = _axis.y; axis.z[2] = _axis.z;
  get3().toAxisAngleR( _axis, axis.w[3] );
  axis.x[3] = _axis.x; axis.y[3] = _axis.y; axis.z[3] = _axis.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toEulerAnglesD(XVector3SOA& eulers) const
{
  get0().toEulerAnglesD( eulers.x[0], eulers.y[0], eulers.z[0] );
  get1().toEulerAnglesD( eulers.x[1], eulers.y[1], eulers.z[1] );
  get2().toEulerAnglesD( eulers.x[2], eulers.y[2], eulers.z[2] );
  get3().toEulerAnglesD( eulers.x[3], eulers.y[3], eulers.z[3] );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toEulerAnglesR(XVector3SOA& eulers) const
{
  get0().toEulerAnglesR( eulers.x[0], eulers.y[0], eulers.z[0] );
  get1().toEulerAnglesR( eulers.x[1], eulers.y[1], eulers.z[1] );
  get2().toEulerAnglesR( eulers.x[2], eulers.y[2], eulers.z[2] );
  get3().toEulerAnglesR( eulers.x[3], eulers.y[3], eulers.z[3] );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromEulerAnglesD(const XVector3SOA& eulers)
{
  XVector3SOA temp=eulers;
  for(int i=0;i<4;++i)
  {
    temp.x[i] = degToRad(temp.x[i]);
    temp.y[i] = degToRad(temp.y[i]);
    temp.z[i] = degToRad(temp.z[i]);
  }
  fromEulerAnglesR(eulers);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromEulerAnglesR(const XVector3SOA& eulers)
{
  XQuaternion q;
  q.fromEulerAnglesR( eulers.x[0], eulers.y[0], eulers.z[0] );
  set0(q);
  q.fromEulerAnglesR( eulers.x[1], eulers.y[1], eulers.z[1] );
  set1(q);
  q.fromEulerAnglesR( eulers.x[2], eulers.y[2], eulers.z[2] );
  set2(q);
  q.fromEulerAnglesR( eulers.x[3], eulers.y[3], eulers.z[3] );
  set3(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::identity()
{
  memset(this,0,sizeof(XQuaternionSOA));
  w[0] = w[1] = w[2] = w[3] = 1.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::copy(const XQuaternionSOA& q)
{
  memcpy(this,&q,sizeof(XQuaternionSOA));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::length(XVector4& len) const
{
  dot(len,*this);
  len.x = XMsqrt(len.x);
  len.y = XMsqrt(len.y);
  len.z = XMsqrt(len.z);
  len.w = XMsqrt(len.w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionSOA::equal(const XQuaternionSOA& b,const XReal eps) const
{
  for(int i=0;i<4;++i)
  {
    if(!float_equal(x[i],b.x[i],eps) ||
       !float_equal(y[i],b.y[i],eps) ||
       !float_equal(z[i],b.z[i],eps) ||
       !float_equal(w[i],b.w[i],eps))
       return false;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::dot(XVector4& product,const XQuaternionSOA& b) const
{
  for(int i=0;i<4;++i)
  {
    product.data[i] = x[i]*b.x[i] + 
                      y[i]*b.y[i] + 
                      z[i]*b.z[i] + 
                      w[i]*b.w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a,const XQuaternionSOA& b)
{
  for(int i=0;i<4;++i)
  {
    x[i] = a.x[i]+b.x[i];
    y[i] = a.y[i]+b.y[i];
    z[i] = a.z[i]+b.z[i];
    w[i] = a.w[i]+b.w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a,const XQuaternion& b)
{
  for(int i=0;i<4;++i)
  {
    x[i] = a.x[i]+b.x;
    y[i] = a.y[i]+b.y;
    z[i] = a.z[i]+b.z;
    w[i] = a.w[i]+b.w;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a)
{
  add(*this,a);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XQuaternionSOA& b)
{
  // args can't be the same as this - values will be modifed
  XM2_ASSERT(&a != this);
  XM2_ASSERT(&b != this);

  for(int i=0;i<4;++i)
  {
    x[i] = a.w[i]*b.x[i] + a.x[i]*b.w[i] + b.y[i]*a.z[i] - b.z[i]*a.y[i];
    y[i] = a.w[i]*b.y[i] + a.y[i]*b.w[i] + b.z[i]*a.x[i] - b.x[i]*a.z[i];
    z[i] = a.w[i]*b.z[i] + a.z[i]*b.w[i] + b.x[i]*a.y[i] - b.y[i]*a.x[i];
    w[i] = a.w[i]*b.w[i] - a.x[i]*b.x[i] - a.y[i]*b.y[i] - a.z[i]*b.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XQuaternion& b)
{
  // args can't be the same as this - values will be modifed
  XM2_ASSERT(&a != this);

  for(int i=0;i<4;++i)
  {
    x[i] = a.w[i]*b.x + a.x[i]*b.w + b.y*a.z[i] - b.z*a.y[i];
    y[i] = a.w[i]*b.y + a.y[i]*b.w + b.z*a.x[i] - b.x*a.z[i];
    z[i] = a.w[i]*b.z + a.z[i]*b.w + b.x*a.y[i] - b.y*a.x[i];
    w[i] = a.w[i]*b.w - a.x[i]*b.x - a.y[i]*b.y - a.z[i]*b.z;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XReal b)
{
  for(int i=0;i<4;++i)
  {
    x[i] = a.x[i]*b;
    y[i] = a.y[i]*b; 
    z[i] = a.z[i]*b;
    w[i] = a.w[i]*b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternion& b)
{
  XQuaternionSOA temp(*this);
  mul(temp,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XReal b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::slerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const XReal t)
{
  for(int i=0;i<4;++i)
  {
    XQuaternion First(a.x[i],a.y[i],a.z[i],a.w[i]);
    XQuaternion Second(b.x[i],b.y[i],b.z[i],b.w[i]);

    XReal product,interpolating_t,interpolating_invt,theta,sinTheta;

    // perform dot product
    product = First.dot(Second);

    if(product < 0.001f)
    {
      Second.invert();
      product = -product;
    }

    // calculate the linear t values for the interpolation
    interpolating_t    = t;
    interpolating_invt = 1.0f-t;

    product = (product>1.0f) ? 0.999999f : product;
    product = (product<-1.0f) ? -0.999999f : product;
    theta    = XMacos(product);

    // pre-calculate the sin value
    sinTheta = XMsin(theta);
    sinTheta = float_equal(sinTheta,0.0f) ? 0.0f : 1.0f/sinTheta;

    interpolating_invt = XMsin(interpolating_invt * theta) * sinTheta;
    interpolating_t    = XMsin(interpolating_t    * theta) * sinTheta;

    // perform the interpolation
    x[i] = First.x * interpolating_invt + Second.x * interpolating_t;
    y[i] = First.y * interpolating_invt + Second.y * interpolating_t;
    z[i] = First.z * interpolating_invt + Second.z * interpolating_t;
    w[i] = First.w * interpolating_invt + Second.w * interpolating_t;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::lerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const XReal t)
{
  for(int i=0;i<4;++i)
  {
    //
    XQuaternion First(a.x[i],a.y[i],a.z[i],a.w[i]);
    XQuaternion Second(b.x[i],b.y[i],b.z[i],b.w[i]);

    // perform dot product
    XReal product = First.dot(Second);

    if(product < 0.000001f)
    {
      Second.invert();
      product = -product;
    }

    // perform the interpolation
    x[i] = First.x + (Second.x - First.x) * t;
    y[i] = First.y + (Second.y - First.x) * t;
    z[i] = First.z + (Second.z - First.x) * t;
    w[i] = First.w + (Second.w - First.x) * t;
  }
  normalise();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::invert()
{
  invert(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::invert(const XQuaternionSOA& q)
{
  for(int i=0;i<4;++i)
  {
    x[i] = -q.x[i];
    y[i] = -q.y[i];
    z[i] = -q.z[i];
    w[i] = q.w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::negate()
{
  negate(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::negate(const XQuaternionSOA& q)
{
  for(int i=0;i<4;++i)
  {
    x[i] = -q.x[i];
    y[i] = -q.y[i];
    z[i] = -q.z[i];
    w[i] = -q.w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::normalise()
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::normalise(const XQuaternionSOA& q)
{
  XVector4 l;
  q.length(l);
  for(int i=0;i<4;++i)
  {
    if(XMabs(l.data[i])>XM2_FLOAT_ZERO) 
    { 
      l.data[i] = 1.0f/l.data[i]; 
      x[i] = l.data[i] * q.x[i];
      y[i] = l.data[i] * q.y[i];
      z[i] = l.data[i] * q.z[i];
      w[i] = l.data[i] * q.w[i];
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::sum(const XQuaternionSOA& a,const XReal t)
{
  for (int i=0;i<4;++i)
  {
    x[i] += (t*a.x[i]);
    y[i] += (t*a.y[i]);
    z[i] += (t*a.z[i]);
    w[i] += (t*a.w[i]);
  }
}
}
#endif
