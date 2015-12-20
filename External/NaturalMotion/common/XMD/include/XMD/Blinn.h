//----------------------------------------------------------------------------------------------------------------------
/// \file   Blinn.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the interface for a Blinn surface shader. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "XMD/Specular.h"
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBlinn
/// \brief Defines a Blinn surface material
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBlinn :
  public XSpecular
{
  XMD_CHUNK(XBlinn);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kEccentricity    = XSpecular::kLast+1; // XReal
  static const XU32 kSpecularRollOff = XSpecular::kLast+2; // XReal
  static const XU32 kLast            = kSpecularRollOff;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Blinn Shader Params
  /// \brief  Sets/Gets the params for the XBlinn Material type
  //@{

  /// \brief  returns the eccentricity of the material
  /// \return  the materials specular eccentricity
  XReal GetEccentricity() const;

  /// \brief  returns the specular roll off of the material
  /// \return  the materials specular roll off
  XReal GetSpecularRollOff() const;

  /// \brief  sets the eccentricity of the material
  /// \param  ecc - the materials new specular eccentricity
  void SetEccentricity(XReal ecc);

  /// \brief  returns the eccentricity of the material
  /// \param  rolloff - the materials new specular roll off
  void SetSpecularRollOff(XReal rolloff);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_Eccentricity;
  XReal m_ShininessRollOff;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Blinn, XBlinn, "blinn", "BLINN");
#endif
}
