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
#include "ArmSwing.h"
#include "ArmSwingPackaging.h"
#include "MyNetwork.h"
#include "ProtectBehaviourInterface.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "BodySection.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmSwing::ArmSwing(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (ArmSwingInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmSwingInputs), __alignof(ArmSwingInputs));
  out = (ArmSwingOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmSwingOutputs), __alignof(ArmSwingOutputs));

  m_apiBase = (ArmSwingAPIBase*)NMPMemoryAllocAligned(sizeof(ArmSwingAPIBase), 16);
  m_updatePackage = (ArmSwingUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmSwingUpdatePackage), 16);
  m_feedbackPackage = (ArmSwingFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmSwingFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmSwing::~ArmSwing()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSwing::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmSwingAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) ArmSwingUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmSwingFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSwing::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmSwing::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmSwing::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<ArmSwingInputs>();
  *out = savedState.getValue<ArmSwingOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmSwing::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSwing::update(float timeStep)
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


