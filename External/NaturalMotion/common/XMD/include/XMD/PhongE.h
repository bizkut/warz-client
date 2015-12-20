//----------------------------------------------------------------------------------------------------------------------
/// \file   PhongE.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  a phongE (extended phong) material type
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Specular.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XPhongE
/// \brief Defines a phong E surface material type
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XPhongE :
  public XSpecular
{
  XMD_CHUNK(XPhongE);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kHighlightSize = XSpecular::kLast+1; // XReal
  static const XU32 kRoughness     = XSpecular::kLast+2; // XReal
  static const XU32 kLast          = kRoughness;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   PhongE Shader Params
  /// \brief  Sets/Gets the params for the XPhongE Material type
  //@{

  /// \brief  returns the size of the highlight
  /// \return the highlight size 
  XReal GetHighlightSize() const;

  /// \brief  returns the roughness amount
  /// \return The roughness of the shader
  XReal GetRoughness() const;

  /// \brief  sets the size of the highlight
  /// \param  value - the highlight size 
  void SetHighlightSize(const XReal value);

  /// \brief  sets the roughness amount
  /// \param  value - the roughness value 
  void SetRoughness(const XReal value);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_HighlightSize;
  XReal m_Roughness;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::PhongE, XPhongE, "phongE", "PHONGE");
#endif
}
