//----------------------------------------------------------------------------------------------------------------------
/// \file   TexPlacement3D.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  3D textures (cube maps, sphere maps and 3D procedural textures) 
///         have an associated 3D texture placement that positions them in the 
///         world with respect to the objects they texture. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Bone.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTexPlacement3D
/// \brief a 3D texture placement 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XTexPlacement3D
  : public XBone
{
  XMD_CHUNK(XTexPlacement3D);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XBone::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   3D Texture Placement Params
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::TexturePlacement3D, XTexPlacement3D, "texturePlacement3D", "TEXTURE_PLACEMENT_3D");
#endif
}
