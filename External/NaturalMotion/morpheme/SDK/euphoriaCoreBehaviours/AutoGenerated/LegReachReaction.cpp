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
#include "LegReachReaction.h"
#include "LegReachReactionPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegReachReaction::LegReachReaction(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (LegReachReactionInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegReachReactionInputs), __alignof(LegReachReactionInputs));
  out = (LegReachReactionOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegReachReactionOutputs), __alignof(LegReachReactionOutputs));

  m_apiBase = (LegReachReactionAPIBase*)NMPMemoryAllocAligned(sizeof(LegReachReactionAPIBase), 16);
  m_updatePackage = (LegReachReactionUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegReachReactionUpdatePackage), 16);
  m_feedbackPackage = (LegReachReactionFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegReachReactionFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegReachReaction::~LegReachReaction()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegReachReactionAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) LegReachReactionUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) LegReachReactionFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegReachReaction::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegReachReaction::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<LegReachReactionInputs>();
  *out = savedState.getValue<LegReachReactionOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegReachReaction::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction::update(float timeStep)
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


