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
#include "Head.h"
#include "HeadPoint.h"
#include "HeadPointPackaging.h"
#include "HeadEyes.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HeadPoint*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HeadPoint_Con::~HeadPoint_Con()
{
  junc_in_supportAmount->destroy();
  junc_in_target->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeadPoint_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeadPoint_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HeadPoint_Con::buildConnections(HeadPoint* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadPoint_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_target = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->eyes->out->getLookTarget()), (float*)&(module->owner->eyes->out->getLookTargetImportanceRef()) );

  junc_in_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadPoint_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_target = ER::Junction::relocate(resource);
  // junc_in_target->getEdges()[0].reinit( (char*)&(module->owner->eyes->out->getLookTarget()), (float*)&(module->owner->eyes->out->getLookTargetImportanceRef()) );

  junc_in_supportAmount = ER::Junction::relocate(resource);
  // junc_in_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HeadPoint_Con::combineInputs(ER::Module* modulePtr)
{
  HeadPoint* module = (HeadPoint*)modulePtr;
  HeadPointInputs& in = *module->in;

  // Junction assignments.
  in.m_targetImportance = junc_in_target->combineDirectInput(&in.target);
  in.m_supportAmountImportance = junc_in_supportAmount->combineDirectInput(&in.supportAmount);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


