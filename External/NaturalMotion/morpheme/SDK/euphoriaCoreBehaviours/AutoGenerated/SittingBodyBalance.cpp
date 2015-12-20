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
#include "SittingBodyBalance.h"
#include "SittingBodyBalancePackaging.h"
#include "MyNetwork.h"
#include "SitBehaviourInterface.h"
#include "SupportPolygon.h"
#include "Spine.h"
#include "Arm.h"
#include "BodyBalance.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SittingBodyBalance::SittingBodyBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (SittingBodyBalanceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(SittingBodyBalanceData), __alignof(SittingBodyBalanceData));
  feedIn = (SittingBodyBalanceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(SittingBodyBalanceFeedbackInputs), __alignof(SittingBodyBalanceFeedbackInputs));
  in = (SittingBodyBalanceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SittingBodyBalanceInputs), __alignof(SittingBodyBalanceInputs));
  feedOut = (SittingBodyBalanceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(SittingBodyBalanceFeedbackOutputs), __alignof(SittingBodyBalanceFeedbackOutputs));
  out = (SittingBodyBalanceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SittingBodyBalanceOutputs), __alignof(SittingBodyBalanceOutputs));

  m_apiBase = (SittingBodyBalanceAPIBase*)NMPMemoryAllocAligned(sizeof(SittingBodyBalanceAPIBase), 16);
  m_updatePackage = (SittingBodyBalanceUpdatePackage*)NMPMemoryAllocAligned(sizeof(SittingBodyBalanceUpdatePackage), 16);
  m_feedbackPackage = (SittingBodyBalanceFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SittingBodyBalanceFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SittingBodyBalance::~SittingBodyBalance()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodyFrame*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SittingBodyBalanceAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) SittingBodyBalanceUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) SittingBodyBalanceFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SittingBodyBalance::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool SittingBodyBalance::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<SittingBodyBalanceData>();
  *feedIn = savedState.getValue<SittingBodyBalanceFeedbackInputs>();
  *in = savedState.getValue<SittingBodyBalanceInputs>();
  *feedOut = savedState.getValue<SittingBodyBalanceFeedbackOutputs>();
  *out = savedState.getValue<SittingBodyBalanceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SittingBodyBalance::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


