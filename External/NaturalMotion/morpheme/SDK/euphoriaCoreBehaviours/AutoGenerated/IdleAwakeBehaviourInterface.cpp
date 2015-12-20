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
#include "IdleAwakeBehaviourInterface.h"
#include "IdleAwakeBehaviourInterfacePackaging.h"
#include "BodyFrame.h"
#include "Spine.h"
#include "Head.h"
#include "Leg.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "LegStandingSupport.h"
#include "BodyBalance.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
IdleAwakeBehaviourInterface::IdleAwakeBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (IdleAwakeBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(IdleAwakeBehaviourInterfaceData), __alignof(IdleAwakeBehaviourInterfaceData));
  feedIn = (IdleAwakeBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(IdleAwakeBehaviourInterfaceFeedbackInputs), __alignof(IdleAwakeBehaviourInterfaceFeedbackInputs));
  out = (IdleAwakeBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(IdleAwakeBehaviourInterfaceOutputs), __alignof(IdleAwakeBehaviourInterfaceOutputs));

  m_apiBase = (IdleAwakeBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(IdleAwakeBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
IdleAwakeBehaviourInterface::~IdleAwakeBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) IdleAwakeBehaviourInterfaceAPIBase( data, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface::clearAllData()
{
  data->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool IdleAwakeBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool IdleAwakeBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<IdleAwakeBehaviourInterfaceData>();
  *feedIn = savedState.getValue<IdleAwakeBehaviourInterfaceFeedbackInputs>();
  *out = savedState.getValue<IdleAwakeBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* IdleAwakeBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


