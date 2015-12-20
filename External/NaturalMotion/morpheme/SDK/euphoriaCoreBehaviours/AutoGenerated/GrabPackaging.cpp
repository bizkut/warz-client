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
#include "Grab.h"
#include "GrabPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "ImpactPredictor.h"
#include "GrabDetection.h"
#include "HoldActionBehaviourInterface.h"
#include "HoldBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void Grab_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((Grab*)module);
}

//----------------------------------------------------------------------------------------------------------------------
Grab_Con::~Grab_Con()
{
  junc_in_ignoreOvershotEdges->destroy();
  junc_in_hasForcedEdge->destroy();
  junc_in_maxTwistBwdPeriod->destroy();
  junc_in_timeRatioBeforeLookingDown->destroy();
  junc_in_aliveParams->destroy();
  junc_in_enableConditions->destroy();
  junc_in_grabActionControl->destroy();
  junc_in_grabAbilityControl->destroy();
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
  junc_in_letGoEdge->destroy();
  junc_in_grabbableEdge->destroy();
  junc_feedIn_braceHazard->destroy();
  junc_feedIn_mostUprightContact->destroy();
  junc_feedIn_centreOfShoulders->destroy();
  junc_feedIn_legsInContact->destroy();
  junc_feedIn_spineInContact->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Grab_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(Grab_Con), 16);

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
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void Grab_Con::buildConnections(Grab* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Grab_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_spineInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContactImportanceRef()) );

  junc_feedIn_legsInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getLegsInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getLegsInContactImportanceRef()) );

  junc_feedIn_centreOfShoulders = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getAverageShoulders()), (float*)&(module->owner->owner->bodyFrame->feedIn->getAverageShouldersImportanceRef()) );

  junc_feedIn_mostUprightContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->feedIn->getClosestContact()), (float*)&(module->owner->owner->lowerBody->feedIn->getClosestContactImportanceRef()) );

  junc_feedIn_braceHazard = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

  junc_in_grabbableEdge = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grabDetection->out->getGrabbableEdge()), (float*)&(module->owner->grabDetection->out->getGrabbableEdgeImportanceRef()) );

  junc_in_letGoEdge = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->grabDetection->out->getLetGoEdge()), (float*)&(module->owner->grabDetection->out->getLetGoEdgeImportanceRef()) );

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

  junc_in_grabAbilityControl = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabControl()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabControlImportanceRef()) );

  junc_in_grabActionControl = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabControl()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabControlImportanceRef()) );

  junc_in_enableConditions = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabEnableConditions()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabEnableConditionsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabEnableConditions()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabEnableConditionsImportanceRef()) );

  junc_in_aliveParams = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );

  junc_in_timeRatioBeforeLookingDown = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getTimeRatioBeforeLookingDown()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getTimeRatioBeforeLookingDownImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getTimeRatioBeforeLookingDown()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getTimeRatioBeforeLookingDownImportanceRef()) );

  junc_in_maxTwistBwdPeriod = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getMaxTwistBwdPeriod()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getMaxTwistBwdPeriodImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getMaxTwistBwdPeriod()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getMaxTwistBwdPeriodImportanceRef()) );

  junc_in_hasForcedEdge = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getHasForcedEdge()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getHasForcedEdgeImportanceRef()) );

  junc_in_ignoreOvershotEdges = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getIgnoreOvershotEdges()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getIgnoreOvershotEdgesImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void Grab_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Grab_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_spineInContact = ER::Junction::relocate(resource);
  // junc_feedIn_spineInContact->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getSpineInContactImportanceRef()) );

  junc_feedIn_legsInContact = ER::Junction::relocate(resource);
  // junc_feedIn_legsInContact->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getLegsInContact()), (float*)&(module->owner->owner->bodyFrame->feedIn->getLegsInContactImportanceRef()) );

  junc_feedIn_centreOfShoulders = ER::Junction::relocate(resource);
  // junc_feedIn_centreOfShoulders->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->feedIn->getAverageShoulders()), (float*)&(module->owner->owner->bodyFrame->feedIn->getAverageShouldersImportanceRef()) );

  junc_feedIn_mostUprightContact = ER::Junction::relocate(resource);
  // junc_feedIn_mostUprightContact->getEdges()[0].reinit( (char*)&(module->owner->owner->lowerBody->feedIn->getClosestContact()), (float*)&(module->owner->owner->lowerBody->feedIn->getClosestContactImportanceRef()) );

  junc_feedIn_braceHazard = ER::Junction::relocate(resource);
  // junc_feedIn_braceHazard->getEdges()[0].reinit( (char*)&(module->owner->chestImpactPredictor->feedOut->getHazard()), (float*)&(module->owner->chestImpactPredictor->feedOut->getHazardImportanceRef()) );

  junc_in_grabbableEdge = ER::Junction::relocate(resource);
  // junc_in_grabbableEdge->getEdges()[0].reinit( (char*)&(module->owner->grabDetection->out->getGrabbableEdge()), (float*)&(module->owner->grabDetection->out->getGrabbableEdgeImportanceRef()) );

  junc_in_letGoEdge = ER::Junction::relocate(resource);
  // junc_in_letGoEdge->getEdges()[0].reinit( (char*)&(module->owner->grabDetection->out->getLetGoEdge()), (float*)&(module->owner->grabDetection->out->getLetGoEdgeImportanceRef()) );

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

  junc_in_grabAbilityControl = ER::Junction::relocate(resource);
  // junc_in_grabAbilityControl->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabControl()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabControlImportanceRef()) );

  junc_in_grabActionControl = ER::Junction::relocate(resource);
  // junc_in_grabActionControl->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabControl()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabControlImportanceRef()) );

  junc_in_enableConditions = ER::Junction::relocate(resource);
  // junc_in_enableConditions->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabEnableConditions()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabEnableConditionsImportanceRef()) );
  // junc_in_enableConditions->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabEnableConditions()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabEnableConditionsImportanceRef()) );

  junc_in_aliveParams = ER::Junction::relocate(resource);
  // junc_in_aliveParams->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );
  // junc_in_aliveParams->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParams()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getGrabAliveParamsImportanceRef()) );

  junc_in_timeRatioBeforeLookingDown = ER::Junction::relocate(resource);
  // junc_in_timeRatioBeforeLookingDown->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getTimeRatioBeforeLookingDown()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getTimeRatioBeforeLookingDownImportanceRef()) );
  // junc_in_timeRatioBeforeLookingDown->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getTimeRatioBeforeLookingDown()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getTimeRatioBeforeLookingDownImportanceRef()) );

  junc_in_maxTwistBwdPeriod = ER::Junction::relocate(resource);
  // junc_in_maxTwistBwdPeriod->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getMaxTwistBwdPeriod()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getMaxTwistBwdPeriodImportanceRef()) );
  // junc_in_maxTwistBwdPeriod->getEdges()[1].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getMaxTwistBwdPeriod()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getMaxTwistBwdPeriodImportanceRef()) );

  junc_in_hasForcedEdge = ER::Junction::relocate(resource);
  // junc_in_hasForcedEdge->getEdges()[0].reinit( (char*)&(module->owner->owner->holdActionBehaviourInterface->out->getHasForcedEdge()), (float*)&(module->owner->owner->holdActionBehaviourInterface->out->getHasForcedEdgeImportanceRef()) );

  junc_in_ignoreOvershotEdges = ER::Junction::relocate(resource);
  // junc_in_ignoreOvershotEdges->getEdges()[0].reinit( (char*)&(module->owner->owner->holdBehaviourInterface->out->getIgnoreOvershotEdges()), (float*)&(module->owner->owner->holdBehaviourInterface->out->getIgnoreOvershotEdgesImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Grab_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  Grab* module = (Grab*)modulePtr;
  GrabFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_spineInContactImportance = junc_feedIn_spineInContact->combineDirectInput(&feedIn.spineInContact);
  feedIn.m_legsInContactImportance = junc_feedIn_legsInContact->combineDirectInput(&feedIn.legsInContact);
  feedIn.m_centreOfShouldersImportance = junc_feedIn_centreOfShoulders->combineDirectInput(&feedIn.centreOfShoulders);
  feedIn.m_mostUprightContactImportance = junc_feedIn_mostUprightContact->combineDirectInput(&feedIn.mostUprightContact);
  feedIn.m_braceHazardImportance = junc_feedIn_braceHazard->combineDirectInput(&feedIn.braceHazard);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Grab_Con::combineInputs(ER::Module* modulePtr)
{
  Grab* module = (Grab*)modulePtr;
  GrabInputs& in = *module->in;

  // Junction assignments.
  in.m_grabbableEdgeImportance = junc_in_grabbableEdge->combineDirectInput(&in.grabbableEdge);
  in.m_letGoEdgeImportance = junc_in_letGoEdge->combineDirectInput(&in.letGoEdge);
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
  in.m_grabAbilityControlImportance = junc_in_grabAbilityControl->combineDirectInput(&in.grabAbilityControl);
  in.m_grabActionControlImportance = junc_in_grabActionControl->combineDirectInput(&in.grabActionControl);
  in.m_enableConditionsImportance = junc_in_enableConditions->combinePriority(&in.enableConditions);
  in.m_grabAliveParamsImportance = junc_in_aliveParams->combinePriority(&in.grabAliveParams);
  in.m_timeRatioBeforeLookingDownImportance = junc_in_timeRatioBeforeLookingDown->combinePriority(&in.timeRatioBeforeLookingDown);
  in.m_maxTwistBwdPeriodImportance = junc_in_maxTwistBwdPeriod->combinePriority(&in.maxTwistBwdPeriod);
  in.m_hasForcedEdgeImportance = junc_in_hasForcedEdge->combineDirectInput(&in.hasForcedEdge);
  in.m_ignoreOvershotEdgesImportance = junc_in_ignoreOvershotEdges->combineDirectInput(&in.ignoreOvershotEdges);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


