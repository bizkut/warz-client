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
#include "ProtectBehaviourInterface.h"
#include "ProtectBehaviourInterfacePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ProtectBehaviourInterface::ProtectBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  userIn = (ProtectBehaviourInterfaceUserInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::UserInputs, sizeof(ProtectBehaviourInterfaceUserInputs), __alignof(ProtectBehaviourInterfaceUserInputs));
  out = (ProtectBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ProtectBehaviourInterfaceOutputs), __alignof(ProtectBehaviourInterfaceOutputs));

  m_apiBase = (ProtectBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(ProtectBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ProtectBehaviourInterface::~ProtectBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ProtectBehaviourInterfaceAPIBase( userIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviourInterface::clearAllData()
{
  userIn->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ProtectBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*userIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ProtectBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *userIn = savedState.getValue<ProtectBehaviourInterfaceUserInputs>();
  *out = savedState.getValue<ProtectBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ProtectBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


