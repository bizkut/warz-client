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
#include "GrabDetection.h"
#include "GrabDetectionPackaging.h"
#include "MyNetwork.h"
#include "EnvironmentAwareness.h"
#include "Grab.h"
#include "BodyFrame.h"
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
GrabDetection::GrabDetection(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (GrabDetectionData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(GrabDetectionData), __alignof(GrabDetectionData));
  feedIn = (GrabDetectionFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(GrabDetectionFeedbackInputs), __alignof(GrabDetectionFeedbackInputs));
  in = (GrabDetectionInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(GrabDetectionInputs), __alignof(GrabDetectionInputs));
  out = (GrabDetectionOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(GrabDetectionOutputs), __alignof(GrabDetectionOutputs));

  m_apiBase = (GrabDetectionAPIBase*)NMPMemoryAllocAligned(sizeof(GrabDetectionAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
GrabDetection::~GrabDetection()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void GrabDetection::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (HazardManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) GrabDetectionAPIBase( data, in, feedIn, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void GrabDetection::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool GrabDetection::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool GrabDetection::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<GrabDetectionData>();
  *feedIn = savedState.getValue<GrabDetectionFeedbackInputs>();
  *in = savedState.getValue<GrabDetectionInputs>();
  *out = savedState.getValue<GrabDetectionOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* GrabDetection::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


