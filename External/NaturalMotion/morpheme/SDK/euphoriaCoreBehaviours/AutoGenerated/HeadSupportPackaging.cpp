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
#include "Head.h"
#include "HeadSupport.h"
#include "HeadSupportPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "BalanceManagement.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadSupport_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HeadSupport*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HeadSupport_Con::~HeadSupport_Con()
{
  junc_head->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeadSupport_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeadSupport_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HeadSupport_Con::buildConnections(HeadSupport* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadSupport_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_head = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getHeadRelChestOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getHeadRelChestOrientationImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceManagement->out->getHeadRelChestOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->out->getHeadRelChestOrientationImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void HeadSupport_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadSupport_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_head = ER::Junction::relocate(resource);
  // junc_head->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getHeadRelChestOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getHeadRelChestOrientationImportanceRef(module->owner->getChildIndex())) );
  // junc_head->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceManagement->out->getHeadRelChestOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->out->getHeadRelChestOrientationImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HeadSupport_Con::combineInputs(ER::Module* modulePtr)
{
  HeadSupport* module = (HeadSupport*)modulePtr;
  HeadSupportInputs& in = *module->in;

  // Junction assignments.
  in.m_desiredHeadRelChestOrientationImportance = junc_head->combinePriority(&in.desiredHeadRelChestOrientation);
  PostCombiners::QuatNormalise(in.desiredHeadRelChestOrientation);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


