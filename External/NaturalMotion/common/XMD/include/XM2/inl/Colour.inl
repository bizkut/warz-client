#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_COLOUR__INL__
#define XM2_COLOUR__INL__
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour()
{
  #if XM2_INIT_CLASSES
  zero();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XColour& c)
{
  *this = c;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal r,const XReal g,const XReal b,const XReal a)
{
  set(r,g,b,a);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColour::notEqual(const XColour& _b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_b);

  return !equal(_b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator += (const XColour& c)
{
  add(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator -= (const XColour& c)
{
  sub(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator /= (const XColour& c)
{
  div(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator *= (const XColour& c)
{
  mul(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator += (const XReal c)
{
  add(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator -= (const XReal c)
{
  sub(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator /= (const XReal c)
{
  div(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator *= (const XReal c)
{
  mul(*this,c);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColour::operator == (const XColour& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColour::operator != (const XColour& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator + (const XColour& c) const
{
  XColour cl;
  cl.add(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator - (const XColour& c) const
{
  XColour cl;
  cl.sub(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator / (const XColour& c) const
{
  XColour cl;
  cl.div(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator * (const XColour& c) const
{
  XColour cl;
  cl.mul(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator + (const XReal c) const
{
  XColour cl;
  cl.add(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator - (const XReal c) const
{
  XColour cl;
  cl.sub(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator / (const XReal c) const
{
  XColour cl;
  cl.div(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour XColour::operator * (const XReal c) const
{
  XColour cl;
  cl.mul(*this,c);
  return cl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayMul(XColour* output,const XColour* b,const int num) 
{ 
  XColour::arrayMul(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayMul(XColour* output,const XColour& b,const int num) 
{ 
  XColour::arrayMul(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayMul(XColour* output,const XReal* b,const int num) 
{ 
  XColour::arrayMul(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayMul(XColour* output,const XReal b,const int num) 
{ 
  XColour::arrayMul(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayDiv(XColour* output,const XColour* b,const int num) 
{ 
  XColour::arrayDiv(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayDiv(XColour* output,const XColour& b,const int num) 
{ 
  XColour::arrayDiv(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayDiv(XColour* output,const XReal* b,const int num) 
{ 
  XColour::arrayDiv(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayDiv(XColour* output,const XReal b,const int num) 
{ 
  XColour::arrayDiv(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayAdd(XColour* output,const XColour* b,const int num) 
{ 
  XColour::arrayAdd(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayAdd(XColour* output,const XColour& b,const int num) 
{ 
  XColour::arrayAdd(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayAdd(XColour* output,const XReal* b,const int num) 
{ 
  XColour::arrayAdd(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayAdd(XColour* output,const XReal b,const int num) 
{ 
  XColour::arrayAdd(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arraySub(XColour* output,const XColour* b,const int num) 
{ 
  XColour::arraySub(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arraySub(XColour* output,const XColour& b,const int num) 
{ 
  XColour::arraySub(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arraySub(XColour* output,const XReal* b,const int num) 
{ 
  XColour::arraySub(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arraySub(XColour* output,const XReal b,const int num) 
{ 
  XColour::arraySub(output,output,b,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayInvert(XColour* output,const int num) 
{ 
  XColour::arrayInvert(output,output,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::arrayNegate(XColour* output,const int num) 
{ 
  XColour::arrayNegate(output,output,num); 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::invert()
{
  invert(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::negate()
{
  negate(*this);
}
#pragma warning(pop)
}
#endif
