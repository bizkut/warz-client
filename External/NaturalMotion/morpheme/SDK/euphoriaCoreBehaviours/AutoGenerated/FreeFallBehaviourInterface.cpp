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
#include "FreeFallBehaviourInterface.h"
#include "FreeFallBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "FreeFallManagement.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
FreeFallBehaviourInterface::FreeFallBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (FreeFallBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(FreeFallBehaviourInterfaceFeedbackInputs), __alignof(FreeFallBehaviourInterfaceFeedbackInputs));
  feedOut = (FreeFallBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(FreeFallBehaviourInterfaceFeedbackOutputs), __alignof(FreeFallBehaviourInterfaceFeedbackOutputs));
  out = (FreeFallBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(FreeFallBehaviourInterfaceOutputs), __alignof(FreeFallBehaviourInterfaceOutputs));

  m_apiBase = (FreeFallBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(FreeFallBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
FreeFallBehaviourInterface::~FreeFallBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) FreeFallBehaviourInterfaceAPIBase( feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface::clearAllData()
{
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool FreeFallBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FreeFallBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<FreeFallBehaviourInterfaceFeedbackInputs>();
  *feedOut = savedState.getValue<FreeFallBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<FreeFallBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* FreeFallBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


