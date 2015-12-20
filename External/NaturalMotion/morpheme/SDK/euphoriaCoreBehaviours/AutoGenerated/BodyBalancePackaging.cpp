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
#include "BodyBalance.h"
#include "BodyBalancePackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStandingSupport.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "LegStep.h"
#include "ArmStep.h"
#include "BalanceBehaviourInterface.h"
#include "BalanceManagement.h"
#include "SupportPolygon.h"
#include "BalanceBehaviourFeedback.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BodyBalance*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BodyBalance_Con::~BodyBalance_Con()
{
  junc_feedOut_isStepping->destroy();
  junc_feedIn_stepDir->destroy();
  junc_feedIn_isDoingRecoveryStep->destroy();
  junc_junc_FeedbackInputs_isStepping_3->destroy();
  junc_junc_FeedbackInputs_isStepping_2->destroy();
  junc_junc_FeedbackInputs_isStepping_1->destroy();
  junc_junc_FeedbackInputs_isStepping_0->destroy();
  junc_feedIn_isSteppingBeingSuppressed->destroy();
  junc_feedIn_lowerPelvisDistance->destroy();
  junc_junc_FeedbackInputs_stepLengths_1->destroy();
  junc_junc_FeedbackInputs_stepLengths_3->destroy();
  junc_junc_FeedbackInputs_stepLengths_0->destroy();
  junc_junc_FeedbackInputs_stepLengths_2->destroy();
  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_0->destroy();
  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_1->destroy();
  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_2->destroy();
  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_3->destroy();
  junc_feedIn_isSupportedByConstraint->destroy();
  junc_junc_FeedbackInputs_limbSupportAmounts_2->destroy();
  junc_junc_FeedbackInputs_limbSupportAmounts_3->destroy();
  junc_junc_FeedbackInputs_limbSupportAmounts_0->destroy();
  junc_junc_FeedbackInputs_limbSupportAmounts_1->destroy();
  junc_feedIn_enableSupportOnArms->destroy();
  junc_feedIn_enableSupportOnLegs->destroy();
  junc_in_enableStepOnArms->destroy();
  junc_in_enableStepOnLegs->destroy();
  junc_in_enableSupportOnArms->destroy();
  junc_in_enableSupportOnLegs->destroy();
  junc_in_supportPolygon->destroy();
  junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount->destroy();
  junc_in_legEndRelativeToRoot_1->destroy();
  junc_in_legEndRelativeToRoot_0->destroy();
  junc_in_spinePoseEndRelativeToRoot_0->destroy();
  junc_in_pelvisRelSupport->destroy();
  junc_in_balancePoseClamping->destroy();
  junc_in_stepRecoveryParameters->destroy();
  junc_in_balanceParameters->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BodyBalance_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BodyBalance_Con), 16);

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
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(4);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BodyBalance_Con::buildConnections(BodyBalance* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodyBalance_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_balanceParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParametersImportanceRef()) );

  junc_in_stepRecoveryParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getStepRecoveryParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getStepRecoveryParametersImportanceRef()) );

  junc_in_balancePoseClamping = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalancePoseClamping()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalancePoseClampingImportanceRef()) );

  junc_in_pelvisRelSupport = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceManagement->out->getPelvisRelSupport()), (float*)&(module->owner->owner->balanceManagement->out->getPelvisRelSupportImportanceRef()) );

  junc_in_spinePoseEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_legEndRelativeToRoot_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_legEndRelativeToRoot_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmount()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportanceRef()) );

  junc_in_supportPolygon = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->supportPolygon->out->getPolygon()), (float*)&(module->owner->supportPolygon->out->getPolygonImportanceRef()) );

  junc_in_enableSupportOnLegs = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_in_enableSupportOnArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_enableStepOnLegs = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithLegsImportanceRef()) );

  junc_in_enableStepOnArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithArmsImportanceRef()) );

  junc_feedIn_enableSupportOnLegs = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithLegsImportanceRef()) );

  junc_feedIn_enableSupportOnArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithArmsImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_isSupportedByConstraint = activeJnc = ER::Junction::init(resource, 4, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->legs[0]->feedOut->getSupportedByConstraintImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->legs[1]->feedOut->getSupportedByConstraintImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->arms[0]->feedOut->getSupportedByConstraintImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->arms[1]->feedOut->getSupportedByConstraintImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->legs[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->legs[0]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getStepLengthImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getStepLengthImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getStepLengthImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getStepLengthImportanceRef()) );

  junc_feedIn_lowerPelvisDistance = activeJnc = ER::Junction::init(resource, 4, ER::Junction::maxFloat);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getLowerRootDistanceImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getLowerRootDistanceImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getLowerRootDistanceImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getLowerRootDistanceImportanceRef()) );

  junc_feedIn_isSteppingBeingSuppressed = activeJnc = ER::Junction::init(resource, 4, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_feedIn_isDoingRecoveryStep = activeJnc = ER::Junction::init(resource, 4, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );

  junc_feedIn_stepDir = activeJnc = ER::Junction::init(resource, 4, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getStepDirImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getStepDirImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getStepDirImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getStepDirImportanceRef()) );

  junc_feedOut_isStepping = activeJnc = ER::Junction::init(resource, 4, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getIsStepping(0)), (float*)&(module->feedIn->getIsSteppingImportanceRef(0)) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getIsStepping(1)), (float*)&(module->feedIn->getIsSteppingImportanceRef(1)) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->feedIn->getIsStepping(2)), (float*)&(module->feedIn->getIsSteppingImportanceRef(2)) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->feedIn->getIsStepping(3)), (float*)&(module->feedIn->getIsSteppingImportanceRef(3)) );

}

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodyBalance_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_balanceParameters = ER::Junction::relocate(resource);
  // junc_in_balanceParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParametersImportanceRef()) );

  junc_in_stepRecoveryParameters = ER::Junction::relocate(resource);
  // junc_in_stepRecoveryParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getStepRecoveryParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getStepRecoveryParametersImportanceRef()) );

  junc_in_balancePoseClamping = ER::Junction::relocate(resource);
  // junc_in_balancePoseClamping->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalancePoseClamping()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalancePoseClampingImportanceRef()) );

  junc_in_pelvisRelSupport = ER::Junction::relocate(resource);
  // junc_in_pelvisRelSupport->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceManagement->out->getPelvisRelSupport()), (float*)&(module->owner->owner->balanceManagement->out->getPelvisRelSupportImportanceRef()) );

  junc_in_spinePoseEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_spinePoseEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSpineBalancePoseEndRelativeToRootImportanceRef(0)) );

  junc_in_legEndRelativeToRoot_0 = ER::Junction::relocate(resource);
  // junc_in_legEndRelativeToRoot_0->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(0)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(0)) );
  junc_in_legEndRelativeToRoot_1 = ER::Junction::relocate(resource);
  // junc_in_legEndRelativeToRoot_1->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(1)), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(1)) );

  junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount = ER::Junction::relocate(resource);
  // junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmount()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportanceRef()) );

  junc_in_supportPolygon = ER::Junction::relocate(resource);
  // junc_in_supportPolygon->getEdges()[0].reinit( (char*)&(module->owner->supportPolygon->out->getPolygon()), (float*)&(module->owner->supportPolygon->out->getPolygonImportanceRef()) );

  junc_in_enableSupportOnLegs = ER::Junction::relocate(resource);
  // junc_in_enableSupportOnLegs->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_in_enableSupportOnArms = ER::Junction::relocate(resource);
  // junc_in_enableSupportOnArms->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_enableStepOnLegs = ER::Junction::relocate(resource);
  // junc_in_enableStepOnLegs->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithLegsImportanceRef()) );

  junc_in_enableStepOnArms = ER::Junction::relocate(resource);
  // junc_in_enableStepOnArms->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getStepWithArmsImportanceRef()) );

  junc_feedIn_enableSupportOnLegs = ER::Junction::relocate(resource);
  // junc_feedIn_enableSupportOnLegs->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithLegsImportanceRef()) );

  junc_feedIn_enableSupportOnArms = ER::Junction::relocate(resource);
  // junc_feedIn_enableSupportOnArms->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourFeedback->feedOut->getSupportWithArmsImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbSupportAmounts_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbSupportAmounts_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbSupportAmounts_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbSupportAmounts_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbSupportAmounts_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_isSupportedByConstraint = ER::Junction::relocate(resource);
  // junc_feedIn_isSupportedByConstraint->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->legs[0]->feedOut->getSupportedByConstraintImportanceRef()) );
  // junc_feedIn_isSupportedByConstraint->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->legs[1]->feedOut->getSupportedByConstraintImportanceRef()) );
  // junc_feedIn_isSupportedByConstraint->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->arms[0]->feedOut->getSupportedByConstraintImportanceRef()) );
  // junc_feedIn_isSupportedByConstraint->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getSupportedByConstraint()), (float*)&(module->owner->owner->arms[1]->feedOut->getSupportedByConstraintImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbEndOnGroundAmounts_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbEndOnGroundAmounts_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->arms[0]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbEndOnGroundAmounts_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->legs[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_limbEndOnGroundAmounts_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_limbEndOnGroundAmounts_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->owner->legs[0]->feedOut->getEndOnGroundAmountImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_stepLengths_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getStepLengthImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_stepLengths_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getStepLengthImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_stepLengths_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getStepLengthImportanceRef()) );

  junc_junc_FeedbackInputs_stepLengths_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_stepLengths_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getStepLength()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getStepLengthImportanceRef()) );

  junc_feedIn_lowerPelvisDistance = ER::Junction::relocate(resource);
  // junc_feedIn_lowerPelvisDistance->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getLowerRootDistanceImportanceRef()) );
  // junc_feedIn_lowerPelvisDistance->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getLowerRootDistanceImportanceRef()) );
  // junc_feedIn_lowerPelvisDistance->getEdges()[2].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getLowerRootDistanceImportanceRef()) );
  // junc_feedIn_lowerPelvisDistance->getEdges()[3].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getLowerRootDistance()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getLowerRootDistanceImportanceRef()) );

  junc_feedIn_isSteppingBeingSuppressed = ER::Junction::relocate(resource);
  // junc_feedIn_isSteppingBeingSuppressed->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );
  // junc_feedIn_isSteppingBeingSuppressed->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );
  // junc_feedIn_isSteppingBeingSuppressed->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );
  // junc_feedIn_isSteppingBeingSuppressed->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getIsSteppingBeingSuppressed()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getIsSteppingBeingSuppressedImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isStepping_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isStepping_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isStepping_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isStepping_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_feedIn_isDoingRecoveryStep = ER::Junction::relocate(resource);
  // junc_feedIn_isDoingRecoveryStep->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );
  // junc_feedIn_isDoingRecoveryStep->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );
  // junc_feedIn_isDoingRecoveryStep->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );
  // junc_feedIn_isDoingRecoveryStep->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getIsDoingRecoveryStep()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getIsDoingRecoveryStepImportanceRef()) );

  junc_feedIn_stepDir = ER::Junction::relocate(resource);
  // junc_feedIn_stepDir->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getStepDirImportanceRef()) );
  // junc_feedIn_stepDir->getEdges()[1].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getStepDirImportanceRef()) );
  // junc_feedIn_stepDir->getEdges()[2].reinit( (char*)&(module->owner->owner->arms[0]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->arms[0]->step->feedOut->getStepDirImportanceRef()) );
  // junc_feedIn_stepDir->getEdges()[3].reinit( (char*)&(module->owner->owner->arms[1]->step->feedOut->getStepDir()), (float*)&(module->owner->owner->arms[1]->step->feedOut->getStepDirImportanceRef()) );

  junc_feedOut_isStepping = ER::Junction::relocate(resource);
  // junc_feedOut_isStepping->getEdges()[0].reinit( (char*)&(module->feedIn->getIsStepping(0)), (float*)&(module->feedIn->getIsSteppingImportanceRef(0)) );
  // junc_feedOut_isStepping->getEdges()[1].reinit( (char*)&(module->feedIn->getIsStepping(1)), (float*)&(module->feedIn->getIsSteppingImportanceRef(1)) );
  // junc_feedOut_isStepping->getEdges()[2].reinit( (char*)&(module->feedIn->getIsStepping(2)), (float*)&(module->feedIn->getIsSteppingImportanceRef(2)) );
  // junc_feedOut_isStepping->getEdges()[3].reinit( (char*)&(module->feedIn->getIsStepping(3)), (float*)&(module->feedIn->getIsSteppingImportanceRef(3)) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyBalance_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BodyBalance* module = (BodyBalance*)modulePtr;
  BodyBalanceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_enableSupportOnLegsImportance = junc_feedIn_enableSupportOnLegs->combineDirectInput(&feedIn.enableSupportOnLegs);
  feedIn.m_enableSupportOnArmsImportance = junc_feedIn_enableSupportOnArms->combineDirectInput(&feedIn.enableSupportOnArms);
  feedIn.m_limbSupportAmountsImportance[0] = junc_junc_FeedbackInputs_limbSupportAmounts_0->combineDirectInput(&feedIn.limbSupportAmounts[0]);
  feedIn.m_limbSupportAmountsImportance[2] = junc_junc_FeedbackInputs_limbSupportAmounts_2->combineDirectInput(&feedIn.limbSupportAmounts[2]);
  feedIn.m_limbSupportAmountsImportance[1] = junc_junc_FeedbackInputs_limbSupportAmounts_1->combineDirectInput(&feedIn.limbSupportAmounts[1]);
  feedIn.m_limbSupportAmountsImportance[3] = junc_junc_FeedbackInputs_limbSupportAmounts_3->combineDirectInput(&feedIn.limbSupportAmounts[3]);
  feedIn.m_isSupportedByConstraintImportance = junc_feedIn_isSupportedByConstraint->combineIsBoolSet(&feedIn.isSupportedByConstraint);
  feedIn.m_limbEndOnGroundAmountsImportance[3] = junc_junc_FeedbackInputs_limbEndOnGroundAmounts_3->combineDirectInput(&feedIn.limbEndOnGroundAmounts[3]);
  feedIn.m_limbEndOnGroundAmountsImportance[0] = junc_junc_FeedbackInputs_limbEndOnGroundAmounts_0->combineDirectInput(&feedIn.limbEndOnGroundAmounts[0]);
  feedIn.m_limbEndOnGroundAmountsImportance[2] = junc_junc_FeedbackInputs_limbEndOnGroundAmounts_2->combineDirectInput(&feedIn.limbEndOnGroundAmounts[2]);
  feedIn.m_limbEndOnGroundAmountsImportance[1] = junc_junc_FeedbackInputs_limbEndOnGroundAmounts_1->combineDirectInput(&feedIn.limbEndOnGroundAmounts[1]);
  feedIn.m_stepLengthsImportance[0] = junc_junc_FeedbackInputs_stepLengths_0->combineDirectInput(&feedIn.stepLengths[0]);
  feedIn.m_stepLengthsImportance[1] = junc_junc_FeedbackInputs_stepLengths_1->combineDirectInput(&feedIn.stepLengths[1]);
  feedIn.m_stepLengthsImportance[2] = junc_junc_FeedbackInputs_stepLengths_2->combineDirectInput(&feedIn.stepLengths[2]);
  feedIn.m_stepLengthsImportance[3] = junc_junc_FeedbackInputs_stepLengths_3->combineDirectInput(&feedIn.stepLengths[3]);
  feedIn.m_lowerPelvisDistanceImportance = junc_feedIn_lowerPelvisDistance->combineMaxFloat(&feedIn.lowerPelvisDistance);
  feedIn.m_isSteppingBeingSuppressedImportance = junc_feedIn_isSteppingBeingSuppressed->combineIsBoolSet(&feedIn.isSteppingBeingSuppressed);
  feedIn.m_isSteppingImportance[0] = junc_junc_FeedbackInputs_isStepping_0->combineDirectInput(&feedIn.isStepping[0]);
  feedIn.m_isSteppingImportance[1] = junc_junc_FeedbackInputs_isStepping_1->combineDirectInput(&feedIn.isStepping[1]);
  feedIn.m_isSteppingImportance[2] = junc_junc_FeedbackInputs_isStepping_2->combineDirectInput(&feedIn.isStepping[2]);
  feedIn.m_isSteppingImportance[3] = junc_junc_FeedbackInputs_isStepping_3->combineDirectInput(&feedIn.isStepping[3]);
  feedIn.m_isDoingRecoveryStepImportance = junc_feedIn_isDoingRecoveryStep->combineIsBoolSet(&feedIn.isDoingRecoveryStep);
  feedIn.m_stepDirImportance = junc_feedIn_stepDir->combineAverage(&feedIn.stepDir);
  PostCombiners::Vector3Normalise(feedIn.stepDir);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyBalance_Con::combineInputs(ER::Module* modulePtr)
{
  BodyBalance* module = (BodyBalance*)modulePtr;
  BodyBalanceInputs& in = *module->in;

  // Junction assignments.
  in.m_balanceParametersImportance = junc_in_balanceParameters->combineDirectInput(&in.balanceParameters);
  in.m_stepRecoveryParametersImportance = junc_in_stepRecoveryParameters->combineDirectInput(&in.stepRecoveryParameters);
  in.m_balancePoseClampingImportance = junc_in_balancePoseClamping->combineDirectInput(&in.balancePoseClamping);
  in.m_pelvisRelSupportImportance = junc_in_pelvisRelSupport->combineDirectInput(&in.pelvisRelSupport);
  in.m_spinePoseEndRelativeToRootImportance[0] = junc_in_spinePoseEndRelativeToRoot_0->combineDirectInput(&in.spinePoseEndRelativeToRoot[0]);
  in.m_legEndRelativeToRootImportance[0] = junc_in_legEndRelativeToRoot_0->combineDirectInput(&in.legEndRelativeToRoot[0]);
  in.m_legEndRelativeToRootImportance[1] = junc_in_legEndRelativeToRoot_1->combineDirectInput(&in.legEndRelativeToRoot[1]);
  in.m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance = junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount->combineDirectInput(&in.balanceUsingAnimationHorizontalPelvisOffsetAmount);
  in.m_supportPolygonImportance = junc_in_supportPolygon->combineDirectInput(&in.supportPolygon);
  in.m_enableSupportOnLegsImportance = junc_in_enableSupportOnLegs->combineDirectInput(&in.enableSupportOnLegs);
  in.m_enableSupportOnArmsImportance = junc_in_enableSupportOnArms->combineDirectInput(&in.enableSupportOnArms);
  in.m_enableStepOnLegsImportance = junc_in_enableStepOnLegs->combineDirectInput(&in.enableStepOnLegs);
  in.m_enableStepOnArmsImportance = junc_in_enableStepOnArms->combineDirectInput(&in.enableStepOnArms);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyBalance_Con::combineFeedbackOutputs(ER::Module* modulePtr)
{
  BodyBalance* module = (BodyBalance*)modulePtr;
  BodyBalanceFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_isSteppingImportance == 0.0f);
  feedOut.m_isSteppingImportance = junc_feedOut_isStepping->combineIsBoolSet(&feedOut.isStepping);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


