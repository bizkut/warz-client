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
#include "HeadAvoid.h"
#include "HeadAvoidPackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "HeadAvoidBehaviourInterface.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadAvoid_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HeadAvoid*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HeadAvoid_Con::~HeadAvoid_Con()
{
  junc_in_twoSidedTarget->destroy();
  junc_in_avoidRadius->destroy();
  junc_pri01->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeadAvoid_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeadAvoid_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HeadAvoid_Con::buildConnections(HeadAvoid* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadAvoid_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_pri01 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getAvoidTarget()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getAvoidTargetImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidTarget()), (float*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidTargetImportanceRef()) );

  junc_in_avoidRadius = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidRadius()), (float*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidRadiusImportanceRef()) );

  junc_in_twoSidedTarget = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->headAvoidBehaviourInterface->out->getTwoSidedTarget()), (float*)&(module->owner->owner->headAvoidBehaviourInterface->out->getTwoSidedTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HeadAvoid_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadAvoid_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_pri01 = ER::Junction::relocate(resource);
  // junc_pri01->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getAvoidTarget()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getAvoidTargetImportanceRef()) );
  // junc_pri01->getEdges()[1].reinit( (char*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidTarget()), (float*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidTargetImportanceRef()) );

  junc_in_avoidRadius = ER::Junction::relocate(resource);
  // junc_in_avoidRadius->getEdges()[0].reinit( (char*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidRadius()), (float*)&(module->owner->owner->headAvoidBehaviourInterface->out->getAvoidRadiusImportanceRef()) );

  junc_in_twoSidedTarget = ER::Junction::relocate(resource);
  // junc_in_twoSidedTarget->getEdges()[0].reinit( (char*)&(module->owner->owner->headAvoidBehaviourInterface->out->getTwoSidedTarget()), (float*)&(module->owner->owner->headAvoidBehaviourInterface->out->getTwoSidedTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HeadAvoid_Con::combineInputs(ER::Module* modulePtr)
{
  HeadAvoid* module = (HeadAvoid*)modulePtr;
  HeadAvoidInputs& in = *module->in;

  // Junction assignments.
  in.m_avoidTargetImportance = junc_pri01->combinePriority(&in.avoidTarget);
  in.avoidTarget.postCombine();
  in.m_avoidRadiusImportance = junc_in_avoidRadius->combineDirectInput(&in.avoidRadius);
  in.m_twoSidedTargetImportance = junc_in_twoSidedTarget->combineDirectInput(&in.twoSidedTarget);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


