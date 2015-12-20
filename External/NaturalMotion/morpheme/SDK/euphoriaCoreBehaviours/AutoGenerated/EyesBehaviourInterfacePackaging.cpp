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
#include "EyesBehaviourInterface.h"
#include "EyesBehaviourInterfacePackaging.h"
#include "Head.h"
#include "HeadEyes.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void EyesBehaviourInterface_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((EyesBehaviourInterface*)module);
}

//----------------------------------------------------------------------------------------------------------------------
EyesBehaviourInterface_Con::~EyesBehaviourInterface_Con()
{
  junc_feedIn_focalRadius->destroy();
  junc_feedIn_focalCentre->destroy();
  junc_feedIn_focalDirection->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EyesBehaviourInterface_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(EyesBehaviourInterface_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void EyesBehaviourInterface_Con::buildConnections(EyesBehaviourInterface* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(EyesBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_focalDirection = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalDirection()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalDirectionImportanceRef()) );

  junc_feedIn_focalCentre = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentre()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentreImportanceRef()) );

  junc_feedIn_focalRadius = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadius()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadiusImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void EyesBehaviourInterface_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(EyesBehaviourInterface_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_focalDirection = ER::Junction::relocate(resource);
  // junc_feedIn_focalDirection->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalDirection()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalDirectionImportanceRef()) );

  junc_feedIn_focalCentre = ER::Junction::relocate(resource);
  // junc_feedIn_focalCentre->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentre()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalCentreImportanceRef()) );

  junc_feedIn_focalRadius = ER::Junction::relocate(resource);
  // junc_feedIn_focalRadius->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadius()), (float*)&(module->owner->heads[0]->eyes->feedOut->getFocalRadiusImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void EyesBehaviourInterface_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  EyesBehaviourInterface* module = (EyesBehaviourInterface*)modulePtr;
  EyesBehaviourInterfaceFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_focalDirectionImportance = junc_feedIn_focalDirection->combineDirectInput(&feedIn.focalDirection);
  feedIn.m_focalCentreImportance = junc_feedIn_focalCentre->combineDirectInput(&feedIn.focalCentre);
  feedIn.m_focalRadiusImportance = junc_feedIn_focalRadius->combineDirectInput(&feedIn.focalRadius);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


