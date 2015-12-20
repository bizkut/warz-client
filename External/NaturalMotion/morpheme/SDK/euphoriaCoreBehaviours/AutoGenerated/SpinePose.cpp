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
#include "Spine.h"
#include "SpinePose.h"
#include "SpinePosePackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "FreeFallManagement.h"
#include "AnimateBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpinePose::SpinePose(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (SpinePoseInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpinePoseInputs), __alignof(SpinePoseInputs));
  out = (SpinePoseOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpinePoseOutputs), __alignof(SpinePoseOutputs));

  m_apiBase = (SpinePoseAPIBase*)NMPMemoryAllocAligned(sizeof(SpinePoseAPIBase), 16);
  m_updatePackage = (SpinePoseUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpinePoseUpdatePackage), 16);
  m_feedbackPackage = (SpinePoseFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpinePoseFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SpinePose::~SpinePose()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SpinePose::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Spine*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SpinePoseAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) SpinePoseUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) SpinePoseFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SpinePose::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SpinePose::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SpinePose::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<SpinePoseInputs>();
  *out = savedState.getValue<SpinePoseOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SpinePose::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SpinePose::update(float timeStep)
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


