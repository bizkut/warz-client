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
#include "ObserveBehaviourInterface.h"
#include "ObserveBehaviourInterfacePackaging.h"
#include "EnvironmentAwareness.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ObserveBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ObserveBehaviourInterface_Con::~ObserveBehaviourInterface_Con()
{
  junc_feedIn_foundObjectMetric->destroy();
  junc_feedIn_foundObject->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ObserveBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ObserveBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourInterface_Con::buildConnections(ObserveBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ObserveBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_foundObject = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->environmentAwareness->feedOut->getFoundObject()), (float*)&(module->owner->environmentAwareness->feedOut->getFoundObjectImportanceRef()) );

  junc_feedIn_foundObjectMetric = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->environmentAwareness->feedOut->getFoundObjectMetric()), (float*)&(module->owner->environmentAwareness->feedOut->getFoundObjectMetricImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ObserveBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_foundObject = ER::Junction::relocate(resource);
  // junc_feedIn_foundObject->getEdges()[0].reinit( (char*)&(module->owner->environmentAwareness->feedOut->getFoundObject()), (float*)&(module->owner->environmentAwareness->feedOut->getFoundObjectImportanceRef()) );

  junc_feedIn_foundObjectMetric = ER::Junction::relocate(resource);
  // junc_feedIn_foundObjectMetric->getEdges()[0].reinit( (char*)&(module->owner->environmentAwareness->feedOut->getFoundObjectMetric()), (float*)&(module->owner->environmentAwareness->feedOut->getFoundObjectMetricImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ObserveBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ObserveBehaviourInterface* module = (ObserveBehaviourInterface*)modulePtr;
  ObserveBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_foundObjectImportance = junc_feedIn_foundObject->combineDirectInput(&feedIn.foundObject);
  feedIn.m_foundObjectMetricImportance = junc_feedIn_foundObjectMetric->combineDirectInput(&feedIn.foundObjectMetric);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


