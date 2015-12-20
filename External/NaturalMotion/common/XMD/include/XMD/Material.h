//----------------------------------------------------------------------------------------------------------------------
/// \file   Material.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the base class for all materials in XMD
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ShadingNode.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XInstance;
typedef XM2::pod_vector<XInstance*> XInstanceList;
#endif
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XMaterial
/// \brief This class is used to store a material. Materials are only connected to surface instances, 
///        not surfaces themselves. This allows you to use the same surface data with multiple material groups
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMaterial
  : public XShadingNode
{
  XMD_CHUNK(XMaterial);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kHardwareShader = XShadingNode::kLast+1; // XId
  static const XU32 kDiffuse        = XShadingNode::kLast+2; // XColourConnection
  static const XU32 kLast           = kDiffuse;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  This returns the hardware shader applied use for this
  ///         material. returns NULL if none applied
  /// \return  A pointer to the hw shader node or NULL
  XHwShaderNode* GetHwShader() const;

  /// \brief  assigns a HW shader node to the material. 
  /// \param  hw_shader - node of type XFn::HwShader
  /// \return true if OK
  bool SetHwShader(const XBase* hw_shader);

  /// \brief  a function to set the diffuse colour
  /// \param  r - the red diffuse colour component
  /// \param  g - the green diffuse colour component
  /// \param  b - the blue diffuse colour component
  /// \param  a - the alpha diffuse colour component
  void SetDiffuse(XReal r,
                  XReal g,
                  XReal b,
                  XReal a = 1.0f);

  /// \brief  a function to set the diffuse colour of the material
  /// \param  rgba - the new diffuse colour component
  void SetDiffuse(const XReal rgba[4]);

  /// \brief  a function to set the diffuse colour of the material
  /// \param  rgba - the new diffuse colour component
  void SetDiffuse(const XColour& rgba);

  /// \brief  a function to set the diffuse input shading node for the material
  /// \param  node - the new shading node input
  bool SetDiffuse(const XShadingNode* node);

  /// \brief  functions to get the diffuse colour
  /// \param  r - the output red diffuse colour component
  /// \param  g - the output green diffuse colour component
  /// \param  b - the output blue diffuse colour component
  void GetDiffuse(XReal& r,
                  XReal& g,
                  XReal& b) const;

  /// \brief  functions to get the diffuse colour
  /// \param  r - the output red diffuse colour component
  /// \param  g - the output green diffuse colour component
  /// \param  b - the output blue diffuse colour component
  /// \param  a - the ambient ambient colour component
  void GetDiffuse(XReal& r,
                  XReal& g,
                  XReal& b,
                  XReal& a) const;

  /// \brief  a function to get the diffuse colour of the material
  /// \param  c - the returned ambient colour 
  void GetDiffuse(XColour& c) const;

  /// \brief  functions to get the diffuse colour of the material
  /// \param  rgba - the output diffuse colour component
  void GetDiffuse(XReal rgba[4]) const;

  /// \brief  function to get the diffuse channel's input shading node
  /// \return the input shading node if any, 0 if none
  XShadingNode* GetDiffuse();

  /// \brief  function to get the diffuse channel's input shading node
  /// \return the input shading node if any, 0 if none
  const XShadingNode* GetDiffuse() const;

  /// \brief  returns a list of all object instances that use this material.
  /// \param  _list  -  the returned list of instances
  /// \return  true if any found
  bool GetObjectInstances(XInstanceList& _list) const;

protected:
#ifndef DOXYGEN_PROCESSING
  /// the corresponding hardware shader id
  XId m_HwShader;
  /// the diffuse colour
  XShadingNode::XColourConnection m_Diffuse;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Material, XMaterial, "material");
#endif

/// \brief  An array of materials
typedef XM2::pod_vector<XMaterial*> XMaterialList;
}
