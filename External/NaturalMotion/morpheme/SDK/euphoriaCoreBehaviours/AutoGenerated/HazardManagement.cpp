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
#include "HazardManagement.h"
#include "HazardManagementPackaging.h"
#include "HazardResponse.h"
#include "BodyFrame.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "GrabDetection.h"
#include "Grab.h"
#include "FreeFallManagement.h"
#include "ImpactPredictor.h"
#include "ShieldManagement.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
HazardManagement::HazardManagement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (HazardManagementFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(HazardManagementFeedbackInputs), __alignof(HazardManagementFeedbackInputs));
  out = (HazardManagementOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HazardManagementOutputs), __alignof(HazardManagementOutputs));

  GrabDetection_Con* con0 = (GrabDetection_Con*)NMPMemoryAllocateFromFormat(GrabDetection_Con::getMemoryRequirements()).ptr;
  new(con0) GrabDetection_Con(); // set up vtable
  m_childModules[0] = grabDetection = (GrabDetection*)NMPMemoryAllocateFromFormat(GrabDetection::getMemoryRequirements()).ptr;
  new (grabDetection) GrabDetection(mdAllocator, con0);

  Grab_Con* con1 = (Grab_Con*)NMPMemoryAllocateFromFormat(Grab_Con::getMemoryRequirements()).ptr;
  new(con1) Grab_Con(); // set up vtable
  m_childModules[1] = grab = (Grab*)NMPMemoryAllocateFromFormat(Grab::getMemoryRequirements()).ptr;
  new (grab) Grab(mdAllocator, con1);

  HazardResponse_Con* con2 = (HazardResponse_Con*)NMPMemoryAllocateFromFormat(HazardResponse_Con::getMemoryRequirements()).ptr;
  new(con2) HazardResponse_Con(); // set up vtable
  m_childModules[2] = hazardResponse = (HazardResponse*)NMPMemoryAllocateFromFormat(HazardResponse::getMemoryRequirements()).ptr;
  new (hazardResponse) HazardResponse(mdAllocator, con2);

  FreeFallManagement_Con* con3 = (FreeFallManagement_Con*)NMPMemoryAllocateFromFormat(FreeFallManagement_Con::getMemoryRequirements()).ptr;
  new(con3) FreeFallManagement_Con(); // set up vtable
  m_childModules[3] = freeFallManagement = (FreeFallManagement*)NMPMemoryAllocateFromFormat(FreeFallManagement::getMemoryRequirements()).ptr;
  new (freeFallManagement) FreeFallManagement(mdAllocator, con3);

  ImpactPredictor_chestCon* con4 = (ImpactPredictor_chestCon*)NMPMemoryAllocateFromFormat(ImpactPredictor_chestCon::getMemoryRequirements()).ptr;
  new(con4) ImpactPredictor_chestCon(); // set up vtable
  m_childModules[4] = chestImpactPredictor = (ImpactPredictor*)NMPMemoryAllocateFromFormat(ImpactPredictor::getMemoryRequirements()).ptr;
  new (chestImpactPredictor) ImpactPredictor(mdAllocator, con4);

  ShieldManagement_Con* con5 = (ShieldManagement_Con*)NMPMemoryAllocateFromFormat(ShieldManagement_Con::getMemoryRequirements()).ptr;
  new(con5) ShieldManagement_Con(); // set up vtable
  m_childModules[5] = shieldManagement = (ShieldManagement*)NMPMemoryAllocateFromFormat(ShieldManagement::getMemoryRequirements()).ptr;
  new (shieldManagement) ShieldManagement(mdAllocator, con5);

  m_apiBase = (HazardManagementAPIBase*)NMPMemoryAllocAligned(sizeof(HazardManagementAPIBase), 16);
  m_updatePackage = (HazardManagementUpdatePackage*)NMPMemoryAllocAligned(sizeof(HazardManagementUpdatePackage), 16);
  m_feedbackPackage = (HazardManagementFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HazardManagementFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
HazardManagement::~HazardManagement()
{
  ER::ModuleCon* shieldManagementConnections = shieldManagement->getConnections();
  if (shieldManagementConnections)
  {
    shieldManagementConnections->~ModuleCon();
    NMP::Memory::memFree(shieldManagementConnections);
  }
  shieldManagement->~ShieldManagement();
  NMP::Memory::memFree(shieldManagement);

  ER::ModuleCon* chestImpactPredictorConnections = chestImpactPredictor->getConnections();
  if (chestImpactPredictorConnections)
  {
    chestImpactPredictorConnections->~ModuleCon();
    NMP::Memory::memFree(chestImpactPredictorConnections);
  }
  chestImpactPredictor->~ImpactPredictor();
  NMP::Memory::memFree(chestImpactPredictor);

  ER::ModuleCon* freeFallManagementConnections = freeFallManagement->getConnections();
  if (freeFallManagementConnections)
  {
    freeFallManagementConnections->~ModuleCon();
    NMP::Memory::memFree(freeFallManagementConnections);
  }
  freeFallManagement->~FreeFallManagement();
  NMP::Memory::memFree(freeFallManagement);

  ER::ModuleCon* hazardResponseConnections = hazardResponse->getConnections();
  if (hazardResponseConnections)
  {
    hazardResponseConnections->~ModuleCon();
    NMP::Memory::memFree(hazardResponseConnections);
  }
  hazardResponse->~HazardResponse();
  NMP::Memory::memFree(hazardResponse);

  ER::ModuleCon* grabConnections = grab->getConnections();
  if (grabConnections)
  {
    grabConnections->~ModuleCon();
    NMP::Memory::memFree(grabConnections);
  }
  grab->~Grab();
  NMP::Memory::memFree(grab);

  ER::ModuleCon* grabDetectionConnections = grabDetection->getConnections();
  if (grabDetectionConnections)
  {
    grabDetectionConnections->~ModuleCon();
    NMP::Memory::memFree(grabDetectionConnections);
  }
  grabDetection->~GrabDetection();
  NMP::Memory::memFree(grabDetection);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void HazardManagement::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  grabDetection->create(this);
  grab->create(this);
  hazardResponse->create(this);
  freeFallManagement->create(this);
  chestImpactPredictor->create(this);
  shieldManagement->create(this);

  new(m_apiBase) HazardManagementAPIBase( feedIn, owner->m_apiBase);
  new(m_updatePackage) HazardManagementUpdatePackage(
    feedIn, out, owner->m_apiBase  );
  new(m_feedbackPackage) HazardManagementFeedbackPackage(
    feedIn, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void HazardManagement::clearAllData()
{
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool HazardManagement::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HazardManagement::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<HazardManagementFeedbackInputs>();
  *out = savedState.getValue<HazardManagementOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* HazardManagement::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "grabDetection";
    case 1: return "grab";
    case 2: return "hazardResponse";
    case 3: return "freeFallManagement";
    case 4: return "chestImpactPredictor";
    case 5: return "shieldManagement";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// No update() api required
//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


