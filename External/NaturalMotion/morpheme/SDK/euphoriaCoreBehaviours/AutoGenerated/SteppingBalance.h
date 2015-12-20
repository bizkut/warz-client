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

#ifndef NM_MDF_STEPPINGBALANCE_H
#define NM_MDF_STEPPINGBALANCE_H

// include definition file to create project dependency
#include "./Definition/Modules/SteppingBalance.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/SteppingBalanceParameters.h"
#include "Types/BodyState.h"
#include "Types/TargetRequest.h"
#include "Types/PoseData.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "SteppingBalanceData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class BalanceManagement;
struct SteppingBalanceAPIBase;
struct SteppingBalanceUpdatePackage;
struct SteppingBalanceFeedbackPackage;

class SteppingBalance : public ER::Module
{
public:

  SteppingBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~SteppingBalance();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "SteppingBalance"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  SteppingBalanceData* data;
  SteppingBalanceFeedbackInputs* feedIn;
  SteppingBalanceInputs* in;
  SteppingBalanceFeedbackOutputs* feedOut;
  SteppingBalanceOutputs* out;

  // owner access
  const BalanceManagement* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  SteppingBalanceAPIBase* m_apiBase;
  SteppingBalanceUpdatePackage* m_updatePackage;
  SteppingBalanceFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(SteppingBalance), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class SteppingBalance_Con : public ER::ModuleCon
{
public:

  virtual ~SteppingBalance_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(SteppingBalance* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_steppingBalanceParameters, 
      *junc_in_armReadyPoseEndRelativeToRoot_0, 
      *junc_in_armReadyPoseEndRelativeToRoot_1, 
      *junc_in_armBalancePoseEndRelativeToRoot_0, 
      *junc_in_armBalancePoseEndRelativeToRoot_1, 
      *junc_junc_FeedbackInputs_isLegStepping_0, 
      *junc_junc_FeedbackInputs_isLegStepping_1, 
      *junc_junc_FeedbackInputs_stepDir_0, 
      *junc_junc_FeedbackInputs_stepDir_1, 
      *junc_feedIn_isDoingRecoveryStep, 
      *junc_feedIn_centreOfMassBodyState, 
      *junc_feedIn_groundVel;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_STEPPINGBALANCE_H

