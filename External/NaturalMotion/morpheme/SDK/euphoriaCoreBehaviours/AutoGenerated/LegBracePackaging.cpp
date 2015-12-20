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
#include "Leg.h"
#include "LegBrace.h"
#include "LegBracePackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegBrace_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegBrace*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegBrace_Con::~LegBrace_Con()
{
  junc_in_shouldBrace->destroy();
  junc_in_braceHazard->destroy();
  junc_in_cushionPoint->destroy();
  junc_in_protectState->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegBrace_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegBrace_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegBrace_Con::buildConnections(LegBrace* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegBrace_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_protectState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getChestState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getChestStateImportanceRef()) );

  junc_in_cushionPoint = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->feedIn->getCushionPoint()), (float*)&(module->owner->owner->lowerBody->feedIn->getCushionPointImportanceRef()) );

  junc_in_braceHazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazardImportanceRef()) );

  junc_in_shouldBrace = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->in->getShouldBrace()), (float*)&(module->owner->in->getShouldBraceImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegBrace_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegBrace_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_protectState = ER::Junction::relocate(resource);
  // junc_in_protectState->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getChestState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getChestStateImportanceRef()) );

  junc_in_cushionPoint = ER::Junction::relocate(resource);
  // junc_in_cushionPoint->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->feedIn->getCushionPoint()), (float*)&(module->owner->owner->lowerBody->feedIn->getCushionPointImportanceRef()) );

  junc_in_braceHazard = ER::Junction::relocate(resource);
  // junc_in_braceHazard->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazardImportanceRef()) );

  junc_in_shouldBrace = ER::Junction::relocate(resource);
  // junc_in_shouldBrace->getEdges()[0].reinit( (char*)&(module->owner->in->getShouldBrace()), (float*)&(module->owner->in->getShouldBraceImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegBrace_Con::combineInputs(ER::Module* modulePtr)
{
  LegBrace* module = (LegBrace*)modulePtr;
  LegBraceInputs& in = *module->in;

  // Junction assignments.
  in.m_protectStateImportance = junc_in_protectState->combineDirectInput(&in.protectState);
  in.m_cushionPointImportance = junc_in_cushionPoint->combineDirectInput(&in.cushionPoint);
  in.m_braceHazardImportance = junc_in_braceHazard->combineDirectInput(&in.braceHazard);
  in.m_shouldBraceImportance = junc_in_shouldBrace->combineDirectInput(&in.shouldBrace);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


