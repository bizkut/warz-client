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
#include "HazardResponse.h"
#include "HazardResponsePackaging.h"
#include "ImpactPredictor.h"
#include "MyNetwork.h"
#include "ShieldBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HazardResponse_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HazardResponse*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HazardResponse_Con::~HazardResponse_Con()
{
  junc_in_armsPlacementWeight->destroy();
  junc_in_headDodgeWeight->destroy();
  junc_in_headAvoidWeight->destroy();
  junc_in_braceWeight->destroy();
  junc_feedIn_hazardsNotBraceable->destroy();
  junc_in_shieldParameters->destroy();
  junc_feedIn_possibleFutureHazard->destroy();
  junc_feedIn_hazard->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HazardResponse_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HazardResponse_Con), 16);

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
void HazardResponse_Con::buildConnections(HazardResponse* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HazardResponse_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_hazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

  junc_feedIn_possibleFutureHazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getPossibleFutureHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getPossibleFutureHazardImportanceRef()) );

  junc_in_shieldParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getShieldParameters()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getShieldParametersImportanceRef()) );

  junc_feedIn_hazardsNotBraceable = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazardsNotBraceable()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardsNotBraceableImportanceRef()) );

  junc_in_braceWeight = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getBraceWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getBraceWeightImportanceRef()) );

  junc_in_headAvoidWeight = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getHeadAvoidWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getHeadAvoidWeightImportanceRef()) );

  junc_in_headDodgeWeight = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getHeadDodgeWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getHeadDodgeWeightImportanceRef()) );

  junc_in_armsPlacementWeight = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getArmsPlacementWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getArmsPlacementWeightImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HazardResponse_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HazardResponse_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_hazard = ER::Junction::relocate(resource);
  // junc_feedIn_hazard->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

  junc_feedIn_possibleFutureHazard = ER::Junction::relocate(resource);
  // junc_feedIn_possibleFutureHazard->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getPossibleFutureHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getPossibleFutureHazardImportanceRef()) );

  junc_in_shieldParameters = ER::Junction::relocate(resource);
  // junc_in_shieldParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getShieldParameters()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getShieldParametersImportanceRef()) );

  junc_feedIn_hazardsNotBraceable = ER::Junction::relocate(resource);
  // junc_feedIn_hazardsNotBraceable->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazardsNotBraceable()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardsNotBraceableImportanceRef()) );

  junc_in_braceWeight = ER::Junction::relocate(resource);
  // junc_in_braceWeight->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getBraceWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getBraceWeightImportanceRef()) );

  junc_in_headAvoidWeight = ER::Junction::relocate(resource);
  // junc_in_headAvoidWeight->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getHeadAvoidWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getHeadAvoidWeightImportanceRef()) );

  junc_in_headDodgeWeight = ER::Junction::relocate(resource);
  // junc_in_headDodgeWeight->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getHeadDodgeWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getHeadDodgeWeightImportanceRef()) );

  junc_in_armsPlacementWeight = ER::Junction::relocate(resource);
  // junc_in_armsPlacementWeight->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getArmsPlacementWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getArmsPlacementWeightImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HazardResponse_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  HazardResponse* module = (HazardResponse*)modulePtr;
  HazardResponseFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_hazardImportance = junc_feedIn_hazard->combineDirectInput(&feedIn.hazard);
  feedIn.m_possibleFutureHazardImportance = junc_feedIn_possibleFutureHazard->combineDirectInput(&feedIn.possibleFutureHazard);
  feedIn.m_hazardsNotBraceableImportance = junc_feedIn_hazardsNotBraceable->combineDirectInput(&feedIn.hazardsNotBraceable);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HazardResponse_Con::combineInputs(ER::Module* modulePtr)
{
  HazardResponse* module = (HazardResponse*)modulePtr;
  HazardResponseInputs& in = *module->in;

  // Junction assignments.
  in.m_shieldParametersImportance = junc_in_shieldParameters->combineDirectInput(&in.shieldParameters);
  in.m_braceWeightImportance = junc_in_braceWeight->combineDirectInput(&in.braceWeight);
  in.m_headAvoidWeightImportance = junc_in_headAvoidWeight->combineDirectInput(&in.headAvoidWeight);
  in.m_headDodgeWeightImportance = junc_in_headDodgeWeight->combineDirectInput(&in.headDodgeWeight);
  in.m_armsPlacementWeightImportance = junc_in_armsPlacementWeight->combineDirectInput(&in.armsPlacementWeight);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


