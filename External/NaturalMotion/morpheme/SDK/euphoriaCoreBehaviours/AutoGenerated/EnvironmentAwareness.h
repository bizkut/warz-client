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

#ifndef NM_MDF_ENVIRONMENTAWARENESS_H
#define NM_MDF_ENVIRONMENTAWARENESS_H

// include definition file to create project dependency
#include "./Definition/Modules/EnvironmentAwareness.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erCollisionProbes.h"

// known types
#include "Types/PatchStore.h"
#include "Types/PatchSet.h"
#include "Types/Environment_ObjectData.h"
#include "Types/Environment_Object.h"
#include "Types/ModuleRNG.h"
#include "Types/SphereTrajectory.h"
#include "Types/ObjectMetric.h"
#include "Types/Environment_State.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "EnvironmentAwarenessData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class MyNetwork;
struct EnvironmentAwarenessAPIBase;
struct EnvironmentAwarenessUpdatePackage;
struct EnvironmentAwarenessFeedbackPackage;

class EnvironmentAwareness : public ER::Module
{
public:

  EnvironmentAwareness(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~EnvironmentAwareness();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "EnvironmentAwareness"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  EnvironmentAwarenessData* data;
  EnvironmentAwarenessFeedbackInputs* feedIn;
  EnvironmentAwarenessInputs* in;
  EnvironmentAwarenessFeedbackOutputs* feedOut;
  EnvironmentAwarenessOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  EnvironmentAwarenessAPIBase* m_apiBase;
  EnvironmentAwarenessUpdatePackage* m_updatePackage;
  EnvironmentAwarenessFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(EnvironmentAwareness), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class EnvironmentAwareness_Con : public ER::ModuleCon
{
public:

  virtual ~EnvironmentAwareness_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(EnvironmentAwareness* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_feedIn_focalCentre, 
      *junc_feedIn_focalRadius, 
      *junc_feedIn_requestNextProbe, 
      *junc_in_sphereProjectionsForImpactPredictor, 
      *junc_in_findObject;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ENVIRONMENTAWARENESS_H

