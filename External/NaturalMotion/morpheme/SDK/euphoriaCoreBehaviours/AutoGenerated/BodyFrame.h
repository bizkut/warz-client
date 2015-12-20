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

#ifndef NM_MDF_BODYFRAME_H
#define NM_MDF_BODYFRAME_H

// include definition file to create project dependency
#include "./Definition/Modules/BodyFrame.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/BodyState.h"
#include "Types/RotationRequest.h"
#include "Types/State.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "BodyFrameData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class SupportPolygon;
class SupportPolygon;
class BodyBalance;
class SittingBodyBalance;
class ReachForBody;
class BalanceAssistant;
class MyNetwork;
struct BodyFrameAPIBase;
struct BodyFrameUpdatePackage;
struct BodyFrameFeedbackPackage;

class BodyFrame : public ER::Module
{
public:

  BodyFrame(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~BodyFrame();

  // private API functions declared in module definition

  void handleKeyboardInput();
  void visualiseDynamicState();


  // No update() api is required for this module.
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "BodyFrame"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  BodyFrameData* data;
  BodyFrameFeedbackInputs* feedIn;
  BodyFrameInputs* in;
  BodyFrameFeedbackOutputs* feedOut;
  BodyFrameOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  SupportPolygon* supportPolygon;
  SupportPolygon* sittingSupportPolygon;
  BodyBalance* bodyBalance;
  SittingBodyBalance* sittingBodyBalance;
  ReachForBody* reachForBody;
  BalanceAssistant* balanceAssistant;

  // child module access
  ER::Module* m_childModules[6];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 6 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 6; }

  // Module packaging
  BodyFrameAPIBase* m_apiBase;
  BodyFrameUpdatePackage* m_updatePackage;
  BodyFrameFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BodyFrame), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class BodyFrame_Con : public ER::ModuleCon
{
public:

  virtual ~BodyFrame_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BodyFrame* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_instabilityPreparation, 
      *junc_feedOut_impendingInstability, 
      *junc_feedOut_supportAmount, 
      *junc_feedIn_averageHips, 
      *junc_feedIn_chestState, 
      *junc_feedIn_averageOfSupportingHands, 
      *junc_feedIn_averageOfSupportingFeet, 
      *junc_feedIn_averageOfHandsAndFeet, 
      *junc_feedIn_averageShoulders, 
      *junc_feedIn_legsInContact, 
      *junc_feedIn_spineInContact, 
      *junc_feedIn_maxLegCollidingTime, 
      *junc_feedIn_isBodyBarFeetContacting, 
      *junc_out_pelvisRotationRequest, 
      *junc_out_isInContact, 
      *junc_feedOut_leanVector;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYFRAME_H

