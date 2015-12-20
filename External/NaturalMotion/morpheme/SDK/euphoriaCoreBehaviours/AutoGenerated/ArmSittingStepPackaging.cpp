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
#include "ArmSittingStep.h"
#include "ArmSittingStepPackaging.h"
#include "MyNetwork.h"
#include "SitBehaviourInterface.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmSittingStep*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmSittingStep_Con::~ArmSittingStep_Con()
{
  junc_feedIn_supportImportance->destroy();
  junc_in_sitParameters->destroy();
  junc_in_armPoseEndRelativeToRoot->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmSittingStep_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmSittingStep_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep_Con::buildConnections(ArmSittingStep* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmSittingStep_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_armPoseEndRelativeToRoot = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

  junc_in_sitParameters = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSitParameters()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSitParametersImportanceRef()) );

  junc_feedIn_supportImportance = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportImportance()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportImportanceImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmSittingStep_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_armPoseEndRelativeToRoot = ER::Junction::relocate(resource);
  // junc_in_armPoseEndRelativeToRoot->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->sitBehaviourInterface->out->getArmPoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

  junc_in_sitParameters = ER::Junction::relocate(resource);
  // junc_in_sitParameters->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSitParameters()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSitParametersImportanceRef()) );

  junc_feedIn_supportImportance = ER::Junction::relocate(resource);
  // junc_feedIn_supportImportance->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportImportance()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getArmSupportImportanceImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmSittingStep_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ArmSittingStep* module = (ArmSittingStep*)modulePtr;
  ArmSittingStepFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportImportanceImportance = junc_feedIn_supportImportance->combineDirectInput(&feedIn.supportImportance);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmSittingStep_Con::combineInputs(ER::Module* modulePtr)
{
  ArmSittingStep* module = (ArmSittingStep*)modulePtr;
  ArmSittingStepInputs& in = *module->in;

  // Junction assignments.
  in.m_armPoseEndRelativeToRootImportance = junc_in_armPoseEndRelativeToRoot->combineDirectInput(&in.armPoseEndRelativeToRoot);
  in.m_sitParametersImportance = junc_in_sitParameters->combineDirectInput(&in.sitParameters);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


