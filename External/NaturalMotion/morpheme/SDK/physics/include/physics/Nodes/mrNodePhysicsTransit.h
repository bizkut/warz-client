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
#ifndef MR_NODE_PHYSICS_TRANSIT_H
#define MR_NODE_PHYSICS_TRANSIT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations.
Task* nodePhysicsTransitQueueTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsTransitSyncEventsQueueTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeQueuePhysicalDeadBlendUpdateTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueFeatherBlend2ToPhysicsTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_PHYSICS_TRANSIT_H
//----------------------------------------------------------------------------------------------------------------------
