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
#include "ArmSittingStep.h"
#include "ArmSittingStepPackaging.h"
#include "MyNetwork.h"
#include "SitBehaviourInterface.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmSittingStep::ArmSittingStep(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (ArmSittingStepFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ArmSittingStepFeedbackInputs), __alignof(ArmSittingStepFeedbackInputs));
  in = (ArmSittingStepInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmSittingStepInputs), __alignof(ArmSittingStepInputs));
  feedOut = (ArmSittingStepFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmSittingStepFeedbackOutputs), __alignof(ArmSittingStepFeedbackOutputs));
  out = (ArmSittingStepOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmSittingStepOutputs), __alignof(ArmSittingStepOutputs));

  m_apiBase = (ArmSittingStepAPIBase*)NMPMemoryAllocAligned(sizeof(ArmSittingStepAPIBase), 16);
  m_updatePackage = (ArmSittingStepUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmSittingStepUpdatePackage), 16);
  m_feedbackPackage = (ArmSittingStepFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmSittingStepFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmSittingStep::~ArmSittingStep()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmSittingStepAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ArmSittingStepUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmSittingStepFeedbackPackage(
    feedIn, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmSittingStep::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmSittingStep::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<ArmSittingStepFeedbackInputs>();
  *in = savedState.getValue<ArmSittingStepInputs>();
  *feedOut = savedState.getValue<ArmSittingStepFeedbackOutputs>();
  *out = savedState.getValue<ArmSittingStepOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmSittingStep::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


