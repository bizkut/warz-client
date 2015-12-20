//----------------------------------------------------------------------------------------------------------------------
/// \file   ExtraAttributes.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the mechanism for storing additional attributes exported from Maya. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Attribute.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XExtraAttributes 
/// \brief  This class adds the ability to dynamically add variables to the scene objects
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XExtraAttributes 
  : public XAttributeList
{
  friend class XMD_EXPORT XBase;
  friend std::istream &operator>>(std::istream& ifs, XExtraAttributes& ea);
  friend std::ostream &operator<<(std::ostream& ofs, const XExtraAttributes& ea);
public:

  /// \brief  returns a pointer to the requested attribute
  /// \param  name  -  the name of the attribute
  /// \return a pointer to the attribute
  const XAttribute* GetAttr(const XString& name) const;

  /// \brief  returns a pointer to the requested attribute
  /// \param  name  -  the name of the attribute
  /// \return a pointer to the attribute
  XAttribute* GetAttr(const XString& name);

  /// \brief  returns a list of attribute names
  /// \param  attr_names  -  the returned list of names
  void ListAttr(XStringList& attr_names) const;

#ifndef DOXYGEN_PROCESSING
  /// \brief  ctor
  XExtraAttributes();

  /// \brief  copy ctor
  /// \param  ea  - the extra attrs to copy
  XExtraAttributes(const XExtraAttributes& ea);

  /// \brief  dtor
  ~XExtraAttributes();

protected:
  /// \brief  This functions reads and writes the binary file format
  /// \param  io  -  the input output file object
  /// \return true if OK
  bool DoData(XFileIO& io, XModel* model);

  /// \brief  returns the size in bytes of the extra attrs chunk when 
  ///         written to disk. Required to write the chunks size prior
  ///         to writing out in the binary format.
  /// \return the data size in bytes of the extra attrs when written
  ///         to a binary file
  XU32 GetDataSize() const;

  /// \brief  Used to notify the attributes about a node's death. This
  ///         allows for any connections to that node to be destroyed.
  /// \param  model - model from which the node is about to be deleted
  /// \param  node  - the id of the node that is about to be destroyed.
  void NodeDeath(XModel* model, XId node);

#endif
};
}
