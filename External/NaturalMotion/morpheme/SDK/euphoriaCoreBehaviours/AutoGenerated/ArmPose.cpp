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
#include "ArmPose.h"
#include "ArmPosePackaging.h"
#include "MyNetwork.h"
#include "AnimateBehaviourInterface.h"
#include "BalanceBehaviourInterface.h"
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "IdleAwakeBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmPose::ArmPose(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (ArmPoseInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmPoseInputs), __alignof(ArmPoseInputs));
  out = (ArmPoseOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmPoseOutputs), __alignof(ArmPoseOutputs));

  m_apiBase = (ArmPoseAPIBase*)NMPMemoryAllocAligned(sizeof(ArmPoseAPIBase), 16);
  m_updatePackage = (ArmPoseUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmPoseUpdatePackage), 16);
  m_feedbackPackage = (ArmPoseFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmPoseFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmPose::~ArmPose()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPose::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmPoseAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) ArmPoseUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmPoseFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPose::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmPose::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmPose::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<ArmPoseInputs>();
  *out = savedState.getValue<ArmPoseOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmPose::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPose::update(float timeStep)
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


