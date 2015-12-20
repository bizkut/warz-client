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

#ifndef NM_MDF_ARMHOLDINGSUPPORT_H
#define NM_MDF_ARMHOLDINGSUPPORT_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmHoldingSupport.module"

// external types
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/LimbControl.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "ArmHoldingSupportData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class Arm;
struct ArmHoldingSupportAPIBase;
struct ArmHoldingSupportUpdatePackage;
struct ArmHoldingSupportFeedbackPackage;

class ArmHoldingSupport : public ER::Module
{
public:

  ArmHoldingSupport(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~ArmHoldingSupport();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "ArmHoldingSupport"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  ArmHoldingSupportData* data;
  ArmHoldingSupportInputs* in;
  ArmHoldingSupportFeedbackOutputs* feedOut;
  ArmHoldingSupportOutputs* out;

  // owner access
  const Arm* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  ArmHoldingSupportAPIBase* m_apiBase;
  ArmHoldingSupportUpdatePackage* m_updatePackage;
  ArmHoldingSupportFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ArmHoldingSupport), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class ArmHoldingSupport_Con : public ER::ModuleCon
{
public:

  virtual ~ArmHoldingSupport_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(ArmHoldingSupport* module);
  void relocate();
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_in_rotationRequest, 
      *junc_in_translationRequest, 
      *junc_in_rootDesiredTM, 
      *junc_in_endDesiredTM, 
      *junc_in_supportStrengthScale;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMHOLDINGSUPPORT_H

