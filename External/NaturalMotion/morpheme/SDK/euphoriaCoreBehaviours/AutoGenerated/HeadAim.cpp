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
#include "HeadAim.h"
#include "HeadAimPackaging.h"
#include "MyNetwork.h"
#include "AimBehaviourInterface.h"
#include "Spine.h"
#include "SpineAim.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HeadAim::HeadAim(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (HeadAimFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(HeadAimFeedbackInputs), __alignof(HeadAimFeedbackInputs));
  in = (HeadAimInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HeadAimInputs), __alignof(HeadAimInputs));
  out = (HeadAimOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HeadAimOutputs), __alignof(HeadAimOutputs));

  m_apiBase = (HeadAimAPIBase*)NMPMemoryAllocAligned(sizeof(HeadAimAPIBase), 16);
  m_updatePackage = (HeadAimUpdatePackage*)NMPMemoryAllocAligned(sizeof(HeadAimUpdatePackage), 16);
  m_feedbackPackage = (HeadAimFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HeadAimFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HeadAim::~HeadAim()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadAim::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Head*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HeadAimAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) HeadAimUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) HeadAimFeedbackPackage(
    feedIn, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HeadAim::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadAim::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HeadAim::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<HeadAimFeedbackInputs>();
  *in = savedState.getValue<HeadAimInputs>();
  *out = savedState.getValue<HeadAimOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HeadAim::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HeadAim::update(float timeStep)
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


