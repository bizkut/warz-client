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

#ifndef NM_MDF_GRAB_H
#define NM_MDF_GRAB_H

// include definition file to create project dependency
#include "./Definition/Modules/Grab.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Edge.h"
#include "Types/GrabControl.h"
#include "Types/GrabEnableConditions.h"
#include "Types/GrabAliveParams.h"
#include "Types/ContactInfo.h"
#include "Types/Hazard.h"
#include "Types/EndConstraintControl.h"
#include "Types/HoldTimer.h"
#include "Types/TranslationRequest.h"
#include "Types/RotationRequest.h"
#include "Types/TargetRequest.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "GrabData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class HazardManagement;
struct GrabAPIBase;

class Grab : public ER::Module
{
public:

  Grab(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~Grab();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "Grab"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  GrabData* data;
  GrabFeedbackInputs* feedIn;
  GrabInputs* in;
  GrabFeedbackOutputs* feedOut;
  GrabOutputs* out;

  // owner access
  const HazardManagement* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  GrabAPIBase* m_apiBase;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(Grab), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class Grab_Con : public ER::ModuleCon
{
public:

  virtual ~Grab_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(Grab* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_feedIn_spineInContact, 
      *junc_feedIn_legsInContact, 
      *junc_feedIn_centreOfShoulders, 
      *junc_feedIn_mostUprightContact, 
      *junc_feedIn_braceHazard, 
      *junc_in_grabbableEdge, 
      *junc_in_letGoEdge, 
      *junc_in_gameEdges_0, 
      *junc_in_gameEdges_1, 
      *junc_in_gameEdges_2, 
      *junc_in_gameEdges_3, 
      *junc_in_gameEdges_4, 
      *junc_in_gameEdges_5, 
      *junc_in_gameEdges_6, 
      *junc_in_gameEdges_7, 
      *junc_in_gameEdges_8, 
      *junc_in_gameEdges_9, 
      *junc_in_grabAbilityControl, 
      *junc_in_grabActionControl, 
      *junc_in_enableConditions, 
      *junc_in_aliveParams, 
      *junc_in_timeRatioBeforeLookingDown, 
      *junc_in_maxTwistBwdPeriod, 
      *junc_in_hasForcedEdge, 
      *junc_in_ignoreOvershotEdges;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_GRAB_H

