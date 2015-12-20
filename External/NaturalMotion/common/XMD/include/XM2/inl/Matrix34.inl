#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_COMMON_MATRIX__INL__
#define XM2_COMMON_MATRIX__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix::XMatrix()
{
  #if XM2_INIT_CLASSES
  identity();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix::XMatrix(const XMatrix& c)
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_USE_SECURE_SCL
    memcpy_s(this,sizeof(XMatrix),&c,sizeof(XMatrix));
  #else
    memcpy(this,&c,sizeof(XMatrix));
  #endif 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix::XMatrix(const XQuaternion& c)
{
  fromQuaternion(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4Array& output,const XVector4Array& input) const 
{ 
  output.resize(input.size());
  transformVector(output.buffer(),input.buffer(),(int)output.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3Array& output,const XVector3Array& input) const
{
  output.resize(input.size());
  transformPoint(output.buffer(),input.buffer(),(int)output.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3Array& output,const XVector3Array& input) const
{ 
  output.resize(input.size());
  transformVector(output.buffer(),input.buffer(),(int)output.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3SOAArray& output,const XVector3SOAArray& input) const
{ 
  output.resize(input.size());
  transformPoint(output.buffer(),input.buffer(),(int)output.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3SOAArray& output,const XVector3SOAArray& input) const
{ 
  output.resize(input.size());
  transformVector(output.buffer(),input.buffer(),(int)output.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4SOAArray& output,const XVector4SOAArray& input) const
{ 
  output.resize(input.size());
  transformVector(output.buffer(),input.buffer(),(int)output.size());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotate(const XQuaternion& _rotate)
{
  XMatrix m = _rotate;
  *this *= m;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4& output) const
{
  XVector4 temp = output;
  transformVector(output,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3& output) const
{
  XVector3 temp = output;
  transformPoint(output,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3& output) const
{
  XVector3 temp = output;
  transformVector(output,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3SOA& output) const
{
  XVector3SOA temp = output;
  transformPoint(output,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3SOA& output) const
{
  XVector3SOA temp = output;
  transformVector(output,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4SOA& output) const
{
  XVector4SOA temp = output;
  transformVector(output,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::zero()
{
  XM2_ALIGN_ASSERT(this);
  memset(this,0,sizeof(XMatrix));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrix::equal(const XMatrix& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  return (xAxis().equal(b.xAxis(),eps)) &&
         (yAxis().equal(b.yAxis(),eps)) &&
         (zAxis().equal(b.zAxis(),eps)) &&
         (wAxis().equal(b.wAxis(),eps));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrix::notEqual(const XMatrix& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotXR(const XReal radians)
{
  XMatrix temp(*this);
  rotXR(temp,radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotYR(const XReal radians)
{
  XMatrix temp(*this);
  rotYR(temp,radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotZR(const XReal radians)
{
  XMatrix temp(*this);
  rotZR(temp,radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotXD(const XReal degrees)
{
  XMatrix temp(*this);
  rotXR(temp,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotYD(const XReal degrees)
{
  XMatrix temp(*this);
  rotYR(temp,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotZD(const XReal degrees)
{
  XMatrix temp(*this);
  rotZR(temp,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotXD(const XMatrix& input,const XReal degrees)
{
  rotXR(input,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotYD(const XMatrix& input,const XReal degrees)
{
  rotYR(input,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotZD(const XMatrix& input,const XReal degrees)
{
  rotZR(input,degToRad(degrees));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotXR(const XMatrix& input,const XReal radians)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  XMatrix m;
  #if !XM2_INIT_CLASSES
  m.identity();
  #endif
  const XReal sr = XMsin( radians );
  const XReal cr = XMcos( radians );
  m.m11 =  cr;
  m.m21 = -sr;
  m.m12 =  sr;
  m.m22 =  cr;
  quickMult(input,m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotYR(const XMatrix& input,const XReal radians)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  XMatrix m;
  #if !XM2_INIT_CLASSES
  m.identity();
  #endif
  const XReal sr = XMsin( radians );
  const XReal cr = XMcos( radians );
  m.m00 =  cr;
  m.m20 =  sr;
  m.m02 = -sr;
  m.m22 =  cr;
  quickMult(input,m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::rotZR(const XMatrix& input,const XReal radians)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  XMatrix m;
  #if !XM2_INIT_CLASSES
  m.identity();
  #endif
  const XReal sr = XMsin( radians );
  const XReal cr = XMcos( radians );
  m.m00 =  cr;
  m.m10 = -sr;
  m.m01 =  sr;
  m.m11 =  cr;
  quickMult(input,m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::scale(const XMatrix& input,const XVector3& scale)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  XM2_ALIGN_ASSERT(&scale);
  XMatrix m;
  #if !XM2_INIT_CLASSES
  m.identity();
  #endif
  m.m00 = scale.x;
  m.m11 = scale.y;
  m.m22 = scale.z;
  quickMult(input,m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::translate(const XMatrix& input,const XVector3& translate)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  XM2_ALIGN_ASSERT(&translate);
  XMatrix m;
  #if !XM2_INIT_CLASSES
  m.identity();
  #endif
  m.m30 = translate.x;
  m.m31 = translate.y;
  m.m32 = translate.z;
  quickMult(input,m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::scale(const XVector3& _scale)
{
  XMatrix temp(*this);
  scale(temp,_scale);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::translate(const XVector3& _translate)
{
  XMatrix temp(*this);
  translate(temp,_translate);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::scale(const XReal x,const XReal y,const XReal z)
{
  scale(XVector3(x,y,z));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::translate(const XReal x,const XReal y,const XReal z)
{
  translate(XVector3(x,y,z));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::toQuaternion(XQuaternion& output) const
{
  output.fromMatrix(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::toAxisAngleR(XVector3& axis,XReal& radians) const
{
  XQuaternion q;
  q.fromMatrix(*this);
  q.toAxisAngleR(axis,radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::toAxisAngleD(XVector3& axis,XReal& degrees) const
{
  XQuaternion q;
  q.fromMatrix(*this);
  q.toAxisAngleD(axis,degrees);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::toEulerAnglesR(XReal& xr,XReal& yr,XReal& zr) const
{
  XQuaternion q;
  q.fromMatrix(*this);
  q.toEulerAnglesR(xr,yr,zr);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::toEulerAnglesD(XReal& xd,XReal& yd,XReal& zd) const
{
  XQuaternion q;
  q.fromMatrix(*this);
  q.toEulerAnglesR(xd,yd,zd);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::fromQuaternion(const XQuaternion& output)
{
  output.toMatrix(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::fromAxisAngleR(const XVector3& axis,const XReal radians)
{
  XQuaternion q;
  q.fromAxisAngleR(axis,radians);
  q.toMatrix(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::fromAxisAngleD(const XVector3& axis,const XReal degrees)
{
  XQuaternion q;
  q.fromAxisAngleD(axis,degrees);
  q.toMatrix(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::fromEulerAnglesR(const XReal xr,const XReal yr,const XReal zr)
{
  XMatrix temp;
  identity();
  temp.identity();
  rotXR(temp,xr);
  temp.rotYR(*this,yr);
  rotZR(temp,zr);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::fromEulerAnglesD(const XReal xd,const XReal yd,const XReal zd)
{
  fromEulerAnglesR(degToRad(xd),degToRad(yd),degToRad(zd));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrix::getScale(XVector3& output) const
{
  XM2_ALIGN_ASSERT(&output);
  XM2_ALIGN_ASSERT(this);
  output.x = xAxis().length();
  output.y = yAxis().length();
  output.z = zAxis().length();
  // now determine if negative scaling is present by creating a temp Z axis
  // by crossing the X and Y axes. 
  XVector3 testZ;
  testZ.cross(xAxis(),yAxis());
  // if our temp Z axis is facing the other way to the actual Z axis, then we 
  // have a matrix with negative scale. 
  bool negative = testZ.dot(zAxis()) < 1.0f;
  // so just invert our X scale value (since we can't determine which of the original axes
  // were scaled negatively...)
  output.x = negative ? -output.x : output.x;
  return !negative;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::getTranslate(XVector3& output) const
{
  XM2_ALIGN_ASSERT(&output);
  XM2_ALIGN_ASSERT(this);
  output.x = m30;
  output.y = m31;
  output.z = m32;
  output.w = 0.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::decompose(XVector3& T,XQuaternion& R,XVector3& S) const
{
  bool xneg = getScale(S);
  getTranslate(T);
  XMatrix temp;
  temp.xAxis().normalise(xAxis());
  temp.yAxis().normalise(yAxis());
  temp.zAxis().normalise(zAxis());
  temp.wAxis().zero();
  if(!xneg)
  {
    temp.xAxis().negate(temp.xAxis());
  }
  temp.toQuaternion(R);
  R.normalise();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::invert()
{
  XMatrix _copy = *this;
  invert(_copy);
}
#ifdef XM2_USE_SSE
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transpose()
{
  transpose(*this);
}
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrix::operator == (const XMatrix& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrix::operator != (const XMatrix& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrix& XMatrix::operator = (const XMatrix& c)
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_USE_SECURE_SCL
    memcpy_s(this,sizeof(XMatrix),&c,sizeof(XMatrix));
  #else
    memcpy(this,&c,sizeof(XMatrix));      
  #endif
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrix& XMatrix::operator *= (const XMatrix& c)
{
  XMatrix temp = *this;
  mul(temp,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix XMatrix::operator * (const XMatrix& c) const
{
  XMatrix m;
  m.mul(*this,c);
  return m;
}
#pragma warning(pop)   
}
#endif
