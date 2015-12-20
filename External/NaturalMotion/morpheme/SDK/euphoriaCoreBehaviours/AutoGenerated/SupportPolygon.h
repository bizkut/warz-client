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

#ifndef NM_MDF_SUPPORTPOLYGON_H
#define NM_MDF_SUPPORTPOLYGON_H

// include definition file to create project dependency
#include "./Definition/Modules/SupportPolygon.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Environment_SupportPoly.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "SupportPolygonData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class BodyFrame;
struct SupportPolygonAPIBase;
struct SupportPolygonUpdatePackage;
struct SupportPolygonFeedbackPackage;

class SupportPolygon : public ER::Module
{
public:

  SupportPolygon(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~SupportPolygon();

  void update(float timeStep);
  // No feedback() api is required for this module.

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineInputs() { m_moduleCon->combineInputs(this); }
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "SupportPolygon"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  SupportPolygonData* data;
  SupportPolygonFeedbackInputs* feedIn;
  SupportPolygonInputs* in;
  SupportPolygonOutputs* out;

  // owner access
  const BodyFrame* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  SupportPolygonAPIBase* m_apiBase;
  SupportPolygonUpdatePackage* m_updatePackage;
  SupportPolygonFeedbackPackage* m_feedbackPackage;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(SupportPolygon), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class SupportPolygon_Con : public ER::ModuleCon
{
public:

  virtual ~SupportPolygon_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(SupportPolygon* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_junc_FeedbackInputs_supportPoints_0, 
      *junc_junc_FeedbackInputs_supportPoints_1, 
      *junc_junc_FeedbackInputs_supportPoints_2, 
      *junc_junc_FeedbackInputs_supportPoints_3, 
      *junc_junc_FeedbackInputs_supportPoints_4, 
      *junc_feedIn_fullySupported;

};

class SupportPolygon_StandingCon : public ER::ModuleCon
{
public:

  virtual ~SupportPolygon_StandingCon();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(SupportPolygon* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_junc_FeedbackInputs_supportPoints_0, 
      *junc_junc_FeedbackInputs_supportPoints_1, 
      *junc_junc_FeedbackInputs_supportPoints_2, 
      *junc_junc_FeedbackInputs_supportPoints_3, 
      *junc_junc_FeedbackInputs_supportPoints_4, 
      *junc_feedIn_fullySupported, 
      *junc_in_includeArmSupportPoints, 
      *junc_in_includeLegSupportPoints;

};

class SupportPolygon_SittingCon : public ER::ModuleCon
{
public:

  virtual ~SupportPolygon_SittingCon();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(SupportPolygon* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineInputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_junc_FeedbackInputs_supportPoints_0, 
      *junc_junc_FeedbackInputs_supportPoints_1, 
      *junc_junc_FeedbackInputs_supportPoints_2, 
      *junc_junc_FeedbackInputs_supportPoints_3, 
      *junc_junc_FeedbackInputs_supportPoints_4, 
      *junc_feedIn_fullySupported, 
      *junc_in_includeLegSupportPoints, 
      *junc_in_includeSpineSupportPoints;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SUPPORTPOLYGON_H

