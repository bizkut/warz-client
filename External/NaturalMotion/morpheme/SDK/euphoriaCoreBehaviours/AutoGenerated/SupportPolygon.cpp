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
#include "BodyFrame.h"
#include "SupportPolygon.h"
#include "SupportPolygonPackaging.h"
#include "MyNetwork.h"
#include "Leg.h"
#include "LegStandingSupport.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "Spine.h"
#include "SpineSupport.h"
#include "BalanceBehaviourInterface.h"
#include "SitBehaviourInterface.h"
#include "BodyFramePackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SupportPolygon::SupportPolygon(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (SupportPolygonData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(SupportPolygonData), __alignof(SupportPolygonData));
  feedIn = (SupportPolygonFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(SupportPolygonFeedbackInputs), __alignof(SupportPolygonFeedbackInputs));
  in = (SupportPolygonInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SupportPolygonInputs), __alignof(SupportPolygonInputs));
  out = (SupportPolygonOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SupportPolygonOutputs), __alignof(SupportPolygonOutputs));

  m_apiBase = (SupportPolygonAPIBase*)NMPMemoryAllocAligned(sizeof(SupportPolygonAPIBase), 16);
  m_updatePackage = (SupportPolygonUpdatePackage*)NMPMemoryAllocAligned(sizeof(SupportPolygonUpdatePackage), 16);
  m_feedbackPackage = (SupportPolygonFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SupportPolygonFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SupportPolygon::~SupportPolygon()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BodyFrame*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SupportPolygonAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) SupportPolygonUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) SupportPolygonFeedbackPackage(
    data, feedIn, in, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SupportPolygon::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SupportPolygon::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<SupportPolygonData>();
  *feedIn = savedState.getValue<SupportPolygonFeedbackInputs>();
  *in = savedState.getValue<SupportPolygonInputs>();
  *out = savedState.getValue<SupportPolygonOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SupportPolygon::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


