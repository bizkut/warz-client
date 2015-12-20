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

#ifndef NM_MDF_SITTINGBODYBALANCE_H
#define NM_MDF_SITTINGBODYBALANCE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/SittingBodyBalance.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMQuat.h"

// known types
#include "Types/SitParameters.h"
#include "Types/Environment_SupportPoly.h"
#include "Types/LimbControl.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "SittingBodyBalanceData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class BodyFrame;
struct SittingBodyBalanceAPIBase;
struct SittingBodyBalanceUpdatePackage;
struct SittingBodyBalanceFeedbackPackage;

class SittingBodyBalance : public ER::Module
{
public:

  SittingBodyBalance(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~SittingBodyBalance();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "SittingBodyBalance"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  SittingBodyBalanceData* data;
  SittingBodyBalanceFeedbackInputs* feedIn;
  SittingBodyBalanceInputs* in;
  SittingBodyBalanceFeedbackOutputs* feedOut;
  SittingBodyBalanceOutputs* out;

  // owner access
  const BodyFrame* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  SittingBodyBalanceAPIBase* m_apiBase;
  SittingBodyBalanceUpdatePackage* m_updatePackage;
  SittingBodyBalanceFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(SittingBodyBalance), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class SittingBodyBalance_Con : public ER::ModuleCon
{
public:

  virtual ~SittingBodyBalance_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(SittingBodyBalance* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_armPoseEndRelativeToRoot_0, 
      *junc_in_armPoseEndRelativeToRoot_1, 
      *junc_in_legPoseEndRelativeToRoot_0, 
      *junc_in_legPoseEndRelativeToRoot_1, 
      *junc_in_spinePoseEndRelativeToRoot_0, 
      *junc_in_headPoseEndRelativeToRoot_0, 
      *junc_in_sitParameters, 
      *junc_in_supportPolygon, 
      *junc_in_supportWithArms, 
      *junc_in_supportWithLegs, 
      *junc_feedIn_chestOnGroundAmount, 
      *junc_feedIn_pelvisOnGroundAmount, 
      *junc_feedIn_handOnGroundAmount_0, 
      *junc_feedIn_handOnGroundAmount_1, 
      *junc_feedIn_handOnGroundCollidingTime_0, 
      *junc_feedIn_handOnGroundCollidingTime_1, 
      *junc_feedIn_standingBalanceAmount;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SITTINGBODYBALANCE_H

