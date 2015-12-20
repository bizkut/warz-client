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
#include "BalanceBehaviourInterface.h"
#include "BalanceBehaviourInterfacePackaging.h"
#include "CharacteristicsBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "Leg.h"
#include "LegStep.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BalanceBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BalanceBehaviourInterface_Con::~BalanceBehaviourInterface_Con()
{
  junc_out_averageLegEndRelativeToRoot->destroy();
  junc_out_averageArmEndRelativeToRoot->destroy();
  junc_feedIn_CoMBodyState->destroy();
  junc_feedIn_legStepFractions_1->destroy();
  junc_feedIn_legStepFractions_0->destroy();
  junc_feedIn_steppingTime->destroy();
  junc_feedIn_stepCount->destroy();
  junc_feedIn_timeSinceLastStep->destroy();
  junc_feedIn_maxLegCollidingTime->destroy();
  junc_feedIn_balanceAmount->destroy();
  junc_feedIn_supportVelocity->destroy();
  junc_feedIn_supportTM->destroy();
  junc_in_armStrengthReduction_1->destroy();
  junc_in_armStrengthReduction_0->destroy();
  junc_in_legStrengthReduction_1->destroy();
  junc_in_legStrengthReduction_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BalanceBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BalanceBehaviourInterface_Con), 16);

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
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface_Con::buildConnections(BalanceBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalanceBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_legStrengthReduction_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReduction(0)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReductionImportanceRef(0)) );
  junc_in_legStrengthReduction_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReduction(1)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReductionImportanceRef(1)) );

  junc_in_armStrengthReduction_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReduction(0)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReductionImportanceRef(0)) );
  junc_in_armStrengthReduction_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReduction(1)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReductionImportanceRef(1)) );

  junc_feedIn_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

  junc_feedIn_supportVelocity = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportVelocity()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportVelocityImportanceRef()) );

  junc_feedIn_balanceAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );

  junc_feedIn_maxLegCollidingTime = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getMaxLegCollidingTime()), (float*)&(module->owner->bodyFrame->feedIn->getMaxLegCollidingTimeImportanceRef()) );

  junc_feedIn_timeSinceLastStep = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->balanceManagement->steppingBalance->feedOut->getTimeSinceLastStep()), (float*)&(module->owner->balanceManagement->steppingBalance->feedOut->getTimeSinceLastStepImportanceRef()) );

  junc_feedIn_stepCount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->balanceManagement->steppingBalance->feedOut->getStepCount()), (float*)&(module->owner->balanceManagement->steppingBalance->feedOut->getStepCountImportanceRef()) );

  junc_feedIn_steppingTime = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->balanceManagement->steppingBalance->feedOut->getSteppingTime()), (float*)&(module->owner->balanceManagement->steppingBalance->feedOut->getSteppingTimeImportanceRef()) );

  junc_feedIn_legStepFractions_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->step->feedOut->getStepFraction()), (float*)&(module->owner->legs[0]->step->feedOut->getStepFractionImportanceRef()) );
  junc_feedIn_legStepFractions_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[1]->step->feedOut->getStepFraction()), (float*)&(module->owner->legs[1]->step->feedOut->getStepFractionImportanceRef()) );

  junc_feedIn_CoMBodyState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getCentreOfMassBodyState()), (float*)&(module->owner->bodyFrame->feedOut->getCentreOfMassBodyStateImportanceRef()) );

  junc_out_averageArmEndRelativeToRoot = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->out->getArmBalancePoseEndRelativeToRoot(0)), (float*)&(module->out->getArmBalancePoseEndRelativeToRootImportanceRef(0)) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->out->getArmBalancePoseEndRelativeToRoot(1)), (float*)&(module->out->getArmBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_out_averageLegEndRelativeToRoot = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->out->getLegBalancePoseEndRelativeToRoot(0)), (float*)&(module->out->getLegBalancePoseEndRelativeToRootImportanceRef(0)) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->out->getLegBalancePoseEndRelativeToRoot(1)), (float*)&(module->out->getLegBalancePoseEndRelativeToRootImportanceRef(1)) );

}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalanceBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_legStrengthReduction_0 = ER::Junction::relocate(resource);
  // junc_in_legStrengthReduction_0->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReduction(0)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReductionImportanceRef(0)) );
  junc_in_legStrengthReduction_1 = ER::Junction::relocate(resource);
  // junc_in_legStrengthReduction_1->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReduction(1)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReductionImportanceRef(1)) );

  junc_in_armStrengthReduction_0 = ER::Junction::relocate(resource);
  // junc_in_armStrengthReduction_0->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReduction(0)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReductionImportanceRef(0)) );
  junc_in_armStrengthReduction_1 = ER::Junction::relocate(resource);
  // junc_in_armStrengthReduction_1->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReduction(1)), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReductionImportanceRef(1)) );

  junc_feedIn_supportTM = ER::Junction::relocate(resource);
  // junc_feedIn_supportTM->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

  junc_feedIn_supportVelocity = ER::Junction::relocate(resource);
  // junc_feedIn_supportVelocity->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportVelocity()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportVelocityImportanceRef()) );

  junc_feedIn_balanceAmount = ER::Junction::relocate(resource);
  // junc_feedIn_balanceAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );

  junc_feedIn_maxLegCollidingTime = ER::Junction::relocate(resource);
  // junc_feedIn_maxLegCollidingTime->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getMaxLegCollidingTime()), (float*)&(module->owner->bodyFrame->feedIn->getMaxLegCollidingTimeImportanceRef()) );

  junc_feedIn_timeSinceLastStep = ER::Junction::relocate(resource);
  // junc_feedIn_timeSinceLastStep->getEdges()[0].reinit( (char*)&(module->owner->balanceManagement->steppingBalance->feedOut->getTimeSinceLastStep()), (float*)&(module->owner->balanceManagement->steppingBalance->feedOut->getTimeSinceLastStepImportanceRef()) );

  junc_feedIn_stepCount = ER::Junction::relocate(resource);
  // junc_feedIn_stepCount->getEdges()[0].reinit( (char*)&(module->owner->balanceManagement->steppingBalance->feedOut->getStepCount()), (float*)&(module->owner->balanceManagement->steppingBalance->feedOut->getStepCountImportanceRef()) );

  junc_feedIn_steppingTime = ER::Junction::relocate(resource);
  // junc_feedIn_steppingTime->getEdges()[0].reinit( (char*)&(module->owner->balanceManagement->steppingBalance->feedOut->getSteppingTime()), (float*)&(module->owner->balanceManagement->steppingBalance->feedOut->getSteppingTimeImportanceRef()) );

  junc_feedIn_legStepFractions_0 = ER::Junction::relocate(resource);
  // junc_feedIn_legStepFractions_0->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->step->feedOut->getStepFraction()), (float*)&(module->owner->legs[0]->step->feedOut->getStepFractionImportanceRef()) );
  junc_feedIn_legStepFractions_1 = ER::Junction::relocate(resource);
  // junc_feedIn_legStepFractions_1->getEdges()[0].reinit( (char*)&(module->owner->legs[1]->step->feedOut->getStepFraction()), (float*)&(module->owner->legs[1]->step->feedOut->getStepFractionImportanceRef()) );

  junc_feedIn_CoMBodyState = ER::Junction::relocate(resource);
  // junc_feedIn_CoMBodyState->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getCentreOfMassBodyState()), (float*)&(module->owner->bodyFrame->feedOut->getCentreOfMassBodyStateImportanceRef()) );

  junc_out_averageArmEndRelativeToRoot = ER::Junction::relocate(resource);
  // junc_out_averageArmEndRelativeToRoot->getEdges()[0].reinit( (char*)&(module->out->getArmBalancePoseEndRelativeToRoot(0)), (float*)&(module->out->getArmBalancePoseEndRelativeToRootImportanceRef(0)) );
  // junc_out_averageArmEndRelativeToRoot->getEdges()[1].reinit( (char*)&(module->out->getArmBalancePoseEndRelativeToRoot(1)), (float*)&(module->out->getArmBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_out_averageLegEndRelativeToRoot = ER::Junction::relocate(resource);
  // junc_out_averageLegEndRelativeToRoot->getEdges()[0].reinit( (char*)&(module->out->getLegBalancePoseEndRelativeToRoot(0)), (float*)&(module->out->getLegBalancePoseEndRelativeToRootImportanceRef(0)) );
  // junc_out_averageLegEndRelativeToRoot->getEdges()[1].reinit( (char*)&(module->out->getLegBalancePoseEndRelativeToRoot(1)), (float*)&(module->out->getLegBalancePoseEndRelativeToRootImportanceRef(1)) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BalanceBehaviourInterface* module = (BalanceBehaviourInterface*)modulePtr;
  BalanceBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportTMImportance = junc_feedIn_supportTM->combineDirectInput(&feedIn.supportTM);
  feedIn.m_supportVelocityImportance = junc_feedIn_supportVelocity->combineDirectInput(&feedIn.supportVelocity);
  feedIn.m_balanceAmountImportance = junc_feedIn_balanceAmount->combineDirectInput(&feedIn.balanceAmount);
  feedIn.m_maxLegCollidingTimeImportance = junc_feedIn_maxLegCollidingTime->combineDirectInput(&feedIn.maxLegCollidingTime);
  feedIn.m_timeSinceLastStepImportance = junc_feedIn_timeSinceLastStep->combineDirectInput(&feedIn.timeSinceLastStep);
  feedIn.m_stepCountImportance = junc_feedIn_stepCount->combineDirectInput(&feedIn.stepCount);
  feedIn.m_steppingTimeImportance = junc_feedIn_steppingTime->combineDirectInput(&feedIn.steppingTime);
  feedIn.m_legStepFractionsImportance[0] = junc_feedIn_legStepFractions_0->combineDirectInput(&feedIn.legStepFractions[0]);
  feedIn.m_legStepFractionsImportance[1] = junc_feedIn_legStepFractions_1->combineDirectInput(&feedIn.legStepFractions[1]);
  feedIn.m_CoMBodyStateImportance = junc_feedIn_CoMBodyState->combineDirectInput(&feedIn.CoMBodyState);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceBehaviourInterface_Con::combineInputs(ER::Module* modulePtr)
{
  BalanceBehaviourInterface* module = (BalanceBehaviourInterface*)modulePtr;
  BalanceBehaviourInterfaceInputs& in = *module->in;

  // Junction assignments.
  in.m_legStrengthReductionImportance[0] = junc_in_legStrengthReduction_0->combineDirectInput(&in.legStrengthReduction[0]);
  in.m_legStrengthReductionImportance[1] = junc_in_legStrengthReduction_1->combineDirectInput(&in.legStrengthReduction[1]);
  in.m_armStrengthReductionImportance[0] = junc_in_armStrengthReduction_0->combineDirectInput(&in.armStrengthReduction[0]);
  in.m_armStrengthReductionImportance[1] = junc_in_armStrengthReduction_1->combineDirectInput(&in.armStrengthReduction[1]);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceBehaviourInterface_Con::combineOutputs(ER::Module* modulePtr)
{
  BalanceBehaviourInterface* module = (BalanceBehaviourInterface*)modulePtr;
  BalanceBehaviourInterfaceOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_averageBalancePoseArmEndRelativeToRootImportance == 0.0f);
  out.m_averageBalancePoseArmEndRelativeToRootImportance = junc_out_averageArmEndRelativeToRoot->combineAverage(&out.averageBalancePoseArmEndRelativeToRoot);
  PostCombiners::Matrix34Orthonormalise(out.averageBalancePoseArmEndRelativeToRoot);
  NMP_ASSERT(out.m_averageBalancePoseLegEndRelativeToRootImportance == 0.0f);
  out.m_averageBalancePoseLegEndRelativeToRootImportance = junc_out_averageLegEndRelativeToRoot->combineAverage(&out.averageBalancePoseLegEndRelativeToRoot);
  PostCombiners::Matrix34Orthonormalise(out.averageBalancePoseLegEndRelativeToRoot);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


