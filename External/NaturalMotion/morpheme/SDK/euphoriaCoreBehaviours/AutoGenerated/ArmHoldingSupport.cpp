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
#include "ArmHoldingSupport.h"
#include "ArmHoldingSupportPackaging.h"
#include "MyNetwork.h"
#include "BodySection.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "ArmHold.h"
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
ArmHoldingSupport::ArmHoldingSupport(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmHoldingSupportData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmHoldingSupportData), __alignof(ArmHoldingSupportData));
  in = (ArmHoldingSupportInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmHoldingSupportInputs), __alignof(ArmHoldingSupportInputs));
  feedOut = (ArmHoldingSupportFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmHoldingSupportFeedbackOutputs), __alignof(ArmHoldingSupportFeedbackOutputs));
  out = (ArmHoldingSupportOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmHoldingSupportOutputs), __alignof(ArmHoldingSupportOutputs));

  m_apiBase = (ArmHoldingSupportAPIBase*)NMPMemoryAllocAligned(sizeof(ArmHoldingSupportAPIBase), 16);
  m_updatePackage = (ArmHoldingSupportUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmHoldingSupportUpdatePackage), 16);
  m_feedbackPackage = (ArmHoldingSupportFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmHoldingSupportFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmHoldingSupport::~ArmHoldingSupport()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmHoldingSupportAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmHoldingSupportUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmHoldingSupportFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmHoldingSupport::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmHoldingSupport::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmHoldingSupportData>();
  *in = savedState.getValue<ArmHoldingSupportInputs>();
  *feedOut = savedState.getValue<ArmHoldingSupportFeedbackOutputs>();
  *out = savedState.getValue<ArmHoldingSupportOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmHoldingSupport::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


