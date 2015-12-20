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

#ifndef NM_MDF_REACHFORBODY_H
#define NM_MDF_REACHFORBODY_H

// include definition file to create project dependency
#include "./Definition/Modules/ReachForBody.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/BraceHazard.h"
#include "Types/ReachActionParams.h"
#include "Types/BodyHitInfo.h"
#include "Types/LimbControl.h"
#include "Types/ReachTarget.h"
#include "Types/TargetRequest.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "ReachForBodyData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class BodyFrame;
struct ReachForBodyAPIBase;
struct ReachForBodyUpdatePackage;
struct ReachForBodyFeedbackPackage;

class ReachForBody : public ER::Module
{
public:

  ReachForBody(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~ReachForBody();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "ReachForBody"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  ReachForBodyData* data;
  ReachForBodyFeedbackInputs* feedIn;
  ReachForBodyInputs* in;
  ReachForBodyFeedbackOutputs* feedOut;
  ReachForBodyOutputs* out;

  // owner access
  const BodyFrame* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  ReachForBodyAPIBase* m_apiBase;
  ReachForBodyUpdatePackage* m_updatePackage;
  ReachForBodyFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ReachForBody), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class ReachForBody_Con : public ER::ModuleCon
{
public:

  virtual ~ReachForBody_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(ReachForBody* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_armReachForBodyPartFlags_0, 
      *junc_in_armReachForBodyPartFlags_1, 
      *junc_in_reachActionParams_0, 
      *junc_in_reachActionParams_1, 
      *junc_in_braceHazard, 
      *junc_feedIn_competingArmControls_0, 
      *junc_feedIn_competingArmControls_1, 
      *junc_feedIn_shoulderPositions_0, 
      *junc_feedIn_shoulderPositions_1, 
      *junc_feedIn_armReachStates_0, 
      *junc_feedIn_armReachStates_1, 
      *junc_feedIn_endEffectorDistanceSquaredToTargets_0, 
      *junc_feedIn_endEffectorDistanceSquaredToTargets_1, 
      *junc_feedIn_armStrengthReduction_0, 
      *junc_feedIn_armStrengthReduction_1;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_REACHFORBODY_H

