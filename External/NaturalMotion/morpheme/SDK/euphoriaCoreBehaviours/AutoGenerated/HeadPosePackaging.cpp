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
#include "HeadPose.h"
#include "HeadPosePackaging.h"
#include "MyNetwork.h"
#include "AnimateBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "WritheBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadPose_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HeadPose*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HeadPose_Con::~HeadPose_Con()
{
  junc_highImportance->destroy();
  junc_lowImportance->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeadPose_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeadPose_Con), 16);

  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(3);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HeadPose_Con::buildConnections(HeadPose* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadPose_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_lowImportance = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->writheBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->writheBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = activeJnc = ER::Junction::init(resource, 3, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void HeadPose_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadPose_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_lowImportance = ER::Junction::relocate(resource);
  // junc_lowImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->writheBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->writheBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = ER::Junction::relocate(resource);
  // junc_highImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getHeadPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getHeadPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getHeadPoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HeadPose_Con::combineInputs(ER::Module* modulePtr)
{
  HeadPose* module = (HeadPose*)modulePtr;
  HeadPoseInputs& in = *module->in;

  // Junction assignments.
  in.m_headPoseLowImpImportance = junc_lowImportance->combinePriority(&in.headPoseLowImp);
  in.headPoseLowImp.postCombine();
  in.m_headPoseHighImpImportance = junc_highImportance->combineWinnerTakesAll(&in.headPoseHighImp);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


