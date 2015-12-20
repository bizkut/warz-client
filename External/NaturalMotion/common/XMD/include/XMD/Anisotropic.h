//----------------------------------------------------------------------------------------------------------------------
/// \file   Anisotropic.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the interface for the anisotropic shader maerial.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Specular.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAnisotropic
/// \brief Defines an Anisotropic surface shader
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAnisotropic :
  public XSpecular
{
  XMD_CHUNK(XAnisotropic);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kTangentUCamera          = XSpecular::kLast+1; // XColour
  static const XU32 kTangentVCamera          = XSpecular::kLast+2; // XColour
  static const XU32 kCorrelationX            = XSpecular::kLast+3; // XReal
  static const XU32 kCorrelationY            = XSpecular::kLast+4; // XReal
  static const XU32 kRoughness               = XSpecular::kLast+5; // XReal
  static const XU32 kRotateAngle             = XSpecular::kLast+6; // XReal
  static const XU32 kRefractiveIndex         = XSpecular::kLast+7; // XReal
  static const XU32 kAnisotropicReflectivity = XSpecular::kLast+8; // bool
  static const XU32 kLast                    = kAnisotropicReflectivity;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Anisotropic Shader Params
  /// \brief  Sets/Gets the params for the XAnistropic Material type
  //@{

  /// \brief  returns the tangent U camera value
  /// \return the tangent U camera value
  const XColour& GetTangentUCamera() const;

  /// \brief  returns the tangent V camera value
  /// \return the tangent V camera value
  const XColour& GetTangentVCamera() const;

  /// \brief  returns the correlation X value
  /// \return the correlation X value
  XReal GetCorrelationX() const;

  /// \brief  returns the correlation Y value
  /// \return the correlation Y value
  XReal GetCorrelationY() const;

  /// \brief  returns the roughness value
  /// \return the roughness value
  XReal GetRoughness() const;

  /// \brief  returns the rotate angle value
  /// \return the rotate angle value
  XReal GetRotateAngle() const;

  /// \brief  returns the refraction index value
  /// \return the refraction index value
  XReal GetRefractiveIndex() const;

  /// \brief  returns the anisotropic reflectivity value
  /// \return the anisotropic reflectivity value
  bool GetAnisotropicReflectivity() const;

  /// \brief  sets the tangent U camera value
  /// \param  value - the tangent U camera value
  void SetTangentUCamera(const XColour& value);

  /// \brief  sets the tangent V camera value
  /// \param  value - the tangent V camera value
  void SetTangentVCamera(const XColour& value);

  /// \brief  sets the correlation X value
  /// \param  value - the correlation X value
  void SetCorrelationX(const XReal value);

  /// \brief  sets the correlation Y value
  /// \param  value - the correlation Y value
  void SetCorrelationY(const XReal value);

  /// \brief  sets the roughness value
  /// \param  value - the roughness value
  void SetRoughness(const XReal value);

  /// \brief  sets the rotate angle value
  /// \param  value - the rotate angle value
  void SetRotateAngle(const XReal value);

  /// \brief  sets the refraction index value
  /// \param  value - the refraction index value
  void SetRefractiveIndex(const XReal value);

  /// \brief  sets the anisotropic reflectivity value
  /// \param  value - the anisotropic reflectivity value
  void SetAnisotropicReflectivity(const bool value);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XColour m_TangentUCamera;
  XColour m_TangentVCamera;
  XReal m_CorrelationX;
  XReal m_CorrelationY;
  XReal m_Roughness;
  XReal m_RotateAngle;
  XReal m_RefractiveIndex;
  bool m_AnisotropicReflectivity;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Anisotropic, XAnisotropic, "anisotropic", "ANISOTROPIC");
#endif
}
