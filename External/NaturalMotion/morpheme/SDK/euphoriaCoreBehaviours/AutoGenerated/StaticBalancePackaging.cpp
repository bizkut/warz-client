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
#include "StaticBalance.h"
#include "StaticBalancePackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "AimBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "Head.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void StaticBalance_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((StaticBalance*)module);
}

//----------------------------------------------------------------------------------------------------------------------
StaticBalance_Con::~StaticBalance_Con()
{
  junc_out_averageAverageLegPoseEndRelSupport->destroy();
  junc_feedIn_currHeadEndPositions_0->destroy();
  junc_feedIn_supportTM->destroy();
  junc_in_stabilisingCrouchPitchAmount->destroy();
  junc_in_stabilisingCrouchDownAmount->destroy();
  junc_in_targetPelvisDirection->destroy();
  junc_in_targetPelvisOrientation->destroy();
  junc_in_balanceOffsetRight->destroy();
  junc_in_balanceOffsetFwd->destroy();
  junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount->destroy();
  junc_in_armPoseEndRelativeToRoot_1->destroy();
  junc_in_armPoseEndRelativeToRoot_0->destroy();
  junc_in_legPoseEndRelativeToRoot_1->destroy();
  junc_in_legPoseEndRelativeToRoot_0->destroy();
  junc_in_spinePoseEndRelativeToRoot_0->destroy();
  junc_in_headPoseEndRelativeToRoot_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format StaticBalance_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(StaticBalance_Con), 16);

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
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void StaticBalance_Con::buildConnections(StaticBalance* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(StaticBalance_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_headPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getHeadBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getHeadBalancePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_spinePoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_legPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_legPoseEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_in_armPoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armPoseEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmount()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportanceRef()) );

  junc_in_balanceOffsetFwd = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetFwd()), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetFwdImportanceRef()) );

  junc_in_balanceOffsetRight = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetRight()), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetRightImportanceRef()) );

  junc_in_targetPelvisOrientation = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisOrientation()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisOrientationImportanceRef()) );

  junc_in_targetPelvisDirection = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getTargetPelvisDirection()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getTargetPelvisDirectionImportanceRef()) );

  junc_in_stabilisingCrouchDownAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchDownAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchDownAmountImportanceRef()) );

  junc_in_stabilisingCrouchPitchAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchPitchAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchPitchAmountImportanceRef()) );

  junc_feedIn_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

  junc_feedIn_currHeadEndPositions_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->heads[0]->feedOut->getCurrEndPartPosition()), (float*)&(module->owner->owner->heads[0]->feedOut->getCurrEndPartPositionImportanceRef()) );

  junc_out_averageAverageLegPoseEndRelSupport = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->out->getLegPoseEndRelSupport(0)), (float*)&(module->out->getLegPoseEndRelSupportImportanceRef(0)) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->out->getLegPoseEndRelSupport(1)), (float*)&(module->out->getLegPoseEndRelSupportImportanceRef(1)) );

}

//----------------------------------------------------------------------------------------------------------------------
void StaticBalance_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(StaticBalance_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_headPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_headPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getHeadBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getHeadBalancePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_spinePoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_spinePoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_legPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_legPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_legPoseEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_legPoseEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_in_armPoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_armPoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_armPoseEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_armPoseEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount = ER::Junction::relocate(resource);
  // junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmount()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportanceRef()) );

  junc_in_balanceOffsetFwd = ER::Junction::relocate(resource);
  // junc_in_balanceOffsetFwd->getEdges()[0].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetFwd()), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetFwdImportanceRef()) );

  junc_in_balanceOffsetRight = ER::Junction::relocate(resource);
  // junc_in_balanceOffsetRight->getEdges()[0].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetRight()), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getBalanceOffsetRightImportanceRef()) );

  junc_in_targetPelvisOrientation = ER::Junction::relocate(resource);
  // junc_in_targetPelvisOrientation->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisOrientation()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisOrientationImportanceRef()) );

  junc_in_targetPelvisDirection = ER::Junction::relocate(resource);
  // junc_in_targetPelvisDirection->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getTargetPelvisDirection()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getTargetPelvisDirectionImportanceRef()) );

  junc_in_stabilisingCrouchDownAmount = ER::Junction::relocate(resource);
  // junc_in_stabilisingCrouchDownAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchDownAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchDownAmountImportanceRef()) );

  junc_in_stabilisingCrouchPitchAmount = ER::Junction::relocate(resource);
  // junc_in_stabilisingCrouchPitchAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchPitchAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getCrouchPitchAmountImportanceRef()) );

  junc_feedIn_supportTM = ER::Junction::relocate(resource);
  // junc_feedIn_supportTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

  junc_feedIn_currHeadEndPositions_0 = ER::Junction::relocate(resource);
  // junc_feedIn_currHeadEndPositions_0->getEdges()[0].reinit( (char*)&(module->owner->owner->heads[0]->feedOut->getCurrEndPartPosition()), (float*)&(module->owner->owner->heads[0]->feedOut->getCurrEndPartPositionImportanceRef()) );

  junc_out_averageAverageLegPoseEndRelSupport = ER::Junction::relocate(resource);
  // junc_out_averageAverageLegPoseEndRelSupport->getEdges()[0].reinit( (char*)&(module->out->getLegPoseEndRelSupport(0)), (float*)&(module->out->getLegPoseEndRelSupportImportanceRef(0)) );
  // junc_out_averageAverageLegPoseEndRelSupport->getEdges()[1].reinit( (char*)&(module->out->getLegPoseEndRelSupport(1)), (float*)&(module->out->getLegPoseEndRelSupportImportanceRef(1)) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void StaticBalance_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  StaticBalance* module = (StaticBalance*)modulePtr;
  StaticBalanceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportTMImportance = junc_feedIn_supportTM->combineDirectInput(&feedIn.supportTM);
  feedIn.m_currHeadEndPositionsImportance[0] = junc_feedIn_currHeadEndPositions_0->combineDirectInput(&feedIn.currHeadEndPositions[0]);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void StaticBalance_Con::combineInputs(ER::Module* modulePtr)
{
  StaticBalance* module = (StaticBalance*)modulePtr;
  StaticBalanceInputs& in = *module->in;

  // Junction assignments.
  in.m_headPoseEndRelativeToRootImportance[0] = junc_in_headPoseEndRelativeToRoot_0->combineDirectInput(&in.headPoseEndRelativeToRoot[0]);
  in.m_spinePoseEndRelativeToRootImportance[0] = junc_in_spinePoseEndRelativeToRoot_0->combineDirectInput(&in.spinePoseEndRelativeToRoot[0]);
  in.m_legPoseEndRelativeToRootImportance[0] = junc_in_legPoseEndRelativeToRoot_0->combineDirectInput(&in.legPoseEndRelativeToRoot[0]);
  in.m_legPoseEndRelativeToRootImportance[1] = junc_in_legPoseEndRelativeToRoot_1->combineDirectInput(&in.legPoseEndRelativeToRoot[1]);
  in.m_armPoseEndRelativeToRootImportance[0] = junc_in_armPoseEndRelativeToRoot_0->combineDirectInput(&in.armPoseEndRelativeToRoot[0]);
  in.m_armPoseEndRelativeToRootImportance[1] = junc_in_armPoseEndRelativeToRoot_1->combineDirectInput(&in.armPoseEndRelativeToRoot[1]);
  in.m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance = junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount->combineDirectInput(&in.balanceUsingAnimationHorizontalPelvisOffsetAmount);
  in.m_balanceOffsetFwdImportance = junc_in_balanceOffsetFwd->combineDirectInput(&in.balanceOffsetFwd);
  in.m_balanceOffsetRightImportance = junc_in_balanceOffsetRight->combineDirectInput(&in.balanceOffsetRight);
  in.m_targetPelvisOrientationImportance = junc_in_targetPelvisOrientation->combineDirectInput(&in.targetPelvisOrientation);
  in.m_targetPelvisDirectionImportance = junc_in_targetPelvisDirection->combineDirectInput(&in.targetPelvisDirection);
  in.m_stabilisingCrouchDownAmountImportance = junc_in_stabilisingCrouchDownAmount->combineDirectInput(&in.stabilisingCrouchDownAmount);
  in.m_stabilisingCrouchPitchAmountImportance = junc_in_stabilisingCrouchPitchAmount->combineDirectInput(&in.stabilisingCrouchPitchAmount);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void StaticBalance_Con::combineOutputs(ER::Module* modulePtr)
{
  StaticBalance* module = (StaticBalance*)modulePtr;
  StaticBalanceOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_averageLegPoseEndRelSupportImportance == 0.0f);
  out.m_averageLegPoseEndRelSupportImportance = junc_out_averageAverageLegPoseEndRelSupport->combineAverage(&out.averageLegPoseEndRelSupport);
  PostCombiners::Matrix34Orthonormalise(out.averageLegPoseEndRelSupport);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


