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
#include "LegSpin.h"
#include "LegSpinPackaging.h"
#include "MyNetwork.h"
#include "BodySection.h"
#include "LegsPedalBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "FreeFallManagement.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegSpin_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegSpin*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegSpin_Con::~LegSpin_Con()
{
  junc_in_ignoreContacts->destroy();
  junc_in_spinParams->destroy();
  junc_in_spinStiffnessScale->destroy();
  junc_in_rotationRequest->destroy();
  junc_in_combinedTargetDirection->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegSpin_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegSpin_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegSpin_Con::buildConnections(LegSpin* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegSpin_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_combinedTargetDirection = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->feedIn->getSpinDirection()), (float*)&(module->owner->owner->lowerBody->feedIn->getSpinDirectionImportanceRef()) );

  junc_in_rotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->out->getRotationRequest()), (float*)&(module->owner->owner->lowerBody->out->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_spinStiffnessScale = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getLegsSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getLegsSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_spinParams = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getLegsSpinParams()), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getLegsSpinParamsImportanceRef()) );

  junc_in_ignoreContacts = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getIsHanging()), (float*)&(module->owner->owner->hazardManagement->grab->out->getIsHangingImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegSpin_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegSpin_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_combinedTargetDirection = ER::Junction::relocate(resource);
  // junc_in_combinedTargetDirection->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->feedIn->getSpinDirection()), (float*)&(module->owner->owner->lowerBody->feedIn->getSpinDirectionImportanceRef()) );

  junc_in_rotationRequest = ER::Junction::relocate(resource);
  // junc_in_rotationRequest->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->out->getRotationRequest()), (float*)&(module->owner->owner->lowerBody->out->getRotationRequestImportanceRef()) );
  // junc_in_rotationRequest->getEdges()[1].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_spinStiffnessScale = ER::Junction::relocate(resource);
  // junc_in_spinStiffnessScale->getEdges()[0].reinit( (char*)&(module->owner->owner->freeFallBehaviourInterface->out->getLegsSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->freeFallBehaviourInterface->out->getLegsSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );
  // junc_in_spinStiffnessScale->getEdges()[1].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSpinStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getSpinStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_spinParams = ER::Junction::relocate(resource);
  // junc_in_spinParams->getEdges()[0].reinit( (char*)&(module->owner->owner->legsPedalBehaviourInterface->out->getLegsSpinParams()), (float*)&(module->owner->owner->legsPedalBehaviourInterface->out->getLegsSpinParamsImportanceRef()) );

  junc_in_ignoreContacts = ER::Junction::relocate(resource);
  // junc_in_ignoreContacts->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getIsHanging()), (float*)&(module->owner->owner->hazardManagement->grab->out->getIsHangingImportanceRef()) );
  // junc_in_ignoreContacts->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegSpin_Con::combineInputs(ER::Module* modulePtr)
{
  LegSpin* module = (LegSpin*)modulePtr;
  LegSpinInputs& in = *module->in;

  // Junction assignments.
  in.m_combinedTargetDirectionImportance = junc_in_combinedTargetDirection->combineDirectInput(&in.combinedTargetDirection);
  in.m_rotationRequestImportance = junc_in_rotationRequest->combinePriority(&in.rotationRequest);
  in.m_spinAmountImportance = junc_in_spinStiffnessScale->combinePriority(&in.spinAmount);
  in.m_spinParamsImportance = junc_in_spinParams->combineDirectInput(&in.spinParams);
  in.m_ignoreContactsImportance = junc_in_ignoreContacts->combineWinnerTakesAll(&in.ignoreContacts);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


