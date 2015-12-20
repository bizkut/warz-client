//----------------------------------------------------------------------------------------------------------------------
/// \file   BumpMap.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the XBumpMap class to handle bump mapped objects
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Texture.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  A 2d bump map node. I'm not deriving from XFn since the bump map
///     is far too simple.
/// \note This is liable to change again soon! I'm still not happy with it :(
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBumpMap
  : public XTexture
{
  XMD_CHUNK(XBumpMap);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kBumpDepth = XTexture::kLast+1; // XReal
  static const XU32 kPlacement = XTexture::kLast+2; // XBase*
  static const XU32 kLast      = kPlacement;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Bump Map Attributes
  /// \brief  Sets/Gets the params for the bump map texture
  //@{

  /// \brief  returns the texture placement node for this bump map
  /// \return a pointer to the texture placement node
  XTexPlacement2D* GetPlacement() const;

  /// \brief  This function allows me to set a placement for the texture
  /// \param  place - the texture placement
  /// \return true if OK
  bool SetPlacement(const XBase* place);

  /// \brief  This function returns a pointer to the requested image
  /// \return a pointer to the image
  XImage* GetImage() const;

  /// \brief  This function sets the image to use for the specified image slot.
  /// \param  ptr - pointer to the image
  /// \return true if Set OK
  bool SetImage(const XImage* ptr);

  /// \brief  This function sets the image to use for the specified image slot.
  /// \param  idx - the internal image index to use
  /// \return true if Set OK
  bool SetImage(XId idx);

  /// \brief  This function returns the depth of the bump
  /// \return the depth as a float
  XReal GetBumpDepth() const;

  /// \brief  This function sets the depth of the bump map
  /// \param  bump_depth - the depth as a float
  void SetBumpDepth(XReal bump_depth);

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  XReal m_BumpDepth;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::BumpMap, XBumpMap, "bumpMap", "BUMP_MAP");
#endif
/// \brief  provided for backwards compatibility
typedef XBumpMap XBumpTexture;
}
