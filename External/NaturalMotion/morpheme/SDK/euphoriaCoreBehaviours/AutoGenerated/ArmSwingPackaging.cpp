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
#include "Arm.h"
#include "ArmSwing.h"
#include "ArmSwingPackaging.h"
#include "MyNetwork.h"
#include "ProtectBehaviourInterface.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "BodySection.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmSwing_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((ArmSwing*)module);
}

//----------------------------------------------------------------------------------------------------------------------
ArmSwing_Con::~ArmSwing_Con()
{
  junc_pri02->destroy();
  junc_in_numConstrainedArms->destroy();
  junc_pri01->destroy();
  junc_sss->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ArmSwing_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(ArmSwing_Con), 16);

  result += ER::Junction::getMemoryRequirements(3);
  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void ArmSwing_Con::buildConnections(ArmSwing* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmSwing_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_sss = activeJnc = ER::Junction::init(resource, 3, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getArmsSwingWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getArmsSwingWeightImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getArmSwingStiffnessScale()), (float*)&(module->owner->owner->hazardManagement->grab->out->getArmSwingStiffnessScaleImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSwingStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSwingStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_pri01 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getRotationRequest()), (float*)&(module->owner->owner->upperBody->out->getRotationRequestImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_numConstrainedArms = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArms()), (float*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArmsImportanceRef()) );

  junc_pri02 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsOutwardsOnly()), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsOutwardsOnlyImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSwingOutwardsOnly()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSwingOutwardsOnlyImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void ArmSwing_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(ArmSwing_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_sss = ER::Junction::relocate(resource);
  // junc_sss->getEdges()[0].reinit( (char*)&(module->owner->owner->protectBehaviourInterface->out->getArmsSwingWeight()), (float*)&(module->owner->owner->protectBehaviourInterface->out->getArmsSwingWeightImportanceRef()) );
  // junc_sss->getEdges()[1].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getArmSwingStiffnessScale()), (float*)&(module->owner->owner->hazardManagement->grab->out->getArmSwingStiffnessScaleImportanceRef()) );
  // junc_sss->getEdges()[2].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSwingStiffnessScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getSwingStiffnessScalesImportanceRef(module->owner->getChildIndex())) );

  junc_pri01 = ER::Junction::relocate(resource);
  // junc_pri01->getEdges()[0].reinit( (char*)&(module->owner->owner->upperBody->out->getRotationRequest()), (float*)&(module->owner->owner->upperBody->out->getRotationRequestImportanceRef()) );
  // junc_pri01->getEdges()[1].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequests(module->owner->getChildIndex())), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getTargetRequestsImportanceRef(module->owner->getChildIndex())) );

  junc_in_numConstrainedArms = ER::Junction::relocate(resource);
  // junc_in_numConstrainedArms->getEdges()[0].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArms()), (float*)&(module->owner->owner->hazardManagement->grab->out->getNumConstrainedArmsImportanceRef()) );

  junc_pri02 = ER::Junction::relocate(resource);
  // junc_pri02->getEdges()[0].reinit( (char*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsOutwardsOnly()), (float*)&(module->owner->owner->armsWindmillBehaviourInterface->out->getArmsOutwardsOnlyImportanceRef()) );
  // junc_pri02->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSwingOutwardsOnly()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getArmsSwingOutwardsOnlyImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void ArmSwing_Con::combineInputs(ER::Module* modulePtr)
{
  ArmSwing* module = (ArmSwing*)modulePtr;
  ArmSwingInputs& in = *module->in;

  // Junction assignments.
  in.m_swingStiffnessScaleImportance = junc_sss->combinePriority(&in.swingStiffnessScale);
  in.m_rotationRequestImportance = junc_pri01->combinePriority(&in.rotationRequest);
  in.m_numConstrainedArmsImportance = junc_in_numConstrainedArms->combineDirectInput(&in.numConstrainedArms);
  in.m_armsOutwardsOnlyImportance = junc_pri02->combineWinnerTakesAll(&in.armsOutwardsOnly);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


