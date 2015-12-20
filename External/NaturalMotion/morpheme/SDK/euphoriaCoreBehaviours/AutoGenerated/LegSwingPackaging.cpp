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
#include "LegSwing.h"
#include "LegSwingPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "SitBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "LegsPedalBehaviourInterface.h"
#include "BodySection.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegSwing_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegSwing*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegSwing_Con::~LegSwing_Con()
{
  junc_in_maxSwingTimeOnGround->destroy();
  junc_pri01->destroy();
  junc_swingFrequencyScale->destroy();
  junc_in_spineInContact->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegSwing_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegSwing_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegSwing_Con::buildConnections(LegSwing* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegSwing_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_spineInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContactImportanceRef()) );

  junc_swingFrequencyScale = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegsSwingWeight()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegsSwingWeightImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getLegsSwingWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getLegsSwingWeightImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSwingStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSwingStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_pri01 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->out->getRotationRequest()), (float*)&(module->owner->owner->lowerBody->out->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_maxSwingTimeOnGround = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getMaxSwingTimeOnGround()), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getMaxSwingTimeOnGroundImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegSwing_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegSwing_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_spineInContact = ER::Junction::relocate(resource);
  // junc_in_spineInContact->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContactImportanceRef()) );

  junc_swingFrequencyScale = ER::Junction::relocate(resource);
  // junc_swingFrequencyScale->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegsSwingWeight()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegsSwingWeightImportanceRef()) );
  // junc_swingFrequencyScale->getEdges()[1].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getLegsSwingWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getLegsSwingWeightImportanceRef()) );
  // junc_swingFrequencyScale->getEdges()[2].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSwingStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSwingStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_pri01 = ER::Junction::relocate(resource);
  // junc_pri01->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->out->getRotationRequest()), (float*)&(module->owner->owner->lowerBody->out->getRotationRequestImportanceRef()) );
  // junc_pri01->getEdges()[1].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_maxSwingTimeOnGround = ER::Junction::relocate(resource);
  // junc_in_maxSwingTimeOnGround->getEdges()[0].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getMaxSwingTimeOnGround()), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getMaxSwingTimeOnGroundImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegSwing_Con::combineInputs(ER::Module* modulePtr)
{
  LegSwing* module = (LegSwing*)modulePtr;
  LegSwingInputs& in = *module->in;

  // Junction assignments.
  in.m_spineInContactImportance = junc_in_spineInContact->combineDirectInput(&in.spineInContact);
  in.m_swingStiffnessScaleImportance = junc_swingFrequencyScale->combinePriority(&in.swingStiffnessScale);
  in.m_rotationRequestImportance = junc_pri01->combinePriority(&in.rotationRequest);
  in.m_maxSwingTimeOnGroundImportance = junc_in_maxSwingTimeOnGround->combineDirectInput(&in.maxSwingTimeOnGround);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


