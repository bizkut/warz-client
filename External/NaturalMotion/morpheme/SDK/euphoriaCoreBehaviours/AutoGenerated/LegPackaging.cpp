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
#include "Leg.h"
#include "LegPackaging.h"
#include "LegSpin.h"
#include "LegSwing.h"
#include "LegPose.h"
#include "LegBrace.h"
#include "LegSittingSupport.h"
#include "LegStandingSupport.h"
#include "LegStep.h"
#include "LegReachReaction.h"
#include "LegWrithe.h"
#include "CharacteristicsBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "BraceChooser.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void Leg_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((Leg*)module);
}

//----------------------------------------------------------------------------------------------------------------------
Leg_Con::~Leg_Con()
{
  junc_in_isBodyBarFeetContacting->destroy();
  junc_in_shouldBrace->destroy();
  junc_in_centreOfSupport->destroy();
  junc_in_centreOfHips->destroy();
  junc_in_limbDampingRatioReduction->destroy();
  junc_in_limbControlCompensationReduction->destroy();
  junc_in_limbStiffnessReduction->destroy();
  junc_feedOut_supportedByConstraint->destroy();
  junc_out_useFullAnkleRange->destroy();
  junc_out_control->destroy();
  junc_wta01->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Leg_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(Leg_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(9);
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
void Leg_Con::buildConnections(Leg* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Leg_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_wta01 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->spin->out->getControl()), (float*)&(module->spin->out->getControlImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->swing->out->getControl()), (float*)&(module->swing->out->getControlImportanceRef()) );

  junc_out_control = activeJnc = ER::Junction::init(resource, 9, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->out->getWinnerTakesAll()), (float*)&(module->out->getWinnerTakesAllImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->brace->out->getControl()), (float*)&(module->brace->out->getControlImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->sittingSupport->out->getControl()), (float*)&(module->sittingSupport->out->getControlImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->standingSupport->out->getControl()), (float*)&(module->standingSupport->out->getControlImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->step->out->getControl()), (float*)&(module->step->out->getControlImportanceRef()) );
  activeJnc->getEdges()[6].reinit( (char*)&(module->reachReaction->out->getControl()), (float*)&(module->reachReaction->out->getControlImportanceRef()) );
  activeJnc->getEdges()[7].reinit( (char*)&(module->writhe->out->getControl()), (float*)&(module->writhe->out->getControlImportanceRef()) );
  activeJnc->getEdges()[8].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_out_useFullAnkleRange = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->step->out->getUseFullEndJointRange()), (float*)&(module->step->out->getUseFullEndJointRangeImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->standingSupport->out->getUseFullEndJointRange()), (float*)&(module->standingSupport->out->getUseFullEndJointRangeImportanceRef()) );

  junc_feedOut_supportedByConstraint = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->standingSupport->feedOut->getSupportedByConstraint()), (float*)&(module->standingSupport->feedOut->getSupportedByConstraintImportanceRef()) );

  junc_in_limbStiffnessReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_centreOfHips = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getAverageHips()), (float*)&(module->owner->bodyFrame->feedIn->getAverageHipsImportanceRef()) );

  junc_in_centreOfSupport = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getAverageOfSupportingFeet()), (float*)&(module->owner->bodyFrame->feedIn->getAverageOfSupportingFeetImportanceRef()) );

  junc_in_shouldBrace = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->braceChooser->out->getShouldBrace(module->getChildIndex())), (float*)&(module->owner->lowerBody->braceChooser->out->getShouldBraceImportanceRef(module->getChildIndex())) );

  junc_in_isBodyBarFeetContacting = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContacting()), (float*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContactingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void Leg_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Leg_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_wta01 = ER::Junction::relocate(resource);
  // junc_wta01->getEdges()[0].reinit( (char*)&(module->spin->out->getControl()), (float*)&(module->spin->out->getControlImportanceRef()) );
  // junc_wta01->getEdges()[1].reinit( (char*)&(module->swing->out->getControl()), (float*)&(module->swing->out->getControlImportanceRef()) );

  junc_out_control = ER::Junction::relocate(resource);
  // junc_out_control->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  // junc_out_control->getEdges()[1].reinit( (char*)&(module->out->getWinnerTakesAll()), (float*)&(module->out->getWinnerTakesAllImportanceRef()) );
  // junc_out_control->getEdges()[2].reinit( (char*)&(module->brace->out->getControl()), (float*)&(module->brace->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[3].reinit( (char*)&(module->sittingSupport->out->getControl()), (float*)&(module->sittingSupport->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[4].reinit( (char*)&(module->standingSupport->out->getControl()), (float*)&(module->standingSupport->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[5].reinit( (char*)&(module->step->out->getControl()), (float*)&(module->step->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[6].reinit( (char*)&(module->reachReaction->out->getControl()), (float*)&(module->reachReaction->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[7].reinit( (char*)&(module->writhe->out->getControl()), (float*)&(module->writhe->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[8].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_out_useFullAnkleRange = ER::Junction::relocate(resource);
  // junc_out_useFullAnkleRange->getEdges()[0].reinit( (char*)&(module->step->out->getUseFullEndJointRange()), (float*)&(module->step->out->getUseFullEndJointRangeImportanceRef()) );
  // junc_out_useFullAnkleRange->getEdges()[1].reinit( (char*)&(module->standingSupport->out->getUseFullEndJointRange()), (float*)&(module->standingSupport->out->getUseFullEndJointRangeImportanceRef()) );

  junc_feedOut_supportedByConstraint = ER::Junction::relocate(resource);
  // junc_feedOut_supportedByConstraint->getEdges()[0].reinit( (char*)&(module->standingSupport->feedOut->getSupportedByConstraint()), (float*)&(module->standingSupport->feedOut->getSupportedByConstraintImportanceRef()) );

  junc_in_limbStiffnessReduction = ER::Junction::relocate(resource);
  // junc_in_limbStiffnessReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = ER::Junction::relocate(resource);
  // junc_in_limbControlCompensationReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = ER::Junction::relocate(resource);
  // junc_in_limbDampingRatioReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getLegDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getLegDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_centreOfHips = ER::Junction::relocate(resource);
  // junc_in_centreOfHips->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getAverageHips()), (float*)&(module->owner->bodyFrame->feedIn->getAverageHipsImportanceRef()) );

  junc_in_centreOfSupport = ER::Junction::relocate(resource);
  // junc_in_centreOfSupport->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getAverageOfSupportingFeet()), (float*)&(module->owner->bodyFrame->feedIn->getAverageOfSupportingFeetImportanceRef()) );

  junc_in_shouldBrace = ER::Junction::relocate(resource);
  // junc_in_shouldBrace->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->braceChooser->out->getShouldBrace(module->getChildIndex())), (float*)&(module->owner->lowerBody->braceChooser->out->getShouldBraceImportanceRef(module->getChildIndex())) );

  junc_in_isBodyBarFeetContacting = ER::Junction::relocate(resource);
  // junc_in_isBodyBarFeetContacting->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContacting()), (float*)&(module->owner->bodyFrame->feedIn->getIsBodyBarFeetContactingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Leg_Con::combineInputs(ER::Module* modulePtr)
{
  Leg* module = (Leg*)modulePtr;
  LegInputs& in = *module->in;

  // Junction assignments.
  in.m_limbStiffnessReductionImportance = junc_in_limbStiffnessReduction->combineDirectInput(&in.limbStiffnessReduction);
  in.m_limbControlCompensationReductionImportance = junc_in_limbControlCompensationReduction->combineDirectInput(&in.limbControlCompensationReduction);
  in.m_limbDampingRatioReductionImportance = junc_in_limbDampingRatioReduction->combineDirectInput(&in.limbDampingRatioReduction);
  in.m_centreOfHipsImportance = junc_in_centreOfHips->combineDirectInput(&in.centreOfHips);
  in.m_centreOfSupportImportance = junc_in_centreOfSupport->combineDirectInput(&in.centreOfSupport);
  in.m_shouldBraceImportance = junc_in_shouldBrace->combineDirectInput(&in.shouldBrace);
  in.m_isBodyBarFeetContactingImportance = junc_in_isBodyBarFeetContacting->combineDirectInput(&in.isBodyBarFeetContacting);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Leg_Con::combineFeedbackOutputs(ER::Module* modulePtr)
{
  Leg* module = (Leg*)modulePtr;
  LegFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_supportedByConstraintImportance == 0.0f);
  feedOut.m_supportedByConstraintImportance = junc_feedOut_supportedByConstraint->combineDirectInput(&feedOut.supportedByConstraint);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Leg_Con::combineOutputs(ER::Module* modulePtr)
{
  Leg* module = (Leg*)modulePtr;
  LegOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_winnerTakesAllImportance == 0.0f);
  out.m_winnerTakesAllImportance = junc_wta01->combineWinnerTakesAll(&out.winnerTakesAll);
  NMP_ASSERT(out.m_controlImportance == 0.0f);
  out.m_controlImportance = junc_out_control->combinePriority(&out.control);
  NMP_ASSERT(out.m_useFullAnkleRangeImportance == 0.0f);
  out.m_useFullAnkleRangeImportance = junc_out_useFullAnkleRange->combineWinnerTakesAll(&out.useFullAnkleRange);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


