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
#include "AimBehaviourInterface.h"
#include "AimBehaviourInterfacePackaging.h"
#include "Arm.h"
#include "ArmAim.h"
#include "Spine.h"
#include "Leg.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
AimBehaviourInterface::AimBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (AimBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(AimBehaviourInterfaceData), __alignof(AimBehaviourInterfaceData));
  feedIn = (AimBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(AimBehaviourInterfaceFeedbackInputs), __alignof(AimBehaviourInterfaceFeedbackInputs));
  out = (AimBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(AimBehaviourInterfaceOutputs), __alignof(AimBehaviourInterfaceOutputs));

  m_apiBase = (AimBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(AimBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
AimBehaviourInterface::~AimBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) AimBehaviourInterfaceAPIBase( data, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface::clearAllData()
{
  data->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool AimBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AimBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<AimBehaviourInterfaceData>();
  *feedIn = savedState.getValue<AimBehaviourInterfaceFeedbackInputs>();
  *out = savedState.getValue<AimBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AimBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


