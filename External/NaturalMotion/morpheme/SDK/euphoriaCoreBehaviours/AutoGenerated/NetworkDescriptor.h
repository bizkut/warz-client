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

#ifndef NM_MDF_NET_DESC_H
#define NM_MDF_NET_DESC_H

#include "euphoria/erNetworkInterface.h"
#include "NMPlatform/NMMatrix34.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
struct NMBehaviourLibrary : public ER::NetworkInterface
{
  virtual const ER::NetworkInterface::Description& getDesc() const NM_OVERRIDE;

  virtual void init() NM_OVERRIDE;
  virtual ER::RootModule* createNetwork() const NM_OVERRIDE;
  virtual bool destroyNetwork(ER::Module** rootModuleOut) const NM_OVERRIDE;
  virtual void term() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkMetrics
{
  enum
  {
    numLimbs = 6,
  };
  enum LimbIndex
  {
    Arm_1,
    Arm_2,
    Head_1,
    Leg_1,
    Leg_2,
    Spine_1,
  };

  const char* getLimbName(LimbIndex index) const;
};

//----------------------------------------------------------------------------------------------------------------------
struct BodyPoseData
{
  NMP::Matrix34 m_poseEndRelativeToRoots[NetworkMetrics::numLimbs];
  float m_poseWeights[NetworkMetrics::numLimbs];
  float m_poseWeight;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkManifest
{
  enum Modules
  {
    rootModule,
    characteristicsBehaviourInterface,
    propertiesBehaviourInterface,
    euphoriaRagdollBehaviourInterface,
    eyesBehaviourInterface,
    armsBraceBehaviourInterface,
    armsPlacementBehaviourInterface,
    aimBehaviourInterface,
    headDodgeBehaviourInterface,
    reachForBodyBehaviourInterface,
    reachForWorldBehaviourInterface,
    animateBehaviourInterface,
    balanceBehaviourInterface,
    balancePoserBehaviourInterface,
    protectBehaviourInterface,
    hazardAwarenessBehaviourInterface,
    userHazardBehaviourInterface,
    observeBehaviourInterface,
    idleAwakeBehaviourInterface,
    lookBehaviourInterface,
    headAvoidBehaviourInterface,
    shieldBehaviourInterface,
    holdBehaviourInterface,
    holdActionBehaviourInterface,
    freeFallBehaviourInterface,
    armsWindmillBehaviourInterface,
    legsPedalBehaviourInterface,
    shieldActionBehaviourInterface,
    sitBehaviourInterface,
    writheBehaviourInterface,
    randomLook,
    hazardManagement,
    hazardManagement_grabDetection,
    hazardManagement_grab,
    hazardManagement_hazardResponse,
    hazardManagement_freeFallManagement,
    hazardManagement_chestImpactPredictor,
    hazardManagement_shieldManagement,
    balanceManagement,
    balanceManagement_staticBalance,
    balanceManagement_steppingBalance,
    balanceManagement_balancePoser,
    environmentAwareness,
    bodyFrame,
    bodyFrame_supportPolygon,
    bodyFrame_sittingSupportPolygon,
    bodyFrame_bodyBalance,
    bodyFrame_sittingBodyBalance,
    bodyFrame_reachForBody,
    bodyFrame_balanceAssistant,
    upperBody,
    upperBody_rotate,
    upperBody_position,
    upperBody_braceChooser,
    lowerBody,
    lowerBody_rotate,
    lowerBody_position,
    lowerBody_braceChooser,
    arms_0,
    arms_0_grab,
    arms_0_aim,
    arms_0_hold,
    arms_0_brace,
    arms_0_standingSupport,
    arms_0_holdingSupport,
    arms_0_sittingStep,
    arms_0_step,
    arms_0_spin,
    arms_0_swing,
    arms_0_reachForBodyPart,
    arms_0_reachForWorld,
    arms_0_placement,
    arms_0_pose,
    arms_0_reachReaction,
    arms_0_writhe,
    arms_1,
    arms_1_grab,
    arms_1_aim,
    arms_1_hold,
    arms_1_brace,
    arms_1_standingSupport,
    arms_1_holdingSupport,
    arms_1_sittingStep,
    arms_1_step,
    arms_1_spin,
    arms_1_swing,
    arms_1_reachForBodyPart,
    arms_1_reachForWorld,
    arms_1_placement,
    arms_1_pose,
    arms_1_reachReaction,
    arms_1_writhe,
    heads_0,
    heads_0_eyes,
    heads_0_avoid,
    heads_0_dodge,
    heads_0_aim,
    heads_0_point,
    heads_0_support,
    heads_0_pose,
    legs_0,
    legs_0_brace,
    legs_0_standingSupport,
    legs_0_sittingSupport,
    legs_0_step,
    legs_0_swing,
    legs_0_pose,
    legs_0_spin,
    legs_0_reachReaction,
    legs_0_writhe,
    legs_1,
    legs_1_brace,
    legs_1_standingSupport,
    legs_1_sittingSupport,
    legs_1_step,
    legs_1_swing,
    legs_1_pose,
    legs_1_spin,
    legs_1_reachReaction,
    legs_1_writhe,
    spines_0,
    spines_0_support,
    spines_0_pose,
    spines_0_control,
    spines_0_reachReaction,
    spines_0_writhe,
    spines_0_aim,
    sceneProbes,
    balanceBehaviourFeedback,
    // -------------
    NumModules
  };

  enum Behaviours
  {
    BehaviourID_Aim = 1,
    BehaviourID_Animate = 2,
    BehaviourID_ArmsBrace = 3,
    BehaviourID_ArmsPlacement = 4,
    BehaviourID_ArmsWindmill = 5,
    BehaviourID_Balance = 6,
    BehaviourID_BalancePoser = 7,
    BehaviourID_Characteristics = 8,
    BehaviourID_EuphoriaRagdoll = 9,
    BehaviourID_Eyes = 10,
    BehaviourID_FreeFall = 11,
    BehaviourID_HazardAwareness = 12,
    BehaviourID_HeadAvoid = 13,
    BehaviourID_HeadDodge = 14,
    BehaviourID_Hold = 15,
    BehaviourID_HoldAction = 16,
    BehaviourID_IdleAwake = 17,
    BehaviourID_LegsPedal = 18,
    BehaviourID_Look = 19,
    BehaviourID_Observe = 20,
    BehaviourID_Properties = 21,
    BehaviourID_Protect = 22,
    BehaviourID_ReachForBody = 23,
    BehaviourID_ReachForWorld = 24,
    BehaviourID_Shield = 25,
    BehaviourID_ShieldAction = 26,
    BehaviourID_Sit = 27,
    BehaviourID_UserHazard = 28,
    BehaviourID_Writhe = 29,
    // -------------
    NumBehaviours
  };
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_NET_DESC_H

