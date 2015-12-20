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
#include "Leg.h"
#include "LegPackaging.h"
#include "LegSpin.h"
#include "LegSwing.h"
#include "LegPose.h"
#include "LegBrace.h"
#include "LegSittingSupport.h"
#include "LegStandingSupport.h"
#include "LegStep.h"
#include "LegReachReaction.h"
#include "LegWrithe.h"
#include "CharacteristicsBehaviourInterface.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "BraceChooser.h"
#include "MyNetworkPackaging.h"

// external types
#include "NMPlatform/NMVector3.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
Leg::Leg(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegData), __alignof(LegData));
  in = (LegInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegInputs), __alignof(LegInputs));
  feedOut = (LegFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(LegFeedbackOutputs), __alignof(LegFeedbackOutputs));
  out = (LegOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegOutputs), __alignof(LegOutputs));

  LegBrace_Con* con0 = (LegBrace_Con*)NMPMemoryAllocateFromFormat(LegBrace_Con::getMemoryRequirements()).ptr;
  new(con0) LegBrace_Con(); // set up vtable
  m_childModules[0] = brace = (LegBrace*)NMPMemoryAllocateFromFormat(LegBrace::getMemoryRequirements()).ptr;
  new (brace) LegBrace(mdAllocator, con0);

  LegStandingSupport_Con* con1 = (LegStandingSupport_Con*)NMPMemoryAllocateFromFormat(LegStandingSupport_Con::getMemoryRequirements()).ptr;
  new(con1) LegStandingSupport_Con(); // set up vtable
  m_childModules[1] = standingSupport = (LegStandingSupport*)NMPMemoryAllocateFromFormat(LegStandingSupport::getMemoryRequirements()).ptr;
  new (standingSupport) LegStandingSupport(mdAllocator, con1);

  LegSittingSupport_Con* con2 = (LegSittingSupport_Con*)NMPMemoryAllocateFromFormat(LegSittingSupport_Con::getMemoryRequirements()).ptr;
  new(con2) LegSittingSupport_Con(); // set up vtable
  m_childModules[2] = sittingSupport = (LegSittingSupport*)NMPMemoryAllocateFromFormat(LegSittingSupport::getMemoryRequirements()).ptr;
  new (sittingSupport) LegSittingSupport(mdAllocator, con2);

  LegStep_Con* con3 = (LegStep_Con*)NMPMemoryAllocateFromFormat(LegStep_Con::getMemoryRequirements()).ptr;
  new(con3) LegStep_Con(); // set up vtable
  m_childModules[3] = step = (LegStep*)NMPMemoryAllocateFromFormat(LegStep::getMemoryRequirements()).ptr;
  new (step) LegStep(mdAllocator, con3);

  LegSwing_Con* con4 = (LegSwing_Con*)NMPMemoryAllocateFromFormat(LegSwing_Con::getMemoryRequirements()).ptr;
  new(con4) LegSwing_Con(); // set up vtable
  m_childModules[4] = swing = (LegSwing*)NMPMemoryAllocateFromFormat(LegSwing::getMemoryRequirements()).ptr;
  new (swing) LegSwing(mdAllocator, con4);

  LegPose_Con* con5 = (LegPose_Con*)NMPMemoryAllocateFromFormat(LegPose_Con::getMemoryRequirements()).ptr;
  new(con5) LegPose_Con(); // set up vtable
  m_childModules[5] = pose = (LegPose*)NMPMemoryAllocateFromFormat(LegPose::getMemoryRequirements()).ptr;
  new (pose) LegPose(mdAllocator, con5);

  LegSpin_Con* con6 = (LegSpin_Con*)NMPMemoryAllocateFromFormat(LegSpin_Con::getMemoryRequirements()).ptr;
  new(con6) LegSpin_Con(); // set up vtable
  m_childModules[6] = spin = (LegSpin*)NMPMemoryAllocateFromFormat(LegSpin::getMemoryRequirements()).ptr;
  new (spin) LegSpin(mdAllocator, con6);

  LegReachReaction_Con* con7 = (LegReachReaction_Con*)NMPMemoryAllocateFromFormat(LegReachReaction_Con::getMemoryRequirements()).ptr;
  new(con7) LegReachReaction_Con(); // set up vtable
  m_childModules[7] = reachReaction = (LegReachReaction*)NMPMemoryAllocateFromFormat(LegReachReaction::getMemoryRequirements()).ptr;
  new (reachReaction) LegReachReaction(mdAllocator, con7);

  LegWrithe_Con* con8 = (LegWrithe_Con*)NMPMemoryAllocateFromFormat(LegWrithe_Con::getMemoryRequirements()).ptr;
  new(con8) LegWrithe_Con(); // set up vtable
  m_childModules[8] = writhe = (LegWrithe*)NMPMemoryAllocateFromFormat(LegWrithe::getMemoryRequirements()).ptr;
  new (writhe) LegWrithe(mdAllocator, con8);

  m_apiBase = (LegAPIBase*)NMPMemoryAllocAligned(sizeof(LegAPIBase), 16);
  m_updatePackage = (LegUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegUpdatePackage), 16);
  m_feedbackPackage = (LegFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
Leg::~Leg()
{
  ER::ModuleCon* writheConnections = writhe->getConnections();
  if (writheConnections)
  {
    writheConnections->~ModuleCon();
    NMP::Memory::memFree(writheConnections);
  }
  writhe->~LegWrithe();
  NMP::Memory::memFree(writhe);

  ER::ModuleCon* reachReactionConnections = reachReaction->getConnections();
  if (reachReactionConnections)
  {
    reachReactionConnections->~ModuleCon();
    NMP::Memory::memFree(reachReactionConnections);
  }
  reachReaction->~LegReachReaction();
  NMP::Memory::memFree(reachReaction);

  ER::ModuleCon* spinConnections = spin->getConnections();
  if (spinConnections)
  {
    spinConnections->~ModuleCon();
    NMP::Memory::memFree(spinConnections);
  }
  spin->~LegSpin();
  NMP::Memory::memFree(spin);

  ER::ModuleCon* poseConnections = pose->getConnections();
  if (poseConnections)
  {
    poseConnections->~ModuleCon();
    NMP::Memory::memFree(poseConnections);
  }
  pose->~LegPose();
  NMP::Memory::memFree(pose);

  ER::ModuleCon* swingConnections = swing->getConnections();
  if (swingConnections)
  {
    swingConnections->~ModuleCon();
    NMP::Memory::memFree(swingConnections);
  }
  swing->~LegSwing();
  NMP::Memory::memFree(swing);

  ER::ModuleCon* stepConnections = step->getConnections();
  if (stepConnections)
  {
    stepConnections->~ModuleCon();
    NMP::Memory::memFree(stepConnections);
  }
  step->~LegStep();
  NMP::Memory::memFree(step);

  ER::ModuleCon* sittingSupportConnections = sittingSupport->getConnections();
  if (sittingSupportConnections)
  {
    sittingSupportConnections->~ModuleCon();
    NMP::Memory::memFree(sittingSupportConnections);
  }
  sittingSupport->~LegSittingSupport();
  NMP::Memory::memFree(sittingSupport);

  ER::ModuleCon* standingSupportConnections = standingSupport->getConnections();
  if (standingSupportConnections)
  {
    standingSupportConnections->~ModuleCon();
    NMP::Memory::memFree(standingSupportConnections);
  }
  standingSupport->~LegStandingSupport();
  NMP::Memory::memFree(standingSupport);

  ER::ModuleCon* braceConnections = brace->getConnections();
  if (braceConnections)
  {
    braceConnections->~ModuleCon();
    NMP::Memory::memFree(braceConnections);
  }
  brace->~LegBrace();
  NMP::Memory::memFree(brace);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void Leg::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  brace->create(this);
  standingSupport->create(this);
  sittingSupport->create(this);
  step->create(this);
  swing->create(this);
  pose->create(this);
  spin->create(this);
  reachReaction->create(this);
  writhe->create(this);

  new(m_apiBase) LegAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) LegUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) LegFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void Leg::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool Leg::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Leg::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegData>();
  *in = savedState.getValue<LegInputs>();
  *feedOut = savedState.getValue<LegFeedbackOutputs>();
  *out = savedState.getValue<LegOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Leg::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "brace";
    case 1: return "standingSupport";
    case 2: return "sittingSupport";
    case 3: return "step";
    case 4: return "swing";
    case 5: return "pose";
    case 6: return "spin";
    case 7: return "reachReaction";
    case 8: return "writhe";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Leg::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void Leg::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


