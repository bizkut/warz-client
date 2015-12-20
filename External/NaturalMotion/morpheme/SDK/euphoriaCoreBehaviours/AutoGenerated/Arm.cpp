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
#include "Arm.h"
#include "ArmPackaging.h"
#include "ArmSpin.h"
#include "ArmSwing.h"
#include "ArmBrace.h"
#include "ArmPlacement.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "ArmStandingSupport.h"
#include "ArmSittingStep.h"
#include "ArmPose.h"
#include "ArmAim.h"
#include "ArmHoldingSupport.h"
#include "ArmStep.h"
#include "ArmReachForWorld.h"
#include "ArmReachForBodyPart.h"
#include "ArmReachReaction.h"
#include "ArmWrithe.h"
#include "CharacteristicsBehaviourInterface.h"
#include "ArmHold.h"
#include "ArmGrab.h"
#include "BodySection.h"
#include "BraceChooser.h"
#include "MyNetworkPackaging.h"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erDebugDraw.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
Arm::Arm(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmData), __alignof(ArmData));
  in = (ArmInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmInputs), __alignof(ArmInputs));
  feedOut = (ArmFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmFeedbackOutputs), __alignof(ArmFeedbackOutputs));
  out = (ArmOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmOutputs), __alignof(ArmOutputs));

  ArmGrab_Con* con0 = (ArmGrab_Con*)NMPMemoryAllocateFromFormat(ArmGrab_Con::getMemoryRequirements()).ptr;
  new(con0) ArmGrab_Con(); // set up vtable
  m_childModules[0] = grab = (ArmGrab*)NMPMemoryAllocateFromFormat(ArmGrab::getMemoryRequirements()).ptr;
  new (grab) ArmGrab(mdAllocator, con0);

  ArmAim_Con* con1 = (ArmAim_Con*)NMPMemoryAllocateFromFormat(ArmAim_Con::getMemoryRequirements()).ptr;
  new(con1) ArmAim_Con(); // set up vtable
  m_childModules[1] = aim = (ArmAim*)NMPMemoryAllocateFromFormat(ArmAim::getMemoryRequirements()).ptr;
  new (aim) ArmAim(mdAllocator, con1);

  ArmHold_Con* con2 = (ArmHold_Con*)NMPMemoryAllocateFromFormat(ArmHold_Con::getMemoryRequirements()).ptr;
  new(con2) ArmHold_Con(); // set up vtable
  m_childModules[2] = hold = (ArmHold*)NMPMemoryAllocateFromFormat(ArmHold::getMemoryRequirements()).ptr;
  new (hold) ArmHold(mdAllocator, con2);

  ArmBrace_Con* con3 = (ArmBrace_Con*)NMPMemoryAllocateFromFormat(ArmBrace_Con::getMemoryRequirements()).ptr;
  new(con3) ArmBrace_Con(); // set up vtable
  m_childModules[3] = brace = (ArmBrace*)NMPMemoryAllocateFromFormat(ArmBrace::getMemoryRequirements()).ptr;
  new (brace) ArmBrace(mdAllocator, con3);

  ArmStandingSupport_Con* con4 = (ArmStandingSupport_Con*)NMPMemoryAllocateFromFormat(ArmStandingSupport_Con::getMemoryRequirements()).ptr;
  new(con4) ArmStandingSupport_Con(); // set up vtable
  m_childModules[4] = standingSupport = (ArmStandingSupport*)NMPMemoryAllocateFromFormat(ArmStandingSupport::getMemoryRequirements()).ptr;
  new (standingSupport) ArmStandingSupport(mdAllocator, con4);

  ArmHoldingSupport_Con* con5 = (ArmHoldingSupport_Con*)NMPMemoryAllocateFromFormat(ArmHoldingSupport_Con::getMemoryRequirements()).ptr;
  new(con5) ArmHoldingSupport_Con(); // set up vtable
  m_childModules[5] = holdingSupport = (ArmHoldingSupport*)NMPMemoryAllocateFromFormat(ArmHoldingSupport::getMemoryRequirements()).ptr;
  new (holdingSupport) ArmHoldingSupport(mdAllocator, con5);

  ArmSittingStep_Con* con6 = (ArmSittingStep_Con*)NMPMemoryAllocateFromFormat(ArmSittingStep_Con::getMemoryRequirements()).ptr;
  new(con6) ArmSittingStep_Con(); // set up vtable
  m_childModules[6] = sittingStep = (ArmSittingStep*)NMPMemoryAllocateFromFormat(ArmSittingStep::getMemoryRequirements()).ptr;
  new (sittingStep) ArmSittingStep(mdAllocator, con6);

  ArmStep_Con* con7 = (ArmStep_Con*)NMPMemoryAllocateFromFormat(ArmStep_Con::getMemoryRequirements()).ptr;
  new(con7) ArmStep_Con(); // set up vtable
  m_childModules[7] = step = (ArmStep*)NMPMemoryAllocateFromFormat(ArmStep::getMemoryRequirements()).ptr;
  new (step) ArmStep(mdAllocator, con7);

  ArmSpin_Con* con8 = (ArmSpin_Con*)NMPMemoryAllocateFromFormat(ArmSpin_Con::getMemoryRequirements()).ptr;
  new(con8) ArmSpin_Con(); // set up vtable
  m_childModules[8] = spin = (ArmSpin*)NMPMemoryAllocateFromFormat(ArmSpin::getMemoryRequirements()).ptr;
  new (spin) ArmSpin(mdAllocator, con8);

  ArmSwing_Con* con9 = (ArmSwing_Con*)NMPMemoryAllocateFromFormat(ArmSwing_Con::getMemoryRequirements()).ptr;
  new(con9) ArmSwing_Con(); // set up vtable
  m_childModules[9] = swing = (ArmSwing*)NMPMemoryAllocateFromFormat(ArmSwing::getMemoryRequirements()).ptr;
  new (swing) ArmSwing(mdAllocator, con9);

  ArmReachForBodyPart_Con* con10 = (ArmReachForBodyPart_Con*)NMPMemoryAllocateFromFormat(ArmReachForBodyPart_Con::getMemoryRequirements()).ptr;
  new(con10) ArmReachForBodyPart_Con(); // set up vtable
  m_childModules[10] = reachForBodyPart = (ArmReachForBodyPart*)NMPMemoryAllocateFromFormat(ArmReachForBodyPart::getMemoryRequirements()).ptr;
  new (reachForBodyPart) ArmReachForBodyPart(mdAllocator, con10);

  ArmReachForWorld_Con* con11 = (ArmReachForWorld_Con*)NMPMemoryAllocateFromFormat(ArmReachForWorld_Con::getMemoryRequirements()).ptr;
  new(con11) ArmReachForWorld_Con(); // set up vtable
  m_childModules[11] = reachForWorld = (ArmReachForWorld*)NMPMemoryAllocateFromFormat(ArmReachForWorld::getMemoryRequirements()).ptr;
  new (reachForWorld) ArmReachForWorld(mdAllocator, con11);

  ArmPlacement_Con* con12 = (ArmPlacement_Con*)NMPMemoryAllocateFromFormat(ArmPlacement_Con::getMemoryRequirements()).ptr;
  new(con12) ArmPlacement_Con(); // set up vtable
  m_childModules[12] = placement = (ArmPlacement*)NMPMemoryAllocateFromFormat(ArmPlacement::getMemoryRequirements()).ptr;
  new (placement) ArmPlacement(mdAllocator, con12);

  ArmPose_Con* con13 = (ArmPose_Con*)NMPMemoryAllocateFromFormat(ArmPose_Con::getMemoryRequirements()).ptr;
  new(con13) ArmPose_Con(); // set up vtable
  m_childModules[13] = pose = (ArmPose*)NMPMemoryAllocateFromFormat(ArmPose::getMemoryRequirements()).ptr;
  new (pose) ArmPose(mdAllocator, con13);

  ArmReachReaction_Con* con14 = (ArmReachReaction_Con*)NMPMemoryAllocateFromFormat(ArmReachReaction_Con::getMemoryRequirements()).ptr;
  new(con14) ArmReachReaction_Con(); // set up vtable
  m_childModules[14] = reachReaction = (ArmReachReaction*)NMPMemoryAllocateFromFormat(ArmReachReaction::getMemoryRequirements()).ptr;
  new (reachReaction) ArmReachReaction(mdAllocator, con14);

  ArmWrithe_Con* con15 = (ArmWrithe_Con*)NMPMemoryAllocateFromFormat(ArmWrithe_Con::getMemoryRequirements()).ptr;
  new(con15) ArmWrithe_Con(); // set up vtable
  m_childModules[15] = writhe = (ArmWrithe*)NMPMemoryAllocateFromFormat(ArmWrithe::getMemoryRequirements()).ptr;
  new (writhe) ArmWrithe(mdAllocator, con15);

  m_apiBase = (ArmAPIBase*)NMPMemoryAllocAligned(sizeof(ArmAPIBase), 16);
  m_updatePackage = (ArmUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmUpdatePackage), 16);
  m_feedbackPackage = (ArmFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
Arm::~Arm()
{
  ER::ModuleCon* writheConnections = writhe->getConnections();
  if (writheConnections)
  {
    writheConnections->~ModuleCon();
    NMP::Memory::memFree(writheConnections);
  }
  writhe->~ArmWrithe();
  NMP::Memory::memFree(writhe);

  ER::ModuleCon* reachReactionConnections = reachReaction->getConnections();
  if (reachReactionConnections)
  {
    reachReactionConnections->~ModuleCon();
    NMP::Memory::memFree(reachReactionConnections);
  }
  reachReaction->~ArmReachReaction();
  NMP::Memory::memFree(reachReaction);

  ER::ModuleCon* poseConnections = pose->getConnections();
  if (poseConnections)
  {
    poseConnections->~ModuleCon();
    NMP::Memory::memFree(poseConnections);
  }
  pose->~ArmPose();
  NMP::Memory::memFree(pose);

  ER::ModuleCon* placementConnections = placement->getConnections();
  if (placementConnections)
  {
    placementConnections->~ModuleCon();
    NMP::Memory::memFree(placementConnections);
  }
  placement->~ArmPlacement();
  NMP::Memory::memFree(placement);

  ER::ModuleCon* reachForWorldConnections = reachForWorld->getConnections();
  if (reachForWorldConnections)
  {
    reachForWorldConnections->~ModuleCon();
    NMP::Memory::memFree(reachForWorldConnections);
  }
  reachForWorld->~ArmReachForWorld();
  NMP::Memory::memFree(reachForWorld);

  ER::ModuleCon* reachForBodyPartConnections = reachForBodyPart->getConnections();
  if (reachForBodyPartConnections)
  {
    reachForBodyPartConnections->~ModuleCon();
    NMP::Memory::memFree(reachForBodyPartConnections);
  }
  reachForBodyPart->~ArmReachForBodyPart();
  NMP::Memory::memFree(reachForBodyPart);

  ER::ModuleCon* swingConnections = swing->getConnections();
  if (swingConnections)
  {
    swingConnections->~ModuleCon();
    NMP::Memory::memFree(swingConnections);
  }
  swing->~ArmSwing();
  NMP::Memory::memFree(swing);

  ER::ModuleCon* spinConnections = spin->getConnections();
  if (spinConnections)
  {
    spinConnections->~ModuleCon();
    NMP::Memory::memFree(spinConnections);
  }
  spin->~ArmSpin();
  NMP::Memory::memFree(spin);

  ER::ModuleCon* stepConnections = step->getConnections();
  if (stepConnections)
  {
    stepConnections->~ModuleCon();
    NMP::Memory::memFree(stepConnections);
  }
  step->~ArmStep();
  NMP::Memory::memFree(step);

  ER::ModuleCon* sittingStepConnections = sittingStep->getConnections();
  if (sittingStepConnections)
  {
    sittingStepConnections->~ModuleCon();
    NMP::Memory::memFree(sittingStepConnections);
  }
  sittingStep->~ArmSittingStep();
  NMP::Memory::memFree(sittingStep);

  ER::ModuleCon* holdingSupportConnections = holdingSupport->getConnections();
  if (holdingSupportConnections)
  {
    holdingSupportConnections->~ModuleCon();
    NMP::Memory::memFree(holdingSupportConnections);
  }
  holdingSupport->~ArmHoldingSupport();
  NMP::Memory::memFree(holdingSupport);

  ER::ModuleCon* standingSupportConnections = standingSupport->getConnections();
  if (standingSupportConnections)
  {
    standingSupportConnections->~ModuleCon();
    NMP::Memory::memFree(standingSupportConnections);
  }
  standingSupport->~ArmStandingSupport();
  NMP::Memory::memFree(standingSupport);

  ER::ModuleCon* braceConnections = brace->getConnections();
  if (braceConnections)
  {
    braceConnections->~ModuleCon();
    NMP::Memory::memFree(braceConnections);
  }
  brace->~ArmBrace();
  NMP::Memory::memFree(brace);

  ER::ModuleCon* holdConnections = hold->getConnections();
  if (holdConnections)
  {
    holdConnections->~ModuleCon();
    NMP::Memory::memFree(holdConnections);
  }
  hold->~ArmHold();
  NMP::Memory::memFree(hold);

  ER::ModuleCon* aimConnections = aim->getConnections();
  if (aimConnections)
  {
    aimConnections->~ModuleCon();
    NMP::Memory::memFree(aimConnections);
  }
  aim->~ArmAim();
  NMP::Memory::memFree(aim);

  ER::ModuleCon* grabConnections = grab->getConnections();
  if (grabConnections)
  {
    grabConnections->~ModuleCon();
    NMP::Memory::memFree(grabConnections);
  }
  grab->~ArmGrab();
  NMP::Memory::memFree(grab);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void Arm::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  grab->create(this);
  aim->create(this);
  hold->create(this);
  brace->create(this);
  standingSupport->create(this);
  holdingSupport->create(this);
  sittingStep->create(this);
  step->create(this);
  spin->create(this);
  swing->create(this);
  reachForBodyPart->create(this);
  reachForWorld->create(this);
  placement->create(this);
  pose->create(this);
  reachReaction->create(this);
  writhe->create(this);

  new(m_apiBase) ArmAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void Arm::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool Arm::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Arm::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmData>();
  *in = savedState.getValue<ArmInputs>();
  *feedOut = savedState.getValue<ArmFeedbackOutputs>();
  *out = savedState.getValue<ArmOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Arm::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "grab";
    case 1: return "aim";
    case 2: return "hold";
    case 3: return "brace";
    case 4: return "standingSupport";
    case 5: return "holdingSupport";
    case 6: return "sittingStep";
    case 7: return "step";
    case 8: return "spin";
    case 9: return "swing";
    case 10: return "reachForBodyPart";
    case 11: return "reachForWorld";
    case 12: return "placement";
    case 13: return "pose";
    case 14: return "reachReaction";
    case 15: return "writhe";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Arm::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void Arm::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


