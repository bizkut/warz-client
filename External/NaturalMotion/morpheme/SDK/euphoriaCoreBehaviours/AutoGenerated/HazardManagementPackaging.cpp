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
#include "HazardManagement.h"
#include "HazardManagementPackaging.h"
#include "HazardResponse.h"
#include "BodyFrame.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HazardManagement_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HazardManagement*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HazardManagement_Con::~HazardManagement_Con()
{
  junc_feedIn_chestState->destroy();
  junc_feedIn_supportAmount->destroy();
  junc_out_instabilityPreparation->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HazardManagement_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HazardManagement_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HazardManagement_Con::buildConnections(HazardManagement* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HazardManagement_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_out_instabilityPreparation = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->hazardResponse->out->getInstabilityPreparation()), (float*)&(module->hazardResponse->out->getInstabilityPreparationImportanceRef()) );

  junc_feedIn_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_chestState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getChestState()), (float*)&(module->owner->bodyFrame->feedOut->getChestStateImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HazardManagement_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HazardManagement_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_out_instabilityPreparation = ER::Junction::relocate(resource);
  // junc_out_instabilityPreparation->getEdges()[0].reinit( (char*)&(module->hazardResponse->out->getInstabilityPreparation()), (float*)&(module->hazardResponse->out->getInstabilityPreparationImportanceRef()) );

  junc_feedIn_supportAmount = ER::Junction::relocate(resource);
  // junc_feedIn_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_chestState = ER::Junction::relocate(resource);
  // junc_feedIn_chestState->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getChestState()), (float*)&(module->owner->bodyFrame->feedOut->getChestStateImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HazardManagement_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  HazardManagement* module = (HazardManagement*)modulePtr;
  HazardManagementFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportAmountImportance = junc_feedIn_supportAmount->combineDirectInput(&feedIn.supportAmount);
  feedIn.m_chestStateImportance = junc_feedIn_chestState->combineDirectInput(&feedIn.chestState);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HazardManagement_Con::combineOutputs(ER::Module* modulePtr)
{
  HazardManagement* module = (HazardManagement*)modulePtr;
  HazardManagementOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_instabilityPreparationImportance == 0.0f);
  out.m_instabilityPreparationImportance = junc_out_instabilityPreparation->combineDirectInput(&out.instabilityPreparation);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


