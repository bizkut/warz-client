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
#include "ShieldBehaviourInterface.h"
#include "ShieldBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ShieldManagement.h"
#include "Arm.h"
#include "ArmGrab.h"
#include "FreeFallManagement.h"
#include "ShieldActionBehaviourInterface.h"
#include "Grab.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ShieldBehaviourInterface::ShieldBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ShieldBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ShieldBehaviourInterfaceData), __alignof(ShieldBehaviourInterfaceData));
  feedIn = (ShieldBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ShieldBehaviourInterfaceFeedbackInputs), __alignof(ShieldBehaviourInterfaceFeedbackInputs));
  feedOut = (ShieldBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ShieldBehaviourInterfaceFeedbackOutputs), __alignof(ShieldBehaviourInterfaceFeedbackOutputs));
  out = (ShieldBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ShieldBehaviourInterfaceOutputs), __alignof(ShieldBehaviourInterfaceOutputs));

  m_apiBase = (ShieldBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(ShieldBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ShieldBehaviourInterface::~ShieldBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ShieldBehaviourInterfaceAPIBase( data, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface::clearAllData()
{
  data->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ShieldBehaviourInterfaceData>();
  *feedIn = savedState.getValue<ShieldBehaviourInterfaceFeedbackInputs>();
  *feedOut = savedState.getValue<ShieldBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<ShieldBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ShieldBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


