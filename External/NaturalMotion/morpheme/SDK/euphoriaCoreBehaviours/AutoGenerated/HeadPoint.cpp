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
#include "HeadPoint.h"
#include "HeadPointPackaging.h"
#include "HeadEyes.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HeadPoint::HeadPoint(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (HeadPointData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(HeadPointData), __alignof(HeadPointData));
  in = (HeadPointInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HeadPointInputs), __alignof(HeadPointInputs));
  feedOut = (HeadPointFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HeadPointFeedbackOutputs), __alignof(HeadPointFeedbackOutputs));
  out = (HeadPointOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HeadPointOutputs), __alignof(HeadPointOutputs));

  m_apiBase = (HeadPointAPIBase*)NMPMemoryAllocAligned(sizeof(HeadPointAPIBase), 16);
  m_updatePackage = (HeadPointUpdatePackage*)NMPMemoryAllocAligned(sizeof(HeadPointUpdatePackage), 16);
  m_feedbackPackage = (HeadPointFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HeadPointFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HeadPoint::~HeadPoint()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Head*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HeadPointAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) HeadPointUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) HeadPointFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadPoint::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadPoint::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<HeadPointData>();
  *in = savedState.getValue<HeadPointInputs>();
  *feedOut = savedState.getValue<HeadPointFeedbackOutputs>();
  *out = savedState.getValue<HeadPointOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HeadPoint::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


