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
#include "RotateCore.h"
#include "RotateCorePackaging.h"
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
RotateCore::RotateCore(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (RotateCoreFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(RotateCoreFeedbackInputs), __alignof(RotateCoreFeedbackInputs));
  in = (RotateCoreInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(RotateCoreInputs), __alignof(RotateCoreInputs));
  feedOut = (RotateCoreFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(RotateCoreFeedbackOutputs), __alignof(RotateCoreFeedbackOutputs));
  out = (RotateCoreOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(RotateCoreOutputs), __alignof(RotateCoreOutputs));

  m_apiBase = (RotateCoreAPIBase*)NMPMemoryAllocAligned(sizeof(RotateCoreAPIBase), 16);
  m_updatePackage = (RotateCoreUpdatePackage*)NMPMemoryAllocAligned(sizeof(RotateCoreUpdatePackage), 16);
  m_feedbackPackage = (RotateCoreFeedbackPackage*)NMPMemoryAllocAligned(sizeof(RotateCoreFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
RotateCore::~RotateCore()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void RotateCore::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodySection*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) RotateCoreAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) RotateCoreUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) RotateCoreFeedbackPackage(
    feedIn, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void RotateCore::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool RotateCore::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool RotateCore::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<RotateCoreFeedbackInputs>();
  *in = savedState.getValue<RotateCoreInputs>();
  *feedOut = savedState.getValue<RotateCoreFeedbackOutputs>();
  *out = savedState.getValue<RotateCoreOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* RotateCore::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void RotateCore::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void RotateCore::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


