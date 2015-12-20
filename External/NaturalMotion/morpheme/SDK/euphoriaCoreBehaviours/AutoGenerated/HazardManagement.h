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

#ifndef NM_MDF_HAZARDMANAGEMENT_H
#define NM_MDF_HAZARDMANAGEMENT_H

// include definition file to create project dependency
#include "./Definition/Modules/HazardManagement.module"

// known types
#include "Types/BodyState.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "HazardManagementData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class GrabDetection;
class Grab;
class HazardResponse;
class FreeFallManagement;
class ImpactPredictor;
class ShieldManagement;
class MyNetwork;
struct HazardManagementAPIBase;
struct HazardManagementUpdatePackage;
struct HazardManagementFeedbackPackage;

class HazardManagement : public ER::Module
{
public:

  HazardManagement(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~HazardManagement();

  // No update() api is required for this module.
  // No feedback() api is required for this module.

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineOutputs() { m_moduleCon->combineOutputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "HazardManagement"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  HazardManagementFeedbackInputs* feedIn;
  HazardManagementOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children
  GrabDetection* grabDetection;
  Grab* grab;
  HazardResponse* hazardResponse;
  FreeFallManagement* freeFallManagement;
  ImpactPredictor* chestImpactPredictor;
  ShieldManagement* shieldManagement;

  // child module access
  ER::Module* m_childModules[6];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 6 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 6; }

  // Module packaging
  HazardManagementAPIBase* m_apiBase;
  HazardManagementUpdatePackage* m_updatePackage;
  HazardManagementFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(HazardManagement), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class HazardManagement_Con : public ER::ModuleCon
{
public:

  virtual ~HazardManagement_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(HazardManagement* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_out_instabilityPreparation, 
      *junc_feedIn_supportAmount, 
      *junc_feedIn_chestState;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HAZARDMANAGEMENT_H

