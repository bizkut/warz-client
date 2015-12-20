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
#include "EnvironmentAwareness.h"
#include "EnvironmentAwarenessPackaging.h"
#include "Head.h"
#include "HeadEyes.h"
#include "SceneProbes.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "ObserveBehaviourInterface.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((EnvironmentAwareness*)module);
}

//----------------------------------------------------------------------------------------------------------------------
EnvironmentAwareness_Con::~EnvironmentAwareness_Con()
{
  junc_in_findObject->destroy();
  junc_in_sphereProjectionsForImpactPredictor->destroy();
  junc_feedIn_requestNextProbe->destroy();
  junc_feedIn_focalRadius->destroy();
  junc_feedIn_focalCentre->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EnvironmentAwareness_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(EnvironmentAwareness_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness_Con::buildConnections(EnvironmentAwareness* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(EnvironmentAwareness_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_focalCentre = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentre()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentreImportanceRef()) );

  junc_feedIn_focalRadius = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadius()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadiusImportanceRef()) );

  junc_feedIn_requestNextProbe = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->sceneProbes->feedOut->getRequestNextProbe()), (float*)&(module->owner->sceneProbes->feedOut->getRequestNextProbeImportanceRef()) );

  junc_in_sphereProjectionsForImpactPredictor = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->chestImpactPredictor->out->getSphereProjection()), (float*)&(module->owner->hazardManagement->chestImpactPredictor->out->getSphereProjectionImportanceRef()) );

  junc_in_findObject = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->observeBehaviourInterface->out->getFindObject()), (float*)&(module->owner->observeBehaviourInterface->out->getFindObjectImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(EnvironmentAwareness_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_focalCentre = ER::Junction::relocate(resource);
  // junc_feedIn_focalCentre->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentre()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentreImportanceRef()) );

  junc_feedIn_focalRadius = ER::Junction::relocate(resource);
  // junc_feedIn_focalRadius->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadius()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadiusImportanceRef()) );

  junc_feedIn_requestNextProbe = ER::Junction::relocate(resource);
  // junc_feedIn_requestNextProbe->getEdges()[0].reinit( (char*)&(module->owner->sceneProbes->feedOut->getRequestNextProbe()), (float*)&(module->owner->sceneProbes->feedOut->getRequestNextProbeImportanceRef()) );

  junc_in_sphereProjectionsForImpactPredictor = ER::Junction::relocate(resource);
  // junc_in_sphereProjectionsForImpactPredictor->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->chestImpactPredictor->out->getSphereProjection()), (float*)&(module->owner->hazardManagement->chestImpactPredictor->out->getSphereProjectionImportanceRef()) );

  junc_in_findObject = ER::Junction::relocate(resource);
  // junc_in_findObject->getEdges()[0].reinit( (char*)&(module->owner->observeBehaviourInterface->out->getFindObject()), (float*)&(module->owner->observeBehaviourInterface->out->getFindObjectImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void EnvironmentAwareness_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  EnvironmentAwareness* module = (EnvironmentAwareness*)modulePtr;
  EnvironmentAwarenessFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_focalCentreImportance = junc_feedIn_focalCentre->combineDirectInput(&feedIn.focalCentre);
  feedIn.m_focalRadiusImportance = junc_feedIn_focalRadius->combineDirectInput(&feedIn.focalRadius);
  feedIn.m_requestNextProbeImportance = junc_feedIn_requestNextProbe->combineDirectInput(&feedIn.requestNextProbe);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void EnvironmentAwareness_Con::combineInputs(ER::Module* modulePtr)
{
  EnvironmentAwareness* module = (EnvironmentAwareness*)modulePtr;
  EnvironmentAwarenessInputs& in = *module->in;

  // Junction assignments.
  in.m_sphereProjectionsForImpactPredictorImportance[0] = junc_in_sphereProjectionsForImpactPredictor->combineDirectInput(&in.sphereProjectionsForImpactPredictor[0]);
  in.m_findObjectImportance = junc_in_findObject->combineDirectInput(&in.findObject);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


