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
#include "BodyBalance.h"
#include "BodyBalancePackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStandingSupport.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "LegStep.h"
#include "ArmStep.h"
#include "BalanceBehaviourInterface.h"
#include "BalanceManagement.h"
#include "SupportPolygon.h"
#include "BalanceBehaviourFeedback.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BodyBalance::BodyBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (BodyBalanceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(BodyBalanceData), __alignof(BodyBalanceData));
  feedIn = (BodyBalanceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BodyBalanceFeedbackInputs), __alignof(BodyBalanceFeedbackInputs));
  in = (BodyBalanceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BodyBalanceInputs), __alignof(BodyBalanceInputs));
  feedOut = (BodyBalanceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(BodyBalanceFeedbackOutputs), __alignof(BodyBalanceFeedbackOutputs));
  out = (BodyBalanceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BodyBalanceOutputs), __alignof(BodyBalanceOutputs));

  m_apiBase = (BodyBalanceAPIBase*)NMPMemoryAllocAligned(sizeof(BodyBalanceAPIBase), 16);
  m_updatePackage = (BodyBalanceUpdatePackage*)NMPMemoryAllocAligned(sizeof(BodyBalanceUpdatePackage), 16);
  m_feedbackPackage = (BodyBalanceFeedbackPackage*)NMPMemoryAllocAligned(sizeof(BodyBalanceFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BodyBalance::~BodyBalance()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodyFrame*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) BodyBalanceAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) BodyBalanceUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) BodyBalanceFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BodyBalance::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool BodyBalance::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<BodyBalanceData>();
  *feedIn = savedState.getValue<BodyBalanceFeedbackInputs>();
  *in = savedState.getValue<BodyBalanceInputs>();
  *feedOut = savedState.getValue<BodyBalanceFeedbackOutputs>();
  *out = savedState.getValue<BodyBalanceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodyBalance::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


