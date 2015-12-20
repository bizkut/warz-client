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
#include "FreeFallBehaviourInterface.h"
#include "FreeFallBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "FreeFallManagement.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((FreeFallBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
FreeFallBehaviourInterface_Con::~FreeFallBehaviourInterface_Con()
{
  junc_feedIn_orientationError->destroy();
  junc_feedIn_isInFreefall->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format FreeFallBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(FreeFallBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface_Con::buildConnections(FreeFallBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(FreeFallBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_isInFreefall = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );

  junc_feedIn_orientationError = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getOrientationError()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getOrientationErrorImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(FreeFallBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_isInFreefall = ER::Junction::relocate(resource);
  // junc_feedIn_isInFreefall->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );

  junc_feedIn_orientationError = ER::Junction::relocate(resource);
  // junc_feedIn_orientationError->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getOrientationError()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getOrientationErrorImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void FreeFallBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  FreeFallBehaviourInterface* module = (FreeFallBehaviourInterface*)modulePtr;
  FreeFallBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_isInFreefallImportance = junc_feedIn_isInFreefall->combineDirectInput(&feedIn.isInFreefall);
  feedIn.m_orientationErrorImportance = junc_feedIn_orientationError->combineDirectInput(&feedIn.orientationError);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


