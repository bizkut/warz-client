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
#ifndef MR_NODE
#define MR_NODE
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrTask.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// forward decls
class NodeDef;
class NetworkDef;
class Network;
class TaskQueue;
struct Message;

//----------------------------------------------------------------------------------------------------------------------
/// Typedef for the queuing function of a specific attrib data item for a given node type.
/// Note that node defs can share these if they wish.
typedef Task* (*QueueAttrTaskFn)(
  NodeDef*       node,          ///< The node we are trying to queue a task for.
  TaskQueue*     queue,         ///< The queue to put any generated tasks on to.
  Network*       network,       ///< Network instance that we are trying to queue a task for.
  TaskParameter* dependentParam ///< IN: The param from the above task that is dependent on this attribute.
);

//----------------------------------------------------------------------------------------------------------------------
/// Typedef for the immediate update of an attribute belonging to a control parameter node.
/// Note that node defs can share these if they wish.
typedef AttribData* (*OutputCPTask)(  
  NodeDef*  node,             ///< The node that this task is being called on.
  PinIndex  outputCPPinIndex, ///< Specifies the output control parameter pin that this task must generate and return data for.
                              ///<  Note that an OutputCPTask can generate data for many output pins and cache the results,
                              ///<  but this is the pin result that a connected node has asked for so this is what must be returned.
  Network*  network           ///< The Network instance that this task is being called on.
);

//----------------------------------------------------------------------------------------------------------------------
///
typedef void (*InitNodeInstance)(
  NodeDef* node, ///<
  Network* net   ///<
);

//----------------------------------------------------------------------------------------------------------------------
/// Typedef for deletion of all attrib data for a specific node existing in the Network AttribDatas array.
/// Also recurses deletion to any children that this node knows may have attrib data existing in the Network.
typedef void (*DeleteNodeInstance)(
  const NodeDef* node,              ///<
  Network*       net                ///<
);

//----------------------------------------------------------------------------------------------------------------------
/// Typedef function that updates all data for this node that can only be computed immediately rather than as part of the task queue.
/// This includes determining those child nodes that are active and hence require update this frame.
/// Also includes update of connected control parameters.
/// This function is called as the first operation on all active nodes in the network, via a recursive call from the root.
/// Each nodes implementation of this function must handle recursion to its children.
typedef NodeID (*UpdateNodeConnections)(
  NodeDef* node,      ///<
  Network* net        ///<
);

//----------------------------------------------------------------------------------------------------------------------
/// Indicates if this node generates attribute semantic data of the specified type.
/// The fromParent flag indicates the direction from which this query came; from the parent or a child.
/// Note that if the node in question passes through any semantic updates, for example a filter node, then
/// this query may be recursed on to a parent or child node.
/// For use by node queuing functions to determine the capabilities of their connected nodes.
typedef NodeID (*FindGeneratingNodeForSemanticFn)(
  NodeID              callingNodeID,
  bool                fromParent,   ///< Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net);

//----------------------------------------------------------------------------------------------------------------------
/// Typedef function that receives messages.
/// It is optional for a node to implement this function.
/// Message may originate from outside of the network or from an emitted request.
/// The message is only guaranteed to exist for the lifetime of the function call, so no references to the message or 
/// any of its data should be stored.
typedef bool (*MessageHandlerFn)(
  const Message&      message,
  NodeID              nodeID,
  Network*            net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE
//----------------------------------------------------------------------------------------------------------------------
