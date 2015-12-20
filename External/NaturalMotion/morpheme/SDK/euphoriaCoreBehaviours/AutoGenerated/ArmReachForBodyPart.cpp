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
#include "Arm.h"
#include "ArmReachForBodyPart.h"
#include "ArmReachForBodyPartPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "BodyBalance.h"
#include "ArmReachReaction.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmReachForBodyPart::ArmReachForBodyPart(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmReachForBodyPartData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmReachForBodyPartData), __alignof(ArmReachForBodyPartData));
  feedIn = (ArmReachForBodyPartFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(ArmReachForBodyPartFeedbackInputs), __alignof(ArmReachForBodyPartFeedbackInputs));
  in = (ArmReachForBodyPartInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmReachForBodyPartInputs), __alignof(ArmReachForBodyPartInputs));
  feedOut = (ArmReachForBodyPartFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(ArmReachForBodyPartFeedbackOutputs), __alignof(ArmReachForBodyPartFeedbackOutputs));
  out = (ArmReachForBodyPartOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmReachForBodyPartOutputs), __alignof(ArmReachForBodyPartOutputs));

  m_apiBase = (ArmReachForBodyPartAPIBase*)NMPMemoryAllocAligned(sizeof(ArmReachForBodyPartAPIBase), 16);
  m_updatePackage = (ArmReachForBodyPartUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmReachForBodyPartUpdatePackage), 16);
  m_feedbackPackage = (ArmReachForBodyPartFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmReachForBodyPartFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmReachForBodyPart::~ArmReachForBodyPart()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmReachForBodyPartAPIBase( data, in, feedIn, owner->m_apiBase);
  new(m_updatePackage) ArmReachForBodyPartUpdatePackage(
    data, in, feedIn, 
    out, owner->m_apiBase  );
  new(m_feedbackPackage) ArmReachForBodyPartFeedbackPackage(
    data, feedIn, in, 
    feedOut, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
  feedIn->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmReachForBodyPart::storeState(MR::PhysicsSerialisationBuffer& savedState)
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
bool ArmReachForBodyPart::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmReachForBodyPartData>();
  *feedIn = savedState.getValue<ArmReachForBodyPartFeedbackInputs>();
  *in = savedState.getValue<ArmReachForBodyPartInputs>();
  *feedOut = savedState.getValue<ArmReachForBodyPartFeedbackOutputs>();
  *out = savedState.getValue<ArmReachForBodyPartOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmReachForBodyPart::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart::feedback(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_feedbackPackage->feedback(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


