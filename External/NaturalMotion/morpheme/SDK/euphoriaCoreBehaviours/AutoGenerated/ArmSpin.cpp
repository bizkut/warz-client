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
#include "Arm.h"
#include "ArmSpin.h"
#include "ArmSpinPackaging.h"
#include "MyNetwork.h"
#include "BodySection.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "FreeFallManagement.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmSpin::ArmSpin(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmSpinData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmSpinData), __alignof(ArmSpinData));
  in = (ArmSpinInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmSpinInputs), __alignof(ArmSpinInputs));
  feedOut = (ArmSpinFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmSpinFeedbackOutputs), __alignof(ArmSpinFeedbackOutputs));
  out = (ArmSpinOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmSpinOutputs), __alignof(ArmSpinOutputs));

  m_apiBase = (ArmSpinAPIBase*)NMPMemoryAllocAligned(sizeof(ArmSpinAPIBase), 16);
  m_updatePackage = (ArmSpinUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmSpinUpdatePackage), 16);
  m_feedbackPackage = (ArmSpinFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmSpinFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmSpin::~ArmSpin()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmSpinAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmSpinUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmSpinFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmSpin::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmSpin::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmSpinData>();
  *in = savedState.getValue<ArmSpinInputs>();
  *feedOut = savedState.getValue<ArmSpinFeedbackOutputs>();
  *out = savedState.getValue<ArmSpinOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmSpin::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


