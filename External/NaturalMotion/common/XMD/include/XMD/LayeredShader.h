//----------------------------------------------------------------------------------------------------------------------
/// \file   LayeredShader.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a layered surface shader
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
///         (C) Copyright 2008 NaturalMotion Ltd
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef XMDAPI__LayeredShader__H__
#define XMDAPI__LayeredShader__H__
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/SurfaceShader.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XLayeredShader
/// \brief Defines a layered shader node that can combine multiple passes
///        into a single shader applied to a surface. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XLayeredShader :
  public XSurfaceShader
{
  XMD_CHUNK(XLayeredShader);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kNumLayers          = XMaterial::kLast+1; // XU32
  static const XU32 kGlowLayers         = XMaterial::kLast+2; // XList
  static const XU32 kTransparencyLayers = XMaterial::kLast+3; // XList
  static const XU32 kColourLayers       = XMaterial::kLast+4; // XList
  static const XU32 kIsTexture          = XMaterial::kLast+5; // bool
  static const XU32 kLast               = kIsTexture;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Layered Shader Params
  /// \brief  Sets/Gets the params for the XLayeredShader Material type
  //@{

  /// \brief  sets the number of layers available within the layered shader.
  /// \param  num_layers  - the number of layers in the shader
  void SetNumLayers(const XU32 num_layers);

  /// \brief  returns the number of layers within the shader
  /// \return the total number of layers in the shader
  XU32 GetNumLayers() const;

  /// \brief  adds a new layer to the layered shader.
  /// \param  colour  - the new colour layer
  /// \param  transparency  - the new transparency layer
  /// \param  glow - the new glow layer
  void AddLayer(const XColour& colour,
                const XColour& transparency,
                const XColour& glow);

  /// \brief  adds a new layer to the layered shader.
  /// \param  colour  - the new colour layer
  /// \param  transparency  - the new transparency layer
  /// \param  glow - the new glow layer
  /// \note   you can specify one or more of the layer materials. i.e. colour only, or colour & transparency etc.... 
  void AddLayer(const XShadingNode* colour = 0,
                const XShadingNode* transparency = 0,
                const XShadingNode* glow = 0);

  /// \brief  removes an existing layer from the layered shader.
  /// \param  layer_index - the layer id
  /// \return true if the layer_index was valid and the layer was removed
  bool RemoveLayer(const XU32 layer_index);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  colour  - the new colour layer
  /// \param  transparency  - the new transparency layer
  /// \param  glow - the new glow layer
  /// \return true if layer set
  /// \note   you can specify one or more of the layer materials. i.e. colour only, or colour & transparency etc.... 
  bool SetLayer(const XU32 layer_index,
                const XColour& colour,
                const XColour& transparency,
                const XColour& glow);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  colour  - the new colour layer
  /// \param  transparency  - the new transparency layer
  /// \param  glow - the new glow layer
  /// \return true if layer set
  /// \note   you can specify one or more of the layer materials. i.e. colour only, or colour & transparency etc.... 
  bool SetLayer(const XU32 layer_index,
                const XShadingNode* colour = 0,
                const XShadingNode* transparency = 0,
                const XShadingNode* glow = 0);

  /// \brief  returns the colour of the specified layer
  /// \param  layer_index - the layer id
  /// \param  transparency - the returned colour
  /// \return true if layer_index is valid
  /// \note   the colour returned ignores any input shading node
  bool GetLayerColour(XU32 layer_index, XColour& colour) const;

  /// \brief  returns the material used to define the colour of the 
  ///         specified layer
  /// \param  layer_index - the layer id
  /// \return the colour material
  XShadingNode* GetLayerColour(XU32 layer_index) const;

  /// \brief  returns the transparency colour of the specified layer
  /// \param  layer_index - the layer id
  /// \param  transparency - the returned transparency colour
  /// \return true if layer_index is valid
  /// \note   the colour returned ignores any input shading node
  bool GetLayerTransparency(XU32 layer_index, XColour& transparency) const;

  /// \brief  returns the material used to define the transparency of the 
  ///         specified layer
  /// \param  layer_index - the layer id
  /// \return the transparency material
  XShadingNode* GetLayerTransparency(XU32 layer_index) const;

  /// \brief  returns the glow colour of the specified layer
  /// \param  layer_index - the layer id
  /// \param  glow - the returned glow colour
  /// \return true if layer_index is valid
  /// \note   the colour returned ignores any input shading node
  bool GetLayerGlow(XU32 layer_index, XColour& glow) const;

  /// \brief  returns the material used to define the glow of the specified layer
  /// \param  layer_index - the layer id
  /// \return the glow material
  XShadingNode* GetLayerGlow(XU32 layer_index) const;

  /// \brief  returns true if this shader is used as a material
  /// \return true if this represents a shader node
  bool IsShader() const;

  /// \brief  returns true if this shader is used as a texture
  /// \return true if this shader is used to create a texture
  bool IsTexture() const;

  /// \brief  sets whether this layered shader is used to represent a texture or a shader node. 
  /// \param  flag - 0 == layered shader. 1 == layered texture
  void SetType(XU32 flag);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XShadingNode::XConnectionArray m_ColourInputs;
  XShadingNode::XConnectionArray m_TransparencyInputs;
  XShadingNode::XConnectionArray m_GlowInputs;
  XU32 m_CompositingType;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::LayeredShader, XLayeredShader, "layeredShader", "LAYERED_SHADER");
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#endif // XMDAPI__LayeredShader__H__
//----------------------------------------------------------------------------------------------------------------------
