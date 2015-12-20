//----------------------------------------------------------------------------------------------------------------------
/// \file   InheritedNode.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines methods and classes to help process inherited node types.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XBase;
class XMD_EXPORT XFileIO;
class XMD_EXPORT XModel;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief This class is used to hold information about the new
///        inherited nodes added into the file when exported from Maya.
///        The info structures will appear within the first part
///        of the file so that the custom chunk headers can be
///        recognised and appropriately parsed.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XInheritedNode
{
public:
#ifndef DOXYGEN_PROCESSING

  /// \brief  ctor
  XInheritedNode(XU16 ID);

  /// \brief  copy ctor
  /// \param  rhs - the node to copy
  XInheritedNode(const XInheritedNode& rhs);

  /// \brief  This function reads the inherited node info from the file.
  /// \param  ifs  -  the input file stream
  /// \return true if OK
  bool Read(std::istream& ifs);

  /// \brief  This function writes the inherited node info to the file.
  /// \param  ofs  -  the output file stream
  /// \return true if OK
  bool Write(std::ostream& ofs) const;

  /// \brief  creates a new inherited node
  /// \param  mod - the model to attach it to
  /// \return the new node instance.
  XBase* Create(XModel* mod) const;

  /// \brief  This function reads/writes the data for the chunk.
  /// \param  io  -  the binary file object
  /// \return true if OK
  bool DoData(XFileIO& io);

  /// \brief  This function returns the size in bytes needed for this chunk
  /// \return the total size in bytes of this chunk
  XU32 GetDataSize() const;

  /// \brief  This function reads/writes the header for the inherited node info.
  /// \param  io  -  the binary file object
  /// \return true if OK
  bool DoHeader(XFileIO& io);

  /// \brief  This function returns the base type of this node
  /// \return the type identifier
  const XFn::Type ApiType() const;

  /// \brief  This function returns the name of the files chunk header
  /// \return the chunk header string
  const XString& GetHeader() const;

  /// \brief  This function returns the base type of this node
  /// \return the type identifier
  const XFn::Type& GetType() const;

#endif  

  /// \brief  returns a custom ID for the inherited node types 
  XU16 CustomTypeID() const;

  /// \brief  This function returns the base type of this node
  /// \return the type identifier
  const XString& GetTypeName() const;

private:
  /// an identifier for the custom node in the binary file
  XU16 m_TypeID;
  /// the chunk header within the file
  XString m_TypeName;
  /// the type from which this node is derived.
  XFn::Type m_BaseType;
};

/// an array of inherited node information
typedef XM2::pod_vector<XInheritedNode*> XInheritedNodeList;

#ifndef DOXYGEN_PROCESSING
extern XFn::Type XMD_EXPORT HeaderToType(const XString& chunk_header);
extern XString XMD_EXPORT TypeToHeader(XFn::Type type);
extern XBase XMD_EXPORT * XCreateNode(XModel* pmod, XFn::Type type);
#endif
}
