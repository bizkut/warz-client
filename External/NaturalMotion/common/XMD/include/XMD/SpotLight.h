//----------------------------------------------------------------------------------------------------------------------
/// \file   SpotLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XSpotLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ExtendedLight.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSpotLight
/// \brief A spot light type
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSpotLight
  : public XExtendedLight
{
  XMD_CHUNK(XSpotLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kConeAngle              = XExtendedLight::kLast+1; // XReal
  static const XU32 kPenumbraAngle          = XExtendedLight::kLast+2; // XReal
  static const XU32 kDropOff                = XExtendedLight::kLast+3; // XReal
  static const XU32 kLeftBarnDoorAngle      = XExtendedLight::kLast+4; // XReal
  static const XU32 kRightBarnDoorAngle     = XExtendedLight::kLast+5; // XReal
  static const XU32 kTopBarnDoorAngle       = XExtendedLight::kLast+6; // XReal
  static const XU32 kBottomBarnDoorAngle    = XExtendedLight::kLast+7; // XReal
  static const XU32 kHasBarnDoors           = XExtendedLight::kLast+8; // bool
  static const XU32 kUseDecayRegions        = XExtendedLight::kLast+9; // bool
  static const XU32 kFirstDecayRegionStart  = XExtendedLight::kLast+10;// XReal
  static const XU32 kFirstDecayRegionEnd    = XExtendedLight::kLast+11;// XReal
  static const XU32 kSecondDecayRegionStart = XExtendedLight::kLast+12;// XReal
  static const XU32 kSecondDecayRegionEnd   = XExtendedLight::kLast+13;// XReal
  static const XU32 kThirdDecayRegionStart  = XExtendedLight::kLast+14;// XReal
  static const XU32 kThirdDecayRegionEnd    = XExtendedLight::kLast+15;// XReal
  static const XU32 kLast                   = kThirdDecayRegionEnd;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Spot Light Params
  /// \brief  Sets/Gets the params for the XSpotLight
  //@{

  /// \brief  returns the cone angle of the spot light
  /// \return the cone angle
  XReal GetConeAngle() const;

  /// \brief  returns the Penumbra angle of the spot light
  /// \return the Penumbra angle
  XReal GetPenumbraAngle() const;

  /// \brief  returns the dropoff of the spot light
  /// \return the dropoff
  XReal GetDropOff() const;

  /// \brief  returns the true if the spot light has barn doors
  /// \return true or false for barn doors
  bool GetHasBarnDoors() const;

  /// \brief  returns the angle of the left barn door of the spot light
  /// \return the left barn door angle
  XReal GetLeftBarnDoorAngle() const;

  /// \brief  returns the angle of the right barn door of the spot light
  /// \return the right barn door angle
  XReal GetRightBarnDoorAngle() const;

  /// \brief  returns the angle of the top barn door of the spot light
  /// \return the top barn door angle
  XReal GetTopBarnDoorAngle() const;

  /// \brief  returns the angle of the bottom barn door of the spot light
  /// \return the bottom barn door angle
  XReal GetBottomBarnDoorAngle() const;

  /// \brief  returns the angle of the bottom barn door of the spot light
  /// \return the bottom barn door angle
  bool GetUseDecayRegions() const;

  /// \brief  returns the first decay region
  /// \param  start - the returned start distance for the region
  /// \param  end   - the returned end distance for the region
  void GetFirstDecayRegion(XReal& start, XReal& end) const;

  /// \brief  returns the second decay region
  /// \param  start - the returned start distance for the region
  /// \param  end   - the returned end distance for the region
  void GetSecondDecayRegion(XReal& start, XReal& end) const;

  /// \brief  returns the third decay region
  /// \param  start - the returned start distance for the region
  /// \param  end   - the returned end distance for the region
  void GetThirdDecayRegion(XReal& start, XReal& end) const;

  /// \brief  set the cone angle of the spot light
  /// \return v - the cone angle
  void SetConeAngle(XReal v);

  /// \brief  set the Penumbra angle of the spot light
  /// \return v - the Penumbra angle
  void SetPenumbraAngle(XReal v);

  /// \brief  sets the dropoff of the spot light
  /// \return v - the dropoff
  void SetDropOff(XReal v);

  /// \brief  enables/disables barn doors on the spot light
  /// \return v - true or false for barn doors
  void SetHasBarnDoors(bool v);

  /// \brief  sets the angle of the left barn door of the spot light
  /// \return the left barn door angle
  void SetLeftBarnDoorAngle(XReal v);

  /// \brief  sets the angle of the right barn door of the spot light
  /// \return the right barn door angle
  void SetRightBarnDoorAngle(XReal v);

  /// \brief  sets the angle of the top barn door of the spot light
  /// \return the top barn door angle
  void SetTopBarnDoorAngle(XReal v);

  /// \brief  sets the angle of the bottom barn door of the spot light
  /// \return the bottom barn door angle
  void SetBottomBarnDoorAngle(XReal v);

  /// \brief  enables or disables the decay regions
  /// \param  v - true or false enabled state
  void SetUseDecayRegions(bool v);

  /// \brief  sets the first decay region
  /// \param  start - the start distance for the region
  /// \param  end   - the end distance for the region
  void SetFirstDecayRegion(XReal start, XReal end);

  /// \brief  sets the second decay region
  /// \param  start - the start distance for the region
  /// \param  end   - the end distance for the region
  void SetSecondDecayRegion(XReal start, XReal end);

  /// \brief  sets the third decay region
  /// \param  start - the start distance for the region
  /// \param  end   - the end distance for the region
  void SetThirdDecayRegion(XReal start, XReal end);


  /// \brief  \b DEPRECATED returns the true if the spot light has barn doors
  /// \return true or false for barn doors
  /// \deprecated Use XSpotLight::GetHasBarnDoors() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XSpotLight::GetHasBarnDoors() instead")
  #endif
  inline bool HasBarnDoors() const { return GetHasBarnDoors(); }

  /// \brief  \b DEPRECATED returns the angle of the bottom barn door of the spot light
  /// \return the bottom barn door angle
  /// \deprecated Use XSpotLight::GetUseDecayRegions() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XSpotLight::GetUseDecayRegions() instead")
  #endif
  inline bool UseDecayRegions() const { return GetUseDecayRegions(); }

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the cone angle for the light
  XReal m_ConeAngle;
  /// the penumbra angle of the light
  XReal m_PenumbraAngle;
  /// the drop off valu
  XReal m_DropOff;
  /// true if barn doors are present
  bool  m_BarnDoors;
  /// angle of the left barn door
  XReal m_LeftBarnDoorAngle;
  /// angle of the right barn door
  XReal m_RightBarnDoorAngle;
  /// angle of the top barn door
  XReal m_TopBarnDoorAngle;
  /// angle of the bottom barn door
  XReal m_BottomBarnDoorAngle;
  /// true if decay regions are used
  bool m_UseDecayRegions;
  /// the start/end for first decay region
  XReal m_Decay1[2];
  /// the start/end for second decay region
  XReal m_Decay2[2];
  /// the start/end for third decay region
  XReal m_Decay3[2];

#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::SpotLight, XSpotLight, "spotLight", "SPOT_LIGHT");
#endif
}
