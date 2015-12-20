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
#include "ArmPlacement.h"
#include "ArmPlacementPackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "ArmsPlacementBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmPlacement*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmPlacement_Con::~ArmPlacement_Con()
{
  junc_in_handOffsetMultiplier->destroy();
  junc_in_swivelAmount->destroy();
  junc_in_maxArmExtensionScale->destroy();
  junc_in_target->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmPlacement_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmPlacement_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement_Con::buildConnections(ArmPlacement* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmPlacement_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_target = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getPlacementTarget()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getPlacementTargetImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getPlacementTarget()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getPlacementTargetImportanceRef()) );

  junc_in_maxArmExtensionScale = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );

  junc_in_swivelAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getSwivelAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmountImportanceRef()) );

  junc_in_handOffsetMultiplier = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getHandOffsetMultiplier()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getHandOffsetMultiplierImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmPlacement_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_target = ER::Junction::relocate(resource);
  // junc_in_target->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getPlacementTarget()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getPlacementTargetImportanceRef()) );
  // junc_in_target->getEdges()[1].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getPlacementTarget()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getPlacementTargetImportanceRef()) );

  junc_in_maxArmExtensionScale = ER::Junction::relocate(resource);
  // junc_in_maxArmExtensionScale->getEdges()[0].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );
  // junc_in_maxArmExtensionScale->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );

  junc_in_swivelAmount = ER::Junction::relocate(resource);
  // junc_in_swivelAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getSwivelAmountImportanceRef()) );
  // junc_in_swivelAmount->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmountImportanceRef()) );

  junc_in_handOffsetMultiplier = ER::Junction::relocate(resource);
  // junc_in_handOffsetMultiplier->getEdges()[0].reinit( (char*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getHandOffsetMultiplier()), (float*)&(module->owner->owner->armsPlacementBehaviourInterface->out->getHandOffsetMultiplierImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmPlacement_Con::combineInputs(ER::Module* modulePtr)
{
  ArmPlacement* module = (ArmPlacement*)modulePtr;
  ArmPlacementInputs& in = *module->in;

  // Junction assignments.
  in.m_targetImportance = junc_in_target->combinePriority(&in.target);
  in.target.postCombine();
  in.m_maxArmExtensionScaleImportance = junc_in_maxArmExtensionScale->combinePriority(&in.maxArmExtensionScale);
  in.m_swivelAmountImportance = junc_in_swivelAmount->combinePriority(&in.swivelAmount);
  in.m_handOffsetMultiplierImportance = junc_in_handOffsetMultiplier->combineDirectInput(&in.handOffsetMultiplier);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


