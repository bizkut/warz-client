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
#include "SittingBodyBalance.h"
#include "SittingBodyBalancePackaging.h"
#include "MyNetwork.h"
#include "SitBehaviourInterface.h"
#include "SupportPolygon.h"
#include "Spine.h"
#include "Arm.h"
#include "BodyBalance.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SittingBodyBalance*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SittingBodyBalance_Con::~SittingBodyBalance_Con()
{
  junc_feedIn_standingBalanceAmount->destroy();
  junc_feedIn_handOnGroundCollidingTime_1->destroy();
  junc_feedIn_handOnGroundCollidingTime_0->destroy();
  junc_feedIn_handOnGroundAmount_1->destroy();
  junc_feedIn_handOnGroundAmount_0->destroy();
  junc_feedIn_pelvisOnGroundAmount->destroy();
  junc_feedIn_chestOnGroundAmount->destroy();
  junc_in_supportWithLegs->destroy();
  junc_in_supportWithArms->destroy();
  junc_in_supportPolygon->destroy();
  junc_in_sitParameters->destroy();
  junc_in_headPoseEndRelativeToRoot_0->destroy();
  junc_in_spinePoseEndRelativeToRoot_0->destroy();
  junc_in_legPoseEndRelativeToRoot_1->destroy();
  junc_in_legPoseEndRelativeToRoot_0->destroy();
  junc_in_armPoseEndRelativeToRoot_1->destroy();
  junc_in_armPoseEndRelativeToRoot_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SittingBodyBalance_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SittingBodyBalance_Con), 16);

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
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance_Con::buildConnections(SittingBodyBalance* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SittingBodyBalance_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_armPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armPoseEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(1)) );

  junc_in_legPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRootImportanceRef(0)) );
  junc_in_legPoseEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRootImportanceRef(1)) );

  junc_in_spinePoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSpinePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSpinePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_headPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getHeadPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getHeadPoseEndRelativeToRootImportanceRef(0)) );

  junc_in_sitParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSitParameters()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSitParametersImportanceRef()) );

  junc_in_supportPolygon = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->sittingSupportPolygon->out->getPolygon()), (float*)&(module->owner->sittingSupportPolygon->out->getPolygonImportanceRef()) );

  junc_in_supportWithArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_supportWithLegs = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_feedIn_chestOnGroundAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_feedIn_pelvisOnGroundAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getRootOnGroundAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getRootOnGroundAmountImportanceRef()) );

  junc_feedIn_handOnGroundAmount_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmountImportanceRef()) );
  junc_feedIn_handOnGroundAmount_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_feedIn_handOnGroundCollidingTime_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getEndGroundCollidingTime()), (float*)&(module->owner->owner->arms[0]->feedOut->getEndGroundCollidingTimeImportanceRef()) );
  junc_feedIn_handOnGroundCollidingTime_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getEndGroundCollidingTime()), (float*)&(module->owner->owner->arms[1]->feedOut->getEndGroundCollidingTimeImportanceRef()) );

  junc_feedIn_standingBalanceAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->owner->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SittingBodyBalance_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_armPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_armPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armPoseEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_armPoseEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(1)) );

  junc_in_legPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_legPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRootImportanceRef(0)) );
  junc_in_legPoseEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_legPoseEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRootImportanceRef(1)) );

  junc_in_spinePoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_spinePoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSpinePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSpinePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_headPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_headPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getHeadPoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->sitBehaviourInterface->out->getHeadPoseEndRelativeToRootImportanceRef(0)) );

  junc_in_sitParameters = ER::Junction::relocate(resource);
  // junc_in_sitParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSitParameters()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSitParametersImportanceRef()) );

  junc_in_supportPolygon = ER::Junction::relocate(resource);
  // junc_in_supportPolygon->getEdges()[0].reinit( (char*)&(module->owner->sittingSupportPolygon->out->getPolygon()), (float*)&(module->owner->sittingSupportPolygon->out->getPolygonImportanceRef()) );

  junc_in_supportWithArms = ER::Junction::relocate(resource);
  // junc_in_supportWithArms->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_supportWithLegs = ER::Junction::relocate(resource);
  // junc_in_supportWithLegs->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_feedIn_chestOnGroundAmount = ER::Junction::relocate(resource);
  // junc_feedIn_chestOnGroundAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_feedIn_pelvisOnGroundAmount = ER::Junction::relocate(resource);
  // junc_feedIn_pelvisOnGroundAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getRootOnGroundAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getRootOnGroundAmountImportanceRef()) );

  junc_feedIn_handOnGroundAmount_0 = ER::Junction::relocate(resource);
  // junc_feedIn_handOnGroundAmount_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmountImportanceRef()) );
  junc_feedIn_handOnGroundAmount_1 = ER::Junction::relocate(resource);
  // junc_feedIn_handOnGroundAmount_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_feedIn_handOnGroundCollidingTime_0 = ER::Junction::relocate(resource);
  // junc_feedIn_handOnGroundCollidingTime_0->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getEndGroundCollidingTime()), (float*)&(module->owner->owner->arms[0]->feedOut->getEndGroundCollidingTimeImportanceRef()) );
  junc_feedIn_handOnGroundCollidingTime_1 = ER::Junction::relocate(resource);
  // junc_feedIn_handOnGroundCollidingTime_1->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getEndGroundCollidingTime()), (float*)&(module->owner->owner->arms[1]->feedOut->getEndGroundCollidingTimeImportanceRef()) );

  junc_feedIn_standingBalanceAmount = ER::Junction::relocate(resource);
  // junc_feedIn_standingBalanceAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->owner->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SittingBodyBalance_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  SittingBodyBalance* module = (SittingBodyBalance*)modulePtr;
  SittingBodyBalanceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_chestOnGroundAmountImportance = junc_feedIn_chestOnGroundAmount->combineDirectInput(&feedIn.chestOnGroundAmount);
  feedIn.m_pelvisOnGroundAmountImportance = junc_feedIn_pelvisOnGroundAmount->combineDirectInput(&feedIn.pelvisOnGroundAmount);
  feedIn.m_handOnGroundAmountImportance[0] = junc_feedIn_handOnGroundAmount_0->combineDirectInput(&feedIn.handOnGroundAmount[0]);
  feedIn.m_handOnGroundAmountImportance[1] = junc_feedIn_handOnGroundAmount_1->combineDirectInput(&feedIn.handOnGroundAmount[1]);
  feedIn.m_handOnGroundCollidingTimeImportance[0] = junc_feedIn_handOnGroundCollidingTime_0->combineDirectInput(&feedIn.handOnGroundCollidingTime[0]);
  feedIn.m_handOnGroundCollidingTimeImportance[1] = junc_feedIn_handOnGroundCollidingTime_1->combineDirectInput(&feedIn.handOnGroundCollidingTime[1]);
  feedIn.m_standingBalanceAmountImportance = junc_feedIn_standingBalanceAmount->combineDirectInput(&feedIn.standingBalanceAmount);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SittingBodyBalance_Con::combineInputs(ER::Module* modulePtr)
{
  SittingBodyBalance* module = (SittingBodyBalance*)modulePtr;
  SittingBodyBalanceInputs& in = *module->in;

  // Junction assignments.
  in.m_armPoseEndRelativeToRootImportance[0] = junc_in_armPoseEndRelativeToRoot_0->combineDirectInput(&in.armPoseEndRelativeToRoot[0]);
  in.m_armPoseEndRelativeToRootImportance[1] = junc_in_armPoseEndRelativeToRoot_1->combineDirectInput(&in.armPoseEndRelativeToRoot[1]);
  in.m_legPoseEndRelativeToRootImportance[0] = junc_in_legPoseEndRelativeToRoot_0->combineDirectInput(&in.legPoseEndRelativeToRoot[0]);
  in.m_legPoseEndRelativeToRootImportance[1] = junc_in_legPoseEndRelativeToRoot_1->combineDirectInput(&in.legPoseEndRelativeToRoot[1]);
  in.m_spinePoseEndRelativeToRootImportance[0] = junc_in_spinePoseEndRelativeToRoot_0->combineDirectInput(&in.spinePoseEndRelativeToRoot[0]);
  in.m_headPoseEndRelativeToRootImportance[0] = junc_in_headPoseEndRelativeToRoot_0->combineDirectInput(&in.headPoseEndRelativeToRoot[0]);
  in.m_sitParametersImportance = junc_in_sitParameters->combineDirectInput(&in.sitParameters);
  in.m_supportPolygonImportance = junc_in_supportPolygon->combineDirectInput(&in.supportPolygon);
  in.m_supportWithArmsImportance = junc_in_supportWithArms->combineDirectInput(&in.supportWithArms);
  in.m_supportWithLegsImportance = junc_in_supportWithLegs->combineDirectInput(&in.supportWithLegs);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


