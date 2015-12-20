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

#ifndef NM_MDF_LEG_H
#define NM_MDF_LEG_H

// include definition file to create project dependency
#include "./Definition/Modules/Leg.module"

// external types
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/LimbControl.h"
#include "Types/EndConstraintControl.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "LegData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class LegBrace;
class LegStandingSupport;
class LegSittingSupport;
class LegStep;
class LegSwing;
class LegPose;
class LegSpin;
class LegReachReaction;
class LegWrithe;
class MyNetwork;
struct LegAPIBase;
struct LegUpdatePackage;
struct LegFeedbackPackage;

class Leg : public ER::Module
{
public:

  Leg(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~Leg();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "Leg"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  LegData* data;
  LegInputs* in;
  LegFeedbackOutputs* feedOut;
  LegOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  LegBrace* brace;
  LegStandingSupport* standingSupport;
  LegSittingSupport* sittingSupport;
  LegStep* step;
  LegSwing* swing;
  LegPose* pose;
  LegSpin* spin;
  LegReachReaction* reachReaction;
  LegWrithe* writhe;

  // child module access
  ER::Module* m_childModules[9];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 9 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 9; }

  // Module packaging
  LegAPIBase* m_apiBase;
  LegUpdatePackage* m_updatePackage;
  LegFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(Leg), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class Leg_Con : public ER::ModuleCon
{
public:

  virtual ~Leg_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(Leg* module);
  void relocate();
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_wta01, 
      *junc_out_control, 
      *junc_out_useFullAnkleRange, 
      *junc_feedOut_supportedByConstraint, 
      *junc_in_limbStiffnessReduction, 
      *junc_in_limbControlCompensationReduction, 
      *junc_in_limbDampingRatioReduction, 
      *junc_in_centreOfHips, 
      *junc_in_centreOfSupport, 
      *junc_in_shouldBrace, 
      *junc_in_isBodyBarFeetContacting;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEG_H

