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
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "ImpactPredictorPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "EnvironmentAwareness.h"
#include "HazardAwarenessBehaviourInterface.h"
#include "UserHazardBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ImpactPredictor::ImpactPredictor(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ImpactPredictorData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ImpactPredictorData), __alignof(ImpactPredictorData));
  feedIn = (ImpactPredictorFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ImpactPredictorFeedbackInputs), __alignof(ImpactPredictorFeedbackInputs));
  in = (ImpactPredictorInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ImpactPredictorInputs), __alignof(ImpactPredictorInputs));
  feedOut = (ImpactPredictorFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ImpactPredictorFeedbackOutputs), __alignof(ImpactPredictorFeedbackOutputs));
  out = (ImpactPredictorOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ImpactPredictorOutputs), __alignof(ImpactPredictorOutputs));

  m_apiBase = (ImpactPredictorAPIBase*)NMPMemoryAllocAligned(sizeof(ImpactPredictorAPIBase), 16);
  m_updatePackage = (ImpactPredictorUpdatePackage*)NMPMemoryAllocAligned(sizeof(ImpactPredictorUpdatePackage), 16);
  m_feedbackPackage = (ImpactPredictorFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ImpactPredictorFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ImpactPredictor::~ImpactPredictor()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (HazardManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ImpactPredictorAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ImpactPredictorUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) ImpactPredictorFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ImpactPredictor::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool ImpactPredictor::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ImpactPredictorData>();
  *feedIn = savedState.getValue<ImpactPredictorFeedbackInputs>();
  *in = savedState.getValue<ImpactPredictorInputs>();
  *feedOut = savedState.getValue<ImpactPredictorFeedbackOutputs>();
  *out = savedState.getValue<ImpactPredictorOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ImpactPredictor::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


