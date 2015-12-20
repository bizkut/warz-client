#pragma once

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

#ifndef NM_MDF_BALANCEMANAGEMENT_H
#define NM_MDF_BALANCEMANAGEMENT_H

// include definition file to create project dependency
#include "./Definition/Modules/BalanceManagement.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/DirectionRequest.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "BalanceManagementData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class StaticBalance;
class SteppingBalance;
class BalancePoser;
class MyNetwork;
struct BalanceManagementAPIBase;
struct BalanceManagementUpdatePackage;
struct BalanceManagementFeedbackPackage;

class BalanceManagement : public ER::Module
{
public:

  BalanceManagement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~BalanceManagement();

  // No update() api is required for this module.
  // No feedback() api is required for this module.

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "BalanceManagement"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  BalanceManagementFeedbackInputs* feedIn;
  BalanceManagementInputs* in;
  BalanceManagementOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  StaticBalance* staticBalance;
  SteppingBalance* steppingBalance;
  BalancePoser* balancePoser;

  // child module access
  ER::Module* m_childModules[3];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 3 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 3; }

  // Module packaging
  BalanceManagementAPIBase* m_apiBase;
  BalanceManagementUpdatePackage* m_updatePackage;
  BalanceManagementFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BalanceManagement), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class BalanceManagement_Con : public ER::ModuleCon
{
public:

  virtual ~BalanceManagement_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BalanceManagement* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_feedIn_impendingInstability, 
      *junc_feedIn_supportAmount, 
      *junc_in_supportWithArms, 
      *junc_in_supportWithLegs, 
      *junc_pelvisRotReq, 
      *junc_feedIn_avgReachForBodyPartPTR, 
      *junc_feedIn_avgReachForWorldPTR, 
      *junc_feedIn_pelvisTranslationRequest, 
      *junc_feedIn_avgReachForBodyPartPDR, 
      *junc_feedIn_avgReachForWorldPDR, 
      *junc_feedIn_pelvisDirectionRequest, 
      *junc_feedIn_avgReachForBodyPartCDR, 
      *junc_feedIn_avgReachForWorldCDR, 
      *junc_feedIn_chestDirectionRequest, 
      *junc_feedIn_isStepping, 
      *junc_in_pelvisRelSupport, 
      *junc_out_headRelChestOrientation_0;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEMANAGEMENT_H

