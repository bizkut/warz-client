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

#ifndef NM_MDF_BALANCEBEHAVIOURINTERFACE_H
#define NM_MDF_BALANCEBEHAVIOURINTERFACE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/BalanceBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/PoseData.h"
#include "Types/BalanceParameters.h"
#include "Types/BalancePoseClamping.h"
#include "Types/StepParameters.h"
#include "Types/SteppingBalanceParameters.h"
#include "Types/StepRecoveryParameters.h"
#include "Types/BalanceAssistanceParameters.h"
#include "Types/BodyState.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "BalanceBehaviourInterfaceData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class MyNetwork;
struct BalanceBehaviourInterfaceAPIBase;

class BalanceBehaviourInterface : public ER::Module
{
public:

  BalanceBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~BalanceBehaviourInterface();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "BalanceBehaviourInterface"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  BalanceBehaviourInterfaceData* data;
  BalanceBehaviourInterfaceFeedbackInputs* feedIn;
  BalanceBehaviourInterfaceInputs* in;
  BalanceBehaviourInterfaceFeedbackOutputs* feedOut;
  BalanceBehaviourInterfaceOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  BalanceBehaviourInterfaceAPIBase* m_apiBase;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BalanceBehaviourInterface), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class BalanceBehaviourInterface_Con : public ER::ModuleCon
{
public:

  virtual ~BalanceBehaviourInterface_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BalanceBehaviourInterface* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_legStrengthReduction_0, 
      *junc_in_legStrengthReduction_1, 
      *junc_in_armStrengthReduction_0, 
      *junc_in_armStrengthReduction_1, 
      *junc_feedIn_supportTM, 
      *junc_feedIn_supportVelocity, 
      *junc_feedIn_balanceAmount, 
      *junc_feedIn_maxLegCollidingTime, 
      *junc_feedIn_timeSinceLastStep, 
      *junc_feedIn_stepCount, 
      *junc_feedIn_steppingTime, 
      *junc_feedIn_legStepFractions_0, 
      *junc_feedIn_legStepFractions_1, 
      *junc_feedIn_CoMBodyState, 
      *junc_out_averageArmEndRelativeToRoot, 
      *junc_out_averageLegEndRelativeToRoot;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEBEHAVIOURINTERFACE_H

