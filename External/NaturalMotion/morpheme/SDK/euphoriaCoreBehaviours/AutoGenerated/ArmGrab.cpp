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
#include "Arm.h"
#include "ArmGrab.h"
#include "ArmGrabPackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "HoldBehaviourInterface.h"
#include "HoldActionBehaviourInterface.h"
#include "ArmHold.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmGrab::ArmGrab(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmGrabData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmGrabData), __alignof(ArmGrabData));
  feedIn = (ArmGrabFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ArmGrabFeedbackInputs), __alignof(ArmGrabFeedbackInputs));
  in = (ArmGrabInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmGrabInputs), __alignof(ArmGrabInputs));
  feedOut = (ArmGrabFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmGrabFeedbackOutputs), __alignof(ArmGrabFeedbackOutputs));
  out = (ArmGrabOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmGrabOutputs), __alignof(ArmGrabOutputs));

  m_apiBase = (ArmGrabAPIBase*)NMPMemoryAllocAligned(sizeof(ArmGrabAPIBase), 16);
  m_updatePackage = (ArmGrabUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmGrabUpdatePackage), 16);
  m_feedbackPackage = (ArmGrabFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmGrabFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmGrab::~ArmGrab()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmGrabAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ArmGrabUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmGrabFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmGrab::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool ArmGrab::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmGrabData>();
  *feedIn = savedState.getValue<ArmGrabFeedbackInputs>();
  *in = savedState.getValue<ArmGrabInputs>();
  *feedOut = savedState.getValue<ArmGrabFeedbackOutputs>();
  *out = savedState.getValue<ArmGrabOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmGrab::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


