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
#include "HoldBehaviourInterface.h"
#include "HoldBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HoldBehaviourInterface::HoldBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (HoldBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(HoldBehaviourInterfaceFeedbackInputs), __alignof(HoldBehaviourInterfaceFeedbackInputs));
  userIn = (HoldBehaviourInterfaceUserInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::UserInputs, sizeof(HoldBehaviourInterfaceUserInputs), __alignof(HoldBehaviourInterfaceUserInputs));
  feedOut = (HoldBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HoldBehaviourInterfaceFeedbackOutputs), __alignof(HoldBehaviourInterfaceFeedbackOutputs));
  out = (HoldBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HoldBehaviourInterfaceOutputs), __alignof(HoldBehaviourInterfaceOutputs));

  m_apiBase = (HoldBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(HoldBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HoldBehaviourInterface::~HoldBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HoldBehaviourInterfaceAPIBase( userIn, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourInterface::clearAllData()
{
  userIn->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*userIn);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<HoldBehaviourInterfaceFeedbackInputs>();
  *userIn = savedState.getValue<HoldBehaviourInterfaceUserInputs>();
  *feedOut = savedState.getValue<HoldBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<HoldBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HoldBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


