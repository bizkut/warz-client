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
#include "SteppingBalance.h"
#include "SteppingBalancePackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStep.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SteppingBalance::SteppingBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (SteppingBalanceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(SteppingBalanceData), __alignof(SteppingBalanceData));
  feedIn = (SteppingBalanceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(SteppingBalanceFeedbackInputs), __alignof(SteppingBalanceFeedbackInputs));
  in = (SteppingBalanceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SteppingBalanceInputs), __alignof(SteppingBalanceInputs));
  feedOut = (SteppingBalanceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(SteppingBalanceFeedbackOutputs), __alignof(SteppingBalanceFeedbackOutputs));
  out = (SteppingBalanceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SteppingBalanceOutputs), __alignof(SteppingBalanceOutputs));

  m_apiBase = (SteppingBalanceAPIBase*)NMPMemoryAllocAligned(sizeof(SteppingBalanceAPIBase), 16);
  m_updatePackage = (SteppingBalanceUpdatePackage*)NMPMemoryAllocAligned(sizeof(SteppingBalanceUpdatePackage), 16);
  m_feedbackPackage = (SteppingBalanceFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SteppingBalanceFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SteppingBalance::~SteppingBalance()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BalanceManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SteppingBalanceAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) SteppingBalanceUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) SteppingBalanceFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SteppingBalance::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool SteppingBalance::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<SteppingBalanceData>();
  *feedIn = savedState.getValue<SteppingBalanceFeedbackInputs>();
  *in = savedState.getValue<SteppingBalanceInputs>();
  *feedOut = savedState.getValue<SteppingBalanceFeedbackOutputs>();
  *out = savedState.getValue<SteppingBalanceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SteppingBalance::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


