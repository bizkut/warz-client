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
#include "HoldActionBehaviourInterface.h"
#include "HoldActionBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HoldActionBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HoldActionBehaviourInterface_Con::~HoldActionBehaviourInterface_Con()
{
  junc_feedIn_edgeForwardNormal->destroy();
  junc_feedIn_holdDuration->destroy();
  junc_feedIn_holdAttemptImportance->destroy();
  junc_feedIn_numConstrainedArms->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HoldActionBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HoldActionBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface_Con::buildConnections(HoldActionBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HoldActionBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_numConstrainedArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getNumConstrainedArms()), (float*)&(module->owner->hazardManagement->grab->feedOut->getNumConstrainedArmsImportanceRef()) );

  junc_feedIn_holdAttemptImportance = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getHoldAttemptImportance()), (float*)&(module->owner->hazardManagement->grab->feedOut->getHoldAttemptImportanceImportanceRef()) );

  junc_feedIn_holdDuration = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getHoldDuration()), (float*)&(module->owner->hazardManagement->grab->feedOut->getHoldDurationImportanceRef()) );

  junc_feedIn_edgeForwardNormal = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getEdgeForwardNormal()), (float*)&(module->owner->hazardManagement->grab->feedOut->getEdgeForwardNormalImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HoldActionBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_numConstrainedArms = ER::Junction::relocate(resource);
  // junc_feedIn_numConstrainedArms->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getNumConstrainedArms()), (float*)&(module->owner->hazardManagement->grab->feedOut->getNumConstrainedArmsImportanceRef()) );

  junc_feedIn_holdAttemptImportance = ER::Junction::relocate(resource);
  // junc_feedIn_holdAttemptImportance->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getHoldAttemptImportance()), (float*)&(module->owner->hazardManagement->grab->feedOut->getHoldAttemptImportanceImportanceRef()) );

  junc_feedIn_holdDuration = ER::Junction::relocate(resource);
  // junc_feedIn_holdDuration->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getHoldDuration()), (float*)&(module->owner->hazardManagement->grab->feedOut->getHoldDurationImportanceRef()) );

  junc_feedIn_edgeForwardNormal = ER::Junction::relocate(resource);
  // junc_feedIn_edgeForwardNormal->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->grab->feedOut->getEdgeForwardNormal()), (float*)&(module->owner->hazardManagement->grab->feedOut->getEdgeForwardNormalImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HoldActionBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  HoldActionBehaviourInterface* module = (HoldActionBehaviourInterface*)modulePtr;
  HoldActionBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_numConstrainedArmsImportance = junc_feedIn_numConstrainedArms->combineDirectInput(&feedIn.numConstrainedArms);
  feedIn.m_holdAttemptImportanceImportance = junc_feedIn_holdAttemptImportance->combineDirectInput(&feedIn.holdAttemptImportance);
  feedIn.m_holdDurationImportance = junc_feedIn_holdDuration->combineDirectInput(&feedIn.holdDuration);
  feedIn.m_edgeForwardNormalImportance = junc_feedIn_edgeForwardNormal->combineDirectInput(&feedIn.edgeForwardNormal);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


