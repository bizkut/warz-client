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
#include "ReachForBodyBehaviourInterface.h"
#include "ReachForBodyBehaviourInterfacePackaging.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ReachForBodyBehaviourInterface::ReachForBodyBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ReachForBodyBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ReachForBodyBehaviourInterfaceData), __alignof(ReachForBodyBehaviourInterfaceData));
  feedIn = (ReachForBodyBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ReachForBodyBehaviourInterfaceFeedbackInputs), __alignof(ReachForBodyBehaviourInterfaceFeedbackInputs));
  out = (ReachForBodyBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ReachForBodyBehaviourInterfaceOutputs), __alignof(ReachForBodyBehaviourInterfaceOutputs));

  m_apiBase = (ReachForBodyBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(ReachForBodyBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ReachForBodyBehaviourInterface::~ReachForBodyBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ReachForBodyBehaviourInterfaceAPIBase( data, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviourInterface::clearAllData()
{
  data->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBodyBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBodyBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ReachForBodyBehaviourInterfaceData>();
  *feedIn = savedState.getValue<ReachForBodyBehaviourInterfaceFeedbackInputs>();
  *out = savedState.getValue<ReachForBodyBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ReachForBodyBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


