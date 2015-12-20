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

#ifndef NM_MDF_BODYBALANCE_H
#define NM_MDF_BODYBALANCE_H

// include definition file to create project dependency
#include "./Definition/Modules/BodyBalance.module"

// external types
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/BalanceParameters.h"
#include "Types/BalancePoseClamping.h"
#include "Types/StepRecoveryParameters.h"
#include "Types/Environment_SupportPoly.h"
#include "Types/RotationRequest.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "BodyBalanceData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class BodyFrame;
struct BodyBalanceAPIBase;
struct BodyBalanceUpdatePackage;
struct BodyBalanceFeedbackPackage;

class BodyBalance : public ER::Module
{
public:

  BodyBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~BodyBalance();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "BodyBalance"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  BodyBalanceData* data;
  BodyBalanceFeedbackInputs* feedIn;
  BodyBalanceInputs* in;
  BodyBalanceFeedbackOutputs* feedOut;
  BodyBalanceOutputs* out;

  // owner access
  const BodyFrame* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  BodyBalanceAPIBase* m_apiBase;
  BodyBalanceUpdatePackage* m_updatePackage;
  BodyBalanceFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BodyBalance), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class BodyBalance_Con : public ER::ModuleCon
{
public:

  virtual ~BodyBalance_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BodyBalance* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_balanceParameters, 
      *junc_in_stepRecoveryParameters, 
      *junc_in_balancePoseClamping, 
      *junc_in_pelvisRelSupport, 
      *junc_in_spinePoseEndRelativeToRoot_0, 
      *junc_in_legEndRelativeToRoot_0, 
      *junc_in_legEndRelativeToRoot_1, 
      *junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount, 
      *junc_in_supportPolygon, 
      *junc_in_enableSupportOnLegs, 
      *junc_in_enableSupportOnArms, 
      *junc_in_enableStepOnLegs, 
      *junc_in_enableStepOnArms, 
      *junc_feedIn_enableSupportOnLegs, 
      *junc_feedIn_enableSupportOnArms, 
      *junc_junc_FeedbackInputs_limbSupportAmounts_1, 
      *junc_junc_FeedbackInputs_limbSupportAmounts_0, 
      *junc_junc_FeedbackInputs_limbSupportAmounts_3, 
      *junc_junc_FeedbackInputs_limbSupportAmounts_2, 
      *junc_feedIn_isSupportedByConstraint, 
      *junc_junc_FeedbackInputs_limbEndOnGroundAmounts_3, 
      *junc_junc_FeedbackInputs_limbEndOnGroundAmounts_2, 
      *junc_junc_FeedbackInputs_limbEndOnGroundAmounts_1, 
      *junc_junc_FeedbackInputs_limbEndOnGroundAmounts_0, 
      *junc_junc_FeedbackInputs_stepLengths_2, 
      *junc_junc_FeedbackInputs_stepLengths_0, 
      *junc_junc_FeedbackInputs_stepLengths_3, 
      *junc_junc_FeedbackInputs_stepLengths_1, 
      *junc_feedIn_lowerPelvisDistance, 
      *junc_feedIn_isSteppingBeingSuppressed, 
      *junc_junc_FeedbackInputs_isStepping_0, 
      *junc_junc_FeedbackInputs_isStepping_1, 
      *junc_junc_FeedbackInputs_isStepping_2, 
      *junc_junc_FeedbackInputs_isStepping_3, 
      *junc_feedIn_isDoingRecoveryStep, 
      *junc_feedIn_stepDir, 
      *junc_feedOut_isStepping;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYBALANCE_H

