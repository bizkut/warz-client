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
#include "SpineAim.h"
#include "SpineAimPackaging.h"
#include "MyNetwork.h"
#include "AimBehaviourInterface.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpineAim::SpineAim(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (SpineAimInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpineAimInputs), __alignof(SpineAimInputs));
  out = (SpineAimOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpineAimOutputs), __alignof(SpineAimOutputs));

  m_apiBase = (SpineAimAPIBase*)NMPMemoryAllocAligned(sizeof(SpineAimAPIBase), 16);
  m_updatePackage = (SpineAimUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpineAimUpdatePackage), 16);
  m_feedbackPackage = (SpineAimFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpineAimFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SpineAim::~SpineAim()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineAim::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Spine*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SpineAimAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) SpineAimUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) SpineAimFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SpineAim::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineAim::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineAim::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<SpineAimInputs>();
  *out = savedState.getValue<SpineAimOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SpineAim::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SpineAim::update(float timeStep)
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


