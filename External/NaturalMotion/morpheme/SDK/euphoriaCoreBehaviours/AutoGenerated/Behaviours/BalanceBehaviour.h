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

#ifndef NM_MDF_BDEF_BALANCE_H
#define NM_MDF_BDEF_BALANCE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Balance.behaviour"

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "euphoria/erBehaviour.h"
#include "euphoria/erLimb.h"

#include "NetworkDescriptor.h"
#include "NetworkConstants.h"


namespace ER
{
  class Character;
  class Module;
}




#ifdef NM_COMPILER_MSVC
# pragma warning ( push )
# pragma warning ( disable : 4324 ) // structure was padded due to __declspec(align()) 
#endif // NM_COMPILER_MSVC

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct BalancePoseRequirements
{
  bool enablePoseInput_BalancePose;
  bool enablePoseInput_ReadyPose;

  BalancePoseRequirements()
  {
    enablePoseInput_BalancePose = true;
    enablePoseInput_ReadyPose = true;
  }
};

struct BalanceBehaviourData
{
  friend class BalanceBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 TargetVelocity;  ///< Creates a tendency to lean/stagger in a certain direction (will provoke stepping if enabled). Requires a non-zero velocity assistance. Specified in absolute units.
    NMP::Vector3 TargetPelvisDirection;  ///< Target direction in world space for the pelvis. The character will try to align the pelvis forwards direction with this. The magnitude determines how much this is used, so 0 means it is ignored, and 1 means it overrides the normal direction, based on the character's current orientation.
    NMP::Vector3 ExclusionZonePoint;  ///< Point on the plane defining the stepping exclusion zone, in absolute units
    NMP::Vector3 ExclusionZoneDirection;  ///< Points into the exclusion zone. If the direction is zero then the exclusion zone is disabled.
    float BalanceFwdOffset;  ///< Sets a forwards offset for the pelvis position when balancing, in metres (standard character).
    float BalanceRightOffset;  ///< Sets a rightwards offset for the pelvis position when balancing, in metres (standard character).
    float LegStepStrength[NetworkConstants::networkMaxNumLegs];  ///< Sets the stepping strength scale on the legs. Accepts values in the range 0 to 1.
    float ArmStepStrength[NetworkConstants::networkMaxNumArms];  ///< Sets the stepping strength scale on the arms. Accepts values in the range 0 to 1.
    float BalanceWithAnimationPose;  ///< If zero, then the balancer tries to place the centre of mass over the support area. If one then it tries to match the animation pose (which may not be intrinsically balanced). Intermediate values blend between these extremes.
    float LegStandStrength[NetworkConstants::networkMaxNumLegs];  ///< Sets the stand strength scale on the legs. Accepts values in the range 0 to 1.
    float ArmStandStrength[NetworkConstants::networkMaxNumArms];  ///< Sets the stand strength scale on the arms. Accepts values in the range 0 to 1.
    float OrientationAssistanceAmount;  ///< Sets the scale of cheat forces which can be used in keeping the pelvis orientated correctly for balance. A value of 1 results in a moderate amount of assistance.
    float PositionAssistanceAmount;  ///< Sets the scale of cheat forces which can be used in keeping the pelvis positioned correctly for balance. A value of 1 results in a moderate amount of assistance.
    float VelocityAssistanceAmount;  ///< Sets the scale of cheat forces which can be used in damping the pelvis/chest velocity towards the target/desired velocity. A value of 1 results in a moderate amount of assistance.
    bool TargetVelocityInCharacterSpace;  ///< If true then the target velocity is specified in character space with x forward, y up and z right. If false then it is specified in world space.
    bool LookInStepDirection;  ///< Activates the tendency for the character to look in the direction it is stepping.
    bool UseCounterForceOnFeet;  ///< Determines whether the stabilisation forces applied to the pelvis should be compensated for by an equal an opposite force on the feet. This helps preserve momentum when stepping on dynamic objects.
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    NMP::Vector3 ForwardsDirection;  ///< Direction forwards relative to the character, projected onto the horizontal plane
    NMP::Vector3 RightDirection;  ///< Direction sideways to the right of the character, projected onto the horizontal plane
    NMP::Vector3 SupportVelocity;  ///< Velocity of the balance support region
    float StandingStillTime;  ///< The time that the character has been balancing without stepping (will be zero if stepping or fallen). In absolute units.
    float FallenTime;  ///< The time that the character has been fallen, defined as when BalanceAmount is 0. In absolute units.
    float OnGroundTime;  ///< The time that the character has been touching something that appears to be ground with his legs/feet. In absolute units.
    float BalanceAmount;  ///< An indicator of how balanced the character is from 0 to 1. Will be close to 1 when balanced, and decrease to 0 if he is falling/lifted off the ground.
    float SteppingTime;  ///< The time that stepping has been active (will be reset after StepCountResetTime). In absolute units.
    int32_t StepCount;  ///< The total number of steps (will be reset after StepCountResetTime)
    float LegStepFractions[NetworkConstants::networkMaxNumLegs];  ///< The approximate fraction through the step so 0 means the leg is just about to leave the ground, 1 when it reaches its target and the step finishes.
  };
  OutputControlParams m_ocp;


  uint32_t m_messages;

  BodyPoseData  pd_BalancePose;
  BodyPoseData  pd_ReadyPose;
  int32_t CollisionGroupIndex;  ///< Collision group used to disable leg/arm collision when stepping. This is the name of the collision group within the collision groups editor. This name is stored as an index into the group as appears in the collision group index.
  float BalancePose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float FootBalanceAmount;  ///< Amount to angle the feet to balance (this is the foot target angle in degrees when at the edge of balancing). Generally make it as large as possible, but if it is too large it will cause the feet to jitter.
  float DecelerationAmount;  ///< Amount that the character should lean away from movement in order to decelerate
  float MaxAngle;  ///< Maximum angle for the feet, in degrees
  float FootLength;  ///< Foot length in metres (standard character). Smaller values result in the character stepping earlier, which can prevent him teetering on the edge of static balance.
  float LowerPelvisDistanceWhenFootLifts;  ///< Distance to lower the pelvis (i.e. crouch) when a supporting foot lifts off the floor. In metres (standard character).
  float BalancePose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float SpinAmount;  ///< Scales the amount and speed of arm spinning
  float SpinThreshold;  ///< How off-balance the character needs to be to start spinning his arms
  float BalancePose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float StepDownDistance;  ///< When the balancer wishes to end a step he aims this distance below the ground. In metres (standard character).
  float BalancePose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  float ReadyPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float SuppressSteppingTime;  ///< Prevent stepping for this time after enabling, in seconds (standard character).
  float ArmSwingStrengthScaleWhenStepping;  /// Scales the strength with which the character can swing its arms when stepping. 
  
  float StepCountResetTime;  /// The StepCount that is emitted as an output parameter is reset if this amount of time has passed from the last step. In seconds (standard character).
  
  float ReadyPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float FootCrossingOffset;  /// Positive values will cut the step short before it would cross the other foot. Negative values will allow for more foot crossing. In metres (standard character).
  
  float StepPredictionTimeForward;  ///< Time used to calculate the step length when stepping forwards, in seconds (standard character).
  float StepPredictionTimeBackward;  ///< Time used to calculate the step length when stepping backwards, in seconds (standard character).
  float StepUpDistance;  ///< How high the steps should be, in metres (standard character). Higher values will tend to make the character stagger with high steps, so the foot will be unlikely to catch on the ground. Lower values may look more natural, at the risk of catching the foot on the ground.
  float ReadyPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float MaxStepLength;  ///< Max step length, in metres (standard character).
  float StepDownSpeed;  ///< Speed to aim the foot down as the step time progresses, in m/s (standard character). This can help prevent the foot from hanging if for some reason it does not progress.
  float FootSeparationFraction;  ///< Target foot separation as a fraction of the balance pose during the step.
  float AlignFootToFloorWeight;  ///< Amount to align the foot to the floor (rather than making it follow the natural swing of the leg) during the step.
  float LowerPelvisAmount;  ///< Amount to lower the pelvis when stepping as a fraction of the step length.
  float StrengthScale;  ///< Step strength multiplier, so larger values make the stepping leg stronger.
  float DampingRatioScale;  ///< Step damping ratio multiplier, so larger values make the stepping leg move slower but more stiffly.
  float SteppingOrientationWeight;  ///< IK orientation weight.
  float IKSubStep;  ///< IK sub step (values < 1 make the stepping foot follow a straighter line rather than an arc).
  float SteppingGravityCompensation;  ///< Amount of gravity compensation on the stepping leg
  float SteppingLimbLengthToAbort;  ///< First condition for preventing or aborting a step - if the distance between the foot and leg root is less than this (e.g. if the character has fallen and the legs are compressed), in metres (standard character).
  float SteppingRootDownSpeedToAbort;  ///< Second condition for preventing or aborting a step - if the leg root is moving downwards with a speed greater than this, in m/s (standard character).
  float SteppingDirectionThreshold;  ///< Threshold for when the character should step toe or heel first. If 0 then it happens when stepping directly forwards. -1 results in stepping toe first even when stepping backwards.
  float SteppingImplicitStiffness;  ///< How much the character anticipates the ground when stepping. Smaller values result in the leg being weaker when it hits the ground (which can appear more natural)
  float ReadyPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  int32_t StepsBeforeLooking;  ///< Look in step direction after this many consecutive steps
  float StopLookingTime;  ///< Time over which to stop looking in the step direction, in seconds (standard character).
  float MinSpeedForLooking;  ///< Speed below which the character shouldn't look where they're stepping. In m/s (standard character).
  float WholeBodyLook;  ///< Set to 0 to only look with the head, or 1 to use as much of the body as possible
  float LookDownAngle;  ///< Angle (in degrees) to look down when stepping
  float LookWeight;  ///< 0- don't look where you step, 1- look with normal strength in the direction you are stepping
  float ReadyPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float FwdDistanceToTriggerStep;  ///< Step if a foot is this much ahead of where it should be, relative to the balance pose, in metres (standard character).
  float SidewaysDistanceToTriggerStep;  ///< Step if a foot is this much sideways relative to the balance pose, in metres (standard character).
  float TimeBeforeShiftingWeight;  ///< Time to wait before starting to shift the balance weight prior to a recovery step, in seconds (standard character).
  float WeightShiftingTime;  ///< Time to spend shifting the balance weight before stepping, in seconds (standard character).
  float MinPelvisPitch;  ///< Minimum pelvis pitch in degrees - positive values pitch up
  float MaxPelvisPitch;  ///< Maximum pelvis pitch in degrees - positive values pitch up
  float MinPelvisRoll;  ///< Minimum pelvis roll in degrees - positive values roll to the right
  float MaxPelvisRoll;  ///< Maximum pelvis roll in degrees - positive values roll to the right
  float MinPelvisYaw;  ///< Minimum pelvis yaw in degrees - positive values yaw to the left
  float MaxPelvisYaw;  ///< Maximum pelvis yaw in degrees - positive values yaw to the left
  float MinPelvisHeight;  ///< Minimum pelvis height above the feet, in metres (standard character).
  float MaxPelvisHeight;  ///< Maximum pelvis height above the feet, in metres (standard character).
  float PelvisPositionChangeTimescale;  ///< Timescale over which to smooth the pelvis target position, in seconds (standard character).
  float PelvisOrientationChangeTimescale;  ///< Timescale over which to smooth the pelvis target orientation, in seconds (standard character).
  float NonSupportingGravityCompensationScale;  ///< Gravity compensation on the non-supporting arm pose. Can be greater than 1 if you want full compensation on otherwise weak arms - in which case set it to one divided by the balance pose weight.
  float NonSupportingDampingScale;  ///< Damping scale on the non-supporting arm pose (1 means use default damping). Note that when using low strengths (in the BalancePose section) the damping will automatically be reduced, so this value may need to be rather high (e.g. 1/strength) in order to see significant damping.
  float NonSupportingDriveCompensationScale;  ///< Drive compensation on the non-supporting arm pose. A value of 1 should be used if you want these limbs to be strong and coordinated. However, if you want the unsupported limbs to be quite loose, then a value of zero is better, as that will prevent 'floaty' artefacts.
  float ArmDisplacementTime;  ///< How much to move the hands in the direction of the step when stepping, calculated by multiplying velocity of the character by this value. In seconds (standard character).
  float BalancePose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float MaxLinearAccelerationAssistance;  ///< Maximum linear acceleration provided by the assistance (disabled if zero) in m/s^2 (standard character).
  float MaxAngularAccelerationAssistance;  ///< Maximum linear acceleration provided by the assistance (disabled if zero) in deg/s^2 (standard character).
  float AssistanceChestToPelvisRatio;  ///< How to distribute the balance cheat forces. A value of 1 results in them all being applied to the chest. A value of 0 results in them all being applied to the pelvis.
  float FallenTimeToEmitRequests;  ///< Messages for fallen/recovered get triggered when the fallen time exceeds this, in seconds (standard character).
  float TargetYawRight;  ///< Ignore Desired Pelvis Direction input if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float TargetYawLeft;  ///< Ignore Desired Pelvis Direction input if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees
  float TargetPitchDown;  ///< Ignore Desired Pelvis Direction input if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float TargetPitchUp;  ///< Ignore Desired Pelvis Direction input if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float ReadyPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  float BalancePose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  bool SupportWithArms;  ///< Use the arms as well as the legs to stand and balance on. This is a possibility for balancing for quadrapeds whose forlegs are marked as arms.
  bool AllowArmSpin;  ///< Allow the character to or windmill its arms (when needed) in order to maintain balance. If a character is unable to step then windmilling its arms can be the only way it can maintain balance.
  bool EnableCollisionGroup;  ///< Use a collision group to avoid leg/ arm collision.  If enabled the collision group index must be provided. 
  bool StepWithLegs;  ///< Use the legs to step with
  bool StepWithArms;  ///< Use the arms to step with. This is a possibility for quadrapeds whose forlegs are marked as arms
  bool SuppressFootCrossing;  /// Attempt to prevent/reduce stepping through such that the step would cross the other stance feet
  
  bool EnableStand;  ///< Unused
  bool EnableLook;  ///< Allow the character to Look in the step direction.
  bool StepToRecoverPose;  ///< Enable/disable stepping to recover the stand pose
  bool EnablePoseClamping;  ///< Enable clamping of the pose - there is a small performance cost to doing so
  bool DynamicClamping;  ///< If set then the clamping is made more restrictive depending on the state of the character. This can help him keep his balance, but in some situations may cause him to oscillate to some degree.
  bool IgnoreDirectionWhenOutOfRange;  ///< Ignore desired pelvis direction input if it is outside of this range. The centre of the range points in the pelvis forward direction. Pitch and Yaw are in degrees.
  bool ReduceAssistanceWhenFalling;  ///< If true then the assistance amount will be reduced if the character looses balance, which means less assistance when more horizontal in angle. This prevents the character from appearing to magically stay up while leaning at an angle that should cause it to fall.
  bool UseSingleFrameForBalancePose;  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  
  bool UseSingleFrameForReadyPose;  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  
  bool EnableAssistance;  ///< Enable assistance forces.


public:

  BalanceBehaviourData()
  {
    EnableStand = true;
    SupportWithArms = false;
    FootBalanceAmount = float(481.191);
    DecelerationAmount = float(0.549265);
    MaxAngle = float(60);
    FootLength = float(0.392197);
    LowerPelvisDistanceWhenFootLifts = float(0.456781);
    AllowArmSpin = false;
    SpinAmount = float(1);
    SpinThreshold = float(2);
    EnableCollisionGroup = true;
    CollisionGroupIndex = -1;
    StepWithLegs = true;
    StepWithArms = false;
    SuppressSteppingTime = float(0);
    ArmSwingStrengthScaleWhenStepping = float(0.5);
    StepCountResetTime = float(1);
    SuppressFootCrossing = false;
    FootCrossingOffset = float(0.1);
    StepPredictionTimeForward = float(0.127242);
    StepPredictionTimeBackward = float(0.408668);
    StepUpDistance = float(0.70291);
    StepDownDistance = float(0.945113);
    MaxStepLength = float(2);
    StepDownSpeed = float(0.942423);
    FootSeparationFraction = float(0.8);
    AlignFootToFloorWeight = float(0);
    LowerPelvisAmount = float(0.354872);
    StrengthScale = float(1.6);
    DampingRatioScale = float(1.13472);
    SteppingOrientationWeight = float(0.475309);
    IKSubStep = float(0.5);
    SteppingGravityCompensation = float(0.042387);
    SteppingLimbLengthToAbort = float(0.7);
    SteppingRootDownSpeedToAbort = float(1);
    SteppingDirectionThreshold = float(-0.502405);
    SteppingImplicitStiffness = float(0);
    EnableLook = true;
    StepsBeforeLooking = 3;
    StopLookingTime = float(1);
    MinSpeedForLooking = float(0.3);
    WholeBodyLook = float(0);
    LookDownAngle = float(20);
    LookWeight = float(1);
    StepToRecoverPose = true;
    FwdDistanceToTriggerStep = float(0.15);
    SidewaysDistanceToTriggerStep = float(0.15);
    TimeBeforeShiftingWeight = float(0.2);
    WeightShiftingTime = float(0.5);
    EnablePoseClamping = true;
    DynamicClamping = false;
    MinPelvisPitch = float(-45);
    MaxPelvisPitch = float(25);
    MinPelvisRoll = float(-30);
    MaxPelvisRoll = float(30);
    MinPelvisYaw = float(-45);
    MaxPelvisYaw = float(45);
    MinPelvisHeight = float(0.4);
    MaxPelvisHeight = float(1);
    PelvisPositionChangeTimescale = float(0.3);
    PelvisOrientationChangeTimescale = float(0.4);
    UseSingleFrameForBalancePose = false;
    NonSupportingGravityCompensationScale = float(0);
    NonSupportingDampingScale = float(1);
    NonSupportingDriveCompensationScale = float(0.2);
    UseSingleFrameForReadyPose = false;
    ArmDisplacementTime = float(1);
    EnableAssistance = true;
    ReduceAssistanceWhenFalling = true;
    MaxLinearAccelerationAssistance = float(0);
    MaxAngularAccelerationAssistance = float(0);
    AssistanceChestToPelvisRatio = float(0.5);
    FallenTimeToEmitRequests = float(1);
    IgnoreDirectionWhenOutOfRange = false;
    TargetYawRight = float(-90);
    TargetYawLeft = float(90);
    TargetPitchDown = float(-90);
    TargetPitchUp = float(90);
    BalancePose_ApplyToArm_1 = float(0.3);
    BalancePose_ApplyToArm_2 = float(0.3);
    BalancePose_ApplyToHead_1 = float(1);
    BalancePose_ApplyToLeg_1 = float(1);
    BalancePose_ApplyToLeg_2 = float(1);
    BalancePose_ApplyToSpine_1 = float(1);
    ReadyPose_ApplyToArm_1 = float(1);
    ReadyPose_ApplyToArm_2 = float(1);
    ReadyPose_ApplyToHead_1 = float(0);
    ReadyPose_ApplyToLeg_1 = float(0);
    ReadyPose_ApplyToLeg_2 = float(0);
    ReadyPose_ApplyToSpine_1 = float(0);
    m_messages = uint32_t(0);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 72);
    NMP_ASSERT(data->m_ints->m_numValues == 18);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    EnableStand = (data->m_ints->m_values[0] != 0);
    SupportWithArms = (data->m_ints->m_values[1] != 0);
    FootBalanceAmount = data->m_floats->m_values[0];
    DecelerationAmount = data->m_floats->m_values[1];
    MaxAngle = data->m_floats->m_values[2];
    FootLength = data->m_floats->m_values[3];
    LowerPelvisDistanceWhenFootLifts = data->m_floats->m_values[4];
    AllowArmSpin = (data->m_ints->m_values[2] != 0);
    SpinAmount = data->m_floats->m_values[5];
    SpinThreshold = data->m_floats->m_values[6];
    EnableCollisionGroup = (data->m_ints->m_values[3] != 0);
    CollisionGroupIndex = data->m_ints->m_values[4];
    StepWithLegs = (data->m_ints->m_values[5] != 0);
    StepWithArms = (data->m_ints->m_values[6] != 0);
    SuppressSteppingTime = data->m_floats->m_values[7];
    ArmSwingStrengthScaleWhenStepping = data->m_floats->m_values[8];
    StepCountResetTime = data->m_floats->m_values[9];
    SuppressFootCrossing = (data->m_ints->m_values[7] != 0);
    FootCrossingOffset = data->m_floats->m_values[10];
    StepPredictionTimeForward = data->m_floats->m_values[11];
    StepPredictionTimeBackward = data->m_floats->m_values[12];
    StepUpDistance = data->m_floats->m_values[13];
    StepDownDistance = data->m_floats->m_values[14];
    MaxStepLength = data->m_floats->m_values[15];
    StepDownSpeed = data->m_floats->m_values[16];
    FootSeparationFraction = data->m_floats->m_values[17];
    AlignFootToFloorWeight = data->m_floats->m_values[18];
    LowerPelvisAmount = data->m_floats->m_values[19];
    StrengthScale = data->m_floats->m_values[20];
    DampingRatioScale = data->m_floats->m_values[21];
    SteppingOrientationWeight = data->m_floats->m_values[22];
    IKSubStep = data->m_floats->m_values[23];
    SteppingGravityCompensation = data->m_floats->m_values[24];
    SteppingLimbLengthToAbort = data->m_floats->m_values[25];
    SteppingRootDownSpeedToAbort = data->m_floats->m_values[26];
    SteppingDirectionThreshold = data->m_floats->m_values[27];
    SteppingImplicitStiffness = data->m_floats->m_values[28];
    EnableLook = (data->m_ints->m_values[8] != 0);
    StepsBeforeLooking = data->m_ints->m_values[9];
    StopLookingTime = data->m_floats->m_values[29];
    MinSpeedForLooking = data->m_floats->m_values[30];
    WholeBodyLook = data->m_floats->m_values[31];
    LookDownAngle = data->m_floats->m_values[32];
    LookWeight = data->m_floats->m_values[33];
    StepToRecoverPose = (data->m_ints->m_values[10] != 0);
    FwdDistanceToTriggerStep = data->m_floats->m_values[34];
    SidewaysDistanceToTriggerStep = data->m_floats->m_values[35];
    TimeBeforeShiftingWeight = data->m_floats->m_values[36];
    WeightShiftingTime = data->m_floats->m_values[37];
    EnablePoseClamping = (data->m_ints->m_values[11] != 0);
    DynamicClamping = (data->m_ints->m_values[12] != 0);
    MinPelvisPitch = data->m_floats->m_values[38];
    MaxPelvisPitch = data->m_floats->m_values[39];
    MinPelvisRoll = data->m_floats->m_values[40];
    MaxPelvisRoll = data->m_floats->m_values[41];
    MinPelvisYaw = data->m_floats->m_values[42];
    MaxPelvisYaw = data->m_floats->m_values[43];
    MinPelvisHeight = data->m_floats->m_values[44];
    MaxPelvisHeight = data->m_floats->m_values[45];
    PelvisPositionChangeTimescale = data->m_floats->m_values[46];
    PelvisOrientationChangeTimescale = data->m_floats->m_values[47];
    UseSingleFrameForBalancePose = (data->m_ints->m_values[13] != 0);
    NonSupportingGravityCompensationScale = data->m_floats->m_values[48];
    NonSupportingDampingScale = data->m_floats->m_values[49];
    NonSupportingDriveCompensationScale = data->m_floats->m_values[50];
    UseSingleFrameForReadyPose = (data->m_ints->m_values[14] != 0);
    ArmDisplacementTime = data->m_floats->m_values[51];
    EnableAssistance = (data->m_ints->m_values[15] != 0);
    ReduceAssistanceWhenFalling = (data->m_ints->m_values[16] != 0);
    MaxLinearAccelerationAssistance = data->m_floats->m_values[52];
    MaxAngularAccelerationAssistance = data->m_floats->m_values[53];
    AssistanceChestToPelvisRatio = data->m_floats->m_values[54];
    FallenTimeToEmitRequests = data->m_floats->m_values[55];
    IgnoreDirectionWhenOutOfRange = (data->m_ints->m_values[17] != 0);
    TargetYawRight = data->m_floats->m_values[56];
    TargetYawLeft = data->m_floats->m_values[57];
    TargetPitchDown = data->m_floats->m_values[58];
    TargetPitchUp = data->m_floats->m_values[59];
    BalancePose_ApplyToArm_1 = data->m_floats->m_values[60];
    BalancePose_ApplyToArm_2 = data->m_floats->m_values[61];
    BalancePose_ApplyToHead_1 = data->m_floats->m_values[62];
    BalancePose_ApplyToLeg_1 = data->m_floats->m_values[63];
    BalancePose_ApplyToLeg_2 = data->m_floats->m_values[64];
    BalancePose_ApplyToSpine_1 = data->m_floats->m_values[65];
    ReadyPose_ApplyToArm_1 = data->m_floats->m_values[66];
    ReadyPose_ApplyToArm_2 = data->m_floats->m_values[67];
    ReadyPose_ApplyToHead_1 = data->m_floats->m_values[68];
    ReadyPose_ApplyToLeg_1 = data->m_floats->m_values[69];
    ReadyPose_ApplyToLeg_2 = data->m_floats->m_values[70];
    ReadyPose_ApplyToSpine_1 = data->m_floats->m_values[71];

    pd_BalancePose.m_poseWeights[0] = BalancePose_ApplyToArm_1;
    pd_BalancePose.m_poseWeights[1] = BalancePose_ApplyToArm_2;
    pd_BalancePose.m_poseWeights[2] = BalancePose_ApplyToHead_1;
    pd_BalancePose.m_poseWeights[3] = BalancePose_ApplyToLeg_1;
    pd_BalancePose.m_poseWeights[4] = BalancePose_ApplyToLeg_2;
    pd_BalancePose.m_poseWeights[5] = BalancePose_ApplyToSpine_1;

    pd_ReadyPose.m_poseWeights[0] = ReadyPose_ApplyToArm_1;
    pd_ReadyPose.m_poseWeights[1] = ReadyPose_ApplyToArm_2;
    pd_ReadyPose.m_poseWeights[2] = ReadyPose_ApplyToHead_1;
    pd_ReadyPose.m_poseWeights[3] = ReadyPose_ApplyToLeg_1;
    pd_ReadyPose.m_poseWeights[4] = ReadyPose_ApplyToLeg_2;
    pd_ReadyPose.m_poseWeights[5] = ReadyPose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 14);
    NMP_ASSERT(data->m_ints->m_numValues == 3);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 16);

    m_cp.TargetVelocity.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.TargetVelocityInCharacterSpace = (data->m_ints->m_values[0] != 0);
    m_cp.BalanceFwdOffset = data->m_floats->m_values[0];
    m_cp.BalanceRightOffset = data->m_floats->m_values[1];
    m_cp.LegStepStrength[0] = data->m_floats->m_values[2];
    m_cp.LegStepStrength[1] = data->m_floats->m_values[3];
    m_cp.ArmStepStrength[0] = data->m_floats->m_values[4];
    m_cp.ArmStepStrength[1] = data->m_floats->m_values[5];
    m_cp.LookInStepDirection = (data->m_ints->m_values[1] != 0);
    m_cp.TargetPelvisDirection.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.BalanceWithAnimationPose = data->m_floats->m_values[6];
    m_cp.LegStandStrength[0] = data->m_floats->m_values[7];
    m_cp.LegStandStrength[1] = data->m_floats->m_values[8];
    m_cp.ArmStandStrength[0] = data->m_floats->m_values[9];
    m_cp.ArmStandStrength[1] = data->m_floats->m_values[10];
    m_cp.ExclusionZonePoint.set(data->m_vector3Data->m_values[8], data->m_vector3Data->m_values[9], data->m_vector3Data->m_values[10]);
    m_cp.ExclusionZoneDirection.set(data->m_vector3Data->m_values[12], data->m_vector3Data->m_values[13], data->m_vector3Data->m_values[14]);
    m_cp.OrientationAssistanceAmount = data->m_floats->m_values[11];
    m_cp.PositionAssistanceAmount = data->m_floats->m_values[12];
    m_cp.VelocityAssistanceAmount = data->m_floats->m_values[13];
    m_cp.UseCounterForceOnFeet = (data->m_ints->m_values[2] != 0);
  }


  /// Settings that govern the characters behaviour when standing (stationary balance)

  /// Unused
  bool getEnableStand() const { return EnableStand; }
  /// Use the arms as well as the legs to stand and balance on. This is a possibility for balancing for quadrapeds whose forlegs are marked as arms.
  bool getSupportWithArms() const { return SupportWithArms; }
  /// Amount to angle the feet to balance (this is the foot target angle in degrees when at the edge of balancing). Generally make it as large as possible, but if it is too large it will cause the feet to jitter.
  float getFootBalanceAmount() const { return FootBalanceAmount; }
  /// Amount that the character should lean away from movement in order to decelerate
  float getDecelerationAmount() const { return DecelerationAmount; }
  /// Maximum angle for the feet, in degrees
  float getMaxAngle() const { return MaxAngle; }
  /// Foot length in metres (standard character). Smaller values result in the character stepping earlier, which can prevent him teetering on the edge of static balance.
  float getFootLength() const { return FootLength; }
  /// Distance to lower the pelvis (i.e. crouch) when a supporting foot lifts off the floor. In metres (standard character).
  float getLowerPelvisDistanceWhenFootLifts() const { return LowerPelvisDistanceWhenFootLifts; }

  /// Settings that govern the tendency to spin arms to regain balance

  /// Allow the character to or windmill its arms (when needed) in order to maintain balance. If a character is unable to step then windmilling its arms can be the only way it can maintain balance.
  bool getAllowArmSpin() const { return AllowArmSpin; }
  /// Scales the amount and speed of arm spinning
  float getSpinAmount() const { return SpinAmount; }
  /// How off-balance the character needs to be to start spinning his arms
  float getSpinThreshold() const { return SpinThreshold; }

  /// Settings that govern the characters behaviour when stepping (to maintain balance)

  /// Use a collision group to avoid leg/ arm collision.  If enabled the collision group index must be provided. 
  bool getEnableCollisionGroup() const { return EnableCollisionGroup; }
  /// Collision group used to disable leg/arm collision when stepping. This is the name of the collision group within the collision groups editor. This name is stored as an index into the group as appears in the collision group index.
  int32_t getCollisionGroupIndex() const { return CollisionGroupIndex; }
  /// Use the legs to step with
  bool getStepWithLegs() const { return StepWithLegs; }
  /// Use the arms to step with. This is a possibility for quadrapeds whose forlegs are marked as arms
  bool getStepWithArms() const { return StepWithArms; }
  /// Prevent stepping for this time after enabling, in seconds (standard character).
  float getSuppressSteppingTime() const { return SuppressSteppingTime; }
  /// Scales the strength with which the character can swing its arms when stepping. 
  float getArmSwingStrengthScaleWhenStepping() const { return ArmSwingStrengthScaleWhenStepping; }
  /// The StepCount that is emitted as an output parameter is reset if this amount of time has passed from the last step. In seconds (standard character).
  float getStepCountResetTime() const { return StepCountResetTime; }
  /// Attempt to prevent/reduce stepping through such that the step would cross the other stance feet
  bool getSuppressFootCrossing() const { return SuppressFootCrossing; }
  /// Positive values will cut the step short before it would cross the other foot. Negative values will allow for more foot crossing. In metres (standard character).
  float getFootCrossingOffset() const { return FootCrossingOffset; }
  /// Time used to calculate the step length when stepping forwards, in seconds (standard character).
  float getStepPredictionTimeForward() const { return StepPredictionTimeForward; }
  /// Time used to calculate the step length when stepping backwards, in seconds (standard character).
  float getStepPredictionTimeBackward() const { return StepPredictionTimeBackward; }
  /// How high the steps should be, in metres (standard character). Higher values will tend to make the character stagger with high steps, so the foot will be unlikely to catch on the ground. Lower values may look more natural, at the risk of catching the foot on the ground.
  float getStepUpDistance() const { return StepUpDistance; }
  /// When the balancer wishes to end a step he aims this distance below the ground. In metres (standard character).
  float getStepDownDistance() const { return StepDownDistance; }
  /// Max step length, in metres (standard character).
  float getMaxStepLength() const { return MaxStepLength; }
  /// Speed to aim the foot down as the step time progresses, in m/s (standard character). This can help prevent the foot from hanging if for some reason it does not progress.
  float getStepDownSpeed() const { return StepDownSpeed; }
  /// Target foot separation as a fraction of the balance pose during the step.
  float getFootSeparationFraction() const { return FootSeparationFraction; }
  /// Amount to align the foot to the floor (rather than making it follow the natural swing of the leg) during the step.
  float getAlignFootToFloorWeight() const { return AlignFootToFloorWeight; }
  /// Amount to lower the pelvis when stepping as a fraction of the step length.
  float getLowerPelvisAmount() const { return LowerPelvisAmount; }
  /// Step strength multiplier, so larger values make the stepping leg stronger.
  float getStrengthScale() const { return StrengthScale; }
  /// Step damping ratio multiplier, so larger values make the stepping leg move slower but more stiffly.
  float getDampingRatioScale() const { return DampingRatioScale; }
  /// IK orientation weight.
  float getSteppingOrientationWeight() const { return SteppingOrientationWeight; }
  /// IK sub step (values < 1 make the stepping foot follow a straighter line rather than an arc).
  float getIKSubStep() const { return IKSubStep; }
  /// Amount of gravity compensation on the stepping leg
  float getSteppingGravityCompensation() const { return SteppingGravityCompensation; }
  /// First condition for preventing or aborting a step - if the distance between the foot and leg root is less than this (e.g. if the character has fallen and the legs are compressed), in metres (standard character).
  float getSteppingLimbLengthToAbort() const { return SteppingLimbLengthToAbort; }
  /// Second condition for preventing or aborting a step - if the leg root is moving downwards with a speed greater than this, in m/s (standard character).
  float getSteppingRootDownSpeedToAbort() const { return SteppingRootDownSpeedToAbort; }
  /// Threshold for when the character should step toe or heel first. If 0 then it happens when stepping directly forwards. -1 results in stepping toe first even when stepping backwards.
  float getSteppingDirectionThreshold() const { return SteppingDirectionThreshold; }
  /// How much the character anticipates the ground when stepping. Smaller values result in the leg being weaker when it hits the ground (which can appear more natural)
  float getSteppingImplicitStiffness() const { return SteppingImplicitStiffness; }

  /// Settings to make the character look where it is stepping.

  /// Allow the character to Look in the step direction.
  bool getEnableLook() const { return EnableLook; }
  /// Look in step direction after this many consecutive steps
  int32_t getStepsBeforeLooking() const { return StepsBeforeLooking; }
  /// Time over which to stop looking in the step direction, in seconds (standard character).
  float getStopLookingTime() const { return StopLookingTime; }
  /// Speed below which the character shouldn't look where they're stepping. In m/s (standard character).
  float getMinSpeedForLooking() const { return MinSpeedForLooking; }
  /// Set to 0 to only look with the head, or 1 to use as much of the body as possible
  float getWholeBodyLook() const { return WholeBodyLook; }
  /// Angle (in degrees) to look down when stepping
  float getLookDownAngle() const { return LookDownAngle; }
  /// 0- don't look where you step, 1- look with normal strength in the direction you are stepping
  float getLookWeight() const { return LookWeight; }

  /// These settings can ensure that the character will attempt to regain the stand pose when sufficiently displaced from that pose

  /// Enable/disable stepping to recover the stand pose
  bool getStepToRecoverPose() const { return StepToRecoverPose; }
  /// Step if a foot is this much ahead of where it should be, relative to the balance pose, in metres (standard character).
  float getFwdDistanceToTriggerStep() const { return FwdDistanceToTriggerStep; }
  /// Step if a foot is this much sideways relative to the balance pose, in metres (standard character).
  float getSidewaysDistanceToTriggerStep() const { return SidewaysDistanceToTriggerStep; }
  /// Time to wait before starting to shift the balance weight prior to a recovery step, in seconds (standard character).
  float getTimeBeforeShiftingWeight() const { return TimeBeforeShiftingWeight; }
  /// Time to spend shifting the balance weight before stepping, in seconds (standard character).
  float getWeightShiftingTime() const { return WeightShiftingTime; }

  /// These settings stabilise the character by restricting and smoothing the target balance pose, which may be modified by other behaviours (for example by BalancePoser).

  /// Enable clamping of the pose - there is a small performance cost to doing so
  bool getEnablePoseClamping() const { return EnablePoseClamping; }
  /// If set then the clamping is made more restrictive depending on the state of the character. This can help him keep his balance, but in some situations may cause him to oscillate to some degree.
  bool getDynamicClamping() const { return DynamicClamping; }
  /// Minimum pelvis pitch in degrees - positive values pitch up
  float getMinPelvisPitch() const { return MinPelvisPitch; }
  /// Maximum pelvis pitch in degrees - positive values pitch up
  float getMaxPelvisPitch() const { return MaxPelvisPitch; }
  /// Minimum pelvis roll in degrees - positive values roll to the right
  float getMinPelvisRoll() const { return MinPelvisRoll; }
  /// Maximum pelvis roll in degrees - positive values roll to the right
  float getMaxPelvisRoll() const { return MaxPelvisRoll; }
  /// Minimum pelvis yaw in degrees - positive values yaw to the left
  float getMinPelvisYaw() const { return MinPelvisYaw; }
  /// Maximum pelvis yaw in degrees - positive values yaw to the left
  float getMaxPelvisYaw() const { return MaxPelvisYaw; }
  /// Minimum pelvis height above the feet, in metres (standard character).
  float getMinPelvisHeight() const { return MinPelvisHeight; }
  /// Maximum pelvis height above the feet, in metres (standard character).
  float getMaxPelvisHeight() const { return MaxPelvisHeight; }
  /// Timescale over which to smooth the pelvis target position, in seconds (standard character).
  float getPelvisPositionChangeTimescale() const { return PelvisPositionChangeTimescale; }
  /// Timescale over which to smooth the pelvis target orientation, in seconds (standard character).
  float getPelvisOrientationChangeTimescale() const { return PelvisOrientationChangeTimescale; }

  /// The pose guide for the limbs when the character is in a stationary balance.

  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  bool getUseSingleFrameForBalancePose() const { return UseSingleFrameForBalancePose; }
  /// Gravity compensation on the non-supporting arm pose. Can be greater than 1 if you want full compensation on otherwise weak arms - in which case set it to one divided by the balance pose weight.
  float getNonSupportingGravityCompensationScale() const { return NonSupportingGravityCompensationScale; }
  /// Damping scale on the non-supporting arm pose (1 means use default damping). Note that when using low strengths (in the BalancePose section) the damping will automatically be reduced, so this value may need to be rather high (e.g. 1/strength) in order to see significant damping.
  float getNonSupportingDampingScale() const { return NonSupportingDampingScale; }
  /// Drive compensation on the non-supporting arm pose. A value of 1 should be used if you want these limbs to be strong and coordinated. However, if you want the unsupported limbs to be quite loose, then a value of zero is better, as that will prevent 'floaty' artefacts.
  float getNonSupportingDriveCompensationScale() const { return NonSupportingDriveCompensationScale; }

  /// The pose used to place the arms when the character is stepping. All other parts of the pose are ignored.

  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  bool getUseSingleFrameForReadyPose() const { return UseSingleFrameForReadyPose; }
  /// How much to move the hands in the direction of the step when stepping, calculated by multiplying velocity of the character by this value. In seconds (standard character).
  float getArmDisplacementTime() const { return ArmDisplacementTime; }

  /// Balance assistance provided through external forces/torques

  /// Enable assistance forces.
  bool getEnableAssistance() const { return EnableAssistance; }
  /// If true then the assistance amount will be reduced if the character looses balance, which means less assistance when more horizontal in angle. This prevents the character from appearing to magically stay up while leaning at an angle that should cause it to fall.
  bool getReduceAssistanceWhenFalling() const { return ReduceAssistanceWhenFalling; }
  /// Maximum linear acceleration provided by the assistance (disabled if zero) in m/s^2 (standard character).
  float getMaxLinearAccelerationAssistance() const { return MaxLinearAccelerationAssistance; }
  /// Maximum linear acceleration provided by the assistance (disabled if zero) in deg/s^2 (standard character).
  float getMaxAngularAccelerationAssistance() const { return MaxAngularAccelerationAssistance; }
  /// How to distribute the balance cheat forces. A value of 1 results in them all being applied to the chest. A value of 0 results in them all being applied to the pelvis.
  float getAssistanceChestToPelvisRatio() const { return AssistanceChestToPelvisRatio; }

  /// Messages for fallen/recovered get triggered when the fallen time exceeds this, in seconds (standard character).
  float getFallenTimeToEmitRequests() const { return FallenTimeToEmitRequests; }

  /// Ignore desired pelvis direction input if it is outside of this range. The centre of the range points in the pelvis forward direction. Pitch and Yaw are in degrees.
  bool getIgnoreDirectionWhenOutOfRange() const { return IgnoreDirectionWhenOutOfRange; }
  /// Ignore Desired Pelvis Direction input if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float getTargetYawRight() const { return TargetYawRight; }
  /// Ignore Desired Pelvis Direction input if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees
  float getTargetYawLeft() const { return TargetYawLeft; }
  /// Ignore Desired Pelvis Direction input if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float getTargetPitchDown() const { return TargetPitchDown; }
  /// Ignore Desired Pelvis Direction input if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float getTargetPitchUp() const { return TargetPitchUp; }


  /// The exclusion zone allows you to specify a region into which the charatcer will not step. This can be used to prevent him stepping off the edge of a cliff, for example.


  /// Target pose when balancing


  /// Pose used for the arms when stepping


  /// Target pose when balancing
  const BodyPoseData& getBalancePose() const { return pd_BalancePose; }
  /// Pose used for the arms when stepping
  const BodyPoseData& getReadyPose() const { return pd_ReadyPose; }

  // control parameters
  /// Creates a tendency to lean/stagger in a certain direction (will provoke stepping if enabled). Requires a non-zero velocity assistance. Specified in absolute units.
  const NMP::Vector3& getTargetVelocityCP() const { return m_cp.TargetVelocity; }
  /// If true then the target velocity is specified in character space with x forward, y up and z right. If false then it is specified in world space.
  bool getTargetVelocityInCharacterSpaceCP() const { return m_cp.TargetVelocityInCharacterSpace; }
  /// Sets a forwards offset for the pelvis position when balancing, in metres (standard character).
  float getBalanceFwdOffsetCP() const { return m_cp.BalanceFwdOffset; }
  /// Sets a rightwards offset for the pelvis position when balancing, in metres (standard character).
  float getBalanceRightOffsetCP() const { return m_cp.BalanceRightOffset; }
  /// Sets the stepping strength scale on the legs. Accepts values in the range 0 to 1.
  float getLegStepStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegStepStrength[index]; }
  enum { maxLegStepStrengthCP = 2 };
  /// Sets the stepping strength scale on the arms. Accepts values in the range 0 to 1.
  float getArmStepStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmStepStrength[index]; }
  enum { maxArmStepStrengthCP = 2 };
  /// Activates the tendency for the character to look in the direction it is stepping.
  bool getLookInStepDirectionCP() const { return m_cp.LookInStepDirection; }
  /// Target direction in world space for the pelvis. The character will try to align the pelvis forwards direction with this. The magnitude determines how much this is used, so 0 means it is ignored, and 1 means it overrides the normal direction, based on the character's current orientation.
  const NMP::Vector3& getTargetPelvisDirectionCP() const { return m_cp.TargetPelvisDirection; }
  /// If zero, then the balancer tries to place the centre of mass over the support area. If one then it tries to match the animation pose (which may not be intrinsically balanced). Intermediate values blend between these extremes.
  float getBalanceWithAnimationPoseCP() const { return m_cp.BalanceWithAnimationPose; }
  /// Sets the stand strength scale on the legs. Accepts values in the range 0 to 1.
  float getLegStandStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegStandStrength[index]; }
  enum { maxLegStandStrengthCP = 2 };
  /// Sets the stand strength scale on the arms. Accepts values in the range 0 to 1.
  float getArmStandStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmStandStrength[index]; }
  enum { maxArmStandStrengthCP = 2 };
  /// The exclusion zone allows you to specify a region into which the charatcer will not step. This can be used to prevent him stepping off the edge of a cliff, for example.

  /// Point on the plane defining the stepping exclusion zone, in absolute units
  const NMP::Vector3& getExclusionZonePointCP() const { return m_cp.ExclusionZonePoint; }
  /// Points into the exclusion zone. If the direction is zero then the exclusion zone is disabled.
  const NMP::Vector3& getExclusionZoneDirectionCP() const { return m_cp.ExclusionZoneDirection; }
  /// Sets the scale of cheat forces which can be used in keeping the pelvis orientated correctly for balance. A value of 1 results in a moderate amount of assistance.
  float getOrientationAssistanceAmountCP() const { return m_cp.OrientationAssistanceAmount; }
  /// Sets the scale of cheat forces which can be used in keeping the pelvis positioned correctly for balance. A value of 1 results in a moderate amount of assistance.
  float getPositionAssistanceAmountCP() const { return m_cp.PositionAssistanceAmount; }
  /// Sets the scale of cheat forces which can be used in damping the pelvis/chest velocity towards the target/desired velocity. A value of 1 results in a moderate amount of assistance.
  float getVelocityAssistanceAmountCP() const { return m_cp.VelocityAssistanceAmount; }
  /// Determines whether the stabilisation forces applied to the pelvis should be compensated for by an equal an opposite force on the feet. This helps preserve momentum when stepping on dynamic objects.
  bool getUseCounterForceOnFeetCP() const { return m_cp.UseCounterForceOnFeet; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// The time that the character has been balancing without stepping (will be zero if stepping or fallen). In absolute units.
  void setStandingStillTimeOCP(float value) { m_ocp.StandingStillTime = value; }
  /// The time that the character has been fallen, defined as when BalanceAmount is 0. In absolute units.
  void setFallenTimeOCP(float value) { m_ocp.FallenTime = value; }
  /// The time that the character has been touching something that appears to be ground with his legs/feet. In absolute units.
  void setOnGroundTimeOCP(float value) { m_ocp.OnGroundTime = value; }
  /// An indicator of how balanced the character is from 0 to 1. Will be close to 1 when balanced, and decrease to 0 if he is falling/lifted off the ground.
  void setBalanceAmountOCP(float value) { m_ocp.BalanceAmount = value; }
  /// Direction forwards relative to the character, projected onto the horizontal plane
  void setForwardsDirectionOCP(const NMP::Vector3&  value) { m_ocp.ForwardsDirection = value; }
  /// Direction sideways to the right of the character, projected onto the horizontal plane
  void setRightDirectionOCP(const NMP::Vector3&  value) { m_ocp.RightDirection = value; }
  /// Velocity of the balance support region
  void setSupportVelocityOCP(const NMP::Vector3&  value) { m_ocp.SupportVelocity = value; }
  /// The time that stepping has been active (will be reset after StepCountResetTime). In absolute units.
  void setSteppingTimeOCP(float value) { m_ocp.SteppingTime = value; }
  /// The total number of steps (will be reset after StepCountResetTime)
  void setStepCountOCP(int value) { m_ocp.StepCount = value; }
  /// The approximate fraction through the step so 0 means the leg is just about to leave the ground, 1 when it reaches its target and the step finishes.
  void setLegStepFractionsOCP(unsigned int index, float value) { NMP_ASSERT(index < 2); m_ocp.LegStepFractions[index] = value; }
  enum { maxLegStepFractionsOCP = 2 };
  void setMessage(uint32_t value) { m_messages |= value; }
  void resetMessages() { m_messages = 0; }

};


class BalanceBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Balance"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 6; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class BalanceBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    BalancePose_ID = 0,
    ReadyPose_ID = 1,
  };

  enum ControlParamIDs
  {
    TargetVelocity_CP_ID = 0,
    TargetVelocityInCharacterSpace_CP_ID = 1,
    BalanceFwdOffset_CP_ID = 2,
    BalanceRightOffset_CP_ID = 3,
    LegStepStrength_0_CP_ID = 4,
    LegStepStrength_1_CP_ID = 5,
    ArmStepStrength_0_CP_ID = 6,
    ArmStepStrength_1_CP_ID = 7,
    LookInStepDirection_CP_ID = 8,
    TargetPelvisDirection_CP_ID = 9,
    BalanceWithAnimationPose_CP_ID = 10,
    LegStandStrength_0_CP_ID = 11,
    LegStandStrength_1_CP_ID = 12,
    ArmStandStrength_0_CP_ID = 13,
    ArmStandStrength_1_CP_ID = 14,
    ExclusionZonePoint_CP_ID = 15,
    ExclusionZoneDirection_CP_ID = 16,
    OrientationAssistanceAmount_CP_ID = 17,
    PositionAssistanceAmount_CP_ID = 18,
    VelocityAssistanceAmount_CP_ID = 19,
    UseCounterForceOnFeet_CP_ID = 20,
  };

  enum OutputControlParamIDs
  {
    StandingStillTime_OCP_ID = 0,
    FallenTime_OCP_ID = 1,
    OnGroundTime_OCP_ID = 2,
    BalanceAmount_OCP_ID = 3,
    ForwardsDirection_OCP_ID = 4,
    RightDirection_OCP_ID = 5,
    SupportVelocity_OCP_ID = 6,
    SteppingTime_OCP_ID = 7,
    StepCount_OCP_ID = 8,
    LegStepFractions_0_OCP_ID = 9,
    LegStepFractions_1_OCP_ID = 10,
  };

  enum EmittedMessages
  {
    HasFallen = (1 << 0),
    HasRecoveredBalance = (1 << 1),
  };

  const BalanceBehaviourData& getParams() const { return m_params; }
  BalanceBehaviourData& getParams() { return m_params; }
  BalancePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  BalanceBehaviourData m_params;
  BalancePoseRequirements m_poseRequirements;
};

class BalanceBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BalanceBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline BalanceBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(BalanceBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) BalanceBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_BALANCE_H

