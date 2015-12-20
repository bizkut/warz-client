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
#include "Arm.h"
#include "ArmPackaging.h"
#include "ArmSpin.h"
#include "ArmSwing.h"
#include "ArmBrace.h"
#include "ArmPlacement.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "ArmStandingSupport.h"
#include "ArmSittingStep.h"
#include "ArmPose.h"
#include "ArmAim.h"
#include "ArmHoldingSupport.h"
#include "ArmStep.h"
#include "ArmReachForWorld.h"
#include "ArmReachForBodyPart.h"
#include "ArmReachReaction.h"
#include "ArmWrithe.h"
#include "CharacteristicsBehaviourInterface.h"
#include "ArmHold.h"
#include "ArmGrab.h"
#include "BodySection.h"
#include "BraceChooser.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void Arm_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((Arm*)module);
}

//----------------------------------------------------------------------------------------------------------------------
Arm_Con::~Arm_Con()
{
  junc_in_isBodyBarFeetContacting->destroy();
  junc_in_shouldBrace->destroy();
  junc_out_holdBreakageOpportunity->destroy();
  junc_out_endConstraint->destroy();
  junc_in_centreOfShoulders->destroy();
  junc_in_limbDampingRatioReduction->destroy();
  junc_in_limbControlCompensationReduction->destroy();
  junc_in_limbStiffnessReduction->destroy();
  junc_feedOut_supportedByConstraint->destroy();
  junc_out_wristLimitReduction->destroy();
  junc_out_control->destroy();
  junc_wtaSittingStepStandingSupport->destroy();
  junc_wtaBracePlacementSitting->destroy();
  junc_wta01->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Arm_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(Arm_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(12);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
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
void Arm_Con::buildConnections(Arm* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Arm_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_wta01 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->spin->out->getControl()), (float*)&(module->spin->out->getControlImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->swing->out->getControl()), (float*)&(module->swing->out->getControlImportanceRef()) );

  junc_wtaBracePlacementSitting = activeJnc = ER::Junction::init(resource, 3, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->brace->out->getControl()), (float*)&(module->brace->out->getControlImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->placement->out->getControl()), (float*)&(module->placement->out->getControlImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->bodyFrame->sittingBodyBalance->out->getArmControl(module->getChildIndex())), (float*)&(module->owner->bodyFrame->sittingBodyBalance->out->getArmControlImportanceRef(module->getChildIndex())) );

  junc_wtaSittingStepStandingSupport = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->standingSupport->out->getControl()), (float*)&(module->standingSupport->out->getControlImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->sittingStep->out->getControl()), (float*)&(module->sittingStep->out->getControlImportanceRef()) );

  junc_out_control = activeJnc = ER::Junction::init(resource, 12, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->aim->out->getControl()), (float*)&(module->aim->out->getControlImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->out->getWinnerTakesAll()), (float*)&(module->out->getWinnerTakesAllImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->out->getWtaBracePlacementSitting()), (float*)&(module->out->getWtaBracePlacementSittingImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->out->getWtaSittingStepStandingSupport()), (float*)&(module->out->getWtaSittingStepStandingSupportImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->holdingSupport->out->getControl()), (float*)&(module->holdingSupport->out->getControlImportanceRef()) );
  activeJnc->getEdges()[6].reinit( (char*)&(module->step->out->getControl()), (float*)&(module->step->out->getControlImportanceRef()) );
  activeJnc->getEdges()[7].reinit( (char*)&(module->reachForWorld->out->getControl()), (float*)&(module->reachForWorld->out->getControlImportanceRef()) );
  activeJnc->getEdges()[8].reinit( (char*)&(module->reachForBodyPart->out->getControl()), (float*)&(module->reachForBodyPart->out->getControlImportanceRef()) );
  activeJnc->getEdges()[9].reinit( (char*)&(module->reachReaction->out->getControl()), (float*)&(module->reachReaction->out->getControlImportanceRef()) );
  activeJnc->getEdges()[10].reinit( (char*)&(module->writhe->out->getControl()), (float*)&(module->writhe->out->getControlImportanceRef()) );
  activeJnc->getEdges()[11].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_out_wristLimitReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->placement->out->getWristLimitReduction()), (float*)&(module->placement->out->getWristLimitReductionImportanceRef()) );

  junc_feedOut_supportedByConstraint = activeJnc = ER::Junction::init(resource, 2, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->holdingSupport->feedOut->getSupportedByConstraint()), (float*)&(module->holdingSupport->feedOut->getSupportedByConstraintImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->standingSupport->feedOut->getSupportedByConstraint()), (float*)&(module->standingSupport->feedOut->getSupportedByConstraintImportanceRef()) );

  junc_in_limbStiffnessReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_centreOfShoulders = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getAverageShoulders()), (float*)&(module->owner->bodyFrame->feedIn->getAverageShouldersImportanceRef()) );

  junc_out_endConstraint = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->hold->out->getEndConstraint()), (float*)&(module->hold->out->getEndConstraintImportanceRef()) );

  junc_out_holdBreakageOpportunity = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->grab->out->getBreakageOpportunity()), (float*)&(module->grab->out->getBreakageOpportunityImportanceRef()) );

  junc_in_shouldBrace = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->upperBody->braceChooser->out->getShouldBrace(module->getChildIndex())), (float*)&(module->owner->upperBody->braceChooser->out->getShouldBraceImportanceRef(module->getChildIndex())) );

  junc_in_isBodyBarFeetContacting = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContacting()), (float*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContactingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void Arm_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Arm_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_wta01 = ER::Junction::relocate(resource);
  // junc_wta01->getEdges()[0].reinit( (char*)&(module->spin->out->getControl()), (float*)&(module->spin->out->getControlImportanceRef()) );
  // junc_wta01->getEdges()[1].reinit( (char*)&(module->swing->out->getControl()), (float*)&(module->swing->out->getControlImportanceRef()) );

  junc_wtaBracePlacementSitting = ER::Junction::relocate(resource);
  // junc_wtaBracePlacementSitting->getEdges()[0].reinit( (char*)&(module->brace->out->getControl()), (float*)&(module->brace->out->getControlImportanceRef()) );
  // junc_wtaBracePlacementSitting->getEdges()[1].reinit( (char*)&(module->placement->out->getControl()), (float*)&(module->placement->out->getControlImportanceRef()) );
  // junc_wtaBracePlacementSitting->getEdges()[2].reinit( (char*)&(module->owner->bodyFrame->sittingBodyBalance->out->getArmControl(module->getChildIndex())), (float*)&(module->owner->bodyFrame->sittingBodyBalance->out->getArmControlImportanceRef(module->getChildIndex())) );

  junc_wtaSittingStepStandingSupport = ER::Junction::relocate(resource);
  // junc_wtaSittingStepStandingSupport->getEdges()[0].reinit( (char*)&(module->standingSupport->out->getControl()), (float*)&(module->standingSupport->out->getControlImportanceRef()) );
  // junc_wtaSittingStepStandingSupport->getEdges()[1].reinit( (char*)&(module->sittingStep->out->getControl()), (float*)&(module->sittingStep->out->getControlImportanceRef()) );

  junc_out_control = ER::Junction::relocate(resource);
  // junc_out_control->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  // junc_out_control->getEdges()[1].reinit( (char*)&(module->aim->out->getControl()), (float*)&(module->aim->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[2].reinit( (char*)&(module->out->getWinnerTakesAll()), (float*)&(module->out->getWinnerTakesAllImportanceRef()) );
  // junc_out_control->getEdges()[3].reinit( (char*)&(module->out->getWtaBracePlacementSitting()), (float*)&(module->out->getWtaBracePlacementSittingImportanceRef()) );
  // junc_out_control->getEdges()[4].reinit( (char*)&(module->out->getWtaSittingStepStandingSupport()), (float*)&(module->out->getWtaSittingStepStandingSupportImportanceRef()) );
  // junc_out_control->getEdges()[5].reinit( (char*)&(module->holdingSupport->out->getControl()), (float*)&(module->holdingSupport->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[6].reinit( (char*)&(module->step->out->getControl()), (float*)&(module->step->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[7].reinit( (char*)&(module->reachForWorld->out->getControl()), (float*)&(module->reachForWorld->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[8].reinit( (char*)&(module->reachForBodyPart->out->getControl()), (float*)&(module->reachForBodyPart->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[9].reinit( (char*)&(module->reachReaction->out->getControl()), (float*)&(module->reachReaction->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[10].reinit( (char*)&(module->writhe->out->getControl()), (float*)&(module->writhe->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[11].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_out_wristLimitReduction = ER::Junction::relocate(resource);
  // junc_out_wristLimitReduction->getEdges()[0].reinit( (char*)&(module->placement->out->getWristLimitReduction()), (float*)&(module->placement->out->getWristLimitReductionImportanceRef()) );

  junc_feedOut_supportedByConstraint = ER::Junction::relocate(resource);
  // junc_feedOut_supportedByConstraint->getEdges()[0].reinit( (char*)&(module->holdingSupport->feedOut->getSupportedByConstraint()), (float*)&(module->holdingSupport->feedOut->getSupportedByConstraintImportanceRef()) );
  // junc_feedOut_supportedByConstraint->getEdges()[1].reinit( (char*)&(module->standingSupport->feedOut->getSupportedByConstraint()), (float*)&(module->standingSupport->feedOut->getSupportedByConstraintImportanceRef()) );

  junc_in_limbStiffnessReduction = ER::Junction::relocate(resource);
  // junc_in_limbStiffnessReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = ER::Junction::relocate(resource);
  // junc_in_limbControlCompensationReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = ER::Junction::relocate(resource);
  // junc_in_limbDampingRatioReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getArmDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getArmDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_centreOfShoulders = ER::Junction::relocate(resource);
  // junc_in_centreOfShoulders->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getAverageShoulders()), (float*)&(module->owner->bodyFrame->feedIn->getAverageShouldersImportanceRef()) );

  junc_out_endConstraint = ER::Junction::relocate(resource);
  // junc_out_endConstraint->getEdges()[0].reinit( (char*)&(module->hold->out->getEndConstraint()), (float*)&(module->hold->out->getEndConstraintImportanceRef()) );

  junc_out_holdBreakageOpportunity = ER::Junction::relocate(resource);
  // junc_out_holdBreakageOpportunity->getEdges()[0].reinit( (char*)&(module->grab->out->getBreakageOpportunity()), (float*)&(module->grab->out->getBreakageOpportunityImportanceRef()) );

  junc_in_shouldBrace = ER::Junction::relocate(resource);
  // junc_in_shouldBrace->getEdges()[0].reinit( (char*)&(module->owner->upperBody->braceChooser->out->getShouldBrace(module->getChildIndex())), (float*)&(module->owner->upperBody->braceChooser->out->getShouldBraceImportanceRef(module->getChildIndex())) );

  junc_in_isBodyBarFeetContacting = ER::Junction::relocate(resource);
  // junc_in_isBodyBarFeetContacting->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContacting()), (float*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContactingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Arm_Con::combineInputs(ER::Module* modulePtr)
{
  Arm* module = (Arm*)modulePtr;
  ArmInputs& in = *module->in;

  // Junction assignments.
  in.m_limbStiffnessReductionImportance = junc_in_limbStiffnessReduction->combineDirectInput(&in.limbStiffnessReduction);
  in.m_limbControlCompensationReductionImportance = junc_in_limbControlCompensationReduction->combineDirectInput(&in.limbControlCompensationReduction);
  in.m_limbDampingRatioReductionImportance = junc_in_limbDampingRatioReduction->combineDirectInput(&in.limbDampingRatioReduction);
  in.m_centreOfShouldersImportance = junc_in_centreOfShoulders->combineDirectInput(&in.centreOfShoulders);
  in.m_shouldBraceImportance = junc_in_shouldBrace->combineDirectInput(&in.shouldBrace);
  in.m_isBodyBarFeetContactingImportance = junc_in_isBodyBarFeetContacting->combineDirectInput(&in.isBodyBarFeetContacting);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Arm_Con::combineFeedbackOutputs(ER::Module* modulePtr)
{
  Arm* module = (Arm*)modulePtr;
  ArmFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_supportedByConstraintImportance == 0.0f);
  feedOut.m_supportedByConstraintImportance = junc_feedOut_supportedByConstraint->combineIsBoolSet(&feedOut.supportedByConstraint);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Arm_Con::combineOutputs(ER::Module* modulePtr)
{
  Arm* module = (Arm*)modulePtr;
  ArmOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_winnerTakesAllImportance == 0.0f);
  out.m_winnerTakesAllImportance = junc_wta01->combineWinnerTakesAll(&out.winnerTakesAll);
  NMP_ASSERT(out.m_wtaBracePlacementSittingImportance == 0.0f);
  out.m_wtaBracePlacementSittingImportance = junc_wtaBracePlacementSitting->combineWinnerTakesAll(&out.wtaBracePlacementSitting);
  NMP_ASSERT(out.m_wtaSittingStepStandingSupportImportance == 0.0f);
  out.m_wtaSittingStepStandingSupportImportance = junc_wtaSittingStepStandingSupport->combineWinnerTakesAll(&out.wtaSittingStepStandingSupport);
  NMP_ASSERT(out.m_controlImportance == 0.0f);
  out.m_controlImportance = junc_out_control->combinePriority(&out.control);
  NMP_ASSERT(out.m_wristLimitReductionImportance == 0.0f);
  out.m_wristLimitReductionImportance = junc_out_wristLimitReduction->combineDirectInput(&out.wristLimitReduction);
  NMP_ASSERT(out.m_endConstraintImportance == 0.0f);
  out.m_endConstraintImportance = junc_out_endConstraint->combineDirectInput(&out.endConstraint);
  NMP_ASSERT(out.m_holdBreakageOpportunityImportance == 0.0f);
  out.m_holdBreakageOpportunityImportance = junc_out_holdBreakageOpportunity->combineDirectInput(&out.holdBreakageOpportunity);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


