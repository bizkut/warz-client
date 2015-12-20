//----------------------------------------------------------------------------------------------------------------------
/// \file   Specular.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the base class for all materials with specular colour in XMD
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Lambert.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSpecular
/// \brief This class defines the base class of all material types that contain specular surface parameters.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSpecular
  : public XLambert
{
  XMD_CHUNK(XSpecular);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kSpecular     = XLambert::kLast+1; // XColourConnection
  static const XU32 kShininess    = XLambert::kLast+2; // XReal
  static const XU32 kLast         = kShininess;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Specular Shader Params
  /// \brief  Sets/Gets the params for the XSpecular Material type
  //@{

  /// \brief  a function to set the specular colour
  /// \param  r - the red specular colour component
  /// \param  g - the green specular colour component
  /// \param  b - the blue specular colour component
  /// \param  a - the alpha specular colour component
  void SetSpecular(XReal r,
                   XReal g,
                   XReal b,
                   XReal a = 1.0f);

  /// \brief  a function to set the specular colour of the material
  /// \param  rgba - the new specular colour component
  void SetSpecular(const XReal rgba[4]);

  /// \brief  a function to set the specular colour of the material
  /// \param  rgba - the new specular colour component
  void SetSpecular(const XColour& rgba);

  /// \brief  a function to set the specular input shading node for the material
  /// \param  node - the new shading node input
  bool SetSpecular(const XShadingNode* node);

  /// \brief  functions to get the specular colour
  /// \param  r - the output red specular colour component
  /// \param  g - the output green specular colour component
  /// \param  b - the output blue specular colour component
  void GetSpecular(XReal& r,
                   XReal& g,
                   XReal& b) const;

  /// \brief  functions to get the specular colour
  /// \param  r - the output red specular colour component
  /// \param  g - the output green specular colour component
  /// \param  b - the output blue specular colour component
  /// \param  a - the output alpha specular colour component
  void GetSpecular(XReal& r,
                   XReal& g,
                   XReal& b,
                   XReal& a) const;

  /// \brief  a function to get the specular colour of the material
  /// \param  c - the returned specular colour 
  void GetSpecular(XColour& c) const;

  /// \brief  functions to get the specular colour of the material
  /// \param  rgba - the output specular colour component
  void GetSpecular(XReal rgba[4]) const;

  /// \brief  function to get the specular channel's input shading node
  /// \return the input shading node if any, 0 if none
  XShadingNode* GetSpecular();

  /// \brief  a function to set the shininess of the material
  /// \param  shine - the new cosine shininess value
  void SetShininess(XReal shine);

  /// \brief  a function to set the shininess of the material
  /// \return the cosine shininess value
  XReal GetShininess() const;

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  XShadingNode::XColourConnection m_Specular;
  XReal m_Shininess;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Specular, XSpecular, "specular");
#endif
}
