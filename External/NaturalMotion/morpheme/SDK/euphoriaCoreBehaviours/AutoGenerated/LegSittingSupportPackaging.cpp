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
#include "Leg.h"
#include "LegSittingSupport.h"
#include "LegSittingSupportPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "SitBehaviourInterface.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegSittingSupport*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegSittingSupport_Con::~LegSittingSupport_Con()
{
  junc_in_balancePoseEndRelativeToRoot->destroy();
  junc_in_supportStrengthScale->destroy();
  junc_in_rootDesiredTM->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegSittingSupport_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegSittingSupport_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport_Con::buildConnections(LegSittingSupport* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegSittingSupport_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_rootDesiredTM = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTM()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTMImportanceRef()) );

  junc_in_supportStrengthScale = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getLegSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getLegSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_balancePoseEndRelativeToRoot = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegSittingSupport_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_rootDesiredTM = ER::Junction::relocate(resource);
  // junc_in_rootDesiredTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTM()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTMImportanceRef()) );

  junc_in_supportStrengthScale = ER::Junction::relocate(resource);
  // junc_in_supportStrengthScale->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getLegSupportStrengthScales(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getLegSupportStrengthScalesImportanceRef(module->owner->getChildIndex())) );

  junc_in_balancePoseEndRelativeToRoot = ER::Junction::relocate(resource);
  // junc_in_balancePoseEndRelativeToRoot->getEdges()[0].reinit( (char*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRoot(module->owner->getChildIndex())), (float*)&(module->owner->owner->sitBehaviourInterface->out->getLegPoseEndRelativeToRootImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegSittingSupport_Con::combineInputs(ER::Module* modulePtr)
{
  LegSittingSupport* module = (LegSittingSupport*)modulePtr;
  LegSittingSupportInputs& in = *module->in;

  // Junction assignments.
  in.m_rootDesiredTMImportance = junc_in_rootDesiredTM->combineDirectInput(&in.rootDesiredTM);
  in.m_supportStrengthScaleImportance = junc_in_supportStrengthScale->combineDirectInput(&in.supportStrengthScale);
  in.m_balancePoseEndRelativeToRootImportance = junc_in_balancePoseEndRelativeToRoot->combineDirectInput(&in.balancePoseEndRelativeToRoot);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


