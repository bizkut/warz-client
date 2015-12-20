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
#include "SpinePose.h"
#include "SpinePosePackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "FreeFallManagement.h"
#include "AnimateBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SpinePose_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SpinePose*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SpinePose_Con::~SpinePose_Con()
{
  junc_highImportance->destroy();
  junc_lowImportance->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SpinePose_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SpinePose_Con), 16);

  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(3);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SpinePose_Con::buildConnections(SpinePose* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpinePose_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_lowImportance = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getSpinePose()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getSpinePoseImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getSpinePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getSpinePoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = activeJnc = ER::Junction::init(resource, 3, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getSpinePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getSpinePoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSpinePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSpinePoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void SpinePose_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpinePose_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_lowImportance = ER::Junction::relocate(resource);
  // junc_lowImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getSpinePose()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getSpinePoseImportanceRef()) );
  // junc_lowImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getSpinePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getSpinePoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = ER::Junction::relocate(resource);
  // junc_highImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getSpinePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getSpinePoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSpinePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getSpinePoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getSpinePoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SpinePose_Con::combineInputs(ER::Module* modulePtr)
{
  SpinePose* module = (SpinePose*)modulePtr;
  SpinePoseInputs& in = *module->in;

  // Junction assignments.
  in.m_spinePoseLowImpImportance = junc_lowImportance->combinePriority(&in.spinePoseLowImp);
  in.spinePoseLowImp.postCombine();
  in.m_spinePoseHighImpImportance = junc_highImportance->combineWinnerTakesAll(&in.spinePoseHighImp);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


