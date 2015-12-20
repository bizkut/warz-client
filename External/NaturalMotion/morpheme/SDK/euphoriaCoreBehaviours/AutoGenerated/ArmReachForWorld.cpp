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
#include "ArmReachForWorld.h"
#include "ArmReachForWorldPackaging.h"
#include "ArmGrab.h"
#include "MyNetwork.h"
#include "ReachForWorldBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmReachForWorld::ArmReachForWorld(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmReachForWorldData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmReachForWorldData), __alignof(ArmReachForWorldData));
  in = (ArmReachForWorldInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmReachForWorldInputs), __alignof(ArmReachForWorldInputs));
  feedOut = (ArmReachForWorldFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmReachForWorldFeedbackOutputs), __alignof(ArmReachForWorldFeedbackOutputs));
  out = (ArmReachForWorldOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmReachForWorldOutputs), __alignof(ArmReachForWorldOutputs));

  m_apiBase = (ArmReachForWorldAPIBase*)NMPMemoryAllocAligned(sizeof(ArmReachForWorldAPIBase), 16);
  m_updatePackage = (ArmReachForWorldUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmReachForWorldUpdatePackage), 16);
  m_feedbackPackage = (ArmReachForWorldFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmReachForWorldFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmReachForWorld::~ArmReachForWorld()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmReachForWorldAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmReachForWorldUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmReachForWorldFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmReachForWorld::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmReachForWorld::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmReachForWorldData>();
  *in = savedState.getValue<ArmReachForWorldInputs>();
  *feedOut = savedState.getValue<ArmReachForWorldFeedbackOutputs>();
  *out = savedState.getValue<ArmReachForWorldOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmReachForWorld::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


