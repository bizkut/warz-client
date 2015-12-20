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
#include "BalanceManagement.h"
#include "BalanceManagementPackaging.h"
#include "BodyFrame.h"
#include "BalanceBehaviourInterface.h"
#include "Arm.h"
#include "ArmReachForBodyPart.h"
#include "BodySection.h"
#include "RotateCore.h"
#include "PositionCore.h"
#include "ArmReachForWorld.h"
#include "BodyBalance.h"
#include "StaticBalance.h"
#include "BalancePoser.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalanceManagement_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BalanceManagement*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BalanceManagement_Con::~BalanceManagement_Con()
{
  junc_out_headRelChestOrientation_0->destroy();
  junc_in_pelvisRelSupport->destroy();
  junc_feedIn_isStepping->destroy();
  junc_feedIn_chestDirectionRequest->destroy();
  junc_feedIn_avgReachForWorldCDR->destroy();
  junc_feedIn_avgReachForBodyPartCDR->destroy();
  junc_feedIn_pelvisDirectionRequest->destroy();
  junc_feedIn_avgReachForWorldPDR->destroy();
  junc_feedIn_avgReachForBodyPartPDR->destroy();
  junc_feedIn_pelvisTranslationRequest->destroy();
  junc_feedIn_avgReachForWorldPTR->destroy();
  junc_feedIn_avgReachForBodyPartPTR->destroy();
  junc_pelvisRotReq->destroy();
  junc_in_supportWithLegs->destroy();
  junc_in_supportWithArms->destroy();
  junc_feedIn_supportAmount->destroy();
  junc_feedIn_impendingInstability->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BalanceManagement_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BalanceManagement_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BalanceManagement_Con::buildConnections(BalanceManagement* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalanceManagement_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_impendingInstability = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getImpendingInstability()), (float*)&(module->owner->bodyFrame->feedOut->getImpendingInstabilityImportanceRef()) );

  junc_feedIn_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_supportWithArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_supportWithLegs = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_pelvisRotReq = activeJnc = ER::Junction::init(resource, 3, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getRotationRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getRotationRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->lowerBody->rotate->feedOut->getRotationRequest()), (float*)&(module->owner->lowerBody->rotate->feedOut->getRotationRequestImportanceRef()) );

  junc_feedIn_avgReachForBodyPartPTR = activeJnc = ER::Junction::init(resource, 3, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->lowerBody->position->feedOut->getTranslationRequest()), (float*)&(module->owner->lowerBody->position->feedOut->getTranslationRequestImportanceRef()) );

  junc_feedIn_avgReachForWorldPTR = activeJnc = ER::Junction::init(resource, 3, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->lowerBody->position->feedOut->getTranslationRequest()), (float*)&(module->owner->lowerBody->position->feedOut->getTranslationRequestImportanceRef()) );

  junc_feedIn_pelvisTranslationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getAvgReachForWorldPTR()), (float*)&(module->feedIn->getAvgReachForWorldPTRImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getAvgReachForBodyPartPTR()), (float*)&(module->feedIn->getAvgReachForBodyPartPTRImportanceRef()) );

  junc_feedIn_avgReachForBodyPartPDR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisDirectionRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisDirectionRequestImportanceRef()) );

  junc_feedIn_avgReachForWorldPDR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisDirectionRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisDirectionRequestImportanceRef()) );

  junc_feedIn_pelvisDirectionRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getAvgReachForWorldPDR()), (float*)&(module->feedIn->getAvgReachForWorldPDRImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getAvgReachForBodyPartPDR()), (float*)&(module->feedIn->getAvgReachForBodyPartPDRImportanceRef()) );

  junc_feedIn_avgReachForBodyPartCDR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestDirectionRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestDirectionRequestImportanceRef()) );

  junc_feedIn_avgReachForWorldCDR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestDirectionRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestDirectionRequestImportanceRef()) );

  junc_feedIn_chestDirectionRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getAvgReachForBodyPartCDR()), (float*)&(module->feedIn->getAvgReachForBodyPartCDRImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getAvgReachForWorldCDR()), (float*)&(module->feedIn->getAvgReachForWorldCDRImportanceRef()) );

  junc_feedIn_isStepping = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsStepping()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsSteppingImportanceRef()) );

  junc_in_pelvisRelSupport = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->staticBalance->out->getPelvisRelSupport()), (float*)&(module->staticBalance->out->getPelvisRelSupportImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->balancePoser->out->getPelvisRelSupport()), (float*)&(module->balancePoser->out->getPelvisRelSupportImportanceRef()) );

  junc_out_headRelChestOrientation_0 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->staticBalance->out->getHeadRelChestOrientation(0)), (float*)&(module->staticBalance->out->getHeadRelChestOrientationImportanceRef(0)) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->balancePoser->out->getHeadRelChestOrientation(0)), (float*)&(module->balancePoser->out->getHeadRelChestOrientationImportanceRef(0)) );

}

//----------------------------------------------------------------------------------------------------------------------
void BalanceManagement_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalanceManagement_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_impendingInstability = ER::Junction::relocate(resource);
  // junc_feedIn_impendingInstability->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getImpendingInstability()), (float*)&(module->owner->bodyFrame->feedOut->getImpendingInstabilityImportanceRef()) );

  junc_feedIn_supportAmount = ER::Junction::relocate(resource);
  // junc_feedIn_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_supportWithArms = ER::Junction::relocate(resource);
  // junc_in_supportWithArms->getEdges()[0].reinit( (char*)&(module->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_supportWithLegs = ER::Junction::relocate(resource);
  // junc_in_supportWithLegs->getEdges()[0].reinit( (char*)&(module->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_pelvisRotReq = ER::Junction::relocate(resource);
  // junc_pelvisRotReq->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getRotationRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getRotationRequestImportanceRef()) );
  // junc_pelvisRotReq->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getRotationRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getRotationRequestImportanceRef()) );
  // junc_pelvisRotReq->getEdges()[2].reinit( (char*)&(module->owner->lowerBody->rotate->feedOut->getRotationRequest()), (float*)&(module->owner->lowerBody->rotate->feedOut->getRotationRequestImportanceRef()) );

  junc_feedIn_avgReachForBodyPartPTR = ER::Junction::relocate(resource);
  // junc_feedIn_avgReachForBodyPartPTR->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisTranslationRequestImportanceRef()) );
  // junc_feedIn_avgReachForBodyPartPTR->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisTranslationRequestImportanceRef()) );
  // junc_feedIn_avgReachForBodyPartPTR->getEdges()[2].reinit( (char*)&(module->owner->lowerBody->position->feedOut->getTranslationRequest()), (float*)&(module->owner->lowerBody->position->feedOut->getTranslationRequestImportanceRef()) );

  junc_feedIn_avgReachForWorldPTR = ER::Junction::relocate(resource);
  // junc_feedIn_avgReachForWorldPTR->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisTranslationRequestImportanceRef()) );
  // junc_feedIn_avgReachForWorldPTR->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisTranslationRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisTranslationRequestImportanceRef()) );
  // junc_feedIn_avgReachForWorldPTR->getEdges()[2].reinit( (char*)&(module->owner->lowerBody->position->feedOut->getTranslationRequest()), (float*)&(module->owner->lowerBody->position->feedOut->getTranslationRequestImportanceRef()) );

  junc_feedIn_pelvisTranslationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_pelvisTranslationRequest->getEdges()[0].reinit( (char*)&(module->feedIn->getAvgReachForWorldPTR()), (float*)&(module->feedIn->getAvgReachForWorldPTRImportanceRef()) );
  // junc_feedIn_pelvisTranslationRequest->getEdges()[1].reinit( (char*)&(module->feedIn->getAvgReachForBodyPartPTR()), (float*)&(module->feedIn->getAvgReachForBodyPartPTRImportanceRef()) );

  junc_feedIn_avgReachForBodyPartPDR = ER::Junction::relocate(resource);
  // junc_feedIn_avgReachForBodyPartPDR->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getPelvisDirectionRequestImportanceRef()) );
  // junc_feedIn_avgReachForBodyPartPDR->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getPelvisDirectionRequestImportanceRef()) );

  junc_feedIn_avgReachForWorldPDR = ER::Junction::relocate(resource);
  // junc_feedIn_avgReachForWorldPDR->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getPelvisDirectionRequestImportanceRef()) );
  // junc_feedIn_avgReachForWorldPDR->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisDirectionRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getPelvisDirectionRequestImportanceRef()) );

  junc_feedIn_pelvisDirectionRequest = ER::Junction::relocate(resource);
  // junc_feedIn_pelvisDirectionRequest->getEdges()[0].reinit( (char*)&(module->feedIn->getAvgReachForWorldPDR()), (float*)&(module->feedIn->getAvgReachForWorldPDRImportanceRef()) );
  // junc_feedIn_pelvisDirectionRequest->getEdges()[1].reinit( (char*)&(module->feedIn->getAvgReachForBodyPartPDR()), (float*)&(module->feedIn->getAvgReachForBodyPartPDRImportanceRef()) );

  junc_feedIn_avgReachForBodyPartCDR = ER::Junction::relocate(resource);
  // junc_feedIn_avgReachForBodyPartCDR->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestDirectionRequestImportanceRef()) );
  // junc_feedIn_avgReachForBodyPartCDR->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestDirectionRequestImportanceRef()) );

  junc_feedIn_avgReachForWorldCDR = ER::Junction::relocate(resource);
  // junc_feedIn_avgReachForWorldCDR->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestDirectionRequestImportanceRef()) );
  // junc_feedIn_avgReachForWorldCDR->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestDirectionRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestDirectionRequestImportanceRef()) );

  junc_feedIn_chestDirectionRequest = ER::Junction::relocate(resource);
  // junc_feedIn_chestDirectionRequest->getEdges()[0].reinit( (char*)&(module->feedIn->getAvgReachForBodyPartCDR()), (float*)&(module->feedIn->getAvgReachForBodyPartCDRImportanceRef()) );
  // junc_feedIn_chestDirectionRequest->getEdges()[1].reinit( (char*)&(module->feedIn->getAvgReachForWorldCDR()), (float*)&(module->feedIn->getAvgReachForWorldCDRImportanceRef()) );

  junc_feedIn_isStepping = ER::Junction::relocate(resource);
  // junc_feedIn_isStepping->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsStepping()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsSteppingImportanceRef()) );

  junc_in_pelvisRelSupport = ER::Junction::relocate(resource);
  // junc_in_pelvisRelSupport->getEdges()[0].reinit( (char*)&(module->staticBalance->out->getPelvisRelSupport()), (float*)&(module->staticBalance->out->getPelvisRelSupportImportanceRef()) );
  // junc_in_pelvisRelSupport->getEdges()[1].reinit( (char*)&(module->balancePoser->out->getPelvisRelSupport()), (float*)&(module->balancePoser->out->getPelvisRelSupportImportanceRef()) );

  junc_out_headRelChestOrientation_0 = ER::Junction::relocate(resource);
  // junc_out_headRelChestOrientation_0->getEdges()[0].reinit( (char*)&(module->staticBalance->out->getHeadRelChestOrientation(0)), (float*)&(module->staticBalance->out->getHeadRelChestOrientationImportanceRef(0)) );
  // junc_out_headRelChestOrientation_0->getEdges()[1].reinit( (char*)&(module->balancePoser->out->getHeadRelChestOrientation(0)), (float*)&(module->balancePoser->out->getHeadRelChestOrientationImportanceRef(0)) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceManagement_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BalanceManagement* module = (BalanceManagement*)modulePtr;
  BalanceManagementFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_impendingInstabilityImportance = junc_feedIn_impendingInstability->combineDirectInput(&feedIn.impendingInstability);
  feedIn.m_supportAmountImportance = junc_feedIn_supportAmount->combineDirectInput(&feedIn.supportAmount);
  feedIn.m_pelvisRotationRequestImportance = junc_pelvisRotReq->combineAverage(&feedIn.pelvisRotationRequest);
  feedIn.m_avgReachForBodyPartPTRImportance = junc_feedIn_avgReachForBodyPartPTR->combineAverage(&feedIn.avgReachForBodyPartPTR);
  feedIn.m_avgReachForWorldPTRImportance = junc_feedIn_avgReachForWorldPTR->combineAverage(&feedIn.avgReachForWorldPTR);
  feedIn.m_pelvisTranslationRequestImportance = junc_feedIn_pelvisTranslationRequest->combineWinnerTakesAll(&feedIn.pelvisTranslationRequest);
  feedIn.m_avgReachForBodyPartPDRImportance = junc_feedIn_avgReachForBodyPartPDR->combineAverage(&feedIn.avgReachForBodyPartPDR);
  feedIn.avgReachForBodyPartPDR.postCombine();
  feedIn.m_avgReachForWorldPDRImportance = junc_feedIn_avgReachForWorldPDR->combineAverage(&feedIn.avgReachForWorldPDR);
  feedIn.avgReachForWorldPDR.postCombine();
  feedIn.m_pelvisDirectionRequestImportance = junc_feedIn_pelvisDirectionRequest->combineWinnerTakesAll(&feedIn.pelvisDirectionRequest);
  feedIn.m_avgReachForBodyPartCDRImportance = junc_feedIn_avgReachForBodyPartCDR->combineAverage(&feedIn.avgReachForBodyPartCDR);
  feedIn.avgReachForBodyPartCDR.postCombine();
  feedIn.m_avgReachForWorldCDRImportance = junc_feedIn_avgReachForWorldCDR->combineAverage(&feedIn.avgReachForWorldCDR);
  feedIn.avgReachForWorldCDR.postCombine();
  feedIn.m_chestDirectionRequestImportance = junc_feedIn_chestDirectionRequest->combineWinnerTakesAll(&feedIn.chestDirectionRequest);
  feedIn.m_isSteppingImportance = junc_feedIn_isStepping->combineDirectInput(&feedIn.isStepping);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceManagement_Con::combineInputs(ER::Module* modulePtr)
{
  BalanceManagement* module = (BalanceManagement*)modulePtr;
  BalanceManagementInputs& in = *module->in;

  // Junction assignments.
  in.m_supportWithArmsImportance = junc_in_supportWithArms->combineDirectInput(&in.supportWithArms);
  in.m_supportWithLegsImportance = junc_in_supportWithLegs->combineDirectInput(&in.supportWithLegs);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceManagement_Con::combineOutputs(ER::Module* modulePtr)
{
  BalanceManagement* module = (BalanceManagement*)modulePtr;
  BalanceManagementOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_pelvisRelSupportImportance == 0.0f);
  out.m_pelvisRelSupportImportance = junc_in_pelvisRelSupport->combinePriority(&out.pelvisRelSupport);
  PostCombiners::Matrix34Orthonormalise(out.pelvisRelSupport);
  NMP_ASSERT(out.m_headRelChestOrientationImportance[0] == 0.0f);
  out.m_headRelChestOrientationImportance[0] = junc_out_headRelChestOrientation_0->combinePriority(&out.headRelChestOrientation[0]);
  PostCombiners::QuatNormalise(out.headRelChestOrientation[0]);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


