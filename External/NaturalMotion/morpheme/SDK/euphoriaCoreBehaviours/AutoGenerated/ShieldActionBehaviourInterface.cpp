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
#include "ShieldActionBehaviourInterface.h"
#include "ShieldActionBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ShieldManagement.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ShieldActionBehaviourInterface::ShieldActionBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ShieldActionBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ShieldActionBehaviourInterfaceData), __alignof(ShieldActionBehaviourInterfaceData));
  feedIn = (ShieldActionBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ShieldActionBehaviourInterfaceFeedbackInputs), __alignof(ShieldActionBehaviourInterfaceFeedbackInputs));
  out = (ShieldActionBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ShieldActionBehaviourInterfaceOutputs), __alignof(ShieldActionBehaviourInterfaceOutputs));

  m_apiBase = (ShieldActionBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(ShieldActionBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ShieldActionBehaviourInterface::~ShieldActionBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ShieldActionBehaviourInterfaceAPIBase( data, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface::clearAllData()
{
  data->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldActionBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldActionBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ShieldActionBehaviourInterfaceData>();
  *feedIn = savedState.getValue<ShieldActionBehaviourInterfaceFeedbackInputs>();
  *out = savedState.getValue<ShieldActionBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ShieldActionBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


