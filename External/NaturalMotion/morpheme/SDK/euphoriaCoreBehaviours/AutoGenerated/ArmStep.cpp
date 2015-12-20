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
#include "ArmStep.h"
#include "ArmStepPackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "BalanceManagement.h"
#include "StaticBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmStep::ArmStep(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmStepData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmStepData), __alignof(ArmStepData));
  in = (ArmStepInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmStepInputs), __alignof(ArmStepInputs));
  feedOut = (ArmStepFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmStepFeedbackOutputs), __alignof(ArmStepFeedbackOutputs));
  out = (ArmStepOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmStepOutputs), __alignof(ArmStepOutputs));

  m_apiBase = (ArmStepAPIBase*)NMPMemoryAllocAligned(sizeof(ArmStepAPIBase), 16);
  m_updatePackage = (ArmStepUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmStepUpdatePackage), 16);
  m_feedbackPackage = (ArmStepFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmStepFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmStep::~ArmStep()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStep::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmStepAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmStepUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmStepFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStep::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmStep::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmStep::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmStepData>();
  *in = savedState.getValue<ArmStepInputs>();
  *feedOut = savedState.getValue<ArmStepFeedbackOutputs>();
  *out = savedState.getValue<ArmStepOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmStep::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStep::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStep::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


