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
#include "ReachForWorldBehaviourInterface.h"
#include "ReachForWorldBehaviourInterfacePackaging.h"
#include "Arm.h"
#include "ArmReachForWorld.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ReachForWorldBehaviourInterface::ReachForWorldBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (ReachForWorldBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ReachForWorldBehaviourInterfaceFeedbackInputs), __alignof(ReachForWorldBehaviourInterfaceFeedbackInputs));
  feedOut = (ReachForWorldBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ReachForWorldBehaviourInterfaceFeedbackOutputs), __alignof(ReachForWorldBehaviourInterfaceFeedbackOutputs));
  out = (ReachForWorldBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ReachForWorldBehaviourInterfaceOutputs), __alignof(ReachForWorldBehaviourInterfaceOutputs));

  m_apiBase = (ReachForWorldBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(ReachForWorldBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ReachForWorldBehaviourInterface::~ReachForWorldBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ReachForWorldBehaviourInterfaceAPIBase( feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface::clearAllData()
{
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForWorldBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForWorldBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<ReachForWorldBehaviourInterfaceFeedbackInputs>();
  *feedOut = savedState.getValue<ReachForWorldBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<ReachForWorldBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ReachForWorldBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


