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
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "ReachForBodyPackaging.h"
#include "MyNetwork.h"
#include "ReachForBodyBehaviourInterface.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "Arm.h"
#include "ArmBrace.h"
#include "ArmReachForBodyPart.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ReachForBody*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ReachForBody_Con::~ReachForBody_Con()
{
  junc_feedIn_armStrengthReduction_1->destroy();
  junc_feedIn_armStrengthReduction_0->destroy();
  junc_feedIn_endEffectorDistanceSquaredToTargets_1->destroy();
  junc_feedIn_endEffectorDistanceSquaredToTargets_0->destroy();
  junc_feedIn_armReachStates_1->destroy();
  junc_feedIn_armReachStates_0->destroy();
  junc_feedIn_shoulderPositions_1->destroy();
  junc_feedIn_shoulderPositions_0->destroy();
  junc_feedIn_competingArmControls_1->destroy();
  junc_feedIn_competingArmControls_0->destroy();
  junc_in_braceHazard->destroy();
  junc_in_reachActionParams_1->destroy();
  junc_in_reachActionParams_0->destroy();
  junc_in_armReachForBodyPartFlags_1->destroy();
  junc_in_armReachForBodyPartFlags_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ReachForBody_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ReachForBody_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ReachForBody_Con::buildConnections(ReachForBody* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ReachForBody_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_armReachForBodyPartFlags_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlags(0)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlagsImportanceRef(0)) );
  junc_in_armReachForBodyPartFlags_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlags(1)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlagsImportanceRef(1)) );

  junc_in_reachActionParams_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParams(0)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParamsImportanceRef(0)) );
  junc_in_reachActionParams_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParams(1)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParamsImportanceRef(1)) );

  junc_in_braceHazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazardImportanceRef()) );

  junc_feedIn_competingArmControls_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->brace->out->getControl()), (float*)&(module->owner->owner->arms[0]->brace->out->getControlImportanceRef()) );
  junc_feedIn_competingArmControls_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->brace->out->getControl()), (float*)&(module->owner->owner->arms[1]->brace->out->getControlImportanceRef()) );

  junc_feedIn_shoulderPositions_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getBasePosition()), (float*)&(module->owner->owner->arms[0]->feedOut->getBasePositionImportanceRef()) );
  junc_feedIn_shoulderPositions_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getBasePosition()), (float*)&(module->owner->owner->arms[1]->feedOut->getBasePositionImportanceRef()) );

  junc_feedIn_armReachStates_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getState()), (float*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getStateImportanceRef()) );
  junc_feedIn_armReachStates_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getState()), (float*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getStateImportanceRef()) );

  junc_feedIn_endEffectorDistanceSquaredToTargets_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getDistanceSquaredToTarget()), (float*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getDistanceSquaredToTargetImportanceRef()) );
  junc_feedIn_endEffectorDistanceSquaredToTargets_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getDistanceSquaredToTarget()), (float*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getDistanceSquaredToTargetImportanceRef()) );

  junc_feedIn_armStrengthReduction_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getArmStrengthReduction()), (float*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getArmStrengthReductionImportanceRef()) );
  junc_feedIn_armStrengthReduction_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getArmStrengthReduction()), (float*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getArmStrengthReductionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ReachForBody_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_armReachForBodyPartFlags_0 = ER::Junction::relocate(resource);
  // junc_in_armReachForBodyPartFlags_0->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlags(0)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlagsImportanceRef(0)) );
  junc_in_armReachForBodyPartFlags_1 = ER::Junction::relocate(resource);
  // junc_in_armReachForBodyPartFlags_1->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlags(1)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getArmReachForBodyPartFlagsImportanceRef(1)) );

  junc_in_reachActionParams_0 = ER::Junction::relocate(resource);
  // junc_in_reachActionParams_0->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParams(0)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParamsImportanceRef(0)) );
  junc_in_reachActionParams_1 = ER::Junction::relocate(resource);
  // junc_in_reachActionParams_1->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParams(1)), (float*)&(module->owner->owner->reachForBodyBehaviourInterface->out->getReachActionParamsImportanceRef(1)) );

  junc_in_braceHazard = ER::Junction::relocate(resource);
  // junc_in_braceHazard->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazardImportanceRef()) );

  junc_feedIn_competingArmControls_0 = ER::Junction::relocate(resource);
  // junc_feedIn_competingArmControls_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->brace->out->getControl()), (float*)&(module->owner->owner->arms[0]->brace->out->getControlImportanceRef()) );
  junc_feedIn_competingArmControls_1 = ER::Junction::relocate(resource);
  // junc_feedIn_competingArmControls_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->brace->out->getControl()), (float*)&(module->owner->owner->arms[1]->brace->out->getControlImportanceRef()) );

  junc_feedIn_shoulderPositions_0 = ER::Junction::relocate(resource);
  // junc_feedIn_shoulderPositions_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getBasePosition()), (float*)&(module->owner->owner->arms[0]->feedOut->getBasePositionImportanceRef()) );
  junc_feedIn_shoulderPositions_1 = ER::Junction::relocate(resource);
  // junc_feedIn_shoulderPositions_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getBasePosition()), (float*)&(module->owner->owner->arms[1]->feedOut->getBasePositionImportanceRef()) );

  junc_feedIn_armReachStates_0 = ER::Junction::relocate(resource);
  // junc_feedIn_armReachStates_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getState()), (float*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getStateImportanceRef()) );
  junc_feedIn_armReachStates_1 = ER::Junction::relocate(resource);
  // junc_feedIn_armReachStates_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getState()), (float*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getStateImportanceRef()) );

  junc_feedIn_endEffectorDistanceSquaredToTargets_0 = ER::Junction::relocate(resource);
  // junc_feedIn_endEffectorDistanceSquaredToTargets_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getDistanceSquaredToTarget()), (float*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getDistanceSquaredToTargetImportanceRef()) );
  junc_feedIn_endEffectorDistanceSquaredToTargets_1 = ER::Junction::relocate(resource);
  // junc_feedIn_endEffectorDistanceSquaredToTargets_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getDistanceSquaredToTarget()), (float*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getDistanceSquaredToTargetImportanceRef()) );

  junc_feedIn_armStrengthReduction_0 = ER::Junction::relocate(resource);
  // junc_feedIn_armStrengthReduction_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getArmStrengthReduction()), (float*)&(module->owner->owner->arms[0]->reachForBodyPart->feedOut->getArmStrengthReductionImportanceRef()) );
  junc_feedIn_armStrengthReduction_1 = ER::Junction::relocate(resource);
  // junc_feedIn_armStrengthReduction_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getArmStrengthReduction()), (float*)&(module->owner->owner->arms[1]->reachForBodyPart->feedOut->getArmStrengthReductionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ReachForBody_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ReachForBody* module = (ReachForBody*)modulePtr;
  ReachForBodyFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_competingArmControlsImportance[0] = junc_feedIn_competingArmControls_0->combineDirectInput(&feedIn.competingArmControls[0]);
  feedIn.m_competingArmControlsImportance[1] = junc_feedIn_competingArmControls_1->combineDirectInput(&feedIn.competingArmControls[1]);
  feedIn.m_shoulderPositionsImportance[0] = junc_feedIn_shoulderPositions_0->combineDirectInput(&feedIn.shoulderPositions[0]);
  feedIn.m_shoulderPositionsImportance[1] = junc_feedIn_shoulderPositions_1->combineDirectInput(&feedIn.shoulderPositions[1]);
  feedIn.m_armReachStatesImportance[0] = junc_feedIn_armReachStates_0->combineDirectInput(&feedIn.armReachStates[0]);
  feedIn.m_armReachStatesImportance[1] = junc_feedIn_armReachStates_1->combineDirectInput(&feedIn.armReachStates[1]);
  feedIn.m_endEffectorDistanceSquaredToTargetsImportance[0] = junc_feedIn_endEffectorDistanceSquaredToTargets_0->combineDirectInput(&feedIn.endEffectorDistanceSquaredToTargets[0]);
  feedIn.m_endEffectorDistanceSquaredToTargetsImportance[1] = junc_feedIn_endEffectorDistanceSquaredToTargets_1->combineDirectInput(&feedIn.endEffectorDistanceSquaredToTargets[1]);
  feedIn.m_armStrengthReductionImportance[0] = junc_feedIn_armStrengthReduction_0->combineDirectInput(&feedIn.armStrengthReduction[0]);
  feedIn.m_armStrengthReductionImportance[1] = junc_feedIn_armStrengthReduction_1->combineDirectInput(&feedIn.armStrengthReduction[1]);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ReachForBody_Con::combineInputs(ER::Module* modulePtr)
{
  ReachForBody* module = (ReachForBody*)modulePtr;
  ReachForBodyInputs& in = *module->in;

  // Junction assignments.
  in.m_armReachForBodyPartFlagsImportance[0] = junc_in_armReachForBodyPartFlags_0->combineDirectInput(&in.armReachForBodyPartFlags[0]);
  in.m_armReachForBodyPartFlagsImportance[1] = junc_in_armReachForBodyPartFlags_1->combineDirectInput(&in.armReachForBodyPartFlags[1]);
  in.m_reachActionParamsImportance[0] = junc_in_reachActionParams_0->combineDirectInput(&in.reachActionParams[0]);
  in.m_reachActionParamsImportance[1] = junc_in_reachActionParams_1->combineDirectInput(&in.reachActionParams[1]);
  in.m_braceHazardImportance = junc_in_braceHazard->combineDirectInput(&in.braceHazard);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


