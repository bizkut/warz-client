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
#include "ShieldActionBehaviourInterface.h"
#include "ShieldActionBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ShieldManagement.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ShieldActionBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ShieldActionBehaviourInterface_Con::~ShieldActionBehaviourInterface_Con()
{
  junc_feedIn_doShieldWithLowerBody->destroy();
  junc_feedIn_doShieldWithUpperBody->destroy();
  junc_feedIn_hazardAngle->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ShieldActionBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ShieldActionBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface_Con::buildConnections(ShieldActionBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ShieldActionBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_hazardAngle = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngle()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngleImportanceRef()) );

  junc_feedIn_doShieldWithUpperBody = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBodyImportanceRef()) );

  junc_feedIn_doShieldWithLowerBody = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBodyImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ShieldActionBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_hazardAngle = ER::Junction::relocate(resource);
  // junc_feedIn_hazardAngle->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngle()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getHazardAngleImportanceRef()) );

  junc_feedIn_doShieldWithUpperBody = ER::Junction::relocate(resource);
  // junc_feedIn_doShieldWithUpperBody->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithUpperBodyImportanceRef()) );

  junc_feedIn_doShieldWithLowerBody = ER::Junction::relocate(resource);
  // junc_feedIn_doShieldWithLowerBody->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBody()), (float*)&(module->owner->hazardManagement->shieldManagement->feedOut->getDoShieldWithLowerBodyImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ShieldActionBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  ShieldActionBehaviourInterface* module = (ShieldActionBehaviourInterface*)modulePtr;
  ShieldActionBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_hazardAngleImportance = junc_feedIn_hazardAngle->combineDirectInput(&feedIn.hazardAngle);
  feedIn.m_doShieldWithUpperBodyImportance = junc_feedIn_doShieldWithUpperBody->combineDirectInput(&feedIn.doShieldWithUpperBody);
  feedIn.m_doShieldWithLowerBodyImportance = junc_feedIn_doShieldWithLowerBody->combineDirectInput(&feedIn.doShieldWithLowerBody);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


