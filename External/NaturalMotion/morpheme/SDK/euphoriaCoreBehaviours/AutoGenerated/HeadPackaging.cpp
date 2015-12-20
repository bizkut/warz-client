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
#include "Head.h"
#include "HeadPackaging.h"
#include "HeadPoint.h"
#include "HeadDodge.h"
#include "HeadAvoid.h"
#include "HeadPose.h"
#include "HeadSupport.h"
#include "HeadAim.h"
#include "CharacteristicsBehaviourInterface.h"
#include "BodySection.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void Head_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((Head*)module);
}

//----------------------------------------------------------------------------------------------------------------------
Head_Con::~Head_Con()
{
  junc_in_upperBodyInContact->destroy();
  junc_in_chestControlledAmount->destroy();
  junc_in_limbDampingRatioReduction->destroy();
  junc_in_limbControlCompensationReduction->destroy();
  junc_in_limbStiffnessReduction->destroy();
  junc_pri01->destroy();
  junc_avg01->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Head_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(Head_Con), 16);

  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(5);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void Head_Con::buildConnections(Head* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Head_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_avg01 = activeJnc = ER::Junction::init(resource, 3, ER::Junction::average);
  activeJnc->getEdges()[0].reinit( (char*)&(module->point->out->getControl()), (float*)&(module->point->out->getControlImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->dodge->out->getControl()), (float*)&(module->dodge->out->getControlImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->avoid->out->getControl()), (float*)&(module->avoid->out->getControlImportanceRef()) );

  junc_pri01 = activeJnc = ER::Junction::init(resource, 5, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->support->out->getControl()), (float*)&(module->support->out->getControlImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->aim->out->getControl()), (float*)&(module->aim->out->getControlImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->out->getAverage()), (float*)&(module->out->getAverageImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_in_limbStiffnessReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getHeadStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getHeadStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getHeadControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getHeadControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getHeadDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getHeadDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_chestControlledAmount = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->upperBody->feedOut->getControlledAmount()), (float*)&(module->owner->upperBody->feedOut->getControlledAmountImportanceRef()) );

  junc_in_upperBodyInContact = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->upperBody->feedIn->getInContact()), (float*)&(module->owner->upperBody->feedIn->getInContactImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void Head_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(Head_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_avg01 = ER::Junction::relocate(resource);
  // junc_avg01->getEdges()[0].reinit( (char*)&(module->point->out->getControl()), (float*)&(module->point->out->getControlImportanceRef()) );
  // junc_avg01->getEdges()[1].reinit( (char*)&(module->dodge->out->getControl()), (float*)&(module->dodge->out->getControlImportanceRef()) );
  // junc_avg01->getEdges()[2].reinit( (char*)&(module->avoid->out->getControl()), (float*)&(module->avoid->out->getControlImportanceRef()) );

  junc_pri01 = ER::Junction::relocate(resource);
  // junc_pri01->getEdges()[0].reinit( (char*)&(module->pose->out->getControlLowImp()), (float*)&(module->pose->out->getControlLowImpImportanceRef()) );
  // junc_pri01->getEdges()[1].reinit( (char*)&(module->support->out->getControl()), (float*)&(module->support->out->getControlImportanceRef()) );
  // junc_pri01->getEdges()[2].reinit( (char*)&(module->aim->out->getControl()), (float*)&(module->aim->out->getControlImportanceRef()) );
  // junc_pri01->getEdges()[3].reinit( (char*)&(module->out->getAverage()), (float*)&(module->out->getAverageImportanceRef()) );
  // junc_pri01->getEdges()[4].reinit( (char*)&(module->pose->out->getControlHighImp()), (float*)&(module->pose->out->getControlHighImpImportanceRef()) );

  junc_in_limbStiffnessReduction = ER::Junction::relocate(resource);
  // junc_in_limbStiffnessReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getHeadStrengthReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getHeadStrengthReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbControlCompensationReduction = ER::Junction::relocate(resource);
  // junc_in_limbControlCompensationReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getHeadControlCompensationReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getHeadControlCompensationReductionImportanceRef(module->getChildIndex())) );

  junc_in_limbDampingRatioReduction = ER::Junction::relocate(resource);
  // junc_in_limbDampingRatioReduction->getEdges()[0].reinit( (char*)&(module->owner->characteristicsBehaviourInterface->out->getHeadDampingRatioReduction(module->getChildIndex())), (float*)&(module->owner->characteristicsBehaviourInterface->out->getHeadDampingRatioReductionImportanceRef(module->getChildIndex())) );

  junc_in_chestControlledAmount = ER::Junction::relocate(resource);
  // junc_in_chestControlledAmount->getEdges()[0].reinit( (char*)&(module->owner->upperBody->feedOut->getControlledAmount()), (float*)&(module->owner->upperBody->feedOut->getControlledAmountImportanceRef()) );

  junc_in_upperBodyInContact = ER::Junction::relocate(resource);
  // junc_in_upperBodyInContact->getEdges()[0].reinit( (char*)&(module->owner->upperBody->feedIn->getInContact()), (float*)&(module->owner->upperBody->feedIn->getInContactImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Head_Con::combineInputs(ER::Module* modulePtr)
{
  Head* module = (Head*)modulePtr;
  HeadInputs& in = *module->in;

  // Junction assignments.
  in.m_limbStiffnessReductionImportance = junc_in_limbStiffnessReduction->combineDirectInput(&in.limbStiffnessReduction);
  in.m_limbControlCompensationReductionImportance = junc_in_limbControlCompensationReduction->combineDirectInput(&in.limbControlCompensationReduction);
  in.m_limbDampingRatioReductionImportance = junc_in_limbDampingRatioReduction->combineDirectInput(&in.limbDampingRatioReduction);
  in.m_chestControlledAmountImportance = junc_in_chestControlledAmount->combineDirectInput(&in.chestControlledAmount);
  in.m_upperBodyInContactImportance = junc_in_upperBodyInContact->combineDirectInput(&in.upperBodyInContact);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void Head_Con::combineOutputs(ER::Module* modulePtr)
{
  Head* module = (Head*)modulePtr;
  HeadOutputs& out = *module->out;

  // Junction assignments.
  NMP_ASSERT(out.m_averageImportance == 0.0f);
  out.m_averageImportance = junc_avg01->combineAverage(&out.average);
  NMP_ASSERT(out.m_controlImportance == 0.0f);
  out.m_controlImportance = junc_pri01->combinePriority(&out.control);

#if NM_CALL_VALIDATORS
  out.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


