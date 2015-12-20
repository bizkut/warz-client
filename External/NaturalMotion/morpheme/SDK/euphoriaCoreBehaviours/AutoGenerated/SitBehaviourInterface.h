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

#ifndef NM_MDF_SITBEHAVIOURINTERFACE_H
#define NM_MDF_SITBEHAVIOURINTERFACE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/SitBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/SitParameters.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "SitBehaviourInterfaceData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

class MyNetwork;
struct SitBehaviourInterfaceAPIBase;

class SitBehaviourInterface : public ER::Module
{
public:

  SitBehaviourInterface(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet);
  virtual ~SitBehaviourInterface();

  void update(float timeStep);
  void feedback(float timeStep);

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  void combineFeedbackInputs() { m_moduleCon->combineFeedbackInputs(this); }
  void combineFeedbackOutputs() { m_moduleCon->combineFeedbackOutputs(this); }
  virtual const char* getClassName() const  NM_OVERRIDE { return "SitBehaviourInterface"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // module data blocks
  SitBehaviourInterfaceData* data;
  SitBehaviourInterfaceFeedbackInputs* feedIn;
  SitBehaviourInterfaceFeedbackOutputs* feedOut;
  SitBehaviourInterfaceOutputs* out;

  // owner access
  const MyNetwork* owner;
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return (const ER::Module*)owner; }

  // module children

  // child module access
  virtual ER::Module* getChild(int32_t) const  NM_OVERRIDE { NMP_ASSERT_FAIL(); return 0; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 0; }

  // Module packaging
  SitBehaviourInterfaceAPIBase* m_apiBase;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(SitBehaviourInterface), NMP_VECTOR_ALIGNMENT);
    return result;
  }
};

//----------------------------------------------------------------------------------------------------------------------
class SitBehaviourInterface_Con : public ER::ModuleCon
{
public:

  virtual ~SitBehaviourInterface_Con();
  static NMP::Memory::Format getMemoryRequirements();

  virtual void create(ER::Module* module, ER::Module* owner) NM_OVERRIDE;
  virtual void buildConnections(SitBehaviourInterface* module);
  void relocate();
  virtual void combineFeedbackInputs(ER::Module* module) NM_OVERRIDE;
  virtual void combineFeedbackOutputs(ER::Module* module) NM_OVERRIDE;

private:

  ER::Junction
      *junc_feedIn_sitAmount, 
      *junc_feedOut_sitAmount;

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SITBEHAVIOURINTERFACE_H

