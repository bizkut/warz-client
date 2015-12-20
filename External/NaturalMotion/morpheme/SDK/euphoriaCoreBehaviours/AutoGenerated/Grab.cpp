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
#include "Grab.h"
#include "GrabPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "ImpactPredictor.h"
#include "GrabDetection.h"
#include "HoldActionBehaviourInterface.h"
#include "HoldBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
Grab::Grab(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (GrabData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(GrabData), __alignof(GrabData));
  feedIn = (GrabFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(GrabFeedbackInputs), __alignof(GrabFeedbackInputs));
  in = (GrabInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(GrabInputs), __alignof(GrabInputs));
  feedOut = (GrabFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(GrabFeedbackOutputs), __alignof(GrabFeedbackOutputs));
  out = (GrabOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(GrabOutputs), __alignof(GrabOutputs));

  m_apiBase = (GrabAPIBase*)NMPMemoryAllocAligned(sizeof(GrabAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
Grab::~Grab()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void Grab::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (HazardManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) GrabAPIBase( data, in, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void Grab::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool Grab::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Grab::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<GrabData>();
  *feedIn = savedState.getValue<GrabFeedbackInputs>();
  *in = savedState.getValue<GrabInputs>();
  *feedOut = savedState.getValue<GrabFeedbackOutputs>();
  *out = savedState.getValue<GrabOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Grab::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


