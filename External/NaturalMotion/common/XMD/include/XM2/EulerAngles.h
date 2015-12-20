//----------------------------------------------------------------------------------------------------------------------
/// \file   EulerAngles.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the routines to rotations represented as Euler-Angles. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
#ifndef DOXYGEN_PROCESSING
class XM2EXPORT XMatrix;
class XM2EXPORT XQuaternion;
#endif
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This enumeration defines the order of the Euler rotations
//---------------------------------------------------------------------------------------------------------------------- 
enum XRotationOrder 
{
  XROT_XYZ,
  XROT_XZY,
  XROT_YXZ,
  XROT_YZX,
  XROT_ZXY,
  XROT_ZYX,

  XROT_XYX,
  XROT_YZY,
  XROT_ZXZ
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XM2::XEulerAngles
/// \brief This class defines a Euler rotation comprising x y and z rotation values. The Euler rotations can also 
///        occur in any combination of XYZ, denoted by the XRotationOrder enumeration. 
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XEulerAngles  
{
public:

  /// \brief  ctor
  /// \param  ro - the order in which the rotations occur.
  XM2_INLINE XEulerAngles(XRotationOrder ro = XROT_XYZ);

  /// \brief  ctor
  /// \param  x - the x rotation
  /// \param  y - the y rotation 
  /// \param  z - the z rotation
  /// \param  ro - the order in which the rotations occur.
  XM2_INLINE XEulerAngles(XReal x, XReal y, XReal z, XRotationOrder ro = XROT_XYZ);

  /// \brief  returns a reference to the X rotation as a radian
  /// \return the X rotation
  XM2_INLINE XReal& RotX();

  /// \brief  returns a reference to the X rotation as a radian
  /// \return the X rotation
  XM2_INLINE XReal RotX() const;

  /// \brief  returns a reference to the Y rotation as a radian
  /// \return the Y rotation
  XM2_INLINE XReal& RotY();

  /// \brief  returns a reference to the Y rotation as a radian
  /// \return the Y rotation
  XM2_INLINE XReal RotY() const;

  /// \brief  returns a reference to the Z rotation as a radian
  /// \return the Z rotation
  XM2_INLINE XReal& RotZ();

  /// \brief  returns a reference to the Z rotation as a radian
  /// \return the Z rotation
  XM2_INLINE XReal RotZ() const;

  /// \brief  This function allows you to set the x rotation of the Euler rotation. 
  /// \param  x_ - the new x value
  XM2_INLINE void SetX(XReal x_);

  /// \brief  This function allows you to set the x rotation of the Euler rotation. 
  /// \param  y_ - the new y value
  XM2_INLINE void SetY(XReal y_);

  /// \brief  This function allows you to set the x rotation of the Euler rotation. 
  /// \param  z_ - the new z value
  XM2_INLINE void SetZ(XReal z_);

  /// \brief  this function returns the x value of the Euler rotation
  /// \return the x rotation value
  XM2_INLINE XReal GetX() const;

  /// \brief  this function returns the y value of the Euler rotation
  /// \return the y rotation value
  XM2_INLINE XReal GetY() const;

  /// \brief  this function returns the z value of the Euler rotation
  /// \return the z rotation value
  XM2_INLINE XReal GetZ() const;

  /// \brief  this function sets the order of the Euler rotations
  /// \param  order - the new rotation order
  XM2_INLINE void SetOrder(XRotationOrder order);

  /// \brief  this function returns the order of the Euler rotations
  /// \return the rotation order
  XM2_INLINE XRotationOrder GetOrder() const;

  /// \brief  this function returns the Euler rotation as a matrix
  /// \return the matrix that represents the Euler rotation
  XMatrix AsMatrix() const;

  /// \brief  this function returns the Euler rotation in quaternion
  /// \return the rotation as a quaternion
  XQuaternion AsQuaternion() const;

  /// \brief  converts the quaternion q into an Euler angle representation using the specified order
  /// \param  q - the quaternion to convert
  /// \param  order - the rotation order for the converted quaternion
  XM2_INLINE void fromQuaternion(const XQuaternion& q, XRotationOrder order);

  /// \brief  converts the quaternion q into an Euler angle representation using the specified order
  /// \param  q - the quaternion to convert
  /// \param  order - the rotation order for the converted matrix
  void fromMatrix(const XMatrix& q, XRotationOrder order);

  union
  {
    struct 
    {
      /// the x rotation of the transform
      XReal m_xRotation;
      /// the y rotation of the transform
      XReal m_yRotation;
      /// the z rotation of the transform
      XReal m_zRotation;
    };
    struct 
    {
      XReal x; XReal y; XReal z;
    };
  };

  /// the rotation order of the transform
  XRotationOrder m_RotationOrder;
};
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Matrix34.h"
#include "XM2/inl/EulerAngles.inl"
//----------------------------------------------------------------------------------------------------------------------
