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
#include "ArmBrace.h"
#include "ArmBracePackaging.h"
#include "MyNetwork.h"
#include "ArmsBraceBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmBrace*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmBrace_Con::~ArmBrace_Con()
{
  junc_in_shouldBrace->destroy();
  junc_in_braceHazard->destroy();
  junc_in_cushionPoint->destroy();
  junc_in_protectState->destroy();
  junc_in_swivelAmount->destroy();
  junc_in_maxArmExtensionScale->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmBrace_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmBrace_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmBrace_Con::buildConnections(ArmBrace* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmBrace_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_maxArmExtensionScale = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsBraceBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->armsBraceBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );

  junc_in_swivelAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsBraceBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->armsBraceBehaviourInterface->out->getSwivelAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmountImportanceRef()) );

  junc_in_protectState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getChestState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getChestStateImportanceRef()) );

  junc_in_cushionPoint = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->feedIn->getCushionPoint()), (float*)&(module->owner->owner->upperBody->feedIn->getCushionPointImportanceRef()) );

  junc_in_braceHazard = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazardImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->armsBraceBehaviourInterface->out->getHazard()), (float*)&(module->owner->owner->armsBraceBehaviourInterface->out->getHazardImportanceRef()) );

  junc_in_shouldBrace = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->in->getShouldBrace()), (float*)&(module->owner->in->getShouldBraceImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmBrace_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_maxArmExtensionScale = ER::Junction::relocate(resource);
  // junc_in_maxArmExtensionScale->getEdges()[0].reinit( (char*)&(module->owner->owner->armsBraceBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->armsBraceBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );
  // junc_in_maxArmExtensionScale->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScale()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getMaxArmExtensionScaleImportanceRef()) );

  junc_in_swivelAmount = ER::Junction::relocate(resource);
  // junc_in_swivelAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->armsBraceBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->armsBraceBehaviourInterface->out->getSwivelAmountImportanceRef()) );
  // junc_in_swivelAmount->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmount()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getSwivelAmountImportanceRef()) );

  junc_in_protectState = ER::Junction::relocate(resource);
  // junc_in_protectState->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getChestState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getChestStateImportanceRef()) );

  junc_in_cushionPoint = ER::Junction::relocate(resource);
  // junc_in_cushionPoint->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->feedIn->getCushionPoint()), (float*)&(module->owner->owner->upperBody->feedIn->getCushionPointImportanceRef()) );

  junc_in_braceHazard = ER::Junction::relocate(resource);
  // junc_in_braceHazard->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazard()), (float*)&(module->owner->owner->hazardManagement->hazardResponse->out->getBraceHazardImportanceRef()) );
  // junc_in_braceHazard->getEdges()[1].reinit( (char*)&(module->owner->owner->armsBraceBehaviourInterface->out->getHazard()), (float*)&(module->owner->owner->armsBraceBehaviourInterface->out->getHazardImportanceRef()) );

  junc_in_shouldBrace = ER::Junction::relocate(resource);
  // junc_in_shouldBrace->getEdges()[0].reinit( (char*)&(module->owner->in->getShouldBrace()), (float*)&(module->owner->in->getShouldBraceImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmBrace_Con::combineInputs(ER::Module* modulePtr)
{
  ArmBrace* module = (ArmBrace*)modulePtr;
  ArmBraceInputs& in = *module->in;

  // Junction assignments.
  in.m_maxArmExtensionScaleImportance = junc_in_maxArmExtensionScale->combinePriority(&in.maxArmExtensionScale);
  in.m_swivelAmountImportance = junc_in_swivelAmount->combinePriority(&in.swivelAmount);
  in.m_protectStateImportance = junc_in_protectState->combineDirectInput(&in.protectState);
  in.m_cushionPointImportance = junc_in_cushionPoint->combineDirectInput(&in.cushionPoint);
  in.m_braceHazardImportance = junc_in_braceHazard->combineWinnerTakesAll(&in.braceHazard);
  in.m_shouldBraceImportance = junc_in_shouldBrace->combineDirectInput(&in.shouldBrace);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


