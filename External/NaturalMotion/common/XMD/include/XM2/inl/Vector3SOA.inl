#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR3SOA__INL__
#define XM2_VECTOR3SOA__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set0(const XReal x_,const XReal y_,const XReal z_)
{
  x[0] = x_;
  y[0] = y_;
  z[0] = z_;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set1(const XReal x_,const XReal y_,const XReal z_)
{
  x[1] = x_;
  y[1] = y_;
  z[1] = z_;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set2(const XReal x_,const XReal y_,const XReal z_)
{
  x[2] = x_;
  y[2] = y_;
  z[2] = z_;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set3(const XReal x_,const XReal y_,const XReal z_)
{
  x[3] = x_;
  y[3] = y_;
  z[3] = z_;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set0(const XVector3& vec)
{
  x[0] = vec.x;
  y[0] = vec.y;
  z[0] = vec.z;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set1(const XVector3& vec)
{
  x[1] = vec.x;
  y[1] = vec.y;
  z[1] = vec.z;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set2(const XVector3& vec)
{
  x[2] = vec.x;
  y[2] = vec.y;
  z[2] = vec.z;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::set3(const XVector3& vec)
{
  x[3] = vec.x;
  y[3] = vec.y;
  z[3] = vec.z;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::get0(XVector3& vec) const 
{
  vec.x = x[0];
  vec.y = y[0];
  vec.z = z[0];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::get1(XVector3& vec) const
{
  vec.x = x[1];
  vec.y = y[1];
  vec.z = z[1];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::get2(XVector3& vec) const
{
  vec.x = x[2];
  vec.y = y[2];
  vec.z = z[2];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::get3(XVector3& vec) const
{
  vec.x = x[3];
  vec.y = y[3];
  vec.z = z[3];
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector3 XVector3SOA::get0() const
{
  return XVector3( x[0], y[0], z[0] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector3 XVector3SOA::get1() const
{
  return XVector3( x[1], y[1], z[1] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector3 XVector3SOA::get2() const
{
  return XVector3( x[2], y[2], z[2] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector3 XVector3SOA::get3() const
{
  return XVector3( x[3], y[3], z[3] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector3SOA::equal(const XVector3SOA& b,const XReal eps) const
{
  return 
    float_equal(x[0],b.x[0],eps) && float_equal(x[1],b.x[1],eps) && 
    float_equal(x[2],b.x[2],eps) && float_equal(x[3],b.x[3],eps) &&
    float_equal(y[0],b.y[0],eps) && float_equal(y[1],b.y[1],eps) && 
    float_equal(y[2],b.y[2],eps) && float_equal(y[3],b.y[3],eps) &&
    float_equal(z[0],b.z[0],eps) && float_equal(z[1],b.z[1],eps) && 
    float_equal(z[2],b.z[2],eps) && float_equal(z[3],b.z[3],eps);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector3SOA::notEqual(const XVector3SOA& b,const XReal eps) const
{
  return !equal(b,eps);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::normalise()
{
  normalise(*this);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::add(const XVector3SOA& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::div(const XVector3SOA& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::add(const XVector4SOA& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::sub(const XVector4SOA& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::div(const XVector4SOA& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::mul(const XVector4SOA& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::add(const XVector3& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::sub(const XVector3& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::div(const XVector3& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::mul(const XVector3& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::add(const XReal b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::sub(const XReal b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::div(const XReal b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::mul(const XReal b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector3SOA::operator == (const XVector3SOA& c) const
{
  return equal(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector3SOA::operator != (const XVector3SOA& c) const
{
  return notEqual(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator = (const XVector3SOA& c)
{
  memcpy(this,&c,sizeof(XVector3SOA));
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator += (const XVector3SOA& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator -= (const XVector3SOA& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator /= (const XVector3SOA& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator *= (const XVector3SOA& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator = (const XVector4SOA& c)
{
  memcpy(this,&c,sizeof(XVector3SOA));
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator += (const XVector4SOA& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator -= (const XVector4SOA& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator /= (const XVector4SOA& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator *= (const XVector4SOA& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator += (const XVector3& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator -= (const XVector3& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator /= (const XVector3& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator *= (const XVector3& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator += (const XReal c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector3SOA& XVector3SOA::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3SOA* b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3SOA& b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3& b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayAdd(XVector3SOA* output,const XReal b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arraySub(XVector3SOA* output,const XVector3SOA* b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arraySub(XVector3SOA* output,const XVector3SOA& b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arraySub(XVector3SOA* output,const XVector3& b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arraySub(XVector3SOA* output,const XReal b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3SOA* b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3SOA& b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3& b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayMul(XVector3SOA* output,const XReal b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3SOA* b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3SOA& b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3& b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayDiv(XVector3SOA* output,const XReal b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayNormalise(XVector3SOA* output,const int num)
{
  arrayNormalise(output,output,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector3SOA::arrayNegate(XVector3SOA* output,const int num)
{
  arrayNegate(output,output,num);
}
#pragma warning(pop)   
}
#endif
