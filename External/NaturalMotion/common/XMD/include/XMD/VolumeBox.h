//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeBox.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a volume box, i use 'em for collision primitives...
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/VolumeObject.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVolumeBox
/// \brief a volume box shape
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeBox :
  public XVolumeObject
{
  XMD_CHUNK(XVolumeBox);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kSizeX = XVolumeObject::kLast+1; // XReal
  static const XU32 kSizeY = XVolumeObject::kLast+2; // XReal
  static const XU32 kSizeZ = XVolumeObject::kLast+3; // XReal
  static const XU32 kLast  = kSizeZ;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Volume Box Params
  /// \brief  Sets/Gets the params for the XVolumeBox
  //@{

  /// \brief  returns the size of the x size of the box
  /// \return the x size
  XReal GetX() const;

  /// \brief  returns the size of the y size of the box
  /// \return the y size
  XReal GetY() const;

  /// \brief  returns the size of the z size of the box
  /// \return the z size
  XReal GetZ() const;

  /// \brief  sets the x size of the box
  /// \param  v - the new value
  void SetX(XReal v);

  /// \brief  sets the y size of the box
  /// \param  v - the new value
  void SetY(XReal v);

  /// \brief  sets the z size of the box
  /// \param  v - the new value
  void SetZ(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal x;
  XReal y;
  XReal z;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::VolumeBox, XVolumeBox, "volumeBox", "VOLUME_BOX");
#endif
}
