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
#include "LegSpin.h"
#include "LegSpinPackaging.h"
#include "MyNetwork.h"
#include "BodySection.h"
#include "LegsPedalBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "FreeFallManagement.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegSpin::LegSpin(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegSpinData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegSpinData), __alignof(LegSpinData));
  in = (LegSpinInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegSpinInputs), __alignof(LegSpinInputs));
  feedOut = (LegSpinFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(LegSpinFeedbackOutputs), __alignof(LegSpinFeedbackOutputs));
  out = (LegSpinOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegSpinOutputs), __alignof(LegSpinOutputs));

  m_apiBase = (LegSpinAPIBase*)NMPMemoryAllocAligned(sizeof(LegSpinAPIBase), 16);
  m_updatePackage = (LegSpinUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegSpinUpdatePackage), 16);
  m_feedbackPackage = (LegSpinFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegSpinFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegSpin::~LegSpin()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegSpin::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegSpinAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) LegSpinUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) LegSpinFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegSpin::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegSpin::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegSpin::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegSpinData>();
  *in = savedState.getValue<LegSpinInputs>();
  *feedOut = savedState.getValue<LegSpinFeedbackOutputs>();
  *out = savedState.getValue<LegSpinOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegSpin::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegSpin::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void LegSpin::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


