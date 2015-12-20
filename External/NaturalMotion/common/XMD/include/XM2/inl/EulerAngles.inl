#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_EULER_ANGLES__INL__
#define XM2_EULER_ANGLES__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XEulerAngles::XEulerAngles(XRotationOrder ro)
  : m_xRotation(0), m_yRotation(0), m_zRotation(0), m_RotationOrder(ro) 
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XEulerAngles::XEulerAngles(const XReal x,const XReal y,const XReal z,XRotationOrder ro)
  : m_xRotation(x), m_yRotation(y), m_zRotation(z), m_RotationOrder(ro) 
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal& XEulerAngles::RotX()
{
  return m_xRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XEulerAngles::RotX() const 
{
  return m_xRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal& XEulerAngles::RotY()
{
  return m_yRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XEulerAngles::RotY() const 
{
  return m_yRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal& XEulerAngles::RotZ() 
{
  return m_zRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XEulerAngles::RotZ() const 
{
  return m_zRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XEulerAngles::SetX(const XReal x_) 
{
  m_xRotation = x_;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XEulerAngles::SetY(const XReal y_) 
{
  m_yRotation = y_;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XEulerAngles::SetZ(const XReal z_) 
{
  m_zRotation = z_;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XEulerAngles::GetX() const 
{
  return m_xRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XEulerAngles::GetY() const 
{
  return m_yRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XEulerAngles::GetZ() const 
{
  return m_zRotation;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XEulerAngles::SetOrder(const XRotationOrder order) 
{
  m_RotationOrder = order;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XRotationOrder XEulerAngles::GetOrder() const 
{
  return m_RotationOrder;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XEulerAngles::fromQuaternion(const XQuaternion& q,const XRotationOrder order)
{
  XMatrix m(q);
  fromMatrix(m,order);
}
}
#endif