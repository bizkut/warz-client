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
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "ReachForBodyPackaging.h"
#include "MyNetwork.h"
#include "ReachForBodyBehaviourInterface.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "Arm.h"
#include "ArmBrace.h"
#include "ArmReachForBodyPart.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ReachForBody::ReachForBody(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ReachForBodyData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ReachForBodyData), __alignof(ReachForBodyData));
  feedIn = (ReachForBodyFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ReachForBodyFeedbackInputs), __alignof(ReachForBodyFeedbackInputs));
  in = (ReachForBodyInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ReachForBodyInputs), __alignof(ReachForBodyInputs));
  feedOut = (ReachForBodyFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ReachForBodyFeedbackOutputs), __alignof(ReachForBodyFeedbackOutputs));
  out = (ReachForBodyOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ReachForBodyOutputs), __alignof(ReachForBodyOutputs));

  m_apiBase = (ReachForBodyAPIBase*)NMPMemoryAllocAligned(sizeof(ReachForBodyAPIBase), 16);
  m_updatePackage = (ReachForBodyUpdatePackage*)NMPMemoryAllocAligned(sizeof(ReachForBodyUpdatePackage), 16);
  m_feedbackPackage = (ReachForBodyFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ReachForBodyFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ReachForBody::~ReachForBody()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodyFrame*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ReachForBodyAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ReachForBodyUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) ReachForBodyFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBody::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBody::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ReachForBodyData>();
  *feedIn = savedState.getValue<ReachForBodyFeedbackInputs>();
  *in = savedState.getValue<ReachForBodyInputs>();
  *feedOut = savedState.getValue<ReachForBodyFeedbackOutputs>();
  *out = savedState.getValue<ReachForBodyOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ReachForBody::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


