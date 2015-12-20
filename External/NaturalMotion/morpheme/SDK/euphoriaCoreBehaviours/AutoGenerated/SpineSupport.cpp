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
#include "Spine.h"
#include "SpineSupport.h"
#include "SpineSupportPackaging.h"
#include "MyNetwork.h"
#include "BalanceManagement.h"
#include "StaticBalance.h"
#include "BalancePoser.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "BodyBalance.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpineSupport::SpineSupport(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (SpineSupportFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(SpineSupportFeedbackInputs), __alignof(SpineSupportFeedbackInputs));
  in = (SpineSupportInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpineSupportInputs), __alignof(SpineSupportInputs));
  feedOut = (SpineSupportFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(SpineSupportFeedbackOutputs), __alignof(SpineSupportFeedbackOutputs));
  out = (SpineSupportOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpineSupportOutputs), __alignof(SpineSupportOutputs));

  m_apiBase = (SpineSupportAPIBase*)NMPMemoryAllocAligned(sizeof(SpineSupportAPIBase), 16);
  m_updatePackage = (SpineSupportUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpineSupportUpdatePackage), 16);
  m_feedbackPackage = (SpineSupportFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpineSupportFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SpineSupport::~SpineSupport()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Spine*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SpineSupportAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) SpineSupportUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) SpineSupportFeedbackPackage(
    feedIn, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineSupport::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineSupport::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<SpineSupportFeedbackInputs>();
  *in = savedState.getValue<SpineSupportInputs>();
  *feedOut = savedState.getValue<SpineSupportFeedbackOutputs>();
  *out = savedState.getValue<SpineSupportOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SpineSupport::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


