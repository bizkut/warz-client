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
#include "Leg.h"
#include "LegSwing.h"
#include "LegSwingPackaging.h"
#include "MyNetwork.h"
#include "BodyFrame.h"
#include "SitBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "LegsPedalBehaviourInterface.h"
#include "BodySection.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
LegSwing::LegSwing(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (LegSwingData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(LegSwingData), __alignof(LegSwingData));
  in = (LegSwingInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(LegSwingInputs), __alignof(LegSwingInputs));
  out = (LegSwingOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(LegSwingOutputs), __alignof(LegSwingOutputs));

  m_apiBase = (LegSwingAPIBase*)NMPMemoryAllocAligned(sizeof(LegSwingAPIBase), 16);
  m_updatePackage = (LegSwingUpdatePackage*)NMPMemoryAllocAligned(sizeof(LegSwingUpdatePackage), 16);
  m_feedbackPackage = (LegSwingFeedbackPackage*)NMPMemoryAllocAligned(sizeof(LegSwingFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
LegSwing::~LegSwing()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void LegSwing::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Leg*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) LegSwingAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) LegSwingUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) LegSwingFeedbackPackage(
    data, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void LegSwing::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool LegSwing::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegSwing::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<LegSwingData>();
  *in = savedState.getValue<LegSwingInputs>();
  *out = savedState.getValue<LegSwingOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LegSwing::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void LegSwing::update(float timeStep)
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


