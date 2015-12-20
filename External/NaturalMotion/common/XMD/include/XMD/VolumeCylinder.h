//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeCylinder.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a volume cylinder primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/VolumeObject.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVolumeCylinder
/// \brief a volume cylinder shape
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeCylinder :
  public XVolumeObject
{
  XMD_CHUNK(XVolumeCylinder);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kRadius = XVolumeObject::kLast+1; // XReal
  static const XU32 kHeight = XVolumeObject::kLast+2; // XReal
  static const XU32 kLast   = kHeight;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  Volume Cylinder Params
  /// \brief Sets/Gets the params for the XVolumeCylinder
  //@{

  /// \brief  Returns the cylinder radius
  /// \return the radius of the cylinder
  XReal GetRadius() const;

  /// \brief  returns the height of the cylinder
  /// \return the cap distance
  XReal GetHeight() const;

  /// \brief  sets the cylinder radius
  /// \param  radius - the radius of the cylinder
  void SetRadius(XReal radius);

  /// \brief  sets the height of the cylinder
  /// \param  height - the height of the cylinder
  void SetHeight(XReal height);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_Radius;
  XReal m_Height;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::VolumeCylinder, XVolumeCylinder, "volumeCylinder", "VOLUME_CYLINDER");
#endif
}
