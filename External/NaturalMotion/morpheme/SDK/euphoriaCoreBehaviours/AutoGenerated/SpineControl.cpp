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
#include "SpineControl.h"
#include "SpineControlPackaging.h"
#include "SpineSupport.h"
#include "MyNetwork.h"
#include "ReachForWorldBehaviourInterface.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpineControl::SpineControl(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (SpineControlInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpineControlInputs), __alignof(SpineControlInputs));
  out = (SpineControlOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpineControlOutputs), __alignof(SpineControlOutputs));

  m_apiBase = (SpineControlAPIBase*)NMPMemoryAllocAligned(sizeof(SpineControlAPIBase), 16);
  m_updatePackage = (SpineControlUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpineControlUpdatePackage), 16);
  m_feedbackPackage = (SpineControlFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpineControlFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SpineControl::~SpineControl()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineControl::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Spine*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SpineControlAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) SpineControlUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) SpineControlFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SpineControl::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineControl::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineControl::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<SpineControlInputs>();
  *out = savedState.getValue<SpineControlOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SpineControl::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SpineControl::update(float timeStep)
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


