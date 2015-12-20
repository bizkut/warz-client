//----------------------------------------------------------------------------------------------------------------------
/// \file   ExtendedLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XExtendedLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Light.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  An extended light type, adds shadow data to the basic light
///         attributes.
///
class XMD_EXPORT XExtendedLight
  : public XLight
{
  XMD_ABSTRACT_CHUNK(XExtendedLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kShadowRadius       = XLight::kLast+1; // XReal
  static const XU32 kDecayRate          = XLight::kLast+2; // XReal
  static const XU32 kDepthMapFilterSize = XLight::kLast+3; // XReal
  static const XU32 kDepthMapResolution = XLight::kLast+4; // XReal
  static const XU32 kDepthMapBias       = XLight::kLast+5; // XReal
  static const XU32 kUseDepthMapShadows = XLight::kLast+6; // bool
  static const XU32 kCastsSoftShadows   = XLight::kLast+7; // bool
  static const XU32 kLast               = kCastsSoftShadows;
  //--------------------------------------------------------------------------------------------------------------------


  /// \name   Extended Light Params
  /// \brief  Sets/Gets the params for the XExtendedLight (non ambient
  ///         light) base class.
  //@{

  /// \brief  returns the depth map shadows used value
  /// \return the depth map shadows used value
  bool GetUseDepthMapShadows() const;

  /// \brief  returns the flag to indicate if it casts soft shadows value
  /// \return the soft shadows enabled flag value 
  bool GetCastSoftShadows() const;

  /// \brief  returns the shadow radius value
  /// \return the shadow radius value
  XReal GetShadowRadius() const;

  /// \brief  returns the decay rate value
  /// \return the decay rate value
  XReal GetDecayRate() const;

  /// \brief  returns the depth map filter size
  /// \return the depth map filter size value
  XReal GetDepthMapFilterSize() const;

  /// \brief  returns the depth map resolution value
  /// \return the depth map resolution value
  XReal GetDepthMapResolution() const;

  /// \brief  returns the depth map bias value
  /// \return the depth map bias value
  XReal GetDepthMapBias() const;

  /// \brief  sets the depth map shadows used value
  /// \param  v - the depth map shadows used value
  void SetUseDepthMapShadows(bool v);

  /// \brief  sets the flag to indicate if it casts soft shadows value
  /// \param  v - the soft shadows enabled flag value
  void SetCastSoftShadows(bool v);

  /// \brief  sets the shadow radius value
  /// \param  v - the shadow radius value
  void SetShadowRadius(XReal v);

  /// \brief  sets the decay rate value
  /// \param  v - the decay rate value
  void SetDecayRate(XReal v);

  /// \brief  sets the depth map filter size
  /// \param  v - the depth map filter size value
  void SetDepthMapFilterSize(XReal v);

  /// \brief  sets the depth map resolution value
  /// \param  v - the depth map resolution value
  void SetDepthMapResolution(XReal v);

  /// \brief  sets the depth map bias value
  /// \param  v - the depth map bias value
  void SetDepthMapBias(XReal v);

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  bool m_bUseDepthMapShadows;
  bool m_bCastSoftShadows;
  XReal m_ShadowRadius;
  XReal m_DecayRate;
  XReal m_DepthMapFilterSize;
  XReal m_DepthMapResolution;
  XReal m_DepthMapBias;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::ExtendedLight, XExtendedLight, "extendedLight");
#endif
}
