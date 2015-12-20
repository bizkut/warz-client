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
#include "BalanceBehaviourInterface.h"
#include "BalanceBehaviourInterfacePackaging.h"
#include "CharacteristicsBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "Leg.h"
#include "LegStep.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BalanceBehaviourInterface::BalanceBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (BalanceBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(BalanceBehaviourInterfaceData), __alignof(BalanceBehaviourInterfaceData));
  feedIn = (BalanceBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BalanceBehaviourInterfaceFeedbackInputs), __alignof(BalanceBehaviourInterfaceFeedbackInputs));
  in = (BalanceBehaviourInterfaceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BalanceBehaviourInterfaceInputs), __alignof(BalanceBehaviourInterfaceInputs));
  feedOut = (BalanceBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(BalanceBehaviourInterfaceFeedbackOutputs), __alignof(BalanceBehaviourInterfaceFeedbackOutputs));
  out = (BalanceBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BalanceBehaviourInterfaceOutputs), __alignof(BalanceBehaviourInterfaceOutputs));

  m_apiBase = (BalanceBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(BalanceBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BalanceBehaviourInterface::~BalanceBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) BalanceBehaviourInterfaceAPIBase( data, in, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<BalanceBehaviourInterfaceData>();
  *feedIn = savedState.getValue<BalanceBehaviourInterfaceFeedbackInputs>();
  *in = savedState.getValue<BalanceBehaviourInterfaceInputs>();
  *feedOut = savedState.getValue<BalanceBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<BalanceBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BalanceBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


