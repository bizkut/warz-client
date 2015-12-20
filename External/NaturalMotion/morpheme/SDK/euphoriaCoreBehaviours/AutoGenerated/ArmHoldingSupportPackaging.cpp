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
#include "ArmHoldingSupport.h"
#include "ArmHoldingSupportPackaging.h"
#include "MyNetwork.h"
#include "BodySection.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "ArmHold.h"
#include "ArmGrab.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmHoldingSupport*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmHoldingSupport_Con::~ArmHoldingSupport_Con()
{
  junc_in_supportStrengthScale->destroy();
  junc_in_endDesiredTM->destroy();
  junc_in_rootDesiredTM->destroy();
  junc_in_translationRequest->destroy();
  junc_in_rotationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmHoldingSupport_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmHoldingSupport_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport_Con::buildConnections(ArmHoldingSupport* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmHoldingSupport_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_rotationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getRotationRequest()), (float*)&(module->owner->owner->upperBody->out->getRotationRequestImportanceRef()) );

  junc_in_translationRequest = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getTranslationRequest()), (float*)&(module->owner->owner->upperBody->out->getTranslationRequestImportanceRef()) );

  junc_in_rootDesiredTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTMImportanceRef()) );

  junc_in_endDesiredTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hold->out->getEndDesiredTM()), (float*)&(module->owner->hold->out->getEndDesiredTMImportanceRef()) );

  junc_in_supportStrengthScale = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getHoldStrengthScale()), (float*)&(module->owner->grab->out->getHoldStrengthScaleImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmHoldingSupport_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_rotationRequest = ER::Junction::relocate(resource);
  // junc_in_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getRotationRequest()), (float*)&(module->owner->owner->upperBody->out->getRotationRequestImportanceRef()) );

  junc_in_translationRequest = ER::Junction::relocate(resource);
  // junc_in_translationRequest->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getTranslationRequest()), (float*)&(module->owner->owner->upperBody->out->getTranslationRequestImportanceRef()) );

  junc_in_rootDesiredTM = ER::Junction::relocate(resource);
  // junc_in_rootDesiredTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTMImportanceRef()) );

  junc_in_endDesiredTM = ER::Junction::relocate(resource);
  // junc_in_endDesiredTM->getEdges()[0].reinit( (char*)&(module->owner->hold->out->getEndDesiredTM()), (float*)&(module->owner->hold->out->getEndDesiredTMImportanceRef()) );

  junc_in_supportStrengthScale = ER::Junction::relocate(resource);
  // junc_in_supportStrengthScale->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getHoldStrengthScale()), (float*)&(module->owner->grab->out->getHoldStrengthScaleImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmHoldingSupport_Con::combineInputs(ER::Module* modulePtr)
{
  ArmHoldingSupport* module = (ArmHoldingSupport*)modulePtr;
  ArmHoldingSupportInputs& in = *module->in;

  // Junction assignments.
  in.m_rotationRequestImportance = junc_in_rotationRequest->combineDirectInput(&in.rotationRequest);
  in.m_translationRequestImportance = junc_in_translationRequest->combineDirectInput(&in.translationRequest);
  in.m_rootDesiredTMImportance = junc_in_rootDesiredTM->combineDirectInput(&in.rootDesiredTM);
  in.m_endDesiredTMImportance = junc_in_endDesiredTM->combineDirectInput(&in.endDesiredTM);
  in.m_supportStrengthScaleImportance = junc_in_supportStrengthScale->combineDirectInput(&in.supportStrengthScale);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


