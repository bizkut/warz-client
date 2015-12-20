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
#include "BodyFrame.h"
#include "BalanceAssistant.h"
#include "BalanceAssistantPackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStep.h"
#include "BodyBalance.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BalanceAssistant*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BalanceAssistant_Con::~BalanceAssistant_Con()
{
  junc_junc_FeedbackInputs_isStepping_1->destroy();
  junc_junc_FeedbackInputs_isStepping_0->destroy();
  junc_in_balanceAssistanceParameters->destroy();
  junc_in_balanceAmount->destroy();
  junc_in_desiredPelvisTM->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BalanceAssistant_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BalanceAssistant_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant_Con::buildConnections(BalanceAssistant* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalanceAssistant_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_desiredPelvisTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyBalance->out->getTargetPelvisTM()), (float*)&(module->owner->bodyBalance->out->getTargetPelvisTMImportanceRef()) );

  junc_in_balanceAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->owner->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );

  junc_in_balanceAssistanceParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceAssistanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceAssistanceParametersImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalanceAssistant_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_desiredPelvisTM = ER::Junction::relocate(resource);
  // junc_in_desiredPelvisTM->getEdges()[0].reinit( (char*)&(module->owner->bodyBalance->out->getTargetPelvisTM()), (float*)&(module->owner->bodyBalance->out->getTargetPelvisTMImportanceRef()) );

  junc_in_balanceAmount = ER::Junction::relocate(resource);
  // junc_in_balanceAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyBalance->feedOut->getBalanceAmount()), (float*)&(module->owner->bodyBalance->feedOut->getBalanceAmountImportanceRef()) );

  junc_in_balanceAssistanceParameters = ER::Junction::relocate(resource);
  // junc_in_balanceAssistanceParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceAssistanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceAssistanceParametersImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isStepping_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[0]->step->feedOut->getIsSteppingImportanceRef()) );

  junc_junc_FeedbackInputs_isStepping_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_isStepping_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->step->feedOut->getIsStepping()), (float*)&(module->owner->owner->legs[1]->step->feedOut->getIsSteppingImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceAssistant_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BalanceAssistant* module = (BalanceAssistant*)modulePtr;
  BalanceAssistantFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_isSteppingImportance[0] = junc_junc_FeedbackInputs_isStepping_0->combineDirectInput(&feedIn.isStepping[0]);
  feedIn.m_isSteppingImportance[1] = junc_junc_FeedbackInputs_isStepping_1->combineDirectInput(&feedIn.isStepping[1]);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalanceAssistant_Con::combineInputs(ER::Module* modulePtr)
{
  BalanceAssistant* module = (BalanceAssistant*)modulePtr;
  BalanceAssistantInputs& in = *module->in;

  // Junction assignments.
  in.m_desiredPelvisTMImportance = junc_in_desiredPelvisTM->combineDirectInput(&in.desiredPelvisTM);
  in.m_balanceAmountImportance = junc_in_balanceAmount->combineDirectInput(&in.balanceAmount);
  in.m_balanceAssistanceParametersImportance = junc_in_balanceAssistanceParameters->combineDirectInput(&in.balanceAssistanceParameters);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


