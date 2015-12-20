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
#include "ImpactPredictor.h"
#include "ImpactPredictorPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "EnvironmentAwareness.h"
#include "HazardAwarenessBehaviourInterface.h"
#include "UserHazardBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor_chestCon::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ImpactPredictor*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ImpactPredictor_chestCon::~ImpactPredictor_chestCon()
{
  junc_in_protectHazardFiltering->destroy();
  junc_in_protectParameters->destroy();
  junc_in_ignoreNonUserHazards->destroy();
  junc_in_potentialUserHazards->destroy();
  junc_in_probeRadius->destroy();
  junc_in_velocityIsControlled->destroy();
  junc_in_ignoreVerticalPredictionAmount->destroy();
  junc_feedIn_potentialEnvironmentHazards->destroy();
  junc_feedIn_supportTM->destroy();
  junc_feedIn_leanVector->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ImpactPredictor_chestCon::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ImpactPredictor_chestCon), 16);

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
void ImpactPredictor_chestCon::buildConnections(ImpactPredictor* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ImpactPredictor_chestCon), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_leanVector = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getLeanVector()), (float*)&(module->owner->owner->bodyFrame->feedOut->getLeanVectorImportanceRef()) );

  junc_feedIn_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

  junc_feedIn_potentialEnvironmentHazards = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictor(0)), (float*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictorImportanceRef(0)) );

  junc_in_ignoreVerticalPredictionAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getIgnoreVerticalPredictionAmount()), (float*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getIgnoreVerticalPredictionAmountImportanceRef()) );

  junc_in_velocityIsControlled = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getVelocityIsControlled()), (float*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getVelocityIsControlledImportanceRef()) );

  junc_in_probeRadius = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getProbeRadius()), (float*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getProbeRadiusImportanceRef()) );

  junc_in_potentialUserHazards = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->userHazardBehaviourInterface->out->getPotentialHazards()), (float*)&(module->owner->owner->userHazardBehaviourInterface->out->getPotentialHazardsImportanceRef()) );

  junc_in_ignoreNonUserHazards = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->userHazardBehaviourInterface->out->getIgnoreNonUserHazards()), (float*)&(module->owner->owner->userHazardBehaviourInterface->out->getIgnoreNonUserHazardsImportanceRef()) );

  junc_in_protectParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorParameters()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorParametersImportanceRef()) );

  junc_in_protectHazardFiltering = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorHazardFiltering()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorHazardFilteringImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor_chestCon::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ImpactPredictor_chestCon), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_leanVector = ER::Junction::relocate(resource);
  // junc_feedIn_leanVector->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getLeanVector()), (float*)&(module->owner->owner->bodyFrame->feedOut->getLeanVectorImportanceRef()) );

  junc_feedIn_supportTM = ER::Junction::relocate(resource);
  // junc_feedIn_supportTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

  junc_feedIn_potentialEnvironmentHazards = ER::Junction::relocate(resource);
  // junc_feedIn_potentialEnvironmentHazards->getEdges()[0].reinit( (char*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictor(0)), (float*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictorImportanceRef(0)) );

  junc_in_ignoreVerticalPredictionAmount = ER::Junction::relocate(resource);
  // junc_in_ignoreVerticalPredictionAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getIgnoreVerticalPredictionAmount()), (float*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getIgnoreVerticalPredictionAmountImportanceRef()) );

  junc_in_velocityIsControlled = ER::Junction::relocate(resource);
  // junc_in_velocityIsControlled->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getVelocityIsControlled()), (float*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getVelocityIsControlledImportanceRef()) );

  junc_in_probeRadius = ER::Junction::relocate(resource);
  // junc_in_probeRadius->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getProbeRadius()), (float*)&(module->owner->owner->hazardAwarenessBehaviourInterface->out->getProbeRadiusImportanceRef()) );

  junc_in_potentialUserHazards = ER::Junction::relocate(resource);
  // junc_in_potentialUserHazards->getEdges()[0].reinit( (char*)&(module->owner->owner->userHazardBehaviourInterface->out->getPotentialHazards()), (float*)&(module->owner->owner->userHazardBehaviourInterface->out->getPotentialHazardsImportanceRef()) );

  junc_in_ignoreNonUserHazards = ER::Junction::relocate(resource);
  // junc_in_ignoreNonUserHazards->getEdges()[0].reinit( (char*)&(module->owner->owner->userHazardBehaviourInterface->out->getIgnoreNonUserHazards()), (float*)&(module->owner->owner->userHazardBehaviourInterface->out->getIgnoreNonUserHazardsImportanceRef()) );

  junc_in_protectParameters = ER::Junction::relocate(resource);
  // junc_in_protectParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorParameters()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorParametersImportanceRef()) );

  junc_in_protectHazardFiltering = ER::Junction::relocate(resource);
  // junc_in_protectHazardFiltering->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorHazardFiltering()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getToImpactPredictorHazardFilteringImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ImpactPredictor_chestCon::combineFeedbackInputs(ER::Module* modulePtr)
{
  ImpactPredictor* module = (ImpactPredictor*)modulePtr;
  ImpactPredictorFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_leanVectorImportance = junc_feedIn_leanVector->combineDirectInput(&feedIn.leanVector);
  feedIn.m_supportTMImportance = junc_feedIn_supportTM->combineDirectInput(&feedIn.supportTM);
  feedIn.m_potentialEnvironmentHazardsImportance = junc_feedIn_potentialEnvironmentHazards->combineDirectInput(&feedIn.potentialEnvironmentHazards);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ImpactPredictor_chestCon::combineInputs(ER::Module* modulePtr)
{
  ImpactPredictor* module = (ImpactPredictor*)modulePtr;
  ImpactPredictorInputs& in = *module->in;

  // Junction assignments.
  in.m_ignoreVerticalPredictionAmountImportance = junc_in_ignoreVerticalPredictionAmount->combineDirectInput(&in.ignoreVerticalPredictionAmount);
  in.m_velocityIsControlledImportance = junc_in_velocityIsControlled->combineDirectInput(&in.velocityIsControlled);
  in.m_probeRadiusImportance = junc_in_probeRadius->combineDirectInput(&in.probeRadius);
  in.m_potentialUserHazardsImportance = junc_in_potentialUserHazards->combineDirectInput(&in.potentialUserHazards);
  in.m_ignoreNonUserHazardsImportance = junc_in_ignoreNonUserHazards->combineDirectInput(&in.ignoreNonUserHazards);
  in.m_protectParametersImportance = junc_in_protectParameters->combineDirectInput(&in.protectParameters);
  in.m_protectHazardFilteringImportance = junc_in_protectHazardFiltering->combineDirectInput(&in.protectHazardFiltering);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


