// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef ER_NODE_BEHAVIOUR_H
#define ER_NODE_BEHAVIOUR_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

MR::NodeID nodeBehaviourUpdateConnections(
  MR::NodeDef* node,
  MR::Network* net);

void nodeBehaviourDeleteInstance(
  const MR::NodeDef* node,
  MR::Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
MR::Task* nodeBehaviourQueueUpdateTrajectory(
  MR::NodeDef*       node,
  MR::TaskQueue*     queue,
  MR::Network*       net,
  MR::TaskParameter* dependentParameter);

MR::Task* nodeBehaviourQueueUpdateTransformsPrePhysics(
  MR::NodeDef*       node,
  MR::TaskQueue*     queue,
  MR::Network*       net,
  MR::TaskParameter* dependentParameter);

MR::Task* nodeBehaviourQueueUpdateTransformsPostPhysics(
  MR::NodeDef*       node,
  MR::TaskQueue*     queue,
  MR::Network*       net,
  MR::TaskParameter* dependentParameter);

MR::AttribData* nodeBehaviorOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex,
  MR::Network* net);

MR::AttribData* nodeBehaviourEmitMessageUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex,
  MR::Network* net);

MR::NodeID nodeBehaviourFindGeneratingNodeForSemantic(
  MR::NodeID              callingNodeID,
  bool                    fromParent,
  MR::AttribDataSemantic  semantic,
  MR::NodeDef*            node,
  MR::Network*            network);

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_NODE_BEHAVIOUR_H
//----------------------------------------------------------------------------------------------------------------------
