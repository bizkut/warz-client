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
#include "LegsPedalBehaviourInterface.h"
#include "LegsPedalBehaviourInterfacePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegsPedalBehaviourInterface::LegsPedalBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  out = (LegsPedalBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegsPedalBehaviourInterfaceOutputs), __alignof(LegsPedalBehaviourInterfaceOutputs));

  m_apiBase = (LegsPedalBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(LegsPedalBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegsPedalBehaviourInterface::~LegsPedalBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegsPedalBehaviourInterfaceAPIBase( owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviourInterface::clearAllData()
{
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegsPedalBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegsPedalBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *out = savedState.getValue<LegsPedalBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegsPedalBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


