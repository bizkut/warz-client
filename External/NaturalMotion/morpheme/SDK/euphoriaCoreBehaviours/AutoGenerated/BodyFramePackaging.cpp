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
#include "BodyFrame.h"
#include "BodyFramePackaging.h"
#include "HazardManagement.h"
#include "BodyBalance.h"
#include "SittingBodyBalance.h"
#include "Leg.h"
#include "Spine.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "LegStandingSupport.h"
#include "Head.h"
#include "SupportPolygon.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BodyFrame_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BodyFrame*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BodyFrame_Con::~BodyFrame_Con()
{
  junc_feedOut_leanVector->destroy();
  junc_out_isInContact->destroy();
  junc_out_pelvisRotationRequest->destroy();
  junc_feedIn_isBodyBarFeetContacting->destroy();
  junc_feedIn_maxLegCollidingTime->destroy();
  junc_feedIn_spineInContact->destroy();
  junc_feedIn_legsInContact->destroy();
  junc_feedIn_averageShoulders->destroy();
  junc_feedIn_averageOfHandsAndFeet->destroy();
  junc_feedIn_averageOfSupportingFeet->destroy();
  junc_feedIn_averageOfSupportingHands->destroy();
  junc_feedIn_chestState->destroy();
  junc_feedIn_averageHips->destroy();
  junc_feedOut_supportAmount->destroy();
  junc_feedOut_impendingInstability->destroy();
  junc_in_instabilityPreparation->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BodyFrame_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BodyFrame_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(6);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(6);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BodyFrame_Con::buildConnections(BodyFrame* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodyFrame_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_instabilityPreparation = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->out->getInstabilityPreparation()), (float*)&(module->owner->hazardManagement->out->getInstabilityPreparationImportanceRef()) );

  junc_feedOut_impendingInstability = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->in->getInstabilityPreparation()), (float*)&(module->in->getInstabilityPreparationImportanceRef()) );

  junc_feedOut_supportAmount = activeJnc = ER::Junction::init(resource, 3, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->sittingBodyBalance->feedOut->getSitAmount()), (float*)&(module->sittingBodyBalance->feedOut->getSitAmountImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->feedIn->getExternalSpineSupportAmount()), (float*)&(module->feedIn->getExternalSpineSupportAmountImportanceRef()) );

  junc_feedIn_averageHips = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getHipPosition()), (float*)&(module->owner->legs[0]->feedOut->getHipPositionImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getHipPosition()), (float*)&(module->owner->legs[1]->feedOut->getHipPositionImportanceRef()) );

  junc_feedIn_chestState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getChestState()), (float*)&(module->owner->spines[0]->feedOut->getChestStateImportanceRef()) );

  junc_feedIn_averageOfSupportingHands = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_feedIn_averageOfSupportingFeet = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_feedIn_averageOfHandsAndFeet = activeJnc = ER::Junction::init(resource, 4, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_feedIn_averageShoulders = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->feedOut->getBasePosition()), (float*)&(module->owner->arms[0]->feedOut->getBasePositionImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->feedOut->getBasePosition()), (float*)&(module->owner->arms[1]->feedOut->getBasePositionImportanceRef()) );

  junc_feedIn_legsInContact = activeJnc = ER::Junction::init(resource, 2, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_spineInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_maxLegCollidingTime = activeJnc = ER::Junction::init(resource, 2, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getCollidingTime()), (float*)&(module->owner->legs[0]->feedOut->getCollidingTimeImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getCollidingTime()), (float*)&(module->owner->legs[1]->feedOut->getCollidingTimeImportanceRef()) );

  junc_feedIn_isBodyBarFeetContacting = activeJnc = ER::Junction::init(resource, 6, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getLimbBarEndIsColliding()), (float*)&(module->owner->legs[0]->feedOut->getLimbBarEndIsCollidingImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getLimbBarEndIsColliding()), (float*)&(module->owner->legs[1]->feedOut->getLimbBarEndIsCollidingImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_out_pelvisRotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->bodyBalance->out->getPelvisRotationRequest()), (float*)&(module->bodyBalance->out->getPelvisRotationRequestImportanceRef()) );

  junc_out_isInContact = activeJnc = ER::Junction::init(resource, 6, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedOut_leanVector = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->supportPolygon->out->getOffsetFromPolygon()), (float*)&(module->supportPolygon->out->getOffsetFromPolygonImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->sittingBodyBalance->feedOut->getLeanVector()), (float*)&(module->sittingBodyBalance->feedOut->getLeanVectorImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void BodyFrame_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodyFrame_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_instabilityPreparation = ER::Junction::relocate(resource);
  // junc_in_instabilityPreparation->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->out->getInstabilityPreparation()), (float*)&(module->owner->hazardManagement->out->getInstabilityPreparationImportanceRef()) );

  junc_feedOut_impendingInstability = ER::Junction::relocate(resource);
  // junc_feedOut_impendingInstability->getEdges()[0].reinit( (char*)&(module->in->getInstabilityPreparation()), (float*)&(module->in->getInstabilityPreparationImportanceRef()) );

  junc_feedOut_supportAmount = ER::Junction::relocate(resource);
  // junc_feedOut_supportAmount->getEdges()[0].reinit( (char*)&(module->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );
  // junc_feedOut_supportAmount->getEdges()[1].reinit( (char*)&(module->sittingBodyBalance->feedOut->getSitAmount()), (float*)&(module->sittingBodyBalance->feedOut->getSitAmountImportanceRef()) );
  // junc_feedOut_supportAmount->getEdges()[2].reinit( (char*)&(module->feedIn->getExternalSpineSupportAmount()), (float*)&(module->feedIn->getExternalSpineSupportAmountImportanceRef()) );

  junc_feedIn_averageHips = ER::Junction::relocate(resource);
  // junc_feedIn_averageHips->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getHipPosition()), (float*)&(module->owner->legs[0]->feedOut->getHipPositionImportanceRef()) );
  // junc_feedIn_averageHips->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getHipPosition()), (float*)&(module->owner->legs[1]->feedOut->getHipPositionImportanceRef()) );

  junc_feedIn_chestState = ER::Junction::relocate(resource);
  // junc_feedIn_chestState->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getChestState()), (float*)&(module->owner->spines[0]->feedOut->getChestStateImportanceRef()) );

  junc_feedIn_averageOfSupportingHands = ER::Junction::relocate(resource);
  // junc_feedIn_averageOfSupportingHands->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  // junc_feedIn_averageOfSupportingHands->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_feedIn_averageOfSupportingFeet = ER::Junction::relocate(resource);
  // junc_feedIn_averageOfSupportingFeet->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  // junc_feedIn_averageOfSupportingFeet->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_feedIn_averageOfHandsAndFeet = ER::Junction::relocate(resource);
  // junc_feedIn_averageOfHandsAndFeet->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  // junc_feedIn_averageOfHandsAndFeet->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  // junc_feedIn_averageOfHandsAndFeet->getEdges()[2].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );
  // junc_feedIn_averageOfHandsAndFeet->getEdges()[3].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_feedIn_averageShoulders = ER::Junction::relocate(resource);
  // junc_feedIn_averageShoulders->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->feedOut->getBasePosition()), (float*)&(module->owner->arms[0]->feedOut->getBasePositionImportanceRef()) );
  // junc_feedIn_averageShoulders->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->feedOut->getBasePosition()), (float*)&(module->owner->arms[1]->feedOut->getBasePositionImportanceRef()) );

  junc_feedIn_legsInContact = ER::Junction::relocate(resource);
  // junc_feedIn_legsInContact->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_legsInContact->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_spineInContact = ER::Junction::relocate(resource);
  // junc_feedIn_spineInContact->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_maxLegCollidingTime = ER::Junction::relocate(resource);
  // junc_feedIn_maxLegCollidingTime->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getCollidingTime()), (float*)&(module->owner->legs[0]->feedOut->getCollidingTimeImportanceRef()) );
  // junc_feedIn_maxLegCollidingTime->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getCollidingTime()), (float*)&(module->owner->legs[1]->feedOut->getCollidingTimeImportanceRef()) );

  junc_feedIn_isBodyBarFeetContacting = ER::Junction::relocate(resource);
  // junc_feedIn_isBodyBarFeetContacting->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getLimbBarEndIsColliding()), (float*)&(module->owner->legs[0]->feedOut->getLimbBarEndIsCollidingImportanceRef()) );
  // junc_feedIn_isBodyBarFeetContacting->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getLimbBarEndIsColliding()), (float*)&(module->owner->legs[1]->feedOut->getLimbBarEndIsCollidingImportanceRef()) );
  // junc_feedIn_isBodyBarFeetContacting->getEdges()[2].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_isBodyBarFeetContacting->getEdges()[3].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_isBodyBarFeetContacting->getEdges()[4].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_isBodyBarFeetContacting->getEdges()[5].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_out_pelvisRotationRequest = ER::Junction::relocate(resource);
  // junc_out_pelvisRotationRequest->getEdges()[0].reinit( (char*)&(module->bodyBalance->out->getPelvisRotationRequest()), (float*)&(module->bodyBalance->out->getPelvisRotationRequestImportanceRef()) );

  junc_out_isInContact = ER::Junction::relocate(resource);
  // junc_out_isInContact->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  // junc_out_isInContact->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );
  // junc_out_isInContact->getEdges()[2].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  // junc_out_isInContact->getEdges()[3].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );
  // junc_out_isInContact->getEdges()[4].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );
  // junc_out_isInContact->getEdges()[5].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedOut_leanVector = ER::Junction::relocate(resource);
  // junc_feedOut_leanVector->getEdges()[0].reinit( (char*)&(module->supportPolygon->out->getOffsetFromPolygon()), (float*)&(module->supportPolygon->out->getOffsetFromPolygonImportanceRef()) );
  // junc_feedOut_leanVector->getEdges()[1].reinit( (char*)&(module->sittingBodyBalance->feedOut->getLeanVector()), (float*)&(module->sittingBodyBalance->feedOut->getLeanVectorImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyFrame_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BodyFrame* module = (BodyFrame*)modulePtr;
  BodyFrameFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_averageHipsImportance = junc_feedIn_averageHips->combineAverage(&feedIn.averageHips);
  feedIn.m_chestStateImportance = junc_feedIn_chestState->combineDirectInput(&feedIn.chestState);
  feedIn.m_averageOfSupportingHandsImportance = junc_feedIn_averageOfSupportingHands->combineAverage(&feedIn.averageOfSupportingHands);
  feedIn.m_averageOfSupportingFeetImportance = junc_feedIn_averageOfSupportingFeet->combineAverage(&feedIn.averageOfSupportingFeet);
  feedIn.m_averageOfSupportingHandsAndFeetImportance = junc_feedIn_averageOfHandsAndFeet->combineAverage(&feedIn.averageOfSupportingHandsAndFeet);
  feedIn.m_averageShouldersImportance = junc_feedIn_averageShoulders->combineAverage(&feedIn.averageShoulders);
  feedIn.m_legsInContactImportance = junc_feedIn_legsInContact->combineIsBoolSet(&feedIn.legsInContact);
  feedIn.m_spineInContactImportance = junc_feedIn_spineInContact->combineDirectInput(&feedIn.spineInContact);
  feedIn.m_maxLegCollidingTimeImportance = junc_feedIn_maxLegCollidingTime->combineMaxFloatValue(&feedIn.maxLegCollidingTime);
  feedIn.m_isBodyBarFeetContactingImportance = junc_feedIn_isBodyBarFeetContacting->combineIsBoolSet(&feedIn.isBodyBarFeetContacting);
  feedIn.m_isInContactImportance = junc_out_isInContact->combineIsBoolSet(&feedIn.isInContact);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyFrame_Con::combineInputs(ER::Module* modulePtr)
{
  BodyFrame* module = (BodyFrame*)modulePtr;
  BodyFrameInputs& in = *module->in;

  // Junction assignments.
  in.m_instabilityPreparationImportance = junc_in_instabilityPreparation->combineDirectInput(&in.instabilityPreparation);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyFrame_Con::combineFeedbackOutputs(ER::Module* modulePtr)
{
  BodyFrame* module = (BodyFrame*)modulePtr;
  BodyFrameFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_impendingInstabilityImportance == 0.0f);
  feedOut.m_impendingInstabilityImportance = junc_feedOut_impendingInstability->combineDirectInput(&feedOut.impendingInstability);
  NMP_ASSERT(feedOut.m_supportAmountImportance == 0.0f);
  feedOut.m_supportAmountImportance = junc_feedOut_supportAmount->combineMaxFloatValue(&feedOut.supportAmount);
  NMP_ASSERT(feedOut.m_leanVectorImportance == 0.0f);
  feedOut.m_leanVectorImportance = junc_feedOut_leanVector->combineWinnerTakesAll(&feedOut.leanVector);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodyFrame_Con::combineOutputs(ER::Module* modulePtr)
{
  BodyFrame* module = (BodyFrame*)modulePtr;
  BodyFrameOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_pelvisRotationRequestImportance == 0.0f);
  out.m_pelvisRotationRequestImportance = junc_out_pelvisRotationRequest->combineDirectInput(&out.pelvisRotationRequest);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


