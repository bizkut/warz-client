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
#include "HazardResponse.h"
#include "HazardResponsePackaging.h"
#include "ImpactPredictor.h"
#include "MyNetwork.h"
#include "ShieldBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HazardResponse::HazardResponse(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (HazardResponseData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(HazardResponseData), __alignof(HazardResponseData));
  feedIn = (HazardResponseFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(HazardResponseFeedbackInputs), __alignof(HazardResponseFeedbackInputs));
  in = (HazardResponseInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HazardResponseInputs), __alignof(HazardResponseInputs));
  out = (HazardResponseOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HazardResponseOutputs), __alignof(HazardResponseOutputs));

  m_apiBase = (HazardResponseAPIBase*)NMPMemoryAllocAligned(sizeof(HazardResponseAPIBase), 16);
  m_updatePackage = (HazardResponseUpdatePackage*)NMPMemoryAllocAligned(sizeof(HazardResponseUpdatePackage), 16);
  m_feedbackPackage = (HazardResponseFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HazardResponseFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HazardResponse::~HazardResponse()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HazardResponse::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (HazardManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) HazardResponseAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) HazardResponseUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) HazardResponseFeedbackPackage(
    data, feedIn, in, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HazardResponse::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HazardResponse::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HazardResponse::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<HazardResponseData>();
  *feedIn = savedState.getValue<HazardResponseFeedbackInputs>();
  *in = savedState.getValue<HazardResponseInputs>();
  *out = savedState.getValue<HazardResponseOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HazardResponse::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HazardResponse::update(float timeStep)
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


