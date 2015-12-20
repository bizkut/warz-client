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
#include "ArmAim.h"
#include "ArmAimPackaging.h"
#include "MyNetwork.h"
#include "Spine.h"
#include "SpineAim.h"
#include "AimBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmAim::ArmAim(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (ArmAimInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmAimInputs), __alignof(ArmAimInputs));
  out = (ArmAimOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmAimOutputs), __alignof(ArmAimOutputs));

  m_apiBase = (ArmAimAPIBase*)NMPMemoryAllocAligned(sizeof(ArmAimAPIBase), 16);
  m_updatePackage = (ArmAimUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmAimUpdatePackage), 16);
  m_feedbackPackage = (ArmAimFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmAimFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmAim::~ArmAim()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmAim::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmAimAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) ArmAimUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmAimFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmAim::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmAim::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmAim::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<ArmAimInputs>();
  *out = savedState.getValue<ArmAimOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmAim::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmAim::update(float timeStep)
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


