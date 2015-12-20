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
#include "HazardManagement.h"
#include "FreeFallManagement.h"
#include "FreeFallManagementPackaging.h"
#include "MyNetwork.h"
#include "FreeFallBehaviourInterface.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "Leg.h"
#include "LegStandingSupport.h"
#include "Spine.h"
#include "ImpactPredictor.h"
#include "BodyFrame.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((FreeFallManagement*)module);
}

//----------------------------------------------------------------------------------------------------------------------
FreeFallManagement_Con::~FreeFallManagement_Con()
{
  junc_feedIn_isInContact->destroy();
  junc_feedIn_chestHazard->destroy();
  junc_feedIn_supportAmount->destroy();
  junc_in_freefallParams->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format FreeFallManagement_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(FreeFallManagement_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(5);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement_Con::buildConnections(FreeFallManagement* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(FreeFallManagement_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_freefallParams = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getFreefallParams()), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getFreefallParamsImportanceRef()) );

  junc_feedIn_supportAmount = activeJnc = ER::Junction::init(resource, 5, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

  junc_feedIn_chestHazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

  junc_feedIn_isInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getIsInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getIsInContactImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(FreeFallManagement_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_freefallParams = ER::Junction::relocate(resource);
  // junc_in_freefallParams->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getFreefallParams()), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getFreefallParamsImportanceRef()) );

  junc_feedIn_supportAmount = ER::Junction::relocate(resource);
  // junc_feedIn_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_supportAmount->getEdges()[1].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_supportAmount->getEdges()[2].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_supportAmount->getEdges()[3].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_supportAmount->getEdges()[4].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

  junc_feedIn_chestHazard = ER::Junction::relocate(resource);
  // junc_feedIn_chestHazard->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

  junc_feedIn_isInContact = ER::Junction::relocate(resource);
  // junc_feedIn_isInContact->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getIsInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getIsInContactImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void FreeFallManagement_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  FreeFallManagement* module = (FreeFallManagement*)modulePtr;
  FreeFallManagementFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportAmountImportance = junc_feedIn_supportAmount->combineMaxFloatValue(&feedIn.supportAmount);
  feedIn.m_chestHazardImportance = junc_feedIn_chestHazard->combineDirectInput(&feedIn.chestHazard);
  feedIn.m_isInContactImportance = junc_feedIn_isInContact->combineDirectInput(&feedIn.isInContact);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void FreeFallManagement_Con::combineInputs(ER::Module* modulePtr)
{
  FreeFallManagement* module = (FreeFallManagement*)modulePtr;
  FreeFallManagementInputs& in = *module->in;

  // Junction assignments.
  in.m_freefallParamsImportance = junc_in_freefallParams->combineDirectInput(&in.freefallParams);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


