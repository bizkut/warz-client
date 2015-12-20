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
#include "BodyFrame.h"
#include "BalanceAssistant.h"
#include "BalanceAssistantPackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStep.h"
#include "BodyBalance.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BalanceAssistant::BalanceAssistant(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (BalanceAssistantFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BalanceAssistantFeedbackInputs), __alignof(BalanceAssistantFeedbackInputs));
  in = (BalanceAssistantInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BalanceAssistantInputs), __alignof(BalanceAssistantInputs));

  m_apiBase = (BalanceAssistantAPIBase*)NMPMemoryAllocAligned(sizeof(BalanceAssistantAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BalanceAssistant::~BalanceAssistant()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodyFrame*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) BalanceAssistantAPIBase( in, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant::clearAllData()
{
  in->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceAssistant::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceAssistant::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<BalanceAssistantFeedbackInputs>();
  *in = savedState.getValue<BalanceAssistantInputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BalanceAssistant::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


