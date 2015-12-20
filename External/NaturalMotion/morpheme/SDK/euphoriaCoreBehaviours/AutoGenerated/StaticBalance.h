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

#ifndef NM_MDF_STATICBALANCE_H
#define NM_MDF_STATICBALANCE_H

// include definition file to create project dependency
#include "./Definition/Modules/StaticBalance.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/DirectionRequest.h"
#include "Types/TranslationRequest.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "StaticBalanceData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class BalanceManagement;
struct StaticBalanceAPIBase;
struct StaticBalanceUpdatePackage;
struct StaticBalanceFeedbackPackage;

class StaticBalance : public ER::Module
{
public:

  StaticBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~StaticBalance();

  void update(float timeStep);
  // No feedback() api is required for this module.

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "StaticBalance"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  StaticBalanceFeedbackInputs* feedIn;
  StaticBalanceInputs* in;
  StaticBalanceOutputs* out;

  // owner access
  const BalanceManagement* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  StaticBalanceAPIBase* m_apiBase;
  StaticBalanceUpdatePackage* m_updatePackage;
  StaticBalanceFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(StaticBalance), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class StaticBalance_Con : public ER::ModuleCon
{
public:

  virtual ~StaticBalance_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(StaticBalance* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_headPoseEndRelativeToRoot_0, 
      *junc_in_spinePoseEndRelativeToRoot_0, 
      *junc_in_legPoseEndRelativeToRoot_0, 
      *junc_in_legPoseEndRelativeToRoot_1, 
      *junc_in_armPoseEndRelativeToRoot_0, 
      *junc_in_armPoseEndRelativeToRoot_1, 
      *junc_in_balanceUsingAnimationHorizontalPelvisOffsetAmount, 
      *junc_in_balanceOffsetFwd, 
      *junc_in_balanceOffsetRight, 
      *junc_in_targetPelvisOrientation, 
      *junc_in_targetPelvisDirection, 
      *junc_in_stabilisingCrouchDownAmount, 
      *junc_in_stabilisingCrouchPitchAmount, 
      *junc_feedIn_supportTM, 
      *junc_feedIn_currHeadEndPositions_0, 
      *junc_out_averageAverageLegPoseEndRelSupport;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_STATICBALANCE_H

