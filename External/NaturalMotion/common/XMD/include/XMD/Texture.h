//----------------------------------------------------------------------------------------------------------------------
/// \file   Texture.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines all of the texture related data types
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ShadingNode.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XModel;
class XMD_EXPORT XTexPlacement2D;
class XMD_EXPORT XTexPlacement3D;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Well, i've caved in and added more data into the texture class to
///     allow easier re-referencing back to original files later.
///     Anyhow, this class is purely designed to hold a 2D texture. In
///     general, the XImages in the background are referenced by 3 things
///
///       XTextures
///       XEnv
///
class XMD_EXPORT XTexture
  : public XShadingNode
{
  XMD_CHUNK(XTexture);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XBase::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Common Texture Properties
  /// \brief  Sets/Gets which placement node to use for this texture
  //@{

  /// \brief  This function will return the 2D texture placement node for the texture.
  XTexPlacement2D* GetPlacement(XU32 idx) const;

  /// \brief  This function will set the texture placement node to use
  ///         for the specified texture index
  /// \param  base  - the base texture class
  /// \param  idx   - the index of the image
  /// \return true if OK
  bool SetPlacement(const XBase* base, XU32 idx);

  /// \brief  This function returns the number of texture placements
  ///         attached to this texture.
  XU32 GetNumPlacements() const;

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  XIndexList m_PlacementIDs;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Texture, XTexture, "texture", "TEXTURE");
#endif
/// \brief  An array of textures
typedef XM2::pod_vector<XTexture*> XTextureList;
}
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/TexPlacement2D.h"
//----------------------------------------------------------------------------------------------------------------------
