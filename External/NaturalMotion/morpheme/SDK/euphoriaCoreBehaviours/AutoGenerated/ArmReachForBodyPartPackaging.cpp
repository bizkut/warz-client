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
#include "Arm.h"
#include "ArmReachForBodyPart.h"
#include "ArmReachForBodyPartPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "BodyBalance.h"
#include "ArmReachReaction.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmReachForBodyPart*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmReachForBodyPart_Con::~ArmReachForBodyPart_Con()
{
  junc_armReachReactionDistanceFromTarget->destroy();
  junc_in_supportTM->destroy();
  junc_in_supportAmount->destroy();
  junc_in_reachActionParams->destroy();
  junc_in_reachEnabled->destroy();
  junc_in_target->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmReachForBodyPart_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmReachForBodyPart_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart_Con::buildConnections(ArmReachForBodyPart* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmReachForBodyPart_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_target = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTargets(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTargetsImportanceRef(module->owner->getChildIndex())) );

  junc_in_reachEnabled = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getArmReachForBodyPartFlags(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getArmReachForBodyPartFlagsImportanceRef(module->owner->getChildIndex())) );

  junc_in_reachActionParams = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachActionParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachActionParamsImportanceRef(module->owner->getChildIndex())) );

  junc_in_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTMImportanceRef()) );

  junc_armReachReactionDistanceFromTarget = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachReaction->out->getDistanceToTarget()), (float*)&(module->owner->owner->arms[0]->reachReaction->out->getDistanceToTargetImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->arms[1]->reachReaction->out->getDistanceToTarget()), (float*)&(module->owner->owner->arms[1]->reachReaction->out->getDistanceToTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmReachForBodyPart_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_target = ER::Junction::relocate(resource);
  // junc_in_target->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTargets(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachTargetsImportanceRef(module->owner->getChildIndex())) );

  junc_in_reachEnabled = ER::Junction::relocate(resource);
  // junc_in_reachEnabled->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getArmReachForBodyPartFlags(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getArmReachForBodyPartFlagsImportanceRef(module->owner->getChildIndex())) );

  junc_in_reachActionParams = ER::Junction::relocate(resource);
  // junc_in_reachActionParams->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachActionParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getReachActionParamsImportanceRef(module->owner->getChildIndex())) );

  junc_in_supportAmount = ER::Junction::relocate(resource);
  // junc_in_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_supportTM = ER::Junction::relocate(resource);
  // junc_in_supportTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTMImportanceRef()) );

  junc_armReachReactionDistanceFromTarget = ER::Junction::relocate(resource);
  // junc_armReachReactionDistanceFromTarget->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachReaction->out->getDistanceToTarget()), (float*)&(module->owner->owner->arms[0]->reachReaction->out->getDistanceToTargetImportanceRef()) );
  // junc_armReachReactionDistanceFromTarget->getEdges()[1].reinit( (char*)&(module->owner->owner->arms[1]->reachReaction->out->getDistanceToTarget()), (float*)&(module->owner->owner->arms[1]->reachReaction->out->getDistanceToTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmReachForBodyPart_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ArmReachForBodyPart* module = (ArmReachForBodyPart*)modulePtr;
  ArmReachForBodyPartFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_armReachReactionDistanceFromTargetImportance = junc_armReachReactionDistanceFromTarget->combineWinnerTakesAll(&feedIn.armReachReactionDistanceFromTarget);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmReachForBodyPart_Con::combineInputs(ER::Module* modulePtr)
{
  ArmReachForBodyPart* module = (ArmReachForBodyPart*)modulePtr;
  ArmReachForBodyPartInputs& in = *module->in;

  // Junction assignments.
  in.m_targetImportance = junc_in_target->combineDirectInput(&in.target);
  in.m_reachEnabledImportance = junc_in_reachEnabled->combineDirectInput(&in.reachEnabled);
  in.m_reachActionParamsImportance = junc_in_reachActionParams->combineDirectInput(&in.reachActionParams);
  in.m_supportAmountImportance = junc_in_supportAmount->combineDirectInput(&in.supportAmount);
  in.m_supportTMImportance = junc_in_supportTM->combineDirectInput(&in.supportTM);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


