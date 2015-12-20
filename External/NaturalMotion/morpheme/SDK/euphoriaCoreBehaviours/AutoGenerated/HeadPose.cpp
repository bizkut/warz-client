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
#include "Head.h"
#include "HeadPose.h"
#include "HeadPosePackaging.h"
#include "MyNetwork.h"
#include "AnimateBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "WritheBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HeadPose::HeadPose(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (HeadPoseInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HeadPoseInputs), __alignof(HeadPoseInputs));
  out = (HeadPoseOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HeadPoseOutputs), __alignof(HeadPoseOutputs));

  m_apiBase = (HeadPoseAPIBase*)NMPMemoryAllocAligned(sizeof(HeadPoseAPIBase), 16);
  m_updatePackage = (HeadPoseUpdatePackage*)NMPMemoryAllocAligned(sizeof(HeadPoseUpdatePackage), 16);
  m_feedbackPackage = (HeadPoseFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HeadPoseFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HeadPose::~HeadPose()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPose::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Head*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HeadPoseAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) HeadPoseUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) HeadPoseFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPose::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadPose::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadPose::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<HeadPoseInputs>();
  *out = savedState.getValue<HeadPoseOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HeadPose::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPose::update(float timeStep)
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


