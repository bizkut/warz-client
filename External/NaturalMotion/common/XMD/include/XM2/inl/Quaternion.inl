#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_QUATERNION__INL__
#define XM2_QUATERNION__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion()
{
  #if XM2_INIT_CLASSES
  identity();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion(const XQuaternion& q)
{
  copy(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion(const XMatrix& m)
{
  fromMatrix(m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion(const XVector3& a,const XReal radians)
{
  fromAxisAngleR(a,radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion(const XReal x,const XReal y,const XReal z,const XReal w)
{
  set(x,y,z,w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::arrayMul(XQuaternion* output, const XReal* b, int num)
{
  arrayMul(output,output,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::arrayMul(XQuaternion* output, XReal b, int num)
{
  arrayMul(output,output,b,num);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::invert()
{
  invert(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::negate()
{
  negate(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XQuaternion::length() const
{
  return XMsqrt(dot(*this));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::normalise()
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toAxisAngleD(XVector3& axis,XReal& angle) const
{
  toAxisAngleR(axis,angle);
  angle = radToDeg(angle);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::notEqual(const XQuaternion& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::equivalent(const XQuaternion& b,const XReal eps) const
{
  XQuaternion c;
  c.negate(b);
  return equal(c,eps) || equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::notEquivalent(const XQuaternion& b,const XReal eps) const
{
  return !equivalent(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toEulerAnglesD(XReal& x,XReal& y,XReal& z) const
{
  toEulerAnglesR(x,y,z);
  x = radToDeg(x);
  y = radToDeg(y);
  z = radToDeg(z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromAxisAngleD(const XVector3& axis,const XReal degrees)
{
  fromAxisAngleR(axis,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromAxisAngleR(const XVector3& axis,const XReal radians)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);

  const XReal sin_a = XMsin( radians * 0.5f );
  const XReal cos_a = XMcos( radians * 0.5f );

  XVector3 temp;
  temp.normalise(axis);
  temp.mul(sin_a);
  x = temp.x;
  y = temp.y;
  z = temp.z;
  w = cos_a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromEulerAnglesD(const XReal x,const XReal y,const XReal z)
{
  fromEulerAnglesR(degToRad(x),degToRad(y),degToRad(z));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromEulerAnglesR(const XReal x,const XReal y,const XReal z)
{
  XM2_ALIGN_ASSERT(this);

  const XVector3 vx(1.0f, 0.0f, 0.0f);
  const XVector3 vy(0.0f, 1.0f, 0.0f);
  const XVector3 vz(0.0f, 0.0f, 1.0f);
  XQuaternion qx, qy, qz, qt;

  qx.fromAxisAngleR(vx,x);
  qy.fromAxisAngleR(vy,y);
  qz.fromAxisAngleR(vz,z);
  qt.mul(qx,qy);
  mul(qt,qz);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::add(const XQuaternion& a)
{
  add(*this,a);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XReal b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a)
{
  XQuaternion temp = *this;
  mul(temp,a);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::operator == (const XQuaternion& q) const
{
  return equal(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::operator != (const XQuaternion& q) const
{
  return notEqual(q);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator = (const XQuaternion& q)
{
  copy(q);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator = (const XMatrix& m)
{
  fromMatrix(m);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator *= (const XQuaternion& q)
{
  XQuaternion temp = *this;
  mul(temp,q);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator *= (const XReal c)
{
  mul(*this,c);
  return *this;
}
#pragma warning(pop)   
}
#endif
