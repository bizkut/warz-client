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
#include "HazardManagement.h"
#include "FreeFallManagement.h"
#include "FreeFallManagementPackaging.h"
#include "MyNetwork.h"
#include "FreeFallBehaviourInterface.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "Leg.h"
#include "LegStandingSupport.h"
#include "Spine.h"
#include "ImpactPredictor.h"
#include "BodyFrame.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
FreeFallManagement::FreeFallManagement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (FreeFallManagementData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(FreeFallManagementData), __alignof(FreeFallManagementData));
  feedIn = (FreeFallManagementFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(FreeFallManagementFeedbackInputs), __alignof(FreeFallManagementFeedbackInputs));
  in = (FreeFallManagementInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(FreeFallManagementInputs), __alignof(FreeFallManagementInputs));
  out = (FreeFallManagementOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(FreeFallManagementOutputs), __alignof(FreeFallManagementOutputs));

  m_apiBase = (FreeFallManagementAPIBase*)NMPMemoryAllocAligned(sizeof(FreeFallManagementAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
FreeFallManagement::~FreeFallManagement()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (HazardManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) FreeFallManagementAPIBase( data, in, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool FreeFallManagement::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FreeFallManagement::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<FreeFallManagementData>();
  *feedIn = savedState.getValue<FreeFallManagementFeedbackInputs>();
  *in = savedState.getValue<FreeFallManagementInputs>();
  *out = savedState.getValue<FreeFallManagementOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* FreeFallManagement::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


