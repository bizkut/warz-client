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
#include "ArmStandingSupport.h"
#include "ArmStandingSupportPackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "BodyBalance.h"
#include "ArmGrab.h"
#include "SitBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmStandingSupport*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmStandingSupport_Con::~ArmStandingSupport_Con()
{
  junc_in_balancePoseEndRelativeToRoot->destroy();
  junc_in_supportStrengthScale->destroy();
  junc_in_stabiliseRootAmount->destroy();
  junc_in_endDesiredTM->destroy();
  junc_in_rootDesiredTM->destroy();
  junc_in_enableExternalSupport->destroy();
  junc_in_balanceParameters->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmStandingSupport_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmStandingSupport_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport_Con::buildConnections(ArmStandingSupport* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmStandingSupport_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_balanceParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParametersImportanceRef()) );

  junc_in_enableExternalSupport = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getSupportWithArms()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getSupportWithArmsImportanceRef()) );

  junc_in_rootDesiredTM = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTMImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredChestTM()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredChestTMImportanceRef()) );

  junc_in_endDesiredTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredArmEndTM(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredArmEndTMImportanceRef(module->owner->getChildIndex())) );

  junc_in_stabiliseRootAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabiliseChestAmount()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabiliseChestAmountImportanceRef()) );

  junc_in_supportStrengthScale = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->grab->out->getHoldStrengthScale()), (float*)&(module->owner->grab->out->getHoldStrengthScaleImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_balancePoseEndRelativeToRoot = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmStandingSupport_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_balanceParameters = ER::Junction::relocate(resource);
  // junc_in_balanceParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParameters()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getBalanceParametersImportanceRef()) );

  junc_in_enableExternalSupport = ER::Junction::relocate(resource);
  // junc_in_enableExternalSupport->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );
  // junc_in_enableExternalSupport->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getSupportWithArms()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getSupportWithArmsImportanceRef()) );

  junc_in_rootDesiredTM = ER::Junction::relocate(resource);
  // junc_in_rootDesiredTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getDesiredChestTMImportanceRef()) );
  // junc_in_rootDesiredTM->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredChestTM()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredChestTMImportanceRef()) );

  junc_in_endDesiredTM = ER::Junction::relocate(resource);
  // junc_in_endDesiredTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredArmEndTM(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredArmEndTMImportanceRef(module->owner->getChildIndex())) );

  junc_in_stabiliseRootAmount = ER::Junction::relocate(resource);
  // junc_in_stabiliseRootAmount->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabiliseChestAmount()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getStabiliseChestAmountImportanceRef()) );

  junc_in_supportStrengthScale = ER::Junction::relocate(resource);
  // junc_in_supportStrengthScale->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );
  // junc_in_supportStrengthScale->getEdges()[1].reinit( (char*)&(module->owner->grab->out->getHoldStrengthScale()), (float*)&(module->owner->grab->out->getHoldStrengthScaleImportanceRef()) );
  // junc_in_supportStrengthScale->getEdges()[2].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_balancePoseEndRelativeToRoot = ER::Junction::relocate(resource);
  // junc_in_balancePoseEndRelativeToRoot->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getArmBalancePoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );
  // junc_in_balancePoseEndRelativeToRoot->getEdges()[1].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmStandingSupport_Con::combineInputs(ER::Module* modulePtr)
{
  ArmStandingSupport* module = (ArmStandingSupport*)modulePtr;
  ArmStandingSupportInputs& in = *module->in;

  // Junction assignments.
  in.m_balanceParametersImportance = junc_in_balanceParameters->combineDirectInput(&in.balanceParameters);
  in.m_enableExternalSupportImportance = junc_in_enableExternalSupport->combineWinnerTakesAll(&in.enableExternalSupport);
  in.m_rootDesiredTMImportance = junc_in_rootDesiredTM->combineWinnerTakesAll(&in.rootDesiredTM);
  in.m_endDesiredTMImportance = junc_in_endDesiredTM->combineDirectInput(&in.endDesiredTM);
  in.m_stabiliseRootAmountImportance = junc_in_stabiliseRootAmount->combineDirectInput(&in.stabiliseRootAmount);
  in.m_supportStrengthScaleImportance = junc_in_supportStrengthScale->combinePriority(&in.supportStrengthScale);
  in.m_balancePoseEndRelativeToRootImportance = junc_in_balancePoseEndRelativeToRoot->combineWinnerTakesAll(&in.balancePoseEndRelativeToRoot);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


