//----------------------------------------------------------------------------------------------------------------------
/// \file   DirectionalLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XDirectionalLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ExtendedLight.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XDirectionalLight
/// \brief A direction light (eg sun)
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XDirectionalLight
  : public XExtendedLight
{
  XMD_CHUNK(XDirectionalLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kShadowAngle = XExtendedLight::kLast+1; // XReal
  static const XU32 kLast        = kShadowAngle;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Directional Light Params
  /// \brief  Sets/Gets the params for the XDirectionalLight
  //@{

  /// \brief  returns the shadow angle of the directional light
  /// \return the shadow angle (in radians)
  XReal GetShadowAngle() const;

  /// \brief  sets the shadow angle on the directional light
  /// \param  v - the new angle value (in radians)
  void SetShadowAngle(const XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the angle of the directional shadow
  XReal m_ShadowAngle;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::DirectionalLight, XDirectionalLight, "directionalLight", "DIRECTIONAL_LIGHT");
#endif
}
