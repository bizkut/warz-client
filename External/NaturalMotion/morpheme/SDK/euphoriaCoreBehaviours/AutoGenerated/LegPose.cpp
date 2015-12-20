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
#include "Leg.h"
#include "LegPose.h"
#include "LegPosePackaging.h"
#include "MyNetwork.h"
#include "AnimateBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegPose::LegPose(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (LegPoseInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegPoseInputs), __alignof(LegPoseInputs));
  out = (LegPoseOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegPoseOutputs), __alignof(LegPoseOutputs));

  m_apiBase = (LegPoseAPIBase*)NMPMemoryAllocAligned(sizeof(LegPoseAPIBase), 16);
  m_updatePackage = (LegPoseUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegPoseUpdatePackage), 16);
  m_feedbackPackage = (LegPoseFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegPoseFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegPose::~LegPose()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegPose::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegPoseAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) LegPoseUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) LegPoseFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegPose::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegPose::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegPose::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<LegPoseInputs>();
  *out = savedState.getValue<LegPoseOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegPose::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegPose::update(float timeStep)
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


