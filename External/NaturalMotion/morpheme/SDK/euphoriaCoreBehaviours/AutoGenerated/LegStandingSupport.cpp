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
#include "LegStandingSupport.h"
#include "LegStandingSupportPackaging.h"
#include "MyNetwork.h"
#include "BalanceBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegStandingSupport::LegStandingSupport(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegStandingSupportData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegStandingSupportData), __alignof(LegStandingSupportData));
  in = (LegStandingSupportInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegStandingSupportInputs), __alignof(LegStandingSupportInputs));
  feedOut = (LegStandingSupportFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(LegStandingSupportFeedbackOutputs), __alignof(LegStandingSupportFeedbackOutputs));
  out = (LegStandingSupportOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegStandingSupportOutputs), __alignof(LegStandingSupportOutputs));

  m_apiBase = (LegStandingSupportAPIBase*)NMPMemoryAllocAligned(sizeof(LegStandingSupportAPIBase), 16);
  m_updatePackage = (LegStandingSupportUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegStandingSupportUpdatePackage), 16);
  m_feedbackPackage = (LegStandingSupportFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegStandingSupportFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegStandingSupport::~LegStandingSupport()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegStandingSupportAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) LegStandingSupportUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) LegStandingSupportFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegStandingSupport::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegStandingSupport::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegStandingSupportData>();
  *in = savedState.getValue<LegStandingSupportInputs>();
  *feedOut = savedState.getValue<LegStandingSupportFeedbackOutputs>();
  *out = savedState.getValue<LegStandingSupportOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegStandingSupport::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


