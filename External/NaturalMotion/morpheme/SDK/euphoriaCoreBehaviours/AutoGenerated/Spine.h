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

#ifndef NM_MDF_SPINE_H
#define NM_MDF_SPINE_H

// include definition file to create project dependency
#include "./Definition/Modules/Spine.module"

// external types
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/ProcessRequest.h"
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/LimbControl.h"
#include "Types/State.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "SpineData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class SpineSupport;
class SpinePose;
class SpineControl;
class SpineReachReaction;
class SpineWrithe;
class SpineAim;
class MyNetwork;
struct SpineAPIBase;
struct SpineUpdatePackage;
struct SpineFeedbackPackage;

class Spine : public ER::Module
{
public:

  Spine(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~Spine();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "Spine"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  SpineData* data;
  SpineInputs* in;
  SpineFeedbackOutputs* feedOut;
  SpineOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  SpineSupport* support;
  SpinePose* pose;
  SpineControl* control;
  SpineReachReaction* reachReaction;
  SpineWrithe* writhe;
  SpineAim* aim;

  // child module access
  ER::Module* m_childModules[6];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 6 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 6; }

  // Module packaging
  SpineAPIBase* m_apiBase;
  SpineUpdatePackage* m_updatePackage;
  SpineFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(Spine), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class Spine_Con : public ER::ModuleCon
{
public:

  virtual ~Spine_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(Spine* module);
  void relocate();
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_out_control, 
      *junc_in_endRotationRequestPri, 
      *junc_in_rootRotationRequest, 
      *junc_in_endTranslationRequest, 
      *junc_in_rootTranslationRequest, 
      *junc_in_isStepping, 
      *junc_in_limbStiffnessReduction, 
      *junc_in_limbControlCompensationReduction, 
      *junc_in_limbDampingRatioReduction, 
      *junc_in_pelvisControlledAmount, 
      *junc_in_chestControlledAmount, 
      *junc_in_defaultPoseWeight;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINE_H

