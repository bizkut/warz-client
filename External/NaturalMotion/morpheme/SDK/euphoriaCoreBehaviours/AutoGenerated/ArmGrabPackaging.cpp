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
#include "ArmGrab.h"
#include "ArmGrabPackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "HoldBehaviourInterface.h"
#include "HoldActionBehaviourInterface.h"
#include "ArmHold.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmGrab*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmGrab_Con::~ArmGrab_Con()
{
  junc_feedIn_timeHeldProportion->destroy();
  junc_in_importance->destroy();
  junc_in_aliveParams->destroy();
  junc_in_failureConditions->destroy();
  junc_in_isHanging->destroy();
  junc_in_endConstraintOptions->destroy();
  junc_in_hasTwistedAround->destroy();
  junc_in_numConstrainedArms->destroy();
  junc_in_edgeIsObstructed->destroy();
  junc_in_edgeToGrab->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmGrab_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmGrab_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmGrab_Con::buildConnections(ArmGrab* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmGrab_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_edgeToGrab = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getEdgeToGrab()), (float*)&(module->owner->owner->hazardManagement->grab->out->getEdgeToGrabImportanceRef()) );

  junc_in_edgeIsObstructed = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getEdgeIsObstructed()), (float*)&(module->owner->owner->hazardManagement->grab->out->getEdgeIsObstructedImportanceRef()) );

  junc_in_numConstrainedArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArms()), (float*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArmsImportanceRef()) );

  junc_in_hasTwistedAround = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getHasTwistedAround()), (float*)&(module->owner->owner->hazardManagement->grab->out->getHasTwistedAroundImportanceRef()) );

  junc_in_endConstraintOptions = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getEndConstraintOptions()), (float*)&(module->owner->owner->hazardManagement->grab->out->getEndConstraintOptionsImportanceRef()) );

  junc_in_isHanging = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getIsHanging()), (float*)&(module->owner->owner->hazardManagement->grab->out->getIsHangingImportanceRef()) );

  junc_in_failureConditions = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFailureConditions()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFailureConditionsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabFailureConditions()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabFailureConditionsImportanceRef()) );

  junc_in_aliveParams = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );

  junc_in_importance = activeJnc = ER::Junction::init(resource, 2, ER::Junction::maxFloat);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getHoldPerArmImportance(module->owner->getChildIndex())), (float*)&(module->owner->owner->holdBehaviourInterface->out->getHoldPerArmImportanceImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getHoldPerArmImportance(module->owner->getChildIndex())), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getHoldPerArmImportanceImportanceRef(module->owner->getChildIndex())) );

  junc_feedIn_timeHeldProportion = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hold->feedOut->getTimeHeldProportion()), (float*)&(module->owner->hold->feedOut->getTimeHeldProportionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmGrab_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_edgeToGrab = ER::Junction::relocate(resource);
  // junc_in_edgeToGrab->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getEdgeToGrab()), (float*)&(module->owner->owner->hazardManagement->grab->out->getEdgeToGrabImportanceRef()) );

  junc_in_edgeIsObstructed = ER::Junction::relocate(resource);
  // junc_in_edgeIsObstructed->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getEdgeIsObstructed()), (float*)&(module->owner->owner->hazardManagement->grab->out->getEdgeIsObstructedImportanceRef()) );

  junc_in_numConstrainedArms = ER::Junction::relocate(resource);
  // junc_in_numConstrainedArms->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArms()), (float*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArmsImportanceRef()) );

  junc_in_hasTwistedAround = ER::Junction::relocate(resource);
  // junc_in_hasTwistedAround->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getHasTwistedAround()), (float*)&(module->owner->owner->hazardManagement->grab->out->getHasTwistedAroundImportanceRef()) );

  junc_in_endConstraintOptions = ER::Junction::relocate(resource);
  // junc_in_endConstraintOptions->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getEndConstraintOptions()), (float*)&(module->owner->owner->hazardManagement->grab->out->getEndConstraintOptionsImportanceRef()) );

  junc_in_isHanging = ER::Junction::relocate(resource);
  // junc_in_isHanging->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getIsHanging()), (float*)&(module->owner->owner->hazardManagement->grab->out->getIsHangingImportanceRef()) );

  junc_in_failureConditions = ER::Junction::relocate(resource);
  // junc_in_failureConditions->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFailureConditions()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFailureConditionsImportanceRef()) );
  // junc_in_failureConditions->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabFailureConditions()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabFailureConditionsImportanceRef()) );

  junc_in_aliveParams = ER::Junction::relocate(resource);
  // junc_in_aliveParams->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );
  // junc_in_aliveParams->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );

  junc_in_importance = ER::Junction::relocate(resource);
  // junc_in_importance->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getHoldPerArmImportance(module->owner->getChildIndex())), (float*)&(module->owner->owner->holdBehaviourInterface->out->getHoldPerArmImportanceImportanceRef(module->owner->getChildIndex())) );
  // junc_in_importance->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getHoldPerArmImportance(module->owner->getChildIndex())), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getHoldPerArmImportanceImportanceRef(module->owner->getChildIndex())) );

  junc_feedIn_timeHeldProportion = ER::Junction::relocate(resource);
  // junc_feedIn_timeHeldProportion->getEdges()[0].reinit( (char*)&(module->owner->hold->feedOut->getTimeHeldProportion()), (float*)&(module->owner->hold->feedOut->getTimeHeldProportionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmGrab_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ArmGrab* module = (ArmGrab*)modulePtr;
  ArmGrabFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_timeHeldProportionImportance = junc_feedIn_timeHeldProportion->combineDirectInput(&feedIn.timeHeldProportion);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmGrab_Con::combineInputs(ER::Module* modulePtr)
{
  ArmGrab* module = (ArmGrab*)modulePtr;
  ArmGrabInputs& in = *module->in;

  // Junction assignments.
  in.m_edgeToGrabImportance = junc_in_edgeToGrab->combineDirectInput(&in.edgeToGrab);
  in.m_edgeIsObstructedImportance = junc_in_edgeIsObstructed->combineDirectInput(&in.edgeIsObstructed);
  in.m_numConstrainedArmsImportance = junc_in_numConstrainedArms->combineDirectInput(&in.numConstrainedArms);
  in.m_hasTwistedAroundImportance = junc_in_hasTwistedAround->combineDirectInput(&in.hasTwistedAround);
  in.m_endConstraintOptionsImportance = junc_in_endConstraintOptions->combineDirectInput(&in.endConstraintOptions);
  in.m_isHangingImportance = junc_in_isHanging->combineDirectInput(&in.isHanging);
  in.m_failureConditionsImportance = junc_in_failureConditions->combinePriority(&in.failureConditions);
  in.m_aliveParamsImportance = junc_in_aliveParams->combinePriority(&in.aliveParams);
  in.m_importanceImportance = junc_in_importance->combineMaxFloat(&in.importance);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


