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
#include "ArmStandingSupport.h"
#include "ArmStandingSupportPackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "BodyBalance.h"
#include "ArmGrab.h"
#include "SitBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmStandingSupport::ArmStandingSupport(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmStandingSupportData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmStandingSupportData), __alignof(ArmStandingSupportData));
  in = (ArmStandingSupportInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmStandingSupportInputs), __alignof(ArmStandingSupportInputs));
  feedOut = (ArmStandingSupportFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmStandingSupportFeedbackOutputs), __alignof(ArmStandingSupportFeedbackOutputs));
  out = (ArmStandingSupportOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmStandingSupportOutputs), __alignof(ArmStandingSupportOutputs));

  m_apiBase = (ArmStandingSupportAPIBase*)NMPMemoryAllocAligned(sizeof(ArmStandingSupportAPIBase), 16);
  m_updatePackage = (ArmStandingSupportUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmStandingSupportUpdatePackage), 16);
  m_feedbackPackage = (ArmStandingSupportFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmStandingSupportFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmStandingSupport::~ArmStandingSupport()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmStandingSupportAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmStandingSupportUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmStandingSupportFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmStandingSupport::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmStandingSupport::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmStandingSupportData>();
  *in = savedState.getValue<ArmStandingSupportInputs>();
  *feedOut = savedState.getValue<ArmStandingSupportFeedbackOutputs>();
  *out = savedState.getValue<ArmStandingSupportOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmStandingSupport::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


