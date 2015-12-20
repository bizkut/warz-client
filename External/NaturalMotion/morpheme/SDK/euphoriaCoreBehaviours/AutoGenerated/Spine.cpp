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
#include "Spine.h"
#include "SpinePackaging.h"
#include "SpinePose.h"
#include "SpineControl.h"
#include "SpineAim.h"
#include "SpineReachReaction.h"
#include "SpineWrithe.h"
#include "BodySection.h"
#include "HazardManagement.h"
#include "ShieldManagement.h"
#include "BodyFrame.h"
#include "BodyBalance.h"
#include "CharacteristicsBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "SpineSupport.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
Spine::Spine(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (SpineData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(SpineData), __alignof(SpineData));
  in = (SpineInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpineInputs), __alignof(SpineInputs));
  feedOut = (SpineFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(SpineFeedbackOutputs), __alignof(SpineFeedbackOutputs));
  out = (SpineOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpineOutputs), __alignof(SpineOutputs));

  SpineSupport_Con* con0 = (SpineSupport_Con*)NMPMemoryAllocateFromFormat(SpineSupport_Con::getMemoryRequirements()).ptr;
  new(con0) SpineSupport_Con(); // set up vtable
  m_childModules[0] = support = (SpineSupport*)NMPMemoryAllocateFromFormat(SpineSupport::getMemoryRequirements()).ptr;
  new (support) SpineSupport(mdAllocator, con0);

  SpinePose_Con* con1 = (SpinePose_Con*)NMPMemoryAllocateFromFormat(SpinePose_Con::getMemoryRequirements()).ptr;
  new(con1) SpinePose_Con(); // set up vtable
  m_childModules[1] = pose = (SpinePose*)NMPMemoryAllocateFromFormat(SpinePose::getMemoryRequirements()).ptr;
  new (pose) SpinePose(mdAllocator, con1);

  SpineControl_Con* con2 = (SpineControl_Con*)NMPMemoryAllocateFromFormat(SpineControl_Con::getMemoryRequirements()).ptr;
  new(con2) SpineControl_Con(); // set up vtable
  m_childModules[2] = control = (SpineControl*)NMPMemoryAllocateFromFormat(SpineControl::getMemoryRequirements()).ptr;
  new (control) SpineControl(mdAllocator, con2);

  SpineReachReaction_Con* con3 = (SpineReachReaction_Con*)NMPMemoryAllocateFromFormat(SpineReachReaction_Con::getMemoryRequirements()).ptr;
  new(con3) SpineReachReaction_Con(); // set up vtable
  m_childModules[3] = reachReaction = (SpineReachReaction*)NMPMemoryAllocateFromFormat(SpineReachReaction::getMemoryRequirements()).ptr;
  new (reachReaction) SpineReachReaction(mdAllocator, con3);

  SpineWrithe_Con* con4 = (SpineWrithe_Con*)NMPMemoryAllocateFromFormat(SpineWrithe_Con::getMemoryRequirements()).ptr;
  new(con4) SpineWrithe_Con(); // set up vtable
  m_childModules[4] = writhe = (SpineWrithe*)NMPMemoryAllocateFromFormat(SpineWrithe::getMemoryRequirements()).ptr;
  new (writhe) SpineWrithe(mdAllocator, con4);

  SpineAim_Con* con5 = (SpineAim_Con*)NMPMemoryAllocateFromFormat(SpineAim_Con::getMemoryRequirements()).ptr;
  new(con5) SpineAim_Con(); // set up vtable
  m_childModules[5] = aim = (SpineAim*)NMPMemoryAllocateFromFormat(SpineAim::getMemoryRequirements()).ptr;
  new (aim) SpineAim(mdAllocator, con5);

  m_apiBase = (SpineAPIBase*)NMPMemoryAllocAligned(sizeof(SpineAPIBase), 16);
  m_updatePackage = (SpineUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpineUpdatePackage), 16);
  m_feedbackPackage = (SpineFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpineFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
Spine::~Spine()
{
  ER::ModuleCon* aimConnections = aim->getConnections();
  if (aimConnections)
  {
    aimConnections->~ModuleCon();
    NMP::Memory::memFree(aimConnections);
  }
  aim->~SpineAim();
  NMP::Memory::memFree(aim);

  ER::ModuleCon* writheConnections = writhe->getConnections();
  if (writheConnections)
  {
    writheConnections->~ModuleCon();
    NMP::Memory::memFree(writheConnections);
  }
  writhe->~SpineWrithe();
  NMP::Memory::memFree(writhe);

  ER::ModuleCon* reachReactionConnections = reachReaction->getConnections();
  if (reachReactionConnections)
  {
    reachReactionConnections->~ModuleCon();
    NMP::Memory::memFree(reachReactionConnections);
  }
  reachReaction->~SpineReachReaction();
  NMP::Memory::memFree(reachReaction);

  ER::ModuleCon* controlConnections = control->getConnections();
  if (controlConnections)
  {
    controlConnections->~ModuleCon();
    NMP::Memory::memFree(controlConnections);
  }
  control->~SpineControl();
  NMP::Memory::memFree(control);

  ER::ModuleCon* poseConnections = pose->getConnections();
  if (poseConnections)
  {
    poseConnections->~ModuleCon();
    NMP::Memory::memFree(poseConnections);
  }
  pose->~SpinePose();
  NMP::Memory::memFree(pose);

  ER::ModuleCon* supportConnections = support->getConnections();
  if (supportConnections)
  {
    supportConnections->~ModuleCon();
    NMP::Memory::memFree(supportConnections);
  }
  support->~SpineSupport();
  NMP::Memory::memFree(support);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void Spine::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  support->create(this);
  pose->create(this);
  control->create(this);
  reachReaction->create(this);
  writhe->create(this);
  aim->create(this);

  new(m_apiBase) SpineAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) SpineUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) SpineFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void Spine::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool Spine::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Spine::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<SpineData>();
  *in = savedState.getValue<SpineInputs>();
  *feedOut = savedState.getValue<SpineFeedbackOutputs>();
  *out = savedState.getValue<SpineOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Spine::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "support";
    case 1: return "pose";
    case 2: return "control";
    case 3: return "reachReaction";
    case 4: return "writhe";
    case 5: return "aim";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Spine::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void Spine::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


