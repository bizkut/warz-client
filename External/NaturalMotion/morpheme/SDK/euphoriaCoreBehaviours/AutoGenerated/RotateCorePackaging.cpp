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
#include "BodySection.h"
#include "RotateCore.h"
#include "RotateCorePackaging.h"
#include "BodySectionPackaging.h"
#include "MyNetwork.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void RotateCore_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((RotateCore*)module);
}

//----------------------------------------------------------------------------------------------------------------------
RotateCore_Con::~RotateCore_Con()
{
  junc_feedIn_oneWayRotationRequest->destroy();
  junc_in_rotationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RotateCore_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(RotateCore_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void RotateCore_Con::buildConnections(RotateCore* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(RotateCore_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_rotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getRotationRequest()), (float*)&(module->owner->feedIn->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->in->getRotationRequest()), (float*)&(module->owner->in->getRotationRequestImportanceRef()) );

  junc_feedIn_oneWayRotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getOneWayRotationRequest()), (float*)&(module->owner->feedIn->getOneWayRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void RotateCore_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(RotateCore_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_rotationRequest = ER::Junction::relocate(resource);
  // junc_in_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getRotationRequest()), (float*)&(module->owner->feedIn->getRotationRequestImportanceRef()) );
  // junc_in_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->in->getRotationRequest()), (float*)&(module->owner->in->getRotationRequestImportanceRef()) );

  junc_feedIn_oneWayRotationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_oneWayRotationRequest->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getOneWayRotationRequest()), (float*)&(module->owner->feedIn->getOneWayRotationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void RotateCore_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  RotateCore* module = (RotateCore*)modulePtr;
  RotateCoreFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_oneWayRotationRequestImportance = junc_feedIn_oneWayRotationRequest->combineDirectInput(&feedIn.oneWayRotationRequest);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void RotateCore_Con::combineInputs(ER::Module* modulePtr)
{
  RotateCore* module = (RotateCore*)modulePtr;
  RotateCoreInputs& in = *module->in;

  // Junction assignments.
  in.m_rotationRequestImportance = junc_in_rotationRequest->combinePriority(&in.rotationRequest);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


