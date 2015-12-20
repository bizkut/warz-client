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

#ifndef NM_MDF_BEHAVIOURINTERFACES_H
#define NM_MDF_BEHAVIOURINTERFACES_H

//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDescriptor.h"
#include "AimBehaviourInterface.h"
#include "AnimateBehaviourInterface.h"
#include "ArmsBraceBehaviourInterface.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "BalanceBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "HazardAwarenessBehaviourInterface.h"
#include "HoldActionBehaviourInterface.h"
#include "HoldBehaviourInterface.h"
#include "LegsPedalBehaviourInterface.h"
#include "LookBehaviourInterface.h"
#include "ObserveBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "ReachForBodyBehaviourInterface.h"
#include "ReachForWorldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "UserHazardBehaviourInterface.h"
#include "WritheBehaviourInterface.h"

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
template <typename BIType>
class BI
{
public:
  BI(const ER::Character *character, NetworkManifest::Modules moduleType) : m_module(0)
  {
    if (!character || !character->m_euphoriaRootModule->isEnabled(moduleType))
      return;
    ER::Module *m = character->m_euphoriaRootModule->getModule(moduleType);
    m_module = (BIType *) m;
  }
  bool isAvailable() const {return m_module != 0;}
protected:
  const BIType *m_module;
};

//----------------------------------------------------------------------------------------------------------------------
class AimBehaviourInterfaceWrapper : public BI<AimBehaviourInterface>
{
public:
  AimBehaviourInterfaceWrapper(const ER::Character* character) : BI<AimBehaviourInterface>(character, NetworkManifest::aimBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class AnimateBehaviourInterfaceWrapper : public BI<AnimateBehaviourInterface>
{
public:
  AnimateBehaviourInterfaceWrapper(const ER::Character* character) : BI<AnimateBehaviourInterface>(character, NetworkManifest::animateBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class ArmsBraceBehaviourInterfaceWrapper : public BI<ArmsBraceBehaviourInterface>
{
public:
  ArmsBraceBehaviourInterfaceWrapper(const ER::Character* character) : BI<ArmsBraceBehaviourInterface>(character, NetworkManifest::armsBraceBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class ArmsWindmillBehaviourInterfaceWrapper : public BI<ArmsWindmillBehaviourInterface>
{
public:
  ArmsWindmillBehaviourInterfaceWrapper(const ER::Character* character) : BI<ArmsWindmillBehaviourInterface>(character, NetworkManifest::armsWindmillBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class BalanceBehaviourInterfaceWrapper : public BI<BalanceBehaviourInterface>
{
public:
  BalanceBehaviourInterfaceWrapper(const ER::Character* character) : BI<BalanceBehaviourInterface>(character, NetworkManifest::balanceBehaviourInterface) {}

  // ==================== Feed outputs ====================
  const BodyState& getCoMBodyState() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getCoMBodyState();}
  float getCoMBodyStateImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getCoMBodyStateImportance();}

  /// Time that the character has been standing still - will be zero if stepping or fallen.
  const float& getStandingStillTime() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getStandingStillTime();}
  float getStandingStillTimeImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getStandingStillTimeImportance();}

  /// Time that the character has been fallen. 
  const float& getFallenTime() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getFallenTime();}
  float getFallenTimeImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getFallenTimeImportance();}

  /// Time that the character has been touching something that appears to be "ground"
  const float& getOnGroundTime() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getOnGroundTime();}
  float getOnGroundTimeImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getOnGroundTimeImportance();}

  /// Indicator of how balanced the character is from 0 to 1
  const float& getBalanceAmount() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getBalanceAmount();}
  float getBalanceAmountImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getBalanceAmountImportance();}

  // stepping
  const float& getSteppingTime() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getSteppingTime();}
  float getSteppingTimeImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getSteppingTimeImportance();}

  const int32_t& getStepCount() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getStepCount();}
  float getStepCountImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getStepCountImportance();}

};

//----------------------------------------------------------------------------------------------------------------------
class FreeFallBehaviourInterfaceWrapper : public BI<FreeFallBehaviourInterface>
{
public:
  FreeFallBehaviourInterfaceWrapper(const ER::Character* character) : BI<FreeFallBehaviourInterface>(character, NetworkManifest::freeFallBehaviourInterface) {}

  // ==================== Feed outputs ====================
  const float& getOrientationError() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getOrientationError();}
  float getOrientationErrorImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getOrientationErrorImportance();}

  const bool& getIsInFreefall() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getIsInFreefall();}
  float getIsInFreefallImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getIsInFreefallImportance();}

};

//----------------------------------------------------------------------------------------------------------------------
class HazardAwarenessBehaviourInterfaceWrapper : public BI<HazardAwarenessBehaviourInterface>
{
public:
  HazardAwarenessBehaviourInterfaceWrapper(const ER::Character* character) : BI<HazardAwarenessBehaviourInterface>(character, NetworkManifest::hazardAwarenessBehaviourInterface) {}

  // ==================== User Inputs ====================
  void setUseControlledVelocity(const bool& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setUseControlledVelocity(value, importance);}

  // ==================== Feed outputs ====================
  const Hazard& getHazard() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getHazard();}
  float getHazardImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getHazardImportance();}

  const float& getHazardFreeTime() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getHazardFreeTime();}
  float getHazardFreeTimeImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getHazardFreeTimeImportance();}

  /// unitless 0-1 value
  const float& getHazardousness() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getHazardousness();}
  float getHazardousnessImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getHazardousnessImportance();}

};

//----------------------------------------------------------------------------------------------------------------------
class HoldActionBehaviourInterfaceWrapper : public BI<HoldActionBehaviourInterface>
{
public:
  HoldActionBehaviourInterfaceWrapper(const ER::Character* character) : BI<HoldActionBehaviourInterface>(character, NetworkManifest::holdActionBehaviourInterface) {}

  // ==================== User Inputs ====================
  void setGameEdgesAt(unsigned int index, const Edge& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setGameEdgesAt(index, value, importance);}
  void setGameEdges(unsigned int number, const Edge* values, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setGameEdges(number, values, importance);}

  // ==================== Feed outputs ====================
  /// Number of currently constrained hands. Simply passed on.
  const uint16_t& getNumConstrainedArms() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getNumConstrainedArms();}
  float getNumConstrainedArmsImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getNumConstrainedArmsImportance();}

};

//----------------------------------------------------------------------------------------------------------------------
class HoldBehaviourInterfaceWrapper : public BI<HoldBehaviourInterface>
{
public:
  HoldBehaviourInterfaceWrapper(const ER::Character* character) : BI<HoldBehaviourInterface>(character, NetworkManifest::holdBehaviourInterface) {}

  // ==================== User Inputs ====================
  /// Bitmask used by filter shader function.
  void setFilterBitmaskWordAt(unsigned int index, const uint32_t& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setFilterBitmaskWordAt(index, value, importance);}
  void setFilterBitmaskWord(unsigned int number, const uint32_t* values, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setFilterBitmaskWord(number, values, importance);}

  /// Toggles edge filtering that uses additional information associated with an edge to evaluate whether this edge should be considered or not.
  void setUseFiltering(const bool& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setUseFiltering(value, importance);}

  // ==================== Feed outputs ====================
  /// Number of currently constrained hands. Simply passed on.
  const uint16_t& getNumConstrainedArms() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getNumConstrainedArms();}
  float getNumConstrainedArmsImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getNumConstrainedArmsImportance();}

};

//----------------------------------------------------------------------------------------------------------------------
class LegsPedalBehaviourInterfaceWrapper : public BI<LegsPedalBehaviourInterface>
{
public:
  LegsPedalBehaviourInterfaceWrapper(const ER::Character* character) : BI<LegsPedalBehaviourInterface>(character, NetworkManifest::legsPedalBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class LookBehaviourInterfaceWrapper : public BI<LookBehaviourInterface>
{
public:
  LookBehaviourInterfaceWrapper(const ER::Character* character) : BI<LookBehaviourInterface>(character, NetworkManifest::lookBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class ObserveBehaviourInterfaceWrapper : public BI<ObserveBehaviourInterface>
{
public:
  ObserveBehaviourInterfaceWrapper(const ER::Character* character) : BI<ObserveBehaviourInterface>(character, NetworkManifest::observeBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class ProtectBehaviourInterfaceWrapper : public BI<ProtectBehaviourInterface>
{
public:
  ProtectBehaviourInterfaceWrapper(const ER::Character* character) : BI<ProtectBehaviourInterface>(character, NetworkManifest::protectBehaviourInterface) {}

  // ==================== User Inputs ====================
  /// Bitmask used by filter shader function.
  void setFilterBitmaskWordAt(unsigned int index, const uint32_t& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setFilterBitmaskWordAt(index, value, importance);}
  void setFilterBitmaskWord(unsigned int number, const uint32_t* values, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setFilterBitmaskWord(number, values, importance);}

  /// means character's velocity is not considered capable of being a hazard
  void setUseControlledVelocity(const bool& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setUseControlledVelocity(value, importance);}

  /// Toggles hazard filtering that uses additional information associated with a hazard to evaluate whether it should be considered or not.
  void setUseFiltering(const bool& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setUseFiltering(value, importance);}

};

//----------------------------------------------------------------------------------------------------------------------
class ReachForBodyBehaviourInterfaceWrapper : public BI<ReachForBodyBehaviourInterface>
{
public:
  ReachForBodyBehaviourInterfaceWrapper(const ER::Character* character) : BI<ReachForBodyBehaviourInterface>(character, NetworkManifest::reachForBodyBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class ReachForWorldBehaviourInterfaceWrapper : public BI<ReachForWorldBehaviourInterface>
{
public:
  ReachForWorldBehaviourInterfaceWrapper(const ER::Character* character) : BI<ReachForWorldBehaviourInterface>(character, NetworkManifest::reachForWorldBehaviourInterface) {}

  // ==================== Feed outputs ====================
  const float& getDistanceToTarget(unsigned int index) const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getDistanceToTarget(index);}
  float getDistanceToTargetImportance(int index) const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getDistanceToTargetImportance(index);}

};

//----------------------------------------------------------------------------------------------------------------------
class ShieldActionBehaviourInterfaceWrapper : public BI<ShieldActionBehaviourInterface>
{
public:
  ShieldActionBehaviourInterfaceWrapper(const ER::Character* character) : BI<ShieldActionBehaviourInterface>(character, NetworkManifest::shieldActionBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
class ShieldBehaviourInterfaceWrapper : public BI<ShieldBehaviourInterface>
{
public:
  ShieldBehaviourInterfaceWrapper(const ER::Character* character) : BI<ShieldBehaviourInterface>(character, NetworkManifest::shieldBehaviourInterface) {}

  // ==================== Feed outputs ====================
  const bool& getIsShielding() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getIsShielding();}
  float getIsShieldingImportance() const {NMP_ASSERT(isAvailable()); return m_module->feedOut->getIsShieldingImportance();}

};

//----------------------------------------------------------------------------------------------------------------------
class UserHazardBehaviourInterfaceWrapper : public BI<UserHazardBehaviourInterface>
{
public:
  UserHazardBehaviourInterfaceWrapper(const ER::Character* character) : BI<UserHazardBehaviourInterface>(character, NetworkManifest::userHazardBehaviourInterface) {}

  // ==================== User Inputs ====================
  /// Potential hazard can be written directly into this structure by client code.
  void setGameHazards(const PatchSet& value, float importance = 1.0f) const {NMP_ASSERT(isAvailable()); m_module->userIn->setGameHazards(value, importance);}

};

//----------------------------------------------------------------------------------------------------------------------
class WritheBehaviourInterfaceWrapper : public BI<WritheBehaviourInterface>
{
public:
  WritheBehaviourInterfaceWrapper(const ER::Character* character) : BI<WritheBehaviourInterface>(character, NetworkManifest::writheBehaviourInterface) {}

};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BEHAVIOURINTERFACES.H_H

