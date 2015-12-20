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
#include "Arm.h"
#include "ArmPose.h"
#include "ArmPosePackaging.h"
#include "MyNetwork.h"
#include "AnimateBehaviourInterface.h"
#include "BalanceBehaviourInterface.h"
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "IdleAwakeBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmPose_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmPose*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmPose_Con::~ArmPose_Con()
{
  junc_highImportance->destroy();
  junc_lowImportance->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmPose_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmPose_Con), 16);

  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(3);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmPose_Con::buildConnections(ArmPose* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmPose_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_lowImportance = activeJnc = ER::Junction::init(resource, 4, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->steppingBalance->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->steppingBalance->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = activeJnc = ER::Junction::init(resource, 3, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmPose_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmPose_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_lowImportance = ER::Junction::relocate(resource);
  // junc_lowImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseLowPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseLowPriorityImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePose(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->steppingBalance->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->steppingBalance->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_lowImportance->getEdges()[3].reinit( (char*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->idleAwakeBehaviourInterface->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );

  junc_highImportance = ER::Junction::relocate(resource);
  // junc_highImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getArmPose(module->owner->getChildIndex())), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getArmPoseImportanceRef(module->owner->getChildIndex())) );
  // junc_highImportance->getEdges()[2].reinit( (char*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseHighPriority(module->owner->getChildIndex())), (float*)&(module->owner->owner->animateBehaviourInterface->out->getArmPoseHighPriorityImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmPose_Con::combineInputs(ER::Module* modulePtr)
{
  ArmPose* module = (ArmPose*)modulePtr;
  ArmPoseInputs& in = *module->in;

  // Junction assignments.
  in.m_armPoseLowImpImportance = junc_lowImportance->combinePriority(&in.armPoseLowImp);
  in.armPoseLowImp.postCombine();
  in.m_armPoseHighImpImportance = junc_highImportance->combineWinnerTakesAll(&in.armPoseHighImp);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


