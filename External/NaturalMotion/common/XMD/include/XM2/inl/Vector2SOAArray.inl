#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR2SOA_ARRAY__INL__
#define XM2_VECTOR2SOA_ARRAY__INL__
namespace XM2
{  
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator*=(const XVector2SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector2SOA::arrayMul(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator/=(const XVector2SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector2SOA::arrayDiv(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator+=(const XVector2SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector2SOA::arrayAdd(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator-=(const XVector2SOAArray& rhs)
{
  XM2_ASSERT(size() == rhs.size());
  XVector2SOA::arraySub(buffer(),buffer(),rhs.buffer(),(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator*=(const XVector2SOA& rhs)
{
  XVector2SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator/=(const XVector2SOA& rhs)
{
  XVector2SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator+=(const XVector2SOA& rhs)
{
  XVector2SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator-=(const XVector2SOA& rhs)
{
  XVector2SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator*=(const XVector2& rhs)
{
  XVector2SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator/=(const XVector2& rhs)
{
  XVector2SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator+=(const XVector2& rhs)
{
  XVector2SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator-=(const XVector2& rhs)
{
  XVector2SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator*=(const XReal rhs)
{
  XVector2SOA::arrayMul(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator/=(const XReal rhs)
{
  XVector2SOA::arrayDiv(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator+=(const XReal rhs)
{
  XVector2SOA::arrayAdd(buffer(),buffer(),rhs,(int)size());
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE const XVector2SOAArray& XVector2SOAArray::operator-=(const XReal rhs)
{
  XVector2SOA::arraySub(buffer(),buffer(),rhs,(int)size());
  return *this;
}
}
#endif
