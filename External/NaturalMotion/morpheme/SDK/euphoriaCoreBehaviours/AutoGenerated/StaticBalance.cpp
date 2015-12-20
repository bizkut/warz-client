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
#include "BalanceManagement.h"
#include "StaticBalance.h"
#include "StaticBalancePackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "AimBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "Head.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
StaticBalance::StaticBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (StaticBalanceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(StaticBalanceFeedbackInputs), __alignof(StaticBalanceFeedbackInputs));
  in = (StaticBalanceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(StaticBalanceInputs), __alignof(StaticBalanceInputs));
  out = (StaticBalanceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(StaticBalanceOutputs), __alignof(StaticBalanceOutputs));

  m_apiBase = (StaticBalanceAPIBase*)NMPMemoryAllocAligned(sizeof(StaticBalanceAPIBase), 16);
  m_updatePackage = (StaticBalanceUpdatePackage*)NMPMemoryAllocAligned(sizeof(StaticBalanceUpdatePackage), 16);
  m_feedbackPackage = (StaticBalanceFeedbackPackage*)NMPMemoryAllocAligned(sizeof(StaticBalanceFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
StaticBalance::~StaticBalance()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void StaticBalance::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BalanceManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) StaticBalanceAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) StaticBalanceUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) StaticBalanceFeedbackPackage(
    feedIn, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void StaticBalance::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool StaticBalance::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StaticBalance::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<StaticBalanceFeedbackInputs>();
  *in = savedState.getValue<StaticBalanceInputs>();
  *out = savedState.getValue<StaticBalanceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* StaticBalance::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void StaticBalance::update(float timeStep)
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


