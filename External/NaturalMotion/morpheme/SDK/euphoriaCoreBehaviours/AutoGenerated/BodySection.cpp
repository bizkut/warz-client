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
#include "BodySection.h"
#include "BodySectionPackaging.h"
#include "PositionCore.h"
#include "RotateCore.h"
#include "Leg.h"
#include "LegBrace.h"
#include "Spine.h"
#include "LegStandingSupport.h"
#include "BodyFrame.h"
#include "HazardManagement.h"
#include "Grab.h"
#include "LegSpin.h"
#include "FreeFallManagement.h"
#include "Head.h"
#include "HeadPoint.h"
#include "HeadAvoid.h"
#include "HeadDodge.h"
#include "Arm.h"
#include "ArmBrace.h"
#include "ArmReachForWorld.h"
#include "ArmHoldingSupport.h"
#include "ArmReachForBodyPart.h"
#include "ArmStandingSupport.h"
#include "ArmSpin.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "BraceChooser.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BodySection::BodySection(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (BodySectionFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BodySectionFeedbackInputs), __alignof(BodySectionFeedbackInputs));
  in = (BodySectionInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BodySectionInputs), __alignof(BodySectionInputs));
  feedOut = (BodySectionFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(BodySectionFeedbackOutputs), __alignof(BodySectionFeedbackOutputs));
  out = (BodySectionOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BodySectionOutputs), __alignof(BodySectionOutputs));

  RotateCore_Con* con0 = (RotateCore_Con*)NMPMemoryAllocateFromFormat(RotateCore_Con::getMemoryRequirements()).ptr;
  new(con0) RotateCore_Con(); // set up vtable
  m_childModules[0] = rotate = (RotateCore*)NMPMemoryAllocateFromFormat(RotateCore::getMemoryRequirements()).ptr;
  new (rotate) RotateCore(mdAllocator, con0);

  PositionCore_Con* con1 = (PositionCore_Con*)NMPMemoryAllocateFromFormat(PositionCore_Con::getMemoryRequirements()).ptr;
  new(con1) PositionCore_Con(); // set up vtable
  m_childModules[1] = position = (PositionCore*)NMPMemoryAllocateFromFormat(PositionCore::getMemoryRequirements()).ptr;
  new (position) PositionCore(mdAllocator, con1);

  BraceChooser_Con* con2 = (BraceChooser_Con*)NMPMemoryAllocateFromFormat(BraceChooser_Con::getMemoryRequirements()).ptr;
  new(con2) BraceChooser_Con(); // set up vtable
  m_childModules[2] = braceChooser = (BraceChooser*)NMPMemoryAllocateFromFormat(BraceChooser::getMemoryRequirements()).ptr;
  new (braceChooser) BraceChooser(mdAllocator, con2);

  m_apiBase = (BodySectionAPIBase*)NMPMemoryAllocAligned(sizeof(BodySectionAPIBase), 16);
  m_updatePackage = (BodySectionUpdatePackage*)NMPMemoryAllocAligned(sizeof(BodySectionUpdatePackage), 16);
  m_feedbackPackage = (BodySectionFeedbackPackage*)NMPMemoryAllocAligned(sizeof(BodySectionFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BodySection::~BodySection()
{
  ER::ModuleCon* braceChooserConnections = braceChooser->getConnections();
  if (braceChooserConnections)
  {
    braceChooserConnections->~ModuleCon();
    NMP::Memory::memFree(braceChooserConnections);
  }
  braceChooser->~BraceChooser();
  NMP::Memory::memFree(braceChooser);

  ER::ModuleCon* positionConnections = position->getConnections();
  if (positionConnections)
  {
    positionConnections->~ModuleCon();
    NMP::Memory::memFree(positionConnections);
  }
  position->~PositionCore();
  NMP::Memory::memFree(position);

  ER::ModuleCon* rotateConnections = rotate->getConnections();
  if (rotateConnections)
  {
    rotateConnections->~ModuleCon();
    NMP::Memory::memFree(rotateConnections);
  }
  rotate->~RotateCore();
  NMP::Memory::memFree(rotate);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void BodySection::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  rotate->create(this);
  position->create(this);
  braceChooser->create(this);

  new(m_apiBase) BodySectionAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) BodySectionUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) BodySectionFeedbackPackage(
    feedIn, in, feedOut, 
    owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void BodySection::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BodySection::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BodySection::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<BodySectionFeedbackInputs>();
  *in = savedState.getValue<BodySectionInputs>();
  *feedOut = savedState.getValue<BodySectionFeedbackOutputs>();
  *out = savedState.getValue<BodySectionOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodySection::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "rotate";
    case 1: return "position";
    case 2: return "braceChooser";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// No update() api required
//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


