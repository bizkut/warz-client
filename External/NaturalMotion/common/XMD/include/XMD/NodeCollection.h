//----------------------------------------------------------------------------------------------------------------------
/// \file   NodeCollection.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the base class of all object sets and layers. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNodeCollection
/// \brief  Holds information about a set of nodes.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNodeCollection
  : public XBase
{
  XMD_CHUNK(XNodeCollection);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kItems = XBase::kLast+1; // XList
  static const XU32 kLast  = kItems;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Node Collection Membership
  /// \brief  Sets/Gets the transforms that belong to this display layer
  //@{

  /// \brief  returns the number of items in this display layer
  /// \return the number of items
  /// \note   all items are transforms!
  XU32 GetNumItems() const;

  /// \brief  returns a pointer to the requested item
  /// \param  idx  -  the index into the item list
  /// \return a pointer to the object
  XBase* GetItem(XU32 idx) const;

  /// \brief  allows you to query if the specified transform belongs to that set.
  /// \param  item - the object to query
  /// \return true if the object is part of the set
  bool IsItem(const XBase* item) const;

  /// \brief  adds an item into the display layer
  /// \param  item - the transform to add to the set
  /// \return true if OK, false if already in set or invalid type
  bool AddItem(const XBase* item);

  /// \brief  deletes the item from the display layer
  /// \param  item - the handle to the object to remove from the layer
  /// \return true if removed OK
  bool DeleteItem(const XBase* item);

  /// \brief  deletes the item from the display layer
  /// \param  idx  -  the index of the item to remove
  /// \return true if deleted, false if not found
  bool DeleteItem(XU32 idx);

  /// \brief  returns the array of items in the display layer
  /// \param  items - the returned array of items stored in the display layer
  void GetItems(XList& items) const;

  /// \brief  sets the array of items in the display layer
  /// \param  items - the new array of items to add into the display layer
  void SetItems(const XList& items);

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  XIndexList m_Objects;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::NodeCollection, XNodeCollection, "nodeCollection");
#endif
}
