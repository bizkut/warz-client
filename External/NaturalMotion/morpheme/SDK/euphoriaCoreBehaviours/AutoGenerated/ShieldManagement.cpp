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
#include "ShieldManagement.h"
#include "ShieldManagementPackaging.h"
#include "MyNetwork.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "HazardResponse.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ShieldManagement::ShieldManagement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ShieldManagementData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ShieldManagementData), __alignof(ShieldManagementData));
  feedIn = (ShieldManagementFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ShieldManagementFeedbackInputs), __alignof(ShieldManagementFeedbackInputs));
  in = (ShieldManagementInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ShieldManagementInputs), __alignof(ShieldManagementInputs));
  feedOut = (ShieldManagementFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ShieldManagementFeedbackOutputs), __alignof(ShieldManagementFeedbackOutputs));
  out = (ShieldManagementOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ShieldManagementOutputs), __alignof(ShieldManagementOutputs));

  m_apiBase = (ShieldManagementAPIBase*)NMPMemoryAllocAligned(sizeof(ShieldManagementAPIBase), 16);
  m_updatePackage = (ShieldManagementUpdatePackage*)NMPMemoryAllocAligned(sizeof(ShieldManagementUpdatePackage), 16);
  m_feedbackPackage = (ShieldManagementFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ShieldManagementFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ShieldManagement::~ShieldManagement()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (HazardManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ShieldManagementAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ShieldManagementUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) ShieldManagementFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldManagement::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool ShieldManagement::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ShieldManagementData>();
  *feedIn = savedState.getValue<ShieldManagementFeedbackInputs>();
  *in = savedState.getValue<ShieldManagementInputs>();
  *feedOut = savedState.getValue<ShieldManagementFeedbackOutputs>();
  *out = savedState.getValue<ShieldManagementOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ShieldManagement::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


