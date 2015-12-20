#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_REALARRAY__INL__
#define XM2_SSE_REALARRAY__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator*=(const XRealArray& rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator/=(const XRealArray& rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator+=(const XRealArray& rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator-=(const XRealArray& rhs)
{
  sub(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator*=(const XReal rhs)
{
  mul(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator/=(const XReal rhs)
{
  div(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator+=(const XReal rhs)
{
  add(rhs);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XRealArray& XRealArray::operator-=(const XReal rhs)
{
  sub(rhs);
  return *this;
}
}
#endif
