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
#include "BalanceManagement.h"
#include "BalanceManagementPackaging.h"
#include "BodyFrame.h"
#include "BalanceBehaviourInterface.h"
#include "Arm.h"
#include "ArmReachForBodyPart.h"
#include "BodySection.h"
#include "RotateCore.h"
#include "PositionCore.h"
#include "ArmReachForWorld.h"
#include "BodyBalance.h"
#include "StaticBalance.h"
#include "BalancePoser.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "SteppingBalance.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
BalanceManagement::BalanceManagement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  feedIn = (BalanceManagementFeedbackInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackInputs, sizeof(BalanceManagementFeedbackInputs), __alignof(BalanceManagementFeedbackInputs));
  in = (BalanceManagementInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(BalanceManagementInputs), __alignof(BalanceManagementInputs));
  out = (BalanceManagementOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(BalanceManagementOutputs), __alignof(BalanceManagementOutputs));

  StaticBalance_Con* con0 = (StaticBalance_Con*)NMPMemoryAllocateFromFormat(StaticBalance_Con::getMemoryRequirements()).ptr;
  new(con0) StaticBalance_Con(); // set up vtable
  m_childModules[0] = staticBalance = (StaticBalance*)NMPMemoryAllocateFromFormat(StaticBalance::getMemoryRequirements()).ptr;
  new (staticBalance) StaticBalance(mdAllocator, con0);

  SteppingBalance_Con* con1 = (SteppingBalance_Con*)NMPMemoryAllocateFromFormat(SteppingBalance_Con::getMemoryRequirements()).ptr;
  new(con1) SteppingBalance_Con(); // set up vtable
  m_childModules[1] = steppingBalance = (SteppingBalance*)NMPMemoryAllocateFromFormat(SteppingBalance::getMemoryRequirements()).ptr;
  new (steppingBalance) SteppingBalance(mdAllocator, con1);

  BalancePoser_Con* con2 = (BalancePoser_Con*)NMPMemoryAllocateFromFormat(BalancePoser_Con::getMemoryRequirements()).ptr;
  new(con2) BalancePoser_Con(); // set up vtable
  m_childModules[2] = balancePoser = (BalancePoser*)NMPMemoryAllocateFromFormat(BalancePoser::getMemoryRequirements()).ptr;
  new (balancePoser) BalancePoser(mdAllocator, con2);

  m_apiBase = (BalanceManagementAPIBase*)NMPMemoryAllocAligned(sizeof(BalanceManagementAPIBase), 16);
  m_updatePackage = (BalanceManagementUpdatePackage*)NMPMemoryAllocAligned(sizeof(BalanceManagementUpdatePackage), 16);
  m_feedbackPackage = (BalanceManagementFeedbackPackage*)NMPMemoryAllocAligned(sizeof(BalanceManagementFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
BalanceManagement::~BalanceManagement()
{
  ER::ModuleCon* balancePoserConnections = balancePoser->getConnections();
  if (balancePoserConnections)
  {
    balancePoserConnections->~ModuleCon();
    NMP::Memory::memFree(balancePoserConnections);
  }
  balancePoser->~BalancePoser();
  NMP::Memory::memFree(balancePoser);

  ER::ModuleCon* steppingBalanceConnections = steppingBalance->getConnections();
  if (steppingBalanceConnections)
  {
    steppingBalanceConnections->~ModuleCon();
    NMP::Memory::memFree(steppingBalanceConnections);
  }
  steppingBalance->~SteppingBalance();
  NMP::Memory::memFree(steppingBalance);

  ER::ModuleCon* staticBalanceConnections = staticBalance->getConnections();
  if (staticBalanceConnections)
  {
    staticBalanceConnections->~ModuleCon();
    NMP::Memory::memFree(staticBalanceConnections);
  }
  staticBalance->~StaticBalance();
  NMP::Memory::memFree(staticBalance);

  owner = 0;
  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceManagement::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);

  staticBalance->create(this);
  steppingBalance->create(this);
  balancePoser->create(this);

  new(m_apiBase) BalanceManagementAPIBase( in, feedIn, owner->m_apiBase);
  new(m_updatePackage) BalanceManagementUpdatePackage(
    in, feedIn, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) BalanceManagementFeedbackPackage(
    feedIn, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceManagement::clearAllData()
{
  in->clear();
  out->clear();
  feedIn->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceManagement::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*feedIn);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceManagement::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *feedIn = savedState.getValue<BalanceManagementFeedbackInputs>();
  *in = savedState.getValue<BalanceManagementInputs>();
  *out = savedState.getValue<BalanceManagementOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* BalanceManagement::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "staticBalance";
    case 1: return "steppingBalance";
    case 2: return "balancePoser";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// No update() api required
//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


