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
#include "MyNetwork.h"
#include "AimBehaviourInterface.h"
#include "AimBehaviourInterfacePackaging.h"
#include "Arm.h"
#include "ArmAim.h"
#include "Spine.h"
#include "Leg.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((AimBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
AimBehaviourInterface_Con::~AimBehaviourInterface_Con()
{
  junc_feedIn_averageFootOnGroundAmount->destroy();
  junc_feedIn_spineInContact->destroy();
  junc_feedIn_armAimLimbControl_1->destroy();
  junc_feedIn_armAimLimbControl_0->destroy();
  junc_feedIn_armFinalLimbControl_1->destroy();
  junc_feedIn_armFinalLimbControl_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AimBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AimBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface_Con::buildConnections(AimBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(AimBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_armFinalLimbControl_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->out->getControl()), (float*)&(module->owner->arms[0]->out->getControlImportanceRef()) );
  junc_feedIn_armFinalLimbControl_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->out->getControl()), (float*)&(module->owner->arms[1]->out->getControlImportanceRef()) );

  junc_feedIn_armAimLimbControl_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->aim->out->getControl()), (float*)&(module->owner->arms[0]->aim->out->getControlImportanceRef()) );
  junc_feedIn_armAimLimbControl_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->aim->out->getControl()), (float*)&(module->owner->arms[1]->aim->out->getControlImportanceRef()) );

  junc_feedIn_spineInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_averageFootOnGroundAmount = activeJnc = ER::Junction::init(resource, 2, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->legs[0]->feedOut->getEndOnGroundAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->legs[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(AimBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_armFinalLimbControl_0 = ER::Junction::relocate(resource);
  // junc_feedIn_armFinalLimbControl_0->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->out->getControl()), (float*)&(module->owner->arms[0]->out->getControlImportanceRef()) );
  junc_feedIn_armFinalLimbControl_1 = ER::Junction::relocate(resource);
  // junc_feedIn_armFinalLimbControl_1->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->out->getControl()), (float*)&(module->owner->arms[1]->out->getControlImportanceRef()) );

  junc_feedIn_armAimLimbControl_0 = ER::Junction::relocate(resource);
  // junc_feedIn_armAimLimbControl_0->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->aim->out->getControl()), (float*)&(module->owner->arms[0]->aim->out->getControlImportanceRef()) );
  junc_feedIn_armAimLimbControl_1 = ER::Junction::relocate(resource);
  // junc_feedIn_armAimLimbControl_1->getEdges()[0].reinit( (char*)&(module->owner->arms[1]->aim->out->getControl()), (float*)&(module->owner->arms[1]->aim->out->getControlImportanceRef()) );

  junc_feedIn_spineInContact = ER::Junction::relocate(resource);
  // junc_feedIn_spineInContact->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_averageFootOnGroundAmount = ER::Junction::relocate(resource);
  // junc_feedIn_averageFootOnGroundAmount->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->legs[0]->feedOut->getEndOnGroundAmountImportanceRef()) );
  // junc_feedIn_averageFootOnGroundAmount->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getEndOnGroundAmount()), (float*)&(module->owner->legs[1]->feedOut->getEndOnGroundAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void AimBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  AimBehaviourInterface* module = (AimBehaviourInterface*)modulePtr;
  AimBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_armFinalLimbControlImportance[0] = junc_feedIn_armFinalLimbControl_0->combineDirectInput(&feedIn.armFinalLimbControl[0]);
  feedIn.m_armFinalLimbControlImportance[1] = junc_feedIn_armFinalLimbControl_1->combineDirectInput(&feedIn.armFinalLimbControl[1]);
  feedIn.m_armAimLimbControlImportance[0] = junc_feedIn_armAimLimbControl_0->combineDirectInput(&feedIn.armAimLimbControl[0]);
  feedIn.m_armAimLimbControlImportance[1] = junc_feedIn_armAimLimbControl_1->combineDirectInput(&feedIn.armAimLimbControl[1]);
  feedIn.m_spineInContactImportance = junc_feedIn_spineInContact->combineDirectInput(&feedIn.spineInContact);
  feedIn.m_feetOnGroundAmountImportance = junc_feedIn_averageFootOnGroundAmount->combineAverage(&feedIn.feetOnGroundAmount);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


