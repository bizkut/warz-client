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
#include "BodyFrame.h"
#include "BodyFramePackaging.h"
#include "HazardManagement.h"
#include "BodyBalance.h"
#include "SittingBodyBalance.h"
#include "Leg.h"
#include "Spine.h"
#include "Arm.h"
#include "ArmStandingSupport.h"
#include "LegStandingSupport.h"
#include "Head.h"
#include "SupportPolygon.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "ReachForBody.h"
#include "BalanceAssistant.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BodyFrame::BodyFrame(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (BodyFrameData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(BodyFrameData), __alignof(BodyFrameData));
  feedIn = (BodyFrameFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BodyFrameFeedbackInputs), __alignof(BodyFrameFeedbackInputs));
  in = (BodyFrameInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BodyFrameInputs), __alignof(BodyFrameInputs));
  feedOut = (BodyFrameFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(BodyFrameFeedbackOutputs), __alignof(BodyFrameFeedbackOutputs));
  out = (BodyFrameOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BodyFrameOutputs), __alignof(BodyFrameOutputs));

  SupportPolygon_StandingCon* con0 = (SupportPolygon_StandingCon*)NMPMemoryAllocateFromFormat(SupportPolygon_StandingCon::getMemoryRequirements()).ptr;
  new(con0) SupportPolygon_StandingCon(); // set up vtable
  m_childModules[0] = supportPolygon = (SupportPolygon*)NMPMemoryAllocateFromFormat(SupportPolygon::getMemoryRequirements()).ptr;
  new (supportPolygon) SupportPolygon(mdAllocator, con0);

  SupportPolygon_SittingCon* con1 = (SupportPolygon_SittingCon*)NMPMemoryAllocateFromFormat(SupportPolygon_SittingCon::getMemoryRequirements()).ptr;
  new(con1) SupportPolygon_SittingCon(); // set up vtable
  m_childModules[1] = sittingSupportPolygon = (SupportPolygon*)NMPMemoryAllocateFromFormat(SupportPolygon::getMemoryRequirements()).ptr;
  new (sittingSupportPolygon) SupportPolygon(mdAllocator, con1);

  BodyBalance_Con* con2 = (BodyBalance_Con*)NMPMemoryAllocateFromFormat(BodyBalance_Con::getMemoryRequirements()).ptr;
  new(con2) BodyBalance_Con(); // set up vtable
  m_childModules[2] = bodyBalance = (BodyBalance*)NMPMemoryAllocateFromFormat(BodyBalance::getMemoryRequirements()).ptr;
  new (bodyBalance) BodyBalance(mdAllocator, con2);

  SittingBodyBalance_Con* con3 = (SittingBodyBalance_Con*)NMPMemoryAllocateFromFormat(SittingBodyBalance_Con::getMemoryRequirements()).ptr;
  new(con3) SittingBodyBalance_Con(); // set up vtable
  m_childModules[3] = sittingBodyBalance = (SittingBodyBalance*)NMPMemoryAllocateFromFormat(SittingBodyBalance::getMemoryRequirements()).ptr;
  new (sittingBodyBalance) SittingBodyBalance(mdAllocator, con3);

  ReachForBody_Con* con4 = (ReachForBody_Con*)NMPMemoryAllocateFromFormat(ReachForBody_Con::getMemoryRequirements()).ptr;
  new(con4) ReachForBody_Con(); // set up vtable
  m_childModules[4] = reachForBody = (ReachForBody*)NMPMemoryAllocateFromFormat(ReachForBody::getMemoryRequirements()).ptr;
  new (reachForBody) ReachForBody(mdAllocator, con4);

  BalanceAssistant_Con* con5 = (BalanceAssistant_Con*)NMPMemoryAllocateFromFormat(BalanceAssistant_Con::getMemoryRequirements()).ptr;
  new(con5) BalanceAssistant_Con(); // set up vtable
  m_childModules[5] = balanceAssistant = (BalanceAssistant*)NMPMemoryAllocateFromFormat(BalanceAssistant::getMemoryRequirements()).ptr;
  new (balanceAssistant) BalanceAssistant(mdAllocator, con5);

  m_apiBase = (BodyFrameAPIBase*)NMPMemoryAllocAligned(sizeof(BodyFrameAPIBase), 16);
  m_updatePackage = (BodyFrameUpdatePackage*)NMPMemoryAllocAligned(sizeof(BodyFrameUpdatePackage), 16);
  m_feedbackPackage = (BodyFrameFeedbackPackage*)NMPMemoryAllocAligned(sizeof(BodyFrameFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BodyFrame::~BodyFrame()
{
  ER::ModuleCon* balanceAssistantConnections = balanceAssistant->getConnections();
  if (balanceAssistantConnections)
  {
    balanceAssistantConnections->~ModuleCon();
    NMP::Memory::memFree(balanceAssistantConnections);
  }
  balanceAssistant->~BalanceAssistant();
  NMP::Memory::memFree(balanceAssistant);

  ER::ModuleCon* reachForBodyConnections = reachForBody->getConnections();
  if (reachForBodyConnections)
  {
    reachForBodyConnections->~ModuleCon();
    NMP::Memory::memFree(reachForBodyConnections);
  }
  reachForBody->~ReachForBody();
  NMP::Memory::memFree(reachForBody);

  ER::ModuleCon* sittingBodyBalanceConnections = sittingBodyBalance->getConnections();
  if (sittingBodyBalanceConnections)
  {
    sittingBodyBalanceConnections->~ModuleCon();
    NMP::Memory::memFree(sittingBodyBalanceConnections);
  }
  sittingBodyBalance->~SittingBodyBalance();
  NMP::Memory::memFree(sittingBodyBalance);

  ER::ModuleCon* bodyBalanceConnections = bodyBalance->getConnections();
  if (bodyBalanceConnections)
  {
    bodyBalanceConnections->~ModuleCon();
    NMP::Memory::memFree(bodyBalanceConnections);
  }
  bodyBalance->~BodyBalance();
  NMP::Memory::memFree(bodyBalance);

  ER::ModuleCon* sittingSupportPolygonConnections = sittingSupportPolygon->getConnections();
  if (sittingSupportPolygonConnections)
  {
    sittingSupportPolygonConnections->~ModuleCon();
    NMP::Memory::memFree(sittingSupportPolygonConnections);
  }
  sittingSupportPolygon->~SupportPolygon();
  NMP::Memory::memFree(sittingSupportPolygon);

  ER::ModuleCon* supportPolygonConnections = supportPolygon->getConnections();
  if (supportPolygonConnections)
  {
    supportPolygonConnections->~ModuleCon();
    NMP::Memory::memFree(supportPolygonConnections);
  }
  supportPolygon->~SupportPolygon();
  NMP::Memory::memFree(supportPolygon);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void BodyFrame::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  supportPolygon->create(this);
  sittingSupportPolygon->create(this);
  bodyBalance->create(this);
  sittingBodyBalance->create(this);
  reachForBody->create(this);
  balanceAssistant->create(this);

  new(m_apiBase) BodyFrameAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) BodyFrameUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) BodyFrameFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void BodyFrame::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BodyFrame::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool BodyFrame::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<BodyFrameData>();
  *feedIn = savedState.getValue<BodyFrameFeedbackInputs>();
  *in = savedState.getValue<BodyFrameInputs>();
  *feedOut = savedState.getValue<BodyFrameFeedbackOutputs>();
  *out = savedState.getValue<BodyFrameOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodyFrame::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "supportPolygon";
    case 1: return "sittingSupportPolygon";
    case 2: return "bodyBalance";
    case 3: return "sittingBodyBalance";
    case 4: return "reachForBody";
    case 5: return "balanceAssistant";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// No update() api required
//----------------------------------------------------------------------------------------------------------------------
void BodyFrame::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


