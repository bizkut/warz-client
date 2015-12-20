//----------------------------------------------------------------------------------------------------------------------
/// \file   Lambert.h
/// \author Rob Bateman [http://robthebloke.org]
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Material.h"
#include "XMD/BumpMap.h"
#include "XMD/CubeMap.h"
#include "XMD/SphereMap.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XLambert
/// \brief Defines a lambert surface material type
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XLambert :
  public XMaterial
{
  XMD_CHUNK(XLambert);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAmbient      = XMaterial::kLast+1; // XColourConnection
  static const XU32 kEmission     = XMaterial::kLast+2; // XColourConnection
  static const XU32 kTransparency = XMaterial::kLast+3; // XColourConnection
  static const XU32 kBumpMap      = XMaterial::kLast+4; // XColourConnection
  static const XU32 kEnvMap       = XMaterial::kLast+5; // XColourConnection
  static const XU32 kLast         = kEnvMap;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  a function to set the ambient colour of the material
  /// \param  r - the red ambient colour component
  /// \param  g - the green ambient colour component
  /// \param  b - the blue ambient colour component
  /// \param  a - the alpha ambient colour component
  void SetAmbient(XReal r,
                  XReal g,
                  XReal b,
                  XReal a = 1.0f);

  /// \brief  a function to set the ambient colour of the material
  /// \param  rgba - the new ambient colour component
  void SetAmbient(const XReal rgba[4]);

  /// \brief  a function to set the ambient colour of the material
  /// \param  rgba - the new ambient colour component
  void SetAmbient(const XColour& rgba);

  /// \brief  a function to set the ambient input shading node for the material
  /// \param  node - the new shading node input
  bool SetAmbient(const XShadingNode* node);

  /// \brief  a function to get the ambient colour of the material
  /// \param  r - the output red ambient colour component
  /// \param  g - the output green ambient colour component
  /// \param  b - the output blue ambient colour component
  void GetAmbient(XReal& r,
                  XReal& g,
                  XReal& b) const;

  /// \brief  a function to get the ambient colour of the material
  /// \param  r - the output red ambient colour component
  /// \param  g - the output green ambient colour component
  /// \param  b - the output blue ambient colour component
  /// \param  a - the output alpha ambient colour component
  void GetAmbient(XReal& r,
                  XReal& g,
                  XReal& b,
                  XReal& a) const;

  /// \brief  a function to get the ambient colour of the material
  /// \param  c - the returned ambient colour 
  void GetAmbient(XColour& c) const;

  /// \brief  a function to get the ambient colour of the material
  /// \param  rgba - the returned ambient colour 
  void GetAmbient(XReal rgba[4]) const;

  /// \brief  function to get the ambient channel's input shading node
  /// \return the input shading node if any, 0 if none
  XShadingNode* GetAmbient();

  /// \brief  a function to set the emission colour
  /// \param  r - the red emission colour component
  /// \param  g - the green emission colour component
  /// \param  b - the blue emission colour component
  /// \param  a - the alpha emission colour component
  void SetEmission(XReal r,
                   XReal g,
                   XReal b,
                   XReal a = 1.0f);

  /// \brief  a function to set the emission colour
  /// \param  rgba - the new emission colour component
  void SetEmission(const XReal rgba[4]);

  /// \brief  a function to set the emission colour
  /// \param  rgba - the new emission colour component
  void SetEmission(const XColour& rgba);

  /// \brief  a function to set the emission input shading node for the material
  /// \param  node - the new shading node input
  bool SetEmission(const XShadingNode* node);

  /// \brief  functions to get the emission colour
  /// \param  r - the output red emission colour component
  /// \param  g - the output green emission colour component
  /// \param  b - the output blue emission colour component
  void GetEmission(XReal& r,
                   XReal& g,
                   XReal& b) const;

  /// \brief  functions to get the emission colour
  /// \param  r - the output red emission colour component
  /// \param  g - the output green emission colour component
  /// \param  b - the output blue emission colour component
  /// \param  a - the output alpha emission colour component
  void GetEmission(XReal& r,
                   XReal& g,
                   XReal& b,
                   XReal& a) const;

  /// \brief  functions to get the emission colour
  /// \param  rgba - the output emission colour component
  void GetEmission(XReal rgba[4]) const;

  /// \brief  a function to get the emission colour of the material
  /// \param  c - the returned emission colour 
  void GetEmission(XColour& c) const;

  /// \brief  function to get the emission channel's input shading node
  /// \return the input shading node if any, 0 if none
  XShadingNode* GetEmission();

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

  /// \brief  creates a bump map on the material
  /// \return the new bump map
  XBumpMap* CreateBumpMap();

  /// \brief  assigns a bump map to the material
  /// \param  bump_map - the bump map to assign
  /// \return true if OK
  bool SetBumpMap(const XBumpMap* bump_map);

  /// \brief  This returns the bump map associated with this material.
  /// \return A pointer to the bump map or NULL
  XBumpMap* GetBumpMap() const;

  /// \brief  This function creates a cube map on the material
  /// \return A pointer to the environment map
  XCubeMap* CreateCubeMap();

  /// \brief  This function creates a sphere map on the material
  /// \return A pointer to the environment map 
  XSphereMap* CreateSphereMap();

  /// \brief  assigns an environment map to the material
  /// \param  env_map - the cube or sphere map
  /// \return true if OK
  bool SetEnvMap(const XEnvmapBaseTexture* env_map);

  /// \brief  This returns the environment map associated with this material.
  /// \return  A pointer to the environment map or NULL
  XEnvmapBaseTexture* GetEnvMap() const;

protected:
#ifndef DOXYGEN_PROCESSING
  XShadingNode::XColourConnection m_Ambient;
  XShadingNode::XColourConnection m_Emission;
  XShadingNode::XColourConnection m_Transparency;
  XShadingNode::XColourConnection m_BumpMap;
  XShadingNode::XColourConnection m_EnvMap;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Lambert, XLambert, "lambert", "LAMBERT");
#endif
}
