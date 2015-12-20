//----------------------------------------------------------------------------------------------------------------------
/// \file   XLayeredTexture.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a layered texture
/// \note     (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
///           (C) Copyright 2008 NaturalMotion Ltd
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef XMDAPI__LayeredTexture__H__
#define XMDAPI__LayeredTexture__H__
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Texture.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XLayeredTexture 
/// \brief Defines a layered texture node that can handle a number of
///        images layered over each other.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XLayeredTexture :
  public XTexture
{
  XMD_CHUNK(XLayeredTexture);
public:

  /// \brief  enum describing the way in which the textures are blended 
  ///         together.
  enum XBlendMode
  {
    kBlendNone,
    kBlendOver,
    kBlendIn,
    kBlendOut,
    kBlendAdd,
    kBlendSubtract,
    kBlendMultiply,
    kBlendDifference,
    kBlendLighten,
    kBlendDarken,
    kBlendSaturate,
    kBlendDeSaturate,
    kBlendIlluminate
  };
  typedef XM2::pod_vector<XBlendMode> XBlendModeArray;

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XTexture::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Layered Texture Blending Params
  /// \brief  Sets/Gets the method used to blend the textures together
  //@{

  /// \brief  returns the blending mode that describes how the textures
  ///         are blended together to form the output
  /// \return the blending mode
  /// \param  layer_index - the index of the layer to set the blend mode for
  XBlendMode GetBlendMode(XU32 layer_index) const;

  /// \brief  sets the blending mode to describe how the textures will be 
  ///         blended together.
  /// \param  mode - the blending mode
  /// \param  layer_index - the index of the layer to set the blend mode for
  bool SetBlendMode(XU32 layer_index, XBlendMode mode);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Layered Texture Params
  /// \brief  Sets/Gets the params for the XLayeredTexture texture type
  //@{

  /// \brief  sets the number of layers available within the layered shader.
  /// \param  num_layers  - the number of layers in the shader
  void SetNumLayers(XU32 num_layers);

  /// \brief  returns the number of layers within the shader
  /// \return the total number of layers in the shader
  XU32 GetNumLayers() const;

  /// \brief  adds a new layer to the layered shader.
  /// \param  texture - the texture to assign to the new texture layer
  bool AddLayer(const XTexture* texture);

  /// \brief  adds a new layer to the layered shader.
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  bool AddLayer(const XTexture* texture, const XColour& colour);

  /// \brief  adds a new layer to the layered shader.
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool AddLayer(const XTexture* texture, const XColour& colour, XBlendMode mode);

  /// \brief  adds a new layer to the layered shader.
  /// \param  texture - the texture to assign to the new texture layer
  bool AddLayer(const XTexPlacement2D* texture);

  /// \brief  adds a new layer to the layered shader.
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  bool AddLayer(const XTexPlacement2D* texture, const XColour& colour);

  /// \brief  adds a new layer to the layered shader.
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  ///
  bool AddLayer(const XTexPlacement2D* texture, const XColour& colour, XBlendMode mode);

  /// \brief  removes an existing layer from the layered shader.
  /// \param  layer_index - the layer id
  /// \return true if the layer_index was valid and the layer was removed
  bool RemoveLayer(XU32 layer_index);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool SetLayer(XU32 layer_index,
                const XTexture* texture);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool SetLayer(XU32 layer_index,
                const XTexture* texture,
                const XColour& colour);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool SetLayer(XU32 layer_index,
                const XTexture* texture,
                const XColour& colour,
                const XBlendMode mode);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool SetLayer(XU32 layer_index,
                const XTexPlacement2D* texture);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool SetLayer(XU32 layer_index,
                const XTexPlacement2D* texture,
                const XColour& colour);

  /// \brief  sets the materials to use on the specified layer.
  /// \param  layer_index - the layer id
  /// \param  texture - the texture to assign to the new texture layer
  /// \param  colour  - the default colour of the new layer
  /// \param  mode - the blend mode for the new texture layer
  bool SetLayer(XU32 layer_index,
                const XTexPlacement2D* texture,
                const XColour& colour,
                const XBlendMode mode);

  /// \brief  returns the colour of the specified layer index
  /// \param  layer_index - the index of the layer to query
  /// \return the default colour of the layer. 
  XColour GetLayerColour(XU32 layer_index) const;

  /// \brief  returns the shading node attached to the specified layer index.
  /// \param  layer_index - the index of the layer to query
  /// \return the default colour of the layer. 
  XShadingNode* GetLayer(XU32 layer_index) const;
  
  /// \brief  returns the texture used for the specified layer index
  /// \param  layer_index - the index of the layer to query
  /// \return the texture used by the layer
  XTexture* GetLayerTexture(XU32 layer_index) const;

  /// \brief  returns the texture used for the specified layer index
  /// \param  layer_index - the index of the layer to query
  /// \return the texture used by the layer
  XTexPlacement2D* GetLayerTexturePlacement(XU32 layer_index) const;

  //@}

// internal io mechanisms
protected:
#ifndef DOXYGEN_PROCESSING
  XBlendModeArray m_BlendModes;
  XShadingNode::XConnectionArray m_TextureInputs;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::LayeredTexture, XLayeredTexture, "layeredTexture", "LAYERED_TEXTURE");
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#endif // XMDAPI__LayeredTexture__H__
//----------------------------------------------------------------------------------------------------------------------
