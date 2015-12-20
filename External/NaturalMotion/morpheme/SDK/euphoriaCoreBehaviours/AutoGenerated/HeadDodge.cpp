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
#include "HeadDodge.h"
#include "HeadDodgePackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "HeadDodgeBehaviourInterface.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HeadDodge::HeadDodge(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (HeadDodgeData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(HeadDodgeData), __alignof(HeadDodgeData));
  in = (HeadDodgeInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HeadDodgeInputs), __alignof(HeadDodgeInputs));
  feedOut = (HeadDodgeFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HeadDodgeFeedbackOutputs), __alignof(HeadDodgeFeedbackOutputs));
  out = (HeadDodgeOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HeadDodgeOutputs), __alignof(HeadDodgeOutputs));

  m_apiBase = (HeadDodgeAPIBase*)NMPMemoryAllocAligned(sizeof(HeadDodgeAPIBase), 16);
  m_updatePackage = (HeadDodgeUpdatePackage*)NMPMemoryAllocAligned(sizeof(HeadDodgeUpdatePackage), 16);
  m_feedbackPackage = (HeadDodgeFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HeadDodgeFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HeadDodge::~HeadDodge()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Head*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HeadDodgeAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) HeadDodgeUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) HeadDodgeFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadDodge::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadDodge::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<HeadDodgeData>();
  *in = savedState.getValue<HeadDodgeInputs>();
  *feedOut = savedState.getValue<HeadDodgeFeedbackOutputs>();
  *out = savedState.getValue<HeadDodgeOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HeadDodge::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


