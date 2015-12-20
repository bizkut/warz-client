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
#include "WritheBehaviourInterface.h"
#include "WritheBehaviourInterfacePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
WritheBehaviourInterface::WritheBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  out = (WritheBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(WritheBehaviourInterfaceOutputs), __alignof(WritheBehaviourInterfaceOutputs));

  m_apiBase = (WritheBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(WritheBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
WritheBehaviourInterface::~WritheBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) WritheBehaviourInterfaceAPIBase( owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviourInterface::clearAllData()
{
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool WritheBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool WritheBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *out = savedState.getValue<WritheBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* WritheBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


