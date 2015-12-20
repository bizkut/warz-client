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
#include "BalancePoserBehaviourInterface.h"
#include "BalancePoserBehaviourInterfacePackaging.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BalancePoserBehaviourInterface::BalancePoserBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (BalancePoserBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BalancePoserBehaviourInterfaceFeedbackInputs), __alignof(BalancePoserBehaviourInterfaceFeedbackInputs));
  out = (BalancePoserBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BalancePoserBehaviourInterfaceOutputs), __alignof(BalancePoserBehaviourInterfaceOutputs));

  m_apiBase = (BalancePoserBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(BalancePoserBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BalancePoserBehaviourInterface::~BalancePoserBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) BalancePoserBehaviourInterfaceAPIBase( feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface::clearAllData()
{
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BalancePoserBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BalancePoserBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<BalancePoserBehaviourInterfaceFeedbackInputs>();
  *out = savedState.getValue<BalancePoserBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BalancePoserBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


