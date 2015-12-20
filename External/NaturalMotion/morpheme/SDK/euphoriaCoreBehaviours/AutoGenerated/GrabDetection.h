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

#ifndef NM_MDF_GRABDETECTION_H
#define NM_MDF_GRABDETECTION_H

// include definition file to create project dependency
#include "./Definition/Modules/GrabDetection.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/EdgeBuffer.h"
#include "Types/Edge.h"
#include "Types/ObjectFiltering.h"
#include "Types/GrabDetectionParams.h"
#include "Types/TargetRequest.h"
#include "Types/PatchSet.h"
#include "Types/BodyState.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "GrabDetectionData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class HazardManagement;
struct GrabDetectionAPIBase;

class GrabDetection : public ER::Module
{
public:

  GrabDetection(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~GrabDetection();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "GrabDetection"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  GrabDetectionData* data;
  GrabDetectionFeedbackInputs* feedIn;
  GrabDetectionInputs* in;
  GrabDetectionOutputs* out;

  // owner access
  const HazardManagement* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  GrabDetectionAPIBase* m_apiBase;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(GrabDetection), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class GrabDetection_Con : public ER::ModuleCon
{
public:

  virtual ~GrabDetection_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(GrabDetection* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_feedIn_potentialGrabPatches, 
      *junc_feedIn_useEnvironmentAwareness, 
      *junc_feedIn_isHanging, 
      *junc_feedIn_centreOfMassBodyState, 
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
      *junc_in_supportStrengthScale, 
      *junc_in_grabFiltering;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_GRABDETECTION_H

