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
#ifndef MR_NODE_RETARGET_H
#define MR_NODE_RETARGET_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeRetargetQueueUpdateTransforms(
    NodeDef        *node,
    TaskQueue      *queue,
    Network        *net,
    TaskParameter  *dependentParameter
    );

Task* nodeRetargetQueueTrajectoryDeltaTransform(
  NodeDef       *node,
  TaskQueue     *queue,
  Network       *net,
  TaskParameter *dependentParameter);

Task* nodeRetargetQueueTrajectoryDeltaAndTransforms(
  NodeDef       *node,
  TaskQueue     *queue,
  Network       *net,
  TaskParameter *dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
NodeID nodeRetargetUpdateConnections(
    NodeDef           *node,
    Network           *net
    );

//----------------------------------------------------------------------------------------------------------------------
// Handles a change of offsets - should only be used by connect, not by runtime
bool nodeRetargetUpdateOffsetsAndScaleMessageHandler(
  const MR::Message& message,
  MR::NodeID nodeID,
  MR::Network* net);


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_RETARGET_H
//----------------------------------------------------------------------------------------------------------------------

