//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeCone.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a volume cone primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/VolumeObject.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVolumeCone
/// \brief a volume cone shape
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeCone :
  public XVolumeObject
{
  XMD_CHUNK(XVolumeCone);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAngle = XVolumeObject::kLast+1; // XReal
  static const XU32 kCap   = XVolumeObject::kLast+2; // XReal
  static const XU32 kLast  = kCap;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Volume Cone Params
  /// \brief  Sets/Gets the params for the XVolumeCone
  //@{

  /// \brief  Returns the cone angle
  /// \return  the angle of the cone
  XReal GetAngle() const;

  /// \brief  returns the cap length of the cone
  /// \return  the cap distance
  XReal GetCap() const;

  /// \brief  sets the cone angle
  /// \param  angle - the angle of the cone
  void SetAngle(XReal angle);

  /// \brief  sets the cap length of the cone
  /// \param  cap - the cap distance
  void SetCap(XReal cap);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_Angle;
  XReal m_Cap;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::VolumeCone, XVolumeCone, "volumeCone", "VOLUME_CONE");
#endif
}
