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
#include "HeadDodge.h"
#include "HeadDodgePackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "HeadDodgeBehaviourInterface.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HeadDodge*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HeadDodge_Con::~HeadDodge_Con()
{
  junc_in_dodgeHazard->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeadDodge_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeadDodge_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HeadDodge_Con::buildConnections(HeadDodge* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadDodge_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_dodgeHazard = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getDodgeHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getDodgeHazardImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->headDodgeBehaviourInterface->out->getDodgeHazard()), (float*)&(module->owner->owner->headDodgeBehaviourInterface->out->getDodgeHazardImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadDodge_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_dodgeHazard = ER::Junction::relocate(resource);
  // junc_in_dodgeHazard->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getDodgeHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getDodgeHazardImportanceRef()) );
  // junc_in_dodgeHazard->getEdges()[1].reinit( (char*)&(module->owner->owner->headDodgeBehaviourInterface->out->getDodgeHazard()), (float*)&(module->owner->owner->headDodgeBehaviourInterface->out->getDodgeHazardImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HeadDodge_Con::combineInputs(ER::Module* modulePtr)
{
  HeadDodge* module = (HeadDodge*)modulePtr;
  HeadDodgeInputs& in = *module->in;

  // Junction assignments.
  in.m_dodgeHazardImportance = junc_in_dodgeHazard->combineAverage(&in.dodgeHazard);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


