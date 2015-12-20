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

#ifndef NM_MDF_BDEF_AIM_H
#define NM_MDF_BDEF_AIM_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Aim.behaviour"

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

struct AimPoseRequirements
{
  bool enablePoseInput_Pose;

  AimPoseRequirements()
  {
    enablePoseInput_Pose = true;
  }
};

struct AimBehaviourData
{
  friend class AimBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 BarrelDirection;  /// The direction of the gun barrel (or whatever is being pointed) in the space of the aiming hand.
    
    NMP::Vector3 TargetPositionInWorldSpace;  /// Current target position in world space.
    
    float ArmStrength[NetworkConstants::networkMaxNumArms];  ///< Weights this behaviour's influence over the arm against that of other behaviours. Lower values will be weaker if no other arm control active
    float HeadStrength[NetworkConstants::networkMaxNumHeads];  ///< Weights this behaviour's influence over the head against that of other behaviours. Lower values will be weaker if no other head control active
    float SpineStrength[NetworkConstants::networkMaxNumSpines];  ///< Weights this behaviour's influence over the spine against that of other behaviours. Lower values will be weaker if no other spine control active
    float ArmDampingScale[NetworkConstants::networkMaxNumArms];  ///< Damping scale for the arm.
    float HeadDampingScale[NetworkConstants::networkMaxNumHeads];  ///< Damping scale for the head.
    float SpineDampingScale[NetworkConstants::networkMaxNumSpines];  ///< Damping scale for the spine.
    float ArmRootRotationCompensation[NetworkConstants::networkMaxNumArms];  ///< How much to adjust the arm target position to compensate for rotation of the chest.
    float HeadRootRotationCompensation[NetworkConstants::networkMaxNumHeads];  ///< How much to adjust the head target position to compensate for rotation of the chest.
    float SpineRootRotationCompensation[NetworkConstants::networkMaxNumSpines];  ///< How much to adjust the chest target position to compensate for rotation of the pelvis.
    float ArmSwivel[NetworkConstants::networkMaxNumArms];  ///< Positive is elbows out and up, negative is elbows in and down
    float ArmIKSubStep[NetworkConstants::networkMaxNumArms];  ///< IK sub step (values < 1 make a moving arm follow a straighter line rather than an arc).
    bool AllowConstraint;  /// Enable configured constraint between a non aiming effector and the aiming effector. This can be used to dynamically enable/disable the supporting arm constraint.
    
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    float targetPitch;  ///< Rotation from target position for pose to actual target position relative to chest perpendicular to spine direction.
    float targetYaw;  ///< Rotation from target position for pose to actual target position relative to chest about spine direction.
    float handSeparation;  ///< How far is the supporting hand from the aiming hand.
  };
  OutputControlParams m_ocp;

  BodyPoseData  pd_Pose;
  float DisableConstraintOnSeparationInPoseMax;  /// Deactivate the constraint if the constrained hand is more than this distance from the aiming hand in the input pose. In metres (standard character).
  
  float Pose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float Pose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float Pose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float Pose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float Pose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float Pose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  int32_t AimingLimbIndex;  /// Index of the arm that the character is aiming with (i.e. the arm holding the gun etc).
  
  float SwingBodyAmount;
  int32_t SupportingLimbIndex;  /// Select an arm whose hand will always be positioned relative to the aiming hand. This is useful for placing a hand on a rifle stock or representing a two handed grip on a pistol etc.
  
  float TwistHeadAmount;
  float DisableHandsSeparationInPoseMax;  /// Stop controlling the supporting hand if it is more than this distance from the aiming hand in the input pose. In metres (standard character).
  
  float SupportingArmTwist;  /// The ideal twist in the supporting arm. This is used to determine when the arm is twisted in an uncomfortable way. The behaviour will atempt to unwind the arm in this case. In degrees.
  
  float SwingHeadAmount;
  float ArmGravityCompensation[NetworkConstants::networkMaxNumArms];  ///< Gravity compensation scale for the arm.
  float ConstraintAccelerationLimit;  /// Maximum acceleration that the constraint can apply, scaled by the one over the separation between the aiming and constrained hands. In metres per second per second (standard character).
  
  float HeadGravityCompensation[NetworkConstants::networkMaxNumHeads];  ///< Gravity compensation scale for the head.
  float SpineGravityCompensation[NetworkConstants::networkMaxNumSpines];  ///< Gravity compensation scale for the spine.
  float DisableConstraintOnSeparationInRigMax;  /// Deactivate the constraint if the constrained hand is more than this distance from the aiming hand in the current rig configuration. In metres (standard character).
  
  float AimingLimbInertiaScale;  /// Scale the inertia of the aiming hand to reduce any movement caused by the constraint.
  
  float TargetYawRight;  ///< Don't aim if target yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float TargetYawLeft;  ///< Don't aim if target yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float TargetPitchDown;  ///< Don't aim if target pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float TargetPitchUp;  ///< Don't aim if target pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float TwistBodyAmount;
  bool EnableSupportingArm;  /// Turn on or off the ability to have an arm that supports the aiming arm
  
  bool ShouldDisableHandsOnSeparationInPose;  /// Stop controlling the supporting hand if it is more than a threshold distance from the aiming hand in the input pose.
  
  bool EnableConstraint;  /// Create a physical constraint between the supporting hand and the aiming hand. Constraint tries to maintain the relative transforms of the end parts in the input pose. 
  
  bool EnableOrientationConstraint;  /// Constrain the orientation of the supporting hand as well as the position.
  
  bool ShouldDisableConstraintOnSeparationInPose;  /// Deactivate the constraint if the constrained hand is more than a threshold distance from the aiming hand in the input pose.
  
  bool DisableWhenLyingOnFront;  ///< Don't Aim if the character is lying with its front facing the ground.
  bool DisableWhenTargetOutsideRange;  ///< Don't Aim if the direction from the pelvis to the target is outside of this range. The center of the range points in the pelvis forward direction. Pitch and Yaw are in degrees.
  bool ShouldDisableConstraintOnSeparationInRig;  /// Deactivate the constraint if the constrained hand is more than a threshold distance from the aiming hand in the current rig configuration.
  
  bool DisableWhenLyingOnGround;  ///< Don't Aim if the character is lying on the ground in any orientation.
  bool UseSingleFrameForPose;  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  


public:

  AimBehaviourData()
  {
    UseSingleFrameForPose = false;
    Pose_ApplyToArm_1 = float(1);
    Pose_ApplyToArm_2 = float(1);
    Pose_ApplyToHead_1 = float(1);
    Pose_ApplyToLeg_1 = float(1);
    Pose_ApplyToLeg_2 = float(1);
    Pose_ApplyToSpine_1 = float(1);
    AimingLimbIndex = 0;
    EnableSupportingArm = true;
    SupportingLimbIndex = 1;
    ShouldDisableHandsOnSeparationInPose = false;
    DisableHandsSeparationInPoseMax = float(0.1);
    SupportingArmTwist = float(-115);
    EnableConstraint = true;
    EnableOrientationConstraint = false;
    ConstraintAccelerationLimit = float(1);
    ShouldDisableConstraintOnSeparationInPose = true;
    DisableConstraintOnSeparationInPoseMax = float(0.2);
    ShouldDisableConstraintOnSeparationInRig = true;
    DisableConstraintOnSeparationInRigMax = float(0.2);
    AimingLimbInertiaScale = float(10);
    DisableWhenLyingOnGround = false;
    DisableWhenLyingOnFront = false;
    DisableWhenTargetOutsideRange = false;
    TargetYawRight = float(-90);
    TargetYawLeft = float(90);
    TargetPitchDown = float(-90);
    TargetPitchUp = float(90);
    TwistBodyAmount = float(0.75);
    SwingBodyAmount = float(0);
    TwistHeadAmount = float(1);
    SwingHeadAmount = float(1);
    ArmGravityCompensation[0] = float(1);
    ArmGravityCompensation[1] = float(1);
    HeadGravityCompensation[0] = float(1);
    SpineGravityCompensation[0] = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 24);
    NMP_ASSERT(data->m_ints->m_numValues == 12);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    UseSingleFrameForPose = (data->m_ints->m_values[0] != 0);
    Pose_ApplyToArm_1 = data->m_floats->m_values[0];
    Pose_ApplyToArm_2 = data->m_floats->m_values[1];
    Pose_ApplyToHead_1 = data->m_floats->m_values[2];
    Pose_ApplyToLeg_1 = data->m_floats->m_values[3];
    Pose_ApplyToLeg_2 = data->m_floats->m_values[4];
    Pose_ApplyToSpine_1 = data->m_floats->m_values[5];
    AimingLimbIndex = data->m_ints->m_values[1];
    EnableSupportingArm = (data->m_ints->m_values[2] != 0);
    SupportingLimbIndex = data->m_ints->m_values[3];
    ShouldDisableHandsOnSeparationInPose = (data->m_ints->m_values[4] != 0);
    DisableHandsSeparationInPoseMax = data->m_floats->m_values[6];
    SupportingArmTwist = data->m_floats->m_values[7];
    EnableConstraint = (data->m_ints->m_values[5] != 0);
    EnableOrientationConstraint = (data->m_ints->m_values[6] != 0);
    ConstraintAccelerationLimit = data->m_floats->m_values[8];
    ShouldDisableConstraintOnSeparationInPose = (data->m_ints->m_values[7] != 0);
    DisableConstraintOnSeparationInPoseMax = data->m_floats->m_values[9];
    ShouldDisableConstraintOnSeparationInRig = (data->m_ints->m_values[8] != 0);
    DisableConstraintOnSeparationInRigMax = data->m_floats->m_values[10];
    AimingLimbInertiaScale = data->m_floats->m_values[11];
    DisableWhenLyingOnGround = (data->m_ints->m_values[9] != 0);
    DisableWhenLyingOnFront = (data->m_ints->m_values[10] != 0);
    DisableWhenTargetOutsideRange = (data->m_ints->m_values[11] != 0);
    TargetYawRight = data->m_floats->m_values[12];
    TargetYawLeft = data->m_floats->m_values[13];
    TargetPitchDown = data->m_floats->m_values[14];
    TargetPitchUp = data->m_floats->m_values[15];
    TwistBodyAmount = data->m_floats->m_values[16];
    SwingBodyAmount = data->m_floats->m_values[17];
    TwistHeadAmount = data->m_floats->m_values[18];
    SwingHeadAmount = data->m_floats->m_values[19];
    ArmGravityCompensation[0] = data->m_floats->m_values[20];
    ArmGravityCompensation[1] = data->m_floats->m_values[21];
    HeadGravityCompensation[0] = data->m_floats->m_values[22];
    SpineGravityCompensation[0] = data->m_floats->m_values[23];

    pd_Pose.m_poseWeights[0] = Pose_ApplyToArm_1;
    pd_Pose.m_poseWeights[1] = Pose_ApplyToArm_2;
    pd_Pose.m_poseWeights[2] = Pose_ApplyToHead_1;
    pd_Pose.m_poseWeights[3] = Pose_ApplyToLeg_1;
    pd_Pose.m_poseWeights[4] = Pose_ApplyToLeg_2;
    pd_Pose.m_poseWeights[5] = Pose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 16);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 8);

    m_cp.ArmStrength[0] = data->m_floats->m_values[0];
    m_cp.ArmStrength[1] = data->m_floats->m_values[1];
    m_cp.HeadStrength[0] = data->m_floats->m_values[2];
    m_cp.SpineStrength[0] = data->m_floats->m_values[3];
    m_cp.ArmDampingScale[0] = data->m_floats->m_values[4];
    m_cp.ArmDampingScale[1] = data->m_floats->m_values[5];
    m_cp.HeadDampingScale[0] = data->m_floats->m_values[6];
    m_cp.SpineDampingScale[0] = data->m_floats->m_values[7];
    m_cp.ArmRootRotationCompensation[0] = data->m_floats->m_values[8];
    m_cp.ArmRootRotationCompensation[1] = data->m_floats->m_values[9];
    m_cp.HeadRootRotationCompensation[0] = data->m_floats->m_values[10];
    m_cp.SpineRootRotationCompensation[0] = data->m_floats->m_values[11];
    m_cp.ArmSwivel[0] = data->m_floats->m_values[12];
    m_cp.ArmSwivel[1] = data->m_floats->m_values[13];
    m_cp.ArmIKSubStep[0] = data->m_floats->m_values[14];
    m_cp.ArmIKSubStep[1] = data->m_floats->m_values[15];
    m_cp.BarrelDirection.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.TargetPositionInWorldSpace.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.AllowConstraint = (data->m_ints->m_values[0] != 0);
  }


  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  bool getUseSingleFrameForPose() const { return UseSingleFrameForPose; }

  /// Configure the arm that the character is aiming with.

  /// Index of the arm that the character is aiming with (i.e. the arm holding the gun etc).
  int32_t getAimingLimbIndex() const { return AimingLimbIndex; }

  /// Enables special treatment of one of the character's arms to make it look more like it is supporting the aiming arm. This includes calculating its IK target relative to the aiming arm, potentially enabling a physical constraint, controlling the path taken by the hand as it moves toward the aiming hand etc.

  /// Turn on or off the ability to have an arm that supports the aiming arm
  bool getEnableSupportingArm() const { return EnableSupportingArm; }
  /// Select an arm whose hand will always be positioned relative to the aiming hand. This is useful for placing a hand on a rifle stock or representing a two handed grip on a pistol etc.
  int32_t getSupportingLimbIndex() const { return SupportingLimbIndex; }
  /// Stop controlling the supporting hand if it is more than a threshold distance from the aiming hand in the input pose.
  bool getShouldDisableHandsOnSeparationInPose() const { return ShouldDisableHandsOnSeparationInPose; }
  /// Stop controlling the supporting hand if it is more than this distance from the aiming hand in the input pose. In metres (standard character).
  float getDisableHandsSeparationInPoseMax() const { return DisableHandsSeparationInPoseMax; }
  /// The ideal twist in the supporting arm. This is used to determine when the arm is twisted in an uncomfortable way. The behaviour will atempt to unwind the arm in this case. In degrees.
  float getSupportingArmTwist() const { return SupportingArmTwist; }
  /// Create a physical constraint between the supporting hand and the aiming hand. Constraint tries to maintain the relative transforms of the end parts in the input pose. 
  bool getEnableConstraint() const { return EnableConstraint; }
  /// Constrain the orientation of the supporting hand as well as the position.
  bool getEnableOrientationConstraint() const { return EnableOrientationConstraint; }
  /// Maximum acceleration that the constraint can apply, scaled by the one over the separation between the aiming and constrained hands. In metres per second per second (standard character).
  float getConstraintAccelerationLimit() const { return ConstraintAccelerationLimit; }
  /// Deactivate the constraint if the constrained hand is more than a threshold distance from the aiming hand in the input pose.
  bool getShouldDisableConstraintOnSeparationInPose() const { return ShouldDisableConstraintOnSeparationInPose; }
  /// Deactivate the constraint if the constrained hand is more than this distance from the aiming hand in the input pose. In metres (standard character).
  float getDisableConstraintOnSeparationInPoseMax() const { return DisableConstraintOnSeparationInPoseMax; }
  /// Deactivate the constraint if the constrained hand is more than a threshold distance from the aiming hand in the current rig configuration.
  bool getShouldDisableConstraintOnSeparationInRig() const { return ShouldDisableConstraintOnSeparationInRig; }
  /// Deactivate the constraint if the constrained hand is more than this distance from the aiming hand in the current rig configuration. In metres (standard character).
  float getDisableConstraintOnSeparationInRigMax() const { return DisableConstraintOnSeparationInRigMax; }
  /// Scale the inertia of the aiming hand to reduce any movement caused by the constraint.
  float getAimingLimbInertiaScale() const { return AimingLimbInertiaScale; }

  /// When to switch off the aiming behaviour.

  /// Don't Aim if the character is lying on the ground in any orientation.
  bool getDisableWhenLyingOnGround() const { return DisableWhenLyingOnGround; }
  /// Don't Aim if the character is lying with its front facing the ground.
  bool getDisableWhenLyingOnFront() const { return DisableWhenLyingOnFront; }
  /// Don't Aim if the direction from the pelvis to the target is outside of this range. The center of the range points in the pelvis forward direction. Pitch and Yaw are in degrees.
  bool getDisableWhenTargetOutsideRange() const { return DisableWhenTargetOutsideRange; }
  /// Don't aim if target yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float getTargetYawRight() const { return TargetYawRight; }
  /// Don't aim if target yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float getTargetYawLeft() const { return TargetYawLeft; }
  /// Don't aim if target pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float getTargetPitchDown() const { return TargetPitchDown; }
  /// Don't aim if target pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float getTargetPitchUp() const { return TargetPitchUp; }

  /// How much should the aim behaviour change the input pose for each limb.

  float getTwistBodyAmount() const { return TwistBodyAmount; }
  float getSwingBodyAmount() const { return SwingBodyAmount; }
  float getTwistHeadAmount() const { return TwistHeadAmount; }
  float getSwingHeadAmount() const { return SwingHeadAmount; }

  /// Set the amount of gravity compensation applied to each limb. Low values will allow the limbs to drop under the influence of gravity, which may make the character look tierd or weak. Values higher than one may be needed to get the appearence of full gravity compensation on otherwise weak limbs.

  /// Gravity compensation scale for the arm.
  float getArmGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 2); return ArmGravityCompensation[index]; }
  enum { maxArmGravityCompensation = 2 };
  /// Gravity compensation scale for the head.
  float getHeadGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 1); return HeadGravityCompensation[index]; }
  enum { maxHeadGravityCompensation = 1 };
  /// Gravity compensation scale for the spine.
  float getSpineGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 1); return SpineGravityCompensation[index]; }
  enum { maxSpineGravityCompensation = 1 };

  /// Set the strength and damping scale applied to each limb.


  /// Set the IK swivel parameters for each arm.



  const BodyPoseData& getPose() const { return pd_Pose; }

  // control parameters
  /// Set the strength and damping scale applied to each limb.

  /// Weights this behaviour's influence over the arm against that of other behaviours. Lower values will be weaker if no other arm control active
  float getArmStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmStrength[index]; }
  enum { maxArmStrengthCP = 2 };
  /// Weights this behaviour's influence over the head against that of other behaviours. Lower values will be weaker if no other head control active
  float getHeadStrengthCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadStrength[index]; }
  enum { maxHeadStrengthCP = 1 };
  /// Weights this behaviour's influence over the spine against that of other behaviours. Lower values will be weaker if no other spine control active
  float getSpineStrengthCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineStrength[index]; }
  enum { maxSpineStrengthCP = 1 };
  /// Damping scale for the arm.
  float getArmDampingScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmDampingScale[index]; }
  enum { maxArmDampingScaleCP = 2 };
  /// Damping scale for the head.
  float getHeadDampingScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadDampingScale[index]; }
  enum { maxHeadDampingScaleCP = 1 };
  /// Damping scale for the spine.
  float getSpineDampingScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineDampingScale[index]; }
  enum { maxSpineDampingScaleCP = 1 };
  /// How much to adjust the arm target position to compensate for rotation of the chest.
  float getArmRootRotationCompensationCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmRootRotationCompensation[index]; }
  enum { maxArmRootRotationCompensationCP = 2 };
  /// How much to adjust the head target position to compensate for rotation of the chest.
  float getHeadRootRotationCompensationCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadRootRotationCompensation[index]; }
  enum { maxHeadRootRotationCompensationCP = 1 };
  /// How much to adjust the chest target position to compensate for rotation of the pelvis.
  float getSpineRootRotationCompensationCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineRootRotationCompensation[index]; }
  enum { maxSpineRootRotationCompensationCP = 1 };
  /// Set the IK swivel parameters for each arm.

  /// Positive is elbows out and up, negative is elbows in and down
  float getArmSwivelCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmSwivel[index]; }
  enum { maxArmSwivelCP = 2 };
  /// IK sub step (values < 1 make a moving arm follow a straighter line rather than an arc).
  float getArmIKSubStepCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmIKSubStep[index]; }
  enum { maxArmIKSubStepCP = 2 };
  /// The direction of the gun barrel (or whatever is being pointed) in the space of the aiming hand.
  const NMP::Vector3& getBarrelDirectionCP() const { return m_cp.BarrelDirection; }
  /// Current target position in world space.
  const NMP::Vector3& getTargetPositionInWorldSpaceCP() const { return m_cp.TargetPositionInWorldSpace; }
  /// Enable configured constraint between a non aiming effector and the aiming effector. This can be used to dynamically enable/disable the supporting arm constraint.
  bool getAllowConstraintCP() const { return m_cp.AllowConstraint; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// Rotation from target position for pose to actual target position relative to chest perpendicular to spine direction.
  void setTargetPitchOCP(float value) { m_ocp.targetPitch = value; }
  /// Rotation from target position for pose to actual target position relative to chest about spine direction.
  void setTargetYawOCP(float value) { m_ocp.targetYaw = value; }
  /// How far is the supporting hand from the aiming hand.
  void setHandSeparationOCP(float value) { m_ocp.handSeparation = value; }
};


class AimBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Aim"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 1; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class AimBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    Pose_ID = 0,
  };

  enum ControlParamIDs
  {
    ArmStrength_0_CP_ID = 0,
    ArmStrength_1_CP_ID = 1,
    HeadStrength_0_CP_ID = 2,
    SpineStrength_0_CP_ID = 3,
    ArmDampingScale_0_CP_ID = 4,
    ArmDampingScale_1_CP_ID = 5,
    HeadDampingScale_0_CP_ID = 6,
    SpineDampingScale_0_CP_ID = 7,
    ArmRootRotationCompensation_0_CP_ID = 8,
    ArmRootRotationCompensation_1_CP_ID = 9,
    HeadRootRotationCompensation_0_CP_ID = 10,
    SpineRootRotationCompensation_0_CP_ID = 11,
    ArmSwivel_0_CP_ID = 12,
    ArmSwivel_1_CP_ID = 13,
    ArmIKSubStep_0_CP_ID = 14,
    ArmIKSubStep_1_CP_ID = 15,
    BarrelDirection_CP_ID = 16,
    TargetPositionInWorldSpace_CP_ID = 17,
    AllowConstraint_CP_ID = 18,
  };

  enum OutputControlParamIDs
  {
    targetPitch_OCP_ID = 0,
    targetYaw_OCP_ID = 1,
    handSeparation_OCP_ID = 2,
  };

  const AimBehaviourData& getParams() const { return m_params; }
  AimBehaviourData& getParams() { return m_params; }
  AimPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  AimBehaviourData m_params;
  AimPoseRequirements m_poseRequirements;
};

class AimBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(AimBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline AimBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(AimBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) AimBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_AIM_H

