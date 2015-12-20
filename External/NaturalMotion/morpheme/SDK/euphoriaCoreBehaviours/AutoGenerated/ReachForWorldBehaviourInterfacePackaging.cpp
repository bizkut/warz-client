/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

// module def dependencies
#include "MyNetwork.h"
#include "ReachForWorldBehaviourInterface.h"
#include "ReachForWorldBehaviourInterfacePackaging.h"
#include "Arm.h"
#include "ArmReachForWorld.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ReachForWorldBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ReachForWorldBehaviourInterface_Con::~ReachForWorldBehaviourInterface_Con()
{
  junc_feedIn_distanceToTarget_1->destroy();
  junc_feedIn_distanceToTarget_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ReachForWorldBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ReachForWorldBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface_Con::buildConnections(ReachForWorldBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ReachForWorldBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_distanceToTarget_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getDistanceToTarget()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getDistanceToTargetImportanceRef()) );
  junc_feedIn_distanceToTarget_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getDistanceToTarget()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getDistanceToTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ReachForWorldBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_distanceToTarget_0 = ER::Junction::relocate(resource);
  // junc_feedIn_distanceToTarget_0->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getDistanceToTarget()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getDistanceToTargetImportanceRef()) );
  junc_feedIn_distanceToTarget_1 = ER::Junction::relocate(resource);
  // junc_feedIn_distanceToTarget_1->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getDistanceToTarget()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getDistanceToTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ReachForWorldBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ReachForWorldBehaviourInterface* module = (ReachForWorldBehaviourInterface*)modulePtr;
  ReachForWorldBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_distanceToTargetImportance[0] = junc_feedIn_distanceToTarget_0->combineDirectInput(&feedIn.distanceToTarget[0]);
  feedIn.m_distanceToTargetImportance[1] = junc_feedIn_distanceToTarget_1->combineDirectInput(&feedIn.distanceToTarget[1]);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


