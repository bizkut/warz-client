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
#include "BodySection.h"
#include "PositionCore.h"
#include "PositionCorePackaging.h"
#include "BodySectionPackaging.h"
#include "MyNetwork.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
PositionCore::PositionCore(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (PositionCoreFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(PositionCoreFeedbackInputs), __alignof(PositionCoreFeedbackInputs));
  in = (PositionCoreInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(PositionCoreInputs), __alignof(PositionCoreInputs));
  feedOut = (PositionCoreFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(PositionCoreFeedbackOutputs), __alignof(PositionCoreFeedbackOutputs));
  out = (PositionCoreOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(PositionCoreOutputs), __alignof(PositionCoreOutputs));

  m_apiBase = (PositionCoreAPIBase*)NMPMemoryAllocAligned(sizeof(PositionCoreAPIBase), 16);
  m_updatePackage = (PositionCoreUpdatePackage*)NMPMemoryAllocAligned(sizeof(PositionCoreUpdatePackage), 16);
  m_feedbackPackage = (PositionCoreFeedbackPackage*)NMPMemoryAllocAligned(sizeof(PositionCoreFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
PositionCore::~PositionCore()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void PositionCore::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodySection*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) PositionCoreAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) PositionCoreUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) PositionCoreFeedbackPackage(
    feedIn, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void PositionCore::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool PositionCore::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PositionCore::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<PositionCoreFeedbackInputs>();
  *in = savedState.getValue<PositionCoreInputs>();
  *feedOut = savedState.getValue<PositionCoreFeedbackOutputs>();
  *out = savedState.getValue<PositionCoreOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* PositionCore::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void PositionCore::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void PositionCore::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


