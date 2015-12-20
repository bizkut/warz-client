//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeAxis.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a volume axis dynamics field
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVolumeAxis
/// \brief Defines a volume axis field  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeAxis :
  public XField
{
  XMD_CHUNK(XVolumeAxis);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDirection           = XField::kLast+1; // XVector3
  static const XU32 kDirectionalSpeed    = XField::kLast+2; // XReal
  static const XU32 kInvertAttenuation   = XField::kLast+3; // bool
  static const XU32 kSpeedAlongAxis      = XField::kLast+4; // XReal
  static const XU32 kSpeedAroundAxis     = XField::kLast+5; // XReal
  static const XU32 kSpeedAwayFromAxis   = XField::kLast+6; // XReal
  static const XU32 kSpeedAwayFromCenter = XField::kLast+7; // XReal
  static const XU32 kTurbulenceFrequency = XField::kLast+8; // XVector3
  static const XU32 kTurbulenceOffset    = XField::kLast+9; // XVector3
  static const XU32 kTurbulence          = XField::kLast+10;// XReal
  static const XU32 kTurbulenceSpeed     = XField::kLast+11;// XReal
  static const XU32 kDetailTurbulence    = XField::kLast+12;// XReal
  static const XU32 kLast                = kDetailTurbulence;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Volume Axis Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XVolumeAxis dynamics field
  //@{

  /// \brief  returns the direction in which the axis is pointing
  const XVector3& GetDirection() const;
  
  /// \brief  returns the directional speed 
  /// \return directional speed
  XReal GetDirectionalSpeed() const;

  /// \brief  returns true if the attenuation is inverted. 
  bool GetInvertAttenuation() const;

  /// \brief  returns the speed along the volume axis
  /// \return speed along axis
  XReal GetSpeedAlongAxis() const;

  /// \brief  returns the speed along the volume axis
  /// \return speed along axis
  XReal GetSpeedAroundAxis() const;

  /// \brief  returns the speed away from the volume axis
  /// \return speed away from the axis
  XReal GetSpeedAwayFromAxis() const;

  /// \brief  
  /// \todo   document
  XReal GetSpeedAwayFromCenter() const;

  /// \brief  
  /// \todo   document
  const XVector3& GetTurbulenceFrequency() const;

  /// \brief  
  /// \todo   document
  const XVector3& GetTurbulenceOffset() const;

  /// \brief  
  /// \todo   document
  XReal GetTurbulence() const;

  /// \brief  
  /// \todo   document
  XReal GetTurbulenceSpeed() const;

  /// \brief  
  /// \todo   document
  XReal GetDetailTurbulence() const;

  /// \brief  returns the direction in which the axis is pointing
  /// \todo   document
  void SetDirection(const XVector3& v);
  
  /// \brief  
  /// \todo   document
  void SetDirectionalSpeed(XReal v);

  /// \brief  sets whether the attenuation is inverted. 
  /// \todo   document
  void SetInvertAttenuation(bool v);

  /// \brief  
  /// \todo   document
  void SetSpeedAlongAxis(XReal v);

  /// \brief  
  /// \todo   document
  void SetSpeedAroundAxis(XReal v);

  /// \brief  
  /// \todo   document
  void SetSpeedAwayFromAxis(XReal v);

  /// \brief  
  /// \todo   document
  void SetSpeedAwayFromCenter(XReal v);

  /// \brief  
  /// \todo   document
  void SetTurbulenceFrequency(const XVector3& v);

  /// \brief  
  /// \todo   document
  void SetTurbulenceOffset(const XVector3& v);

  /// \brief  
  /// \todo   document
  void SetTurbulence(XReal v);

  /// \brief  
  /// \todo   document
  void SetTurbulenceSpeed(XReal v);

  /// \brief  
  /// \todo   document
  void SetDetailTurbulence(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the direction
  XVector3 m_Direction;
  /// directional speed 
  XReal m_DirectionalSpeed;
  /// speed along axis
  XReal m_SpeedAlongAxis;
  /// tanjential speed around axis
  XReal m_SpeedAroundAxis;
  /// speed away from axis
  XReal m_SpeedAwayFromAxis;
  /// speed away from venter of volume
  XReal m_SpeedAwayFromCenter;
  /// Turbulence frequency vector
  XVector3 m_TurbulenceFrequency;
  /// Turbulence offset vector
  XVector3 m_TurbulenceOffset;
  /// turbulence amount
  XReal m_Turbulence;
  /// speed of the turbulence
  XReal m_TurbulenceSpeed;
  /// dunno.
  XReal m_DetailTurbulence;
  /// is the attenuation inverted?
  bool m_InvertAttenuation;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::VolumeAxis, XVolumeAxis, "volumeAxis", "VOLUME_AXIS_FIELD");
#endif
}
