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
#include "Head.h"
#include "HeadPackaging.h"
#include "HeadPoint.h"
#include "HeadDodge.h"
#include "HeadAvoid.h"
#include "HeadPose.h"
#include "HeadSupport.h"
#include "HeadAim.h"
#include "CharacteristicsBehaviourInterface.h"
#include "BodySection.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "HeadEyes.h"

// external types
#include "NMPlatform/NMVector3.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
Head::Head(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (HeadData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(HeadData), __alignof(HeadData));
  in = (HeadInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(HeadInputs), __alignof(HeadInputs));
  feedOut = (HeadFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(HeadFeedbackOutputs), __alignof(HeadFeedbackOutputs));
  out = (HeadOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(HeadOutputs), __alignof(HeadOutputs));

  HeadEyes_Con* con0 = (HeadEyes_Con*)NMPMemoryAllocateFromFormat(HeadEyes_Con::getMemoryRequirements()).ptr;
  new(con0) HeadEyes_Con(); // set up vtable
  m_childModules[0] = eyes = (HeadEyes*)NMPMemoryAllocateFromFormat(HeadEyes::getMemoryRequirements()).ptr;
  new (eyes) HeadEyes(mdAllocator, con0);

  HeadAvoid_Con* con1 = (HeadAvoid_Con*)NMPMemoryAllocateFromFormat(HeadAvoid_Con::getMemoryRequirements()).ptr;
  new(con1) HeadAvoid_Con(); // set up vtable
  m_childModules[1] = avoid = (HeadAvoid*)NMPMemoryAllocateFromFormat(HeadAvoid::getMemoryRequirements()).ptr;
  new (avoid) HeadAvoid(mdAllocator, con1);

  HeadDodge_Con* con2 = (HeadDodge_Con*)NMPMemoryAllocateFromFormat(HeadDodge_Con::getMemoryRequirements()).ptr;
  new(con2) HeadDodge_Con(); // set up vtable
  m_childModules[2] = dodge = (HeadDodge*)NMPMemoryAllocateFromFormat(HeadDodge::getMemoryRequirements()).ptr;
  new (dodge) HeadDodge(mdAllocator, con2);

  HeadAim_Con* con3 = (HeadAim_Con*)NMPMemoryAllocateFromFormat(HeadAim_Con::getMemoryRequirements()).ptr;
  new(con3) HeadAim_Con(); // set up vtable
  m_childModules[3] = aim = (HeadAim*)NMPMemoryAllocateFromFormat(HeadAim::getMemoryRequirements()).ptr;
  new (aim) HeadAim(mdAllocator, con3);

  HeadPoint_Con* con4 = (HeadPoint_Con*)NMPMemoryAllocateFromFormat(HeadPoint_Con::getMemoryRequirements()).ptr;
  new(con4) HeadPoint_Con(); // set up vtable
  m_childModules[4] = point = (HeadPoint*)NMPMemoryAllocateFromFormat(HeadPoint::getMemoryRequirements()).ptr;
  new (point) HeadPoint(mdAllocator, con4);

  HeadSupport_Con* con5 = (HeadSupport_Con*)NMPMemoryAllocateFromFormat(HeadSupport_Con::getMemoryRequirements()).ptr;
  new(con5) HeadSupport_Con(); // set up vtable
  m_childModules[5] = support = (HeadSupport*)NMPMemoryAllocateFromFormat(HeadSupport::getMemoryRequirements()).ptr;
  new (support) HeadSupport(mdAllocator, con5);

  HeadPose_Con* con6 = (HeadPose_Con*)NMPMemoryAllocateFromFormat(HeadPose_Con::getMemoryRequirements()).ptr;
  new(con6) HeadPose_Con(); // set up vtable
  m_childModules[6] = pose = (HeadPose*)NMPMemoryAllocateFromFormat(HeadPose::getMemoryRequirements()).ptr;
  new (pose) HeadPose(mdAllocator, con6);

  m_apiBase = (HeadAPIBase*)NMPMemoryAllocAligned(sizeof(HeadAPIBase), 16);
  m_updatePackage = (HeadUpdatePackage*)NMPMemoryAllocAligned(sizeof(HeadUpdatePackage), 16);
  m_feedbackPackage = (HeadFeedbackPackage*)NMPMemoryAllocAligned(sizeof(HeadFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
Head::~Head()
{
  ER::ModuleCon* poseConnections = pose->getConnections();
  if (poseConnections)
  {
    poseConnections->~ModuleCon();
    NMP::Memory::memFree(poseConnections);
  }
  pose->~HeadPose();
  NMP::Memory::memFree(pose);

  ER::ModuleCon* supportConnections = support->getConnections();
  if (supportConnections)
  {
    supportConnections->~ModuleCon();
    NMP::Memory::memFree(supportConnections);
  }
  support->~HeadSupport();
  NMP::Memory::memFree(support);

  ER::ModuleCon* pointConnections = point->getConnections();
  if (pointConnections)
  {
    pointConnections->~ModuleCon();
    NMP::Memory::memFree(pointConnections);
  }
  point->~HeadPoint();
  NMP::Memory::memFree(point);

  ER::ModuleCon* aimConnections = aim->getConnections();
  if (aimConnections)
  {
    aimConnections->~ModuleCon();
    NMP::Memory::memFree(aimConnections);
  }
  aim->~HeadAim();
  NMP::Memory::memFree(aim);

  ER::ModuleCon* dodgeConnections = dodge->getConnections();
  if (dodgeConnections)
  {
    dodgeConnections->~ModuleCon();
    NMP::Memory::memFree(dodgeConnections);
  }
  dodge->~HeadDodge();
  NMP::Memory::memFree(dodge);

  ER::ModuleCon* avoidConnections = avoid->getConnections();
  if (avoidConnections)
  {
    avoidConnections->~ModuleCon();
    NMP::Memory::memFree(avoidConnections);
  }
  avoid->~HeadAvoid();
  NMP::Memory::memFree(avoid);

  ER::ModuleCon* eyesConnections = eyes->getConnections();
  if (eyesConnections)
  {
    eyesConnections->~ModuleCon();
    NMP::Memory::memFree(eyesConnections);
  }
  eyes->~HeadEyes();
  NMP::Memory::memFree(eyes);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void Head::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  eyes->create(this);
  avoid->create(this);
  dodge->create(this);
  aim->create(this);
  point->create(this);
  support->create(this);
  pose->create(this);

  new(m_apiBase) HeadAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) HeadUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) HeadFeedbackPackage(
    data, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void Head::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool Head::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Head::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<HeadData>();
  *in = savedState.getValue<HeadInputs>();
  *feedOut = savedState.getValue<HeadFeedbackOutputs>();
  *out = savedState.getValue<HeadOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Head::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "eyes";
    case 1: return "avoid";
    case 2: return "dodge";
    case 3: return "aim";
    case 4: return "point";
    case 5: return "support";
    case 6: return "pose";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Head::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void Head::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


