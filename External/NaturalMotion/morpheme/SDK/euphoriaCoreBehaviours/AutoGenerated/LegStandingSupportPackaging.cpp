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
#include "LegStandingSupport.h"
#include "LegStandingSupportPackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegStandingSupport*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegStandingSupport_Con::~LegStandingSupport_Con()
{
  junc_in_balancePoseEndRelativeToRoot->destroy();
  junc_in_supportStrengthScale->destroy();
  junc_in_stabiliseRootAmount->destroy();
  junc_in_rootDesiredTM->destroy();
  junc_in_enableExternalSupport->destroy();
  junc_in_balanceParameters->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegStandingSupport_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegStandingSupport_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport_Con::buildConnections(LegStandingSupport* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegStandingSupport_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_balanceParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParametersImportanceRef()) );

  junc_in_enableExternalSupport = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_in_rootDesiredTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredPelvisTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredPelvisTMImportanceRef()) );

  junc_in_stabiliseRootAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabilisePelvisAmount()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabilisePelvisAmountImportanceRef()) );

  junc_in_supportStrengthScale = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getLegSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getLegSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_balancePoseEndRelativeToRoot = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegStandingSupport_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_balanceParameters = ER::Junction::relocate(resource);
  // junc_in_balanceParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParametersImportanceRef()) );

  junc_in_enableExternalSupport = ER::Junction::relocate(resource);
  // junc_in_enableExternalSupport->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_in_rootDesiredTM = ER::Junction::relocate(resource);
  // junc_in_rootDesiredTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredPelvisTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredPelvisTMImportanceRef()) );

  junc_in_stabiliseRootAmount = ER::Junction::relocate(resource);
  // junc_in_stabiliseRootAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabilisePelvisAmount()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabilisePelvisAmountImportanceRef()) );

  junc_in_supportStrengthScale = ER::Junction::relocate(resource);
  // junc_in_supportStrengthScale->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getLegSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getLegSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_balancePoseEndRelativeToRoot = ER::Junction::relocate(resource);
  // junc_in_balancePoseEndRelativeToRoot->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getLegBalancePoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegStandingSupport_Con::combineInputs(ER::Module* modulePtr)
{
  LegStandingSupport* module = (LegStandingSupport*)modulePtr;
  LegStandingSupportInputs& in = *module->in;

  // Junction assignments.
  in.m_balanceParametersImportance = junc_in_balanceParameters->combineDirectInput(&in.balanceParameters);
  in.m_enableExternalSupportImportance = junc_in_enableExternalSupport->combineDirectInput(&in.enableExternalSupport);
  in.m_rootDesiredTMImportance = junc_in_rootDesiredTM->combineDirectInput(&in.rootDesiredTM);
  in.m_stabiliseRootAmountImportance = junc_in_stabiliseRootAmount->combineDirectInput(&in.stabiliseRootAmount);
  in.m_supportStrengthScaleImportance = junc_in_supportStrengthScale->combineDirectInput(&in.supportStrengthScale);
  in.m_balancePoseEndRelativeToRootImportance = junc_in_balancePoseEndRelativeToRoot->combineDirectInput(&in.balancePoseEndRelativeToRoot);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


