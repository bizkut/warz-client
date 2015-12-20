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

#ifndef NM_MDF_MYNETWORK_PKG_H
#define NM_MDF_MYNETWORK_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/MyNetwork.module"

#include "MyNetworkData.h"
#include "CharacteristicsBehaviourInterfaceData.h"
#include "PropertiesBehaviourInterfaceData.h"
#include "EuphoriaRagdollBehaviourInterfaceData.h"
#include "EyesBehaviourInterfaceData.h"
#include "ArmsBraceBehaviourInterfaceData.h"
#include "ArmsPlacementBehaviourInterfaceData.h"
#include "AimBehaviourInterfaceData.h"
#include "HeadDodgeBehaviourInterfaceData.h"
#include "ReachForBodyBehaviourInterfaceData.h"
#include "ReachForWorldBehaviourInterfaceData.h"
#include "AnimateBehaviourInterfaceData.h"
#include "BalanceBehaviourInterfaceData.h"
#include "BalancePoserBehaviourInterfaceData.h"
#include "ProtectBehaviourInterfaceData.h"
#include "HazardAwarenessBehaviourInterfaceData.h"
#include "UserHazardBehaviourInterfaceData.h"
#include "ObserveBehaviourInterfaceData.h"
#include "IdleAwakeBehaviourInterfaceData.h"
#include "LookBehaviourInterfaceData.h"
#include "HeadAvoidBehaviourInterfaceData.h"
#include "ShieldBehaviourInterfaceData.h"
#include "HoldBehaviourInterfaceData.h"
#include "HoldActionBehaviourInterfaceData.h"
#include "FreeFallBehaviourInterfaceData.h"
#include "ArmsWindmillBehaviourInterfaceData.h"
#include "LegsPedalBehaviourInterfaceData.h"
#include "ShieldActionBehaviourInterfaceData.h"
#include "SitBehaviourInterfaceData.h"
#include "WritheBehaviourInterfaceData.h"
#include "RandomLookData.h"
#include "HazardManagementData.h"
#include "BalanceManagementData.h"
#include "EnvironmentAwarenessData.h"
#include "BodyFrameData.h"
#include "BodySectionData.h"
#include "BodySectionData.h"
#include "ArmData.h"
#include "HeadData.h"
#include "LegData.h"
#include "SpineData.h"
#include "SceneProbesData.h"
#include "BalanceBehaviourFeedbackData.h"




namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct CharacteristicsBehaviourInterfaceUpdatePackage;
struct CharacteristicsBehaviourInterfaceFeedbackPackage;
struct PropertiesBehaviourInterfaceUpdatePackage;
struct PropertiesBehaviourInterfaceFeedbackPackage;
struct EuphoriaRagdollBehaviourInterfaceUpdatePackage;
struct EuphoriaRagdollBehaviourInterfaceFeedbackPackage;
struct EyesBehaviourInterfaceUpdatePackage;
struct EyesBehaviourInterfaceFeedbackPackage;
class EyesBehaviourInterface_Con;
struct ArmsBraceBehaviourInterfaceUpdatePackage;
struct ArmsBraceBehaviourInterfaceFeedbackPackage;
struct ArmsPlacementBehaviourInterfaceUpdatePackage;
struct ArmsPlacementBehaviourInterfaceFeedbackPackage;
struct AimBehaviourInterfaceUpdatePackage;
struct AimBehaviourInterfaceFeedbackPackage;
class AimBehaviourInterface_Con;
struct HeadDodgeBehaviourInterfaceUpdatePackage;
struct HeadDodgeBehaviourInterfaceFeedbackPackage;
struct ReachForBodyBehaviourInterfaceUpdatePackage;
struct ReachForBodyBehaviourInterfaceFeedbackPackage;
class ReachForBodyBehaviourInterface_Con;
struct ReachForWorldBehaviourInterfaceUpdatePackage;
struct ReachForWorldBehaviourInterfaceFeedbackPackage;
class ReachForWorldBehaviourInterface_Con;
struct AnimateBehaviourInterfaceUpdatePackage;
struct AnimateBehaviourInterfaceFeedbackPackage;
struct BalanceBehaviourInterfaceUpdatePackage;
struct BalanceBehaviourInterfaceFeedbackPackage;
class BalanceBehaviourInterface_Con;
struct BalancePoserBehaviourInterfaceUpdatePackage;
struct BalancePoserBehaviourInterfaceFeedbackPackage;
class BalancePoserBehaviourInterface_Con;
struct ProtectBehaviourInterfaceUpdatePackage;
struct ProtectBehaviourInterfaceFeedbackPackage;
struct HazardAwarenessBehaviourInterfaceUpdatePackage;
struct HazardAwarenessBehaviourInterfaceFeedbackPackage;
class HazardAwarenessBehaviourInterface_Con;
struct UserHazardBehaviourInterfaceUpdatePackage;
struct UserHazardBehaviourInterfaceFeedbackPackage;
class UserHazardBehaviourInterface_Con;
struct ObserveBehaviourInterfaceUpdatePackage;
struct ObserveBehaviourInterfaceFeedbackPackage;
class ObserveBehaviourInterface_Con;
struct IdleAwakeBehaviourInterfaceUpdatePackage;
struct IdleAwakeBehaviourInterfaceFeedbackPackage;
class IdleAwakeBehaviourInterface_Con;
struct LookBehaviourInterfaceUpdatePackage;
struct LookBehaviourInterfaceFeedbackPackage;
struct HeadAvoidBehaviourInterfaceUpdatePackage;
struct HeadAvoidBehaviourInterfaceFeedbackPackage;
struct ShieldBehaviourInterfaceUpdatePackage;
struct ShieldBehaviourInterfaceFeedbackPackage;
class ShieldBehaviourInterface_Con;
struct HoldBehaviourInterfaceUpdatePackage;
struct HoldBehaviourInterfaceFeedbackPackage;
class HoldBehaviourInterface_Con;
struct HoldActionBehaviourInterfaceUpdatePackage;
struct HoldActionBehaviourInterfaceFeedbackPackage;
class HoldActionBehaviourInterface_Con;
struct FreeFallBehaviourInterfaceUpdatePackage;
struct FreeFallBehaviourInterfaceFeedbackPackage;
class FreeFallBehaviourInterface_Con;
struct ArmsWindmillBehaviourInterfaceUpdatePackage;
struct ArmsWindmillBehaviourInterfaceFeedbackPackage;
struct LegsPedalBehaviourInterfaceUpdatePackage;
struct LegsPedalBehaviourInterfaceFeedbackPackage;
struct ShieldActionBehaviourInterfaceUpdatePackage;
struct ShieldActionBehaviourInterfaceFeedbackPackage;
class ShieldActionBehaviourInterface_Con;
struct SitBehaviourInterfaceUpdatePackage;
struct SitBehaviourInterfaceFeedbackPackage;
class SitBehaviourInterface_Con;
struct WritheBehaviourInterfaceUpdatePackage;
struct WritheBehaviourInterfaceFeedbackPackage;
struct RandomLookUpdatePackage;
struct RandomLookFeedbackPackage;
class RandomLook_Con;
struct HazardManagementUpdatePackage;
struct HazardManagementFeedbackPackage;
class HazardManagement_Con;
struct GrabDetectionUpdatePackage;
struct GrabDetectionFeedbackPackage;
class GrabDetection_Con;
struct GrabUpdatePackage;
struct GrabFeedbackPackage;
class Grab_Con;
struct HazardResponseUpdatePackage;
struct HazardResponseFeedbackPackage;
class HazardResponse_Con;
struct FreeFallManagementUpdatePackage;
struct FreeFallManagementFeedbackPackage;
class FreeFallManagement_Con;
struct ImpactPredictorUpdatePackage;
struct ImpactPredictorFeedbackPackage;
class ImpactPredictor_chestCon;
struct ShieldManagementUpdatePackage;
struct ShieldManagementFeedbackPackage;
class ShieldManagement_Con;
struct BalanceManagementUpdatePackage;
struct BalanceManagementFeedbackPackage;
class BalanceManagement_Con;
struct StaticBalanceUpdatePackage;
struct StaticBalanceFeedbackPackage;
class StaticBalance_Con;
struct SteppingBalanceUpdatePackage;
struct SteppingBalanceFeedbackPackage;
class SteppingBalance_Con;
struct BalancePoserUpdatePackage;
struct BalancePoserFeedbackPackage;
class BalancePoser_Con;
struct EnvironmentAwarenessUpdatePackage;
struct EnvironmentAwarenessFeedbackPackage;
class EnvironmentAwareness_Con;
struct BodyFrameUpdatePackage;
struct BodyFrameFeedbackPackage;
class BodyFrame_Con;
struct SupportPolygonUpdatePackage;
struct SupportPolygonFeedbackPackage;
class SupportPolygon_StandingCon;
struct SupportPolygonUpdatePackage;
struct SupportPolygonFeedbackPackage;
class SupportPolygon_SittingCon;
struct BodyBalanceUpdatePackage;
struct BodyBalanceFeedbackPackage;
class BodyBalance_Con;
struct SittingBodyBalanceUpdatePackage;
struct SittingBodyBalanceFeedbackPackage;
class SittingBodyBalance_Con;
struct ReachForBodyUpdatePackage;
struct ReachForBodyFeedbackPackage;
class ReachForBody_Con;
struct BalanceAssistantUpdatePackage;
struct BalanceAssistantFeedbackPackage;
class BalanceAssistant_Con;
struct BodySectionUpdatePackage;
struct BodySectionFeedbackPackage;
class BodySection_upperCon;
struct RotateCoreUpdatePackage;
struct RotateCoreFeedbackPackage;
class RotateCore_Con;
struct PositionCoreUpdatePackage;
struct PositionCoreFeedbackPackage;
class PositionCore_Con;
struct BraceChooserUpdatePackage;
struct BraceChooserFeedbackPackage;
class BraceChooser_Con;
struct BodySectionUpdatePackage;
struct BodySectionFeedbackPackage;
class BodySection_lowerCon;
struct RotateCoreUpdatePackage;
struct RotateCoreFeedbackPackage;
class RotateCore_Con;
struct PositionCoreUpdatePackage;
struct PositionCoreFeedbackPackage;
class PositionCore_Con;
struct BraceChooserUpdatePackage;
struct BraceChooserFeedbackPackage;
class BraceChooser_Con;
struct ArmUpdatePackage;
struct ArmFeedbackPackage;
class Arm_Con;
struct ArmGrabUpdatePackage;
struct ArmGrabFeedbackPackage;
class ArmGrab_Con;
struct ArmAimUpdatePackage;
struct ArmAimFeedbackPackage;
class ArmAim_Con;
struct ArmHoldUpdatePackage;
struct ArmHoldFeedbackPackage;
class ArmHold_Con;
struct ArmBraceUpdatePackage;
struct ArmBraceFeedbackPackage;
class ArmBrace_Con;
struct ArmStandingSupportUpdatePackage;
struct ArmStandingSupportFeedbackPackage;
class ArmStandingSupport_Con;
struct ArmHoldingSupportUpdatePackage;
struct ArmHoldingSupportFeedbackPackage;
class ArmHoldingSupport_Con;
struct ArmSittingStepUpdatePackage;
struct ArmSittingStepFeedbackPackage;
class ArmSittingStep_Con;
struct ArmStepUpdatePackage;
struct ArmStepFeedbackPackage;
class ArmStep_Con;
struct ArmSpinUpdatePackage;
struct ArmSpinFeedbackPackage;
class ArmSpin_Con;
struct ArmSwingUpdatePackage;
struct ArmSwingFeedbackPackage;
class ArmSwing_Con;
struct ArmReachForBodyPartUpdatePackage;
struct ArmReachForBodyPartFeedbackPackage;
class ArmReachForBodyPart_Con;
struct ArmReachForWorldUpdatePackage;
struct ArmReachForWorldFeedbackPackage;
class ArmReachForWorld_Con;
struct ArmPlacementUpdatePackage;
struct ArmPlacementFeedbackPackage;
class ArmPlacement_Con;
struct ArmPoseUpdatePackage;
struct ArmPoseFeedbackPackage;
class ArmPose_Con;
struct ArmReachReactionUpdatePackage;
struct ArmReachReactionFeedbackPackage;
class ArmReachReaction_Con;
struct ArmWritheUpdatePackage;
struct ArmWritheFeedbackPackage;
class ArmWrithe_Con;
struct HeadUpdatePackage;
struct HeadFeedbackPackage;
class Head_Con;
struct HeadEyesUpdatePackage;
struct HeadEyesFeedbackPackage;
class HeadEyes_Con;
struct HeadAvoidUpdatePackage;
struct HeadAvoidFeedbackPackage;
class HeadAvoid_Con;
struct HeadDodgeUpdatePackage;
struct HeadDodgeFeedbackPackage;
class HeadDodge_Con;
struct HeadAimUpdatePackage;
struct HeadAimFeedbackPackage;
class HeadAim_Con;
struct HeadPointUpdatePackage;
struct HeadPointFeedbackPackage;
class HeadPoint_Con;
struct HeadSupportUpdatePackage;
struct HeadSupportFeedbackPackage;
class HeadSupport_Con;
struct HeadPoseUpdatePackage;
struct HeadPoseFeedbackPackage;
class HeadPose_Con;
struct LegUpdatePackage;
struct LegFeedbackPackage;
class Leg_Con;
struct LegBraceUpdatePackage;
struct LegBraceFeedbackPackage;
class LegBrace_Con;
struct LegStandingSupportUpdatePackage;
struct LegStandingSupportFeedbackPackage;
class LegStandingSupport_Con;
struct LegSittingSupportUpdatePackage;
struct LegSittingSupportFeedbackPackage;
class LegSittingSupport_Con;
struct LegStepUpdatePackage;
struct LegStepFeedbackPackage;
class LegStep_Con;
struct LegSwingUpdatePackage;
struct LegSwingFeedbackPackage;
class LegSwing_Con;
struct LegPoseUpdatePackage;
struct LegPoseFeedbackPackage;
class LegPose_Con;
struct LegSpinUpdatePackage;
struct LegSpinFeedbackPackage;
class LegSpin_Con;
struct LegReachReactionUpdatePackage;
struct LegReachReactionFeedbackPackage;
class LegReachReaction_Con;
struct LegWritheUpdatePackage;
struct LegWritheFeedbackPackage;
class LegWrithe_Con;
struct SpineUpdatePackage;
struct SpineFeedbackPackage;
class Spine_Con;
struct SpineSupportUpdatePackage;
struct SpineSupportFeedbackPackage;
class SpineSupport_Con;
struct SpinePoseUpdatePackage;
struct SpinePoseFeedbackPackage;
class SpinePose_Con;
struct SpineControlUpdatePackage;
struct SpineControlFeedbackPackage;
class SpineControl_Con;
struct SpineReachReactionUpdatePackage;
struct SpineReachReactionFeedbackPackage;
class SpineReachReaction_Con;
struct SpineWritheUpdatePackage;
struct SpineWritheFeedbackPackage;
class SpineWrithe_Con;
struct SpineAimUpdatePackage;
struct SpineAimFeedbackPackage;
class SpineAim_Con;
struct SceneProbesUpdatePackage;
struct SceneProbesFeedbackPackage;
class SceneProbes_Con;
struct BalanceBehaviourFeedbackUpdatePackage;
struct BalanceBehaviourFeedbackFeedbackPackage;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct MyNetworkAPIBase
{
  MyNetworkAPIBase(
    const MyNetworkData* const _data  ) :data(_data)  {}

  const MyNetworkData* const data;

  MyNetworkAPIBase(const MyNetworkAPIBase& rhs);
  MyNetworkAPIBase& operator = (const MyNetworkAPIBase& rhs);

  // public API functions declared in module definition

  bool rampDownLargeAccelerations(NMP::Vector3& acc) const;
  NMP::Vector3 smoothAcceleration(const NMP::Vector3& currentAcc, const NMP::Vector3& newAcc, float timeStep) const;
  NMP::Vector3 smoothAngularVelocity(const NMP::Vector3& currentAngVel, const NMP::Vector3& newAngVel, float timeStep) const;

};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_MYNETWORK_PKG_H

