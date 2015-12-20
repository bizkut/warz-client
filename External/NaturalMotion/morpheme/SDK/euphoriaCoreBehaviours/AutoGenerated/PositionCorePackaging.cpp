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
#include "PositionCore.h"
#include "PositionCorePackaging.h"
#include "BodySectionPackaging.h"
#include "MyNetwork.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void PositionCore_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((PositionCore*)module);
}

//----------------------------------------------------------------------------------------------------------------------
PositionCore_Con::~PositionCore_Con()
{
  junc_feedIn_oneWayTranslationRequest->destroy();
  junc_in_translationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PositionCore_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(PositionCore_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void PositionCore_Con::buildConnections(PositionCore* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(PositionCore_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_translationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getTranslationRequest()), (float*)&(module->owner->feedIn->getTranslationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->in->getTranslationRequest()), (float*)&(module->owner->in->getTranslationRequestImportanceRef()) );

  junc_feedIn_oneWayTranslationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getOneWayTranslationRequest()), (float*)&(module->owner->feedIn->getOneWayTranslationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void PositionCore_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(PositionCore_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_translationRequest = ER::Junction::relocate(resource);
  // junc_in_translationRequest->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getTranslationRequest()), (float*)&(module->owner->feedIn->getTranslationRequestImportanceRef()) );
  // junc_in_translationRequest->getEdges()[1].reinit( (char*)&(module->owner->in->getTranslationRequest()), (float*)&(module->owner->in->getTranslationRequestImportanceRef()) );

  junc_feedIn_oneWayTranslationRequest = ER::Junction::relocate(resource);
  // junc_feedIn_oneWayTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getOneWayTranslationRequest()), (float*)&(module->owner->feedIn->getOneWayTranslationRequestImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void PositionCore_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  PositionCore* module = (PositionCore*)modulePtr;
  PositionCoreFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_oneWayTranslationRequestImportance = junc_feedIn_oneWayTranslationRequest->combineDirectInput(&feedIn.oneWayTranslationRequest);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void PositionCore_Con::combineInputs(ER::Module* modulePtr)
{
  PositionCore* module = (PositionCore*)modulePtr;
  PositionCoreInputs& in = *module->in;

  // Junction assignments.
  in.m_translationRequestImportance = junc_in_translationRequest->combinePriority(&in.translationRequest);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


