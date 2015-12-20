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
#include "SitBehaviourInterface.h"
#include "SitBehaviourInterfacePackaging.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SitBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SitBehaviourInterface_Con::~SitBehaviourInterface_Con()
{
  junc_feedOut_sitAmount->destroy();
  junc_feedIn_sitAmount->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SitBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SitBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourInterface_Con::buildConnections(SitBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SitBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_sitAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->sittingBodyBalance->feedOut->getSitAmount()), (float*)&(module->owner->bodyFrame->sittingBodyBalance->feedOut->getSitAmountImportanceRef()) );

  junc_feedOut_sitAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->feedIn->getSitAmount()), (float*)&(module->feedIn->getSitAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SitBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_sitAmount = ER::Junction::relocate(resource);
  // junc_feedIn_sitAmount->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->sittingBodyBalance->feedOut->getSitAmount()), (float*)&(module->owner->bodyFrame->sittingBodyBalance->feedOut->getSitAmountImportanceRef()) );

  junc_feedOut_sitAmount = ER::Junction::relocate(resource);
  // junc_feedOut_sitAmount->getEdges()[0].reinit( (char*)&(module->feedIn->getSitAmount()), (float*)&(module->feedIn->getSitAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SitBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  SitBehaviourInterface* module = (SitBehaviourInterface*)modulePtr;
  SitBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_sitAmountImportance = junc_feedIn_sitAmount->combineDirectInput(&feedIn.sitAmount);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SitBehaviourInterface_Con::combineFeedbackOutputs(ER::Module* modulePtr)
{
  SitBehaviourInterface* module = (SitBehaviourInterface*)modulePtr;
  SitBehaviourInterfaceFeedbackOutputs& feedOut = *module->feedOut;

  // Junction assignments.
  NMP_ASSERT(feedOut.m_sitAmountImportance == 0.0f);
  feedOut.m_sitAmountImportance = junc_feedOut_sitAmount->combineDirectInput(&feedOut.sitAmount);

#if NM_CALL_VALIDATORS
  feedOut.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


