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
#include "ArmReachForWorld.h"
#include "ArmReachForWorldPackaging.h"
#include "ArmGrab.h"
#include "MyNetwork.h"
#include "ReachForWorldBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmReachForWorld*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmReachForWorld_Con::~ArmReachForWorld_Con()
{
  junc_in_supportTM->destroy();
  junc_in_supportAmount->destroy();
  junc_reachActionParams->destroy();
  junc_target->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmReachForWorld_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmReachForWorld_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld_Con::buildConnections(ArmReachForWorld* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmReachForWorld_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_target = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getLimbControl()), (float*)&(module->owner->grab->out->getLimbControlImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLimbControls(module->owner->getChildIndex())), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLimbControlsImportanceRef(module->owner->getChildIndex())) );

  junc_reachActionParams = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getReachParams()), (float*)&(module->owner->grab->out->getReachParamsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getReachParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getReachParamsImportanceRef(module->owner->getChildIndex())) );

  junc_in_supportAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_supportTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmReachForWorld_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_target = ER::Junction::relocate(resource);
  // junc_target->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getLimbControl()), (float*)&(module->owner->grab->out->getLimbControlImportanceRef()) );
  // junc_target->getEdges()[1].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLimbControls(module->owner->getChildIndex())), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLimbControlsImportanceRef(module->owner->getChildIndex())) );

  junc_reachActionParams = ER::Junction::relocate(resource);
  // junc_reachActionParams->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getReachParams()), (float*)&(module->owner->grab->out->getReachParamsImportanceRef()) );
  // junc_reachActionParams->getEdges()[1].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getReachParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getReachParamsImportanceRef(module->owner->getChildIndex())) );

  junc_in_supportAmount = ER::Junction::relocate(resource);
  // junc_in_supportAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmount()), (float*)&(module->owner->owner->bodyFrame->feedOut->getSupportAmountImportanceRef()) );

  junc_in_supportTM = ER::Junction::relocate(resource);
  // junc_in_supportTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSupportTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmReachForWorld_Con::combineInputs(ER::Module* modulePtr)
{
  ArmReachForWorld* module = (ArmReachForWorld*)modulePtr;
  ArmReachForWorldInputs& in = *module->in;

  // Junction assignments.
  in.m_limbControlImportance = junc_target->combineWinnerTakesAll(&in.limbControl);
  in.m_reachParamsImportance = junc_reachActionParams->combineWinnerTakesAll(&in.reachParams);
  in.m_supportAmountImportance = junc_in_supportAmount->combineDirectInput(&in.supportAmount);
  in.m_supportTMImportance = junc_in_supportTM->combineDirectInput(&in.supportTM);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


