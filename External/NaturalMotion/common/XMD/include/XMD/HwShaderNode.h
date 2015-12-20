//----------------------------------------------------------------------------------------------------------------------
/// \file   HwShaderNode.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Hardware Shader node applied to a material.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
#include "XMD/Texture.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XBumpMap;
class XMD_EXPORT XEnvmapBaseTexture;
class XMD_EXPORT XTexture;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class is not really intended for use as it is, though
///     it will still work in a limited capacity. Since Hw shaders
///     are likely to be highly site specific, this class is unlikely
///     to be good enough for your needs. It does however simplify things
///     a little bit.
///       By default, the class will use any attributes it finds on
///     the node that it thinks are likely to be a common attribute.
///     For example, "ambient", "ambientColor" and "ambientColour"
///     would all be sensible names for an ambient colour. Therefore
///     if it finds any attributes with those names, it will just treat
///     the attribute as though it were that colour. (ie, the GetAmbientColour,
///     and SetAmbientColour() would happily work with an additional
///     attribute. This means a simple lua script in the export options
///     can output carefully worked out names, and have a fairly
///     generic interface from day one).
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XHwShaderNode
  : public XBase 
{
  XMD_CHUNK(XHwShaderNode);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast          = XBase::kLast;
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  /// used to return textures from the material
  enum TextureType
  {
    INVALID_MAP=0,
    AMBIENT_MAP,
    DIFFUSE_MAP,
    SPECULAR_MAP,
    EMISSION_MAP,
    TRANSPARENCY_MAP
  };

  /// \name   Hardware Shader Params
  /// \brief  Sets/Gets the params for the XHwShader type. These 
  ///         parameters will only exist if they were present in the 
  ///         original shader node. They will also exist as XAttributes
  ///         if you wish to query them via that mechanism instead.
  //@{

  //-------------------------------------------------------------------
  /// \brief  A function to return a texture applied to the specified channel.
  /// \param  type  -  the map type you want returned
  /// \return a pointer to the texture interface
  /// \note   sphere maps, cube maps and environment maps also get
  ///         returned via the same interface.
  ///
  //XTexPlacement2D* GetTexture(TextureType);


  /// \brief  This returns the environment map associated with this material.
  /// \return A pointer to the environment map or NULL
  XEnvmapBaseTexture* GetEnvMap();

  /// \brief  This returns the bump map associated with this material.
  /// \return A pointer to the bump map or NULL
  XBumpMap* GetBumpMap();

  /// \brief  a function to set the ambient colour
  /// \param  r - the new red colour component
  /// \param  g - the new green colour component
  /// \param  b - the new blue colour component
  /// \param  a - the new alpha colour component
  void SetAmbient(XReal r,
                  XReal g,
                  XReal b,
                  XReal a = 1.0f);

  /// \brief  a function to set the ambient colour
  /// \param  rgba  - the colour as a float array
  void SetAmbient(const XReal rgba[4]);

  /// \brief  functions to get the ambient colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  void GetAmbient(XReal& r,
                  XReal& g,
                  XReal& b);

  /// \brief  a function to get the ambient colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  /// \param  a - the returned alpha colour component
  void GetAmbient(XReal& r,
                  XReal& g,
                  XReal& b,
                  XReal& a);

  /// \brief  a function to get the ambient colour
  /// \param  rgba - the returned RGBA colour value
  void GetAmbient(XReal rgba[4]);

  /// \brief  a function to set the diffuse colour
  /// \param  r - the new red colour component
  /// \param  g - the new green colour component
  /// \param  b - the new blue colour component
  /// \param  a - the new alpha colour component
  void SetDiffuse(XReal r,
                  XReal g,
                  XReal b,
                  XReal a = 1.0f);

  /// \brief  a function to set the diffuse colour
  /// \param  rgba  - the colour as a float array
  void SetDiffuse(const XReal rgba[4]);

  /// \brief  a function to get the diffuse colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  void GetDiffuse(XReal& r,
                  XReal& g,
                  XReal& b);

  /// \brief  a function to get the diffuse colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  /// \param  a - the returned alpha colour component
  void GetDiffuse(XReal& r,
                  XReal& g,
                  XReal& b,
                  XReal& a);

  /// \brief  a function to get the diffuse colour
  /// \param  rgba - the returned RGBA colour value
  void GetDiffuse(XReal rgba[4]);

  /// \brief  a function to set the specular colour
  /// \param  r - the new red colour component
  /// \param  g - the new green colour component
  /// \param  b - the new blue colour component
  /// \param  a - the new alpha colour component
  void SetSpecular(XReal r,
                   XReal g,
                   XReal b,
                   XReal a = 1.0f);

  /// \brief  a function to set the specular colour
  /// \param  rgba  - the colour as a float array
  void SetSpecular(const XReal rgba[4]);

  /// \brief  a function to get the specular colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  void GetSpecular(XReal& r,
                   XReal& g,
                   XReal& b);

  /// \brief  a function to get the specular colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  /// \param  a - the returned alpha colour component
  void GetSpecular(XReal& r,
                   XReal& g,
                   XReal& b,
                   XReal& a);

  /// \brief  a function to get the specular colour
  /// \param  rgba - the returned RGBA colour value
  void GetSpecular(XReal rgba[4]);

  /// \brief  a function to set the emission colour
  /// \param  r - the new red colour component
  /// \param  g - the new green colour component
  /// \param  b - the new blue colour component
  /// \param  a - the new alpha colour component
  void SetEmission(XReal r,
                   XReal g,
                   XReal b,
                   XReal a = 1.0f);

  /// \brief  a function to set the emission colour
  /// \param  rgba  - the colour as a float array
  void SetEmission(const XReal rgba[4]);

  /// \brief  a function to get the emission colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  void GetEmission(XReal& r,
                   XReal& g,
                   XReal& b);

  /// \brief  a function to get the emission colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  /// \param  a - the returned alpha colour component
  void GetEmission(XReal& r,
                   XReal& g,
                   XReal& b,
                   XReal& a);

  /// \brief  a function to get the emission colour
  /// \param  rgba - the returned RGBA colour value
  void GetEmission(XReal rgba[4]);

  /// \brief  a function to set the transparency colour
  /// \param  r - the new red colour component
  /// \param  g - the new green colour component
  /// \param  b - the new blue colour component
  /// \param  a - the new alpha colour component
  void SetTransparency(XReal r,
                       XReal g,
                       XReal b,
                       XReal a = 1.0f);

  /// \brief  a function to set the transparency colour
  /// \param  rgba  - the colour as a float array
  void SetTransparency(const XReal rgba[4]);

  /// \brief  a function to get the transparency colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  void GetTransparency(XReal& r,
                       XReal& g,
                       XReal& b);

  /// \brief  a function to get the transparency colour
  /// \param  r - the returned red colour component
  /// \param  g - the returned green colour component
  /// \param  b - the returned blue colour component
  /// \param  a - the returned alpha colour component
  void GetTransparency(XReal& r,
                       XReal& g,
                       XReal& b,
                       XReal& a);

  /// \brief  a function to get the transparency colour
  /// \param  rgba - the returned RGBA colour value
  void GetTransparency(XReal rgba[4]);

  /// \brief  a function to set the shininess of the material
  /// \param  shine -  the shininess value
  void SetShininess(XReal shine);

  /// \brief  a function to set the shininess of the material
  /// \return the shininess value
  XReal GetShininess();

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  XAttribute* GetAmbientAttribute();
  XAttribute* GetDiffuseAttribute();
  XAttribute* GetSpecularAttribute();
  XAttribute* GetEmissionAttribute();
  XAttribute* GetTransparencyAttribute();
  XAttribute* GetShininessAttribute();
  XAttribute* GetBumpAttribute();
  XAttribute* GetEnvMapAttribute();
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::HwShader, XHwShaderNode, "hwShader", "HW_SHADER");
#endif
}
