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
#include "ArmSpin.h"
#include "ArmSpinPackaging.h"
#include "MyNetwork.h"
#include "BodySection.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "FreeFallManagement.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmSpin*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmSpin_Con::~ArmSpin_Con()
{
  junc_in_ignoreContacts->destroy();
  junc_in_spinParams->destroy();
  junc_in_armsSpinInPhase->destroy();
  junc_in_spinStiffnessScale->destroy();
  junc_in_rotationRequest->destroy();
  junc_in_combinedTargetDirection->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmSpin_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmSpin_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmSpin_Con::buildConnections(ArmSpin* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmSpin_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_combinedTargetDirection = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->feedIn->getSpinDirection()), (float*)&(module->owner->owner->upperBody->feedIn->getSpinDirectionImportanceRef()) );

  junc_in_rotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getRotationRequest()), (float*)&(module->owner->owner->upperBody->out->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_spinStiffnessScale = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_armsSpinInPhase = activeJnc = ER::Junction::init(resource, 3, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinInPhase()), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinInPhaseImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinInPhase()), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinInPhaseImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSpinInPhase()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSpinInPhaseImportanceRef()) );

  junc_in_spinParams = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinParams()), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinParamsImportanceRef()) );

  junc_in_ignoreContacts = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getIsHanging()), (float*)&(module->owner->owner->hazardManagement->grab->out->getIsHangingImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmSpin_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_combinedTargetDirection = ER::Junction::relocate(resource);
  // junc_in_combinedTargetDirection->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->feedIn->getSpinDirection()), (float*)&(module->owner->owner->upperBody->feedIn->getSpinDirectionImportanceRef()) );

  junc_in_rotationRequest = ER::Junction::relocate(resource);
  // junc_in_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getRotationRequest()), (float*)&(module->owner->owner->upperBody->out->getRotationRequestImportanceRef()) );
  // junc_in_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_spinStiffnessScale = ER::Junction::relocate(resource);
  // junc_in_spinStiffnessScale->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );
  // junc_in_spinStiffnessScale->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_armsSpinInPhase = ER::Junction::relocate(resource);
  // junc_in_armsSpinInPhase->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinInPhase()), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getArmsSpinInPhaseImportanceRef()) );
  // junc_in_armsSpinInPhase->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinInPhase()), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinInPhaseImportanceRef()) );
  // junc_in_armsSpinInPhase->getEdges()[2].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSpinInPhase()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSpinInPhaseImportanceRef()) );

  junc_in_spinParams = ER::Junction::relocate(resource);
  // junc_in_spinParams->getEdges()[0].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinParams()), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsSpinParamsImportanceRef()) );

  junc_in_ignoreContacts = ER::Junction::relocate(resource);
  // junc_in_ignoreContacts->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getIsHanging()), (float*)&(module->owner->owner->hazardManagement->grab->out->getIsHangingImportanceRef()) );
  // junc_in_ignoreContacts->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmSpin_Con::combineInputs(ER::Module* modulePtr)
{
  ArmSpin* module = (ArmSpin*)modulePtr;
  ArmSpinInputs& in = *module->in;

  // Junction assignments.
  in.m_combinedTargetDirectionImportance = junc_in_combinedTargetDirection->combineDirectInput(&in.combinedTargetDirection);
  in.m_rotationRequestImportance = junc_in_rotationRequest->combinePriority(&in.rotationRequest);
  in.m_spinAmountImportance = junc_in_spinStiffnessScale->combinePriority(&in.spinAmount);
  in.m_armsSpinInPhaseImportance = junc_in_armsSpinInPhase->combineWinnerTakesAll(&in.armsSpinInPhase);
  in.m_spinParamsImportance = junc_in_spinParams->combineDirectInput(&in.spinParams);
  in.m_ignoreContactsImportance = junc_in_ignoreContacts->combineWinnerTakesAll(&in.ignoreContacts);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


