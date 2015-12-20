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
#include "UserHazardBehaviourInterface.h"
#include "UserHazardBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
UserHazardBehaviourInterface::UserHazardBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (UserHazardBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(UserHazardBehaviourInterfaceFeedbackInputs), __alignof(UserHazardBehaviourInterfaceFeedbackInputs));
  userIn = (UserHazardBehaviourInterfaceUserInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::UserInputs, sizeof(UserHazardBehaviourInterfaceUserInputs), __alignof(UserHazardBehaviourInterfaceUserInputs));
  out = (UserHazardBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(UserHazardBehaviourInterfaceOutputs), __alignof(UserHazardBehaviourInterfaceOutputs));

  m_apiBase = (UserHazardBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(UserHazardBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
UserHazardBehaviourInterface::~UserHazardBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) UserHazardBehaviourInterfaceAPIBase( userIn, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface::clearAllData()
{
  userIn->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool UserHazardBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*userIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool UserHazardBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<UserHazardBehaviourInterfaceFeedbackInputs>();
  *userIn = savedState.getValue<UserHazardBehaviourInterfaceUserInputs>();
  *out = savedState.getValue<UserHazardBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* UserHazardBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


