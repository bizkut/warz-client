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
#include "HoldActionBehaviourInterface.h"
#include "HoldActionBehaviourInterfacePackaging.h"
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
HoldActionBehaviourInterface::HoldActionBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (HoldActionBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(HoldActionBehaviourInterfaceFeedbackInputs), __alignof(HoldActionBehaviourInterfaceFeedbackInputs));
  userIn = (HoldActionBehaviourInterfaceUserInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::UserInputs, sizeof(HoldActionBehaviourInterfaceUserInputs), __alignof(HoldActionBehaviourInterfaceUserInputs));
  feedOut = (HoldActionBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HoldActionBehaviourInterfaceFeedbackOutputs), __alignof(HoldActionBehaviourInterfaceFeedbackOutputs));
  out = (HoldActionBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HoldActionBehaviourInterfaceOutputs), __alignof(HoldActionBehaviourInterfaceOutputs));

  m_apiBase = (HoldActionBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(HoldActionBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HoldActionBehaviourInterface::~HoldActionBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HoldActionBehaviourInterfaceAPIBase( userIn, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface::clearAllData()
{
  userIn->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldActionBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*userIn);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldActionBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<HoldActionBehaviourInterfaceFeedbackInputs>();
  *userIn = savedState.getValue<HoldActionBehaviourInterfaceUserInputs>();
  *feedOut = savedState.getValue<HoldActionBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<HoldActionBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HoldActionBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


