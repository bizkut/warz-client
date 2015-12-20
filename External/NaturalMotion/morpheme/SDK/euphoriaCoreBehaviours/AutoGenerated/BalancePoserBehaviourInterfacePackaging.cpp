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
#include "BalancePoserBehaviourInterface.h"
#include "BalancePoserBehaviourInterfacePackaging.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BalancePoserBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BalancePoserBehaviourInterface_Con::~BalancePoserBehaviourInterface_Con()
{
  junc_feedIn_supportTM->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BalancePoserBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BalancePoserBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface_Con::buildConnections(BalancePoserBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalancePoserBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalancePoserBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_supportTM = ER::Junction::relocate(resource);
  // junc_feedIn_supportTM->getEdges()[0].reinit( (char*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTM()), (float*)&(module->owner->bodyFrame->bodyBalance->feedOut->getSupportTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalancePoserBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BalancePoserBehaviourInterface* module = (BalancePoserBehaviourInterface*)modulePtr;
  BalancePoserBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportTMImportance = junc_feedIn_supportTM->combineDirectInput(&feedIn.supportTM);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


