//----------------------------------------------------------------------------------------------------------------------
/// \file   DisplayLayer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the a class to handle display layers
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
/// \class XMD::XDisplayLayer
/// \brief Holds information about display layers within the scene.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XDisplayLayer
  : public XNodeCollection
{
  XMD_CHUNK(XDisplayLayer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XNodeCollection::kLast; 
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Display Layer Membership
  /// \brief  Sets/Gets the transforms that belong to this display layer
  //@{

  /// \brief  returns the array of items in the display layer
  /// \param  items - the returned array of items stored in the display layer
  void GetItems(XBoneList& items) const;

  /// \brief  sets the array of items in the display layer
  /// \param  items - the new array of items to add into the display layer
  void SetItems(const XBoneList& items);

  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::DisplayLayer, XDisplayLayer, "displayLayer", "DISPLAY_LAYER");
#endif
}
