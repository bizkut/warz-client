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
#include "HazardAwarenessBehaviourInterface.h"
#include "HazardAwarenessBehaviourInterfacePackaging.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HazardAwarenessBehaviourInterface::HazardAwarenessBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (HazardAwarenessBehaviourInterfaceData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(HazardAwarenessBehaviourInterfaceData), __alignof(HazardAwarenessBehaviourInterfaceData));
  feedIn = (HazardAwarenessBehaviourInterfaceFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(HazardAwarenessBehaviourInterfaceFeedbackInputs), __alignof(HazardAwarenessBehaviourInterfaceFeedbackInputs));
  userIn = (HazardAwarenessBehaviourInterfaceUserInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::UserInputs, sizeof(HazardAwarenessBehaviourInterfaceUserInputs), __alignof(HazardAwarenessBehaviourInterfaceUserInputs));
  feedOut = (HazardAwarenessBehaviourInterfaceFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HazardAwarenessBehaviourInterfaceFeedbackOutputs), __alignof(HazardAwarenessBehaviourInterfaceFeedbackOutputs));
  out = (HazardAwarenessBehaviourInterfaceOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HazardAwarenessBehaviourInterfaceOutputs), __alignof(HazardAwarenessBehaviourInterfaceOutputs));

  m_apiBase = (HazardAwarenessBehaviourInterfaceAPIBase*)NMPMemoryAllocAligned(sizeof(HazardAwarenessBehaviourInterfaceAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HazardAwarenessBehaviourInterface::~HazardAwarenessBehaviourInterface()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void HazardAwarenessBehaviourInterface::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HazardAwarenessBehaviourInterfaceAPIBase( data, userIn, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void HazardAwarenessBehaviourInterface::clearAllData()
{
  data->clear();
  userIn->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HazardAwarenessBehaviourInterface::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*userIn);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HazardAwarenessBehaviourInterface::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<HazardAwarenessBehaviourInterfaceData>();
  *feedIn = savedState.getValue<HazardAwarenessBehaviourInterfaceFeedbackInputs>();
  *userIn = savedState.getValue<HazardAwarenessBehaviourInterfaceUserInputs>();
  *feedOut = savedState.getValue<HazardAwarenessBehaviourInterfaceFeedbackOutputs>();
  *out = savedState.getValue<HazardAwarenessBehaviourInterfaceOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HazardAwarenessBehaviourInterface::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


