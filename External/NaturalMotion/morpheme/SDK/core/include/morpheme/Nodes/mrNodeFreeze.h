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
#ifndef MR_NODE_FREEZE_H
#define MR_NODE_FREEZE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------

void nodeFreezeInitInstance(NodeDef* node, Network* net);

NodeID nodeFreezeUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

/// \brief Passes through the previous frame's transforms
Task* nodeFreezePassThroughLastTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFreezePassThroughLastTrajectoryDeltaAndTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

/// \brief Passes through the previous frame's transforms with a lifespan of LIFESPAN_FOREVER and valid frame of VALID_FOREVER.
///   This has the effect of only updating the attrib data the first time the task is called when the attrib data doesn't 
///   exist.
Task* nodeFreezePassThroughLastTransformsOnce(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFreezePassThroughLastTrajectoryDeltaAndTransformsOnce(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeFreezeHasQueuingFunction(
  NodeID             callingNodeID,
  bool               fromParent,
  AttribDataSemantic semantic,
  NodeDef*           node,
  Network*           network);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_FREEZE_H
//----------------------------------------------------------------------------------------------------------------------
