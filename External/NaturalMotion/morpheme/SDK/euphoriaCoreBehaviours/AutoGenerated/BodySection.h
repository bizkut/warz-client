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

#ifndef NM_MDF_BODYSECTION_H
#define NM_MDF_BODYSECTION_H

// include definition file to create project dependency
#include "./Definition/Modules/BodySection.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/BraceState.h"
#include "Types/ContactInfo.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "BodySectionData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class RotateCore;
class PositionCore;
class BraceChooser;
class MyNetwork;
struct BodySectionAPIBase;
struct BodySectionUpdatePackage;
struct BodySectionFeedbackPackage;

class BodySection : public ER::Module
{
public:

  BodySection(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~BodySection();

  // No update() api is required for this module.
  // No feedback() api is required for this module.

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "BodySection"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  BodySectionFeedbackInputs* feedIn;
  BodySectionInputs* in;
  BodySectionFeedbackOutputs* feedOut;
  BodySectionOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  RotateCore* rotate;
  PositionCore* position;
  BraceChooser* braceChooser;

  // child module access
  ER::Module* m_childModules[3];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 3 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 3; }

  // Module packaging
  BodySectionAPIBase* m_apiBase;
  BodySectionUpdatePackage* m_updatePackage;
  BodySectionFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BodySection), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class BodySection_Con : public ER::ModuleCon
{
public:

  virtual ~BodySection_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BodySection* module);
  void relocate();
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_out_oneWayTranslationRequest, 
      *junc_out_translationRequest, 
      *junc_feedOut_controlledAmount, 
      *junc_out_intermediateRR;

};

class BodySection_lowerCon : public ER::ModuleCon
{
public:

  virtual ~BodySection_lowerCon();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BodySection* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_out_oneWayTranslationRequest, 
      *junc_out_translationRequest, 
      *junc_feedOut_controlledAmount, 
      *junc_out_intermediateRR, 
      *junc_rotationRequest, 
      *junc_feedIn_oneWayRotationRequest, 
      *junc_feedIn_oneWayTranslationRequest, 
      *junc_feedIn_supportingAmount, 
      *junc_feedIn_externallyControlledAmount, 
      *junc_feedIn_inContact, 
      *junc_out_oneWayRotationRequest, 
      *junc_feedIn_braceStates_0, 
      *junc_feedIn_braceStates_1, 
      *junc_feedIn_cushionPoint, 
      *junc_out_contactDirectionRequest, 
      *junc_feedIn_limbTargetDirection, 
      *junc_out_rotationRequest;

};

class BodySection_upperCon : public ER::ModuleCon
{
public:

  virtual ~BodySection_upperCon();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(BodySection* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_out_oneWayTranslationRequest, 
      *junc_out_translationRequest, 
      *junc_feedOut_controlledAmount, 
      *junc_out_intermediateRR, 
      *junc_in_rotationRequest, 
      *junc_in_translationRequest, 
      *junc_feedIn_rotationRequest, 
      *junc_feedIn_translationRequest, 
      *junc_feedIn_oneWayRotationRequest, 
      *junc_feedIn_oneWayTranslationRequest, 
      *junc_feedIn_externallyControlledAmount, 
      *junc_feedIn_supportingAmount, 
      *junc_feedIn_inContact, 
      *junc_feedIn_braceStates_0, 
      *junc_feedIn_braceStates_1, 
      *junc_feedIn_cushionPoint, 
      *junc_out_oneWayRotationRequest, 
      *junc_feedIn_limbTargetDirection, 
      *junc_out_rotationRequest;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYSECTION_H

