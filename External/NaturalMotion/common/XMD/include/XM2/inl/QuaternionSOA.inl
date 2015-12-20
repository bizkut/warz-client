#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_QUATERNION_SOA__INL__
#define XM2_QUATERNION_SOA__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion XQuaternionSOA::get0() const
{
  XQuaternion q;
  get0(q);
  return q;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion XQuaternionSOA::get1() const
{
  XQuaternion q;
  get1(q);
  return q;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion XQuaternionSOA::get2() const
{
  XQuaternion q;
  get2(q);
  return q;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion XQuaternionSOA::get3() const
{
  XQuaternion q;
  get3(q);
  return q;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set0(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set0(XQuaternion(x_,y_,z_,w_));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set1(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set1(XQuaternion(x_,y_,z_,w_));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set2(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set2(XQuaternion(x_,y_,z_,w_));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set3(const XReal x_,const XReal y_,const XReal z_,const XReal w_)
{
  set3(XQuaternion(x_,y_,z_,w_));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionSOA::notEqual(const XQuaternionSOA& b,const XReal eps) const
{
  return !equal(b,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionSOA::operator == (const XQuaternionSOA& c) const
{
  return equal(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionSOA::operator != (const XQuaternionSOA& c) const
{
  return notEqual(c);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternionSOA& XQuaternionSOA::operator = (const XQuaternionSOA& c)
{
  copy(c);
  return *this;
}
}
#endif
