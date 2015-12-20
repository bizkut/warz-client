//----------------------------------------------------------------------------------------------------------------------
/// \file   SurfaceShader.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a constant coloured shader node that is un-affected by
///         lighting. In XSI, this is known as a constant shader. In Maya,
///         surface shader...
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Material.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSurfaceShader
/// \brief Defines a constant shader node - one that is unaffected by lighting
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSurfaceShader :
  public XMaterial
{
  XMD_CHUNK(XSurfaceShader);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kTransparency  = XMaterial::kLast+1;      // XColourConnection
  static const XU32 kLast          = kTransparency;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  a function to set the transparency colour
  /// \param  r - the red transparency colour component
  /// \param  g - the green transparency colour component
  /// \param  b - the blue transparency colour component
  /// \param  a - the alpha transparency colour component
  void SetTransparency(XReal r,
                       XReal g,
                       XReal b,
                       XReal a = 1.0f);

  /// \brief  a function to set the transparency colour
  /// \param  rgba - the new transparency colour component
  void SetTransparency(const XReal rgba[4]);

  /// \brief  a function to set the transparency colour
  /// \param  rgba - the new transparency colour component
  void SetTransparency(const XColour& rgba);

  /// \brief  a function to set the transparency input shading node for the material
  /// \param  node - the new shading node input
  bool SetTransparency(const XShadingNode* node);

  /// \brief  functions to get the transparency colour
  /// \param  r - the output red transparency colour component
  /// \param  g - the output green transparency colour component
  /// \param  b - the output blue transparency colour component
  void GetTransparency(XReal& r,
                       XReal& g,
                       XReal& b) const;

  /// \brief  functions to get the transparency colour
  /// \param  r - the output red transparency colour component
  /// \param  g - the output green transparency colour component
  /// \param  b - the output blue transparency colour component
  /// \param  a - the output alpha transparency colour component
  void GetTransparency(XReal& r,
                       XReal& g,
                       XReal& b,
                       XReal& a) const;

  /// \brief  functions to get the transparency colour
  /// \param  rgba - the output transparency colour component
  void GetTransparency(XReal rgba[4]) const;

  /// \brief  a function to get the transparency colour of the material
  /// \param  c - the returned transparency colour
  void GetTransparency(XColour& c) const;

  /// \brief  function to get the transparency channel's input shading node
  /// \return the input shading node if any, 0 if none
  XShadingNode* GetTransparency();

protected:
#ifndef DOXYGEN_PROCESSING
  XShadingNode::XColourConnection m_Transparency;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::SurfaceShader, XSurfaceShader, "surfaceShader", "SURFACE_SHADER");
#endif
}
