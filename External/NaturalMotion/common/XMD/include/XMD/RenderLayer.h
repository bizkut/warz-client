//----------------------------------------------------------------------------------------------------------------------
/// \file   RenderLayer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the input/output method for RenderLayer
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/NodeCollection.h"
#include "XMD/Bone.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XRenderLayer
/// \brief Holds information about display layers within the scene.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XRenderLayer
  : public XNodeCollection
{
  XMD_CHUNK(XRenderLayer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XNodeCollection::kLast; 
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Render Layer Membership
  /// \brief  Sets/Gets the transforms that belong to this render layer
  //@{

  /// \brief  returns the array of items in the render layer
  /// \param  items - the returned array of items stored in the render layer
  void GetItems(XBoneList& items) const;

  /// \brief  sets the array of items in the render layer
  /// \param  items - the new array of items to add into the render layer
  void SetItems(const XBoneList& items);

  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::RenderLayer, XRenderLayer, "renderLayer", "RENDER_LAYER");
#endif
}
