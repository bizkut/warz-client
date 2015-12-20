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
#include "LegPose.h"
#include "LegPosePackaging.h"
#include "MyNetwork.h"
#include "AnimateBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegPose_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegPose*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegPose_Con::~LegPose_Con()
{
  junc_highImportance->destroy();
  junc_lowImportance->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegPose_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegPose_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(3);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegPose_Con::buildConnections(LegPose* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegPose_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_lowImportance = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getLegPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getLegPoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = activeJnc = ER::Junction::init(resource, 3, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getLegPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getLegPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getLegPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getLegPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegPose_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegPose_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_lowImportance = ER::Junction::relocate(resource);
  // junc_lowImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getLegPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getLegPoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = ER::Junction::relocate(resource);
  // junc_highImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getLegPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getLegPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getLegPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getLegPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getLegPoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegPose_Con::combineInputs(ER::Module* modulePtr)
{
  LegPose* module = (LegPose*)modulePtr;
  LegPoseInputs& in = *module->in;

  // Junction assignments.
  in.m_legPoseLowImpImportance = junc_lowImportance->combinePriority(&in.legPoseLowImp);
  in.legPoseLowImp.postCombine();
  in.m_legPoseHighImpImportance = junc_highImportance->combineWinnerTakesAll(&in.legPoseHighImp);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


