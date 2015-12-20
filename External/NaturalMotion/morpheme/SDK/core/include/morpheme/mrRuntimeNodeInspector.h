// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_RUNTIME_NODE_INSPECTOR_H
#define MR_RUNTIME_NODE_INSPECTOR_H
//----------------------------------------------------------------------------------------------------------------------
#include "../../../SDK/sharedDefines/mSharedDefines.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup morphemecomms
/// \brief A set of functions used to buffer and extract data from Nodes.
///
/// This set of utility-function provides support for inspection of nodes and
/// their runtime data. They operate on specific network instances and use their
/// public interfaces to collect the data.
namespace RuntimeNodeInspector
{

#if defined(MR_OUTPUT_DEBUGGING)

/// \brief Tells an array of Nodes to start or stop storing their output data in a buffer for access later.
///
/// This allows us to examine the content of data structures that may only exist temporarily.
/// \return false if all Nodes were unable to be set.
extern bool enableNodeDataBuffering(
  MR::Network*           net,
  MR::NodeID             nodeID,        ///<  Node Network ID to switch on or off.
  MR::NodeOutputDataType dataType,      ///<
  bool                   enable         ///< Turn buffering on or off for this node
);

/// \brief Fill an array with all the output data types that this Node can buffer.
/// \return Number of bufferable types supported by this Node.
extern uint32_t getNodeNodeOutputDataTypes(
  MR::Network*            net,
  MR::NodeID              nodeID,           ///< Node Network ID.
  MR::NodeOutputDataType* typesArray,       ///< Array of types to be filled in.
  uint32_t                maxTypesArraySize ///< The size of the provided array (number of entries).
);

/// \brief Get the size of the buffered data of the specified type from a node.
/// \return The size of the buffered data in bytes.
extern uint32_t getNodeBufferedDataTypeSize(
  MR::Network*            net,
  MR::NodeID              owningNodeID,   ///< Node Network ID.
  MR::NodeOutputDataType  outputType,     ///< Type we want to get the size of. NOTE: This type is currently used by
                                          ///<  connect to identify not only what the data structure is but also what node
                                          ///<  type it is from i.e. a float attrib is automatically identified as coming
                                          ///<  from a control param.
                                          //    MORPH-21346: This is inaccurate as a float attrib can be created
                                          //    by any node type. Interpreting code in connect needs to look at the source
                                          //    node in conjunction with the full runtime attrib address in order to
                                          //    determine its purpose.
  MR::NodeID&             targetNodeID,   ///< Returned - usually MR::INVALID_NODE_ID
  MR::AttribDataType&     attribType,     ///< Runtime attribute data type.
  MR::AttribDataSemantic& attribSemantic, ///< Runtime semantic identifier.
  MR::AnimSetIndex&       animSetIndex,   ///< What anim set this attrib data was created for.
  MR::FrameCount&         validFrame      ///< Usually the frame it was created or MR::VALID_FOREVER.
);

/// \brief Get the attrib data of the requested output data type from the node.
/// \return 0 if there is no attrib data of this output data type for this Node.
extern MR::AttribData* getNodeAttribDataForOutputDataType(
  MR::Network*           net,
  MR::NodeID             nodeID,           ///< Node Network ID.
  MR::NodeOutputDataType type              ///< Type we want to get buffered data for.
);

/// \brief Copy attrib data into the provided buffer.
/// \return true if successful
extern bool serialiseAttribData(
  MR::Network*           net,
  MR::NodeID             owningNodeID,     ///< Node Network ID.
  MR::AttribData*        attribData,       ///< Attrib data to serialise
  void*                  buffer,           ///< Buffer to copy into.
  uint32_t               bufferSize        ///< In bytes.
);

#endif // defined (MR_OUTPUT_DEBUGGING)

}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_RUNTIME_NODE_INSPECTOR_H
//----------------------------------------------------------------------------------------------------------------------
