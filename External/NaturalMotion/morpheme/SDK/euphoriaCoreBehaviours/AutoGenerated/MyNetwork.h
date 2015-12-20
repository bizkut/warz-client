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

#ifndef NM_MDF_MYNETWORK_H
#define NM_MDF_MYNETWORK_H

// include definition file to create project dependency
#include "./Definition/Modules/MyNetwork.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMMatrix34.h"
#include "Helpers/LimbSharedState.h"

// base dependencies
#include "euphoria/erJunction.h"
#include "euphoria/erModule.h"
#include "euphoria/erCharacter.h"

// module data
#include "MyNetworkData.h"

namespace MR { struct PhysicsSerialisationBuffer; }

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct LimbControl; 

class CharacteristicsBehaviourInterface;
class PropertiesBehaviourInterface;
class EuphoriaRagdollBehaviourInterface;
class EyesBehaviourInterface;
class ArmsBraceBehaviourInterface;
class ArmsPlacementBehaviourInterface;
class AimBehaviourInterface;
class HeadDodgeBehaviourInterface;
class ReachForBodyBehaviourInterface;
class ReachForWorldBehaviourInterface;
class AnimateBehaviourInterface;
class BalanceBehaviourInterface;
class BalancePoserBehaviourInterface;
class ProtectBehaviourInterface;
class HazardAwarenessBehaviourInterface;
class UserHazardBehaviourInterface;
class ObserveBehaviourInterface;
class IdleAwakeBehaviourInterface;
class LookBehaviourInterface;
class HeadAvoidBehaviourInterface;
class ShieldBehaviourInterface;
class HoldBehaviourInterface;
class HoldActionBehaviourInterface;
class FreeFallBehaviourInterface;
class ArmsWindmillBehaviourInterface;
class LegsPedalBehaviourInterface;
class ShieldActionBehaviourInterface;
class SitBehaviourInterface;
class WritheBehaviourInterface;
class RandomLook;
class HazardManagement;
class BalanceManagement;
class EnvironmentAwareness;
class BodyFrame;
class BodySection;
class BodySection;
class Arm;
class Head;
class Leg;
class Spine;
class SceneProbes;
class BalanceBehaviourFeedback;
class MyNetworkDataAllocator : public ER::ModuleDataAllocator
{
public:

  virtual void init() NM_OVERRIDE;
  virtual void* alloc(ER::ModuleDataAllocator::Section sct, size_t size, size_t alignment) NM_OVERRIDE;
  virtual void clear(ER::ModuleDataAllocator::Section sct) NM_OVERRIDE;
  virtual void term() NM_OVERRIDE;
private:
  unsigned char*   m_mem[ER::ModuleDataAllocator::NumSections];
  size_t           m_offsets[ER::ModuleDataAllocator::NumSections],
                   m_limits[ER::ModuleDataAllocator::NumSections];
};

//----------------------------------------------------------------------------------------------------------------------
struct MyNetworkAPIBase;

class MyNetwork : public ER::RootModule
{
public:

  MyNetwork();
  virtual ~MyNetwork();

  // private API functions declared in module definition

  const LimbControl* getLimbControl(uint32_t iLimb) const;
  void setStrengths();
  void setProperties();
  void adjustLimbSoftLimits(uint32_t limbIndex, const LimbControl* limbControl, float stiffnessScale);


  virtual void update(float timeStep) NM_OVERRIDE;
  virtual void feedback(float timeStep) NM_OVERRIDE;
  virtual void executeNetworkUpdate(float timeStep) NM_OVERRIDE;
  virtual void executeNetworkFeedback(float timeStep) NM_OVERRIDE;

  virtual void clearAllData() NM_OVERRIDE;
  virtual void entry() NM_OVERRIDE;
  virtual void create(ER::Module* parent, int childIndex = -1) NM_OVERRIDE;
  virtual const char* getClassName() const  NM_OVERRIDE { return "MyNetwork"; }

  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

  // from ER::RootModule
  virtual void initLimbIndices() NM_OVERRIDE;
  virtual ER::ModuleDataAllocator* getDataAllocator() NM_OVERRIDE { return &m_mdAllocator; }


  // module data blocks
  MyNetworkData* data;

  // owner access
  virtual const ER::Module* getOwner() const  NM_OVERRIDE { return 0; }

  // module children
  CharacteristicsBehaviourInterface* characteristicsBehaviourInterface;
  PropertiesBehaviourInterface* propertiesBehaviourInterface;
  EuphoriaRagdollBehaviourInterface* euphoriaRagdollBehaviourInterface;
  EyesBehaviourInterface* eyesBehaviourInterface;
  ArmsBraceBehaviourInterface* armsBraceBehaviourInterface;
  ArmsPlacementBehaviourInterface* armsPlacementBehaviourInterface;
  AimBehaviourInterface* aimBehaviourInterface;
  HeadDodgeBehaviourInterface* headDodgeBehaviourInterface;
  ReachForBodyBehaviourInterface* reachForBodyBehaviourInterface;
  ReachForWorldBehaviourInterface* reachForWorldBehaviourInterface;
  AnimateBehaviourInterface* animateBehaviourInterface;
  BalanceBehaviourInterface* balanceBehaviourInterface;
  BalancePoserBehaviourInterface* balancePoserBehaviourInterface;
  ProtectBehaviourInterface* protectBehaviourInterface;
  HazardAwarenessBehaviourInterface* hazardAwarenessBehaviourInterface;
  UserHazardBehaviourInterface* userHazardBehaviourInterface;
  ObserveBehaviourInterface* observeBehaviourInterface;
  IdleAwakeBehaviourInterface* idleAwakeBehaviourInterface;
  LookBehaviourInterface* lookBehaviourInterface;
  HeadAvoidBehaviourInterface* headAvoidBehaviourInterface;
  ShieldBehaviourInterface* shieldBehaviourInterface;
  HoldBehaviourInterface* holdBehaviourInterface;
  HoldActionBehaviourInterface* holdActionBehaviourInterface;
  FreeFallBehaviourInterface* freeFallBehaviourInterface;
  ArmsWindmillBehaviourInterface* armsWindmillBehaviourInterface;
  LegsPedalBehaviourInterface* legsPedalBehaviourInterface;
  ShieldActionBehaviourInterface* shieldActionBehaviourInterface;
  SitBehaviourInterface* sitBehaviourInterface;
  WritheBehaviourInterface* writheBehaviourInterface;
  RandomLook* randomLook;
  HazardManagement* hazardManagement;
  BalanceManagement* balanceManagement;
  EnvironmentAwareness* environmentAwareness;
  BodyFrame* bodyFrame;
  BodySection* upperBody;
  BodySection* lowerBody;
  Arm* arms[2];
  Head* heads[1];
  Leg* legs[2];
  Spine* spines[1];
  SceneProbes* sceneProbes;
  BalanceBehaviourFeedback* balanceBehaviourFeedback;

  // child module access
  ER::Module* m_childModules[44];
  virtual ER::Module* getChild(int32_t index) const  NM_OVERRIDE { NMP_ASSERT(index >= 0 && index < 44 && m_childModules[index] != 0); return m_childModules[index]; }
  virtual const char* getChildName(int32_t index) const NM_OVERRIDE;
  virtual int32_t getNumChildren() const  NM_OVERRIDE { return 44; }

  // Module packaging
  MyNetworkAPIBase* m_apiBase;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(MyNetwork), NMP_VECTOR_ALIGNMENT);
    return result;
  }

  MyNetworkDataAllocator   m_mdAllocator;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_MYNETWORK_H

