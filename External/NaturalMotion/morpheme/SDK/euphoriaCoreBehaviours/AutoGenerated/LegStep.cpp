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
#include "Leg.h"
#include "LegStep.h"
#include "LegStepPackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "SittingBodyBalance.h"
#include "BalanceManagement.h"
#include "StaticBalance.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegStep::LegStep(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegStepData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegStepData), __alignof(LegStepData));
  in = (LegStepInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegStepInputs), __alignof(LegStepInputs));
  feedOut = (LegStepFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(LegStepFeedbackOutputs), __alignof(LegStepFeedbackOutputs));
  out = (LegStepOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegStepOutputs), __alignof(LegStepOutputs));

  m_apiBase = (LegStepAPIBase*)NMPMemoryAllocAligned(sizeof(LegStepAPIBase), 16);
  m_updatePackage = (LegStepUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegStepUpdatePackage), 16);
  m_feedbackPackage = (LegStepFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegStepFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegStep::~LegStep()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegStep::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegStepAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) LegStepUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) LegStepFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegStep::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegStep::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegStep::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegStepData>();
  *in = savedState.getValue<LegStepInputs>();
  *feedOut = savedState.getValue<LegStepFeedbackOutputs>();
  *out = savedState.getValue<LegStepOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegStep::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegStep::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void LegStep::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


