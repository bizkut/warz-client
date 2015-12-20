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
#include "MyNetwork.h"
#include "RandomLook.h"
#include "RandomLookPackaging.h"
#include "IdleAwakeBehaviourInterface.h"
#include "WritheBehaviourInterface.h"
#include "Head.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
RandomLook::RandomLook(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (RandomLookData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(RandomLookData), __alignof(RandomLookData));
  feedIn = (RandomLookFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(RandomLookFeedbackInputs), __alignof(RandomLookFeedbackInputs));
  in = (RandomLookInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(RandomLookInputs), __alignof(RandomLookInputs));
  out = (RandomLookOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(RandomLookOutputs), __alignof(RandomLookOutputs));

  m_apiBase = (RandomLookAPIBase*)NMPMemoryAllocAligned(sizeof(RandomLookAPIBase), 16);
  m_updatePackage = (RandomLookUpdatePackage*)NMPMemoryAllocAligned(sizeof(RandomLookUpdatePackage), 16);
  m_feedbackPackage = (RandomLookFeedbackPackage*)NMPMemoryAllocAligned(sizeof(RandomLookFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
RandomLook::~RandomLook()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void RandomLook::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) RandomLookAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) RandomLookUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) RandomLookFeedbackPackage(
    data, feedIn, in, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void RandomLook::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool RandomLook::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool RandomLook::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<RandomLookData>();
  *feedIn = savedState.getValue<RandomLookFeedbackInputs>();
  *in = savedState.getValue<RandomLookInputs>();
  *out = savedState.getValue<RandomLookOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* RandomLook::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void RandomLook::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


