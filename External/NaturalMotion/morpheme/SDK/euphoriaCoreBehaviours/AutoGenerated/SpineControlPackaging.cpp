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
#include "Spine.h"
#include "SpineControl.h"
#include "SpineControlPackaging.h"
#include "SpineSupport.h"
#include "MyNetwork.h"
#include "ReachForWorldBehaviourInterface.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SpineControl_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SpineControl*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SpineControl_Con::~SpineControl_Con()
{
  junc_in_dampingIncrease->destroy();
  junc_in_rootTranslationRequest->destroy();
  junc_in_endTranslationRequest->destroy();
  junc_in_rootRotationRequest->destroy();
  junc_in_endRotationRequest->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SpineControl_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SpineControl_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(3);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SpineControl_Con::buildConnections(SpineControl* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineControl_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_endRotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->support->out->getUpperRotationRequestLocal()), (float*)&(module->owner->support->out->getUpperRotationRequestLocalImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->support->out->getUpperRotationRequestWorld()), (float*)&(module->owner->support->out->getUpperRotationRequestWorldImportanceRef()) );

  junc_in_rootRotationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->support->out->getLowerRotationRequestLocal()), (float*)&(module->owner->support->out->getLowerRotationRequestLocalImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->support->out->getLowerRotationRequestWorld()), (float*)&(module->owner->support->out->getLowerRotationRequestWorldImportanceRef()) );

  junc_in_endTranslationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->support->out->getUpperTranslationRequestLocal()), (float*)&(module->owner->support->out->getUpperTranslationRequestLocalImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->support->out->getUpperTranslationRequestWorld()), (float*)&(module->owner->support->out->getUpperTranslationRequestWorldImportanceRef()) );

  junc_in_rootTranslationRequest = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->support->out->getLowerTranslationRequestLocal()), (float*)&(module->owner->support->out->getLowerTranslationRequestLocalImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->support->out->getLowerTranslationRequestWorld()), (float*)&(module->owner->support->out->getLowerTranslationRequestWorldImportanceRef()) );

  junc_in_dampingIncrease = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getSpineDampingIncrease()), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getSpineDampingIncreaseImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getSpineDampingIncrease()), (float*)&(module->owner->owner->hazardManagement->grab->out->getSpineDampingIncreaseImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSpineDampingIncrease()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSpineDampingIncreaseImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SpineControl_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineControl_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_endRotationRequest = ER::Junction::relocate(resource);
  // junc_in_endRotationRequest->getEdges()[0].reinit( (char*)&(module->owner->support->out->getUpperRotationRequestLocal()), (float*)&(module->owner->support->out->getUpperRotationRequestLocalImportanceRef()) );
  // junc_in_endRotationRequest->getEdges()[1].reinit( (char*)&(module->owner->support->out->getUpperRotationRequestWorld()), (float*)&(module->owner->support->out->getUpperRotationRequestWorldImportanceRef()) );

  junc_in_rootRotationRequest = ER::Junction::relocate(resource);
  // junc_in_rootRotationRequest->getEdges()[0].reinit( (char*)&(module->owner->support->out->getLowerRotationRequestLocal()), (float*)&(module->owner->support->out->getLowerRotationRequestLocalImportanceRef()) );
  // junc_in_rootRotationRequest->getEdges()[1].reinit( (char*)&(module->owner->support->out->getLowerRotationRequestWorld()), (float*)&(module->owner->support->out->getLowerRotationRequestWorldImportanceRef()) );

  junc_in_endTranslationRequest = ER::Junction::relocate(resource);
  // junc_in_endTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->support->out->getUpperTranslationRequestLocal()), (float*)&(module->owner->support->out->getUpperTranslationRequestLocalImportanceRef()) );
  // junc_in_endTranslationRequest->getEdges()[1].reinit( (char*)&(module->owner->support->out->getUpperTranslationRequestWorld()), (float*)&(module->owner->support->out->getUpperTranslationRequestWorldImportanceRef()) );

  junc_in_rootTranslationRequest = ER::Junction::relocate(resource);
  // junc_in_rootTranslationRequest->getEdges()[0].reinit( (char*)&(module->owner->support->out->getLowerTranslationRequestLocal()), (float*)&(module->owner->support->out->getLowerTranslationRequestLocalImportanceRef()) );
  // junc_in_rootTranslationRequest->getEdges()[1].reinit( (char*)&(module->owner->support->out->getLowerTranslationRequestWorld()), (float*)&(module->owner->support->out->getLowerTranslationRequestWorldImportanceRef()) );

  junc_in_dampingIncrease = ER::Junction::relocate(resource);
  // junc_in_dampingIncrease->getEdges()[0].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getSpineDampingIncrease()), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getSpineDampingIncreaseImportanceRef()) );
  // junc_in_dampingIncrease->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getSpineDampingIncrease()), (float*)&(module->owner->owner->hazardManagement->grab->out->getSpineDampingIncreaseImportanceRef()) );
  // junc_in_dampingIncrease->getEdges()[2].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSpineDampingIncrease()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getSpineDampingIncreaseImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SpineControl_Con::combineInputs(ER::Module* modulePtr)
{
  SpineControl* module = (SpineControl*)modulePtr;
  SpineControlInputs& in = *module->in;

  // Junction assignments.
  in.m_endRotationRequestImportance = junc_in_endRotationRequest->combinePriority(&in.endRotationRequest);
  in.m_rootRotationRequestImportance = junc_in_rootRotationRequest->combinePriority(&in.rootRotationRequest);
  in.m_endTranslationRequestImportance = junc_in_endTranslationRequest->combinePriority(&in.endTranslationRequest);
  in.m_rootTranslationRequestImportance = junc_in_rootTranslationRequest->combinePriority(&in.rootTranslationRequest);
  in.m_dampingIncreaseImportance = junc_in_dampingIncrease->combinePriority(&in.dampingIncrease);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


