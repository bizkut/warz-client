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

#include "NetworkDescriptor.h"
#include "euphoria/erModule.h"
#include "NMPlatform/NMTimer.h"

#include "AimBehaviourInterface.h"
#include "AnimateBehaviourInterface.h"
#include "ArmsBraceBehaviourInterface.h"
#include "ArmsPlacementBehaviourInterface.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "BalanceBehaviourInterface.h"
#include "BalancePoserBehaviourInterface.h"
#include "CharacteristicsBehaviourInterface.h"
#include "EuphoriaRagdollBehaviourInterface.h"
#include "EyesBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "HazardAwarenessBehaviourInterface.h"
#include "HeadAvoidBehaviourInterface.h"
#include "HeadDodgeBehaviourInterface.h"
#include "HoldActionBehaviourInterface.h"
#include "HoldBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "LegsPedalBehaviourInterface.h"
#include "LookBehaviourInterface.h"
#include "ObserveBehaviourInterface.h"
#include "PropertiesBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "ReachForBodyBehaviourInterface.h"
#include "ReachForWorldBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "SitBehaviourInterface.h"
#include "SittingBodyBalance.h"
#include "UserHazardBehaviourInterface.h"
#include "WritheBehaviourInterface.h"
#include "Arm.h"
#include "ArmAim.h"
#include "ArmBrace.h"
#include "ArmGrab.h"
#include "ArmHold.h"
#include "ArmHoldingSupport.h"
#include "ArmPlacement.h"
#include "ArmPose.h"
#include "ArmReachForBodyPart.h"
#include "ArmReachForWorld.h"
#include "ArmReachReaction.h"
#include "ArmSittingStep.h"
#include "ArmSpin.h"
#include "ArmStandingSupport.h"
#include "ArmStep.h"
#include "ArmSwing.h"
#include "ArmWrithe.h"
#include "BalanceAssistant.h"
#include "BalanceBehaviourFeedback.h"
#include "BalanceManagement.h"
#include "BalancePoser.h"
#include "BodyBalance.h"
#include "BodyFrame.h"
#include "BodySection.h"
#include "BraceChooser.h"
#include "EnvironmentAwareness.h"
#include "FreeFallManagement.h"
#include "Grab.h"
#include "GrabDetection.h"
#include "HazardManagement.h"
#include "HazardResponse.h"
#include "Head.h"
#include "HeadAim.h"
#include "HeadAvoid.h"
#include "HeadDodge.h"
#include "HeadEyes.h"
#include "HeadPoint.h"
#include "HeadPose.h"
#include "HeadSupport.h"
#include "ImpactPredictor.h"
#include "Leg.h"
#include "LegBrace.h"
#include "LegPose.h"
#include "LegReachReaction.h"
#include "LegSittingSupport.h"
#include "LegSpin.h"
#include "LegStandingSupport.h"
#include "LegStep.h"
#include "LegSwing.h"
#include "LegWrithe.h"
#include "MyNetwork.h"
#include "PositionCore.h"
#include "RandomLook.h"
#include "ReachForBody.h"
#include "RotateCore.h"
#include "SceneProbes.h"
#include "ShieldManagement.h"
#include "Spine.h"
#include "SpineAim.h"
#include "SpineControl.h"
#include "SpinePose.h"
#include "SpineReachReaction.h"
#include "SpineSupport.h"
#include "SpineWrithe.h"
#include "StaticBalance.h"
#include "SteppingBalance.h"
#include "SupportPolygon.h"
//----------------------------------------------------------------------------------------------------------------------
#define TIMETYPE float
TIMETYPE g_profilingTimerFraction = TIMETYPE(1.0); // amount to update the timings by each update - for a rolling average
TIMETYPE g_profilingTotalTimerFraction = TIMETYPE(1.0); // amount to update the total timings by each update - for a rolling average
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

#ifdef EUPHORIA_PROFILING
TIMETYPE timer_update_rootModule[3] = {0,0,0};
TIMETYPE timer_feedback_rootModule[3] = {0,0,0};
TIMETYPE timer_update_characteristicsBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_characteristicsBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_propertiesBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_propertiesBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_euphoriaRagdollBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_euphoriaRagdollBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_eyesBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_eyesBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_armsBraceBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_armsBraceBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_armsPlacementBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_armsPlacementBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_aimBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_aimBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_headDodgeBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_headDodgeBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_reachForBodyBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_reachForBodyBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_reachForWorldBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_reachForWorldBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_animateBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_animateBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_balanceBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_balanceBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_balancePoserBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_balancePoserBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_protectBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_protectBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_hazardAwarenessBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_hazardAwarenessBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_userHazardBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_userHazardBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_observeBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_observeBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_idleAwakeBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_idleAwakeBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_lookBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_lookBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_headAvoidBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_headAvoidBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_shieldBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_shieldBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_holdBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_holdBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_holdActionBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_holdActionBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_freeFallBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_freeFallBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_armsWindmillBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_armsWindmillBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_legsPedalBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_legsPedalBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_shieldActionBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_shieldActionBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_sitBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_sitBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_writheBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_feedback_writheBehaviourInterface[3] = {0,0,0};
TIMETYPE timer_update_randomLook[3] = {0,0,0};
TIMETYPE timer_feedback_randomLook[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement_grabDetection[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement_grabDetection[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement_grab[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement_grab[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement_hazardResponse[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement_hazardResponse[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement_freeFallManagement[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement_freeFallManagement[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement_chestImpactPredictor[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement_chestImpactPredictor[3] = {0,0,0};
TIMETYPE timer_update_hazardManagement_shieldManagement[3] = {0,0,0};
TIMETYPE timer_feedback_hazardManagement_shieldManagement[3] = {0,0,0};
TIMETYPE timer_update_balanceManagement[3] = {0,0,0};
TIMETYPE timer_feedback_balanceManagement[3] = {0,0,0};
TIMETYPE timer_update_balanceManagement_staticBalance[3] = {0,0,0};
TIMETYPE timer_feedback_balanceManagement_staticBalance[3] = {0,0,0};
TIMETYPE timer_update_balanceManagement_steppingBalance[3] = {0,0,0};
TIMETYPE timer_feedback_balanceManagement_steppingBalance[3] = {0,0,0};
TIMETYPE timer_update_balanceManagement_balancePoser[3] = {0,0,0};
TIMETYPE timer_feedback_balanceManagement_balancePoser[3] = {0,0,0};
TIMETYPE timer_update_environmentAwareness[3] = {0,0,0};
TIMETYPE timer_feedback_environmentAwareness[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame_supportPolygon[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame_supportPolygon[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame_sittingSupportPolygon[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame_sittingSupportPolygon[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame_bodyBalance[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame_bodyBalance[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame_sittingBodyBalance[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame_sittingBodyBalance[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame_reachForBody[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame_reachForBody[3] = {0,0,0};
TIMETYPE timer_update_bodyFrame_balanceAssistant[3] = {0,0,0};
TIMETYPE timer_feedback_bodyFrame_balanceAssistant[3] = {0,0,0};
TIMETYPE timer_update_upperBody[3] = {0,0,0};
TIMETYPE timer_feedback_upperBody[3] = {0,0,0};
TIMETYPE timer_update_upperBody_rotate[3] = {0,0,0};
TIMETYPE timer_feedback_upperBody_rotate[3] = {0,0,0};
TIMETYPE timer_update_upperBody_position[3] = {0,0,0};
TIMETYPE timer_feedback_upperBody_position[3] = {0,0,0};
TIMETYPE timer_update_upperBody_braceChooser[3] = {0,0,0};
TIMETYPE timer_feedback_upperBody_braceChooser[3] = {0,0,0};
TIMETYPE timer_update_lowerBody[3] = {0,0,0};
TIMETYPE timer_feedback_lowerBody[3] = {0,0,0};
TIMETYPE timer_update_lowerBody_rotate[3] = {0,0,0};
TIMETYPE timer_feedback_lowerBody_rotate[3] = {0,0,0};
TIMETYPE timer_update_lowerBody_position[3] = {0,0,0};
TIMETYPE timer_feedback_lowerBody_position[3] = {0,0,0};
TIMETYPE timer_update_lowerBody_braceChooser[3] = {0,0,0};
TIMETYPE timer_feedback_lowerBody_braceChooser[3] = {0,0,0};
TIMETYPE timer_update_arms_0[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0[3] = {0,0,0};
TIMETYPE timer_update_arms_0_grab[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_grab[3] = {0,0,0};
TIMETYPE timer_update_arms_0_aim[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_aim[3] = {0,0,0};
TIMETYPE timer_update_arms_0_hold[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_hold[3] = {0,0,0};
TIMETYPE timer_update_arms_0_brace[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_brace[3] = {0,0,0};
TIMETYPE timer_update_arms_0_standingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_standingSupport[3] = {0,0,0};
TIMETYPE timer_update_arms_0_holdingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_holdingSupport[3] = {0,0,0};
TIMETYPE timer_update_arms_0_sittingStep[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_sittingStep[3] = {0,0,0};
TIMETYPE timer_update_arms_0_step[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_step[3] = {0,0,0};
TIMETYPE timer_update_arms_0_spin[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_spin[3] = {0,0,0};
TIMETYPE timer_update_arms_0_swing[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_swing[3] = {0,0,0};
TIMETYPE timer_update_arms_0_reachForBodyPart[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_reachForBodyPart[3] = {0,0,0};
TIMETYPE timer_update_arms_0_reachForWorld[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_reachForWorld[3] = {0,0,0};
TIMETYPE timer_update_arms_0_placement[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_placement[3] = {0,0,0};
TIMETYPE timer_update_arms_0_pose[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_pose[3] = {0,0,0};
TIMETYPE timer_update_arms_0_reachReaction[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_reachReaction[3] = {0,0,0};
TIMETYPE timer_update_arms_0_writhe[3] = {0,0,0};
TIMETYPE timer_feedback_arms_0_writhe[3] = {0,0,0};
TIMETYPE timer_update_arms_1[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1[3] = {0,0,0};
TIMETYPE timer_update_arms_1_grab[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_grab[3] = {0,0,0};
TIMETYPE timer_update_arms_1_aim[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_aim[3] = {0,0,0};
TIMETYPE timer_update_arms_1_hold[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_hold[3] = {0,0,0};
TIMETYPE timer_update_arms_1_brace[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_brace[3] = {0,0,0};
TIMETYPE timer_update_arms_1_standingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_standingSupport[3] = {0,0,0};
TIMETYPE timer_update_arms_1_holdingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_holdingSupport[3] = {0,0,0};
TIMETYPE timer_update_arms_1_sittingStep[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_sittingStep[3] = {0,0,0};
TIMETYPE timer_update_arms_1_step[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_step[3] = {0,0,0};
TIMETYPE timer_update_arms_1_spin[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_spin[3] = {0,0,0};
TIMETYPE timer_update_arms_1_swing[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_swing[3] = {0,0,0};
TIMETYPE timer_update_arms_1_reachForBodyPart[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_reachForBodyPart[3] = {0,0,0};
TIMETYPE timer_update_arms_1_reachForWorld[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_reachForWorld[3] = {0,0,0};
TIMETYPE timer_update_arms_1_placement[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_placement[3] = {0,0,0};
TIMETYPE timer_update_arms_1_pose[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_pose[3] = {0,0,0};
TIMETYPE timer_update_arms_1_reachReaction[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_reachReaction[3] = {0,0,0};
TIMETYPE timer_update_arms_1_writhe[3] = {0,0,0};
TIMETYPE timer_feedback_arms_1_writhe[3] = {0,0,0};
TIMETYPE timer_update_heads_0[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0[3] = {0,0,0};
TIMETYPE timer_update_heads_0_eyes[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_eyes[3] = {0,0,0};
TIMETYPE timer_update_heads_0_avoid[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_avoid[3] = {0,0,0};
TIMETYPE timer_update_heads_0_dodge[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_dodge[3] = {0,0,0};
TIMETYPE timer_update_heads_0_aim[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_aim[3] = {0,0,0};
TIMETYPE timer_update_heads_0_point[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_point[3] = {0,0,0};
TIMETYPE timer_update_heads_0_support[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_support[3] = {0,0,0};
TIMETYPE timer_update_heads_0_pose[3] = {0,0,0};
TIMETYPE timer_feedback_heads_0_pose[3] = {0,0,0};
TIMETYPE timer_update_legs_0[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0[3] = {0,0,0};
TIMETYPE timer_update_legs_0_brace[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_brace[3] = {0,0,0};
TIMETYPE timer_update_legs_0_standingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_standingSupport[3] = {0,0,0};
TIMETYPE timer_update_legs_0_sittingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_sittingSupport[3] = {0,0,0};
TIMETYPE timer_update_legs_0_step[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_step[3] = {0,0,0};
TIMETYPE timer_update_legs_0_swing[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_swing[3] = {0,0,0};
TIMETYPE timer_update_legs_0_pose[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_pose[3] = {0,0,0};
TIMETYPE timer_update_legs_0_spin[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_spin[3] = {0,0,0};
TIMETYPE timer_update_legs_0_reachReaction[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_reachReaction[3] = {0,0,0};
TIMETYPE timer_update_legs_0_writhe[3] = {0,0,0};
TIMETYPE timer_feedback_legs_0_writhe[3] = {0,0,0};
TIMETYPE timer_update_legs_1[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1[3] = {0,0,0};
TIMETYPE timer_update_legs_1_brace[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_brace[3] = {0,0,0};
TIMETYPE timer_update_legs_1_standingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_standingSupport[3] = {0,0,0};
TIMETYPE timer_update_legs_1_sittingSupport[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_sittingSupport[3] = {0,0,0};
TIMETYPE timer_update_legs_1_step[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_step[3] = {0,0,0};
TIMETYPE timer_update_legs_1_swing[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_swing[3] = {0,0,0};
TIMETYPE timer_update_legs_1_pose[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_pose[3] = {0,0,0};
TIMETYPE timer_update_legs_1_spin[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_spin[3] = {0,0,0};
TIMETYPE timer_update_legs_1_reachReaction[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_reachReaction[3] = {0,0,0};
TIMETYPE timer_update_legs_1_writhe[3] = {0,0,0};
TIMETYPE timer_feedback_legs_1_writhe[3] = {0,0,0};
TIMETYPE timer_update_spines_0[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0[3] = {0,0,0};
TIMETYPE timer_update_spines_0_support[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0_support[3] = {0,0,0};
TIMETYPE timer_update_spines_0_pose[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0_pose[3] = {0,0,0};
TIMETYPE timer_update_spines_0_control[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0_control[3] = {0,0,0};
TIMETYPE timer_update_spines_0_reachReaction[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0_reachReaction[3] = {0,0,0};
TIMETYPE timer_update_spines_0_writhe[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0_writhe[3] = {0,0,0};
TIMETYPE timer_update_spines_0_aim[3] = {0,0,0};
TIMETYPE timer_feedback_spines_0_aim[3] = {0,0,0};
TIMETYPE timer_update_sceneProbes[3] = {0,0,0};
TIMETYPE timer_feedback_sceneProbes[3] = {0,0,0};
TIMETYPE timer_update_balanceBehaviourFeedback[3] = {0,0,0};
TIMETYPE timer_feedback_balanceBehaviourFeedback[3] = {0,0,0};
TIMETYPE timer_update_total[3] = {0,0,0};
TIMETYPE timer_feedback_total[3] = {0,0,0};
TIMETYPE timer_total[3] = {0,0,0};
#endif // EUPHORIA_PROFILING

void MyNetwork::executeNetworkUpdate(float timeStep)
{
#ifdef EUPHORIA_PROFILING
  LARGE_INTEGER TicksPerSecond;
  QueryPerformanceFrequency( &TicksPerSecond );
  TIMETYPE dMicrosecondsPerTick = (TIMETYPE) (double(1.0) / ((double)TicksPerSecond.QuadPart * 0.000001));
  LARGE_INTEGER StartTimer, EndTimer;
  __int64 UpdateTicks;
#endif // EUPHORIA_PROFILING
  {
    CharacteristicsBehaviourInterface* module_characteristicsBehaviourInterface = (CharacteristicsBehaviourInterface*)getModule(NetworkManifest::characteristicsBehaviourInterface);
    if (isEnabled(NetworkManifest::characteristicsBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_characteristicsBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_characteristicsBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_characteristicsBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    PropertiesBehaviourInterface* module_propertiesBehaviourInterface = (PropertiesBehaviourInterface*)getModule(NetworkManifest::propertiesBehaviourInterface);
    if (isEnabled(NetworkManifest::propertiesBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_propertiesBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_propertiesBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_propertiesBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    EuphoriaRagdollBehaviourInterface* module_euphoriaRagdollBehaviourInterface = (EuphoriaRagdollBehaviourInterface*)getModule(NetworkManifest::euphoriaRagdollBehaviourInterface);
    if (isEnabled(NetworkManifest::euphoriaRagdollBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_euphoriaRagdollBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_euphoriaRagdollBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_euphoriaRagdollBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ArmsBraceBehaviourInterface* module_armsBraceBehaviourInterface = (ArmsBraceBehaviourInterface*)getModule(NetworkManifest::armsBraceBehaviourInterface);
    if (isEnabled(NetworkManifest::armsBraceBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_armsBraceBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_armsBraceBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_armsBraceBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ArmsPlacementBehaviourInterface* module_armsPlacementBehaviourInterface = (ArmsPlacementBehaviourInterface*)getModule(NetworkManifest::armsPlacementBehaviourInterface);
    if (isEnabled(NetworkManifest::armsPlacementBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_armsPlacementBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_armsPlacementBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_armsPlacementBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    AimBehaviourInterface* module_aimBehaviourInterface = (AimBehaviourInterface*)getModule(NetworkManifest::aimBehaviourInterface);
    if (isEnabled(NetworkManifest::aimBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_aimBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_aimBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_aimBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HeadDodgeBehaviourInterface* module_headDodgeBehaviourInterface = (HeadDodgeBehaviourInterface*)getModule(NetworkManifest::headDodgeBehaviourInterface);
    if (isEnabled(NetworkManifest::headDodgeBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_headDodgeBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_headDodgeBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_headDodgeBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ReachForBodyBehaviourInterface* module_reachForBodyBehaviourInterface = (ReachForBodyBehaviourInterface*)getModule(NetworkManifest::reachForBodyBehaviourInterface);
    if (isEnabled(NetworkManifest::reachForBodyBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_reachForBodyBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_reachForBodyBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_reachForBodyBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ReachForWorldBehaviourInterface* module_reachForWorldBehaviourInterface = (ReachForWorldBehaviourInterface*)getModule(NetworkManifest::reachForWorldBehaviourInterface);
    if (isEnabled(NetworkManifest::reachForWorldBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_reachForWorldBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_reachForWorldBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_reachForWorldBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    AnimateBehaviourInterface* module_animateBehaviourInterface = (AnimateBehaviourInterface*)getModule(NetworkManifest::animateBehaviourInterface);
    if (isEnabled(NetworkManifest::animateBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_animateBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_animateBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_animateBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    BalancePoserBehaviourInterface* module_balancePoserBehaviourInterface = (BalancePoserBehaviourInterface*)getModule(NetworkManifest::balancePoserBehaviourInterface);
    if (isEnabled(NetworkManifest::balancePoserBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balancePoserBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_balancePoserBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balancePoserBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ProtectBehaviourInterface* module_protectBehaviourInterface = (ProtectBehaviourInterface*)getModule(NetworkManifest::protectBehaviourInterface);
    if (isEnabled(NetworkManifest::protectBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_protectBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_protectBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_protectBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HazardAwarenessBehaviourInterface* module_hazardAwarenessBehaviourInterface = (HazardAwarenessBehaviourInterface*)getModule(NetworkManifest::hazardAwarenessBehaviourInterface);
    if (isEnabled(NetworkManifest::hazardAwarenessBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_hazardAwarenessBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_hazardAwarenessBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardAwarenessBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    UserHazardBehaviourInterface* module_userHazardBehaviourInterface = (UserHazardBehaviourInterface*)getModule(NetworkManifest::userHazardBehaviourInterface);
    if (isEnabled(NetworkManifest::userHazardBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_userHazardBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_userHazardBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_userHazardBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ObserveBehaviourInterface* module_observeBehaviourInterface = (ObserveBehaviourInterface*)getModule(NetworkManifest::observeBehaviourInterface);
    if (isEnabled(NetworkManifest::observeBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_observeBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_observeBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_observeBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    IdleAwakeBehaviourInterface* module_idleAwakeBehaviourInterface = (IdleAwakeBehaviourInterface*)getModule(NetworkManifest::idleAwakeBehaviourInterface);
    if (isEnabled(NetworkManifest::idleAwakeBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_idleAwakeBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_idleAwakeBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_idleAwakeBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    LookBehaviourInterface* module_lookBehaviourInterface = (LookBehaviourInterface*)getModule(NetworkManifest::lookBehaviourInterface);
    if (isEnabled(NetworkManifest::lookBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_lookBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_lookBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lookBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HeadAvoidBehaviourInterface* module_headAvoidBehaviourInterface = (HeadAvoidBehaviourInterface*)getModule(NetworkManifest::headAvoidBehaviourInterface);
    if (isEnabled(NetworkManifest::headAvoidBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_headAvoidBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_headAvoidBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_headAvoidBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ShieldBehaviourInterface* module_shieldBehaviourInterface = (ShieldBehaviourInterface*)getModule(NetworkManifest::shieldBehaviourInterface);
    if (isEnabled(NetworkManifest::shieldBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_shieldBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_shieldBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_shieldBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HoldBehaviourInterface* module_holdBehaviourInterface = (HoldBehaviourInterface*)getModule(NetworkManifest::holdBehaviourInterface);
    if (isEnabled(NetworkManifest::holdBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_holdBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_holdBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_holdBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HoldActionBehaviourInterface* module_holdActionBehaviourInterface = (HoldActionBehaviourInterface*)getModule(NetworkManifest::holdActionBehaviourInterface);
    if (isEnabled(NetworkManifest::holdActionBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_holdActionBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_holdActionBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_holdActionBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    FreeFallBehaviourInterface* module_freeFallBehaviourInterface = (FreeFallBehaviourInterface*)getModule(NetworkManifest::freeFallBehaviourInterface);
    if (isEnabled(NetworkManifest::freeFallBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_freeFallBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_freeFallBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_freeFallBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ArmsWindmillBehaviourInterface* module_armsWindmillBehaviourInterface = (ArmsWindmillBehaviourInterface*)getModule(NetworkManifest::armsWindmillBehaviourInterface);
    if (isEnabled(NetworkManifest::armsWindmillBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_armsWindmillBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_armsWindmillBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_armsWindmillBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    LegsPedalBehaviourInterface* module_legsPedalBehaviourInterface = (LegsPedalBehaviourInterface*)getModule(NetworkManifest::legsPedalBehaviourInterface);
    if (isEnabled(NetworkManifest::legsPedalBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legsPedalBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legsPedalBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legsPedalBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ShieldActionBehaviourInterface* module_shieldActionBehaviourInterface = (ShieldActionBehaviourInterface*)getModule(NetworkManifest::shieldActionBehaviourInterface);
    if (isEnabled(NetworkManifest::shieldActionBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_shieldActionBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_shieldActionBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_shieldActionBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    SitBehaviourInterface* module_sitBehaviourInterface = (SitBehaviourInterface*)getModule(NetworkManifest::sitBehaviourInterface);
    if (isEnabled(NetworkManifest::sitBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sitBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_sitBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_sitBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    WritheBehaviourInterface* module_writheBehaviourInterface = (WritheBehaviourInterface*)getModule(NetworkManifest::writheBehaviourInterface);
    if (isEnabled(NetworkManifest::writheBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_writheBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_writheBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_writheBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    BalanceBehaviourInterface* module_balanceBehaviourInterface = (BalanceBehaviourInterface*)getModule(NetworkManifest::balanceBehaviourInterface);
    if (isEnabled(NetworkManifest::balanceBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceBehaviourInterface->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_balanceBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceBehaviourInterface->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_balanceBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceBehaviourInterface->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_balanceBehaviourInterface[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceBehaviourInterface[2]));
#endif // EUPHORIA_PROFILING
    }
    RandomLook* module_randomLook = (RandomLook*)getModule(NetworkManifest::randomLook);
    if (isEnabled(NetworkManifest::randomLook))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_randomLook->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_randomLook[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_randomLook[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_randomLook->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_randomLook[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_randomLook[1]));
#endif // EUPHORIA_PROFILING
    }
    HazardManagement* module_hazardManagement = (HazardManagement*)getModule(NetworkManifest::hazardManagement);
    if (isEnabled(NetworkManifest::hazardManagement))
    {
      {
        GrabDetection* module_hazardManagement_grabDetection = (GrabDetection*)getModule(NetworkManifest::hazardManagement_grabDetection);
        if (isEnabled(NetworkManifest::hazardManagement_grabDetection))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grabDetection->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_grabDetection[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_grabDetection[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grabDetection->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_grabDetection[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_grabDetection[1]));
#endif // EUPHORIA_PROFILING
        }
        HazardResponse* module_hazardManagement_hazardResponse = (HazardResponse*)getModule(NetworkManifest::hazardManagement_hazardResponse);
        if (isEnabled(NetworkManifest::hazardManagement_hazardResponse))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_hazardResponse->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_hazardResponse[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_hazardResponse[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_hazardResponse->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_hazardResponse[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_hazardResponse[1]));
#endif // EUPHORIA_PROFILING
        }
        FreeFallManagement* module_hazardManagement_freeFallManagement = (FreeFallManagement*)getModule(NetworkManifest::hazardManagement_freeFallManagement);
        if (isEnabled(NetworkManifest::hazardManagement_freeFallManagement))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_freeFallManagement->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_freeFallManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_freeFallManagement[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_freeFallManagement->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_freeFallManagement[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_freeFallManagement[1]));
#endif // EUPHORIA_PROFILING
        }
        ImpactPredictor* module_hazardManagement_chestImpactPredictor = (ImpactPredictor*)getModule(NetworkManifest::hazardManagement_chestImpactPredictor);
        if (isEnabled(NetworkManifest::hazardManagement_chestImpactPredictor))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_chestImpactPredictor->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_chestImpactPredictor[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_chestImpactPredictor[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_chestImpactPredictor->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_chestImpactPredictor[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_chestImpactPredictor[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        Grab* module_hazardManagement_grab = (Grab*)getModule(NetworkManifest::hazardManagement_grab);
        if (isEnabled(NetworkManifest::hazardManagement_grab))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grab->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_grab[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_grab[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grab->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_grab[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_grab[1]));
#endif // EUPHORIA_PROFILING
        }
        ShieldManagement* module_hazardManagement_shieldManagement = (ShieldManagement*)getModule(NetworkManifest::hazardManagement_shieldManagement);
        if (isEnabled(NetworkManifest::hazardManagement_shieldManagement))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_shieldManagement->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_shieldManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_shieldManagement[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_shieldManagement->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_hazardManagement_shieldManagement[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement_shieldManagement[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_hazardManagement->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_hazardManagement[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_hazardManagement[2]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    BalanceManagement* module_balanceManagement = (BalanceManagement*)getModule(NetworkManifest::balanceManagement);
    if (isEnabled(NetworkManifest::balanceManagement))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceManagement->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_balanceManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement[0]));
#endif // EUPHORIA_PROFILING
      {
        StaticBalance* module_balanceManagement_staticBalance = (StaticBalance*)getModule(NetworkManifest::balanceManagement_staticBalance);
        if (isEnabled(NetworkManifest::balanceManagement_staticBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_staticBalance->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_staticBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_staticBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_staticBalance->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_staticBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_staticBalance[1]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_staticBalance->combineOutputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_staticBalance[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_staticBalance[2]));
#endif // EUPHORIA_PROFILING
        }
        SteppingBalance* module_balanceManagement_steppingBalance = (SteppingBalance*)getModule(NetworkManifest::balanceManagement_steppingBalance);
        if (isEnabled(NetworkManifest::balanceManagement_steppingBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_steppingBalance->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_steppingBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_steppingBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_steppingBalance->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_steppingBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_steppingBalance[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        BalancePoser* module_balanceManagement_balancePoser = (BalancePoser*)getModule(NetworkManifest::balanceManagement_balancePoser);
        if (isEnabled(NetworkManifest::balanceManagement_balancePoser))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_balancePoser->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_balancePoser[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_balancePoser[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_balancePoser->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_balanceManagement_balancePoser[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement_balancePoser[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceManagement->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_balanceManagement[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_balanceManagement[2]));
#endif // EUPHORIA_PROFILING
    }
    EnvironmentAwareness* module_environmentAwareness = (EnvironmentAwareness*)getModule(NetworkManifest::environmentAwareness);
    if (isEnabled(NetworkManifest::environmentAwareness))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_environmentAwareness->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_environmentAwareness[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_environmentAwareness[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_environmentAwareness->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_environmentAwareness[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_environmentAwareness[1]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    BodyFrame* module_bodyFrame = (BodyFrame*)getModule(NetworkManifest::bodyFrame);
    if (isEnabled(NetworkManifest::bodyFrame))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_bodyFrame->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_bodyFrame[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame[0]));
#endif // EUPHORIA_PROFILING
      {
        SupportPolygon* module_bodyFrame_supportPolygon = (SupportPolygon*)getModule(NetworkManifest::bodyFrame_supportPolygon);
        if (isEnabled(NetworkManifest::bodyFrame_supportPolygon))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_supportPolygon->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_supportPolygon[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_supportPolygon[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_supportPolygon->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_supportPolygon[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_supportPolygon[1]));
#endif // EUPHORIA_PROFILING
        }
        SupportPolygon* module_bodyFrame_sittingSupportPolygon = (SupportPolygon*)getModule(NetworkManifest::bodyFrame_sittingSupportPolygon);
        if (isEnabled(NetworkManifest::bodyFrame_sittingSupportPolygon))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingSupportPolygon->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_sittingSupportPolygon[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_sittingSupportPolygon[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingSupportPolygon->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_sittingSupportPolygon[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_sittingSupportPolygon[1]));
#endif // EUPHORIA_PROFILING
        }
        ReachForBody* module_bodyFrame_reachForBody = (ReachForBody*)getModule(NetworkManifest::bodyFrame_reachForBody);
        if (isEnabled(NetworkManifest::bodyFrame_reachForBody))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_reachForBody->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_reachForBody[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_reachForBody[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_reachForBody->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_reachForBody[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_reachForBody[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        BodyBalance* module_bodyFrame_bodyBalance = (BodyBalance*)getModule(NetworkManifest::bodyFrame_bodyBalance);
        if (isEnabled(NetworkManifest::bodyFrame_bodyBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_bodyBalance->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_bodyBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_bodyBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_bodyBalance->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_bodyBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_bodyBalance[1]));
#endif // EUPHORIA_PROFILING
        }
        SittingBodyBalance* module_bodyFrame_sittingBodyBalance = (SittingBodyBalance*)getModule(NetworkManifest::bodyFrame_sittingBodyBalance);
        if (isEnabled(NetworkManifest::bodyFrame_sittingBodyBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingBodyBalance->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_sittingBodyBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_sittingBodyBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingBodyBalance->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_sittingBodyBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_sittingBodyBalance[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        BalanceAssistant* module_bodyFrame_balanceAssistant = (BalanceAssistant*)getModule(NetworkManifest::bodyFrame_balanceAssistant);
        if (isEnabled(NetworkManifest::bodyFrame_balanceAssistant))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_balanceAssistant->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_balanceAssistant[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_balanceAssistant[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_balanceAssistant->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_bodyFrame_balanceAssistant[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame_balanceAssistant[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_bodyFrame->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_bodyFrame[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_bodyFrame[2]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    BodySection* module_upperBody = (BodySection*)getModule(NetworkManifest::upperBody);
    if (isEnabled(NetworkManifest::upperBody))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_upperBody->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_upperBody[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody[0]));
#endif // EUPHORIA_PROFILING
      {
        RotateCore* module_upperBody_rotate = (RotateCore*)getModule(NetworkManifest::upperBody_rotate);
        if (isEnabled(NetworkManifest::upperBody_rotate))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_rotate->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_upperBody_rotate[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody_rotate[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_rotate->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_upperBody_rotate[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody_rotate[1]));
#endif // EUPHORIA_PROFILING
        }
        PositionCore* module_upperBody_position = (PositionCore*)getModule(NetworkManifest::upperBody_position);
        if (isEnabled(NetworkManifest::upperBody_position))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_position->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_upperBody_position[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody_position[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_position->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_upperBody_position[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody_position[1]));
#endif // EUPHORIA_PROFILING
        }
        BraceChooser* module_upperBody_braceChooser = (BraceChooser*)getModule(NetworkManifest::upperBody_braceChooser);
        if (isEnabled(NetworkManifest::upperBody_braceChooser))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_braceChooser->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_upperBody_braceChooser[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody_braceChooser[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_upperBody->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_upperBody[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_upperBody[2]));
#endif // EUPHORIA_PROFILING
    }
    BodySection* module_lowerBody = (BodySection*)getModule(NetworkManifest::lowerBody);
    if (isEnabled(NetworkManifest::lowerBody))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_lowerBody->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_lowerBody[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody[0]));
#endif // EUPHORIA_PROFILING
      {
        RotateCore* module_lowerBody_rotate = (RotateCore*)getModule(NetworkManifest::lowerBody_rotate);
        if (isEnabled(NetworkManifest::lowerBody_rotate))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_rotate->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_lowerBody_rotate[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody_rotate[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_rotate->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_lowerBody_rotate[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody_rotate[1]));
#endif // EUPHORIA_PROFILING
        }
        PositionCore* module_lowerBody_position = (PositionCore*)getModule(NetworkManifest::lowerBody_position);
        if (isEnabled(NetworkManifest::lowerBody_position))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_position->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_lowerBody_position[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody_position[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_position->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_lowerBody_position[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody_position[1]));
#endif // EUPHORIA_PROFILING
        }
        BraceChooser* module_lowerBody_braceChooser = (BraceChooser*)getModule(NetworkManifest::lowerBody_braceChooser);
        if (isEnabled(NetworkManifest::lowerBody_braceChooser))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_braceChooser->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_lowerBody_braceChooser[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody_braceChooser[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_lowerBody->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_lowerBody[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_lowerBody[2]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    Leg* module_legs_0 = (Leg*)getModule(NetworkManifest::legs_0);
    if (isEnabled(NetworkManifest::legs_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_0->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legs_0[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_0->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legs_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0[1]));
#endif // EUPHORIA_PROFILING
      {
        LegBrace* module_legs_0_brace = (LegBrace*)getModule(NetworkManifest::legs_0_brace);
        if (isEnabled(NetworkManifest::legs_0_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_brace->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_brace[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_brace[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_brace->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStandingSupport* module_legs_0_standingSupport = (LegStandingSupport*)getModule(NetworkManifest::legs_0_standingSupport);
        if (isEnabled(NetworkManifest::legs_0_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_standingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_standingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_standingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_standingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSittingSupport* module_legs_0_sittingSupport = (LegSittingSupport*)getModule(NetworkManifest::legs_0_sittingSupport);
        if (isEnabled(NetworkManifest::legs_0_sittingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_sittingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_sittingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_sittingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_sittingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_sittingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_sittingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStep* module_legs_0_step = (LegStep*)getModule(NetworkManifest::legs_0_step);
        if (isEnabled(NetworkManifest::legs_0_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_step->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_step[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_step[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_step->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_step[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSwing* module_legs_0_swing = (LegSwing*)getModule(NetworkManifest::legs_0_swing);
        if (isEnabled(NetworkManifest::legs_0_swing))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_swing->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_swing[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_swing[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_swing->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_swing[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_swing[1]));
#endif // EUPHORIA_PROFILING
        }
        LegPose* module_legs_0_pose = (LegPose*)getModule(NetworkManifest::legs_0_pose);
        if (isEnabled(NetworkManifest::legs_0_pose))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_pose->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_pose[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_pose[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_pose->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_pose[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_pose[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSpin* module_legs_0_spin = (LegSpin*)getModule(NetworkManifest::legs_0_spin);
        if (isEnabled(NetworkManifest::legs_0_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_spin->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_spin[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_spin[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_spin->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_spin[1]));
#endif // EUPHORIA_PROFILING
        }
        LegReachReaction* module_legs_0_reachReaction = (LegReachReaction*)getModule(NetworkManifest::legs_0_reachReaction);
        if (isEnabled(NetworkManifest::legs_0_reachReaction))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_reachReaction->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_reachReaction[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_reachReaction[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_reachReaction->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_reachReaction[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_reachReaction[1]));
#endif // EUPHORIA_PROFILING
        }
        LegWrithe* module_legs_0_writhe = (LegWrithe*)getModule(NetworkManifest::legs_0_writhe);
        if (isEnabled(NetworkManifest::legs_0_writhe))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_writhe->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_writhe[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_writhe[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_writhe->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_0_writhe[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0_writhe[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_0->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legs_0[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_0[2]));
#endif // EUPHORIA_PROFILING
    }
    Leg* module_legs_1 = (Leg*)getModule(NetworkManifest::legs_1);
    if (isEnabled(NetworkManifest::legs_1))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_1->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legs_1[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_1->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legs_1[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1[1]));
#endif // EUPHORIA_PROFILING
      {
        LegBrace* module_legs_1_brace = (LegBrace*)getModule(NetworkManifest::legs_1_brace);
        if (isEnabled(NetworkManifest::legs_1_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_brace->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_brace[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_brace[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_brace->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStandingSupport* module_legs_1_standingSupport = (LegStandingSupport*)getModule(NetworkManifest::legs_1_standingSupport);
        if (isEnabled(NetworkManifest::legs_1_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_standingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_standingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_standingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_standingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSittingSupport* module_legs_1_sittingSupport = (LegSittingSupport*)getModule(NetworkManifest::legs_1_sittingSupport);
        if (isEnabled(NetworkManifest::legs_1_sittingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_sittingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_sittingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_sittingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_sittingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_sittingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_sittingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStep* module_legs_1_step = (LegStep*)getModule(NetworkManifest::legs_1_step);
        if (isEnabled(NetworkManifest::legs_1_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_step->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_step[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_step[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_step->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_step[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSwing* module_legs_1_swing = (LegSwing*)getModule(NetworkManifest::legs_1_swing);
        if (isEnabled(NetworkManifest::legs_1_swing))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_swing->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_swing[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_swing[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_swing->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_swing[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_swing[1]));
#endif // EUPHORIA_PROFILING
        }
        LegPose* module_legs_1_pose = (LegPose*)getModule(NetworkManifest::legs_1_pose);
        if (isEnabled(NetworkManifest::legs_1_pose))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_pose->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_pose[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_pose[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_pose->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_pose[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_pose[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSpin* module_legs_1_spin = (LegSpin*)getModule(NetworkManifest::legs_1_spin);
        if (isEnabled(NetworkManifest::legs_1_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_spin->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_spin[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_spin[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_spin->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_spin[1]));
#endif // EUPHORIA_PROFILING
        }
        LegReachReaction* module_legs_1_reachReaction = (LegReachReaction*)getModule(NetworkManifest::legs_1_reachReaction);
        if (isEnabled(NetworkManifest::legs_1_reachReaction))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_reachReaction->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_reachReaction[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_reachReaction[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_reachReaction->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_reachReaction[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_reachReaction[1]));
#endif // EUPHORIA_PROFILING
        }
        LegWrithe* module_legs_1_writhe = (LegWrithe*)getModule(NetworkManifest::legs_1_writhe);
        if (isEnabled(NetworkManifest::legs_1_writhe))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_writhe->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_writhe[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_writhe[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_writhe->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_legs_1_writhe[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1_writhe[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_1->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_legs_1[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_legs_1[2]));
#endif // EUPHORIA_PROFILING
    }
    Spine* module_spines_0 = (Spine*)getModule(NetworkManifest::spines_0);
    if (isEnabled(NetworkManifest::spines_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_spines_0->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_spines_0[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_spines_0->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_spines_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0[1]));
#endif // EUPHORIA_PROFILING
      {
        SpineSupport* module_spines_0_support = (SpineSupport*)getModule(NetworkManifest::spines_0_support);
        if (isEnabled(NetworkManifest::spines_0_support))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_support->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_support[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_support[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_support->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_support[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_support[1]));
#endif // EUPHORIA_PROFILING
        }
        SpinePose* module_spines_0_pose = (SpinePose*)getModule(NetworkManifest::spines_0_pose);
        if (isEnabled(NetworkManifest::spines_0_pose))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_pose->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_pose[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_pose[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_pose->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_pose[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_pose[1]));
#endif // EUPHORIA_PROFILING
        }
        SpineReachReaction* module_spines_0_reachReaction = (SpineReachReaction*)getModule(NetworkManifest::spines_0_reachReaction);
        if (isEnabled(NetworkManifest::spines_0_reachReaction))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_reachReaction->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_reachReaction[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_reachReaction[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_reachReaction->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_reachReaction[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_reachReaction[1]));
#endif // EUPHORIA_PROFILING
        }
        SpineWrithe* module_spines_0_writhe = (SpineWrithe*)getModule(NetworkManifest::spines_0_writhe);
        if (isEnabled(NetworkManifest::spines_0_writhe))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_writhe->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_writhe[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_writhe[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_writhe->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_writhe[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_writhe[1]));
#endif // EUPHORIA_PROFILING
        }
        SpineAim* module_spines_0_aim = (SpineAim*)getModule(NetworkManifest::spines_0_aim);
        if (isEnabled(NetworkManifest::spines_0_aim))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_aim->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_aim[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_aim[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_aim->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_aim[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_aim[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        SpineControl* module_spines_0_control = (SpineControl*)getModule(NetworkManifest::spines_0_control);
        if (isEnabled(NetworkManifest::spines_0_control))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_control->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_control[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_control[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_control->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_spines_0_control[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0_control[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_spines_0->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_spines_0[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_spines_0[2]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    Arm* module_arms_0 = (Arm*)getModule(NetworkManifest::arms_0);
    if (isEnabled(NetworkManifest::arms_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_0->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_arms_0[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_0->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_arms_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0[1]));
#endif // EUPHORIA_PROFILING
      {
        ArmGrab* module_arms_0_grab = (ArmGrab*)getModule(NetworkManifest::arms_0_grab);
        if (isEnabled(NetworkManifest::arms_0_grab))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_grab->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_grab[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_grab[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_grab->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_grab[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_grab[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmAim* module_arms_0_aim = (ArmAim*)getModule(NetworkManifest::arms_0_aim);
        if (isEnabled(NetworkManifest::arms_0_aim))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_aim->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_aim[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_aim[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_aim->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_aim[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_aim[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmBrace* module_arms_0_brace = (ArmBrace*)getModule(NetworkManifest::arms_0_brace);
        if (isEnabled(NetworkManifest::arms_0_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_brace->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_brace[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_brace[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_brace->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSittingStep* module_arms_0_sittingStep = (ArmSittingStep*)getModule(NetworkManifest::arms_0_sittingStep);
        if (isEnabled(NetworkManifest::arms_0_sittingStep))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_sittingStep->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_sittingStep[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_sittingStep[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_sittingStep->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_sittingStep[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_sittingStep[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmStep* module_arms_0_step = (ArmStep*)getModule(NetworkManifest::arms_0_step);
        if (isEnabled(NetworkManifest::arms_0_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_step->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_step[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_step[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_step->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_step[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSpin* module_arms_0_spin = (ArmSpin*)getModule(NetworkManifest::arms_0_spin);
        if (isEnabled(NetworkManifest::arms_0_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_spin->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_spin[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_spin[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_spin->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_spin[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSwing* module_arms_0_swing = (ArmSwing*)getModule(NetworkManifest::arms_0_swing);
        if (isEnabled(NetworkManifest::arms_0_swing))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_swing->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_swing[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_swing[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_swing->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_swing[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_swing[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForBodyPart* module_arms_0_reachForBodyPart = (ArmReachForBodyPart*)getModule(NetworkManifest::arms_0_reachForBodyPart);
        if (isEnabled(NetworkManifest::arms_0_reachForBodyPart))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForBodyPart->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_reachForBodyPart[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_reachForBodyPart[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForBodyPart->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_reachForBodyPart[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_reachForBodyPart[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmPlacement* module_arms_0_placement = (ArmPlacement*)getModule(NetworkManifest::arms_0_placement);
        if (isEnabled(NetworkManifest::arms_0_placement))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_placement->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_placement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_placement[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_placement->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_placement[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_placement[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmPose* module_arms_0_pose = (ArmPose*)getModule(NetworkManifest::arms_0_pose);
        if (isEnabled(NetworkManifest::arms_0_pose))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_pose->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_pose[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_pose[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_pose->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_pose[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_pose[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachReaction* module_arms_0_reachReaction = (ArmReachReaction*)getModule(NetworkManifest::arms_0_reachReaction);
        if (isEnabled(NetworkManifest::arms_0_reachReaction))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachReaction->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_reachReaction[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_reachReaction[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachReaction->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_reachReaction[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_reachReaction[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmWrithe* module_arms_0_writhe = (ArmWrithe*)getModule(NetworkManifest::arms_0_writhe);
        if (isEnabled(NetworkManifest::arms_0_writhe))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_writhe->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_writhe[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_writhe[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_writhe->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_writhe[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_writhe[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        ArmHold* module_arms_0_hold = (ArmHold*)getModule(NetworkManifest::arms_0_hold);
        if (isEnabled(NetworkManifest::arms_0_hold))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_hold->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_hold[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_hold[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_hold->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_hold[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_hold[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmStandingSupport* module_arms_0_standingSupport = (ArmStandingSupport*)getModule(NetworkManifest::arms_0_standingSupport);
        if (isEnabled(NetworkManifest::arms_0_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_standingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_standingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_standingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_standingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForWorld* module_arms_0_reachForWorld = (ArmReachForWorld*)getModule(NetworkManifest::arms_0_reachForWorld);
        if (isEnabled(NetworkManifest::arms_0_reachForWorld))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForWorld->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_reachForWorld[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_reachForWorld[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForWorld->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_reachForWorld[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_reachForWorld[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        ArmHoldingSupport* module_arms_0_holdingSupport = (ArmHoldingSupport*)getModule(NetworkManifest::arms_0_holdingSupport);
        if (isEnabled(NetworkManifest::arms_0_holdingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_holdingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_holdingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_holdingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_holdingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_0_holdingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0_holdingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_0->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_arms_0[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_0[2]));
#endif // EUPHORIA_PROFILING
    }
    Arm* module_arms_1 = (Arm*)getModule(NetworkManifest::arms_1);
    if (isEnabled(NetworkManifest::arms_1))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_1->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_arms_1[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_1->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_arms_1[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1[1]));
#endif // EUPHORIA_PROFILING
      {
        ArmGrab* module_arms_1_grab = (ArmGrab*)getModule(NetworkManifest::arms_1_grab);
        if (isEnabled(NetworkManifest::arms_1_grab))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_grab->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_grab[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_grab[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_grab->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_grab[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_grab[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmAim* module_arms_1_aim = (ArmAim*)getModule(NetworkManifest::arms_1_aim);
        if (isEnabled(NetworkManifest::arms_1_aim))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_aim->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_aim[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_aim[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_aim->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_aim[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_aim[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmBrace* module_arms_1_brace = (ArmBrace*)getModule(NetworkManifest::arms_1_brace);
        if (isEnabled(NetworkManifest::arms_1_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_brace->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_brace[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_brace[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_brace->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSittingStep* module_arms_1_sittingStep = (ArmSittingStep*)getModule(NetworkManifest::arms_1_sittingStep);
        if (isEnabled(NetworkManifest::arms_1_sittingStep))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_sittingStep->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_sittingStep[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_sittingStep[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_sittingStep->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_sittingStep[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_sittingStep[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmStep* module_arms_1_step = (ArmStep*)getModule(NetworkManifest::arms_1_step);
        if (isEnabled(NetworkManifest::arms_1_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_step->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_step[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_step[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_step->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_step[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSpin* module_arms_1_spin = (ArmSpin*)getModule(NetworkManifest::arms_1_spin);
        if (isEnabled(NetworkManifest::arms_1_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_spin->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_spin[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_spin[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_spin->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_spin[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSwing* module_arms_1_swing = (ArmSwing*)getModule(NetworkManifest::arms_1_swing);
        if (isEnabled(NetworkManifest::arms_1_swing))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_swing->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_swing[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_swing[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_swing->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_swing[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_swing[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForBodyPart* module_arms_1_reachForBodyPart = (ArmReachForBodyPart*)getModule(NetworkManifest::arms_1_reachForBodyPart);
        if (isEnabled(NetworkManifest::arms_1_reachForBodyPart))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForBodyPart->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_reachForBodyPart[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_reachForBodyPart[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForBodyPart->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_reachForBodyPart[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_reachForBodyPart[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmPlacement* module_arms_1_placement = (ArmPlacement*)getModule(NetworkManifest::arms_1_placement);
        if (isEnabled(NetworkManifest::arms_1_placement))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_placement->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_placement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_placement[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_placement->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_placement[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_placement[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmPose* module_arms_1_pose = (ArmPose*)getModule(NetworkManifest::arms_1_pose);
        if (isEnabled(NetworkManifest::arms_1_pose))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_pose->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_pose[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_pose[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_pose->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_pose[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_pose[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachReaction* module_arms_1_reachReaction = (ArmReachReaction*)getModule(NetworkManifest::arms_1_reachReaction);
        if (isEnabled(NetworkManifest::arms_1_reachReaction))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachReaction->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_reachReaction[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_reachReaction[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachReaction->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_reachReaction[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_reachReaction[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmWrithe* module_arms_1_writhe = (ArmWrithe*)getModule(NetworkManifest::arms_1_writhe);
        if (isEnabled(NetworkManifest::arms_1_writhe))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_writhe->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_writhe[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_writhe[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_writhe->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_writhe[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_writhe[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        ArmHold* module_arms_1_hold = (ArmHold*)getModule(NetworkManifest::arms_1_hold);
        if (isEnabled(NetworkManifest::arms_1_hold))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_hold->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_hold[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_hold[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_hold->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_hold[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_hold[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmStandingSupport* module_arms_1_standingSupport = (ArmStandingSupport*)getModule(NetworkManifest::arms_1_standingSupport);
        if (isEnabled(NetworkManifest::arms_1_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_standingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_standingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_standingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_standingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForWorld* module_arms_1_reachForWorld = (ArmReachForWorld*)getModule(NetworkManifest::arms_1_reachForWorld);
        if (isEnabled(NetworkManifest::arms_1_reachForWorld))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForWorld->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_reachForWorld[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_reachForWorld[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForWorld->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_reachForWorld[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_reachForWorld[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        ArmHoldingSupport* module_arms_1_holdingSupport = (ArmHoldingSupport*)getModule(NetworkManifest::arms_1_holdingSupport);
        if (isEnabled(NetworkManifest::arms_1_holdingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_holdingSupport->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_holdingSupport[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_holdingSupport[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_holdingSupport->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_arms_1_holdingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1_holdingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_1->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_arms_1[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_arms_1[2]));
#endif // EUPHORIA_PROFILING
    }
    Head* module_heads_0 = (Head*)getModule(NetworkManifest::heads_0);
    if (isEnabled(NetworkManifest::heads_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_heads_0->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_heads_0[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_heads_0->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_heads_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0[1]));
#endif // EUPHORIA_PROFILING
      {
        HeadEyes* module_heads_0_eyes = (HeadEyes*)getModule(NetworkManifest::heads_0_eyes);
        if (isEnabled(NetworkManifest::heads_0_eyes))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_eyes->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_eyes[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_eyes[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_eyes->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_eyes[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_eyes[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadAvoid* module_heads_0_avoid = (HeadAvoid*)getModule(NetworkManifest::heads_0_avoid);
        if (isEnabled(NetworkManifest::heads_0_avoid))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_avoid->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_avoid[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_avoid[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_avoid->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_avoid[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_avoid[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadDodge* module_heads_0_dodge = (HeadDodge*)getModule(NetworkManifest::heads_0_dodge);
        if (isEnabled(NetworkManifest::heads_0_dodge))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_dodge->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_dodge[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_dodge[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_dodge->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_dodge[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_dodge[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadAim* module_heads_0_aim = (HeadAim*)getModule(NetworkManifest::heads_0_aim);
        if (isEnabled(NetworkManifest::heads_0_aim))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_aim->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_aim[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_aim[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_aim->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_aim[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_aim[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadSupport* module_heads_0_support = (HeadSupport*)getModule(NetworkManifest::heads_0_support);
        if (isEnabled(NetworkManifest::heads_0_support))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_support->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_support[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_support[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_support->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_support[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_support[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadPose* module_heads_0_pose = (HeadPose*)getModule(NetworkManifest::heads_0_pose);
        if (isEnabled(NetworkManifest::heads_0_pose))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_pose->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_pose[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_pose[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_pose->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_pose[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_pose[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        HeadPoint* module_heads_0_point = (HeadPoint*)getModule(NetworkManifest::heads_0_point);
        if (isEnabled(NetworkManifest::heads_0_point))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_point->combineInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_point[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_point[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_point->update(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_update_heads_0_point[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0_point[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_heads_0->combineOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_heads_0[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_heads_0[2]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    SceneProbes* module_sceneProbes = (SceneProbes*)getModule(NetworkManifest::sceneProbes);
    if (isEnabled(NetworkManifest::sceneProbes))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sceneProbes->combineInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_sceneProbes[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_sceneProbes[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sceneProbes->update(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_update_sceneProbes[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_update_sceneProbes[1]));
#endif // EUPHORIA_PROFILING
    }
  }
#ifdef EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("\n\nUPDATE TIMINGS:");
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] = timer_update_total[1] = timer_update_total[2] = 0.0f;
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update rootModule, %f, %f, %f, %f", timer_update_rootModule[0], timer_update_rootModule[1], timer_update_rootModule[2], timer_update_rootModule[0] + timer_update_rootModule[1] + timer_update_rootModule[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_rootModule[0];
  timer_update_total[1] += timer_update_rootModule[1];
  timer_update_total[2] += timer_update_rootModule[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update characteristicsBehaviourInterface, %f, %f, %f, %f", timer_update_characteristicsBehaviourInterface[0], timer_update_characteristicsBehaviourInterface[1], timer_update_characteristicsBehaviourInterface[2], timer_update_characteristicsBehaviourInterface[0] + timer_update_characteristicsBehaviourInterface[1] + timer_update_characteristicsBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_characteristicsBehaviourInterface[0];
  timer_update_total[1] += timer_update_characteristicsBehaviourInterface[1];
  timer_update_total[2] += timer_update_characteristicsBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update propertiesBehaviourInterface, %f, %f, %f, %f", timer_update_propertiesBehaviourInterface[0], timer_update_propertiesBehaviourInterface[1], timer_update_propertiesBehaviourInterface[2], timer_update_propertiesBehaviourInterface[0] + timer_update_propertiesBehaviourInterface[1] + timer_update_propertiesBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_propertiesBehaviourInterface[0];
  timer_update_total[1] += timer_update_propertiesBehaviourInterface[1];
  timer_update_total[2] += timer_update_propertiesBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update euphoriaRagdollBehaviourInterface, %f, %f, %f, %f", timer_update_euphoriaRagdollBehaviourInterface[0], timer_update_euphoriaRagdollBehaviourInterface[1], timer_update_euphoriaRagdollBehaviourInterface[2], timer_update_euphoriaRagdollBehaviourInterface[0] + timer_update_euphoriaRagdollBehaviourInterface[1] + timer_update_euphoriaRagdollBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_euphoriaRagdollBehaviourInterface[0];
  timer_update_total[1] += timer_update_euphoriaRagdollBehaviourInterface[1];
  timer_update_total[2] += timer_update_euphoriaRagdollBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update eyesBehaviourInterface, %f, %f, %f, %f", timer_update_eyesBehaviourInterface[0], timer_update_eyesBehaviourInterface[1], timer_update_eyesBehaviourInterface[2], timer_update_eyesBehaviourInterface[0] + timer_update_eyesBehaviourInterface[1] + timer_update_eyesBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_eyesBehaviourInterface[0];
  timer_update_total[1] += timer_update_eyesBehaviourInterface[1];
  timer_update_total[2] += timer_update_eyesBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update armsBraceBehaviourInterface, %f, %f, %f, %f", timer_update_armsBraceBehaviourInterface[0], timer_update_armsBraceBehaviourInterface[1], timer_update_armsBraceBehaviourInterface[2], timer_update_armsBraceBehaviourInterface[0] + timer_update_armsBraceBehaviourInterface[1] + timer_update_armsBraceBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_armsBraceBehaviourInterface[0];
  timer_update_total[1] += timer_update_armsBraceBehaviourInterface[1];
  timer_update_total[2] += timer_update_armsBraceBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update armsPlacementBehaviourInterface, %f, %f, %f, %f", timer_update_armsPlacementBehaviourInterface[0], timer_update_armsPlacementBehaviourInterface[1], timer_update_armsPlacementBehaviourInterface[2], timer_update_armsPlacementBehaviourInterface[0] + timer_update_armsPlacementBehaviourInterface[1] + timer_update_armsPlacementBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_armsPlacementBehaviourInterface[0];
  timer_update_total[1] += timer_update_armsPlacementBehaviourInterface[1];
  timer_update_total[2] += timer_update_armsPlacementBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update aimBehaviourInterface, %f, %f, %f, %f", timer_update_aimBehaviourInterface[0], timer_update_aimBehaviourInterface[1], timer_update_aimBehaviourInterface[2], timer_update_aimBehaviourInterface[0] + timer_update_aimBehaviourInterface[1] + timer_update_aimBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_aimBehaviourInterface[0];
  timer_update_total[1] += timer_update_aimBehaviourInterface[1];
  timer_update_total[2] += timer_update_aimBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update headDodgeBehaviourInterface, %f, %f, %f, %f", timer_update_headDodgeBehaviourInterface[0], timer_update_headDodgeBehaviourInterface[1], timer_update_headDodgeBehaviourInterface[2], timer_update_headDodgeBehaviourInterface[0] + timer_update_headDodgeBehaviourInterface[1] + timer_update_headDodgeBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_headDodgeBehaviourInterface[0];
  timer_update_total[1] += timer_update_headDodgeBehaviourInterface[1];
  timer_update_total[2] += timer_update_headDodgeBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update reachForBodyBehaviourInterface, %f, %f, %f, %f", timer_update_reachForBodyBehaviourInterface[0], timer_update_reachForBodyBehaviourInterface[1], timer_update_reachForBodyBehaviourInterface[2], timer_update_reachForBodyBehaviourInterface[0] + timer_update_reachForBodyBehaviourInterface[1] + timer_update_reachForBodyBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_reachForBodyBehaviourInterface[0];
  timer_update_total[1] += timer_update_reachForBodyBehaviourInterface[1];
  timer_update_total[2] += timer_update_reachForBodyBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update reachForWorldBehaviourInterface, %f, %f, %f, %f", timer_update_reachForWorldBehaviourInterface[0], timer_update_reachForWorldBehaviourInterface[1], timer_update_reachForWorldBehaviourInterface[2], timer_update_reachForWorldBehaviourInterface[0] + timer_update_reachForWorldBehaviourInterface[1] + timer_update_reachForWorldBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_reachForWorldBehaviourInterface[0];
  timer_update_total[1] += timer_update_reachForWorldBehaviourInterface[1];
  timer_update_total[2] += timer_update_reachForWorldBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update animateBehaviourInterface, %f, %f, %f, %f", timer_update_animateBehaviourInterface[0], timer_update_animateBehaviourInterface[1], timer_update_animateBehaviourInterface[2], timer_update_animateBehaviourInterface[0] + timer_update_animateBehaviourInterface[1] + timer_update_animateBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_animateBehaviourInterface[0];
  timer_update_total[1] += timer_update_animateBehaviourInterface[1];
  timer_update_total[2] += timer_update_animateBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balanceBehaviourInterface, %f, %f, %f, %f", timer_update_balanceBehaviourInterface[0], timer_update_balanceBehaviourInterface[1], timer_update_balanceBehaviourInterface[2], timer_update_balanceBehaviourInterface[0] + timer_update_balanceBehaviourInterface[1] + timer_update_balanceBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balanceBehaviourInterface[0];
  timer_update_total[1] += timer_update_balanceBehaviourInterface[1];
  timer_update_total[2] += timer_update_balanceBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balancePoserBehaviourInterface, %f, %f, %f, %f", timer_update_balancePoserBehaviourInterface[0], timer_update_balancePoserBehaviourInterface[1], timer_update_balancePoserBehaviourInterface[2], timer_update_balancePoserBehaviourInterface[0] + timer_update_balancePoserBehaviourInterface[1] + timer_update_balancePoserBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balancePoserBehaviourInterface[0];
  timer_update_total[1] += timer_update_balancePoserBehaviourInterface[1];
  timer_update_total[2] += timer_update_balancePoserBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update protectBehaviourInterface, %f, %f, %f, %f", timer_update_protectBehaviourInterface[0], timer_update_protectBehaviourInterface[1], timer_update_protectBehaviourInterface[2], timer_update_protectBehaviourInterface[0] + timer_update_protectBehaviourInterface[1] + timer_update_protectBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_protectBehaviourInterface[0];
  timer_update_total[1] += timer_update_protectBehaviourInterface[1];
  timer_update_total[2] += timer_update_protectBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardAwarenessBehaviourInterface, %f, %f, %f, %f", timer_update_hazardAwarenessBehaviourInterface[0], timer_update_hazardAwarenessBehaviourInterface[1], timer_update_hazardAwarenessBehaviourInterface[2], timer_update_hazardAwarenessBehaviourInterface[0] + timer_update_hazardAwarenessBehaviourInterface[1] + timer_update_hazardAwarenessBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardAwarenessBehaviourInterface[0];
  timer_update_total[1] += timer_update_hazardAwarenessBehaviourInterface[1];
  timer_update_total[2] += timer_update_hazardAwarenessBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update userHazardBehaviourInterface, %f, %f, %f, %f", timer_update_userHazardBehaviourInterface[0], timer_update_userHazardBehaviourInterface[1], timer_update_userHazardBehaviourInterface[2], timer_update_userHazardBehaviourInterface[0] + timer_update_userHazardBehaviourInterface[1] + timer_update_userHazardBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_userHazardBehaviourInterface[0];
  timer_update_total[1] += timer_update_userHazardBehaviourInterface[1];
  timer_update_total[2] += timer_update_userHazardBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update observeBehaviourInterface, %f, %f, %f, %f", timer_update_observeBehaviourInterface[0], timer_update_observeBehaviourInterface[1], timer_update_observeBehaviourInterface[2], timer_update_observeBehaviourInterface[0] + timer_update_observeBehaviourInterface[1] + timer_update_observeBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_observeBehaviourInterface[0];
  timer_update_total[1] += timer_update_observeBehaviourInterface[1];
  timer_update_total[2] += timer_update_observeBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update idleAwakeBehaviourInterface, %f, %f, %f, %f", timer_update_idleAwakeBehaviourInterface[0], timer_update_idleAwakeBehaviourInterface[1], timer_update_idleAwakeBehaviourInterface[2], timer_update_idleAwakeBehaviourInterface[0] + timer_update_idleAwakeBehaviourInterface[1] + timer_update_idleAwakeBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_idleAwakeBehaviourInterface[0];
  timer_update_total[1] += timer_update_idleAwakeBehaviourInterface[1];
  timer_update_total[2] += timer_update_idleAwakeBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update lookBehaviourInterface, %f, %f, %f, %f", timer_update_lookBehaviourInterface[0], timer_update_lookBehaviourInterface[1], timer_update_lookBehaviourInterface[2], timer_update_lookBehaviourInterface[0] + timer_update_lookBehaviourInterface[1] + timer_update_lookBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_lookBehaviourInterface[0];
  timer_update_total[1] += timer_update_lookBehaviourInterface[1];
  timer_update_total[2] += timer_update_lookBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update headAvoidBehaviourInterface, %f, %f, %f, %f", timer_update_headAvoidBehaviourInterface[0], timer_update_headAvoidBehaviourInterface[1], timer_update_headAvoidBehaviourInterface[2], timer_update_headAvoidBehaviourInterface[0] + timer_update_headAvoidBehaviourInterface[1] + timer_update_headAvoidBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_headAvoidBehaviourInterface[0];
  timer_update_total[1] += timer_update_headAvoidBehaviourInterface[1];
  timer_update_total[2] += timer_update_headAvoidBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update shieldBehaviourInterface, %f, %f, %f, %f", timer_update_shieldBehaviourInterface[0], timer_update_shieldBehaviourInterface[1], timer_update_shieldBehaviourInterface[2], timer_update_shieldBehaviourInterface[0] + timer_update_shieldBehaviourInterface[1] + timer_update_shieldBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_shieldBehaviourInterface[0];
  timer_update_total[1] += timer_update_shieldBehaviourInterface[1];
  timer_update_total[2] += timer_update_shieldBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update holdBehaviourInterface, %f, %f, %f, %f", timer_update_holdBehaviourInterface[0], timer_update_holdBehaviourInterface[1], timer_update_holdBehaviourInterface[2], timer_update_holdBehaviourInterface[0] + timer_update_holdBehaviourInterface[1] + timer_update_holdBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_holdBehaviourInterface[0];
  timer_update_total[1] += timer_update_holdBehaviourInterface[1];
  timer_update_total[2] += timer_update_holdBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update holdActionBehaviourInterface, %f, %f, %f, %f", timer_update_holdActionBehaviourInterface[0], timer_update_holdActionBehaviourInterface[1], timer_update_holdActionBehaviourInterface[2], timer_update_holdActionBehaviourInterface[0] + timer_update_holdActionBehaviourInterface[1] + timer_update_holdActionBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_holdActionBehaviourInterface[0];
  timer_update_total[1] += timer_update_holdActionBehaviourInterface[1];
  timer_update_total[2] += timer_update_holdActionBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update freeFallBehaviourInterface, %f, %f, %f, %f", timer_update_freeFallBehaviourInterface[0], timer_update_freeFallBehaviourInterface[1], timer_update_freeFallBehaviourInterface[2], timer_update_freeFallBehaviourInterface[0] + timer_update_freeFallBehaviourInterface[1] + timer_update_freeFallBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_freeFallBehaviourInterface[0];
  timer_update_total[1] += timer_update_freeFallBehaviourInterface[1];
  timer_update_total[2] += timer_update_freeFallBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update armsWindmillBehaviourInterface, %f, %f, %f, %f", timer_update_armsWindmillBehaviourInterface[0], timer_update_armsWindmillBehaviourInterface[1], timer_update_armsWindmillBehaviourInterface[2], timer_update_armsWindmillBehaviourInterface[0] + timer_update_armsWindmillBehaviourInterface[1] + timer_update_armsWindmillBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_armsWindmillBehaviourInterface[0];
  timer_update_total[1] += timer_update_armsWindmillBehaviourInterface[1];
  timer_update_total[2] += timer_update_armsWindmillBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legsPedalBehaviourInterface, %f, %f, %f, %f", timer_update_legsPedalBehaviourInterface[0], timer_update_legsPedalBehaviourInterface[1], timer_update_legsPedalBehaviourInterface[2], timer_update_legsPedalBehaviourInterface[0] + timer_update_legsPedalBehaviourInterface[1] + timer_update_legsPedalBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legsPedalBehaviourInterface[0];
  timer_update_total[1] += timer_update_legsPedalBehaviourInterface[1];
  timer_update_total[2] += timer_update_legsPedalBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update shieldActionBehaviourInterface, %f, %f, %f, %f", timer_update_shieldActionBehaviourInterface[0], timer_update_shieldActionBehaviourInterface[1], timer_update_shieldActionBehaviourInterface[2], timer_update_shieldActionBehaviourInterface[0] + timer_update_shieldActionBehaviourInterface[1] + timer_update_shieldActionBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_shieldActionBehaviourInterface[0];
  timer_update_total[1] += timer_update_shieldActionBehaviourInterface[1];
  timer_update_total[2] += timer_update_shieldActionBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update sitBehaviourInterface, %f, %f, %f, %f", timer_update_sitBehaviourInterface[0], timer_update_sitBehaviourInterface[1], timer_update_sitBehaviourInterface[2], timer_update_sitBehaviourInterface[0] + timer_update_sitBehaviourInterface[1] + timer_update_sitBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_sitBehaviourInterface[0];
  timer_update_total[1] += timer_update_sitBehaviourInterface[1];
  timer_update_total[2] += timer_update_sitBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update writheBehaviourInterface, %f, %f, %f, %f", timer_update_writheBehaviourInterface[0], timer_update_writheBehaviourInterface[1], timer_update_writheBehaviourInterface[2], timer_update_writheBehaviourInterface[0] + timer_update_writheBehaviourInterface[1] + timer_update_writheBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_writheBehaviourInterface[0];
  timer_update_total[1] += timer_update_writheBehaviourInterface[1];
  timer_update_total[2] += timer_update_writheBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update randomLook, %f, %f, %f, %f", timer_update_randomLook[0], timer_update_randomLook[1], timer_update_randomLook[2], timer_update_randomLook[0] + timer_update_randomLook[1] + timer_update_randomLook[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_randomLook[0];
  timer_update_total[1] += timer_update_randomLook[1];
  timer_update_total[2] += timer_update_randomLook[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement, %f, %f, %f, %f", timer_update_hazardManagement[0], timer_update_hazardManagement[1], timer_update_hazardManagement[2], timer_update_hazardManagement[0] + timer_update_hazardManagement[1] + timer_update_hazardManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement[0];
  timer_update_total[1] += timer_update_hazardManagement[1];
  timer_update_total[2] += timer_update_hazardManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement_grabDetection, %f, %f, %f, %f", timer_update_hazardManagement_grabDetection[0], timer_update_hazardManagement_grabDetection[1], timer_update_hazardManagement_grabDetection[2], timer_update_hazardManagement_grabDetection[0] + timer_update_hazardManagement_grabDetection[1] + timer_update_hazardManagement_grabDetection[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement_grabDetection[0];
  timer_update_total[1] += timer_update_hazardManagement_grabDetection[1];
  timer_update_total[2] += timer_update_hazardManagement_grabDetection[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement_grab, %f, %f, %f, %f", timer_update_hazardManagement_grab[0], timer_update_hazardManagement_grab[1], timer_update_hazardManagement_grab[2], timer_update_hazardManagement_grab[0] + timer_update_hazardManagement_grab[1] + timer_update_hazardManagement_grab[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement_grab[0];
  timer_update_total[1] += timer_update_hazardManagement_grab[1];
  timer_update_total[2] += timer_update_hazardManagement_grab[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement_hazardResponse, %f, %f, %f, %f", timer_update_hazardManagement_hazardResponse[0], timer_update_hazardManagement_hazardResponse[1], timer_update_hazardManagement_hazardResponse[2], timer_update_hazardManagement_hazardResponse[0] + timer_update_hazardManagement_hazardResponse[1] + timer_update_hazardManagement_hazardResponse[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement_hazardResponse[0];
  timer_update_total[1] += timer_update_hazardManagement_hazardResponse[1];
  timer_update_total[2] += timer_update_hazardManagement_hazardResponse[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement_freeFallManagement, %f, %f, %f, %f", timer_update_hazardManagement_freeFallManagement[0], timer_update_hazardManagement_freeFallManagement[1], timer_update_hazardManagement_freeFallManagement[2], timer_update_hazardManagement_freeFallManagement[0] + timer_update_hazardManagement_freeFallManagement[1] + timer_update_hazardManagement_freeFallManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement_freeFallManagement[0];
  timer_update_total[1] += timer_update_hazardManagement_freeFallManagement[1];
  timer_update_total[2] += timer_update_hazardManagement_freeFallManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement_chestImpactPredictor, %f, %f, %f, %f", timer_update_hazardManagement_chestImpactPredictor[0], timer_update_hazardManagement_chestImpactPredictor[1], timer_update_hazardManagement_chestImpactPredictor[2], timer_update_hazardManagement_chestImpactPredictor[0] + timer_update_hazardManagement_chestImpactPredictor[1] + timer_update_hazardManagement_chestImpactPredictor[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement_chestImpactPredictor[0];
  timer_update_total[1] += timer_update_hazardManagement_chestImpactPredictor[1];
  timer_update_total[2] += timer_update_hazardManagement_chestImpactPredictor[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update hazardManagement_shieldManagement, %f, %f, %f, %f", timer_update_hazardManagement_shieldManagement[0], timer_update_hazardManagement_shieldManagement[1], timer_update_hazardManagement_shieldManagement[2], timer_update_hazardManagement_shieldManagement[0] + timer_update_hazardManagement_shieldManagement[1] + timer_update_hazardManagement_shieldManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_hazardManagement_shieldManagement[0];
  timer_update_total[1] += timer_update_hazardManagement_shieldManagement[1];
  timer_update_total[2] += timer_update_hazardManagement_shieldManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balanceManagement, %f, %f, %f, %f", timer_update_balanceManagement[0], timer_update_balanceManagement[1], timer_update_balanceManagement[2], timer_update_balanceManagement[0] + timer_update_balanceManagement[1] + timer_update_balanceManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balanceManagement[0];
  timer_update_total[1] += timer_update_balanceManagement[1];
  timer_update_total[2] += timer_update_balanceManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balanceManagement_staticBalance, %f, %f, %f, %f", timer_update_balanceManagement_staticBalance[0], timer_update_balanceManagement_staticBalance[1], timer_update_balanceManagement_staticBalance[2], timer_update_balanceManagement_staticBalance[0] + timer_update_balanceManagement_staticBalance[1] + timer_update_balanceManagement_staticBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balanceManagement_staticBalance[0];
  timer_update_total[1] += timer_update_balanceManagement_staticBalance[1];
  timer_update_total[2] += timer_update_balanceManagement_staticBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balanceManagement_steppingBalance, %f, %f, %f, %f", timer_update_balanceManagement_steppingBalance[0], timer_update_balanceManagement_steppingBalance[1], timer_update_balanceManagement_steppingBalance[2], timer_update_balanceManagement_steppingBalance[0] + timer_update_balanceManagement_steppingBalance[1] + timer_update_balanceManagement_steppingBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balanceManagement_steppingBalance[0];
  timer_update_total[1] += timer_update_balanceManagement_steppingBalance[1];
  timer_update_total[2] += timer_update_balanceManagement_steppingBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balanceManagement_balancePoser, %f, %f, %f, %f", timer_update_balanceManagement_balancePoser[0], timer_update_balanceManagement_balancePoser[1], timer_update_balanceManagement_balancePoser[2], timer_update_balanceManagement_balancePoser[0] + timer_update_balanceManagement_balancePoser[1] + timer_update_balanceManagement_balancePoser[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balanceManagement_balancePoser[0];
  timer_update_total[1] += timer_update_balanceManagement_balancePoser[1];
  timer_update_total[2] += timer_update_balanceManagement_balancePoser[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update environmentAwareness, %f, %f, %f, %f", timer_update_environmentAwareness[0], timer_update_environmentAwareness[1], timer_update_environmentAwareness[2], timer_update_environmentAwareness[0] + timer_update_environmentAwareness[1] + timer_update_environmentAwareness[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_environmentAwareness[0];
  timer_update_total[1] += timer_update_environmentAwareness[1];
  timer_update_total[2] += timer_update_environmentAwareness[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame, %f, %f, %f, %f", timer_update_bodyFrame[0], timer_update_bodyFrame[1], timer_update_bodyFrame[2], timer_update_bodyFrame[0] + timer_update_bodyFrame[1] + timer_update_bodyFrame[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame[0];
  timer_update_total[1] += timer_update_bodyFrame[1];
  timer_update_total[2] += timer_update_bodyFrame[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame_supportPolygon, %f, %f, %f, %f", timer_update_bodyFrame_supportPolygon[0], timer_update_bodyFrame_supportPolygon[1], timer_update_bodyFrame_supportPolygon[2], timer_update_bodyFrame_supportPolygon[0] + timer_update_bodyFrame_supportPolygon[1] + timer_update_bodyFrame_supportPolygon[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame_supportPolygon[0];
  timer_update_total[1] += timer_update_bodyFrame_supportPolygon[1];
  timer_update_total[2] += timer_update_bodyFrame_supportPolygon[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame_sittingSupportPolygon, %f, %f, %f, %f", timer_update_bodyFrame_sittingSupportPolygon[0], timer_update_bodyFrame_sittingSupportPolygon[1], timer_update_bodyFrame_sittingSupportPolygon[2], timer_update_bodyFrame_sittingSupportPolygon[0] + timer_update_bodyFrame_sittingSupportPolygon[1] + timer_update_bodyFrame_sittingSupportPolygon[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame_sittingSupportPolygon[0];
  timer_update_total[1] += timer_update_bodyFrame_sittingSupportPolygon[1];
  timer_update_total[2] += timer_update_bodyFrame_sittingSupportPolygon[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame_bodyBalance, %f, %f, %f, %f", timer_update_bodyFrame_bodyBalance[0], timer_update_bodyFrame_bodyBalance[1], timer_update_bodyFrame_bodyBalance[2], timer_update_bodyFrame_bodyBalance[0] + timer_update_bodyFrame_bodyBalance[1] + timer_update_bodyFrame_bodyBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame_bodyBalance[0];
  timer_update_total[1] += timer_update_bodyFrame_bodyBalance[1];
  timer_update_total[2] += timer_update_bodyFrame_bodyBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame_sittingBodyBalance, %f, %f, %f, %f", timer_update_bodyFrame_sittingBodyBalance[0], timer_update_bodyFrame_sittingBodyBalance[1], timer_update_bodyFrame_sittingBodyBalance[2], timer_update_bodyFrame_sittingBodyBalance[0] + timer_update_bodyFrame_sittingBodyBalance[1] + timer_update_bodyFrame_sittingBodyBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame_sittingBodyBalance[0];
  timer_update_total[1] += timer_update_bodyFrame_sittingBodyBalance[1];
  timer_update_total[2] += timer_update_bodyFrame_sittingBodyBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame_reachForBody, %f, %f, %f, %f", timer_update_bodyFrame_reachForBody[0], timer_update_bodyFrame_reachForBody[1], timer_update_bodyFrame_reachForBody[2], timer_update_bodyFrame_reachForBody[0] + timer_update_bodyFrame_reachForBody[1] + timer_update_bodyFrame_reachForBody[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame_reachForBody[0];
  timer_update_total[1] += timer_update_bodyFrame_reachForBody[1];
  timer_update_total[2] += timer_update_bodyFrame_reachForBody[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update bodyFrame_balanceAssistant, %f, %f, %f, %f", timer_update_bodyFrame_balanceAssistant[0], timer_update_bodyFrame_balanceAssistant[1], timer_update_bodyFrame_balanceAssistant[2], timer_update_bodyFrame_balanceAssistant[0] + timer_update_bodyFrame_balanceAssistant[1] + timer_update_bodyFrame_balanceAssistant[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_bodyFrame_balanceAssistant[0];
  timer_update_total[1] += timer_update_bodyFrame_balanceAssistant[1];
  timer_update_total[2] += timer_update_bodyFrame_balanceAssistant[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update upperBody, %f, %f, %f, %f", timer_update_upperBody[0], timer_update_upperBody[1], timer_update_upperBody[2], timer_update_upperBody[0] + timer_update_upperBody[1] + timer_update_upperBody[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_upperBody[0];
  timer_update_total[1] += timer_update_upperBody[1];
  timer_update_total[2] += timer_update_upperBody[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update upperBody_rotate, %f, %f, %f, %f", timer_update_upperBody_rotate[0], timer_update_upperBody_rotate[1], timer_update_upperBody_rotate[2], timer_update_upperBody_rotate[0] + timer_update_upperBody_rotate[1] + timer_update_upperBody_rotate[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_upperBody_rotate[0];
  timer_update_total[1] += timer_update_upperBody_rotate[1];
  timer_update_total[2] += timer_update_upperBody_rotate[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update upperBody_position, %f, %f, %f, %f", timer_update_upperBody_position[0], timer_update_upperBody_position[1], timer_update_upperBody_position[2], timer_update_upperBody_position[0] + timer_update_upperBody_position[1] + timer_update_upperBody_position[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_upperBody_position[0];
  timer_update_total[1] += timer_update_upperBody_position[1];
  timer_update_total[2] += timer_update_upperBody_position[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update upperBody_braceChooser, %f, %f, %f, %f", timer_update_upperBody_braceChooser[0], timer_update_upperBody_braceChooser[1], timer_update_upperBody_braceChooser[2], timer_update_upperBody_braceChooser[0] + timer_update_upperBody_braceChooser[1] + timer_update_upperBody_braceChooser[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_upperBody_braceChooser[0];
  timer_update_total[1] += timer_update_upperBody_braceChooser[1];
  timer_update_total[2] += timer_update_upperBody_braceChooser[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update lowerBody, %f, %f, %f, %f", timer_update_lowerBody[0], timer_update_lowerBody[1], timer_update_lowerBody[2], timer_update_lowerBody[0] + timer_update_lowerBody[1] + timer_update_lowerBody[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_lowerBody[0];
  timer_update_total[1] += timer_update_lowerBody[1];
  timer_update_total[2] += timer_update_lowerBody[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update lowerBody_rotate, %f, %f, %f, %f", timer_update_lowerBody_rotate[0], timer_update_lowerBody_rotate[1], timer_update_lowerBody_rotate[2], timer_update_lowerBody_rotate[0] + timer_update_lowerBody_rotate[1] + timer_update_lowerBody_rotate[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_lowerBody_rotate[0];
  timer_update_total[1] += timer_update_lowerBody_rotate[1];
  timer_update_total[2] += timer_update_lowerBody_rotate[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update lowerBody_position, %f, %f, %f, %f", timer_update_lowerBody_position[0], timer_update_lowerBody_position[1], timer_update_lowerBody_position[2], timer_update_lowerBody_position[0] + timer_update_lowerBody_position[1] + timer_update_lowerBody_position[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_lowerBody_position[0];
  timer_update_total[1] += timer_update_lowerBody_position[1];
  timer_update_total[2] += timer_update_lowerBody_position[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update lowerBody_braceChooser, %f, %f, %f, %f", timer_update_lowerBody_braceChooser[0], timer_update_lowerBody_braceChooser[1], timer_update_lowerBody_braceChooser[2], timer_update_lowerBody_braceChooser[0] + timer_update_lowerBody_braceChooser[1] + timer_update_lowerBody_braceChooser[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_lowerBody_braceChooser[0];
  timer_update_total[1] += timer_update_lowerBody_braceChooser[1];
  timer_update_total[2] += timer_update_lowerBody_braceChooser[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0, %f, %f, %f, %f", timer_update_arms_0[0], timer_update_arms_0[1], timer_update_arms_0[2], timer_update_arms_0[0] + timer_update_arms_0[1] + timer_update_arms_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0[0];
  timer_update_total[1] += timer_update_arms_0[1];
  timer_update_total[2] += timer_update_arms_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_grab, %f, %f, %f, %f", timer_update_arms_0_grab[0], timer_update_arms_0_grab[1], timer_update_arms_0_grab[2], timer_update_arms_0_grab[0] + timer_update_arms_0_grab[1] + timer_update_arms_0_grab[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_grab[0];
  timer_update_total[1] += timer_update_arms_0_grab[1];
  timer_update_total[2] += timer_update_arms_0_grab[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_aim, %f, %f, %f, %f", timer_update_arms_0_aim[0], timer_update_arms_0_aim[1], timer_update_arms_0_aim[2], timer_update_arms_0_aim[0] + timer_update_arms_0_aim[1] + timer_update_arms_0_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_aim[0];
  timer_update_total[1] += timer_update_arms_0_aim[1];
  timer_update_total[2] += timer_update_arms_0_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_hold, %f, %f, %f, %f", timer_update_arms_0_hold[0], timer_update_arms_0_hold[1], timer_update_arms_0_hold[2], timer_update_arms_0_hold[0] + timer_update_arms_0_hold[1] + timer_update_arms_0_hold[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_hold[0];
  timer_update_total[1] += timer_update_arms_0_hold[1];
  timer_update_total[2] += timer_update_arms_0_hold[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_brace, %f, %f, %f, %f", timer_update_arms_0_brace[0], timer_update_arms_0_brace[1], timer_update_arms_0_brace[2], timer_update_arms_0_brace[0] + timer_update_arms_0_brace[1] + timer_update_arms_0_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_brace[0];
  timer_update_total[1] += timer_update_arms_0_brace[1];
  timer_update_total[2] += timer_update_arms_0_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_standingSupport, %f, %f, %f, %f", timer_update_arms_0_standingSupport[0], timer_update_arms_0_standingSupport[1], timer_update_arms_0_standingSupport[2], timer_update_arms_0_standingSupport[0] + timer_update_arms_0_standingSupport[1] + timer_update_arms_0_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_standingSupport[0];
  timer_update_total[1] += timer_update_arms_0_standingSupport[1];
  timer_update_total[2] += timer_update_arms_0_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_holdingSupport, %f, %f, %f, %f", timer_update_arms_0_holdingSupport[0], timer_update_arms_0_holdingSupport[1], timer_update_arms_0_holdingSupport[2], timer_update_arms_0_holdingSupport[0] + timer_update_arms_0_holdingSupport[1] + timer_update_arms_0_holdingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_holdingSupport[0];
  timer_update_total[1] += timer_update_arms_0_holdingSupport[1];
  timer_update_total[2] += timer_update_arms_0_holdingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_sittingStep, %f, %f, %f, %f", timer_update_arms_0_sittingStep[0], timer_update_arms_0_sittingStep[1], timer_update_arms_0_sittingStep[2], timer_update_arms_0_sittingStep[0] + timer_update_arms_0_sittingStep[1] + timer_update_arms_0_sittingStep[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_sittingStep[0];
  timer_update_total[1] += timer_update_arms_0_sittingStep[1];
  timer_update_total[2] += timer_update_arms_0_sittingStep[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_step, %f, %f, %f, %f", timer_update_arms_0_step[0], timer_update_arms_0_step[1], timer_update_arms_0_step[2], timer_update_arms_0_step[0] + timer_update_arms_0_step[1] + timer_update_arms_0_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_step[0];
  timer_update_total[1] += timer_update_arms_0_step[1];
  timer_update_total[2] += timer_update_arms_0_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_spin, %f, %f, %f, %f", timer_update_arms_0_spin[0], timer_update_arms_0_spin[1], timer_update_arms_0_spin[2], timer_update_arms_0_spin[0] + timer_update_arms_0_spin[1] + timer_update_arms_0_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_spin[0];
  timer_update_total[1] += timer_update_arms_0_spin[1];
  timer_update_total[2] += timer_update_arms_0_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_swing, %f, %f, %f, %f", timer_update_arms_0_swing[0], timer_update_arms_0_swing[1], timer_update_arms_0_swing[2], timer_update_arms_0_swing[0] + timer_update_arms_0_swing[1] + timer_update_arms_0_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_swing[0];
  timer_update_total[1] += timer_update_arms_0_swing[1];
  timer_update_total[2] += timer_update_arms_0_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_reachForBodyPart, %f, %f, %f, %f", timer_update_arms_0_reachForBodyPart[0], timer_update_arms_0_reachForBodyPart[1], timer_update_arms_0_reachForBodyPart[2], timer_update_arms_0_reachForBodyPart[0] + timer_update_arms_0_reachForBodyPart[1] + timer_update_arms_0_reachForBodyPart[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_reachForBodyPart[0];
  timer_update_total[1] += timer_update_arms_0_reachForBodyPart[1];
  timer_update_total[2] += timer_update_arms_0_reachForBodyPart[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_reachForWorld, %f, %f, %f, %f", timer_update_arms_0_reachForWorld[0], timer_update_arms_0_reachForWorld[1], timer_update_arms_0_reachForWorld[2], timer_update_arms_0_reachForWorld[0] + timer_update_arms_0_reachForWorld[1] + timer_update_arms_0_reachForWorld[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_reachForWorld[0];
  timer_update_total[1] += timer_update_arms_0_reachForWorld[1];
  timer_update_total[2] += timer_update_arms_0_reachForWorld[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_placement, %f, %f, %f, %f", timer_update_arms_0_placement[0], timer_update_arms_0_placement[1], timer_update_arms_0_placement[2], timer_update_arms_0_placement[0] + timer_update_arms_0_placement[1] + timer_update_arms_0_placement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_placement[0];
  timer_update_total[1] += timer_update_arms_0_placement[1];
  timer_update_total[2] += timer_update_arms_0_placement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_pose, %f, %f, %f, %f", timer_update_arms_0_pose[0], timer_update_arms_0_pose[1], timer_update_arms_0_pose[2], timer_update_arms_0_pose[0] + timer_update_arms_0_pose[1] + timer_update_arms_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_pose[0];
  timer_update_total[1] += timer_update_arms_0_pose[1];
  timer_update_total[2] += timer_update_arms_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_reachReaction, %f, %f, %f, %f", timer_update_arms_0_reachReaction[0], timer_update_arms_0_reachReaction[1], timer_update_arms_0_reachReaction[2], timer_update_arms_0_reachReaction[0] + timer_update_arms_0_reachReaction[1] + timer_update_arms_0_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_reachReaction[0];
  timer_update_total[1] += timer_update_arms_0_reachReaction[1];
  timer_update_total[2] += timer_update_arms_0_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_0_writhe, %f, %f, %f, %f", timer_update_arms_0_writhe[0], timer_update_arms_0_writhe[1], timer_update_arms_0_writhe[2], timer_update_arms_0_writhe[0] + timer_update_arms_0_writhe[1] + timer_update_arms_0_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_0_writhe[0];
  timer_update_total[1] += timer_update_arms_0_writhe[1];
  timer_update_total[2] += timer_update_arms_0_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1, %f, %f, %f, %f", timer_update_arms_1[0], timer_update_arms_1[1], timer_update_arms_1[2], timer_update_arms_1[0] + timer_update_arms_1[1] + timer_update_arms_1[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1[0];
  timer_update_total[1] += timer_update_arms_1[1];
  timer_update_total[2] += timer_update_arms_1[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_grab, %f, %f, %f, %f", timer_update_arms_1_grab[0], timer_update_arms_1_grab[1], timer_update_arms_1_grab[2], timer_update_arms_1_grab[0] + timer_update_arms_1_grab[1] + timer_update_arms_1_grab[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_grab[0];
  timer_update_total[1] += timer_update_arms_1_grab[1];
  timer_update_total[2] += timer_update_arms_1_grab[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_aim, %f, %f, %f, %f", timer_update_arms_1_aim[0], timer_update_arms_1_aim[1], timer_update_arms_1_aim[2], timer_update_arms_1_aim[0] + timer_update_arms_1_aim[1] + timer_update_arms_1_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_aim[0];
  timer_update_total[1] += timer_update_arms_1_aim[1];
  timer_update_total[2] += timer_update_arms_1_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_hold, %f, %f, %f, %f", timer_update_arms_1_hold[0], timer_update_arms_1_hold[1], timer_update_arms_1_hold[2], timer_update_arms_1_hold[0] + timer_update_arms_1_hold[1] + timer_update_arms_1_hold[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_hold[0];
  timer_update_total[1] += timer_update_arms_1_hold[1];
  timer_update_total[2] += timer_update_arms_1_hold[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_brace, %f, %f, %f, %f", timer_update_arms_1_brace[0], timer_update_arms_1_brace[1], timer_update_arms_1_brace[2], timer_update_arms_1_brace[0] + timer_update_arms_1_brace[1] + timer_update_arms_1_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_brace[0];
  timer_update_total[1] += timer_update_arms_1_brace[1];
  timer_update_total[2] += timer_update_arms_1_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_standingSupport, %f, %f, %f, %f", timer_update_arms_1_standingSupport[0], timer_update_arms_1_standingSupport[1], timer_update_arms_1_standingSupport[2], timer_update_arms_1_standingSupport[0] + timer_update_arms_1_standingSupport[1] + timer_update_arms_1_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_standingSupport[0];
  timer_update_total[1] += timer_update_arms_1_standingSupport[1];
  timer_update_total[2] += timer_update_arms_1_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_holdingSupport, %f, %f, %f, %f", timer_update_arms_1_holdingSupport[0], timer_update_arms_1_holdingSupport[1], timer_update_arms_1_holdingSupport[2], timer_update_arms_1_holdingSupport[0] + timer_update_arms_1_holdingSupport[1] + timer_update_arms_1_holdingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_holdingSupport[0];
  timer_update_total[1] += timer_update_arms_1_holdingSupport[1];
  timer_update_total[2] += timer_update_arms_1_holdingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_sittingStep, %f, %f, %f, %f", timer_update_arms_1_sittingStep[0], timer_update_arms_1_sittingStep[1], timer_update_arms_1_sittingStep[2], timer_update_arms_1_sittingStep[0] + timer_update_arms_1_sittingStep[1] + timer_update_arms_1_sittingStep[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_sittingStep[0];
  timer_update_total[1] += timer_update_arms_1_sittingStep[1];
  timer_update_total[2] += timer_update_arms_1_sittingStep[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_step, %f, %f, %f, %f", timer_update_arms_1_step[0], timer_update_arms_1_step[1], timer_update_arms_1_step[2], timer_update_arms_1_step[0] + timer_update_arms_1_step[1] + timer_update_arms_1_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_step[0];
  timer_update_total[1] += timer_update_arms_1_step[1];
  timer_update_total[2] += timer_update_arms_1_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_spin, %f, %f, %f, %f", timer_update_arms_1_spin[0], timer_update_arms_1_spin[1], timer_update_arms_1_spin[2], timer_update_arms_1_spin[0] + timer_update_arms_1_spin[1] + timer_update_arms_1_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_spin[0];
  timer_update_total[1] += timer_update_arms_1_spin[1];
  timer_update_total[2] += timer_update_arms_1_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_swing, %f, %f, %f, %f", timer_update_arms_1_swing[0], timer_update_arms_1_swing[1], timer_update_arms_1_swing[2], timer_update_arms_1_swing[0] + timer_update_arms_1_swing[1] + timer_update_arms_1_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_swing[0];
  timer_update_total[1] += timer_update_arms_1_swing[1];
  timer_update_total[2] += timer_update_arms_1_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_reachForBodyPart, %f, %f, %f, %f", timer_update_arms_1_reachForBodyPart[0], timer_update_arms_1_reachForBodyPart[1], timer_update_arms_1_reachForBodyPart[2], timer_update_arms_1_reachForBodyPart[0] + timer_update_arms_1_reachForBodyPart[1] + timer_update_arms_1_reachForBodyPart[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_reachForBodyPart[0];
  timer_update_total[1] += timer_update_arms_1_reachForBodyPart[1];
  timer_update_total[2] += timer_update_arms_1_reachForBodyPart[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_reachForWorld, %f, %f, %f, %f", timer_update_arms_1_reachForWorld[0], timer_update_arms_1_reachForWorld[1], timer_update_arms_1_reachForWorld[2], timer_update_arms_1_reachForWorld[0] + timer_update_arms_1_reachForWorld[1] + timer_update_arms_1_reachForWorld[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_reachForWorld[0];
  timer_update_total[1] += timer_update_arms_1_reachForWorld[1];
  timer_update_total[2] += timer_update_arms_1_reachForWorld[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_placement, %f, %f, %f, %f", timer_update_arms_1_placement[0], timer_update_arms_1_placement[1], timer_update_arms_1_placement[2], timer_update_arms_1_placement[0] + timer_update_arms_1_placement[1] + timer_update_arms_1_placement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_placement[0];
  timer_update_total[1] += timer_update_arms_1_placement[1];
  timer_update_total[2] += timer_update_arms_1_placement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_pose, %f, %f, %f, %f", timer_update_arms_1_pose[0], timer_update_arms_1_pose[1], timer_update_arms_1_pose[2], timer_update_arms_1_pose[0] + timer_update_arms_1_pose[1] + timer_update_arms_1_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_pose[0];
  timer_update_total[1] += timer_update_arms_1_pose[1];
  timer_update_total[2] += timer_update_arms_1_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_reachReaction, %f, %f, %f, %f", timer_update_arms_1_reachReaction[0], timer_update_arms_1_reachReaction[1], timer_update_arms_1_reachReaction[2], timer_update_arms_1_reachReaction[0] + timer_update_arms_1_reachReaction[1] + timer_update_arms_1_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_reachReaction[0];
  timer_update_total[1] += timer_update_arms_1_reachReaction[1];
  timer_update_total[2] += timer_update_arms_1_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update arms_1_writhe, %f, %f, %f, %f", timer_update_arms_1_writhe[0], timer_update_arms_1_writhe[1], timer_update_arms_1_writhe[2], timer_update_arms_1_writhe[0] + timer_update_arms_1_writhe[1] + timer_update_arms_1_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_arms_1_writhe[0];
  timer_update_total[1] += timer_update_arms_1_writhe[1];
  timer_update_total[2] += timer_update_arms_1_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0, %f, %f, %f, %f", timer_update_heads_0[0], timer_update_heads_0[1], timer_update_heads_0[2], timer_update_heads_0[0] + timer_update_heads_0[1] + timer_update_heads_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0[0];
  timer_update_total[1] += timer_update_heads_0[1];
  timer_update_total[2] += timer_update_heads_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_eyes, %f, %f, %f, %f", timer_update_heads_0_eyes[0], timer_update_heads_0_eyes[1], timer_update_heads_0_eyes[2], timer_update_heads_0_eyes[0] + timer_update_heads_0_eyes[1] + timer_update_heads_0_eyes[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_eyes[0];
  timer_update_total[1] += timer_update_heads_0_eyes[1];
  timer_update_total[2] += timer_update_heads_0_eyes[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_avoid, %f, %f, %f, %f", timer_update_heads_0_avoid[0], timer_update_heads_0_avoid[1], timer_update_heads_0_avoid[2], timer_update_heads_0_avoid[0] + timer_update_heads_0_avoid[1] + timer_update_heads_0_avoid[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_avoid[0];
  timer_update_total[1] += timer_update_heads_0_avoid[1];
  timer_update_total[2] += timer_update_heads_0_avoid[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_dodge, %f, %f, %f, %f", timer_update_heads_0_dodge[0], timer_update_heads_0_dodge[1], timer_update_heads_0_dodge[2], timer_update_heads_0_dodge[0] + timer_update_heads_0_dodge[1] + timer_update_heads_0_dodge[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_dodge[0];
  timer_update_total[1] += timer_update_heads_0_dodge[1];
  timer_update_total[2] += timer_update_heads_0_dodge[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_aim, %f, %f, %f, %f", timer_update_heads_0_aim[0], timer_update_heads_0_aim[1], timer_update_heads_0_aim[2], timer_update_heads_0_aim[0] + timer_update_heads_0_aim[1] + timer_update_heads_0_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_aim[0];
  timer_update_total[1] += timer_update_heads_0_aim[1];
  timer_update_total[2] += timer_update_heads_0_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_point, %f, %f, %f, %f", timer_update_heads_0_point[0], timer_update_heads_0_point[1], timer_update_heads_0_point[2], timer_update_heads_0_point[0] + timer_update_heads_0_point[1] + timer_update_heads_0_point[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_point[0];
  timer_update_total[1] += timer_update_heads_0_point[1];
  timer_update_total[2] += timer_update_heads_0_point[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_support, %f, %f, %f, %f", timer_update_heads_0_support[0], timer_update_heads_0_support[1], timer_update_heads_0_support[2], timer_update_heads_0_support[0] + timer_update_heads_0_support[1] + timer_update_heads_0_support[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_support[0];
  timer_update_total[1] += timer_update_heads_0_support[1];
  timer_update_total[2] += timer_update_heads_0_support[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update heads_0_pose, %f, %f, %f, %f", timer_update_heads_0_pose[0], timer_update_heads_0_pose[1], timer_update_heads_0_pose[2], timer_update_heads_0_pose[0] + timer_update_heads_0_pose[1] + timer_update_heads_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_heads_0_pose[0];
  timer_update_total[1] += timer_update_heads_0_pose[1];
  timer_update_total[2] += timer_update_heads_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0, %f, %f, %f, %f", timer_update_legs_0[0], timer_update_legs_0[1], timer_update_legs_0[2], timer_update_legs_0[0] + timer_update_legs_0[1] + timer_update_legs_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0[0];
  timer_update_total[1] += timer_update_legs_0[1];
  timer_update_total[2] += timer_update_legs_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_brace, %f, %f, %f, %f", timer_update_legs_0_brace[0], timer_update_legs_0_brace[1], timer_update_legs_0_brace[2], timer_update_legs_0_brace[0] + timer_update_legs_0_brace[1] + timer_update_legs_0_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_brace[0];
  timer_update_total[1] += timer_update_legs_0_brace[1];
  timer_update_total[2] += timer_update_legs_0_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_standingSupport, %f, %f, %f, %f", timer_update_legs_0_standingSupport[0], timer_update_legs_0_standingSupport[1], timer_update_legs_0_standingSupport[2], timer_update_legs_0_standingSupport[0] + timer_update_legs_0_standingSupport[1] + timer_update_legs_0_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_standingSupport[0];
  timer_update_total[1] += timer_update_legs_0_standingSupport[1];
  timer_update_total[2] += timer_update_legs_0_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_sittingSupport, %f, %f, %f, %f", timer_update_legs_0_sittingSupport[0], timer_update_legs_0_sittingSupport[1], timer_update_legs_0_sittingSupport[2], timer_update_legs_0_sittingSupport[0] + timer_update_legs_0_sittingSupport[1] + timer_update_legs_0_sittingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_sittingSupport[0];
  timer_update_total[1] += timer_update_legs_0_sittingSupport[1];
  timer_update_total[2] += timer_update_legs_0_sittingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_step, %f, %f, %f, %f", timer_update_legs_0_step[0], timer_update_legs_0_step[1], timer_update_legs_0_step[2], timer_update_legs_0_step[0] + timer_update_legs_0_step[1] + timer_update_legs_0_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_step[0];
  timer_update_total[1] += timer_update_legs_0_step[1];
  timer_update_total[2] += timer_update_legs_0_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_swing, %f, %f, %f, %f", timer_update_legs_0_swing[0], timer_update_legs_0_swing[1], timer_update_legs_0_swing[2], timer_update_legs_0_swing[0] + timer_update_legs_0_swing[1] + timer_update_legs_0_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_swing[0];
  timer_update_total[1] += timer_update_legs_0_swing[1];
  timer_update_total[2] += timer_update_legs_0_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_pose, %f, %f, %f, %f", timer_update_legs_0_pose[0], timer_update_legs_0_pose[1], timer_update_legs_0_pose[2], timer_update_legs_0_pose[0] + timer_update_legs_0_pose[1] + timer_update_legs_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_pose[0];
  timer_update_total[1] += timer_update_legs_0_pose[1];
  timer_update_total[2] += timer_update_legs_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_spin, %f, %f, %f, %f", timer_update_legs_0_spin[0], timer_update_legs_0_spin[1], timer_update_legs_0_spin[2], timer_update_legs_0_spin[0] + timer_update_legs_0_spin[1] + timer_update_legs_0_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_spin[0];
  timer_update_total[1] += timer_update_legs_0_spin[1];
  timer_update_total[2] += timer_update_legs_0_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_reachReaction, %f, %f, %f, %f", timer_update_legs_0_reachReaction[0], timer_update_legs_0_reachReaction[1], timer_update_legs_0_reachReaction[2], timer_update_legs_0_reachReaction[0] + timer_update_legs_0_reachReaction[1] + timer_update_legs_0_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_reachReaction[0];
  timer_update_total[1] += timer_update_legs_0_reachReaction[1];
  timer_update_total[2] += timer_update_legs_0_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_0_writhe, %f, %f, %f, %f", timer_update_legs_0_writhe[0], timer_update_legs_0_writhe[1], timer_update_legs_0_writhe[2], timer_update_legs_0_writhe[0] + timer_update_legs_0_writhe[1] + timer_update_legs_0_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_0_writhe[0];
  timer_update_total[1] += timer_update_legs_0_writhe[1];
  timer_update_total[2] += timer_update_legs_0_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1, %f, %f, %f, %f", timer_update_legs_1[0], timer_update_legs_1[1], timer_update_legs_1[2], timer_update_legs_1[0] + timer_update_legs_1[1] + timer_update_legs_1[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1[0];
  timer_update_total[1] += timer_update_legs_1[1];
  timer_update_total[2] += timer_update_legs_1[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_brace, %f, %f, %f, %f", timer_update_legs_1_brace[0], timer_update_legs_1_brace[1], timer_update_legs_1_brace[2], timer_update_legs_1_brace[0] + timer_update_legs_1_brace[1] + timer_update_legs_1_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_brace[0];
  timer_update_total[1] += timer_update_legs_1_brace[1];
  timer_update_total[2] += timer_update_legs_1_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_standingSupport, %f, %f, %f, %f", timer_update_legs_1_standingSupport[0], timer_update_legs_1_standingSupport[1], timer_update_legs_1_standingSupport[2], timer_update_legs_1_standingSupport[0] + timer_update_legs_1_standingSupport[1] + timer_update_legs_1_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_standingSupport[0];
  timer_update_total[1] += timer_update_legs_1_standingSupport[1];
  timer_update_total[2] += timer_update_legs_1_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_sittingSupport, %f, %f, %f, %f", timer_update_legs_1_sittingSupport[0], timer_update_legs_1_sittingSupport[1], timer_update_legs_1_sittingSupport[2], timer_update_legs_1_sittingSupport[0] + timer_update_legs_1_sittingSupport[1] + timer_update_legs_1_sittingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_sittingSupport[0];
  timer_update_total[1] += timer_update_legs_1_sittingSupport[1];
  timer_update_total[2] += timer_update_legs_1_sittingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_step, %f, %f, %f, %f", timer_update_legs_1_step[0], timer_update_legs_1_step[1], timer_update_legs_1_step[2], timer_update_legs_1_step[0] + timer_update_legs_1_step[1] + timer_update_legs_1_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_step[0];
  timer_update_total[1] += timer_update_legs_1_step[1];
  timer_update_total[2] += timer_update_legs_1_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_swing, %f, %f, %f, %f", timer_update_legs_1_swing[0], timer_update_legs_1_swing[1], timer_update_legs_1_swing[2], timer_update_legs_1_swing[0] + timer_update_legs_1_swing[1] + timer_update_legs_1_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_swing[0];
  timer_update_total[1] += timer_update_legs_1_swing[1];
  timer_update_total[2] += timer_update_legs_1_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_pose, %f, %f, %f, %f", timer_update_legs_1_pose[0], timer_update_legs_1_pose[1], timer_update_legs_1_pose[2], timer_update_legs_1_pose[0] + timer_update_legs_1_pose[1] + timer_update_legs_1_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_pose[0];
  timer_update_total[1] += timer_update_legs_1_pose[1];
  timer_update_total[2] += timer_update_legs_1_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_spin, %f, %f, %f, %f", timer_update_legs_1_spin[0], timer_update_legs_1_spin[1], timer_update_legs_1_spin[2], timer_update_legs_1_spin[0] + timer_update_legs_1_spin[1] + timer_update_legs_1_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_spin[0];
  timer_update_total[1] += timer_update_legs_1_spin[1];
  timer_update_total[2] += timer_update_legs_1_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_reachReaction, %f, %f, %f, %f", timer_update_legs_1_reachReaction[0], timer_update_legs_1_reachReaction[1], timer_update_legs_1_reachReaction[2], timer_update_legs_1_reachReaction[0] + timer_update_legs_1_reachReaction[1] + timer_update_legs_1_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_reachReaction[0];
  timer_update_total[1] += timer_update_legs_1_reachReaction[1];
  timer_update_total[2] += timer_update_legs_1_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update legs_1_writhe, %f, %f, %f, %f", timer_update_legs_1_writhe[0], timer_update_legs_1_writhe[1], timer_update_legs_1_writhe[2], timer_update_legs_1_writhe[0] + timer_update_legs_1_writhe[1] + timer_update_legs_1_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_legs_1_writhe[0];
  timer_update_total[1] += timer_update_legs_1_writhe[1];
  timer_update_total[2] += timer_update_legs_1_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0, %f, %f, %f, %f", timer_update_spines_0[0], timer_update_spines_0[1], timer_update_spines_0[2], timer_update_spines_0[0] + timer_update_spines_0[1] + timer_update_spines_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0[0];
  timer_update_total[1] += timer_update_spines_0[1];
  timer_update_total[2] += timer_update_spines_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0_support, %f, %f, %f, %f", timer_update_spines_0_support[0], timer_update_spines_0_support[1], timer_update_spines_0_support[2], timer_update_spines_0_support[0] + timer_update_spines_0_support[1] + timer_update_spines_0_support[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0_support[0];
  timer_update_total[1] += timer_update_spines_0_support[1];
  timer_update_total[2] += timer_update_spines_0_support[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0_pose, %f, %f, %f, %f", timer_update_spines_0_pose[0], timer_update_spines_0_pose[1], timer_update_spines_0_pose[2], timer_update_spines_0_pose[0] + timer_update_spines_0_pose[1] + timer_update_spines_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0_pose[0];
  timer_update_total[1] += timer_update_spines_0_pose[1];
  timer_update_total[2] += timer_update_spines_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0_control, %f, %f, %f, %f", timer_update_spines_0_control[0], timer_update_spines_0_control[1], timer_update_spines_0_control[2], timer_update_spines_0_control[0] + timer_update_spines_0_control[1] + timer_update_spines_0_control[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0_control[0];
  timer_update_total[1] += timer_update_spines_0_control[1];
  timer_update_total[2] += timer_update_spines_0_control[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0_reachReaction, %f, %f, %f, %f", timer_update_spines_0_reachReaction[0], timer_update_spines_0_reachReaction[1], timer_update_spines_0_reachReaction[2], timer_update_spines_0_reachReaction[0] + timer_update_spines_0_reachReaction[1] + timer_update_spines_0_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0_reachReaction[0];
  timer_update_total[1] += timer_update_spines_0_reachReaction[1];
  timer_update_total[2] += timer_update_spines_0_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0_writhe, %f, %f, %f, %f", timer_update_spines_0_writhe[0], timer_update_spines_0_writhe[1], timer_update_spines_0_writhe[2], timer_update_spines_0_writhe[0] + timer_update_spines_0_writhe[1] + timer_update_spines_0_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0_writhe[0];
  timer_update_total[1] += timer_update_spines_0_writhe[1];
  timer_update_total[2] += timer_update_spines_0_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update spines_0_aim, %f, %f, %f, %f", timer_update_spines_0_aim[0], timer_update_spines_0_aim[1], timer_update_spines_0_aim[2], timer_update_spines_0_aim[0] + timer_update_spines_0_aim[1] + timer_update_spines_0_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_spines_0_aim[0];
  timer_update_total[1] += timer_update_spines_0_aim[1];
  timer_update_total[2] += timer_update_spines_0_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update sceneProbes, %f, %f, %f, %f", timer_update_sceneProbes[0], timer_update_sceneProbes[1], timer_update_sceneProbes[2], timer_update_sceneProbes[0] + timer_update_sceneProbes[1] + timer_update_sceneProbes[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_sceneProbes[0];
  timer_update_total[1] += timer_update_sceneProbes[1];
  timer_update_total[2] += timer_update_sceneProbes[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update balanceBehaviourFeedback, %f, %f, %f, %f", timer_update_balanceBehaviourFeedback[0], timer_update_balanceBehaviourFeedback[1], timer_update_balanceBehaviourFeedback[2], timer_update_balanceBehaviourFeedback[0] + timer_update_balanceBehaviourFeedback[1] + timer_update_balanceBehaviourFeedback[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_update_total[0] += timer_update_balanceBehaviourFeedback[0];
  timer_update_total[1] += timer_update_balanceBehaviourFeedback[1];
  timer_update_total[2] += timer_update_balanceBehaviourFeedback[2];
#ifndef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("update total, %f, %f, %f, %f", timer_update_total[0], timer_update_total[1], timer_update_total[2], timer_update_total[0] + timer_update_total[1] + timer_update_total[2]);
#endif // EUPHORIA_PROFILING_DETAILED
#endif // EUPHORIA_PROFILING
}
//----------------------------------------------------------------------------------------------------------------------
void MyNetwork::executeNetworkFeedback(float timeStep)
{
#ifdef EUPHORIA_PROFILING
  LARGE_INTEGER TicksPerSecond;
  QueryPerformanceFrequency( &TicksPerSecond );
  TIMETYPE dMicrosecondsPerTick = (TIMETYPE) (double(1.0) / ((double)TicksPerSecond.QuadPart * 0.000001));
  LARGE_INTEGER StartTimer, EndTimer;
  __int64 UpdateTicks;
#endif // EUPHORIA_PROFILING
  {
    AnimateBehaviourInterface* module_animateBehaviourInterface = (AnimateBehaviourInterface*)getModule(NetworkManifest::animateBehaviourInterface);
    if (isEnabled(NetworkManifest::animateBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_animateBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_animateBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_animateBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    FreeFallBehaviourInterface* module_freeFallBehaviourInterface = (FreeFallBehaviourInterface*)getModule(NetworkManifest::freeFallBehaviourInterface);
    if (isEnabled(NetworkManifest::freeFallBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_freeFallBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_freeFallBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_freeFallBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_freeFallBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_freeFallBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_freeFallBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    Arm* module_arms_0 = (Arm*)getModule(NetworkManifest::arms_0);
    if (isEnabled(NetworkManifest::arms_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_0->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_arms_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0[1]));
#endif // EUPHORIA_PROFILING
      {
        ArmHold* module_arms_0_hold = (ArmHold*)getModule(NetworkManifest::arms_0_hold);
        if (isEnabled(NetworkManifest::arms_0_hold))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_hold->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_hold[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_hold[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmBrace* module_arms_0_brace = (ArmBrace*)getModule(NetworkManifest::arms_0_brace);
        if (isEnabled(NetworkManifest::arms_0_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_brace->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmStandingSupport* module_arms_0_standingSupport = (ArmStandingSupport*)getModule(NetworkManifest::arms_0_standingSupport);
        if (isEnabled(NetworkManifest::arms_0_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_standingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmHoldingSupport* module_arms_0_holdingSupport = (ArmHoldingSupport*)getModule(NetworkManifest::arms_0_holdingSupport);
        if (isEnabled(NetworkManifest::arms_0_holdingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_holdingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_holdingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_holdingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSittingStep* module_arms_0_sittingStep = (ArmSittingStep*)getModule(NetworkManifest::arms_0_sittingStep);
        if (isEnabled(NetworkManifest::arms_0_sittingStep))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_sittingStep->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_sittingStep[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_sittingStep[0]));
#endif // EUPHORIA_PROFILING
        }
        ArmStep* module_arms_0_step = (ArmStep*)getModule(NetworkManifest::arms_0_step);
        if (isEnabled(NetworkManifest::arms_0_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_step->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_step[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSpin* module_arms_0_spin = (ArmSpin*)getModule(NetworkManifest::arms_0_spin);
        if (isEnabled(NetworkManifest::arms_0_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_spin->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_spin[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForBodyPart* module_arms_0_reachForBodyPart = (ArmReachForBodyPart*)getModule(NetworkManifest::arms_0_reachForBodyPart);
        if (isEnabled(NetworkManifest::arms_0_reachForBodyPart))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForBodyPart->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_reachForBodyPart[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_reachForBodyPart[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForBodyPart->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_reachForBodyPart[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_reachForBodyPart[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForWorld* module_arms_0_reachForWorld = (ArmReachForWorld*)getModule(NetworkManifest::arms_0_reachForWorld);
        if (isEnabled(NetworkManifest::arms_0_reachForWorld))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_reachForWorld->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_reachForWorld[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_reachForWorld[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        ArmGrab* module_arms_0_grab = (ArmGrab*)getModule(NetworkManifest::arms_0_grab);
        if (isEnabled(NetworkManifest::arms_0_grab))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_grab->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_grab[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_grab[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_0_grab->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_0_grab[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0_grab[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_0->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_arms_0[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_0[2]));
#endif // EUPHORIA_PROFILING
    }
    Arm* module_arms_1 = (Arm*)getModule(NetworkManifest::arms_1);
    if (isEnabled(NetworkManifest::arms_1))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_1->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_arms_1[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1[1]));
#endif // EUPHORIA_PROFILING
      {
        ArmHold* module_arms_1_hold = (ArmHold*)getModule(NetworkManifest::arms_1_hold);
        if (isEnabled(NetworkManifest::arms_1_hold))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_hold->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_hold[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_hold[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmBrace* module_arms_1_brace = (ArmBrace*)getModule(NetworkManifest::arms_1_brace);
        if (isEnabled(NetworkManifest::arms_1_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_brace->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmStandingSupport* module_arms_1_standingSupport = (ArmStandingSupport*)getModule(NetworkManifest::arms_1_standingSupport);
        if (isEnabled(NetworkManifest::arms_1_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_standingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmHoldingSupport* module_arms_1_holdingSupport = (ArmHoldingSupport*)getModule(NetworkManifest::arms_1_holdingSupport);
        if (isEnabled(NetworkManifest::arms_1_holdingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_holdingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_holdingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_holdingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSittingStep* module_arms_1_sittingStep = (ArmSittingStep*)getModule(NetworkManifest::arms_1_sittingStep);
        if (isEnabled(NetworkManifest::arms_1_sittingStep))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_sittingStep->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_sittingStep[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_sittingStep[0]));
#endif // EUPHORIA_PROFILING
        }
        ArmStep* module_arms_1_step = (ArmStep*)getModule(NetworkManifest::arms_1_step);
        if (isEnabled(NetworkManifest::arms_1_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_step->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_step[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmSpin* module_arms_1_spin = (ArmSpin*)getModule(NetworkManifest::arms_1_spin);
        if (isEnabled(NetworkManifest::arms_1_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_spin->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_spin[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForBodyPart* module_arms_1_reachForBodyPart = (ArmReachForBodyPart*)getModule(NetworkManifest::arms_1_reachForBodyPart);
        if (isEnabled(NetworkManifest::arms_1_reachForBodyPart))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForBodyPart->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_reachForBodyPart[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_reachForBodyPart[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForBodyPart->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_reachForBodyPart[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_reachForBodyPart[1]));
#endif // EUPHORIA_PROFILING
        }
        ArmReachForWorld* module_arms_1_reachForWorld = (ArmReachForWorld*)getModule(NetworkManifest::arms_1_reachForWorld);
        if (isEnabled(NetworkManifest::arms_1_reachForWorld))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_reachForWorld->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_reachForWorld[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_reachForWorld[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        ArmGrab* module_arms_1_grab = (ArmGrab*)getModule(NetworkManifest::arms_1_grab);
        if (isEnabled(NetworkManifest::arms_1_grab))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_grab->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_grab[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_grab[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_arms_1_grab->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_arms_1_grab[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1_grab[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_arms_1->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_arms_1[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_arms_1[2]));
#endif // EUPHORIA_PROFILING
    }
    Head* module_heads_0 = (Head*)getModule(NetworkManifest::heads_0);
    if (isEnabled(NetworkManifest::heads_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_heads_0->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_heads_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_heads_0[1]));
#endif // EUPHORIA_PROFILING
      {
        HeadEyes* module_heads_0_eyes = (HeadEyes*)getModule(NetworkManifest::heads_0_eyes);
        if (isEnabled(NetworkManifest::heads_0_eyes))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_eyes->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_heads_0_eyes[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_heads_0_eyes[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadAvoid* module_heads_0_avoid = (HeadAvoid*)getModule(NetworkManifest::heads_0_avoid);
        if (isEnabled(NetworkManifest::heads_0_avoid))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_avoid->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_heads_0_avoid[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_heads_0_avoid[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadDodge* module_heads_0_dodge = (HeadDodge*)getModule(NetworkManifest::heads_0_dodge);
        if (isEnabled(NetworkManifest::heads_0_dodge))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_dodge->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_heads_0_dodge[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_heads_0_dodge[1]));
#endif // EUPHORIA_PROFILING
        }
        HeadPoint* module_heads_0_point = (HeadPoint*)getModule(NetworkManifest::heads_0_point);
        if (isEnabled(NetworkManifest::heads_0_point))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_heads_0_point->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_heads_0_point[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_heads_0_point[1]));
#endif // EUPHORIA_PROFILING
        }
      }
    }
    Leg* module_legs_0 = (Leg*)getModule(NetworkManifest::legs_0);
    if (isEnabled(NetworkManifest::legs_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_0->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_legs_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0[1]));
#endif // EUPHORIA_PROFILING
      {
        LegBrace* module_legs_0_brace = (LegBrace*)getModule(NetworkManifest::legs_0_brace);
        if (isEnabled(NetworkManifest::legs_0_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_brace->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_0_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStandingSupport* module_legs_0_standingSupport = (LegStandingSupport*)getModule(NetworkManifest::legs_0_standingSupport);
        if (isEnabled(NetworkManifest::legs_0_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_standingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_0_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSittingSupport* module_legs_0_sittingSupport = (LegSittingSupport*)getModule(NetworkManifest::legs_0_sittingSupport);
        if (isEnabled(NetworkManifest::legs_0_sittingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_sittingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_0_sittingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0_sittingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStep* module_legs_0_step = (LegStep*)getModule(NetworkManifest::legs_0_step);
        if (isEnabled(NetworkManifest::legs_0_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_step->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_0_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0_step[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSpin* module_legs_0_spin = (LegSpin*)getModule(NetworkManifest::legs_0_spin);
        if (isEnabled(NetworkManifest::legs_0_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_0_spin->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_0_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0_spin[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_0->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_legs_0[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_0[2]));
#endif // EUPHORIA_PROFILING
    }
    Leg* module_legs_1 = (Leg*)getModule(NetworkManifest::legs_1);
    if (isEnabled(NetworkManifest::legs_1))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_1->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_legs_1[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1[1]));
#endif // EUPHORIA_PROFILING
      {
        LegBrace* module_legs_1_brace = (LegBrace*)getModule(NetworkManifest::legs_1_brace);
        if (isEnabled(NetworkManifest::legs_1_brace))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_brace->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_1_brace[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1_brace[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStandingSupport* module_legs_1_standingSupport = (LegStandingSupport*)getModule(NetworkManifest::legs_1_standingSupport);
        if (isEnabled(NetworkManifest::legs_1_standingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_standingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_1_standingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1_standingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSittingSupport* module_legs_1_sittingSupport = (LegSittingSupport*)getModule(NetworkManifest::legs_1_sittingSupport);
        if (isEnabled(NetworkManifest::legs_1_sittingSupport))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_sittingSupport->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_1_sittingSupport[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1_sittingSupport[1]));
#endif // EUPHORIA_PROFILING
        }
        LegStep* module_legs_1_step = (LegStep*)getModule(NetworkManifest::legs_1_step);
        if (isEnabled(NetworkManifest::legs_1_step))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_step->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_1_step[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1_step[1]));
#endif // EUPHORIA_PROFILING
        }
        LegSpin* module_legs_1_spin = (LegSpin*)getModule(NetworkManifest::legs_1_spin);
        if (isEnabled(NetworkManifest::legs_1_spin))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_legs_1_spin->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_legs_1_spin[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1_spin[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_legs_1->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_legs_1[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_legs_1[2]));
#endif // EUPHORIA_PROFILING
    }
    Spine* module_spines_0 = (Spine*)getModule(NetworkManifest::spines_0);
    if (isEnabled(NetworkManifest::spines_0))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_spines_0->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_spines_0[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_spines_0[1]));
#endif // EUPHORIA_PROFILING
      {
        SpineSupport* module_spines_0_support = (SpineSupport*)getModule(NetworkManifest::spines_0_support);
        if (isEnabled(NetworkManifest::spines_0_support))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_support->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_spines_0_support[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_spines_0_support[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_spines_0_support->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_spines_0_support[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_spines_0_support[1]));
#endif // EUPHORIA_PROFILING
        }
      }
    }
    SceneProbes* module_sceneProbes = (SceneProbes*)getModule(NetworkManifest::sceneProbes);
    if (isEnabled(NetworkManifest::sceneProbes))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sceneProbes->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_sceneProbes[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_sceneProbes[1]));
#endif // EUPHORIA_PROFILING
    }
    BalanceBehaviourFeedback* module_balanceBehaviourFeedback = (BalanceBehaviourFeedback*)getModule(NetworkManifest::balanceBehaviourFeedback);
    if (isEnabled(NetworkManifest::balanceBehaviourFeedback))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceBehaviourFeedback->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_balanceBehaviourFeedback[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceBehaviourFeedback[1]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    EyesBehaviourInterface* module_eyesBehaviourInterface = (EyesBehaviourInterface*)getModule(NetworkManifest::eyesBehaviourInterface);
    if (isEnabled(NetworkManifest::eyesBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_eyesBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_eyesBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_eyesBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_eyesBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_eyesBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_eyesBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    AimBehaviourInterface* module_aimBehaviourInterface = (AimBehaviourInterface*)getModule(NetworkManifest::aimBehaviourInterface);
    if (isEnabled(NetworkManifest::aimBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_aimBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_aimBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_aimBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_aimBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_aimBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_aimBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ReachForWorldBehaviourInterface* module_reachForWorldBehaviourInterface = (ReachForWorldBehaviourInterface*)getModule(NetworkManifest::reachForWorldBehaviourInterface);
    if (isEnabled(NetworkManifest::reachForWorldBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_reachForWorldBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_reachForWorldBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_reachForWorldBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_reachForWorldBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_reachForWorldBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_reachForWorldBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    RandomLook* module_randomLook = (RandomLook*)getModule(NetworkManifest::randomLook);
    if (isEnabled(NetworkManifest::randomLook))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_randomLook->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_randomLook[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_randomLook[0]));
#endif // EUPHORIA_PROFILING
    }
    EnvironmentAwareness* module_environmentAwareness = (EnvironmentAwareness*)getModule(NetworkManifest::environmentAwareness);
    if (isEnabled(NetworkManifest::environmentAwareness))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_environmentAwareness->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_environmentAwareness[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_environmentAwareness[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_environmentAwareness->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_environmentAwareness[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_environmentAwareness[1]));
#endif // EUPHORIA_PROFILING
    }
    BodyFrame* module_bodyFrame = (BodyFrame*)getModule(NetworkManifest::bodyFrame);
    if (isEnabled(NetworkManifest::bodyFrame))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_bodyFrame->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_bodyFrame[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_bodyFrame->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_bodyFrame[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame[1]));
#endif // EUPHORIA_PROFILING
      {
        SupportPolygon* module_bodyFrame_supportPolygon = (SupportPolygon*)getModule(NetworkManifest::bodyFrame_supportPolygon);
        if (isEnabled(NetworkManifest::bodyFrame_supportPolygon))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_supportPolygon->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_supportPolygon[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_supportPolygon[0]));
#endif // EUPHORIA_PROFILING
        }
        SupportPolygon* module_bodyFrame_sittingSupportPolygon = (SupportPolygon*)getModule(NetworkManifest::bodyFrame_sittingSupportPolygon);
        if (isEnabled(NetworkManifest::bodyFrame_sittingSupportPolygon))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingSupportPolygon->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_sittingSupportPolygon[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_sittingSupportPolygon[0]));
#endif // EUPHORIA_PROFILING
        }
        BodyBalance* module_bodyFrame_bodyBalance = (BodyBalance*)getModule(NetworkManifest::bodyFrame_bodyBalance);
        if (isEnabled(NetworkManifest::bodyFrame_bodyBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_bodyBalance->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_bodyBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_bodyBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_bodyBalance->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_bodyBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_bodyBalance[1]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_bodyBalance->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_bodyBalance[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_bodyBalance[2]));
#endif // EUPHORIA_PROFILING
        }
        ReachForBody* module_bodyFrame_reachForBody = (ReachForBody*)getModule(NetworkManifest::bodyFrame_reachForBody);
        if (isEnabled(NetworkManifest::bodyFrame_reachForBody))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_reachForBody->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_reachForBody[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_reachForBody[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_reachForBody->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_reachForBody[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_reachForBody[1]));
#endif // EUPHORIA_PROFILING
        }
        BalanceAssistant* module_bodyFrame_balanceAssistant = (BalanceAssistant*)getModule(NetworkManifest::bodyFrame_balanceAssistant);
        if (isEnabled(NetworkManifest::bodyFrame_balanceAssistant))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_balanceAssistant->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_balanceAssistant[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_balanceAssistant[0]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        SittingBodyBalance* module_bodyFrame_sittingBodyBalance = (SittingBodyBalance*)getModule(NetworkManifest::bodyFrame_sittingBodyBalance);
        if (isEnabled(NetworkManifest::bodyFrame_sittingBodyBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingBodyBalance->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_sittingBodyBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_sittingBodyBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_bodyFrame_sittingBodyBalance->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_bodyFrame_sittingBodyBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame_sittingBodyBalance[1]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_bodyFrame->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_bodyFrame[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_bodyFrame[2]));
#endif // EUPHORIA_PROFILING
    }
    BodySection* module_upperBody = (BodySection*)getModule(NetworkManifest::upperBody);
    if (isEnabled(NetworkManifest::upperBody))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_upperBody->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_upperBody[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody[0]));
#endif // EUPHORIA_PROFILING
      {
        RotateCore* module_upperBody_rotate = (RotateCore*)getModule(NetworkManifest::upperBody_rotate);
        if (isEnabled(NetworkManifest::upperBody_rotate))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_rotate->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_upperBody_rotate[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody_rotate[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_rotate->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_upperBody_rotate[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody_rotate[1]));
#endif // EUPHORIA_PROFILING
        }
        PositionCore* module_upperBody_position = (PositionCore*)getModule(NetworkManifest::upperBody_position);
        if (isEnabled(NetworkManifest::upperBody_position))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_position->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_upperBody_position[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody_position[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_position->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_upperBody_position[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody_position[1]));
#endif // EUPHORIA_PROFILING
        }
        BraceChooser* module_upperBody_braceChooser = (BraceChooser*)getModule(NetworkManifest::upperBody_braceChooser);
        if (isEnabled(NetworkManifest::upperBody_braceChooser))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_upperBody_braceChooser->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_upperBody_braceChooser[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody_braceChooser[0]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_upperBody->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_upperBody[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_upperBody[2]));
#endif // EUPHORIA_PROFILING
    }
    BodySection* module_lowerBody = (BodySection*)getModule(NetworkManifest::lowerBody);
    if (isEnabled(NetworkManifest::lowerBody))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_lowerBody->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_lowerBody[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody[0]));
#endif // EUPHORIA_PROFILING
      {
        RotateCore* module_lowerBody_rotate = (RotateCore*)getModule(NetworkManifest::lowerBody_rotate);
        if (isEnabled(NetworkManifest::lowerBody_rotate))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_rotate->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_lowerBody_rotate[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody_rotate[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_rotate->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_lowerBody_rotate[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody_rotate[1]));
#endif // EUPHORIA_PROFILING
        }
        PositionCore* module_lowerBody_position = (PositionCore*)getModule(NetworkManifest::lowerBody_position);
        if (isEnabled(NetworkManifest::lowerBody_position))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_position->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_lowerBody_position[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody_position[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_position->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_lowerBody_position[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody_position[1]));
#endif // EUPHORIA_PROFILING
        }
        BraceChooser* module_lowerBody_braceChooser = (BraceChooser*)getModule(NetworkManifest::lowerBody_braceChooser);
        if (isEnabled(NetworkManifest::lowerBody_braceChooser))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_lowerBody_braceChooser->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_lowerBody_braceChooser[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody_braceChooser[0]));
#endif // EUPHORIA_PROFILING
        }
      }
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_lowerBody->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_lowerBody[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_lowerBody[2]));
#endif // EUPHORIA_PROFILING
    }
    // SYNC
    ReachForBodyBehaviourInterface* module_reachForBodyBehaviourInterface = (ReachForBodyBehaviourInterface*)getModule(NetworkManifest::reachForBodyBehaviourInterface);
    if (isEnabled(NetworkManifest::reachForBodyBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_reachForBodyBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_reachForBodyBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_reachForBodyBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_reachForBodyBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_reachForBodyBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_reachForBodyBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    BalancePoserBehaviourInterface* module_balancePoserBehaviourInterface = (BalancePoserBehaviourInterface*)getModule(NetworkManifest::balancePoserBehaviourInterface);
    if (isEnabled(NetworkManifest::balancePoserBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balancePoserBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_balancePoserBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balancePoserBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
    }
    ObserveBehaviourInterface* module_observeBehaviourInterface = (ObserveBehaviourInterface*)getModule(NetworkManifest::observeBehaviourInterface);
    if (isEnabled(NetworkManifest::observeBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_observeBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_observeBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_observeBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_observeBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_observeBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_observeBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    IdleAwakeBehaviourInterface* module_idleAwakeBehaviourInterface = (IdleAwakeBehaviourInterface*)getModule(NetworkManifest::idleAwakeBehaviourInterface);
    if (isEnabled(NetworkManifest::idleAwakeBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_idleAwakeBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_idleAwakeBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_idleAwakeBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_idleAwakeBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_idleAwakeBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_idleAwakeBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    SitBehaviourInterface* module_sitBehaviourInterface = (SitBehaviourInterface*)getModule(NetworkManifest::sitBehaviourInterface);
    if (isEnabled(NetworkManifest::sitBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sitBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_sitBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_sitBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sitBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_sitBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_sitBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_sitBehaviourInterface->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_sitBehaviourInterface[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_sitBehaviourInterface[2]));
#endif // EUPHORIA_PROFILING
    }
    HazardManagement* module_hazardManagement = (HazardManagement*)getModule(NetworkManifest::hazardManagement);
    if (isEnabled(NetworkManifest::hazardManagement))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_hazardManagement->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_hazardManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement[0]));
#endif // EUPHORIA_PROFILING
      {
        ImpactPredictor* module_hazardManagement_chestImpactPredictor = (ImpactPredictor*)getModule(NetworkManifest::hazardManagement_chestImpactPredictor);
        if (isEnabled(NetworkManifest::hazardManagement_chestImpactPredictor))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_chestImpactPredictor->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_chestImpactPredictor[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_chestImpactPredictor[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_chestImpactPredictor->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_chestImpactPredictor[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_chestImpactPredictor[1]));
#endif // EUPHORIA_PROFILING
        }
        ShieldManagement* module_hazardManagement_shieldManagement = (ShieldManagement*)getModule(NetworkManifest::hazardManagement_shieldManagement);
        if (isEnabled(NetworkManifest::hazardManagement_shieldManagement))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_shieldManagement->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_shieldManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_shieldManagement[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_shieldManagement->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_shieldManagement[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_shieldManagement[1]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        Grab* module_hazardManagement_grab = (Grab*)getModule(NetworkManifest::hazardManagement_grab);
        if (isEnabled(NetworkManifest::hazardManagement_grab))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grab->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_grab[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_grab[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grab->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_grab[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_grab[1]));
#endif // EUPHORIA_PROFILING
        }
        HazardResponse* module_hazardManagement_hazardResponse = (HazardResponse*)getModule(NetworkManifest::hazardManagement_hazardResponse);
        if (isEnabled(NetworkManifest::hazardManagement_hazardResponse))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_hazardResponse->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_hazardResponse[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_hazardResponse[0]));
#endif // EUPHORIA_PROFILING
        }
        FreeFallManagement* module_hazardManagement_freeFallManagement = (FreeFallManagement*)getModule(NetworkManifest::hazardManagement_freeFallManagement);
        if (isEnabled(NetworkManifest::hazardManagement_freeFallManagement))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_freeFallManagement->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_freeFallManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_freeFallManagement[0]));
#endif // EUPHORIA_PROFILING
        }
        // SYNC
        GrabDetection* module_hazardManagement_grabDetection = (GrabDetection*)getModule(NetworkManifest::hazardManagement_grabDetection);
        if (isEnabled(NetworkManifest::hazardManagement_grabDetection))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grabDetection->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_grabDetection[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_grabDetection[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_hazardManagement_grabDetection->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_hazardManagement_grabDetection[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardManagement_grabDetection[1]));
#endif // EUPHORIA_PROFILING
        }
      }
    }
    BalanceManagement* module_balanceManagement = (BalanceManagement*)getModule(NetworkManifest::balanceManagement);
    if (isEnabled(NetworkManifest::balanceManagement))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceManagement->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_balanceManagement[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceManagement[0]));
#endif // EUPHORIA_PROFILING
      {
        StaticBalance* module_balanceManagement_staticBalance = (StaticBalance*)getModule(NetworkManifest::balanceManagement_staticBalance);
        if (isEnabled(NetworkManifest::balanceManagement_staticBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_staticBalance->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_balanceManagement_staticBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceManagement_staticBalance[0]));
#endif // EUPHORIA_PROFILING
        }
        SteppingBalance* module_balanceManagement_steppingBalance = (SteppingBalance*)getModule(NetworkManifest::balanceManagement_steppingBalance);
        if (isEnabled(NetworkManifest::balanceManagement_steppingBalance))
        {
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_steppingBalance->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_balanceManagement_steppingBalance[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceManagement_steppingBalance[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
          module_balanceManagement_steppingBalance->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
          QueryPerformanceCounter( &EndTimer );
          UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
          timer_feedback_balanceManagement_steppingBalance[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceManagement_steppingBalance[1]));
#endif // EUPHORIA_PROFILING
        }
      }
    }
    // SYNC
    BalanceBehaviourInterface* module_balanceBehaviourInterface = (BalanceBehaviourInterface*)getModule(NetworkManifest::balanceBehaviourInterface);
    if (isEnabled(NetworkManifest::balanceBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_balanceBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_balanceBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_balanceBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_balanceBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HazardAwarenessBehaviourInterface* module_hazardAwarenessBehaviourInterface = (HazardAwarenessBehaviourInterface*)getModule(NetworkManifest::hazardAwarenessBehaviourInterface);
    if (isEnabled(NetworkManifest::hazardAwarenessBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_hazardAwarenessBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_hazardAwarenessBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardAwarenessBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_hazardAwarenessBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_hazardAwarenessBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardAwarenessBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_hazardAwarenessBehaviourInterface->combineFeedbackOutputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_hazardAwarenessBehaviourInterface[2] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_hazardAwarenessBehaviourInterface[2]));
#endif // EUPHORIA_PROFILING
    }
    UserHazardBehaviourInterface* module_userHazardBehaviourInterface = (UserHazardBehaviourInterface*)getModule(NetworkManifest::userHazardBehaviourInterface);
    if (isEnabled(NetworkManifest::userHazardBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_userHazardBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_userHazardBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_userHazardBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
    }
    ShieldBehaviourInterface* module_shieldBehaviourInterface = (ShieldBehaviourInterface*)getModule(NetworkManifest::shieldBehaviourInterface);
    if (isEnabled(NetworkManifest::shieldBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_shieldBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_shieldBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_shieldBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_shieldBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_shieldBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_shieldBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HoldBehaviourInterface* module_holdBehaviourInterface = (HoldBehaviourInterface*)getModule(NetworkManifest::holdBehaviourInterface);
    if (isEnabled(NetworkManifest::holdBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_holdBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_holdBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_holdBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_holdBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_holdBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_holdBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    HoldActionBehaviourInterface* module_holdActionBehaviourInterface = (HoldActionBehaviourInterface*)getModule(NetworkManifest::holdActionBehaviourInterface);
    if (isEnabled(NetworkManifest::holdActionBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_holdActionBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_holdActionBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_holdActionBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_holdActionBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_holdActionBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_holdActionBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
    ShieldActionBehaviourInterface* module_shieldActionBehaviourInterface = (ShieldActionBehaviourInterface*)getModule(NetworkManifest::shieldActionBehaviourInterface);
    if (isEnabled(NetworkManifest::shieldActionBehaviourInterface))
    {
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_shieldActionBehaviourInterface->combineFeedbackInputs();
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_shieldActionBehaviourInterface[0] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_shieldActionBehaviourInterface[0]));
#endif // EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &StartTimer );
#endif // EUPHORIA_PROFILING
      module_shieldActionBehaviourInterface->feedback(timeStep);
#ifdef EUPHORIA_PROFILING
      QueryPerformanceCounter( &EndTimer );
      UpdateTicks = EndTimer.QuadPart - StartTimer.QuadPart;
      timer_feedback_shieldActionBehaviourInterface[1] += (g_profilingTimerFraction * (UpdateTicks * dMicrosecondsPerTick - timer_feedback_shieldActionBehaviourInterface[1]));
#endif // EUPHORIA_PROFILING
    }
  }
#ifdef EUPHORIA_PROFILING
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("\n\nFEEDBACK TIMINGS:");
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] = timer_feedback_total[1] = timer_feedback_total[2] = 0.0f;
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback rootModule, %f, %f, %f, %f", timer_feedback_rootModule[0], timer_feedback_rootModule[1], timer_feedback_rootModule[2], timer_feedback_rootModule[0] + timer_feedback_rootModule[1] + timer_feedback_rootModule[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_rootModule[0];
  timer_feedback_total[1] += timer_feedback_rootModule[1];
  timer_feedback_total[2] += timer_feedback_rootModule[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback characteristicsBehaviourInterface, %f, %f, %f, %f", timer_feedback_characteristicsBehaviourInterface[0], timer_feedback_characteristicsBehaviourInterface[1], timer_feedback_characteristicsBehaviourInterface[2], timer_feedback_characteristicsBehaviourInterface[0] + timer_feedback_characteristicsBehaviourInterface[1] + timer_feedback_characteristicsBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_characteristicsBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_characteristicsBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_characteristicsBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback propertiesBehaviourInterface, %f, %f, %f, %f", timer_feedback_propertiesBehaviourInterface[0], timer_feedback_propertiesBehaviourInterface[1], timer_feedback_propertiesBehaviourInterface[2], timer_feedback_propertiesBehaviourInterface[0] + timer_feedback_propertiesBehaviourInterface[1] + timer_feedback_propertiesBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_propertiesBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_propertiesBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_propertiesBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback euphoriaRagdollBehaviourInterface, %f, %f, %f, %f", timer_feedback_euphoriaRagdollBehaviourInterface[0], timer_feedback_euphoriaRagdollBehaviourInterface[1], timer_feedback_euphoriaRagdollBehaviourInterface[2], timer_feedback_euphoriaRagdollBehaviourInterface[0] + timer_feedback_euphoriaRagdollBehaviourInterface[1] + timer_feedback_euphoriaRagdollBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_euphoriaRagdollBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_euphoriaRagdollBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_euphoriaRagdollBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback eyesBehaviourInterface, %f, %f, %f, %f", timer_feedback_eyesBehaviourInterface[0], timer_feedback_eyesBehaviourInterface[1], timer_feedback_eyesBehaviourInterface[2], timer_feedback_eyesBehaviourInterface[0] + timer_feedback_eyesBehaviourInterface[1] + timer_feedback_eyesBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_eyesBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_eyesBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_eyesBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback armsBraceBehaviourInterface, %f, %f, %f, %f", timer_feedback_armsBraceBehaviourInterface[0], timer_feedback_armsBraceBehaviourInterface[1], timer_feedback_armsBraceBehaviourInterface[2], timer_feedback_armsBraceBehaviourInterface[0] + timer_feedback_armsBraceBehaviourInterface[1] + timer_feedback_armsBraceBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_armsBraceBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_armsBraceBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_armsBraceBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback armsPlacementBehaviourInterface, %f, %f, %f, %f", timer_feedback_armsPlacementBehaviourInterface[0], timer_feedback_armsPlacementBehaviourInterface[1], timer_feedback_armsPlacementBehaviourInterface[2], timer_feedback_armsPlacementBehaviourInterface[0] + timer_feedback_armsPlacementBehaviourInterface[1] + timer_feedback_armsPlacementBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_armsPlacementBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_armsPlacementBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_armsPlacementBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback aimBehaviourInterface, %f, %f, %f, %f", timer_feedback_aimBehaviourInterface[0], timer_feedback_aimBehaviourInterface[1], timer_feedback_aimBehaviourInterface[2], timer_feedback_aimBehaviourInterface[0] + timer_feedback_aimBehaviourInterface[1] + timer_feedback_aimBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_aimBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_aimBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_aimBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback headDodgeBehaviourInterface, %f, %f, %f, %f", timer_feedback_headDodgeBehaviourInterface[0], timer_feedback_headDodgeBehaviourInterface[1], timer_feedback_headDodgeBehaviourInterface[2], timer_feedback_headDodgeBehaviourInterface[0] + timer_feedback_headDodgeBehaviourInterface[1] + timer_feedback_headDodgeBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_headDodgeBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_headDodgeBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_headDodgeBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback reachForBodyBehaviourInterface, %f, %f, %f, %f", timer_feedback_reachForBodyBehaviourInterface[0], timer_feedback_reachForBodyBehaviourInterface[1], timer_feedback_reachForBodyBehaviourInterface[2], timer_feedback_reachForBodyBehaviourInterface[0] + timer_feedback_reachForBodyBehaviourInterface[1] + timer_feedback_reachForBodyBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_reachForBodyBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_reachForBodyBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_reachForBodyBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback reachForWorldBehaviourInterface, %f, %f, %f, %f", timer_feedback_reachForWorldBehaviourInterface[0], timer_feedback_reachForWorldBehaviourInterface[1], timer_feedback_reachForWorldBehaviourInterface[2], timer_feedback_reachForWorldBehaviourInterface[0] + timer_feedback_reachForWorldBehaviourInterface[1] + timer_feedback_reachForWorldBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_reachForWorldBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_reachForWorldBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_reachForWorldBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback animateBehaviourInterface, %f, %f, %f, %f", timer_feedback_animateBehaviourInterface[0], timer_feedback_animateBehaviourInterface[1], timer_feedback_animateBehaviourInterface[2], timer_feedback_animateBehaviourInterface[0] + timer_feedback_animateBehaviourInterface[1] + timer_feedback_animateBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_animateBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_animateBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_animateBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balanceBehaviourInterface, %f, %f, %f, %f", timer_feedback_balanceBehaviourInterface[0], timer_feedback_balanceBehaviourInterface[1], timer_feedback_balanceBehaviourInterface[2], timer_feedback_balanceBehaviourInterface[0] + timer_feedback_balanceBehaviourInterface[1] + timer_feedback_balanceBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balanceBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_balanceBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_balanceBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balancePoserBehaviourInterface, %f, %f, %f, %f", timer_feedback_balancePoserBehaviourInterface[0], timer_feedback_balancePoserBehaviourInterface[1], timer_feedback_balancePoserBehaviourInterface[2], timer_feedback_balancePoserBehaviourInterface[0] + timer_feedback_balancePoserBehaviourInterface[1] + timer_feedback_balancePoserBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balancePoserBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_balancePoserBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_balancePoserBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback protectBehaviourInterface, %f, %f, %f, %f", timer_feedback_protectBehaviourInterface[0], timer_feedback_protectBehaviourInterface[1], timer_feedback_protectBehaviourInterface[2], timer_feedback_protectBehaviourInterface[0] + timer_feedback_protectBehaviourInterface[1] + timer_feedback_protectBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_protectBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_protectBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_protectBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardAwarenessBehaviourInterface, %f, %f, %f, %f", timer_feedback_hazardAwarenessBehaviourInterface[0], timer_feedback_hazardAwarenessBehaviourInterface[1], timer_feedback_hazardAwarenessBehaviourInterface[2], timer_feedback_hazardAwarenessBehaviourInterface[0] + timer_feedback_hazardAwarenessBehaviourInterface[1] + timer_feedback_hazardAwarenessBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardAwarenessBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_hazardAwarenessBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_hazardAwarenessBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback userHazardBehaviourInterface, %f, %f, %f, %f", timer_feedback_userHazardBehaviourInterface[0], timer_feedback_userHazardBehaviourInterface[1], timer_feedback_userHazardBehaviourInterface[2], timer_feedback_userHazardBehaviourInterface[0] + timer_feedback_userHazardBehaviourInterface[1] + timer_feedback_userHazardBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_userHazardBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_userHazardBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_userHazardBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback observeBehaviourInterface, %f, %f, %f, %f", timer_feedback_observeBehaviourInterface[0], timer_feedback_observeBehaviourInterface[1], timer_feedback_observeBehaviourInterface[2], timer_feedback_observeBehaviourInterface[0] + timer_feedback_observeBehaviourInterface[1] + timer_feedback_observeBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_observeBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_observeBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_observeBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback idleAwakeBehaviourInterface, %f, %f, %f, %f", timer_feedback_idleAwakeBehaviourInterface[0], timer_feedback_idleAwakeBehaviourInterface[1], timer_feedback_idleAwakeBehaviourInterface[2], timer_feedback_idleAwakeBehaviourInterface[0] + timer_feedback_idleAwakeBehaviourInterface[1] + timer_feedback_idleAwakeBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_idleAwakeBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_idleAwakeBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_idleAwakeBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback lookBehaviourInterface, %f, %f, %f, %f", timer_feedback_lookBehaviourInterface[0], timer_feedback_lookBehaviourInterface[1], timer_feedback_lookBehaviourInterface[2], timer_feedback_lookBehaviourInterface[0] + timer_feedback_lookBehaviourInterface[1] + timer_feedback_lookBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_lookBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_lookBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_lookBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback headAvoidBehaviourInterface, %f, %f, %f, %f", timer_feedback_headAvoidBehaviourInterface[0], timer_feedback_headAvoidBehaviourInterface[1], timer_feedback_headAvoidBehaviourInterface[2], timer_feedback_headAvoidBehaviourInterface[0] + timer_feedback_headAvoidBehaviourInterface[1] + timer_feedback_headAvoidBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_headAvoidBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_headAvoidBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_headAvoidBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback shieldBehaviourInterface, %f, %f, %f, %f", timer_feedback_shieldBehaviourInterface[0], timer_feedback_shieldBehaviourInterface[1], timer_feedback_shieldBehaviourInterface[2], timer_feedback_shieldBehaviourInterface[0] + timer_feedback_shieldBehaviourInterface[1] + timer_feedback_shieldBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_shieldBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_shieldBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_shieldBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback holdBehaviourInterface, %f, %f, %f, %f", timer_feedback_holdBehaviourInterface[0], timer_feedback_holdBehaviourInterface[1], timer_feedback_holdBehaviourInterface[2], timer_feedback_holdBehaviourInterface[0] + timer_feedback_holdBehaviourInterface[1] + timer_feedback_holdBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_holdBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_holdBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_holdBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback holdActionBehaviourInterface, %f, %f, %f, %f", timer_feedback_holdActionBehaviourInterface[0], timer_feedback_holdActionBehaviourInterface[1], timer_feedback_holdActionBehaviourInterface[2], timer_feedback_holdActionBehaviourInterface[0] + timer_feedback_holdActionBehaviourInterface[1] + timer_feedback_holdActionBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_holdActionBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_holdActionBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_holdActionBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback freeFallBehaviourInterface, %f, %f, %f, %f", timer_feedback_freeFallBehaviourInterface[0], timer_feedback_freeFallBehaviourInterface[1], timer_feedback_freeFallBehaviourInterface[2], timer_feedback_freeFallBehaviourInterface[0] + timer_feedback_freeFallBehaviourInterface[1] + timer_feedback_freeFallBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_freeFallBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_freeFallBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_freeFallBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback armsWindmillBehaviourInterface, %f, %f, %f, %f", timer_feedback_armsWindmillBehaviourInterface[0], timer_feedback_armsWindmillBehaviourInterface[1], timer_feedback_armsWindmillBehaviourInterface[2], timer_feedback_armsWindmillBehaviourInterface[0] + timer_feedback_armsWindmillBehaviourInterface[1] + timer_feedback_armsWindmillBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_armsWindmillBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_armsWindmillBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_armsWindmillBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legsPedalBehaviourInterface, %f, %f, %f, %f", timer_feedback_legsPedalBehaviourInterface[0], timer_feedback_legsPedalBehaviourInterface[1], timer_feedback_legsPedalBehaviourInterface[2], timer_feedback_legsPedalBehaviourInterface[0] + timer_feedback_legsPedalBehaviourInterface[1] + timer_feedback_legsPedalBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legsPedalBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_legsPedalBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_legsPedalBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback shieldActionBehaviourInterface, %f, %f, %f, %f", timer_feedback_shieldActionBehaviourInterface[0], timer_feedback_shieldActionBehaviourInterface[1], timer_feedback_shieldActionBehaviourInterface[2], timer_feedback_shieldActionBehaviourInterface[0] + timer_feedback_shieldActionBehaviourInterface[1] + timer_feedback_shieldActionBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_shieldActionBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_shieldActionBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_shieldActionBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback sitBehaviourInterface, %f, %f, %f, %f", timer_feedback_sitBehaviourInterface[0], timer_feedback_sitBehaviourInterface[1], timer_feedback_sitBehaviourInterface[2], timer_feedback_sitBehaviourInterface[0] + timer_feedback_sitBehaviourInterface[1] + timer_feedback_sitBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_sitBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_sitBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_sitBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback writheBehaviourInterface, %f, %f, %f, %f", timer_feedback_writheBehaviourInterface[0], timer_feedback_writheBehaviourInterface[1], timer_feedback_writheBehaviourInterface[2], timer_feedback_writheBehaviourInterface[0] + timer_feedback_writheBehaviourInterface[1] + timer_feedback_writheBehaviourInterface[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_writheBehaviourInterface[0];
  timer_feedback_total[1] += timer_feedback_writheBehaviourInterface[1];
  timer_feedback_total[2] += timer_feedback_writheBehaviourInterface[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback randomLook, %f, %f, %f, %f", timer_feedback_randomLook[0], timer_feedback_randomLook[1], timer_feedback_randomLook[2], timer_feedback_randomLook[0] + timer_feedback_randomLook[1] + timer_feedback_randomLook[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_randomLook[0];
  timer_feedback_total[1] += timer_feedback_randomLook[1];
  timer_feedback_total[2] += timer_feedback_randomLook[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement, %f, %f, %f, %f", timer_feedback_hazardManagement[0], timer_feedback_hazardManagement[1], timer_feedback_hazardManagement[2], timer_feedback_hazardManagement[0] + timer_feedback_hazardManagement[1] + timer_feedback_hazardManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement_grabDetection, %f, %f, %f, %f", timer_feedback_hazardManagement_grabDetection[0], timer_feedback_hazardManagement_grabDetection[1], timer_feedback_hazardManagement_grabDetection[2], timer_feedback_hazardManagement_grabDetection[0] + timer_feedback_hazardManagement_grabDetection[1] + timer_feedback_hazardManagement_grabDetection[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement_grabDetection[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement_grabDetection[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement_grabDetection[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement_grab, %f, %f, %f, %f", timer_feedback_hazardManagement_grab[0], timer_feedback_hazardManagement_grab[1], timer_feedback_hazardManagement_grab[2], timer_feedback_hazardManagement_grab[0] + timer_feedback_hazardManagement_grab[1] + timer_feedback_hazardManagement_grab[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement_grab[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement_grab[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement_grab[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement_hazardResponse, %f, %f, %f, %f", timer_feedback_hazardManagement_hazardResponse[0], timer_feedback_hazardManagement_hazardResponse[1], timer_feedback_hazardManagement_hazardResponse[2], timer_feedback_hazardManagement_hazardResponse[0] + timer_feedback_hazardManagement_hazardResponse[1] + timer_feedback_hazardManagement_hazardResponse[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement_hazardResponse[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement_hazardResponse[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement_hazardResponse[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement_freeFallManagement, %f, %f, %f, %f", timer_feedback_hazardManagement_freeFallManagement[0], timer_feedback_hazardManagement_freeFallManagement[1], timer_feedback_hazardManagement_freeFallManagement[2], timer_feedback_hazardManagement_freeFallManagement[0] + timer_feedback_hazardManagement_freeFallManagement[1] + timer_feedback_hazardManagement_freeFallManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement_freeFallManagement[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement_freeFallManagement[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement_freeFallManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement_chestImpactPredictor, %f, %f, %f, %f", timer_feedback_hazardManagement_chestImpactPredictor[0], timer_feedback_hazardManagement_chestImpactPredictor[1], timer_feedback_hazardManagement_chestImpactPredictor[2], timer_feedback_hazardManagement_chestImpactPredictor[0] + timer_feedback_hazardManagement_chestImpactPredictor[1] + timer_feedback_hazardManagement_chestImpactPredictor[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement_chestImpactPredictor[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement_chestImpactPredictor[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement_chestImpactPredictor[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback hazardManagement_shieldManagement, %f, %f, %f, %f", timer_feedback_hazardManagement_shieldManagement[0], timer_feedback_hazardManagement_shieldManagement[1], timer_feedback_hazardManagement_shieldManagement[2], timer_feedback_hazardManagement_shieldManagement[0] + timer_feedback_hazardManagement_shieldManagement[1] + timer_feedback_hazardManagement_shieldManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_hazardManagement_shieldManagement[0];
  timer_feedback_total[1] += timer_feedback_hazardManagement_shieldManagement[1];
  timer_feedback_total[2] += timer_feedback_hazardManagement_shieldManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balanceManagement, %f, %f, %f, %f", timer_feedback_balanceManagement[0], timer_feedback_balanceManagement[1], timer_feedback_balanceManagement[2], timer_feedback_balanceManagement[0] + timer_feedback_balanceManagement[1] + timer_feedback_balanceManagement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balanceManagement[0];
  timer_feedback_total[1] += timer_feedback_balanceManagement[1];
  timer_feedback_total[2] += timer_feedback_balanceManagement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balanceManagement_staticBalance, %f, %f, %f, %f", timer_feedback_balanceManagement_staticBalance[0], timer_feedback_balanceManagement_staticBalance[1], timer_feedback_balanceManagement_staticBalance[2], timer_feedback_balanceManagement_staticBalance[0] + timer_feedback_balanceManagement_staticBalance[1] + timer_feedback_balanceManagement_staticBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balanceManagement_staticBalance[0];
  timer_feedback_total[1] += timer_feedback_balanceManagement_staticBalance[1];
  timer_feedback_total[2] += timer_feedback_balanceManagement_staticBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balanceManagement_steppingBalance, %f, %f, %f, %f", timer_feedback_balanceManagement_steppingBalance[0], timer_feedback_balanceManagement_steppingBalance[1], timer_feedback_balanceManagement_steppingBalance[2], timer_feedback_balanceManagement_steppingBalance[0] + timer_feedback_balanceManagement_steppingBalance[1] + timer_feedback_balanceManagement_steppingBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balanceManagement_steppingBalance[0];
  timer_feedback_total[1] += timer_feedback_balanceManagement_steppingBalance[1];
  timer_feedback_total[2] += timer_feedback_balanceManagement_steppingBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balanceManagement_balancePoser, %f, %f, %f, %f", timer_feedback_balanceManagement_balancePoser[0], timer_feedback_balanceManagement_balancePoser[1], timer_feedback_balanceManagement_balancePoser[2], timer_feedback_balanceManagement_balancePoser[0] + timer_feedback_balanceManagement_balancePoser[1] + timer_feedback_balanceManagement_balancePoser[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balanceManagement_balancePoser[0];
  timer_feedback_total[1] += timer_feedback_balanceManagement_balancePoser[1];
  timer_feedback_total[2] += timer_feedback_balanceManagement_balancePoser[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback environmentAwareness, %f, %f, %f, %f", timer_feedback_environmentAwareness[0], timer_feedback_environmentAwareness[1], timer_feedback_environmentAwareness[2], timer_feedback_environmentAwareness[0] + timer_feedback_environmentAwareness[1] + timer_feedback_environmentAwareness[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_environmentAwareness[0];
  timer_feedback_total[1] += timer_feedback_environmentAwareness[1];
  timer_feedback_total[2] += timer_feedback_environmentAwareness[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame, %f, %f, %f, %f", timer_feedback_bodyFrame[0], timer_feedback_bodyFrame[1], timer_feedback_bodyFrame[2], timer_feedback_bodyFrame[0] + timer_feedback_bodyFrame[1] + timer_feedback_bodyFrame[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame_supportPolygon, %f, %f, %f, %f", timer_feedback_bodyFrame_supportPolygon[0], timer_feedback_bodyFrame_supportPolygon[1], timer_feedback_bodyFrame_supportPolygon[2], timer_feedback_bodyFrame_supportPolygon[0] + timer_feedback_bodyFrame_supportPolygon[1] + timer_feedback_bodyFrame_supportPolygon[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame_supportPolygon[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame_supportPolygon[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame_supportPolygon[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame_sittingSupportPolygon, %f, %f, %f, %f", timer_feedback_bodyFrame_sittingSupportPolygon[0], timer_feedback_bodyFrame_sittingSupportPolygon[1], timer_feedback_bodyFrame_sittingSupportPolygon[2], timer_feedback_bodyFrame_sittingSupportPolygon[0] + timer_feedback_bodyFrame_sittingSupportPolygon[1] + timer_feedback_bodyFrame_sittingSupportPolygon[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame_sittingSupportPolygon[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame_sittingSupportPolygon[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame_sittingSupportPolygon[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame_bodyBalance, %f, %f, %f, %f", timer_feedback_bodyFrame_bodyBalance[0], timer_feedback_bodyFrame_bodyBalance[1], timer_feedback_bodyFrame_bodyBalance[2], timer_feedback_bodyFrame_bodyBalance[0] + timer_feedback_bodyFrame_bodyBalance[1] + timer_feedback_bodyFrame_bodyBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame_bodyBalance[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame_bodyBalance[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame_bodyBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame_sittingBodyBalance, %f, %f, %f, %f", timer_feedback_bodyFrame_sittingBodyBalance[0], timer_feedback_bodyFrame_sittingBodyBalance[1], timer_feedback_bodyFrame_sittingBodyBalance[2], timer_feedback_bodyFrame_sittingBodyBalance[0] + timer_feedback_bodyFrame_sittingBodyBalance[1] + timer_feedback_bodyFrame_sittingBodyBalance[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame_sittingBodyBalance[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame_sittingBodyBalance[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame_sittingBodyBalance[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame_reachForBody, %f, %f, %f, %f", timer_feedback_bodyFrame_reachForBody[0], timer_feedback_bodyFrame_reachForBody[1], timer_feedback_bodyFrame_reachForBody[2], timer_feedback_bodyFrame_reachForBody[0] + timer_feedback_bodyFrame_reachForBody[1] + timer_feedback_bodyFrame_reachForBody[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame_reachForBody[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame_reachForBody[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame_reachForBody[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback bodyFrame_balanceAssistant, %f, %f, %f, %f", timer_feedback_bodyFrame_balanceAssistant[0], timer_feedback_bodyFrame_balanceAssistant[1], timer_feedback_bodyFrame_balanceAssistant[2], timer_feedback_bodyFrame_balanceAssistant[0] + timer_feedback_bodyFrame_balanceAssistant[1] + timer_feedback_bodyFrame_balanceAssistant[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_bodyFrame_balanceAssistant[0];
  timer_feedback_total[1] += timer_feedback_bodyFrame_balanceAssistant[1];
  timer_feedback_total[2] += timer_feedback_bodyFrame_balanceAssistant[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback upperBody, %f, %f, %f, %f", timer_feedback_upperBody[0], timer_feedback_upperBody[1], timer_feedback_upperBody[2], timer_feedback_upperBody[0] + timer_feedback_upperBody[1] + timer_feedback_upperBody[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_upperBody[0];
  timer_feedback_total[1] += timer_feedback_upperBody[1];
  timer_feedback_total[2] += timer_feedback_upperBody[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback upperBody_rotate, %f, %f, %f, %f", timer_feedback_upperBody_rotate[0], timer_feedback_upperBody_rotate[1], timer_feedback_upperBody_rotate[2], timer_feedback_upperBody_rotate[0] + timer_feedback_upperBody_rotate[1] + timer_feedback_upperBody_rotate[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_upperBody_rotate[0];
  timer_feedback_total[1] += timer_feedback_upperBody_rotate[1];
  timer_feedback_total[2] += timer_feedback_upperBody_rotate[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback upperBody_position, %f, %f, %f, %f", timer_feedback_upperBody_position[0], timer_feedback_upperBody_position[1], timer_feedback_upperBody_position[2], timer_feedback_upperBody_position[0] + timer_feedback_upperBody_position[1] + timer_feedback_upperBody_position[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_upperBody_position[0];
  timer_feedback_total[1] += timer_feedback_upperBody_position[1];
  timer_feedback_total[2] += timer_feedback_upperBody_position[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback upperBody_braceChooser, %f, %f, %f, %f", timer_feedback_upperBody_braceChooser[0], timer_feedback_upperBody_braceChooser[1], timer_feedback_upperBody_braceChooser[2], timer_feedback_upperBody_braceChooser[0] + timer_feedback_upperBody_braceChooser[1] + timer_feedback_upperBody_braceChooser[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_upperBody_braceChooser[0];
  timer_feedback_total[1] += timer_feedback_upperBody_braceChooser[1];
  timer_feedback_total[2] += timer_feedback_upperBody_braceChooser[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback lowerBody, %f, %f, %f, %f", timer_feedback_lowerBody[0], timer_feedback_lowerBody[1], timer_feedback_lowerBody[2], timer_feedback_lowerBody[0] + timer_feedback_lowerBody[1] + timer_feedback_lowerBody[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_lowerBody[0];
  timer_feedback_total[1] += timer_feedback_lowerBody[1];
  timer_feedback_total[2] += timer_feedback_lowerBody[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback lowerBody_rotate, %f, %f, %f, %f", timer_feedback_lowerBody_rotate[0], timer_feedback_lowerBody_rotate[1], timer_feedback_lowerBody_rotate[2], timer_feedback_lowerBody_rotate[0] + timer_feedback_lowerBody_rotate[1] + timer_feedback_lowerBody_rotate[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_lowerBody_rotate[0];
  timer_feedback_total[1] += timer_feedback_lowerBody_rotate[1];
  timer_feedback_total[2] += timer_feedback_lowerBody_rotate[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback lowerBody_position, %f, %f, %f, %f", timer_feedback_lowerBody_position[0], timer_feedback_lowerBody_position[1], timer_feedback_lowerBody_position[2], timer_feedback_lowerBody_position[0] + timer_feedback_lowerBody_position[1] + timer_feedback_lowerBody_position[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_lowerBody_position[0];
  timer_feedback_total[1] += timer_feedback_lowerBody_position[1];
  timer_feedback_total[2] += timer_feedback_lowerBody_position[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback lowerBody_braceChooser, %f, %f, %f, %f", timer_feedback_lowerBody_braceChooser[0], timer_feedback_lowerBody_braceChooser[1], timer_feedback_lowerBody_braceChooser[2], timer_feedback_lowerBody_braceChooser[0] + timer_feedback_lowerBody_braceChooser[1] + timer_feedback_lowerBody_braceChooser[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_lowerBody_braceChooser[0];
  timer_feedback_total[1] += timer_feedback_lowerBody_braceChooser[1];
  timer_feedback_total[2] += timer_feedback_lowerBody_braceChooser[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0, %f, %f, %f, %f", timer_feedback_arms_0[0], timer_feedback_arms_0[1], timer_feedback_arms_0[2], timer_feedback_arms_0[0] + timer_feedback_arms_0[1] + timer_feedback_arms_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0[0];
  timer_feedback_total[1] += timer_feedback_arms_0[1];
  timer_feedback_total[2] += timer_feedback_arms_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_grab, %f, %f, %f, %f", timer_feedback_arms_0_grab[0], timer_feedback_arms_0_grab[1], timer_feedback_arms_0_grab[2], timer_feedback_arms_0_grab[0] + timer_feedback_arms_0_grab[1] + timer_feedback_arms_0_grab[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_grab[0];
  timer_feedback_total[1] += timer_feedback_arms_0_grab[1];
  timer_feedback_total[2] += timer_feedback_arms_0_grab[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_aim, %f, %f, %f, %f", timer_feedback_arms_0_aim[0], timer_feedback_arms_0_aim[1], timer_feedback_arms_0_aim[2], timer_feedback_arms_0_aim[0] + timer_feedback_arms_0_aim[1] + timer_feedback_arms_0_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_aim[0];
  timer_feedback_total[1] += timer_feedback_arms_0_aim[1];
  timer_feedback_total[2] += timer_feedback_arms_0_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_hold, %f, %f, %f, %f", timer_feedback_arms_0_hold[0], timer_feedback_arms_0_hold[1], timer_feedback_arms_0_hold[2], timer_feedback_arms_0_hold[0] + timer_feedback_arms_0_hold[1] + timer_feedback_arms_0_hold[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_hold[0];
  timer_feedback_total[1] += timer_feedback_arms_0_hold[1];
  timer_feedback_total[2] += timer_feedback_arms_0_hold[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_brace, %f, %f, %f, %f", timer_feedback_arms_0_brace[0], timer_feedback_arms_0_brace[1], timer_feedback_arms_0_brace[2], timer_feedback_arms_0_brace[0] + timer_feedback_arms_0_brace[1] + timer_feedback_arms_0_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_brace[0];
  timer_feedback_total[1] += timer_feedback_arms_0_brace[1];
  timer_feedback_total[2] += timer_feedback_arms_0_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_standingSupport, %f, %f, %f, %f", timer_feedback_arms_0_standingSupport[0], timer_feedback_arms_0_standingSupport[1], timer_feedback_arms_0_standingSupport[2], timer_feedback_arms_0_standingSupport[0] + timer_feedback_arms_0_standingSupport[1] + timer_feedback_arms_0_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_standingSupport[0];
  timer_feedback_total[1] += timer_feedback_arms_0_standingSupport[1];
  timer_feedback_total[2] += timer_feedback_arms_0_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_holdingSupport, %f, %f, %f, %f", timer_feedback_arms_0_holdingSupport[0], timer_feedback_arms_0_holdingSupport[1], timer_feedback_arms_0_holdingSupport[2], timer_feedback_arms_0_holdingSupport[0] + timer_feedback_arms_0_holdingSupport[1] + timer_feedback_arms_0_holdingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_holdingSupport[0];
  timer_feedback_total[1] += timer_feedback_arms_0_holdingSupport[1];
  timer_feedback_total[2] += timer_feedback_arms_0_holdingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_sittingStep, %f, %f, %f, %f", timer_feedback_arms_0_sittingStep[0], timer_feedback_arms_0_sittingStep[1], timer_feedback_arms_0_sittingStep[2], timer_feedback_arms_0_sittingStep[0] + timer_feedback_arms_0_sittingStep[1] + timer_feedback_arms_0_sittingStep[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_sittingStep[0];
  timer_feedback_total[1] += timer_feedback_arms_0_sittingStep[1];
  timer_feedback_total[2] += timer_feedback_arms_0_sittingStep[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_step, %f, %f, %f, %f", timer_feedback_arms_0_step[0], timer_feedback_arms_0_step[1], timer_feedback_arms_0_step[2], timer_feedback_arms_0_step[0] + timer_feedback_arms_0_step[1] + timer_feedback_arms_0_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_step[0];
  timer_feedback_total[1] += timer_feedback_arms_0_step[1];
  timer_feedback_total[2] += timer_feedback_arms_0_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_spin, %f, %f, %f, %f", timer_feedback_arms_0_spin[0], timer_feedback_arms_0_spin[1], timer_feedback_arms_0_spin[2], timer_feedback_arms_0_spin[0] + timer_feedback_arms_0_spin[1] + timer_feedback_arms_0_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_spin[0];
  timer_feedback_total[1] += timer_feedback_arms_0_spin[1];
  timer_feedback_total[2] += timer_feedback_arms_0_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_swing, %f, %f, %f, %f", timer_feedback_arms_0_swing[0], timer_feedback_arms_0_swing[1], timer_feedback_arms_0_swing[2], timer_feedback_arms_0_swing[0] + timer_feedback_arms_0_swing[1] + timer_feedback_arms_0_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_swing[0];
  timer_feedback_total[1] += timer_feedback_arms_0_swing[1];
  timer_feedback_total[2] += timer_feedback_arms_0_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_reachForBodyPart, %f, %f, %f, %f", timer_feedback_arms_0_reachForBodyPart[0], timer_feedback_arms_0_reachForBodyPart[1], timer_feedback_arms_0_reachForBodyPart[2], timer_feedback_arms_0_reachForBodyPart[0] + timer_feedback_arms_0_reachForBodyPart[1] + timer_feedback_arms_0_reachForBodyPart[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_reachForBodyPart[0];
  timer_feedback_total[1] += timer_feedback_arms_0_reachForBodyPart[1];
  timer_feedback_total[2] += timer_feedback_arms_0_reachForBodyPart[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_reachForWorld, %f, %f, %f, %f", timer_feedback_arms_0_reachForWorld[0], timer_feedback_arms_0_reachForWorld[1], timer_feedback_arms_0_reachForWorld[2], timer_feedback_arms_0_reachForWorld[0] + timer_feedback_arms_0_reachForWorld[1] + timer_feedback_arms_0_reachForWorld[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_reachForWorld[0];
  timer_feedback_total[1] += timer_feedback_arms_0_reachForWorld[1];
  timer_feedback_total[2] += timer_feedback_arms_0_reachForWorld[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_placement, %f, %f, %f, %f", timer_feedback_arms_0_placement[0], timer_feedback_arms_0_placement[1], timer_feedback_arms_0_placement[2], timer_feedback_arms_0_placement[0] + timer_feedback_arms_0_placement[1] + timer_feedback_arms_0_placement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_placement[0];
  timer_feedback_total[1] += timer_feedback_arms_0_placement[1];
  timer_feedback_total[2] += timer_feedback_arms_0_placement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_pose, %f, %f, %f, %f", timer_feedback_arms_0_pose[0], timer_feedback_arms_0_pose[1], timer_feedback_arms_0_pose[2], timer_feedback_arms_0_pose[0] + timer_feedback_arms_0_pose[1] + timer_feedback_arms_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_pose[0];
  timer_feedback_total[1] += timer_feedback_arms_0_pose[1];
  timer_feedback_total[2] += timer_feedback_arms_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_reachReaction, %f, %f, %f, %f", timer_feedback_arms_0_reachReaction[0], timer_feedback_arms_0_reachReaction[1], timer_feedback_arms_0_reachReaction[2], timer_feedback_arms_0_reachReaction[0] + timer_feedback_arms_0_reachReaction[1] + timer_feedback_arms_0_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_reachReaction[0];
  timer_feedback_total[1] += timer_feedback_arms_0_reachReaction[1];
  timer_feedback_total[2] += timer_feedback_arms_0_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_0_writhe, %f, %f, %f, %f", timer_feedback_arms_0_writhe[0], timer_feedback_arms_0_writhe[1], timer_feedback_arms_0_writhe[2], timer_feedback_arms_0_writhe[0] + timer_feedback_arms_0_writhe[1] + timer_feedback_arms_0_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_0_writhe[0];
  timer_feedback_total[1] += timer_feedback_arms_0_writhe[1];
  timer_feedback_total[2] += timer_feedback_arms_0_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1, %f, %f, %f, %f", timer_feedback_arms_1[0], timer_feedback_arms_1[1], timer_feedback_arms_1[2], timer_feedback_arms_1[0] + timer_feedback_arms_1[1] + timer_feedback_arms_1[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1[0];
  timer_feedback_total[1] += timer_feedback_arms_1[1];
  timer_feedback_total[2] += timer_feedback_arms_1[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_grab, %f, %f, %f, %f", timer_feedback_arms_1_grab[0], timer_feedback_arms_1_grab[1], timer_feedback_arms_1_grab[2], timer_feedback_arms_1_grab[0] + timer_feedback_arms_1_grab[1] + timer_feedback_arms_1_grab[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_grab[0];
  timer_feedback_total[1] += timer_feedback_arms_1_grab[1];
  timer_feedback_total[2] += timer_feedback_arms_1_grab[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_aim, %f, %f, %f, %f", timer_feedback_arms_1_aim[0], timer_feedback_arms_1_aim[1], timer_feedback_arms_1_aim[2], timer_feedback_arms_1_aim[0] + timer_feedback_arms_1_aim[1] + timer_feedback_arms_1_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_aim[0];
  timer_feedback_total[1] += timer_feedback_arms_1_aim[1];
  timer_feedback_total[2] += timer_feedback_arms_1_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_hold, %f, %f, %f, %f", timer_feedback_arms_1_hold[0], timer_feedback_arms_1_hold[1], timer_feedback_arms_1_hold[2], timer_feedback_arms_1_hold[0] + timer_feedback_arms_1_hold[1] + timer_feedback_arms_1_hold[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_hold[0];
  timer_feedback_total[1] += timer_feedback_arms_1_hold[1];
  timer_feedback_total[2] += timer_feedback_arms_1_hold[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_brace, %f, %f, %f, %f", timer_feedback_arms_1_brace[0], timer_feedback_arms_1_brace[1], timer_feedback_arms_1_brace[2], timer_feedback_arms_1_brace[0] + timer_feedback_arms_1_brace[1] + timer_feedback_arms_1_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_brace[0];
  timer_feedback_total[1] += timer_feedback_arms_1_brace[1];
  timer_feedback_total[2] += timer_feedback_arms_1_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_standingSupport, %f, %f, %f, %f", timer_feedback_arms_1_standingSupport[0], timer_feedback_arms_1_standingSupport[1], timer_feedback_arms_1_standingSupport[2], timer_feedback_arms_1_standingSupport[0] + timer_feedback_arms_1_standingSupport[1] + timer_feedback_arms_1_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_standingSupport[0];
  timer_feedback_total[1] += timer_feedback_arms_1_standingSupport[1];
  timer_feedback_total[2] += timer_feedback_arms_1_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_holdingSupport, %f, %f, %f, %f", timer_feedback_arms_1_holdingSupport[0], timer_feedback_arms_1_holdingSupport[1], timer_feedback_arms_1_holdingSupport[2], timer_feedback_arms_1_holdingSupport[0] + timer_feedback_arms_1_holdingSupport[1] + timer_feedback_arms_1_holdingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_holdingSupport[0];
  timer_feedback_total[1] += timer_feedback_arms_1_holdingSupport[1];
  timer_feedback_total[2] += timer_feedback_arms_1_holdingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_sittingStep, %f, %f, %f, %f", timer_feedback_arms_1_sittingStep[0], timer_feedback_arms_1_sittingStep[1], timer_feedback_arms_1_sittingStep[2], timer_feedback_arms_1_sittingStep[0] + timer_feedback_arms_1_sittingStep[1] + timer_feedback_arms_1_sittingStep[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_sittingStep[0];
  timer_feedback_total[1] += timer_feedback_arms_1_sittingStep[1];
  timer_feedback_total[2] += timer_feedback_arms_1_sittingStep[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_step, %f, %f, %f, %f", timer_feedback_arms_1_step[0], timer_feedback_arms_1_step[1], timer_feedback_arms_1_step[2], timer_feedback_arms_1_step[0] + timer_feedback_arms_1_step[1] + timer_feedback_arms_1_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_step[0];
  timer_feedback_total[1] += timer_feedback_arms_1_step[1];
  timer_feedback_total[2] += timer_feedback_arms_1_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_spin, %f, %f, %f, %f", timer_feedback_arms_1_spin[0], timer_feedback_arms_1_spin[1], timer_feedback_arms_1_spin[2], timer_feedback_arms_1_spin[0] + timer_feedback_arms_1_spin[1] + timer_feedback_arms_1_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_spin[0];
  timer_feedback_total[1] += timer_feedback_arms_1_spin[1];
  timer_feedback_total[2] += timer_feedback_arms_1_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_swing, %f, %f, %f, %f", timer_feedback_arms_1_swing[0], timer_feedback_arms_1_swing[1], timer_feedback_arms_1_swing[2], timer_feedback_arms_1_swing[0] + timer_feedback_arms_1_swing[1] + timer_feedback_arms_1_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_swing[0];
  timer_feedback_total[1] += timer_feedback_arms_1_swing[1];
  timer_feedback_total[2] += timer_feedback_arms_1_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_reachForBodyPart, %f, %f, %f, %f", timer_feedback_arms_1_reachForBodyPart[0], timer_feedback_arms_1_reachForBodyPart[1], timer_feedback_arms_1_reachForBodyPart[2], timer_feedback_arms_1_reachForBodyPart[0] + timer_feedback_arms_1_reachForBodyPart[1] + timer_feedback_arms_1_reachForBodyPart[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_reachForBodyPart[0];
  timer_feedback_total[1] += timer_feedback_arms_1_reachForBodyPart[1];
  timer_feedback_total[2] += timer_feedback_arms_1_reachForBodyPart[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_reachForWorld, %f, %f, %f, %f", timer_feedback_arms_1_reachForWorld[0], timer_feedback_arms_1_reachForWorld[1], timer_feedback_arms_1_reachForWorld[2], timer_feedback_arms_1_reachForWorld[0] + timer_feedback_arms_1_reachForWorld[1] + timer_feedback_arms_1_reachForWorld[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_reachForWorld[0];
  timer_feedback_total[1] += timer_feedback_arms_1_reachForWorld[1];
  timer_feedback_total[2] += timer_feedback_arms_1_reachForWorld[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_placement, %f, %f, %f, %f", timer_feedback_arms_1_placement[0], timer_feedback_arms_1_placement[1], timer_feedback_arms_1_placement[2], timer_feedback_arms_1_placement[0] + timer_feedback_arms_1_placement[1] + timer_feedback_arms_1_placement[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_placement[0];
  timer_feedback_total[1] += timer_feedback_arms_1_placement[1];
  timer_feedback_total[2] += timer_feedback_arms_1_placement[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_pose, %f, %f, %f, %f", timer_feedback_arms_1_pose[0], timer_feedback_arms_1_pose[1], timer_feedback_arms_1_pose[2], timer_feedback_arms_1_pose[0] + timer_feedback_arms_1_pose[1] + timer_feedback_arms_1_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_pose[0];
  timer_feedback_total[1] += timer_feedback_arms_1_pose[1];
  timer_feedback_total[2] += timer_feedback_arms_1_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_reachReaction, %f, %f, %f, %f", timer_feedback_arms_1_reachReaction[0], timer_feedback_arms_1_reachReaction[1], timer_feedback_arms_1_reachReaction[2], timer_feedback_arms_1_reachReaction[0] + timer_feedback_arms_1_reachReaction[1] + timer_feedback_arms_1_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_reachReaction[0];
  timer_feedback_total[1] += timer_feedback_arms_1_reachReaction[1];
  timer_feedback_total[2] += timer_feedback_arms_1_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback arms_1_writhe, %f, %f, %f, %f", timer_feedback_arms_1_writhe[0], timer_feedback_arms_1_writhe[1], timer_feedback_arms_1_writhe[2], timer_feedback_arms_1_writhe[0] + timer_feedback_arms_1_writhe[1] + timer_feedback_arms_1_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_arms_1_writhe[0];
  timer_feedback_total[1] += timer_feedback_arms_1_writhe[1];
  timer_feedback_total[2] += timer_feedback_arms_1_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0, %f, %f, %f, %f", timer_feedback_heads_0[0], timer_feedback_heads_0[1], timer_feedback_heads_0[2], timer_feedback_heads_0[0] + timer_feedback_heads_0[1] + timer_feedback_heads_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0[0];
  timer_feedback_total[1] += timer_feedback_heads_0[1];
  timer_feedback_total[2] += timer_feedback_heads_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_eyes, %f, %f, %f, %f", timer_feedback_heads_0_eyes[0], timer_feedback_heads_0_eyes[1], timer_feedback_heads_0_eyes[2], timer_feedback_heads_0_eyes[0] + timer_feedback_heads_0_eyes[1] + timer_feedback_heads_0_eyes[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_eyes[0];
  timer_feedback_total[1] += timer_feedback_heads_0_eyes[1];
  timer_feedback_total[2] += timer_feedback_heads_0_eyes[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_avoid, %f, %f, %f, %f", timer_feedback_heads_0_avoid[0], timer_feedback_heads_0_avoid[1], timer_feedback_heads_0_avoid[2], timer_feedback_heads_0_avoid[0] + timer_feedback_heads_0_avoid[1] + timer_feedback_heads_0_avoid[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_avoid[0];
  timer_feedback_total[1] += timer_feedback_heads_0_avoid[1];
  timer_feedback_total[2] += timer_feedback_heads_0_avoid[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_dodge, %f, %f, %f, %f", timer_feedback_heads_0_dodge[0], timer_feedback_heads_0_dodge[1], timer_feedback_heads_0_dodge[2], timer_feedback_heads_0_dodge[0] + timer_feedback_heads_0_dodge[1] + timer_feedback_heads_0_dodge[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_dodge[0];
  timer_feedback_total[1] += timer_feedback_heads_0_dodge[1];
  timer_feedback_total[2] += timer_feedback_heads_0_dodge[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_aim, %f, %f, %f, %f", timer_feedback_heads_0_aim[0], timer_feedback_heads_0_aim[1], timer_feedback_heads_0_aim[2], timer_feedback_heads_0_aim[0] + timer_feedback_heads_0_aim[1] + timer_feedback_heads_0_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_aim[0];
  timer_feedback_total[1] += timer_feedback_heads_0_aim[1];
  timer_feedback_total[2] += timer_feedback_heads_0_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_point, %f, %f, %f, %f", timer_feedback_heads_0_point[0], timer_feedback_heads_0_point[1], timer_feedback_heads_0_point[2], timer_feedback_heads_0_point[0] + timer_feedback_heads_0_point[1] + timer_feedback_heads_0_point[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_point[0];
  timer_feedback_total[1] += timer_feedback_heads_0_point[1];
  timer_feedback_total[2] += timer_feedback_heads_0_point[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_support, %f, %f, %f, %f", timer_feedback_heads_0_support[0], timer_feedback_heads_0_support[1], timer_feedback_heads_0_support[2], timer_feedback_heads_0_support[0] + timer_feedback_heads_0_support[1] + timer_feedback_heads_0_support[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_support[0];
  timer_feedback_total[1] += timer_feedback_heads_0_support[1];
  timer_feedback_total[2] += timer_feedback_heads_0_support[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback heads_0_pose, %f, %f, %f, %f", timer_feedback_heads_0_pose[0], timer_feedback_heads_0_pose[1], timer_feedback_heads_0_pose[2], timer_feedback_heads_0_pose[0] + timer_feedback_heads_0_pose[1] + timer_feedback_heads_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_heads_0_pose[0];
  timer_feedback_total[1] += timer_feedback_heads_0_pose[1];
  timer_feedback_total[2] += timer_feedback_heads_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0, %f, %f, %f, %f", timer_feedback_legs_0[0], timer_feedback_legs_0[1], timer_feedback_legs_0[2], timer_feedback_legs_0[0] + timer_feedback_legs_0[1] + timer_feedback_legs_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0[0];
  timer_feedback_total[1] += timer_feedback_legs_0[1];
  timer_feedback_total[2] += timer_feedback_legs_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_brace, %f, %f, %f, %f", timer_feedback_legs_0_brace[0], timer_feedback_legs_0_brace[1], timer_feedback_legs_0_brace[2], timer_feedback_legs_0_brace[0] + timer_feedback_legs_0_brace[1] + timer_feedback_legs_0_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_brace[0];
  timer_feedback_total[1] += timer_feedback_legs_0_brace[1];
  timer_feedback_total[2] += timer_feedback_legs_0_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_standingSupport, %f, %f, %f, %f", timer_feedback_legs_0_standingSupport[0], timer_feedback_legs_0_standingSupport[1], timer_feedback_legs_0_standingSupport[2], timer_feedback_legs_0_standingSupport[0] + timer_feedback_legs_0_standingSupport[1] + timer_feedback_legs_0_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_standingSupport[0];
  timer_feedback_total[1] += timer_feedback_legs_0_standingSupport[1];
  timer_feedback_total[2] += timer_feedback_legs_0_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_sittingSupport, %f, %f, %f, %f", timer_feedback_legs_0_sittingSupport[0], timer_feedback_legs_0_sittingSupport[1], timer_feedback_legs_0_sittingSupport[2], timer_feedback_legs_0_sittingSupport[0] + timer_feedback_legs_0_sittingSupport[1] + timer_feedback_legs_0_sittingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_sittingSupport[0];
  timer_feedback_total[1] += timer_feedback_legs_0_sittingSupport[1];
  timer_feedback_total[2] += timer_feedback_legs_0_sittingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_step, %f, %f, %f, %f", timer_feedback_legs_0_step[0], timer_feedback_legs_0_step[1], timer_feedback_legs_0_step[2], timer_feedback_legs_0_step[0] + timer_feedback_legs_0_step[1] + timer_feedback_legs_0_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_step[0];
  timer_feedback_total[1] += timer_feedback_legs_0_step[1];
  timer_feedback_total[2] += timer_feedback_legs_0_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_swing, %f, %f, %f, %f", timer_feedback_legs_0_swing[0], timer_feedback_legs_0_swing[1], timer_feedback_legs_0_swing[2], timer_feedback_legs_0_swing[0] + timer_feedback_legs_0_swing[1] + timer_feedback_legs_0_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_swing[0];
  timer_feedback_total[1] += timer_feedback_legs_0_swing[1];
  timer_feedback_total[2] += timer_feedback_legs_0_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_pose, %f, %f, %f, %f", timer_feedback_legs_0_pose[0], timer_feedback_legs_0_pose[1], timer_feedback_legs_0_pose[2], timer_feedback_legs_0_pose[0] + timer_feedback_legs_0_pose[1] + timer_feedback_legs_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_pose[0];
  timer_feedback_total[1] += timer_feedback_legs_0_pose[1];
  timer_feedback_total[2] += timer_feedback_legs_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_spin, %f, %f, %f, %f", timer_feedback_legs_0_spin[0], timer_feedback_legs_0_spin[1], timer_feedback_legs_0_spin[2], timer_feedback_legs_0_spin[0] + timer_feedback_legs_0_spin[1] + timer_feedback_legs_0_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_spin[0];
  timer_feedback_total[1] += timer_feedback_legs_0_spin[1];
  timer_feedback_total[2] += timer_feedback_legs_0_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_reachReaction, %f, %f, %f, %f", timer_feedback_legs_0_reachReaction[0], timer_feedback_legs_0_reachReaction[1], timer_feedback_legs_0_reachReaction[2], timer_feedback_legs_0_reachReaction[0] + timer_feedback_legs_0_reachReaction[1] + timer_feedback_legs_0_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_reachReaction[0];
  timer_feedback_total[1] += timer_feedback_legs_0_reachReaction[1];
  timer_feedback_total[2] += timer_feedback_legs_0_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_0_writhe, %f, %f, %f, %f", timer_feedback_legs_0_writhe[0], timer_feedback_legs_0_writhe[1], timer_feedback_legs_0_writhe[2], timer_feedback_legs_0_writhe[0] + timer_feedback_legs_0_writhe[1] + timer_feedback_legs_0_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_0_writhe[0];
  timer_feedback_total[1] += timer_feedback_legs_0_writhe[1];
  timer_feedback_total[2] += timer_feedback_legs_0_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1, %f, %f, %f, %f", timer_feedback_legs_1[0], timer_feedback_legs_1[1], timer_feedback_legs_1[2], timer_feedback_legs_1[0] + timer_feedback_legs_1[1] + timer_feedback_legs_1[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1[0];
  timer_feedback_total[1] += timer_feedback_legs_1[1];
  timer_feedback_total[2] += timer_feedback_legs_1[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_brace, %f, %f, %f, %f", timer_feedback_legs_1_brace[0], timer_feedback_legs_1_brace[1], timer_feedback_legs_1_brace[2], timer_feedback_legs_1_brace[0] + timer_feedback_legs_1_brace[1] + timer_feedback_legs_1_brace[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_brace[0];
  timer_feedback_total[1] += timer_feedback_legs_1_brace[1];
  timer_feedback_total[2] += timer_feedback_legs_1_brace[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_standingSupport, %f, %f, %f, %f", timer_feedback_legs_1_standingSupport[0], timer_feedback_legs_1_standingSupport[1], timer_feedback_legs_1_standingSupport[2], timer_feedback_legs_1_standingSupport[0] + timer_feedback_legs_1_standingSupport[1] + timer_feedback_legs_1_standingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_standingSupport[0];
  timer_feedback_total[1] += timer_feedback_legs_1_standingSupport[1];
  timer_feedback_total[2] += timer_feedback_legs_1_standingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_sittingSupport, %f, %f, %f, %f", timer_feedback_legs_1_sittingSupport[0], timer_feedback_legs_1_sittingSupport[1], timer_feedback_legs_1_sittingSupport[2], timer_feedback_legs_1_sittingSupport[0] + timer_feedback_legs_1_sittingSupport[1] + timer_feedback_legs_1_sittingSupport[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_sittingSupport[0];
  timer_feedback_total[1] += timer_feedback_legs_1_sittingSupport[1];
  timer_feedback_total[2] += timer_feedback_legs_1_sittingSupport[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_step, %f, %f, %f, %f", timer_feedback_legs_1_step[0], timer_feedback_legs_1_step[1], timer_feedback_legs_1_step[2], timer_feedback_legs_1_step[0] + timer_feedback_legs_1_step[1] + timer_feedback_legs_1_step[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_step[0];
  timer_feedback_total[1] += timer_feedback_legs_1_step[1];
  timer_feedback_total[2] += timer_feedback_legs_1_step[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_swing, %f, %f, %f, %f", timer_feedback_legs_1_swing[0], timer_feedback_legs_1_swing[1], timer_feedback_legs_1_swing[2], timer_feedback_legs_1_swing[0] + timer_feedback_legs_1_swing[1] + timer_feedback_legs_1_swing[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_swing[0];
  timer_feedback_total[1] += timer_feedback_legs_1_swing[1];
  timer_feedback_total[2] += timer_feedback_legs_1_swing[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_pose, %f, %f, %f, %f", timer_feedback_legs_1_pose[0], timer_feedback_legs_1_pose[1], timer_feedback_legs_1_pose[2], timer_feedback_legs_1_pose[0] + timer_feedback_legs_1_pose[1] + timer_feedback_legs_1_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_pose[0];
  timer_feedback_total[1] += timer_feedback_legs_1_pose[1];
  timer_feedback_total[2] += timer_feedback_legs_1_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_spin, %f, %f, %f, %f", timer_feedback_legs_1_spin[0], timer_feedback_legs_1_spin[1], timer_feedback_legs_1_spin[2], timer_feedback_legs_1_spin[0] + timer_feedback_legs_1_spin[1] + timer_feedback_legs_1_spin[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_spin[0];
  timer_feedback_total[1] += timer_feedback_legs_1_spin[1];
  timer_feedback_total[2] += timer_feedback_legs_1_spin[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_reachReaction, %f, %f, %f, %f", timer_feedback_legs_1_reachReaction[0], timer_feedback_legs_1_reachReaction[1], timer_feedback_legs_1_reachReaction[2], timer_feedback_legs_1_reachReaction[0] + timer_feedback_legs_1_reachReaction[1] + timer_feedback_legs_1_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_reachReaction[0];
  timer_feedback_total[1] += timer_feedback_legs_1_reachReaction[1];
  timer_feedback_total[2] += timer_feedback_legs_1_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback legs_1_writhe, %f, %f, %f, %f", timer_feedback_legs_1_writhe[0], timer_feedback_legs_1_writhe[1], timer_feedback_legs_1_writhe[2], timer_feedback_legs_1_writhe[0] + timer_feedback_legs_1_writhe[1] + timer_feedback_legs_1_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_legs_1_writhe[0];
  timer_feedback_total[1] += timer_feedback_legs_1_writhe[1];
  timer_feedback_total[2] += timer_feedback_legs_1_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0, %f, %f, %f, %f", timer_feedback_spines_0[0], timer_feedback_spines_0[1], timer_feedback_spines_0[2], timer_feedback_spines_0[0] + timer_feedback_spines_0[1] + timer_feedback_spines_0[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0[0];
  timer_feedback_total[1] += timer_feedback_spines_0[1];
  timer_feedback_total[2] += timer_feedback_spines_0[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0_support, %f, %f, %f, %f", timer_feedback_spines_0_support[0], timer_feedback_spines_0_support[1], timer_feedback_spines_0_support[2], timer_feedback_spines_0_support[0] + timer_feedback_spines_0_support[1] + timer_feedback_spines_0_support[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0_support[0];
  timer_feedback_total[1] += timer_feedback_spines_0_support[1];
  timer_feedback_total[2] += timer_feedback_spines_0_support[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0_pose, %f, %f, %f, %f", timer_feedback_spines_0_pose[0], timer_feedback_spines_0_pose[1], timer_feedback_spines_0_pose[2], timer_feedback_spines_0_pose[0] + timer_feedback_spines_0_pose[1] + timer_feedback_spines_0_pose[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0_pose[0];
  timer_feedback_total[1] += timer_feedback_spines_0_pose[1];
  timer_feedback_total[2] += timer_feedback_spines_0_pose[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0_control, %f, %f, %f, %f", timer_feedback_spines_0_control[0], timer_feedback_spines_0_control[1], timer_feedback_spines_0_control[2], timer_feedback_spines_0_control[0] + timer_feedback_spines_0_control[1] + timer_feedback_spines_0_control[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0_control[0];
  timer_feedback_total[1] += timer_feedback_spines_0_control[1];
  timer_feedback_total[2] += timer_feedback_spines_0_control[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0_reachReaction, %f, %f, %f, %f", timer_feedback_spines_0_reachReaction[0], timer_feedback_spines_0_reachReaction[1], timer_feedback_spines_0_reachReaction[2], timer_feedback_spines_0_reachReaction[0] + timer_feedback_spines_0_reachReaction[1] + timer_feedback_spines_0_reachReaction[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0_reachReaction[0];
  timer_feedback_total[1] += timer_feedback_spines_0_reachReaction[1];
  timer_feedback_total[2] += timer_feedback_spines_0_reachReaction[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0_writhe, %f, %f, %f, %f", timer_feedback_spines_0_writhe[0], timer_feedback_spines_0_writhe[1], timer_feedback_spines_0_writhe[2], timer_feedback_spines_0_writhe[0] + timer_feedback_spines_0_writhe[1] + timer_feedback_spines_0_writhe[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0_writhe[0];
  timer_feedback_total[1] += timer_feedback_spines_0_writhe[1];
  timer_feedback_total[2] += timer_feedback_spines_0_writhe[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback spines_0_aim, %f, %f, %f, %f", timer_feedback_spines_0_aim[0], timer_feedback_spines_0_aim[1], timer_feedback_spines_0_aim[2], timer_feedback_spines_0_aim[0] + timer_feedback_spines_0_aim[1] + timer_feedback_spines_0_aim[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_spines_0_aim[0];
  timer_feedback_total[1] += timer_feedback_spines_0_aim[1];
  timer_feedback_total[2] += timer_feedback_spines_0_aim[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback sceneProbes, %f, %f, %f, %f", timer_feedback_sceneProbes[0], timer_feedback_sceneProbes[1], timer_feedback_sceneProbes[2], timer_feedback_sceneProbes[0] + timer_feedback_sceneProbes[1] + timer_feedback_sceneProbes[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_sceneProbes[0];
  timer_feedback_total[1] += timer_feedback_sceneProbes[1];
  timer_feedback_total[2] += timer_feedback_sceneProbes[2];
#ifdef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback balanceBehaviourFeedback, %f, %f, %f, %f", timer_feedback_balanceBehaviourFeedback[0], timer_feedback_balanceBehaviourFeedback[1], timer_feedback_balanceBehaviourFeedback[2], timer_feedback_balanceBehaviourFeedback[0] + timer_feedback_balanceBehaviourFeedback[1] + timer_feedback_balanceBehaviourFeedback[2]);
#endif // EUPHORIA_PROFILING_DETAILED
  timer_feedback_total[0] += timer_feedback_balanceBehaviourFeedback[0];
  timer_feedback_total[1] += timer_feedback_balanceBehaviourFeedback[1];
  timer_feedback_total[2] += timer_feedback_balanceBehaviourFeedback[2];
#ifndef EUPHORIA_PROFILING_DETAILED
  NMP_MSG("feedback total, %f, %f, %f, %f", timer_feedback_total[0], timer_feedback_total[1], timer_feedback_total[2], timer_feedback_total[0] + timer_feedback_total[1] + timer_feedback_total[2]);
  timer_total[0] += g_profilingTotalTimerFraction * (timer_feedback_total[0] + timer_update_total[0] - timer_total[0]);
  timer_total[1] += g_profilingTotalTimerFraction * (timer_feedback_total[1] + timer_update_total[1] - timer_total[1]);
  timer_total[2] += g_profilingTotalTimerFraction * (timer_feedback_total[2] + timer_update_total[2] - timer_total[2]);
  NMP_MSG("total, %f, %f, %f, %f\n", timer_total[0], timer_total[1], timer_total[2], timer_total[0] + timer_total[1] + timer_total[2]);
#endif // EUPHORIA_PROFILING_DETAILED
#endif // EUPHORIA_PROFILING
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

