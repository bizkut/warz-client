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
#include "Leg.h"
#include "LegReachReaction.h"
#include "LegReachReactionPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegReachReaction*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegReachReaction_Con::~LegReachReaction_Con()
{
  junc_in_avgReachingHandPos->destroy();
  junc_in_supportAmount->destroy();
  junc_in_reachTarget->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegReachReaction_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegReachReaction_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction_Con::buildConnections(LegReachReaction* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegReachReaction_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_reachTarget = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTarget()), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTargetImportanceRef()) );

  junc_in_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_avgReachingHandPos = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getErrorWeightedAverageEffectorPosition()), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getErrorWeightedAverageEffectorPositionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegReachReaction_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_reachTarget = ER::Junction::relocate(resource);
  // junc_in_reachTarget->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTarget()), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTargetImportanceRef()) );

  junc_in_supportAmount = ER::Junction::relocate(resource);
  // junc_in_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_avgReachingHandPos = ER::Junction::relocate(resource);
  // junc_in_avgReachingHandPos->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getErrorWeightedAverageEffectorPosition()), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getErrorWeightedAverageEffectorPositionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegReachReaction_Con::combineInputs(ER::Module* modulePtr)
{
  LegReachReaction* module = (LegReachReaction*)modulePtr;
  LegReachReactionInputs& in = *module->in;

  // Junction assignments.
  in.m_reachTargetImportance = junc_in_reachTarget->combineDirectInput(&in.reachTarget);
  in.m_supportAmountImportance = junc_in_supportAmount->combineDirectInput(&in.supportAmount);
  in.m_avgReachingHandPosImportance = junc_in_avgReachingHandPos->combineDirectInput(&in.avgReachingHandPos);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


