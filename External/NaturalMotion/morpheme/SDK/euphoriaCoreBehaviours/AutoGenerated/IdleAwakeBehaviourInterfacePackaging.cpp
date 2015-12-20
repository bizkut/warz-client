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
#include "IdleAwakeBehaviourInterface.h"
#include "IdleAwakeBehaviourInterfacePackaging.h"
#include "BodyFrame.h"
#include "Spine.h"
#include "Head.h"
#include "Leg.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "LegStandingSupport.h"
#include "BodyBalance.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((IdleAwakeBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
IdleAwakeBehaviourInterface_Con::~IdleAwakeBehaviourInterface_Con()
{
  junc_feedIn_isStepping->destroy();
  junc_feedIn_armOrLegSupportAmount->destroy();
  junc_feedIn_armsInContact->destroy();
  junc_feedIn_legsInContact->destroy();
  junc_feedIn_headInContact->destroy();
  junc_feedIn_spineInContact->destroy();
  junc_feedIn_supportAmount->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format IdleAwakeBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(IdleAwakeBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface_Con::buildConnections(IdleAwakeBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(IdleAwakeBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_spineInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_headInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_legsInContact = activeJnc = ER::Junction::init(resource, 2, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_armsInContact = activeJnc = ER::Junction::init(resource, 2, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_armOrLegSupportAmount = activeJnc = ER::Junction::init(resource, 4, ER::Junction::maxFloatValue);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_isStepping = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsStepping()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsSteppingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(IdleAwakeBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_supportAmount = ER::Junction::relocate(resource);
  // junc_feedIn_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_spineInContact = ER::Junction::relocate(resource);
  // junc_feedIn_spineInContact->getEdges()[0].reinit( (char*)&(module->owner->spines[0]->feedOut->getInContact()), (float*)&(module->owner->spines[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_headInContact = ER::Junction::relocate(resource);
  // junc_feedIn_headInContact->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->feedOut->getInContact()), (float*)&(module->owner->heads[0]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_legsInContact = ER::Junction::relocate(resource);
  // junc_feedIn_legsInContact->getEdges()[0].reinit( (char*)&(module->owner->legs[0]->feedOut->getInContact()), (float*)&(module->owner->legs[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_legsInContact->getEdges()[1].reinit( (char*)&(module->owner->legs[1]->feedOut->getInContact()), (float*)&(module->owner->legs[1]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_armsInContact = ER::Junction::relocate(resource);
  // junc_feedIn_armsInContact->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->feedOut->getInContact()), (float*)&(module->owner->arms[0]->feedOut->getInContactImportanceRef()) );
  // junc_feedIn_armsInContact->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->feedOut->getInContact()), (float*)&(module->owner->arms[1]->feedOut->getInContactImportanceRef()) );

  junc_feedIn_armOrLegSupportAmount = ER::Junction::relocate(resource);
  // junc_feedIn_armOrLegSupportAmount->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->arms[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_armOrLegSupportAmount->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->arms[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_armOrLegSupportAmount->getEdges()[2].reinit( (char*)&(module->owner->legs[0]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->legs[0]->standingSupport->feedOut->getSupportAmountImportanceRef()) );
  // junc_feedIn_armOrLegSupportAmount->getEdges()[3].reinit( (char*)&(module->owner->legs[1]->standingSupport->feedOut->getSupportAmount()), (float*)&(module->owner->legs[1]->standingSupport->feedOut->getSupportAmountImportanceRef()) );

  junc_feedIn_isStepping = ER::Junction::relocate(resource);
  // junc_feedIn_isStepping->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsStepping()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getIsSteppingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void IdleAwakeBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  IdleAwakeBehaviourInterface* module = (IdleAwakeBehaviourInterface*)modulePtr;
  IdleAwakeBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportAmountImportance = junc_feedIn_supportAmount->combineDirectInput(&feedIn.supportAmount);
  feedIn.m_spineInContactImportance = junc_feedIn_spineInContact->combineDirectInput(&feedIn.spineInContact);
  feedIn.m_headsInContactImportance = junc_feedIn_headInContact->combineDirectInput(&feedIn.headsInContact);
  feedIn.m_legsInContactImportance = junc_feedIn_legsInContact->combineIsBoolSet(&feedIn.legsInContact);
  feedIn.m_armsInContactImportance = junc_feedIn_armsInContact->combineIsBoolSet(&feedIn.armsInContact);
  feedIn.m_armOrLegSupportAmountImportance = junc_feedIn_armOrLegSupportAmount->combineMaxFloatValue(&feedIn.armOrLegSupportAmount);
  feedIn.m_isSteppingImportance = junc_feedIn_isStepping->combineDirectInput(&feedIn.isStepping);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


