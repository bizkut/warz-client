//----------------------------------------------------------------------------------------------------------------------
/// \file   Factory.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a node creation factory for XMD
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Instance.h"
#include "XMD/Base.h"
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFactory
/// \brief This class is used to create any nodes. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFactory
{
#ifndef DOXYGEN_PROCESSING
  friend class XModel;

protected:

  /// \brief  Creates a new node
  /// \param  pmod  -  the model to attach the node to
  /// \param  type  -  the new node type
  /// \return a pointer to the new node
  static XBase* CreateNode(XModel* pmod, XFn::Type type);

  /// \brief  converts the Type id to the chunk header identifier
  ///         used in the ascii file.
  /// \param  type  -  the type to get the cunk header for
  /// \return the chunk header name for this type 
  static XString TypeToHeader(XFn::Type type);

  /// \brief  returns the type for a specified file chunk header
  /// \param  chunk_header  -  the cunk header name
  /// \return the type ID for the header
  static XFn::Type HeaderToType(const XString& chunk_header);
#endif
};
}
