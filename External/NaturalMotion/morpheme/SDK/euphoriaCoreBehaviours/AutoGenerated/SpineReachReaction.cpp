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
#include "SpineReachReaction.h"
#include "SpineReachReactionPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpineReachReaction::SpineReachReaction(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (SpineReachReactionInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpineReachReactionInputs), __alignof(SpineReachReactionInputs));
  out = (SpineReachReactionOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpineReachReactionOutputs), __alignof(SpineReachReactionOutputs));

  m_apiBase = (SpineReachReactionAPIBase*)NMPMemoryAllocAligned(sizeof(SpineReachReactionAPIBase), 16);
  m_updatePackage = (SpineReachReactionUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpineReachReactionUpdatePackage), 16);
  m_feedbackPackage = (SpineReachReactionFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpineReachReactionFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SpineReachReaction::~SpineReachReaction()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineReachReaction::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Spine*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SpineReachReactionAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) SpineReachReactionUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) SpineReachReactionFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SpineReachReaction::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineReachReaction::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineReachReaction::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<SpineReachReactionInputs>();
  *out = savedState.getValue<SpineReachReactionOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SpineReachReaction::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SpineReachReaction::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


