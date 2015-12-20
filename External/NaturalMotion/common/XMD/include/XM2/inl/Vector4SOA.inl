#ifdef _MSC_VER
# pragma once
#endif

#ifndef XM2_VECTOR4SOA__INL__
#define XM2_VECTOR4SOA__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set0(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set0(XVector4(x_,y_,z_,w_));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set1(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set1(XVector4(x_,y_,z_,w_));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set2(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set2(XVector4(x_,y_,z_,w_));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set3(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set3(XVector4(x_,y_,z_,w_));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4& b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4& b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4& b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4& b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XReal b)
{
  add(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XReal b)
{
  sub(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XReal b)
{
  div(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XReal b)
{
  mul(*this,b);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4 XVector4SOA::get0() const
{
  return XVector4( x[0], y[0], z[0], w[0] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4 XVector4SOA::get1() const
{
  return XVector4( x[1], y[1], z[1], w[1] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4 XVector4SOA::get2() const
{
  return XVector4( x[2], y[2], z[2], w[2] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4 XVector4SOA::get3() const
{
  return XVector4( x[3], y[3], z[3], w[3] );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector4SOA::equal(const XVector4SOA& b,const XReal eps) const
{
return float_equal(x[0],b.x[0],eps) && float_equal(y[0],b.y[0],eps) && float_equal(z[0],b.z[0],eps) && float_equal(w[0],b.w[0],eps) &&
       float_equal(x[1],b.x[1],eps) && float_equal(y[1],b.y[1],eps) && float_equal(z[1],b.z[1],eps) && float_equal(w[1],b.w[1],eps) &&
       float_equal(x[2],b.x[2],eps) && float_equal(y[2],b.y[2],eps) && float_equal(z[2],b.z[2],eps) && float_equal(w[2],b.w[2],eps) &&
       float_equal(x[3],b.x[3],eps) && float_equal(y[3],b.y[3],eps) && float_equal(z[3],b.z[3],eps) && float_equal(w[3],b.w[3],eps);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector4SOA::notEqual(const XVector4SOA& b,const XReal eps) const
{
  return !equal(b,eps);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector4SOA::operator == (const XVector4SOA& c) const
{
  return equal(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE bool XVector4SOA::operator != (const XVector4SOA& c) const
{
  return notEqual(c);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator = (const XVector4SOA& c)
{
  memcpy(this,&c,sizeof(XVector4SOA));
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator = (const XVector4& c)
{
  x[0] = c.x; x[1] = c.x; x[2] = c.x; x[3] = c.x;
  y[0] = c.y; y[1] = c.y; y[2] = c.y; y[3] = c.y;
  z[0] = c.z; z[1] = c.z; z[2] = c.z; z[3] = c.z;
  w[0] = c.w; w[1] = c.w; w[2] = c.w; w[3] = c.w;
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator = (const XReal c)
{
  x[0] = c; x[1] = c; x[2] = c; x[3] = c;
  y[0] = c; y[1] = c; y[2] = c; y[3] = c;
  z[0] = c; z[1] = c; z[2] = c; z[3] = c;
  w[0] = c; w[1] = c; w[2] = c; w[3] = c;
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator += (const XVector4SOA& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator -= (const XVector4SOA& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator *= (const XVector4SOA& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator /= (const XVector4SOA& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator += (const XVector4& c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator -= (const XVector4& c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator *= (const XVector4& c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator /= (const XVector4& c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator += (const XReal c)
{
  add(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator -= (const XReal c)
{
  sub(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator /= (const XReal c)
{
  div(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE const XVector4SOA& XVector4SOA::operator *= (const XReal c)
{
  mul(c);
  return *this;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator + (const XVector4SOA& c) const
{
  XVector4SOA temp;
  temp.add(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator - (const XVector4SOA& c) const
{
  XVector4SOA temp;
  temp.sub(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator * (const XVector4SOA& c) const
{
  XVector4SOA temp;
  temp.mul(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator / (const XVector4SOA& c) const
{
  XVector4SOA temp;
  temp.div(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator + (const XVector4& c) const
{
  XVector4SOA temp;
  temp.add(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator - (const XVector4& c) const
{
  XVector4SOA temp;
  temp.sub(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator * (const XVector4& c) const
{
  XVector4SOA temp;
  temp.mul(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator / (const XVector4& c) const
{
  XVector4SOA temp;
  temp.div(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator + (const XReal c) const
{
  XVector4SOA temp;
  temp.add(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator - (const XReal c) const
{
  XVector4SOA temp;
  temp.sub(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator / (const XReal c) const
{
  XVector4SOA temp;
  temp.div(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE XVector4SOA XVector4SOA::operator * (const XReal c) const
{
  XVector4SOA temp;
  temp.mul(*this,c);
  return temp;
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4SOA* b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4SOA& b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4& b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayAdd(XVector4SOA* output,const XReal b,const int num)
{
  arrayAdd(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arraySub(XVector4SOA* output,const XVector4SOA* b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arraySub(XVector4SOA* output,const XVector4SOA& b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arraySub(XVector4SOA* output,const XVector4& b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arraySub(XVector4SOA* output,const XReal b,const int num)
{
  arraySub(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4SOA* b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4SOA& b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4& b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayMul(XVector4SOA* output,const XReal b,const int num)
{
  arrayMul(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4SOA* b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4SOA& b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4& b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayDiv(XVector4SOA* output,const XReal b,const int num)
{
  arrayDiv(output,output,b,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayNormalise(XVector4SOA* output,const int num)
{
  arrayNormalise(output,output,num);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::arrayNegate(XVector4SOA* output,const int num)
{
  arrayNegate(output,output,num);
}
#pragma warning(pop)   
}

#endif

