//----------------------------------------------------------------------------------------------------------------------
/// \file   AmbientLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XAmbientLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Light.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAmbientLight
/// \brief A basic ambient light that illuminates everything
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAmbientLight
  : public XLight
{
  XMD_CHUNK(XAmbientLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAmbientShade     = XLight::kLast+1; // XReal
  static const XU32 kShadowRadius     = XLight::kLast+2; // XReal
  static const XU32 kCastsSoftShadows = XLight::kLast+3; // bool
  static const XU32 kLast             = kCastsSoftShadows;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Ambient Light Params
  /// \brief  Sets/Gets the params for the XAmbientLight
  //@{

  /// \brief  returns the ambient shade of the light
  /// \return the returned light ambient shade
  XReal GetAmbientShade() const;

  /// \brief  sets the lights ambient shade
  /// \param  shade - the new ambient shade value
  void SetAmbientShade(XReal shade);

  /// \brief  returns the shadow radius of the light
  /// \return the returned light shadow radius
  XReal GetShadowRadius() const;

  /// \brief  sets the lights shadow radius
  /// \param  radius - the new shadow radius value
  void SetShadowRadius(XReal radius);

  /// \brief  returns true if the light casts soft shadows (as apposed to hard)
  /// \return true for soft, false for hard
  bool GetCastSoftShadows() const;

  /// \brief  enables or disables soft shadows
  /// \param  enabled - true to enable soft shadows, false to disable
  void SetCastSoftShadows(bool enabled);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the ambient shade value
  XReal m_AmbientShade;
  /// the radius of the shadow
  XReal m_ShadowRadius;
  /// true if the shadow cast is soft
  bool m_CastSoftShadows;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::AmbientLight,XAmbientLight,"ambientLight","AMBIENT_LIGHT");
#endif
}
