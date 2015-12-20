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
#include "ArmBrace.h"
#include "ArmBracePackaging.h"
#include "MyNetwork.h"
#include "ArmsBraceBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmBrace::ArmBrace(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmBraceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmBraceData), __alignof(ArmBraceData));
  feedIn = (ArmBraceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ArmBraceFeedbackInputs), __alignof(ArmBraceFeedbackInputs));
  in = (ArmBraceInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmBraceInputs), __alignof(ArmBraceInputs));
  feedOut = (ArmBraceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmBraceFeedbackOutputs), __alignof(ArmBraceFeedbackOutputs));
  out = (ArmBraceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmBraceOutputs), __alignof(ArmBraceOutputs));

  m_apiBase = (ArmBraceAPIBase*)NMPMemoryAllocAligned(sizeof(ArmBraceAPIBase), 16);
  m_updatePackage = (ArmBraceUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmBraceUpdatePackage), 16);
  m_feedbackPackage = (ArmBraceFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmBraceFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmBrace::~ArmBrace()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmBraceAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ArmBraceUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmBraceFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmBrace::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool ArmBrace::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmBraceData>();
  *feedIn = savedState.getValue<ArmBraceFeedbackInputs>();
  *in = savedState.getValue<ArmBraceInputs>();
  *feedOut = savedState.getValue<ArmBraceFeedbackOutputs>();
  *out = savedState.getValue<ArmBraceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmBrace::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


