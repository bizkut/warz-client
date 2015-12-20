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
#include "ArmReachReaction.h"
#include "ArmReachReactionPackaging.h"
#include "MyNetwork.h"
#include "ArmReachForBodyPart.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "ReachForBodyBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmReachReaction::ArmReachReaction(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (ArmReachReactionInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmReachReactionInputs), __alignof(ArmReachReactionInputs));
  out = (ArmReachReactionOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmReachReactionOutputs), __alignof(ArmReachReactionOutputs));

  m_apiBase = (ArmReachReactionAPIBase*)NMPMemoryAllocAligned(sizeof(ArmReachReactionAPIBase), 16);
  m_updatePackage = (ArmReachReactionUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmReachReactionUpdatePackage), 16);
  m_feedbackPackage = (ArmReachReactionFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmReachReactionFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmReachReaction::~ArmReachReaction()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachReaction::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmReachReactionAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) ArmReachReactionUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmReachReactionFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachReaction::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmReachReaction::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmReachReaction::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<ArmReachReactionInputs>();
  *out = savedState.getValue<ArmReachReactionOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmReachReaction::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachReaction::update(float timeStep)
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


