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
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "SteppingBalancePackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStep.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SteppingBalance*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SteppingBalance_Con::~SteppingBalance_Con()
{
  junc_feedIn_groundVel->destroy();
  junc_feedIn_centreOfMassBodyState->destroy();
  junc_feedIn_isDoingRecoveryStep->destroy();
  junc_junc_FeedbackInputs_stepDir_1->destroy();
  junc_junc_FeedbackInputs_stepDir_0->destroy();
  junc_junc_FeedbackInputs_isLegStepping_1->destroy();
  junc_junc_FeedbackInputs_isLegStepping_0->destroy();
  junc_in_armBalancePoseEndRelativeToRoot_1->destroy();
  junc_in_armBalancePoseEndRelativeToRoot_0->destroy();
  junc_in_armReadyPoseEndRelativeToRoot_1->destroy();
  junc_in_armReadyPoseEndRelativeToRoot_0->destroy();
  junc_in_steppingBalanceParameters->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SteppingBalance_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SteppingBalance_Con), 16);

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
void SteppingBalance_Con::buildConnections(SteppingBalance* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SteppingBalance_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_steppingBalanceParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSteppingBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSteppingBalanceParametersImportanceRef()) );

  junc_in_armReadyPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armReadyPoseEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRootImportanceRef(1)) );

  junc_in_armBalancePoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armBalancePoseEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_junc_FeedbackInputs_isLegStepping_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isLegStepping_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_stepDir_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getStepDirImportanceRef()) );

  junc_junc_FeedbackInputs_stepDir_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getStepDirImportanceRef()) );

  junc_feedIn_isDoingRecoveryStep = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedIn->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedIn->getIsDoingRecoveryStepImportanceRef()) );

  junc_feedIn_centreOfMassBodyState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyStateImportanceRef()) );

  junc_feedIn_groundVel = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getGroundVelocity()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getGroundVelocityImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SteppingBalance_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_steppingBalanceParameters = ER::Junction::relocate(resource);
  // junc_in_steppingBalanceParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSteppingBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSteppingBalanceParametersImportanceRef()) );

  junc_in_armReadyPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_armReadyPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armReadyPoseEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_armReadyPoseEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmReadyPoseEndRelativeToRootImportanceRef(1)) );

  junc_in_armBalancePoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_armBalancePoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armBalancePoseEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_armBalancePoseEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_junc_FeedbackInputs_isLegStepping_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isLegStepping_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isLegStepping_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isLegStepping_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_stepDir_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_stepDir_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getStepDirImportanceRef()) );

  junc_junc_FeedbackInputs_stepDir_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_stepDir_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getStepDirImportanceRef()) );

  junc_feedIn_isDoingRecoveryStep = ER::Junction::relocate(resource);
  // junc_feedIn_isDoingRecoveryStep->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedIn->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedIn->getIsDoingRecoveryStepImportanceRef()) );

  junc_feedIn_centreOfMassBodyState = ER::Junction::relocate(resource);
  // junc_feedIn_centreOfMassBodyState->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyStateImportanceRef()) );

  junc_feedIn_groundVel = ER::Junction::relocate(resource);
  // junc_feedIn_groundVel->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getGroundVelocity()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getGroundVelocityImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SteppingBalance_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  SteppingBalance* module = (SteppingBalance*)modulePtr;
  SteppingBalanceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_isLegSteppingImportance[0] = junc_junc_FeedbackInputs_isLegStepping_0->combineDirectInput(&feedIn.isLegStepping[0]);
  feedIn.m_isLegSteppingImportance[1] = junc_junc_FeedbackInputs_isLegStepping_1->combineDirectInput(&feedIn.isLegStepping[1]);
  feedIn.m_stepDirImportance[0] = junc_junc_FeedbackInputs_stepDir_0->combineDirectInput(&feedIn.stepDir[0]);
  feedIn.m_stepDirImportance[1] = junc_junc_FeedbackInputs_stepDir_1->combineDirectInput(&feedIn.stepDir[1]);
  feedIn.m_isDoingRecoveryStepImportance = junc_feedIn_isDoingRecoveryStep->combineDirectInput(&feedIn.isDoingRecoveryStep);
  feedIn.m_centreOfMassBodyStateImportance = junc_feedIn_centreOfMassBodyState->combineDirectInput(&feedIn.centreOfMassBodyState);
  feedIn.m_groundVelImportance = junc_feedIn_groundVel->combineDirectInput(&feedIn.groundVel);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SteppingBalance_Con::combineInputs(ER::Module* modulePtr)
{
  SteppingBalance* module = (SteppingBalance*)modulePtr;
  SteppingBalanceInputs& in = *module->in;

  // Junction assignments.
  in.m_steppingBalanceParametersImportance = junc_in_steppingBalanceParameters->combineDirectInput(&in.steppingBalanceParameters);
  in.m_armReadyPoseEndRelativeToRootImportance[0] = junc_in_armReadyPoseEndRelativeToRoot_0->combineDirectInput(&in.armReadyPoseEndRelativeToRoot[0]);
  in.m_armReadyPoseEndRelativeToRootImportance[1] = junc_in_armReadyPoseEndRelativeToRoot_1->combineDirectInput(&in.armReadyPoseEndRelativeToRoot[1]);
  in.m_armBalancePoseEndRelativeToRootImportance[0] = junc_in_armBalancePoseEndRelativeToRoot_0->combineDirectInput(&in.armBalancePoseEndRelativeToRoot[0]);
  in.m_armBalancePoseEndRelativeToRootImportance[1] = junc_in_armBalancePoseEndRelativeToRoot_1->combineDirectInput(&in.armBalancePoseEndRelativeToRoot[1]);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


