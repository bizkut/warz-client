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
#include "LegSittingSupport.h"
#include "LegSittingSupportPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "SitBehaviourInterface.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegSittingSupport::LegSittingSupport(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegSittingSupportData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegSittingSupportData), __alignof(LegSittingSupportData));
  in = (LegSittingSupportInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegSittingSupportInputs), __alignof(LegSittingSupportInputs));
  feedOut = (LegSittingSupportFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(LegSittingSupportFeedbackOutputs), __alignof(LegSittingSupportFeedbackOutputs));
  out = (LegSittingSupportOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegSittingSupportOutputs), __alignof(LegSittingSupportOutputs));

  m_apiBase = (LegSittingSupportAPIBase*)NMPMemoryAllocAligned(sizeof(LegSittingSupportAPIBase), 16);
  m_updatePackage = (LegSittingSupportUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegSittingSupportUpdatePackage), 16);
  m_feedbackPackage = (LegSittingSupportFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegSittingSupportFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegSittingSupport::~LegSittingSupport()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegSittingSupportAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) LegSittingSupportUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) LegSittingSupportFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegSittingSupport::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegSittingSupport::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegSittingSupportData>();
  *in = savedState.getValue<LegSittingSupportInputs>();
  *feedOut = savedState.getValue<LegSittingSupportFeedbackOutputs>();
  *out = savedState.getValue<LegSittingSupportOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegSittingSupport::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


