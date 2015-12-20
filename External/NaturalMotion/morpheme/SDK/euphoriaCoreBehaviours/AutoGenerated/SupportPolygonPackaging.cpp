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
#include "BodyFrame.h"
#include "SupportPolygon.h"
#include "SupportPolygonPackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStandingSupport.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "Spine.h"
#include "SpineSupport.h"
#include "BalanceBehaviourInterface.h"
#include "SitBehaviourInterface.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SupportPolygon*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SupportPolygon_Con::~SupportPolygon_Con()
{
  junc_feedIn_fullySupported->destroy();
  junc_junc_FeedbackInputs_supportPoints_4->destroy();
  junc_junc_FeedbackInputs_supportPoints_3->destroy();
  junc_junc_FeedbackInputs_supportPoints_2->destroy();
  junc_junc_FeedbackInputs_supportPoints_1->destroy();
  junc_junc_FeedbackInputs_supportPoints_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SupportPolygon_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SupportPolygon_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_Con::buildConnections(SupportPolygon* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SupportPolygon_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_junc_FeedbackInputs_supportPoints_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_4 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPoint()), (float*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPointImportanceRef()) );

  junc_feedIn_fullySupported = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SupportPolygon_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_junc_FeedbackInputs_supportPoints_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_4 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_4->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPoint()), (float*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPointImportanceRef()) );

  junc_feedIn_fullySupported = ER::Junction::relocate(resource);
  // junc_feedIn_fullySupported->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SupportPolygon_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  SupportPolygon* module = (SupportPolygon*)modulePtr;
  SupportPolygonFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportPointsImportance[0] = junc_junc_FeedbackInputs_supportPoints_0->combineDirectInput(&feedIn.supportPoints[0]);
  feedIn.m_supportPointsImportance[1] = junc_junc_FeedbackInputs_supportPoints_1->combineDirectInput(&feedIn.supportPoints[1]);
  feedIn.m_supportPointsImportance[2] = junc_junc_FeedbackInputs_supportPoints_2->combineDirectInput(&feedIn.supportPoints[2]);
  feedIn.m_supportPointsImportance[3] = junc_junc_FeedbackInputs_supportPoints_3->combineDirectInput(&feedIn.supportPoints[3]);
  feedIn.m_supportPointsImportance[4] = junc_junc_FeedbackInputs_supportPoints_4->combineDirectInput(&feedIn.supportPoints[4]);
  feedIn.m_fullySupportedImportance = junc_feedIn_fullySupported->combineDirectInput(&feedIn.fullySupported);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_StandingCon::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SupportPolygon*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SupportPolygon_StandingCon::~SupportPolygon_StandingCon()
{
  junc_in_includeLegSupportPoints->destroy();
  junc_in_includeArmSupportPoints->destroy();
  junc_feedIn_fullySupported->destroy();
  junc_junc_FeedbackInputs_supportPoints_4->destroy();
  junc_junc_FeedbackInputs_supportPoints_3->destroy();
  junc_junc_FeedbackInputs_supportPoints_2->destroy();
  junc_junc_FeedbackInputs_supportPoints_1->destroy();
  junc_junc_FeedbackInputs_supportPoints_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SupportPolygon_StandingCon::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SupportPolygon_StandingCon), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_StandingCon::buildConnections(SupportPolygon* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SupportPolygon_StandingCon), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_junc_FeedbackInputs_supportPoints_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_4 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPoint()), (float*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPointImportanceRef()) );

  junc_feedIn_fullySupported = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

  junc_in_includeArmSupportPoints = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_includeLegSupportPoints = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_StandingCon::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SupportPolygon_StandingCon), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_junc_FeedbackInputs_supportPoints_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_4 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_4->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPoint()), (float*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPointImportanceRef()) );

  junc_feedIn_fullySupported = ER::Junction::relocate(resource);
  // junc_feedIn_fullySupported->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

  junc_in_includeArmSupportPoints = ER::Junction::relocate(resource);
  // junc_in_includeArmSupportPoints->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArms()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithArmsImportanceRef()) );

  junc_in_includeLegSupportPoints = ER::Junction::relocate(resource);
  // junc_in_includeLegSupportPoints->getEdges()[0].reinit( (char*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->balanceBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SupportPolygon_StandingCon::combineFeedbackInputs(ER::Module* modulePtr)
{
  SupportPolygon* module = (SupportPolygon*)modulePtr;
  SupportPolygonFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportPointsImportance[0] = junc_junc_FeedbackInputs_supportPoints_0->combineDirectInput(&feedIn.supportPoints[0]);
  feedIn.m_supportPointsImportance[1] = junc_junc_FeedbackInputs_supportPoints_1->combineDirectInput(&feedIn.supportPoints[1]);
  feedIn.m_supportPointsImportance[2] = junc_junc_FeedbackInputs_supportPoints_2->combineDirectInput(&feedIn.supportPoints[2]);
  feedIn.m_supportPointsImportance[3] = junc_junc_FeedbackInputs_supportPoints_3->combineDirectInput(&feedIn.supportPoints[3]);
  feedIn.m_supportPointsImportance[4] = junc_junc_FeedbackInputs_supportPoints_4->combineDirectInput(&feedIn.supportPoints[4]);
  feedIn.m_fullySupportedImportance = junc_feedIn_fullySupported->combineDirectInput(&feedIn.fullySupported);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SupportPolygon_StandingCon::combineInputs(ER::Module* modulePtr)
{
  SupportPolygon* module = (SupportPolygon*)modulePtr;
  SupportPolygonInputs& in = *module->in;

  // Junction assignments.
  in.m_includeArmSupportPointsImportance = junc_in_includeArmSupportPoints->combineDirectInput(&in.includeArmSupportPoints);
  in.m_includeLegSupportPointsImportance = junc_in_includeLegSupportPoints->combineDirectInput(&in.includeLegSupportPoints);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_SittingCon::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SupportPolygon*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SupportPolygon_SittingCon::~SupportPolygon_SittingCon()
{
  junc_in_includeSpineSupportPoints->destroy();
  junc_in_includeLegSupportPoints->destroy();
  junc_feedIn_fullySupported->destroy();
  junc_junc_FeedbackInputs_supportPoints_4->destroy();
  junc_junc_FeedbackInputs_supportPoints_3->destroy();
  junc_junc_FeedbackInputs_supportPoints_2->destroy();
  junc_junc_FeedbackInputs_supportPoints_1->destroy();
  junc_junc_FeedbackInputs_supportPoints_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SupportPolygon_SittingCon::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SupportPolygon_SittingCon), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_SittingCon::buildConnections(SupportPolygon* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SupportPolygon_SittingCon), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_junc_FeedbackInputs_supportPoints_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_2 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_3 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_4 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPoint()), (float*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPointImportanceRef()) );

  junc_feedIn_fullySupported = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

  junc_in_includeLegSupportPoints = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_in_includeSpineSupportPoints = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithSpines()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithSpinesImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon_SittingCon::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SupportPolygon_SittingCon), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_junc_FeedbackInputs_supportPoints_0 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_0->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_1 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_1->getEdges()[0].reinit( (char*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->legs[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_2 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_2->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[0]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_3 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_3->getEdges()[0].reinit( (char*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPoint()), (float*)&(module->owner->owner->arms[1]->standingSupport->feedOut->getEndSupportPointImportanceRef()) );

  junc_junc_FeedbackInputs_supportPoints_4 = ER::Junction::relocate(resource);
  // junc_junc_FeedbackInputs_supportPoints_4->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPoint()), (float*)&(module->owner->owner->spines[0]->support->feedOut->getRootSupportPointImportanceRef()) );

  junc_feedIn_fullySupported = ER::Junction::relocate(resource);
  // junc_feedIn_fullySupported->getEdges()[0].reinit( (char*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmount()), (float*)&(module->owner->owner->spines[0]->feedOut->getExternalSupportAmountImportanceRef()) );

  junc_in_includeLegSupportPoints = ER::Junction::relocate(resource);
  // junc_in_includeLegSupportPoints->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegs()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithLegsImportanceRef()) );

  junc_in_includeSpineSupportPoints = ER::Junction::relocate(resource);
  // junc_in_includeSpineSupportPoints->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithSpines()), (float*)&(module->owner->owner->sitBehaviourInterface->out->getSupportWithSpinesImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SupportPolygon_SittingCon::combineFeedbackInputs(ER::Module* modulePtr)
{
  SupportPolygon* module = (SupportPolygon*)modulePtr;
  SupportPolygonFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_supportPointsImportance[0] = junc_junc_FeedbackInputs_supportPoints_0->combineDirectInput(&feedIn.supportPoints[0]);
  feedIn.m_supportPointsImportance[1] = junc_junc_FeedbackInputs_supportPoints_1->combineDirectInput(&feedIn.supportPoints[1]);
  feedIn.m_supportPointsImportance[2] = junc_junc_FeedbackInputs_supportPoints_2->combineDirectInput(&feedIn.supportPoints[2]);
  feedIn.m_supportPointsImportance[3] = junc_junc_FeedbackInputs_supportPoints_3->combineDirectInput(&feedIn.supportPoints[3]);
  feedIn.m_supportPointsImportance[4] = junc_junc_FeedbackInputs_supportPoints_4->combineDirectInput(&feedIn.supportPoints[4]);
  feedIn.m_fullySupportedImportance = junc_feedIn_fullySupported->combineDirectInput(&feedIn.fullySupported);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SupportPolygon_SittingCon::combineInputs(ER::Module* modulePtr)
{
  SupportPolygon* module = (SupportPolygon*)modulePtr;
  SupportPolygonInputs& in = *module->in;

  // Junction assignments.
  in.m_includeLegSupportPointsImportance = junc_in_includeLegSupportPoints->combineDirectInput(&in.includeLegSupportPoints);
  in.m_includeSpineSupportPointsImportance = junc_in_includeSpineSupportPoints->combineDirectInput(&in.includeSpineSupportPoints);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


