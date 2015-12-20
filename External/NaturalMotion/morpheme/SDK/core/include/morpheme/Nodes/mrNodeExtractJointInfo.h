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
#ifndef MR_NODE_EXTRACT_JOINT_INFO
#define MR_NODE_EXTRACT_JOINT_INFO
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations.

// Outputs Object space transforms of a predefined joint.
Task* nodeExtractJointInfoObjectQueueTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

// Outputs Object space transforms of a predefined joint.
Task* nodeExtractJointInfoObjectQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

// Outputs Local space transforms of a predefined joint.
Task* nodeExtractJointInfoLocalQueueTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

// Outputs Local space transforms of a predefined joint.
Task* nodeExtractJointInfoLocalQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

// Outputs Object space transforms of a dynamically selected joint.
Task* nodeExtractJointInfoObjectJointSelectQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

// Outputs Object space transforms of a dynamically selected joint.
Task* nodeExtractJointInfoObjectJointSelectQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

// Outputs Local space transforms of a dynamically selected joint.
Task* nodeExtractJointInfoLocalJointSelectQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

// Outputs Local space transforms of a dynamically selected joint.
Task* nodeExtractJointInfoLocalJointSelectQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Overloaded update connections function.

// Use this one only when we are dynamically selecting the joint to output otherwise use nodeUpdateConnectionsDefaultChildren.
NodeID nodeExtractJointInfoJointSelectUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeExtractJointInfoOutCPPinIDs
{
  NODE_EXTRACT_JOINT_INFO_OUT_PINID_POSITION = 0,
  NODE_EXTRACT_JOINT_INFO_OUT_PINID_ANGLE,
  NODE_EXTRACT_JOINT_INFO_OUT_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_EXTRACT_JOINT_INFO
//----------------------------------------------------------------------------------------------------------------------
