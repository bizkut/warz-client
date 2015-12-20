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

#ifndef NM_MDF_DATA_MEM_H
#define NM_MDF_DATA_MEM_H

#include "NMPlatform/NMPlatform.h"
#include "AimBehaviourInterfaceData.h"
#include "AnimateBehaviourInterfaceData.h"
#include "ArmsBraceBehaviourInterfaceData.h"
#include "ArmsPlacementBehaviourInterfaceData.h"
#include "ArmsWindmillBehaviourInterfaceData.h"
#include "BalanceBehaviourInterfaceData.h"
#include "BalancePoserBehaviourInterfaceData.h"
#include "CharacteristicsBehaviourInterfaceData.h"
#include "EuphoriaRagdollBehaviourInterfaceData.h"
#include "EyesBehaviourInterfaceData.h"
#include "FreeFallBehaviourInterfaceData.h"
#include "HazardAwarenessBehaviourInterfaceData.h"
#include "HeadAvoidBehaviourInterfaceData.h"
#include "HeadDodgeBehaviourInterfaceData.h"
#include "HoldActionBehaviourInterfaceData.h"
#include "HoldBehaviourInterfaceData.h"
#include "IdleAwakeBehaviourInterfaceData.h"
#include "LegsPedalBehaviourInterfaceData.h"
#include "LookBehaviourInterfaceData.h"
#include "ObserveBehaviourInterfaceData.h"
#include "PropertiesBehaviourInterfaceData.h"
#include "ProtectBehaviourInterfaceData.h"
#include "ReachForBodyBehaviourInterfaceData.h"
#include "ReachForWorldBehaviourInterfaceData.h"
#include "ShieldActionBehaviourInterfaceData.h"
#include "ShieldBehaviourInterfaceData.h"
#include "SitBehaviourInterfaceData.h"
#include "SittingBodyBalanceData.h"
#include "UserHazardBehaviourInterfaceData.h"
#include "WritheBehaviourInterfaceData.h"
#include "ArmData.h"
#include "ArmAimData.h"
#include "ArmBraceData.h"
#include "ArmGrabData.h"
#include "ArmHoldData.h"
#include "ArmHoldingSupportData.h"
#include "ArmPlacementData.h"
#include "ArmPoseData.h"
#include "ArmReachForBodyPartData.h"
#include "ArmReachForWorldData.h"
#include "ArmReachReactionData.h"
#include "ArmSittingStepData.h"
#include "ArmSpinData.h"
#include "ArmStandingSupportData.h"
#include "ArmStepData.h"
#include "ArmSwingData.h"
#include "ArmWritheData.h"
#include "BalanceAssistantData.h"
#include "BalanceBehaviourFeedbackData.h"
#include "BalanceManagementData.h"
#include "BalancePoserData.h"
#include "BodyBalanceData.h"
#include "BodyFrameData.h"
#include "BodySectionData.h"
#include "BraceChooserData.h"
#include "EnvironmentAwarenessData.h"
#include "FreeFallManagementData.h"
#include "GrabData.h"
#include "GrabDetectionData.h"
#include "HazardManagementData.h"
#include "HazardResponseData.h"
#include "HeadData.h"
#include "HeadAimData.h"
#include "HeadAvoidData.h"
#include "HeadDodgeData.h"
#include "HeadEyesData.h"
#include "HeadPointData.h"
#include "HeadPoseData.h"
#include "HeadSupportData.h"
#include "ImpactPredictorData.h"
#include "LegData.h"
#include "LegBraceData.h"
#include "LegPoseData.h"
#include "LegReachReactionData.h"
#include "LegSittingSupportData.h"
#include "LegSpinData.h"
#include "LegStandingSupportData.h"
#include "LegStepData.h"
#include "LegSwingData.h"
#include "LegWritheData.h"
#include "MyNetworkData.h"
#include "PositionCoreData.h"
#include "RandomLookData.h"
#include "ReachForBodyData.h"
#include "RotateCoreData.h"
#include "SceneProbesData.h"
#include "ShieldManagementData.h"
#include "SpineData.h"
#include "SpineAimData.h"
#include "SpineControlData.h"
#include "SpinePoseData.h"
#include "SpineReachReactionData.h"
#include "SpineSupportData.h"
#include "SpineWritheData.h"
#include "StaticBalanceData.h"
#include "SteppingBalanceData.h"
#include "SupportPolygonData.h"


#ifdef NM_COMPILER_MSVC
# pragma warning ( push )
# pragma warning ( disable : 4324 ) // structure was padded due to __declspec(align()) 
#endif // NM_COMPILER_MSVC

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(32) struct DataMemoryData
{
  MyNetworkData inst_rootModule;
  AimBehaviourInterfaceData inst_aimBehaviourInterface;
  ReachForBodyBehaviourInterfaceData inst_reachForBodyBehaviourInterface;
  AnimateBehaviourInterfaceData inst_animateBehaviourInterface;
  BalanceBehaviourInterfaceData inst_balanceBehaviourInterface;
  HazardAwarenessBehaviourInterfaceData inst_hazardAwarenessBehaviourInterface;
  ObserveBehaviourInterfaceData inst_observeBehaviourInterface;
  IdleAwakeBehaviourInterfaceData inst_idleAwakeBehaviourInterface;
  ShieldBehaviourInterfaceData inst_shieldBehaviourInterface;
  ShieldActionBehaviourInterfaceData inst_shieldActionBehaviourInterface;
  SitBehaviourInterfaceData inst_sitBehaviourInterface;
  RandomLookData inst_randomLook;
  GrabDetectionData inst_hazardManagement_grabDetection;
  GrabData inst_hazardManagement_grab;
  HazardResponseData inst_hazardManagement_hazardResponse;
  FreeFallManagementData inst_hazardManagement_freeFallManagement;
  ImpactPredictorData inst_hazardManagement_chestImpactPredictor;
  ShieldManagementData inst_hazardManagement_shieldManagement;
  SteppingBalanceData inst_balanceManagement_steppingBalance;
  EnvironmentAwarenessData inst_environmentAwareness;
  BodyFrameData inst_bodyFrame;
  SupportPolygonData inst_bodyFrame_supportPolygon;
  SupportPolygonData inst_bodyFrame_sittingSupportPolygon;
  BodyBalanceData inst_bodyFrame_bodyBalance;
  SittingBodyBalanceData inst_bodyFrame_sittingBodyBalance;
  ReachForBodyData inst_bodyFrame_reachForBody;
  ArmData inst_arms_0;
  ArmGrabData inst_arms_0_grab;
  ArmHoldData inst_arms_0_hold;
  ArmBraceData inst_arms_0_brace;
  ArmStandingSupportData inst_arms_0_standingSupport;
  ArmHoldingSupportData inst_arms_0_holdingSupport;
  ArmStepData inst_arms_0_step;
  ArmSpinData inst_arms_0_spin;
  ArmReachForBodyPartData inst_arms_0_reachForBodyPart;
  ArmReachForWorldData inst_arms_0_reachForWorld;
  ArmPlacementData inst_arms_0_placement;
  ArmWritheData inst_arms_0_writhe;
  ArmData inst_arms_1;
  ArmGrabData inst_arms_1_grab;
  ArmHoldData inst_arms_1_hold;
  ArmBraceData inst_arms_1_brace;
  ArmStandingSupportData inst_arms_1_standingSupport;
  ArmHoldingSupportData inst_arms_1_holdingSupport;
  ArmStepData inst_arms_1_step;
  ArmSpinData inst_arms_1_spin;
  ArmReachForBodyPartData inst_arms_1_reachForBodyPart;
  ArmReachForWorldData inst_arms_1_reachForWorld;
  ArmPlacementData inst_arms_1_placement;
  ArmWritheData inst_arms_1_writhe;
  HeadData inst_heads_0;
  HeadEyesData inst_heads_0_eyes;
  HeadAvoidData inst_heads_0_avoid;
  HeadDodgeData inst_heads_0_dodge;
  HeadPointData inst_heads_0_point;
  LegData inst_legs_0;
  LegBraceData inst_legs_0_brace;
  LegStandingSupportData inst_legs_0_standingSupport;
  LegSittingSupportData inst_legs_0_sittingSupport;
  LegStepData inst_legs_0_step;
  LegSwingData inst_legs_0_swing;
  LegSpinData inst_legs_0_spin;
  LegWritheData inst_legs_0_writhe;
  LegData inst_legs_1;
  LegBraceData inst_legs_1_brace;
  LegStandingSupportData inst_legs_1_standingSupport;
  LegSittingSupportData inst_legs_1_sittingSupport;
  LegStepData inst_legs_1_step;
  LegSwingData inst_legs_1_swing;
  LegSpinData inst_legs_1_spin;
  LegWritheData inst_legs_1_writhe;
  SpineData inst_spines_0;
  SpineWritheData inst_spines_0_writhe;
  SceneProbesData inst_sceneProbes;
} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(32) struct DataMemoryFeedbackInputs
{
  EyesBehaviourInterfaceFeedbackInputs inst_eyesBehaviourInterface;
  AimBehaviourInterfaceFeedbackInputs inst_aimBehaviourInterface;
  ReachForBodyBehaviourInterfaceFeedbackInputs inst_reachForBodyBehaviourInterface;
  ReachForWorldBehaviourInterfaceFeedbackInputs inst_reachForWorldBehaviourInterface;
  BalanceBehaviourInterfaceFeedbackInputs inst_balanceBehaviourInterface;
  BalancePoserBehaviourInterfaceFeedbackInputs inst_balancePoserBehaviourInterface;
  HazardAwarenessBehaviourInterfaceFeedbackInputs inst_hazardAwarenessBehaviourInterface;
  UserHazardBehaviourInterfaceFeedbackInputs inst_userHazardBehaviourInterface;
  ObserveBehaviourInterfaceFeedbackInputs inst_observeBehaviourInterface;
  IdleAwakeBehaviourInterfaceFeedbackInputs inst_idleAwakeBehaviourInterface;
  ShieldBehaviourInterfaceFeedbackInputs inst_shieldBehaviourInterface;
  HoldBehaviourInterfaceFeedbackInputs inst_holdBehaviourInterface;
  HoldActionBehaviourInterfaceFeedbackInputs inst_holdActionBehaviourInterface;
  FreeFallBehaviourInterfaceFeedbackInputs inst_freeFallBehaviourInterface;
  ShieldActionBehaviourInterfaceFeedbackInputs inst_shieldActionBehaviourInterface;
  SitBehaviourInterfaceFeedbackInputs inst_sitBehaviourInterface;
  RandomLookFeedbackInputs inst_randomLook;
  HazardManagementFeedbackInputs inst_hazardManagement;
  GrabDetectionFeedbackInputs inst_hazardManagement_grabDetection;
  GrabFeedbackInputs inst_hazardManagement_grab;
  HazardResponseFeedbackInputs inst_hazardManagement_hazardResponse;
  FreeFallManagementFeedbackInputs inst_hazardManagement_freeFallManagement;
  ImpactPredictorFeedbackInputs inst_hazardManagement_chestImpactPredictor;
  ShieldManagementFeedbackInputs inst_hazardManagement_shieldManagement;
  BalanceManagementFeedbackInputs inst_balanceManagement;
  StaticBalanceFeedbackInputs inst_balanceManagement_staticBalance;
  SteppingBalanceFeedbackInputs inst_balanceManagement_steppingBalance;
  EnvironmentAwarenessFeedbackInputs inst_environmentAwareness;
  BodyFrameFeedbackInputs inst_bodyFrame;
  SupportPolygonFeedbackInputs inst_bodyFrame_supportPolygon;
  SupportPolygonFeedbackInputs inst_bodyFrame_sittingSupportPolygon;
  BodyBalanceFeedbackInputs inst_bodyFrame_bodyBalance;
  SittingBodyBalanceFeedbackInputs inst_bodyFrame_sittingBodyBalance;
  ReachForBodyFeedbackInputs inst_bodyFrame_reachForBody;
  BalanceAssistantFeedbackInputs inst_bodyFrame_balanceAssistant;
  BodySectionFeedbackInputs inst_upperBody;
  RotateCoreFeedbackInputs inst_upperBody_rotate;
  PositionCoreFeedbackInputs inst_upperBody_position;
  BraceChooserFeedbackInputs inst_upperBody_braceChooser;
  BodySectionFeedbackInputs inst_lowerBody;
  RotateCoreFeedbackInputs inst_lowerBody_rotate;
  PositionCoreFeedbackInputs inst_lowerBody_position;
  BraceChooserFeedbackInputs inst_lowerBody_braceChooser;
  ArmGrabFeedbackInputs inst_arms_0_grab;
  ArmBraceFeedbackInputs inst_arms_0_brace;
  ArmSittingStepFeedbackInputs inst_arms_0_sittingStep;
  ArmReachForBodyPartFeedbackInputs inst_arms_0_reachForBodyPart;
  ArmGrabFeedbackInputs inst_arms_1_grab;
  ArmBraceFeedbackInputs inst_arms_1_brace;
  ArmSittingStepFeedbackInputs inst_arms_1_sittingStep;
  ArmReachForBodyPartFeedbackInputs inst_arms_1_reachForBodyPart;
  HeadAimFeedbackInputs inst_heads_0_aim;
  LegBraceFeedbackInputs inst_legs_0_brace;
  LegBraceFeedbackInputs inst_legs_1_brace;
  SpineSupportFeedbackInputs inst_spines_0_support;
} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(32) struct DataMemoryInputs
{
  BalanceBehaviourInterfaceInputs inst_balanceBehaviourInterface;
  RandomLookInputs inst_randomLook;
  GrabDetectionInputs inst_hazardManagement_grabDetection;
  GrabInputs inst_hazardManagement_grab;
  HazardResponseInputs inst_hazardManagement_hazardResponse;
  FreeFallManagementInputs inst_hazardManagement_freeFallManagement;
  ImpactPredictorInputs inst_hazardManagement_chestImpactPredictor;
  ShieldManagementInputs inst_hazardManagement_shieldManagement;
  BalanceManagementInputs inst_balanceManagement;
  StaticBalanceInputs inst_balanceManagement_staticBalance;
  SteppingBalanceInputs inst_balanceManagement_steppingBalance;
  BalancePoserInputs inst_balanceManagement_balancePoser;
  EnvironmentAwarenessInputs inst_environmentAwareness;
  BodyFrameInputs inst_bodyFrame;
  SupportPolygonInputs inst_bodyFrame_supportPolygon;
  SupportPolygonInputs inst_bodyFrame_sittingSupportPolygon;
  BodyBalanceInputs inst_bodyFrame_bodyBalance;
  SittingBodyBalanceInputs inst_bodyFrame_sittingBodyBalance;
  ReachForBodyInputs inst_bodyFrame_reachForBody;
  BalanceAssistantInputs inst_bodyFrame_balanceAssistant;
  BodySectionInputs inst_upperBody;
  RotateCoreInputs inst_upperBody_rotate;
  PositionCoreInputs inst_upperBody_position;
  BodySectionInputs inst_lowerBody;
  RotateCoreInputs inst_lowerBody_rotate;
  PositionCoreInputs inst_lowerBody_position;
  ArmInputs inst_arms_0;
  ArmGrabInputs inst_arms_0_grab;
  ArmAimInputs inst_arms_0_aim;
  ArmHoldInputs inst_arms_0_hold;
  ArmBraceInputs inst_arms_0_brace;
  ArmStandingSupportInputs inst_arms_0_standingSupport;
  ArmHoldingSupportInputs inst_arms_0_holdingSupport;
  ArmSittingStepInputs inst_arms_0_sittingStep;
  ArmStepInputs inst_arms_0_step;
  ArmSpinInputs inst_arms_0_spin;
  ArmSwingInputs inst_arms_0_swing;
  ArmReachForBodyPartInputs inst_arms_0_reachForBodyPart;
  ArmReachForWorldInputs inst_arms_0_reachForWorld;
  ArmPlacementInputs inst_arms_0_placement;
  ArmPoseInputs inst_arms_0_pose;
  ArmReachReactionInputs inst_arms_0_reachReaction;
  ArmWritheInputs inst_arms_0_writhe;
  ArmInputs inst_arms_1;
  ArmGrabInputs inst_arms_1_grab;
  ArmAimInputs inst_arms_1_aim;
  ArmHoldInputs inst_arms_1_hold;
  ArmBraceInputs inst_arms_1_brace;
  ArmStandingSupportInputs inst_arms_1_standingSupport;
  ArmHoldingSupportInputs inst_arms_1_holdingSupport;
  ArmSittingStepInputs inst_arms_1_sittingStep;
  ArmStepInputs inst_arms_1_step;
  ArmSpinInputs inst_arms_1_spin;
  ArmSwingInputs inst_arms_1_swing;
  ArmReachForBodyPartInputs inst_arms_1_reachForBodyPart;
  ArmReachForWorldInputs inst_arms_1_reachForWorld;
  ArmPlacementInputs inst_arms_1_placement;
  ArmPoseInputs inst_arms_1_pose;
  ArmReachReactionInputs inst_arms_1_reachReaction;
  ArmWritheInputs inst_arms_1_writhe;
  HeadInputs inst_heads_0;
  HeadEyesInputs inst_heads_0_eyes;
  HeadAvoidInputs inst_heads_0_avoid;
  HeadDodgeInputs inst_heads_0_dodge;
  HeadAimInputs inst_heads_0_aim;
  HeadPointInputs inst_heads_0_point;
  HeadSupportInputs inst_heads_0_support;
  HeadPoseInputs inst_heads_0_pose;
  LegInputs inst_legs_0;
  LegBraceInputs inst_legs_0_brace;
  LegStandingSupportInputs inst_legs_0_standingSupport;
  LegSittingSupportInputs inst_legs_0_sittingSupport;
  LegStepInputs inst_legs_0_step;
  LegSwingInputs inst_legs_0_swing;
  LegPoseInputs inst_legs_0_pose;
  LegSpinInputs inst_legs_0_spin;
  LegReachReactionInputs inst_legs_0_reachReaction;
  LegWritheInputs inst_legs_0_writhe;
  LegInputs inst_legs_1;
  LegBraceInputs inst_legs_1_brace;
  LegStandingSupportInputs inst_legs_1_standingSupport;
  LegSittingSupportInputs inst_legs_1_sittingSupport;
  LegStepInputs inst_legs_1_step;
  LegSwingInputs inst_legs_1_swing;
  LegPoseInputs inst_legs_1_pose;
  LegSpinInputs inst_legs_1_spin;
  LegReachReactionInputs inst_legs_1_reachReaction;
  LegWritheInputs inst_legs_1_writhe;
  SpineInputs inst_spines_0;
  SpineSupportInputs inst_spines_0_support;
  SpinePoseInputs inst_spines_0_pose;
  SpineControlInputs inst_spines_0_control;
  SpineReachReactionInputs inst_spines_0_reachReaction;
  SpineWritheInputs inst_spines_0_writhe;
  SpineAimInputs inst_spines_0_aim;
  SceneProbesInputs inst_sceneProbes;
} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(32) struct DataMemoryUserInputs
{
  ProtectBehaviourInterfaceUserInputs inst_protectBehaviourInterface;
  HazardAwarenessBehaviourInterfaceUserInputs inst_hazardAwarenessBehaviourInterface;
  UserHazardBehaviourInterfaceUserInputs inst_userHazardBehaviourInterface;
  HoldBehaviourInterfaceUserInputs inst_holdBehaviourInterface;
  HoldActionBehaviourInterfaceUserInputs inst_holdActionBehaviourInterface;
} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(32) struct DataMemoryFeedbackOutputs
{
  ReachForWorldBehaviourInterfaceFeedbackOutputs inst_reachForWorldBehaviourInterface;
  BalanceBehaviourInterfaceFeedbackOutputs inst_balanceBehaviourInterface;
  HazardAwarenessBehaviourInterfaceFeedbackOutputs inst_hazardAwarenessBehaviourInterface;
  ShieldBehaviourInterfaceFeedbackOutputs inst_shieldBehaviourInterface;
  HoldBehaviourInterfaceFeedbackOutputs inst_holdBehaviourInterface;
  HoldActionBehaviourInterfaceFeedbackOutputs inst_holdActionBehaviourInterface;
  FreeFallBehaviourInterfaceFeedbackOutputs inst_freeFallBehaviourInterface;
  SitBehaviourInterfaceFeedbackOutputs inst_sitBehaviourInterface;
  GrabFeedbackOutputs inst_hazardManagement_grab;
  ImpactPredictorFeedbackOutputs inst_hazardManagement_chestImpactPredictor;
  ShieldManagementFeedbackOutputs inst_hazardManagement_shieldManagement;
  SteppingBalanceFeedbackOutputs inst_balanceManagement_steppingBalance;
  EnvironmentAwarenessFeedbackOutputs inst_environmentAwareness;
  BodyFrameFeedbackOutputs inst_bodyFrame;
  BodyBalanceFeedbackOutputs inst_bodyFrame_bodyBalance;
  SittingBodyBalanceFeedbackOutputs inst_bodyFrame_sittingBodyBalance;
  ReachForBodyFeedbackOutputs inst_bodyFrame_reachForBody;
  BodySectionFeedbackOutputs inst_upperBody;
  RotateCoreFeedbackOutputs inst_upperBody_rotate;
  PositionCoreFeedbackOutputs inst_upperBody_position;
  BodySectionFeedbackOutputs inst_lowerBody;
  RotateCoreFeedbackOutputs inst_lowerBody_rotate;
  PositionCoreFeedbackOutputs inst_lowerBody_position;
  ArmFeedbackOutputs inst_arms_0;
  ArmGrabFeedbackOutputs inst_arms_0_grab;
  ArmHoldFeedbackOutputs inst_arms_0_hold;
  ArmBraceFeedbackOutputs inst_arms_0_brace;
  ArmStandingSupportFeedbackOutputs inst_arms_0_standingSupport;
  ArmHoldingSupportFeedbackOutputs inst_arms_0_holdingSupport;
  ArmSittingStepFeedbackOutputs inst_arms_0_sittingStep;
  ArmStepFeedbackOutputs inst_arms_0_step;
  ArmSpinFeedbackOutputs inst_arms_0_spin;
  ArmReachForBodyPartFeedbackOutputs inst_arms_0_reachForBodyPart;
  ArmReachForWorldFeedbackOutputs inst_arms_0_reachForWorld;
  ArmFeedbackOutputs inst_arms_1;
  ArmGrabFeedbackOutputs inst_arms_1_grab;
  ArmHoldFeedbackOutputs inst_arms_1_hold;
  ArmBraceFeedbackOutputs inst_arms_1_brace;
  ArmStandingSupportFeedbackOutputs inst_arms_1_standingSupport;
  ArmHoldingSupportFeedbackOutputs inst_arms_1_holdingSupport;
  ArmSittingStepFeedbackOutputs inst_arms_1_sittingStep;
  ArmStepFeedbackOutputs inst_arms_1_step;
  ArmSpinFeedbackOutputs inst_arms_1_spin;
  ArmReachForBodyPartFeedbackOutputs inst_arms_1_reachForBodyPart;
  ArmReachForWorldFeedbackOutputs inst_arms_1_reachForWorld;
  HeadFeedbackOutputs inst_heads_0;
  HeadEyesFeedbackOutputs inst_heads_0_eyes;
  HeadAvoidFeedbackOutputs inst_heads_0_avoid;
  HeadDodgeFeedbackOutputs inst_heads_0_dodge;
  HeadPointFeedbackOutputs inst_heads_0_point;
  LegFeedbackOutputs inst_legs_0;
  LegBraceFeedbackOutputs inst_legs_0_brace;
  LegStandingSupportFeedbackOutputs inst_legs_0_standingSupport;
  LegSittingSupportFeedbackOutputs inst_legs_0_sittingSupport;
  LegStepFeedbackOutputs inst_legs_0_step;
  LegSpinFeedbackOutputs inst_legs_0_spin;
  LegFeedbackOutputs inst_legs_1;
  LegBraceFeedbackOutputs inst_legs_1_brace;
  LegStandingSupportFeedbackOutputs inst_legs_1_standingSupport;
  LegSittingSupportFeedbackOutputs inst_legs_1_sittingSupport;
  LegStepFeedbackOutputs inst_legs_1_step;
  LegSpinFeedbackOutputs inst_legs_1_spin;
  SpineFeedbackOutputs inst_spines_0;
  SpineSupportFeedbackOutputs inst_spines_0_support;
  SceneProbesFeedbackOutputs inst_sceneProbes;
  BalanceBehaviourFeedbackFeedbackOutputs inst_balanceBehaviourFeedback;
} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(32) struct DataMemoryOutputs
{
  CharacteristicsBehaviourInterfaceOutputs inst_characteristicsBehaviourInterface;
  EuphoriaRagdollBehaviourInterfaceOutputs inst_euphoriaRagdollBehaviourInterface;
  ArmsBraceBehaviourInterfaceOutputs inst_armsBraceBehaviourInterface;
  ArmsPlacementBehaviourInterfaceOutputs inst_armsPlacementBehaviourInterface;
  AimBehaviourInterfaceOutputs inst_aimBehaviourInterface;
  HeadDodgeBehaviourInterfaceOutputs inst_headDodgeBehaviourInterface;
  ReachForBodyBehaviourInterfaceOutputs inst_reachForBodyBehaviourInterface;
  ReachForWorldBehaviourInterfaceOutputs inst_reachForWorldBehaviourInterface;
  AnimateBehaviourInterfaceOutputs inst_animateBehaviourInterface;
  BalanceBehaviourInterfaceOutputs inst_balanceBehaviourInterface;
  BalancePoserBehaviourInterfaceOutputs inst_balancePoserBehaviourInterface;
  ProtectBehaviourInterfaceOutputs inst_protectBehaviourInterface;
  HazardAwarenessBehaviourInterfaceOutputs inst_hazardAwarenessBehaviourInterface;
  UserHazardBehaviourInterfaceOutputs inst_userHazardBehaviourInterface;
  ObserveBehaviourInterfaceOutputs inst_observeBehaviourInterface;
  IdleAwakeBehaviourInterfaceOutputs inst_idleAwakeBehaviourInterface;
  LookBehaviourInterfaceOutputs inst_lookBehaviourInterface;
  HeadAvoidBehaviourInterfaceOutputs inst_headAvoidBehaviourInterface;
  ShieldBehaviourInterfaceOutputs inst_shieldBehaviourInterface;
  HoldBehaviourInterfaceOutputs inst_holdBehaviourInterface;
  HoldActionBehaviourInterfaceOutputs inst_holdActionBehaviourInterface;
  FreeFallBehaviourInterfaceOutputs inst_freeFallBehaviourInterface;
  ArmsWindmillBehaviourInterfaceOutputs inst_armsWindmillBehaviourInterface;
  LegsPedalBehaviourInterfaceOutputs inst_legsPedalBehaviourInterface;
  ShieldActionBehaviourInterfaceOutputs inst_shieldActionBehaviourInterface;
  SitBehaviourInterfaceOutputs inst_sitBehaviourInterface;
  WritheBehaviourInterfaceOutputs inst_writheBehaviourInterface;
  RandomLookOutputs inst_randomLook;
  HazardManagementOutputs inst_hazardManagement;
  GrabDetectionOutputs inst_hazardManagement_grabDetection;
  GrabOutputs inst_hazardManagement_grab;
  HazardResponseOutputs inst_hazardManagement_hazardResponse;
  FreeFallManagementOutputs inst_hazardManagement_freeFallManagement;
  ImpactPredictorOutputs inst_hazardManagement_chestImpactPredictor;
  ShieldManagementOutputs inst_hazardManagement_shieldManagement;
  BalanceManagementOutputs inst_balanceManagement;
  StaticBalanceOutputs inst_balanceManagement_staticBalance;
  SteppingBalanceOutputs inst_balanceManagement_steppingBalance;
  BalancePoserOutputs inst_balanceManagement_balancePoser;
  EnvironmentAwarenessOutputs inst_environmentAwareness;
  BodyFrameOutputs inst_bodyFrame;
  SupportPolygonOutputs inst_bodyFrame_supportPolygon;
  SupportPolygonOutputs inst_bodyFrame_sittingSupportPolygon;
  BodyBalanceOutputs inst_bodyFrame_bodyBalance;
  SittingBodyBalanceOutputs inst_bodyFrame_sittingBodyBalance;
  ReachForBodyOutputs inst_bodyFrame_reachForBody;
  BodySectionOutputs inst_upperBody;
  RotateCoreOutputs inst_upperBody_rotate;
  PositionCoreOutputs inst_upperBody_position;
  BraceChooserOutputs inst_upperBody_braceChooser;
  BodySectionOutputs inst_lowerBody;
  RotateCoreOutputs inst_lowerBody_rotate;
  PositionCoreOutputs inst_lowerBody_position;
  BraceChooserOutputs inst_lowerBody_braceChooser;
  ArmOutputs inst_arms_0;
  ArmGrabOutputs inst_arms_0_grab;
  ArmAimOutputs inst_arms_0_aim;
  ArmHoldOutputs inst_arms_0_hold;
  ArmBraceOutputs inst_arms_0_brace;
  ArmStandingSupportOutputs inst_arms_0_standingSupport;
  ArmHoldingSupportOutputs inst_arms_0_holdingSupport;
  ArmSittingStepOutputs inst_arms_0_sittingStep;
  ArmStepOutputs inst_arms_0_step;
  ArmSpinOutputs inst_arms_0_spin;
  ArmSwingOutputs inst_arms_0_swing;
  ArmReachForBodyPartOutputs inst_arms_0_reachForBodyPart;
  ArmReachForWorldOutputs inst_arms_0_reachForWorld;
  ArmPlacementOutputs inst_arms_0_placement;
  ArmPoseOutputs inst_arms_0_pose;
  ArmReachReactionOutputs inst_arms_0_reachReaction;
  ArmWritheOutputs inst_arms_0_writhe;
  ArmOutputs inst_arms_1;
  ArmGrabOutputs inst_arms_1_grab;
  ArmAimOutputs inst_arms_1_aim;
  ArmHoldOutputs inst_arms_1_hold;
  ArmBraceOutputs inst_arms_1_brace;
  ArmStandingSupportOutputs inst_arms_1_standingSupport;
  ArmHoldingSupportOutputs inst_arms_1_holdingSupport;
  ArmSittingStepOutputs inst_arms_1_sittingStep;
  ArmStepOutputs inst_arms_1_step;
  ArmSpinOutputs inst_arms_1_spin;
  ArmSwingOutputs inst_arms_1_swing;
  ArmReachForBodyPartOutputs inst_arms_1_reachForBodyPart;
  ArmReachForWorldOutputs inst_arms_1_reachForWorld;
  ArmPlacementOutputs inst_arms_1_placement;
  ArmPoseOutputs inst_arms_1_pose;
  ArmReachReactionOutputs inst_arms_1_reachReaction;
  ArmWritheOutputs inst_arms_1_writhe;
  HeadOutputs inst_heads_0;
  HeadEyesOutputs inst_heads_0_eyes;
  HeadAvoidOutputs inst_heads_0_avoid;
  HeadDodgeOutputs inst_heads_0_dodge;
  HeadAimOutputs inst_heads_0_aim;
  HeadPointOutputs inst_heads_0_point;
  HeadSupportOutputs inst_heads_0_support;
  HeadPoseOutputs inst_heads_0_pose;
  LegOutputs inst_legs_0;
  LegBraceOutputs inst_legs_0_brace;
  LegStandingSupportOutputs inst_legs_0_standingSupport;
  LegSittingSupportOutputs inst_legs_0_sittingSupport;
  LegStepOutputs inst_legs_0_step;
  LegSwingOutputs inst_legs_0_swing;
  LegPoseOutputs inst_legs_0_pose;
  LegSpinOutputs inst_legs_0_spin;
  LegReachReactionOutputs inst_legs_0_reachReaction;
  LegWritheOutputs inst_legs_0_writhe;
  LegOutputs inst_legs_1;
  LegBraceOutputs inst_legs_1_brace;
  LegStandingSupportOutputs inst_legs_1_standingSupport;
  LegSittingSupportOutputs inst_legs_1_sittingSupport;
  LegStepOutputs inst_legs_1_step;
  LegSwingOutputs inst_legs_1_swing;
  LegPoseOutputs inst_legs_1_pose;
  LegSpinOutputs inst_legs_1_spin;
  LegReachReactionOutputs inst_legs_1_reachReaction;
  LegWritheOutputs inst_legs_1_writhe;
  SpineOutputs inst_spines_0;
  SpineSupportOutputs inst_spines_0_support;
  SpinePoseOutputs inst_spines_0_pose;
  SpineControlOutputs inst_spines_0_control;
  SpineReachReactionOutputs inst_spines_0_reachReaction;
  SpineWritheOutputs inst_spines_0_writhe;
  SpineAimOutputs inst_spines_0_aim;
} NMP_ALIGN_SUFFIX(32);


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_DATA_MEM_H

