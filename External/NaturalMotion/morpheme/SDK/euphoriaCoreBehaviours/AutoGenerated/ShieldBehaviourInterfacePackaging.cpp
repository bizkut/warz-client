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
#include "ShieldBehaviourInterface.h"
#include "ShieldBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ShieldManagement.h"
#include "Arm.h"
#include "ArmGrab.h"
#include "FreeFallManagement.h"
#include "ShieldActionBehaviourInterface.h"
#include "Grab.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ShieldBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ShieldBehaviourInterface_Con::~ShieldBehaviourInterface_Con()
{
  junc_feedIn_suppress->destroy();
  junc_feedIn_doShieldWithLowerBody->destroy();
  junc_feedIn_doShieldWithUpperBody->destroy();
  junc_feedIn_driveCompensationScale->destroy();
  junc_feedIn_hazardAngle->destroy();
  junc_feedIn_yawAngle->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ShieldBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ShieldBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(5);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface_Con::buildConnections(ShieldBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ShieldBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_yawAngle = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getShieldSpineYawAngle()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getShieldSpineYawAngleImportanceRef()) );

  junc_feedIn_hazardAngle = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngle()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngleImportanceRef()) );

  junc_feedIn_driveCompensationScale = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDriveCompensationScale()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDriveCompensationScaleImportanceRef()) );

  junc_feedIn_doShieldWithUpperBody = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBodyImportanceRef()) );

  junc_feedIn_doShieldWithLowerBody = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBodyImportanceRef()) );

  junc_feedIn_suppress = activeJnc = ER::Junction::init(resource, 5, ER::Junction::isBoolSet);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->grab->feedOut->getIsHighPriority()), (float*)&(module->owner->arms[0]->grab->feedOut->getIsHighPriorityImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->grab->feedOut->getIsHighPriority()), (float*)&(module->owner->arms[1]->grab->feedOut->getIsHighPriorityImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->shieldActionBehaviourInterface->out->getIsRunning()), (float*)&(module->owner->shieldActionBehaviourInterface->out->getIsRunningImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->hazardManagement->grab->out->getEdgeOvershot()), (float*)&(module->owner->hazardManagement->grab->out->getEdgeOvershotImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ShieldBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_yawAngle = ER::Junction::relocate(resource);
  // junc_feedIn_yawAngle->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getShieldSpineYawAngle()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getShieldSpineYawAngleImportanceRef()) );

  junc_feedIn_hazardAngle = ER::Junction::relocate(resource);
  // junc_feedIn_hazardAngle->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngle()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngleImportanceRef()) );

  junc_feedIn_driveCompensationScale = ER::Junction::relocate(resource);
  // junc_feedIn_driveCompensationScale->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDriveCompensationScale()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDriveCompensationScaleImportanceRef()) );

  junc_feedIn_doShieldWithUpperBody = ER::Junction::relocate(resource);
  // junc_feedIn_doShieldWithUpperBody->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBodyImportanceRef()) );

  junc_feedIn_doShieldWithLowerBody = ER::Junction::relocate(resource);
  // junc_feedIn_doShieldWithLowerBody->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBodyImportanceRef()) );

  junc_feedIn_suppress = ER::Junction::relocate(resource);
  // junc_feedIn_suppress->getEdges()[0].reinit( (char*)&(module->owner->arms[0]->grab->feedOut->getIsHighPriority()), (float*)&(module->owner->arms[0]->grab->feedOut->getIsHighPriorityImportanceRef()) );
  // junc_feedIn_suppress->getEdges()[1].reinit( (char*)&(module->owner->arms[1]->grab->feedOut->getIsHighPriority()), (float*)&(module->owner->arms[1]->grab->feedOut->getIsHighPriorityImportanceRef()) );
  // junc_feedIn_suppress->getEdges()[2].reinit( (char*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefall()), (float*)&(module->owner->hazardManagement->freeFallManagement->out->getIsInFreefallImportanceRef()) );
  // junc_feedIn_suppress->getEdges()[3].reinit( (char*)&(module->owner->shieldActionBehaviourInterface->out->getIsRunning()), (float*)&(module->owner->shieldActionBehaviourInterface->out->getIsRunningImportanceRef()) );
  // junc_feedIn_suppress->getEdges()[4].reinit( (char*)&(module->owner->hazardManagement->grab->out->getEdgeOvershot()), (float*)&(module->owner->hazardManagement->grab->out->getEdgeOvershotImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ShieldBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ShieldBehaviourInterface* module = (ShieldBehaviourInterface*)modulePtr;
  ShieldBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_yawAngleImportance = junc_feedIn_yawAngle->combineDirectInput(&feedIn.yawAngle);
  feedIn.m_hazardAngleImportance = junc_feedIn_hazardAngle->combineDirectInput(&feedIn.hazardAngle);
  feedIn.m_driveCompensationScaleImportance = junc_feedIn_driveCompensationScale->combineDirectInput(&feedIn.driveCompensationScale);
  feedIn.m_doShieldWithUpperBodyImportance = junc_feedIn_doShieldWithUpperBody->combineDirectInput(&feedIn.doShieldWithUpperBody);
  feedIn.m_doShieldWithLowerBodyImportance = junc_feedIn_doShieldWithLowerBody->combineDirectInput(&feedIn.doShieldWithLowerBody);
  feedIn.m_suppressImportance = junc_feedIn_suppress->combineIsBoolSet(&feedIn.suppress);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


