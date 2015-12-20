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
#include "Head.h"
#include "HeadEyes.h"
#include "HeadEyesPackaging.h"
#include "MyNetwork.h"
#include "LookBehaviourInterface.h"
#include "RandomLook.h"
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "ObserveBehaviourInterface.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "FreeFallManagement.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "ReachForWorldBehaviourInterface.h"
#include "GrabDetection.h"
#include "Grab.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HeadEyes::HeadEyes(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (HeadEyesData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(HeadEyesData), __alignof(HeadEyesData));
  in = (HeadEyesInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HeadEyesInputs), __alignof(HeadEyesInputs));
  feedOut = (HeadEyesFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HeadEyesFeedbackOutputs), __alignof(HeadEyesFeedbackOutputs));
  out = (HeadEyesOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HeadEyesOutputs), __alignof(HeadEyesOutputs));

  m_apiBase = (HeadEyesAPIBase*)NMPMemoryAllocAligned(sizeof(HeadEyesAPIBase), 16);
  m_updatePackage = (HeadEyesUpdatePackage*)NMPMemoryAllocAligned(sizeof(HeadEyesUpdatePackage), 16);
  m_feedbackPackage = (HeadEyesFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HeadEyesFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HeadEyes::~HeadEyes()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Head*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HeadEyesAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) HeadEyesUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) HeadEyesFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadEyes::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadEyes::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<HeadEyesData>();
  *in = savedState.getValue<HeadEyesInputs>();
  *feedOut = savedState.getValue<HeadEyesFeedbackOutputs>();
  *out = savedState.getValue<HeadEyesOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HeadEyes::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


