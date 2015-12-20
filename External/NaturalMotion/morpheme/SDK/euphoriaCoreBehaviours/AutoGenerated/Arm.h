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

#ifndef NM_MDF_ARM_H
#define NM_MDF_ARM_H

// include definition file to create project dependency
#include "./Definition/Modules/Arm.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erDimensionalScaling.h"

// known types
#include "Types/LimbControl.h"
#include "Types/EndConstraintControl.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "ArmData.h"

namespace MR { class InstanceDebugInterface; } 

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class ArmGrab;
class ArmAim;
class ArmHold;
class ArmBrace;
class ArmStandingSupport;
class ArmHoldingSupport;
class ArmSittingStep;
class ArmStep;
class ArmSpin;
class ArmSwing;
class ArmReachForBodyPart;
class ArmReachForWorld;
class ArmPlacement;
class ArmPose;
class ArmReachReaction;
class ArmWrithe;
class MyNetwork;
struct ArmAPIBase;
struct ArmUpdatePackage;
struct ArmFeedbackPackage;

class Arm : public ER::Module
{
public:

  Arm(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~Arm();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "Arm"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  ArmData* data;
  ArmInputs* in;
  ArmFeedbackOutputs* feedOut;
  ArmOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  ArmGrab* grab;
  ArmAim* aim;
  ArmHold* hold;
  ArmBrace* brace;
  ArmStandingSupport* standingSupport;
  ArmHoldingSupport* holdingSupport;
  ArmSittingStep* sittingStep;
  ArmStep* step;
  ArmSpin* spin;
  ArmSwing* swing;
  ArmReachForBodyPart* reachForBodyPart;
  ArmReachForWorld* reachForWorld;
  ArmPlacement* placement;
  ArmPose* pose;
  ArmReachReaction* reachReaction;
  ArmWrithe* writhe;

  // child module access
  ER::Module* m_childModules[16];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 16 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 16; }

  // Module packaging
  ArmAPIBase* m_apiBase;
  ArmUpdatePackage* m_updatePackage;
  ArmFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(Arm), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class Arm_Con : public ER::ModuleCon
{
public:

  virtual ~Arm_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(Arm* module);
  void relocate();
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_wta01, 
      *junc_wtaBracePlacementSitting, 
      *junc_wtaSittingStepStandingSupport, 
      *junc_out_control, 
      *junc_out_wristLimitReduction, 
      *junc_feedOut_supportedByConstraint, 
      *junc_in_limbStiffnessReduction, 
      *junc_in_limbControlCompensationReduction, 
      *junc_in_limbDampingRatioReduction, 
      *junc_in_centreOfShoulders, 
      *junc_out_endConstraint, 
      *junc_out_holdBreakageOpportunity, 
      *junc_in_shouldBrace, 
      *junc_in_isBodyBarFeetContacting;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARM_H

