//----------------------------------------------------------------------------------------------------------------------
/// \file   UseBackgroundShader.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a shader that is akin to the 'predator' effect, where the
///         object uses the background colour, but may have a specular highlight 
///         contained over the top. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Material.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief defines a shader that is akin to the 'predator' effect, where 
///        the object uses the background colour, but may have a specular  
///        highlight contained over the top. 
class XMD_EXPORT XUseBackground :
  public XMaterial
{
  XMD_CHUNK(XUseBackground);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast      = XMaterial::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Use Background Shader Params
  /// \brief  Sets/Gets the params for the XUseBackground Material type
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::UseBackgroundShader, XUseBackground, "useBackground", "USE_BACKGROUND");
#endif
}
