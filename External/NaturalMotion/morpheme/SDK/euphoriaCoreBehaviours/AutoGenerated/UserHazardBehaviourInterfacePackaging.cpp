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
#include "UserHazardBehaviourInterface.h"
#include "UserHazardBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((UserHazardBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
UserHazardBehaviourInterface_Con::~UserHazardBehaviourInterface_Con()
{
  junc_feedIn_hazard->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format UserHazardBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(UserHazardBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface_Con::buildConnections(UserHazardBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(UserHazardBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_hazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->hazardManagement->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(UserHazardBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_hazard = ER::Junction::relocate(resource);
  // junc_feedIn_hazard->getEdges()[0].reinit( (char*)&(module->owner->hazardManagement->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->hazardManagement->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void UserHazardBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  UserHazardBehaviourInterface* module = (UserHazardBehaviourInterface*)modulePtr;
  UserHazardBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_hazardImportance = junc_feedIn_hazard->combineDirectInput(&feedIn.hazard);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


