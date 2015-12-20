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
#include "ArmHold.h"
#include "ArmHoldPackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "ArmGrab.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmHold::ArmHold(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmHoldData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmHoldData), __alignof(ArmHoldData));
  in = (ArmHoldInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmHoldInputs), __alignof(ArmHoldInputs));
  feedOut = (ArmHoldFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmHoldFeedbackOutputs), __alignof(ArmHoldFeedbackOutputs));
  out = (ArmHoldOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmHoldOutputs), __alignof(ArmHoldOutputs));

  m_apiBase = (ArmHoldAPIBase*)NMPMemoryAllocAligned(sizeof(ArmHoldAPIBase), 16);
  m_updatePackage = (ArmHoldUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmHoldUpdatePackage), 16);
  m_feedbackPackage = (ArmHoldFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmHoldFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmHold::~ArmHold()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHold::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmHoldAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmHoldUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmHoldFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHold::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmHold::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmHold::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmHoldData>();
  *in = savedState.getValue<ArmHoldInputs>();
  *feedOut = savedState.getValue<ArmHoldFeedbackOutputs>();
  *out = savedState.getValue<ArmHoldOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmHold::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHold::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHold::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


