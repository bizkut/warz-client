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
#include "BalanceManagement.h"
#include "BalancePoser.h"
#include "BalancePoserPackaging.h"
#include "MyNetwork.h"
#include "BalancePoserBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "StaticBalance.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalancePoser_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BalancePoser*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BalancePoser_Con::~BalancePoser_Con()
{
  junc_in_headRelChestOrientation_0->destroy();
  junc_in_chestRelPelvisTranslation_0->destroy();
  junc_in_chestRelPelvisOrientation_0->destroy();
  junc_in_pelvisRelSupport->destroy();
  junc_in_poseParameters->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BalancePoser_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BalancePoser_Con), 16);

  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BalancePoser_Con::buildConnections(BalancePoser* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalancePoser_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_poseParameters = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balancePoserBehaviourInterface->out->getPoseParameters()), (float*)&(module->owner->owner->balancePoserBehaviourInterface->out->getPoseParametersImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getPoseParameters()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getPoseParametersImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getPoseParameters()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getPoseParametersImportanceRef()) );

  junc_in_pelvisRelSupport = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getPelvisRelSupport()), (float*)&(module->owner->staticBalance->out->getPelvisRelSupportImportanceRef()) );

  junc_in_chestRelPelvisOrientation_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getChestRelPelvisOrientation(0)), (float*)&(module->owner->staticBalance->out->getChestRelPelvisOrientationImportanceRef(0)) );

  junc_in_chestRelPelvisTranslation_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getChestRelPelvisTranslation(0)), (float*)&(module->owner->staticBalance->out->getChestRelPelvisTranslationImportanceRef(0)) );

  junc_in_headRelChestOrientation_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getHeadRelChestOrientation(0)), (float*)&(module->owner->staticBalance->out->getHeadRelChestOrientationImportanceRef(0)) );

}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoser_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BalancePoser_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_poseParameters = ER::Junction::relocate(resource);
  // junc_in_poseParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->balancePoserBehaviourInterface->out->getPoseParameters()), (float*)&(module->owner->owner->balancePoserBehaviourInterface->out->getPoseParametersImportanceRef()) );
  // junc_in_poseParameters->getEdges()[1].reinit( (char*)&(module->owner->owner->shieldBehaviourInterface->out->getPoseParameters()), (float*)&(module->owner->owner->shieldBehaviourInterface->out->getPoseParametersImportanceRef()) );
  // junc_in_poseParameters->getEdges()[2].reinit( (char*)&(module->owner->owner->shieldActionBehaviourInterface->out->getPoseParameters()), (float*)&(module->owner->owner->shieldActionBehaviourInterface->out->getPoseParametersImportanceRef()) );

  junc_in_pelvisRelSupport = ER::Junction::relocate(resource);
  // junc_in_pelvisRelSupport->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getPelvisRelSupport()), (float*)&(module->owner->staticBalance->out->getPelvisRelSupportImportanceRef()) );

  junc_in_chestRelPelvisOrientation_0 = ER::Junction::relocate(resource);
  // junc_in_chestRelPelvisOrientation_0->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getChestRelPelvisOrientation(0)), (float*)&(module->owner->staticBalance->out->getChestRelPelvisOrientationImportanceRef(0)) );

  junc_in_chestRelPelvisTranslation_0 = ER::Junction::relocate(resource);
  // junc_in_chestRelPelvisTranslation_0->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getChestRelPelvisTranslation(0)), (float*)&(module->owner->staticBalance->out->getChestRelPelvisTranslationImportanceRef(0)) );

  junc_in_headRelChestOrientation_0 = ER::Junction::relocate(resource);
  // junc_in_headRelChestOrientation_0->getEdges()[0].reinit( (char*)&(module->owner->staticBalance->out->getHeadRelChestOrientation(0)), (float*)&(module->owner->staticBalance->out->getHeadRelChestOrientationImportanceRef(0)) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BalancePoser_Con::combineInputs(ER::Module* modulePtr)
{
  BalancePoser* module = (BalancePoser*)modulePtr;
  BalancePoserInputs& in = *module->in;

  // Junction assignments.
  in.m_poseParametersImportance = junc_in_poseParameters->combinePriority(&in.poseParameters);
  in.m_pelvisRelSupportImportance = junc_in_pelvisRelSupport->combineDirectInput(&in.pelvisRelSupport);
  in.m_chestRelPelvisOrientationImportance[0] = junc_in_chestRelPelvisOrientation_0->combineDirectInput(&in.chestRelPelvisOrientation[0]);
  in.m_chestRelPelvisTranslationImportance[0] = junc_in_chestRelPelvisTranslation_0->combineDirectInput(&in.chestRelPelvisTranslation[0]);
  in.m_headRelChestOrientationImportance[0] = junc_in_headRelChestOrientation_0->combineDirectInput(&in.headRelChestOrientation[0]);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


