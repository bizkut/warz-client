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
#include "HazardManagement.h"
#include "GrabDetection.h"
#include "GrabDetectionPackaging.h"
#include "MyNetwork.h"
#include "EnvironmentAwareness.h"
#include "Grab.h"
#include "BodyFrame.h"
#include "HoldActionBehaviourInterface.h"
#include "HoldBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void GrabDetection_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((GrabDetection*)module);
}

//----------------------------------------------------------------------------------------------------------------------
GrabDetection_Con::~GrabDetection_Con()
{
  junc_in_grabFiltering->destroy();
  junc_in_supportStrengthScale->destroy();
  junc_in_gameEdges_9->destroy();
  junc_in_gameEdges_8->destroy();
  junc_in_gameEdges_7->destroy();
  junc_in_gameEdges_6->destroy();
  junc_in_gameEdges_5->destroy();
  junc_in_gameEdges_4->destroy();
  junc_in_gameEdges_3->destroy();
  junc_in_gameEdges_2->destroy();
  junc_in_gameEdges_1->destroy();
  junc_in_gameEdges_0->destroy();
  junc_feedIn_centreOfMassBodyState->destroy();
  junc_feedIn_isHanging->destroy();
  junc_feedIn_useEnvironmentAwareness->destroy();
  junc_feedIn_potentialGrabPatches->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format GrabDetection_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(GrabDetection_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void GrabDetection_Con::buildConnections(GrabDetection* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(GrabDetection_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_potentialGrabPatches = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictor(0)), (float*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictorImportanceRef(0)) );

  junc_feedIn_useEnvironmentAwareness = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grab->feedOut->getUseEnvironmentAwareness()), (float*)&(module->owner->grab->feedOut->getUseEnvironmentAwarenessImportanceRef()) );

  junc_feedIn_isHanging = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getIsHanging()), (float*)&(module->owner->grab->out->getIsHangingImportanceRef()) );

  junc_feedIn_centreOfMassBodyState = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyStateImportanceRef()) );

  junc_in_gameEdges_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(0)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(0)) );
  junc_in_gameEdges_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(1)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(1)) );
  junc_in_gameEdges_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(2)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(2)) );
  junc_in_gameEdges_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(3)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(3)) );
  junc_in_gameEdges_4 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(4)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(4)) );
  junc_in_gameEdges_5 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(5)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(5)) );
  junc_in_gameEdges_6 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(6)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(6)) );
  junc_in_gameEdges_7 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(7)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(7)) );
  junc_in_gameEdges_8 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(8)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(8)) );
  junc_in_gameEdges_9 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(9)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(9)) );

  junc_in_supportStrengthScale = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabDetectionParams()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabDetectionParamsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabDetectionParams()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabDetectionParamsImportanceRef()) );

  junc_in_grabFiltering = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFiltering()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFilteringImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void GrabDetection_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(GrabDetection_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_potentialGrabPatches = ER::Junction::relocate(resource);
  // junc_feedIn_potentialGrabPatches->getEdges()[0].reinit( (char*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictor(0)), (float*)&(module->owner->owner->environmentAwareness->feedOut->getPotentialHazardsForImpactPredictorImportanceRef(0)) );

  junc_feedIn_useEnvironmentAwareness = ER::Junction::relocate(resource);
  // junc_feedIn_useEnvironmentAwareness->getEdges()[0].reinit( (char*)&(module->owner->grab->feedOut->getUseEnvironmentAwareness()), (float*)&(module->owner->grab->feedOut->getUseEnvironmentAwarenessImportanceRef()) );

  junc_feedIn_isHanging = ER::Junction::relocate(resource);
  // junc_feedIn_isHanging->getEdges()[0].reinit( (char*)&(module->owner->grab->out->getIsHanging()), (float*)&(module->owner->grab->out->getIsHangingImportanceRef()) );

  junc_feedIn_centreOfMassBodyState = ER::Junction::relocate(resource);
  // junc_feedIn_centreOfMassBodyState->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyState()), (float*)&(module->owner->owner->bodyFrame->feedOut->getCentreOfMassBodyStateImportanceRef()) );

  junc_in_gameEdges_0 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_0->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(0)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(0)) );
  junc_in_gameEdges_1 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_1->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(1)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(1)) );
  junc_in_gameEdges_2 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_2->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(2)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(2)) );
  junc_in_gameEdges_3 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_3->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(3)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(3)) );
  junc_in_gameEdges_4 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_4->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(4)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(4)) );
  junc_in_gameEdges_5 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_5->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(5)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(5)) );
  junc_in_gameEdges_6 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_6->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(6)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(6)) );
  junc_in_gameEdges_7 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_7->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(7)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(7)) );
  junc_in_gameEdges_8 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_8->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(8)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(8)) );
  junc_in_gameEdges_9 = ER::Junction::relocate(resource);
  // junc_in_gameEdges_9->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdges(9)), (float*)&(module->owner->owner->holdActionBehaviourInterface->userIn->getGameEdgesImportanceRef(9)) );

  junc_in_supportStrengthScale = ER::Junction::relocate(resource);
  // junc_in_supportStrengthScale->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabDetectionParams()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabDetectionParamsImportanceRef()) );
  // junc_in_supportStrengthScale->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabDetectionParams()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabDetectionParamsImportanceRef()) );

  junc_in_grabFiltering = ER::Junction::relocate(resource);
  // junc_in_grabFiltering->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFiltering()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabFilteringImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void GrabDetection_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  GrabDetection* module = (GrabDetection*)modulePtr;
  GrabDetectionFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_potentialGrabPatchesImportance = junc_feedIn_potentialGrabPatches->combineDirectInput(&feedIn.potentialGrabPatches);
  feedIn.m_useEnvironmentAwarenessImportance = junc_feedIn_useEnvironmentAwareness->combineDirectInput(&feedIn.useEnvironmentAwareness);
  feedIn.m_isHangingImportance = junc_feedIn_isHanging->combineDirectInput(&feedIn.isHanging);
  feedIn.m_centreOfMassBodyStateImportance = junc_feedIn_centreOfMassBodyState->combineDirectInput(&feedIn.centreOfMassBodyState);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void GrabDetection_Con::combineInputs(ER::Module* modulePtr)
{
  GrabDetection* module = (GrabDetection*)modulePtr;
  GrabDetectionInputs& in = *module->in;

  // Junction assignments.
  in.m_gameEdgesImportance[0] = junc_in_gameEdges_0->combineDirectInput(&in.gameEdges[0]);
  in.m_gameEdgesImportance[1] = junc_in_gameEdges_1->combineDirectInput(&in.gameEdges[1]);
  in.m_gameEdgesImportance[2] = junc_in_gameEdges_2->combineDirectInput(&in.gameEdges[2]);
  in.m_gameEdgesImportance[3] = junc_in_gameEdges_3->combineDirectInput(&in.gameEdges[3]);
  in.m_gameEdgesImportance[4] = junc_in_gameEdges_4->combineDirectInput(&in.gameEdges[4]);
  in.m_gameEdgesImportance[5] = junc_in_gameEdges_5->combineDirectInput(&in.gameEdges[5]);
  in.m_gameEdgesImportance[6] = junc_in_gameEdges_6->combineDirectInput(&in.gameEdges[6]);
  in.m_gameEdgesImportance[7] = junc_in_gameEdges_7->combineDirectInput(&in.gameEdges[7]);
  in.m_gameEdgesImportance[8] = junc_in_gameEdges_8->combineDirectInput(&in.gameEdges[8]);
  in.m_gameEdgesImportance[9] = junc_in_gameEdges_9->combineDirectInput(&in.gameEdges[9]);
  in.m_paramsImportance = junc_in_supportStrengthScale->combinePriority(&in.params);
  in.m_grabFilteringImportance = junc_in_grabFiltering->combineDirectInput(&in.grabFiltering);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


