//----------------------------------------------------------------------------------------------------------------------
/// \file   Light.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XLight whixh is used to hold information about a light.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef XMDAPI__Light__H__
#define XMDAPI__Light__H__
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Shape.h"
#include "XM2/Colour.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XLight
/// \brief a class to store the base (common) light properties.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XLight
  : public XShape
{
  XMD_ABSTRACT_CHUNK(XLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kIntensity    = XShape::kLast+1; // XReal
  static const XU32 kColour       = XShape::kLast+2; // XColour
  static const XU32 kShadowColour = XShape::kLast+3; // XColour
  static const XU32 kEmitAmbient  = XShape::kLast+4; // bool
  static const XU32 kEmitDiffuse  = XShape::kLast+5; // bool
  static const XU32 kEmitSpecular = XShape::kLast+6; // bool
  static const XU32 kLast         = kEmitSpecular;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Common Light Params
  /// \brief  Sets/Gets the params for the XLight base class
  //@{

  /// \brief  returns the lights intensity value
  /// \return the light intensity
  XReal GetIntensity() const;

  /// \brief  returns the colour of the light
  /// \param  value - the returned light colour
  void GetColour(XColour& value) const;

  /// \brief  returns the shadow colour of the light
  /// \param  value - the returned shadow colour
  void GetShadowColour(XColour& value) const;

  /// \brief  returns the colour of the light
  /// \return the returned light colour
  XColour GetColour() const;

  /// \brief  returns the shadow colour of the light
  /// \return the returned shadow colour
  XColour GetShadowColour() const;

  /// \brief  sets the light intensity
  /// \param  value - the new intensity value
  void SetIntensity(XReal value);

  /// \brief  sets the colour of the light
  /// \param  value - the new colour
  void SetColour(const XColour& value);

  /// \brief  sets the shadow colour of the light
  /// \param  value - the new shadow colour
  void SetShadowColour(const XColour& value);

  /// \brief  returns true if the light emits ambient light
  /// \return true or false
  bool GetEmitAmbient() const;

  /// \brief  returns true if the light emits diffuse light
  /// \return true or false
  bool GetEmitDiffuse() const;

  /// \brief  returns true if the light emits specular light
  /// \return true or false
  bool GetEmitSpecular() const;

  /// \brief  sets a flag for the light to emit ambient
  /// \param  value - true or false
  void SetEmitAmbient(bool value);

  /// \brief  sets a flag for the light to emit ambient
  /// \param  value - true or false
  void SetEmitDiffuse(bool value);

  /// \brief  sets a flag for the light to emit specular
  /// \param  value - true or false
  void SetEmitSpecular(bool value);

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  /// the intensity of the light
  XReal m_fIntensity;
  /// the lights colour
  XReal m_Color[4];
  /// the shadow colour
  XReal m_ShadowColor[4];
  /// true if this light emits ambient light
  bool m_bEmitAmbient;
  /// true if this light emits diffuse light
  bool m_bEmitDiffuse;
  /// true if this light emits specular light
  bool m_bEmitSpecular;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Light,XLight,"light");
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#endif // XMDAPI__Light__H__
//----------------------------------------------------------------------------------------------------------------------
