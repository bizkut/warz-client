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

#ifndef NM_MDF_IMPACTPREDICTOR_H
#define NM_MDF_IMPACTPREDICTOR_H

// include definition file to create project dependency
#include "./Definition/Modules/ImpactPredictor.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/Hazard.h"
#include "Types/SphereTrajectory.h"
#include "Types/ProtectParameters.h"
#include "Types/ObjectFiltering.h"
#include "Types/PatchSet.h"
#include "Types/SpatialTarget.h"
#include "Types/TargetRequest.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "ImpactPredictorData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class HazardManagement;
struct ImpactPredictorAPIBase;
struct ImpactPredictorUpdatePackage;
struct ImpactPredictorFeedbackPackage;

class ImpactPredictor : public ER::Module
{
public:

  ImpactPredictor(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~ImpactPredictor();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "ImpactPredictor"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  ImpactPredictorData* data;
  ImpactPredictorFeedbackInputs* feedIn;
  ImpactPredictorInputs* in;
  ImpactPredictorFeedbackOutputs* feedOut;
  ImpactPredictorOutputs* out;

  // owner access
  const HazardManagement* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  ImpactPredictorAPIBase* m_apiBase;
  ImpactPredictorUpdatePackage* m_updatePackage;
  ImpactPredictorFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ImpactPredictor), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class ImpactPredictor_chestCon : public ER::ModuleCon
{
public:

  virtual ~ImpactPredictor_chestCon();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(ImpactPredictor* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_feedIn_leanVector, 
      *junc_feedIn_supportTM, 
      *junc_feedIn_potentialEnvironmentHazards, 
      *junc_in_ignoreVerticalPredictionAmount, 
      *junc_in_velocityIsControlled, 
      *junc_in_probeRadius, 
      *junc_in_potentialUserHazards, 
      *junc_in_ignoreNonUserHazards, 
      *junc_in_protectParameters, 
      *junc_in_protectHazardFiltering;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_IMPACTPREDICTOR_H

