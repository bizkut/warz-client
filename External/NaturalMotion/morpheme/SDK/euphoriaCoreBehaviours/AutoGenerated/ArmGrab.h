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

#ifndef NM_MDF_ARMGRAB_H
#define NM_MDF_ARMGRAB_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmGrab.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Edge.h"
#include "Types/EndConstraintControl.h"
#include "Types/GrabFailureConditions.h"
#include "Types/GrabAliveParams.h"
#include "Types/ReachWorldParameters.h"
#include "Types/LimbControl.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "ArmGrabData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class Arm;
struct ArmGrabAPIBase;
struct ArmGrabUpdatePackage;
struct ArmGrabFeedbackPackage;

class ArmGrab : public ER::Module
{
public:

  ArmGrab(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~ArmGrab();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "ArmGrab"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  ArmGrabData* data;
  ArmGrabFeedbackInputs* feedIn;
  ArmGrabInputs* in;
  ArmGrabFeedbackOutputs* feedOut;
  ArmGrabOutputs* out;

  // owner access
  const Arm* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  ArmGrabAPIBase* m_apiBase;
  ArmGrabUpdatePackage* m_updatePackage;
  ArmGrabFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ArmGrab), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class ArmGrab_Con : public ER::ModuleCon
{
public:

  virtual ~ArmGrab_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(ArmGrab* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_edgeToGrab, 
      *junc_in_edgeIsObstructed, 
      *junc_in_numConstrainedArms, 
      *junc_in_hasTwistedAround, 
      *junc_in_endConstraintOptions, 
      *junc_in_isHanging, 
      *junc_in_failureConditions, 
      *junc_in_aliveParams, 
      *junc_in_importance, 
      *junc_feedIn_timeHeldProportion;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMGRAB_H

