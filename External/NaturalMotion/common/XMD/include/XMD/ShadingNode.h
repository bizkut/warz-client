//----------------------------------------------------------------------------------------------------------------------
/// \file   ShadingNode.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a the base shading node class
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
#include "XM2/Colour.h"
#include "XM2/vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
// forward declarations
class XMD_EXPORT XHwShaderNode;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XShadingNode
/// \brief Defines the base shading node from which all shading related nodes are derived.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XShadingNode :
  public XBase
{
  XMD_CHUNK(XShadingNode);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast      = XBase::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  function to list all shading nodes connected to this node
  /// \param  list - the list of connected nodes
  /// \param  inputs - whether to list input connections
  /// \param  outputs - whether to list output connections
  void ListConnections(XList& list, bool inputs = true, bool outputs = true);

protected:
#ifndef DOXYGEN_PROCESSING

  /// \brief  Returns the number of times this node is referenced by
  ///         other shading nodes
  /// \param  ignore_id - an optional node id to ignore
  /// \return the number of references to the node
  XU32 NumTimesReferenced(XId* ignore_id = 0) const;

  /// \brief  This class is used for the properties of shading nodes allowing
  ///         either a colour or another shading node as input.
  class XMD_EXPORT XColourConnection
  {
  public:
    /// \brief  ctor
    XColourConnection(const XChar* name);
    /// \brief  ctor
    XColourConnection(const XChar* name, XId input);
    /// \brief  ctor
    XColourConnection(const XChar* name, const XColour& colour);
    /// \brief  ctor
    XColourConnection(const XChar* name, XId input, const XColour& colour);

    bool ReadChunk(std::istream& ifs);
    bool WriteChunk(std::ostream& ofs);
    bool DoData(XFileIO& io);
    XU32 GetDataSize() const;

    /// name of the connection
    XString m_Name;
    // the input shading node to this connection
    XId m_InputNode;
    // the colour of this connection
    XColour m_Colour;
  };

  typedef XM2::pod_vector<XColourConnection*> XConnectionArray;
  // all colour connection member variables in derived nodes must be added
  // to this list of connections
  XConnectionArray m_Connections;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::ShadingNode, XShadingNode, "shadingNode");
#endif
}
