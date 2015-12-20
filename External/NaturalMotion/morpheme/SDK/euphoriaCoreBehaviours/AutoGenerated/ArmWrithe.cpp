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
#include "ArmWrithe.h"
#include "ArmWrithePackaging.h"
#include "MyNetwork.h"
#include "WritheBehaviourInterface.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
ArmWrithe::ArmWrithe(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (ArmWritheData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(ArmWritheData), __alignof(ArmWritheData));
  in = (ArmWritheInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(ArmWritheInputs), __alignof(ArmWritheInputs));
  out = (ArmWritheOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(ArmWritheOutputs), __alignof(ArmWritheOutputs));

  m_apiBase = (ArmWritheAPIBase*)NMPMemoryAllocAligned(sizeof(ArmWritheAPIBase), 16);
  m_updatePackage = (ArmWritheUpdatePackage*)NMPMemoryAllocAligned(sizeof(ArmWritheUpdatePackage), 16);
  m_feedbackPackage = (ArmWritheFeedbackPackage*)NMPMemoryAllocAligned(sizeof(ArmWritheFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
ArmWrithe::~ArmWrithe()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmWrithe::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Arm*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) ArmWritheAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) ArmWritheUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) ArmWritheFeedbackPackage(
    data, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void ArmWrithe::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmWrithe::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ArmWrithe::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<ArmWritheData>();
  *in = savedState.getValue<ArmWritheInputs>();
  *out = savedState.getValue<ArmWritheOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ArmWrithe::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmWrithe::update(float timeStep)
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


