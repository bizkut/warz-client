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
#include "ArmPlacement.h"
#include "ArmPlacementPackaging.h"
#include "MyNetwork.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "ArmsPlacementBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmPlacement::ArmPlacement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmPlacementData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmPlacementData), __alignof(ArmPlacementData));
  in = (ArmPlacementInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmPlacementInputs), __alignof(ArmPlacementInputs));
  out = (ArmPlacementOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmPlacementOutputs), __alignof(ArmPlacementOutputs));

  m_apiBase = (ArmPlacementAPIBase*)NMPMemoryAllocAligned(sizeof(ArmPlacementAPIBase), 16);
  m_updatePackage = (ArmPlacementUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmPlacementUpdatePackage), 16);
  m_feedbackPackage = (ArmPlacementFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmPlacementFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmPlacement::~ArmPlacement()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmPlacementAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmPlacementUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmPlacementFeedbackPackage(
    data, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmPlacement::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmPlacement::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmPlacementData>();
  *in = savedState.getValue<ArmPlacementInputs>();
  *out = savedState.getValue<ArmPlacementOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmPlacement::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement::update(float timeStep)
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


