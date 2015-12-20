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
#include "ShieldManagement.h"
#include "ShieldManagementPackaging.h"
#include "MyNetwork.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "HazardResponse.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ShieldManagement*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ShieldManagement_Con::~ShieldManagement_Con()
{
  junc_feedIn_supportTM->destroy();
  junc_wta04->destroy();
  junc_wta03->destroy();
  junc_wta02->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ShieldManagement_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ShieldManagement_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement_Con::buildConnections(ShieldManagement* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ShieldManagement_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_wta02 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getTurnAwayScale()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getTurnAwayScaleImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getTurnAwayScale()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getTurnAwayScaleImportanceRef()) );

  junc_wta03 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getSmoothReturnTP()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getSmoothReturnTPImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSmoothReturnTP()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSmoothReturnTPImportanceRef()) );

  junc_wta04 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardResponse->out->getShieldDirectionToHazard()), (float*)&(module->owner->hazardResponse->out->getShieldDirectionToHazardImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getShieldActionDirectionToHazard()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getShieldActionDirectionToHazardImportanceRef()) );

  junc_feedIn_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ShieldManagement_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_wta02 = ER::Junction::relocate(resource);
  // junc_wta02->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getTurnAwayScale()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getTurnAwayScaleImportanceRef()) );
  // junc_wta02->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getTurnAwayScale()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getTurnAwayScaleImportanceRef()) );

  junc_wta03 = ER::Junction::relocate(resource);
  // junc_wta03->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getSmoothReturnTP()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getSmoothReturnTPImportanceRef()) );
  // junc_wta03->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSmoothReturnTP()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSmoothReturnTPImportanceRef()) );

  junc_wta04 = ER::Junction::relocate(resource);
  // junc_wta04->getEdges()[0].reinit( (char*)&(module->owner->hazardResponse->out->getShieldDirectionToHazard()), (float*)&(module->owner->hazardResponse->out->getShieldDirectionToHazardImportanceRef()) );
  // junc_wta04->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getShieldActionDirectionToHazard()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getShieldActionDirectionToHazardImportanceRef()) );

  junc_feedIn_supportTM = ER::Junction::relocate(resource);
  // junc_feedIn_supportTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ShieldManagement_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ShieldManagement* module = (ShieldManagement*)modulePtr;
  ShieldManagementFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportTMImportance = junc_feedIn_supportTM->combineDirectInput(&feedIn.supportTM);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ShieldManagement_Con::combineInputs(ER::Module* modulePtr)
{
  ShieldManagement* module = (ShieldManagement*)modulePtr;
  ShieldManagementInputs& in = *module->in;

  // Junction assignments.
  in.m_turnAwayScaleImportance = junc_wta02->combineWinnerTakesAll(&in.turnAwayScale);
  in.m_smoothReturnTPImportance = junc_wta03->combineWinnerTakesAll(&in.smoothReturnTP);
  in.m_directionToHazardImportance = junc_wta04->combineWinnerTakesAll(&in.directionToHazard);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


