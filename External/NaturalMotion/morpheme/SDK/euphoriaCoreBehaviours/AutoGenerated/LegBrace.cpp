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
#include "LegBrace.h"
#include "LegBracePackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegBrace::LegBrace(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegBraceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegBraceData), __alignof(LegBraceData));
  feedIn = (LegBraceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(LegBraceFeedbackInputs), __alignof(LegBraceFeedbackInputs));
  in = (LegBraceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegBraceInputs), __alignof(LegBraceInputs));
  feedOut = (LegBraceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(LegBraceFeedbackOutputs), __alignof(LegBraceFeedbackOutputs));
  out = (LegBraceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegBraceOutputs), __alignof(LegBraceOutputs));

  m_apiBase = (LegBraceAPIBase*)NMPMemoryAllocAligned(sizeof(LegBraceAPIBase), 16);
  m_updatePackage = (LegBraceUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegBraceUpdatePackage), 16);
  m_feedbackPackage = (LegBraceFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegBraceFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegBrace::~LegBrace()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegBrace::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegBraceAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) LegBraceUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) LegBraceFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegBrace::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegBrace::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool LegBrace::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegBraceData>();
  *feedIn = savedState.getValue<LegBraceFeedbackInputs>();
  *in = savedState.getValue<LegBraceInputs>();
  *feedOut = savedState.getValue<LegBraceFeedbackOutputs>();
  *out = savedState.getValue<LegBraceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegBrace::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegBrace::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void LegBrace::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


