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
#include "Spine.h"
#include "SpinePackaging.h"
#include "SpinePose.h"
#include "SpineControl.h"
#include "SpineAim.h"
#include "SpineReachReaction.h"
#include "SpineWrithe.h"
#include "BodySection.h"
#include "HazardManagement.h"
#include "ShieldManagement.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "CharacteristicsBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void Spine_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((Spine*)module);
}

//----------------------------------------------------------------------------------------------------------------------
Spine_Con::~Spine_Con()
{
  junc_in_defaultPoseWeight->destroy();
  junc_in_chestControlledAmount->destroy();
  junc_in_pelvisControlledAmount->destroy();
  junc_in_limbDampingRatioReduction->destroy();
  junc_in_limbControlCompensationReduction->destroy();
  junc_in_limbStiffnessReduction->destroy();
  junc_in_isStepping->destroy();
  junc_in_rootTranslationRequest->destroy();
  junc_in_endTranslationRequest->destroy();
  junc_in_rootRotationRequest->destroy();
  junc_in_endRotationRequestPri->destroy();
  junc_out_control->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Spine_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(Spine_Con), 16);

  result += ER::Junction::getMemoryRequirements(6);
  result += ER::Junction::getMemoryRequirements(2);
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
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void Spine_Con::buildConnections(Spine* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Spine_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_out_control = activeJnc = ER::Junction::init(resource, 6, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->control->out->getControl()), (float*)&(module->control->out->getControlImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->aim->out->getControl()), (float*)&(module->aim->out->getControlImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->reachReaction->out->getControl()), (float*)&(module->reachReaction->out->getControlImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->writhe->out->getControl()), (float*)&(module->writhe->out->getControlImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_in_endRotationRequestPri = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->upperBody->out->getOneWayRotationRequest()), (float*)&(module->owner->upperBody->out->getOneWayRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->out->getSpineRotationRequest(module->getChildIndex())), (float*)&(module->owner->hazardManagement->shieldManagement->out->getSpineRotationRequestImportanceRef(module->getChildIndex())) );

  junc_in_rootRotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->out->getOneWayRotationRequest()), (float*)&(module->owner->lowerBody->out->getOneWayRotationRequestImportanceRef()) );

  junc_in_endTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->upperBody->out->getOneWayTranslationRequest()), (float*)&(module->owner->upperBody->out->getOneWayTranslationRequestImportanceRef()) );

  junc_in_rootTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->out->getOneWayTranslationRequest()), (float*)&(module->owner->lowerBody->out->getOneWayTranslationRequestImportanceRef()) );

  junc_in_isStepping = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsStepping()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsSteppingImportanceRef()) );

  junc_in_limbStiffnessReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getSpineStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getSpineStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getSpineControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getSpineControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getSpineDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getSpineDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_pelvisControlledAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->feedIn->getControlledAmount()), (float*)&(module->owner->lowerBody->feedIn->getControlledAmountImportanceRef()) );

  junc_in_chestControlledAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->upperBody->feedIn->getControlledAmount()), (float*)&(module->owner->upperBody->feedIn->getControlledAmountImportanceRef()) );

  junc_in_defaultPoseWeight = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->idleAwakeBehaviourInterface->out->getUseSpineDefaultPose()), (float*)&(module->owner->idleAwakeBehaviourInterface->out->getUseSpineDefaultPoseImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void Spine_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Spine_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_out_control = ER::Junction::relocate(resource);
  // junc_out_control->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  // junc_out_control->getEdges()[1].reinit( (char*)&(module->control->out->getControl()), (float*)&(module->control->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[2].reinit( (char*)&(module->aim->out->getControl()), (float*)&(module->aim->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[3].reinit( (char*)&(module->reachReaction->out->getControl()), (float*)&(module->reachReaction->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[4].reinit( (char*)&(module->writhe->out->getControl()), (float*)&(module->writhe->out->getControlImportanceRef()) );
  // junc_out_control->getEdges()[5].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_in_endRotationRequestPri = ER::Junction::relocate(resource);
  // junc_in_endRotationRequestPri->getEdges()[0].reinit( (char*)&(module->owner->upperBody->out->getOneWayRotationRequest()), (float*)&(module->owner->upperBody->out->getOneWayRotationRequestImportanceRef()) );
  // junc_in_endRotationRequestPri->getEdges()[1].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->out->getSpineRotationRequest(module->getChildIndex())), (float*)&(module->owner->hazardManagement->shieldManagement->out->getSpineRotationRequestImportanceRef(module->getChildIndex())) );

  junc_in_rootRotationRequest = ER::Junction::relocate(resource);
  // junc_in_rootRotationRequest->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->out->getOneWayRotationRequest()), (float*)&(module->owner->lowerBody->out->getOneWayRotationRequestImportanceRef()) );

  junc_in_endTranslationRequest = ER::Junction::relocate(resource);
  // junc_in_endTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->upperBody->out->getOneWayTranslationRequest()), (float*)&(module->owner->upperBody->out->getOneWayTranslationRequestImportanceRef()) );

  junc_in_rootTranslationRequest = ER::Junction::relocate(resource);
  // junc_in_rootTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->out->getOneWayTranslationRequest()), (float*)&(module->owner->lowerBody->out->getOneWayTranslationRequestImportanceRef()) );

  junc_in_isStepping = ER::Junction::relocate(resource);
  // junc_in_isStepping->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsStepping()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsSteppingImportanceRef()) );

  junc_in_limbStiffnessReduction = ER::Junction::relocate(resource);
  // junc_in_limbStiffnessReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getSpineStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getSpineStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = ER::Junction::relocate(resource);
  // junc_in_limbControlCompensationReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getSpineControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getSpineControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = ER::Junction::relocate(resource);
  // junc_in_limbDampingRatioReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getSpineDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getSpineDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_pelvisControlledAmount = ER::Junction::relocate(resource);
  // junc_in_pelvisControlledAmount->getEdges()[0].reinit( (char*)&(module->owner->lowerBody->feedIn->getControlledAmount()), (float*)&(module->owner->lowerBody->feedIn->getControlledAmountImportanceRef()) );

  junc_in_chestControlledAmount = ER::Junction::relocate(resource);
  // junc_in_chestControlledAmount->getEdges()[0].reinit( (char*)&(module->owner->upperBody->feedIn->getControlledAmount()), (float*)&(module->owner->upperBody->feedIn->getControlledAmountImportanceRef()) );

  junc_in_defaultPoseWeight = ER::Junction::relocate(resource);
  // junc_in_defaultPoseWeight->getEdges()[0].reinit( (char*)&(module->owner->idleAwakeBehaviourInterface->out->getUseSpineDefaultPose()), (float*)&(module->owner->idleAwakeBehaviourInterface->out->getUseSpineDefaultPoseImportanceRef()) );
  // junc_in_defaultPoseWeight->getEdges()[1].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Spine_Con::combineInputs(ER::Module* modulePtr)
{
  Spine* module = (Spine*)modulePtr;
  SpineInputs& in = *module->in;

  // Junction assignments.
  in.m_endRotationRequestImportance = junc_in_endRotationRequestPri->combinePriority(&in.endRotationRequest);
  in.m_rootRotationRequestImportance = junc_in_rootRotationRequest->combineDirectInput(&in.rootRotationRequest);
  in.m_endTranslationRequestImportance = junc_in_endTranslationRequest->combineDirectInput(&in.endTranslationRequest);
  in.m_rootTranslationRequestImportance = junc_in_rootTranslationRequest->combineDirectInput(&in.rootTranslationRequest);
  in.m_isSteppingImportance = junc_in_isStepping->combineDirectInput(&in.isStepping);
  in.m_limbStiffnessReductionImportance = junc_in_limbStiffnessReduction->combineDirectInput(&in.limbStiffnessReduction);
  in.m_limbControlCompensationReductionImportance = junc_in_limbControlCompensationReduction->combineDirectInput(&in.limbControlCompensationReduction);
  in.m_limbDampingRatioReductionImportance = junc_in_limbDampingRatioReduction->combineDirectInput(&in.limbDampingRatioReduction);
  in.m_pelvisControlledAmountImportance = junc_in_pelvisControlledAmount->combineDirectInput(&in.pelvisControlledAmount);
  in.m_chestControlledAmountImportance = junc_in_chestControlledAmount->combineDirectInput(&in.chestControlledAmount);
  in.m_defaultPoseWeightImportance = junc_in_defaultPoseWeight->combineWinnerTakesAll(&in.defaultPoseWeight);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Spine_Con::combineOutputs(ER::Module* modulePtr)
{
  Spine* module = (Spine*)modulePtr;
  SpineOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_controlImportance == 0.0f);
  out.m_controlImportance = junc_out_control->combinePriority(&out.control);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


