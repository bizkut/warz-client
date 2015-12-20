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
#include "Spine.h"
#include "SpineAim.h"
#include "SpineAimPackaging.h"
#include "MyNetwork.h"
#include "AimBehaviourInterface.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SpineAim_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SpineAim*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SpineAim_Con::~SpineAim_Con()
{
  junc_in_windingDirection->destroy();
  junc_in_poseAimDirRelRoot->destroy();
  junc_in_aimInfo->destroy();
  junc_in_targetRootTM->destroy();
  junc_in_targetPositionInWorldSpace->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SpineAim_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SpineAim_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SpineAim_Con::buildConnections(SpineAim* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineAim_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_targetPositionInWorldSpace = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPositionInWorldSpace()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPositionInWorldSpaceImportanceRef()) );

  junc_in_targetRootTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisTM()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisTMImportanceRef()) );

  junc_in_aimInfo = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getSpineAimInfo(module->owner->getChildIndex())), (float*)&(module->owner->owner->aimBehaviourInterface->out->getSpineAimInfoImportanceRef(module->owner->getChildIndex())) );

  junc_in_poseAimDirRelRoot = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getPoseAimDirRelPelvis()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getPoseAimDirRelPelvisImportanceRef()) );

  junc_in_windingDirection = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getWindingDirection()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getWindingDirectionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SpineAim_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineAim_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_targetPositionInWorldSpace = ER::Junction::relocate(resource);
  // junc_in_targetPositionInWorldSpace->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPositionInWorldSpace()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPositionInWorldSpaceImportanceRef()) );

  junc_in_targetRootTM = ER::Junction::relocate(resource);
  // junc_in_targetRootTM->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisTM()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getTargetPelvisTMImportanceRef()) );

  junc_in_aimInfo = ER::Junction::relocate(resource);
  // junc_in_aimInfo->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getSpineAimInfo(module->owner->getChildIndex())), (float*)&(module->owner->owner->aimBehaviourInterface->out->getSpineAimInfoImportanceRef(module->owner->getChildIndex())) );

  junc_in_poseAimDirRelRoot = ER::Junction::relocate(resource);
  // junc_in_poseAimDirRelRoot->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getPoseAimDirRelPelvis()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getPoseAimDirRelPelvisImportanceRef()) );

  junc_in_windingDirection = ER::Junction::relocate(resource);
  // junc_in_windingDirection->getEdges()[0].reinit( (char*)&(module->owner->owner->aimBehaviourInterface->out->getWindingDirection()), (float*)&(module->owner->owner->aimBehaviourInterface->out->getWindingDirectionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SpineAim_Con::combineInputs(ER::Module* modulePtr)
{
  SpineAim* module = (SpineAim*)modulePtr;
  SpineAimInputs& in = *module->in;

  // Junction assignments.
  in.m_targetPositionInWorldSpaceImportance = junc_in_targetPositionInWorldSpace->combineDirectInput(&in.targetPositionInWorldSpace);
  in.m_targetRootTMImportance = junc_in_targetRootTM->combineDirectInput(&in.targetRootTM);
  in.m_aimInfoImportance = junc_in_aimInfo->combineDirectInput(&in.aimInfo);
  in.m_poseAimDirRelRootImportance = junc_in_poseAimDirRelRoot->combineDirectInput(&in.poseAimDirRelRoot);
  in.m_windingDirectionImportance = junc_in_windingDirection->combineDirectInput(&in.windingDirection);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


