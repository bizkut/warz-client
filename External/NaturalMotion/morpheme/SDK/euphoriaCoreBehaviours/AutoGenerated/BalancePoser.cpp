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
#include "BalanceManagement.h"
#include "BalancePoser.h"
#include "BalancePoserPackaging.h"
#include "MyNetwork.h"
#include "BalancePoserBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "StaticBalance.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BalancePoser::BalancePoser(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  in = (BalancePoserInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BalancePoserInputs), __alignof(BalancePoserInputs));
  out = (BalancePoserOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BalancePoserOutputs), __alignof(BalancePoserOutputs));

  m_apiBase = (BalancePoserAPIBase*)NMPMemoryAllocAligned(sizeof(BalancePoserAPIBase), 16);
  m_updatePackage = (BalancePoserUpdatePackage*)NMPMemoryAllocAligned(sizeof(BalancePoserUpdatePackage), 16);
  m_feedbackPackage = (BalancePoserFeedbackPackage*)NMPMemoryAllocAligned(sizeof(BalancePoserFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BalancePoser::~BalancePoser()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoser::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (BalanceManagement*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) BalancePoserAPIBase( in, owner->m_apiBase);
  new(m_updatePackage) BalancePoserUpdatePackage(
    in, out, owner->m_apiBase  );
  new(m_feedbackPackage) BalancePoserFeedbackPackage(
    in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoser::clearAllData()
{
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BalancePoser::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BalancePoser::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *in = savedState.getValue<BalancePoserInputs>();
  *out = savedState.getValue<BalancePoserOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BalancePoser::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoser::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


