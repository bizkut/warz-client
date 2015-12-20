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
#include "BodySection.h"
#include "BodySectionPackaging.h"
#include "PositionCore.h"
#include "RotateCore.h"
#include "Leg.h"
#include "LegBrace.h"
#include "Spine.h"
#include "LegStandingSupport.h"
#include "BodyFrame.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "LegSpin.h"
#include "FreeFallManagement.h"
#include "Head.h"
#include "HeadPoint.h"
#include "HeadAvoid.h"
#include "HeadDodge.h"
#include "Arm.h"
#include "ArmBrace.h"
#include "ArmReachForWorld.h"
#include "ArmHoldingSupport.h"
#include "ArmReachForBodyPart.h"
#include "ArmStandingSupport.h"
#include "ArmSpin.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BodySection_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BodySection*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BodySection_Con::~BodySection_Con()
{
  junc_out_intermediateRR->destroy();
  junc_feedOut_controlledAmount->destroy();
  junc_out_translationRequest->destroy();
  junc_out_oneWayTranslationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BodySection_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BodySection_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BodySection_Con::buildConnections(BodySection* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodySection_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_out_oneWayTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->position->out->getOneWayTranslationRequest()), (float*)&(module->position->out->getOneWayTranslationRequestImportanceRef()) );

  junc_out_translationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->position->out->getTranslationUnsupportedRequest()), (float*)&(module->position->out->getTranslationUnsupportedRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->position->out->getTranslationRequest()), (float*)&(module->position->out->getTranslationRequestImportanceRef()) );

  junc_feedOut_controlledAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getControlledAmount()), (float*)&(module->feedIn->getControlledAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getExternallyControlledAmount()), (float*)&(module->feedIn->getExternallyControlledAmountImportanceRef()) );

  junc_out_intermediateRR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->rotate->out->getRotationUnsupportedRequest()), (float*)&(module->rotate->out->getRotationUnsupportedRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->rotate->out->getRotationRequest()), (float*)&(module->rotate->out->getRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void BodySection_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodySection_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_out_oneWayTranslationRequest = ER::Junction::relocate(resource);
  // junc_out_oneWayTranslationRequest->getEdges()[0].reinit( (char*)&(module->position->out->getOneWayTranslationRequest()), (float*)&(module->position->out->getOneWayTranslationRequestImportanceRef()) );

  junc_out_translationRequest = ER::Junction::relocate(resource);
  // junc_out_translationRequest->getEdges()[0].reinit( (char*)&(module->position->out->getTranslationUnsupportedRequest()), (float*)&(module->position->out->getTranslationUnsupportedRequestImportanceRef()) );
  // junc_out_translationRequest->getEdges()[1].reinit( (char*)&(module->position->out->getTranslationRequest()), (float*)&(module->position->out->getTranslationRequestImportanceRef()) );

  junc_feedOut_controlledAmount = ER::Junction::relocate(resource);
  // junc_feedOut_controlledAmount->getEdges()[0].reinit( (char*)&(module->feedIn->getControlledAmount()), (float*)&(module->feedIn->getControlledAmountImportanceRef()) );
  // junc_feedOut_controlledAmount->getEdges()[1].reinit( (char*)&(module->feedIn->getExternallyControlledAmount()), (float*)&(module->feedIn->getExternallyControlledAmountImportanceRef()) );

  junc_out_intermediateRR = ER::Junction::relocate(resource);
  // junc_out_intermediateRR->getEdges()[0].reinit( (char*)&(module->rotate->out->getRotationUnsupportedRequest()), (float*)&(module->rotate->out->getRotationUnsupportedRequestImportanceRef()) );
  // junc_out_intermediateRR->getEdges()[1].reinit( (char*)&(module->rotate->out->getRotationRequest()), (float*)&(module->rotate->out->getRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_Con::combineFeedbackOutputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_controlledAmountImportance == 0.0f);
  feedOut.m_controlledAmountImportance = junc_feedOut_controlledAmount->combineMaxFloatValue(&feedOut.controlledAmount);
  PostCombiners::ClampWeight(feedOut.controlledAmount);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_Con::combineOutputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_oneWayTranslationRequestImportance == 0.0f);
  out.m_oneWayTranslationRequestImportance = junc_out_oneWayTranslationRequest->combineDirectInput(&out.oneWayTranslationRequest);
  NMP_ASSERT(out.m_translationRequestImportance == 0.0f);
  out.m_translationRequestImportance = junc_out_translationRequest->combineAverage(&out.translationRequest);
  NMP_ASSERT(out.m_intermediateRRImportance == 0.0f);
  out.m_intermediateRRImportance = junc_out_intermediateRR->combineAverage(&out.intermediateRR);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void BodySection_lowerCon::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BodySection*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BodySection_lowerCon::~BodySection_lowerCon()
{
  junc_out_rotationRequest->destroy();
  junc_feedIn_limbTargetDirection->destroy();
  junc_out_contactDirectionRequest->destroy();
  junc_feedIn_cushionPoint->destroy();
  junc_feedIn_braceStates_1->destroy();
  junc_feedIn_braceStates_0->destroy();
  junc_out_oneWayRotationRequest->destroy();
  junc_feedIn_inContact->destroy();
  junc_feedIn_externallyControlledAmount->destroy();
  junc_feedIn_supportingAmount->destroy();
  junc_feedIn_oneWayTranslationRequest->destroy();
  junc_feedIn_oneWayRotationRequest->destroy();
  junc_rotationRequest->destroy();
  junc_out_intermediateRR->destroy();
  junc_feedOut_controlledAmount->destroy();
  junc_out_translationRequest->destroy();
  junc_out_oneWayTranslationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BodySection_lowerCon::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BodySection_lowerCon), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BodySection_lowerCon::buildConnections(BodySection* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodySection_lowerCon), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_out_oneWayTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->position->out->getOneWayTranslationRequest()), (float*)&(module->position->out->getOneWayTranslationRequestImportanceRef()) );

  junc_out_translationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->position->out->getTranslationUnsupportedRequest()), (float*)&(module->position->out->getTranslationUnsupportedRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->position->out->getTranslationRequest()), (float*)&(module->position->out->getTranslationRequestImportanceRef()) );

  junc_feedOut_controlledAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getControlledAmount()), (float*)&(module->feedIn->getControlledAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getExternallyControlledAmount()), (float*)&(module->feedIn->getExternallyControlledAmountImportanceRef()) );

  junc_out_intermediateRR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->rotate->out->getRotationUnsupportedRequest()), (float*)&(module->rotate->out->getRotationUnsupportedRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->rotate->out->getRotationRequest()), (float*)&(module->rotate->out->getRotationRequestImportanceRef()) );

  junc_rotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->legs[0]->brace->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->legs[1]->brace->feedOut->getRootRotationRequestImportanceRef()) );

  junc_feedIn_oneWayRotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyRotationRequest()), (float*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyRotationRequestImportanceRef()) );

  junc_feedIn_oneWayTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyTranslationRequest()), (float*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyTranslationRequestImportanceRef()) );

  junc_feedIn_supportingAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::sum);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getPelvisControlAmount()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getPelvisControlAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getPelvisControlAmount()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getPelvisControlAmountImportanceRef()) );

  junc_feedIn_externallyControlledAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getChestControlledAmount()), (float*)&(module->owner->spines[0]->feedOut->getChestControlledAmountImportanceRef()) );

  junc_feedIn_inContact = activeJnc = ER::Junction::init(resource, 3, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_out_oneWayRotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->rotate->out->getOneWayRotationRequest()), (float*)&(module->rotate->out->getOneWayRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->bodyFrame->out->getPelvisRotationRequest()), (float*)&(module->owner->bodyFrame->out->getPelvisRotationRequestImportanceRef()) );

  junc_feedIn_braceStates_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->brace->feedOut->getBraceState()), (float*)&(module->owner->legs[0]->brace->feedOut->getBraceStateImportanceRef()) );
  junc_feedIn_braceStates_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[1]->brace->feedOut->getBraceState()), (float*)&(module->owner->legs[1]->brace->feedOut->getBraceStateImportanceRef()) );

  junc_feedIn_cushionPoint = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->legs[0]->brace->feedOut->getEndCushionPointImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->legs[1]->brace->feedOut->getEndCushionPointImportanceRef()) );

  junc_out_contactDirectionRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->out->getUprightContactDir()), (float*)&(module->owner->hazardManagement->grab->out->getUprightContactDirImportanceRef()) );

  junc_feedIn_limbTargetDirection = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->legs[0]->spin->feedOut->getTargetDirectionImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->legs[1]->spin->feedOut->getTargetDirectionImportanceRef()) );

  junc_out_rotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->out->getIntermediateRR()), (float*)&(module->out->getIntermediateRRImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getLegsSpinRotationRequest()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getLegsSpinRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void BodySection_lowerCon::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodySection_lowerCon), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_out_oneWayTranslationRequest = ER::Junction::relocate(resource);
  // junc_out_oneWayTranslationRequest->getEdges()[0].reinit( (char*)&(module->position->out->getOneWayTranslationRequest()), (float*)&(module->position->out->getOneWayTranslationRequestImportanceRef()) );

  junc_out_translationRequest = ER::Junction::relocate(resource);
  // junc_out_translationRequest->getEdges()[0].reinit( (char*)&(module->position->out->getTranslationUnsupportedRequest()), (float*)&(module->position->out->getTranslationUnsupportedRequestImportanceRef()) );
  // junc_out_translationRequest->getEdges()[1].reinit( (char*)&(module->position->out->getTranslationRequest()), (float*)&(module->position->out->getTranslationRequestImportanceRef()) );

  junc_feedOut_controlledAmount = ER::Junction::relocate(resource);
  // junc_feedOut_controlledAmount->getEdges()[0].reinit( (char*)&(module->feedIn->getControlledAmount()), (float*)&(module->feedIn->getControlledAmountImportanceRef()) );
  // junc_feedOut_controlledAmount->getEdges()[1].reinit( (char*)&(module->feedIn->getExternallyControlledAmount()), (float*)&(module->feedIn->getExternallyControlledAmountImportanceRef()) );

  junc_out_intermediateRR = ER::Junction::relocate(resource);
  // junc_out_intermediateRR->getEdges()[0].reinit( (char*)&(module->rotate->out->getRotationUnsupportedRequest()), (float*)&(module->rotate->out->getRotationUnsupportedRequestImportanceRef()) );
  // junc_out_intermediateRR->getEdges()[1].reinit( (char*)&(module->rotate->out->getRotationRequest()), (float*)&(module->rotate->out->getRotationRequestImportanceRef()) );

  junc_rotationRequest = ER::Junction::relocate(resource);
  // junc_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->legs[0]->brace->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->legs[1]->brace->feedOut->getRootRotationRequestImportanceRef()) );

  junc_feedIn_oneWayRotationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_oneWayRotationRequest->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyRotationRequest()), (float*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyRotationRequestImportanceRef()) );

  junc_feedIn_oneWayTranslationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_oneWayTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyTranslationRequest()), (float*)&(module->owner->spines[0]->feedOut->getDownstreamOnlyTranslationRequestImportanceRef()) );

  junc_feedIn_supportingAmount = ER::Junction::relocate(resource);
  // junc_feedIn_supportingAmount->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getPelvisControlAmount()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getPelvisControlAmountImportanceRef()) );
  // junc_feedIn_supportingAmount->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getPelvisControlAmount()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getPelvisControlAmountImportanceRef()) );

  junc_feedIn_externallyControlledAmount = ER::Junction::relocate(resource);
  // junc_feedIn_externallyControlledAmount->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getChestControlledAmount()), (float*)&(module->owner->spines[0]->feedOut->getChestControlledAmountImportanceRef()) );

  junc_feedIn_inContact = ER::Junction::relocate(resource);
  // junc_feedIn_inContact->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_inContact->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_inContact->getEdges()[2].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_out_oneWayRotationRequest = ER::Junction::relocate(resource);
  // junc_out_oneWayRotationRequest->getEdges()[0].reinit( (char*)&(module->rotate->out->getOneWayRotationRequest()), (float*)&(module->rotate->out->getOneWayRotationRequestImportanceRef()) );
  // junc_out_oneWayRotationRequest->getEdges()[1].reinit( (char*)&(module->owner->bodyFrame->out->getPelvisRotationRequest()), (float*)&(module->owner->bodyFrame->out->getPelvisRotationRequestImportanceRef()) );

  junc_feedIn_braceStates_0 = ER::Junction::relocate(resource);
  // junc_feedIn_braceStates_0->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->brace->feedOut->getBraceState()), (float*)&(module->owner->legs[0]->brace->feedOut->getBraceStateImportanceRef()) );
  junc_feedIn_braceStates_1 = ER::Junction::relocate(resource);
  // junc_feedIn_braceStates_1->getEdges()[0].reinit( (char*)&(module->owner->legs[1]->brace->feedOut->getBraceState()), (float*)&(module->owner->legs[1]->brace->feedOut->getBraceStateImportanceRef()) );

  junc_feedIn_cushionPoint = ER::Junction::relocate(resource);
  // junc_feedIn_cushionPoint->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->legs[0]->brace->feedOut->getEndCushionPointImportanceRef()) );
  // junc_feedIn_cushionPoint->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->legs[1]->brace->feedOut->getEndCushionPointImportanceRef()) );

  junc_out_contactDirectionRequest = ER::Junction::relocate(resource);
  // junc_out_contactDirectionRequest->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->out->getUprightContactDir()), (float*)&(module->owner->hazardManagement->grab->out->getUprightContactDirImportanceRef()) );

  junc_feedIn_limbTargetDirection = ER::Junction::relocate(resource);
  // junc_feedIn_limbTargetDirection->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->legs[0]->spin->feedOut->getTargetDirectionImportanceRef()) );
  // junc_feedIn_limbTargetDirection->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->legs[1]->spin->feedOut->getTargetDirectionImportanceRef()) );

  junc_out_rotationRequest = ER::Junction::relocate(resource);
  // junc_out_rotationRequest->getEdges()[0].reinit( (char*)&(module->out->getIntermediateRR()), (float*)&(module->out->getIntermediateRRImportanceRef()) );
  // junc_out_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getLegsSpinRotationRequest()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getLegsSpinRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_lowerCon::combineFeedbackInputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_rotationRequestImportance = junc_rotationRequest->combineAverage(&feedIn.rotationRequest);
  feedIn.m_oneWayRotationRequestImportance = junc_feedIn_oneWayRotationRequest->combineDirectInput(&feedIn.oneWayRotationRequest);
  feedIn.m_oneWayTranslationRequestImportance = junc_feedIn_oneWayTranslationRequest->combineDirectInput(&feedIn.oneWayTranslationRequest);
  feedIn.m_controlledAmountImportance = junc_feedIn_supportingAmount->combineSum(&feedIn.controlledAmount);
  PostCombiners::ClampWeight(feedIn.controlledAmount);
  feedIn.m_externallyControlledAmountImportance = junc_feedIn_externallyControlledAmount->combineDirectInput(&feedIn.externallyControlledAmount);
  feedIn.m_inContactImportance = junc_feedIn_inContact->combineIsBoolSet(&feedIn.inContact);
  feedIn.m_braceStatesImportance[0] = junc_feedIn_braceStates_0->combineDirectInput(&feedIn.braceStates[0]);
  feedIn.m_braceStatesImportance[1] = junc_feedIn_braceStates_1->combineDirectInput(&feedIn.braceStates[1]);
  feedIn.m_cushionPointImportance = junc_feedIn_cushionPoint->combineAverage(&feedIn.cushionPoint);
  feedIn.m_spinDirectionImportance = junc_feedIn_limbTargetDirection->combineAverage(&feedIn.spinDirection);
  PostCombiners::Vector3Normalise(feedIn.spinDirection);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_lowerCon::combineFeedbackOutputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_controlledAmountImportance == 0.0f);
  feedOut.m_controlledAmountImportance = junc_feedOut_controlledAmount->combineMaxFloatValue(&feedOut.controlledAmount);
  PostCombiners::ClampWeight(feedOut.controlledAmount);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_lowerCon::combineOutputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_oneWayTranslationRequestImportance == 0.0f);
  out.m_oneWayTranslationRequestImportance = junc_out_oneWayTranslationRequest->combineDirectInput(&out.oneWayTranslationRequest);
  NMP_ASSERT(out.m_translationRequestImportance == 0.0f);
  out.m_translationRequestImportance = junc_out_translationRequest->combineAverage(&out.translationRequest);
  NMP_ASSERT(out.m_intermediateRRImportance == 0.0f);
  out.m_intermediateRRImportance = junc_out_intermediateRR->combineAverage(&out.intermediateRR);
  NMP_ASSERT(out.m_oneWayRotationRequestImportance == 0.0f);
  out.m_oneWayRotationRequestImportance = junc_out_oneWayRotationRequest->combinePriority(&out.oneWayRotationRequest);
  NMP_ASSERT(out.m_contactDirectionRequestImportance == 0.0f);
  out.m_contactDirectionRequestImportance = junc_out_contactDirectionRequest->combineDirectInput(&out.contactDirectionRequest);
  NMP_ASSERT(out.m_rotationRequestImportance == 0.0f);
  out.m_rotationRequestImportance = junc_out_rotationRequest->combinePriority(&out.rotationRequest);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void BodySection_upperCon::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BodySection*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BodySection_upperCon::~BodySection_upperCon()
{
  junc_out_rotationRequest->destroy();
  junc_feedIn_limbTargetDirection->destroy();
  junc_out_oneWayRotationRequest->destroy();
  junc_feedIn_cushionPoint->destroy();
  junc_feedIn_braceStates_1->destroy();
  junc_feedIn_braceStates_0->destroy();
  junc_feedIn_inContact->destroy();
  junc_feedIn_supportingAmount->destroy();
  junc_feedIn_externallyControlledAmount->destroy();
  junc_feedIn_oneWayTranslationRequest->destroy();
  junc_feedIn_oneWayRotationRequest->destroy();
  junc_feedIn_translationRequest->destroy();
  junc_feedIn_rotationRequest->destroy();
  junc_in_translationRequest->destroy();
  junc_in_rotationRequest->destroy();
  junc_out_intermediateRR->destroy();
  junc_feedOut_controlledAmount->destroy();
  junc_out_translationRequest->destroy();
  junc_out_oneWayTranslationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BodySection_upperCon::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BodySection_upperCon), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(9);
  result += ER::Junction::getMemoryRequirements(6);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BodySection_upperCon::buildConnections(BodySection* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodySection_upperCon), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_out_oneWayTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->position->out->getOneWayTranslationRequest()), (float*)&(module->position->out->getOneWayTranslationRequestImportanceRef()) );

  junc_out_translationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->position->out->getTranslationUnsupportedRequest()), (float*)&(module->position->out->getTranslationUnsupportedRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->position->out->getTranslationRequest()), (float*)&(module->position->out->getTranslationRequestImportanceRef()) );

  junc_feedOut_controlledAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getControlledAmount()), (float*)&(module->feedIn->getControlledAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->feedIn->getExternallyControlledAmount()), (float*)&(module->feedIn->getExternallyControlledAmountImportanceRef()) );

  junc_out_intermediateRR = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->rotate->out->getRotationUnsupportedRequest()), (float*)&(module->rotate->out->getRotationUnsupportedRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->rotate->out->getRotationRequest()), (float*)&(module->rotate->out->getRotationRequestImportanceRef()) );

  junc_in_rotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->out->getRotationRequest()), (float*)&(module->owner->hazardManagement->grab->out->getRotationRequestImportanceRef()) );

  junc_in_translationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->out->getTranslationRequest()), (float*)&(module->owner->hazardManagement->grab->out->getTranslationRequestImportanceRef()) );

  junc_feedIn_rotationRequest = activeJnc = ER::Junction::init(resource, 9, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->point->feedOut->getRootRotationRequest()), (float*)&(module->owner->heads[0]->point->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->heads[0]->avoid->feedOut->getRootRotationRequest()), (float*)&(module->owner->heads[0]->avoid->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->heads[0]->dodge->feedOut->getRootRotationRequest()), (float*)&(module->owner->heads[0]->dodge->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->arms[0]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[0]->brace->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->arms[1]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[1]->brace->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[6].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[7].reinit( (char*)&(module->owner->arms[0]->holdingSupport->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[0]->holdingSupport->feedOut->getRootRotationRequestImportanceRef()) );
  activeJnc->getEdges()[8].reinit( (char*)&(module->owner->arms[1]->holdingSupport->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[1]->holdingSupport->feedOut->getRootRotationRequestImportanceRef()) );

  junc_feedIn_translationRequest = activeJnc = ER::Junction::init(resource, 6, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->dodge->feedOut->getRootTranslationRequest()), (float*)&(module->owner->heads[0]->dodge->feedOut->getRootTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->heads[0]->avoid->feedOut->getRootTranslationRequest()), (float*)&(module->owner->heads[0]->avoid->feedOut->getRootTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestTranslationRequestImportanceRef()) );

  junc_feedIn_oneWayRotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyRotationRequest()), (float*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyRotationRequestImportanceRef()) );

  junc_feedIn_oneWayTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyTranslationRequest()), (float*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyTranslationRequestImportanceRef()) );

  junc_feedIn_externallyControlledAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getPelvisControlledAmount()), (float*)&(module->owner->spines[0]->feedOut->getPelvisControlledAmountImportanceRef()) );

  junc_feedIn_supportingAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::sum);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getChestControlAmount()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getChestControlAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getChestControlAmount()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getChestControlAmountImportanceRef()) );

  junc_feedIn_inContact = activeJnc = ER::Junction::init(resource, 4, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_braceStates_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->brace->feedOut->getBraceState()), (float*)&(module->owner->arms[0]->brace->feedOut->getBraceStateImportanceRef()) );
  junc_feedIn_braceStates_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->brace->feedOut->getBraceState()), (float*)&(module->owner->arms[1]->brace->feedOut->getBraceStateImportanceRef()) );

  junc_feedIn_cushionPoint = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->arms[0]->brace->feedOut->getEndCushionPointImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->arms[1]->brace->feedOut->getEndCushionPointImportanceRef()) );

  junc_out_oneWayRotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->rotate->out->getOneWayRotationRequest()), (float*)&(module->rotate->out->getOneWayRotationRequestImportanceRef()) );

  junc_feedIn_limbTargetDirection = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->arms[0]->spin->feedOut->getTargetDirectionImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->arms[1]->spin->feedOut->getTargetDirectionImportanceRef()) );

  junc_out_rotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->out->getIntermediateRR()), (float*)&(module->out->getIntermediateRRImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getArmsSpinRotationRequest()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getArmsSpinRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void BodySection_upperCon::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BodySection_upperCon), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_out_oneWayTranslationRequest = ER::Junction::relocate(resource);
  // junc_out_oneWayTranslationRequest->getEdges()[0].reinit( (char*)&(module->position->out->getOneWayTranslationRequest()), (float*)&(module->position->out->getOneWayTranslationRequestImportanceRef()) );

  junc_out_translationRequest = ER::Junction::relocate(resource);
  // junc_out_translationRequest->getEdges()[0].reinit( (char*)&(module->position->out->getTranslationUnsupportedRequest()), (float*)&(module->position->out->getTranslationUnsupportedRequestImportanceRef()) );
  // junc_out_translationRequest->getEdges()[1].reinit( (char*)&(module->position->out->getTranslationRequest()), (float*)&(module->position->out->getTranslationRequestImportanceRef()) );

  junc_feedOut_controlledAmount = ER::Junction::relocate(resource);
  // junc_feedOut_controlledAmount->getEdges()[0].reinit( (char*)&(module->feedIn->getControlledAmount()), (float*)&(module->feedIn->getControlledAmountImportanceRef()) );
  // junc_feedOut_controlledAmount->getEdges()[1].reinit( (char*)&(module->feedIn->getExternallyControlledAmount()), (float*)&(module->feedIn->getExternallyControlledAmountImportanceRef()) );

  junc_out_intermediateRR = ER::Junction::relocate(resource);
  // junc_out_intermediateRR->getEdges()[0].reinit( (char*)&(module->rotate->out->getRotationUnsupportedRequest()), (float*)&(module->rotate->out->getRotationUnsupportedRequestImportanceRef()) );
  // junc_out_intermediateRR->getEdges()[1].reinit( (char*)&(module->rotate->out->getRotationRequest()), (float*)&(module->rotate->out->getRotationRequestImportanceRef()) );

  junc_in_rotationRequest = ER::Junction::relocate(resource);
  // junc_in_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->out->getRotationRequest()), (float*)&(module->owner->hazardManagement->grab->out->getRotationRequestImportanceRef()) );

  junc_in_translationRequest = ER::Junction::relocate(resource);
  // junc_in_translationRequest->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->out->getTranslationRequest()), (float*)&(module->owner->hazardManagement->grab->out->getTranslationRequestImportanceRef()) );

  junc_feedIn_rotationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->point->feedOut->getRootRotationRequest()), (float*)&(module->owner->heads[0]->point->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->heads[0]->avoid->feedOut->getRootRotationRequest()), (float*)&(module->owner->heads[0]->avoid->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[2].reinit( (char*)&(module->owner->heads[0]->dodge->feedOut->getRootRotationRequest()), (float*)&(module->owner->heads[0]->dodge->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[3].reinit( (char*)&(module->owner->arms[0]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[0]->brace->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[4].reinit( (char*)&(module->owner->arms[1]->brace->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[1]->brace->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[5].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[6].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[7].reinit( (char*)&(module->owner->arms[0]->holdingSupport->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[0]->holdingSupport->feedOut->getRootRotationRequestImportanceRef()) );
  // junc_feedIn_rotationRequest->getEdges()[8].reinit( (char*)&(module->owner->arms[1]->holdingSupport->feedOut->getRootRotationRequest()), (float*)&(module->owner->arms[1]->holdingSupport->feedOut->getRootRotationRequestImportanceRef()) );

  junc_feedIn_translationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_translationRequest->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->dodge->feedOut->getRootTranslationRequest()), (float*)&(module->owner->heads[0]->dodge->feedOut->getRootTranslationRequestImportanceRef()) );
  // junc_feedIn_translationRequest->getEdges()[1].reinit( (char*)&(module->owner->heads[0]->avoid->feedOut->getRootTranslationRequest()), (float*)&(module->owner->heads[0]->avoid->feedOut->getRootTranslationRequestImportanceRef()) );
  // junc_feedIn_translationRequest->getEdges()[2].reinit( (char*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[0]->reachForWorld->feedOut->getChestTranslationRequestImportanceRef()) );
  // junc_feedIn_translationRequest->getEdges()[3].reinit( (char*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[1]->reachForWorld->feedOut->getChestTranslationRequestImportanceRef()) );
  // junc_feedIn_translationRequest->getEdges()[4].reinit( (char*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[0]->reachForBodyPart->feedOut->getChestTranslationRequestImportanceRef()) );
  // junc_feedIn_translationRequest->getEdges()[5].reinit( (char*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestTranslationRequest()), (float*)&(module->owner->arms[1]->reachForBodyPart->feedOut->getChestTranslationRequestImportanceRef()) );

  junc_feedIn_oneWayRotationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_oneWayRotationRequest->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyRotationRequest()), (float*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyRotationRequestImportanceRef()) );

  junc_feedIn_oneWayTranslationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_oneWayTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyTranslationRequest()), (float*)&(module->owner->spines[0]->feedOut->getUpstreamOnlyTranslationRequestImportanceRef()) );

  junc_feedIn_externallyControlledAmount = ER::Junction::relocate(resource);
  // junc_feedIn_externallyControlledAmount->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getPelvisControlledAmount()), (float*)&(module->owner->spines[0]->feedOut->getPelvisControlledAmountImportanceRef()) );

  junc_feedIn_supportingAmount = ER::Junction::relocate(resource);
  // junc_feedIn_supportingAmount->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getChestControlAmount()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getChestControlAmountImportanceRef()) );
  // junc_feedIn_supportingAmount->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getChestControlAmount()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getChestControlAmountImportanceRef()) );

  junc_feedIn_inContact = ER::Junction::relocate(resource);
  // junc_feedIn_inContact->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_inContact->getEdges()[1].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_inContact->getEdges()[2].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_inContact->getEdges()[3].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_braceStates_0 = ER::Junction::relocate(resource);
  // junc_feedIn_braceStates_0->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->brace->feedOut->getBraceState()), (float*)&(module->owner->arms[0]->brace->feedOut->getBraceStateImportanceRef()) );
  junc_feedIn_braceStates_1 = ER::Junction::relocate(resource);
  // junc_feedIn_braceStates_1->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->brace->feedOut->getBraceState()), (float*)&(module->owner->arms[1]->brace->feedOut->getBraceStateImportanceRef()) );

  junc_feedIn_cushionPoint = ER::Junction::relocate(resource);
  // junc_feedIn_cushionPoint->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->arms[0]->brace->feedOut->getEndCushionPointImportanceRef()) );
  // junc_feedIn_cushionPoint->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->brace->feedOut->getEndCushionPoint()), (float*)&(module->owner->arms[1]->brace->feedOut->getEndCushionPointImportanceRef()) );

  junc_out_oneWayRotationRequest = ER::Junction::relocate(resource);
  // junc_out_oneWayRotationRequest->getEdges()[0].reinit( (char*)&(module->rotate->out->getOneWayRotationRequest()), (float*)&(module->rotate->out->getOneWayRotationRequestImportanceRef()) );

  junc_feedIn_limbTargetDirection = ER::Junction::relocate(resource);
  // junc_feedIn_limbTargetDirection->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->arms[0]->spin->feedOut->getTargetDirectionImportanceRef()) );
  // junc_feedIn_limbTargetDirection->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->spin->feedOut->getTargetDirection()), (float*)&(module->owner->arms[1]->spin->feedOut->getTargetDirectionImportanceRef()) );

  junc_out_rotationRequest = ER::Junction::relocate(resource);
  // junc_out_rotationRequest->getEdges()[0].reinit( (char*)&(module->out->getIntermediateRR()), (float*)&(module->out->getIntermediateRRImportanceRef()) );
  // junc_out_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getArmsSpinRotationRequest()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getArmsSpinRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_upperCon::combineFeedbackInputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_rotationRequestImportance = junc_feedIn_rotationRequest->combineAverage(&feedIn.rotationRequest);
  feedIn.m_translationRequestImportance = junc_feedIn_translationRequest->combineAverage(&feedIn.translationRequest);
  feedIn.m_oneWayRotationRequestImportance = junc_feedIn_oneWayRotationRequest->combineDirectInput(&feedIn.oneWayRotationRequest);
  feedIn.m_oneWayTranslationRequestImportance = junc_feedIn_oneWayTranslationRequest->combineDirectInput(&feedIn.oneWayTranslationRequest);
  feedIn.m_externallyControlledAmountImportance = junc_feedIn_externallyControlledAmount->combineDirectInput(&feedIn.externallyControlledAmount);
  feedIn.m_controlledAmountImportance = junc_feedIn_supportingAmount->combineSum(&feedIn.controlledAmount);
  PostCombiners::ClampWeight(feedIn.controlledAmount);
  feedIn.m_inContactImportance = junc_feedIn_inContact->combineIsBoolSet(&feedIn.inContact);
  feedIn.m_braceStatesImportance[0] = junc_feedIn_braceStates_0->combineDirectInput(&feedIn.braceStates[0]);
  feedIn.m_braceStatesImportance[1] = junc_feedIn_braceStates_1->combineDirectInput(&feedIn.braceStates[1]);
  feedIn.m_cushionPointImportance = junc_feedIn_cushionPoint->combineAverage(&feedIn.cushionPoint);
  feedIn.m_spinDirectionImportance = junc_feedIn_limbTargetDirection->combineAverage(&feedIn.spinDirection);
  PostCombiners::Vector3Normalise(feedIn.spinDirection);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_upperCon::combineInputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionInputs& in = *module->in;

  // Junction assignments.
  in.m_rotationRequestImportance = junc_in_rotationRequest->combineDirectInput(&in.rotationRequest);
  in.m_translationRequestImportance = junc_in_translationRequest->combineDirectInput(&in.translationRequest);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_upperCon::combineFeedbackOutputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_controlledAmountImportance == 0.0f);
  feedOut.m_controlledAmountImportance = junc_feedOut_controlledAmount->combineMaxFloatValue(&feedOut.controlledAmount);
  PostCombiners::ClampWeight(feedOut.controlledAmount);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BodySection_upperCon::combineOutputs(ER::Module* modulePtr)
{
  BodySection* module = (BodySection*)modulePtr;
  BodySectionOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_oneWayTranslationRequestImportance == 0.0f);
  out.m_oneWayTranslationRequestImportance = junc_out_oneWayTranslationRequest->combineDirectInput(&out.oneWayTranslationRequest);
  NMP_ASSERT(out.m_translationRequestImportance == 0.0f);
  out.m_translationRequestImportance = junc_out_translationRequest->combineAverage(&out.translationRequest);
  NMP_ASSERT(out.m_intermediateRRImportance == 0.0f);
  out.m_intermediateRRImportance = junc_out_intermediateRR->combineAverage(&out.intermediateRR);
  NMP_ASSERT(out.m_oneWayRotationRequestImportance == 0.0f);
  out.m_oneWayRotationRequestImportance = junc_out_oneWayRotationRequest->combineDirectInput(&out.oneWayRotationRequest);
  NMP_ASSERT(out.m_rotationRequestImportance == 0.0f);
  out.m_rotationRequestImportance = junc_out_rotationRequest->combinePriority(&out.rotationRequest);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


