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

#ifndef NM_MDF_LEGSTEP_H
#define NM_MDF_LEGSTEP_H

// include definition file to create project dependency
#include "./Definition/Modules/LegStep.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/StepParameters.h"
#include "Types/LimbControl.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "LegStepData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class Leg;
struct LegStepAPIBase;
struct LegStepUpdatePackage;
struct LegStepFeedbackPackage;

class LegStep : public ER::Module
{
public:

  LegStep(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~LegStep();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "LegStep"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  LegStepData* data;
  LegStepInputs* in;
  LegStepFeedbackOutputs* feedOut;
  LegStepOutputs* out;

  // owner access
  const Leg* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  LegStepAPIBase* m_apiBase;
  LegStepUpdatePackage* m_updatePackage;
  LegStepFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(LegStep), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class LegStep_Con : public ER::ModuleCon
{
public:

  virtual ~LegStep_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(LegStep* module);
  void relocate();
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_enableStep, 
      *junc_in_stepParameters, 
      *junc_in_averageEndContactNormal, 
      *junc_wtaAllowStep, 
      *junc_in_recoveryStepTarget, 
      *junc_in_evaluateStep, 
      *junc_in_supportTM, 
      *junc_in_groundVelocity, 
      *junc_in_standingPoseEndRelativeToSupport_0, 
      *junc_in_standingPoseEndRelativeToSupport_1, 
      *junc_in_averageStandingPoseEndRelativeToSupport;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSTEP_H

