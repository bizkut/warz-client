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
#include "MyNetwork.h"
#include "EnvironmentAwareness.h"
#include "EnvironmentAwarenessPackaging.h"
#include "Head.h"
#include "HeadEyes.h"
#include "SceneProbes.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "ObserveBehaviourInterface.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
EnvironmentAwareness::EnvironmentAwareness(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (EnvironmentAwarenessData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(EnvironmentAwarenessData), __alignof(EnvironmentAwarenessData));
  feedIn = (EnvironmentAwarenessFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(EnvironmentAwarenessFeedbackInputs), __alignof(EnvironmentAwarenessFeedbackInputs));
  in = (EnvironmentAwarenessInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(EnvironmentAwarenessInputs), __alignof(EnvironmentAwarenessInputs));
  feedOut = (EnvironmentAwarenessFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(EnvironmentAwarenessFeedbackOutputs), __alignof(EnvironmentAwarenessFeedbackOutputs));
  out = (EnvironmentAwarenessOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(EnvironmentAwarenessOutputs), __alignof(EnvironmentAwarenessOutputs));

  m_apiBase = (EnvironmentAwarenessAPIBase*)NMPMemoryAllocAligned(sizeof(EnvironmentAwarenessAPIBase), 16);
  m_updatePackage = (EnvironmentAwarenessUpdatePackage*)NMPMemoryAllocAligned(sizeof(EnvironmentAwarenessUpdatePackage), 16);
  m_feedbackPackage = (EnvironmentAwarenessFeedbackPackage*)NMPMemoryAllocAligned(sizeof(EnvironmentAwarenessFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
EnvironmentAwareness::~EnvironmentAwareness()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) EnvironmentAwarenessAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) EnvironmentAwarenessUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) EnvironmentAwarenessFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool EnvironmentAwareness::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool EnvironmentAwareness::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<EnvironmentAwarenessData>();
  *feedIn = savedState.getValue<EnvironmentAwarenessFeedbackInputs>();
  *in = savedState.getValue<EnvironmentAwarenessInputs>();
  *feedOut = savedState.getValue<EnvironmentAwarenessFeedbackOutputs>();
  *out = savedState.getValue<EnvironmentAwarenessOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* EnvironmentAwareness::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


