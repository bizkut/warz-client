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

#ifndef NM_MDF_TYPE_LIMBCONTROL_H
#define NM_MDF_TYPE_LIMBCONTROL_H

// include definition file to create project dependency
#include "./Definition/Types/Limb.types"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"
#include "Euphoria/erDebugDraw.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace ER { class LimbControlAmounts; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Limb.types'
// Data Payload: 384 Bytes
// Alignment: 16

/// Class for controlling the rig-independent limb structures 
/// This class has a number of idiosyncrasies, which will hopefully disappear eventually, but for now,
/// they are very important to pay attention to:
/// 1. the rootTarget and velocities only work if you have an end target or orientation set (with a weight>0)
/// 2. The end velocity can be slowed down by rotational damping (which exists even if the orientation weight is 0) if it requires rotation of the end effector
/// 3. The end angular velocity can be slowed down by linear damping (which exists even if the position weight is 0) if it requires movement of the end effector
/// 4. The VM provides no damping in the normal axis when you specify a normal for the control
/// Lastly, there is a weighting system employed in this class so that (for example)
/// if you combine an unimportant position (with a weight of 0), and a valid position (with a weight of 1),
/// you don't get a random position with a weight of half, instead you get the valid position with weight of half.

struct LimbControl
{


  // functions

  LimbControl(float stiffness, float dampingRatio, float driveCompensation);
  void setTargetPos(const NMP::Vector3& targetPos, float positionWeight, const NMP::Vector3& velocityOfTargetPos=NMP::Vector3(0,0,0));
  void setTargetNormal(const NMP::Vector3& targetNormal, float normalWeight, const NMP::Vector3& angularVelocityOfTargetNormal=NMP::Vector3(0,0,0), const NMP::Vector3& localNormalVector=NMP::Vector3(1,0,0));
  /// Note that only the orientation part of targetorientation is used.
  void setTargetOrientation(const NMP::Matrix34& targetOrientation, float orientWeight, const NMP::Vector3& angularVelocityOfTargetOrientation=NMP::Vector3(0,0,0));
  void setTargetOrientation(const NMP::Quat& targetOrientation, float orientWeight, const NMP::Vector3& angularVelocityOfTargetOrientation=NMP::Vector3(0,0,0));
  void setExpectedRootPosForTarget(const NMP::Vector3& rootTargetPos, float rootPositionWeight, const NMP::Vector3& velocityOfRootTarget=NMP::Vector3(0,0,0));
  void setExpectedRootOrientationForTarget(const NMP::Quat& rootTargetOrientation, float rootOrientationWeight, const NMP::Vector3& angularVelocityOfRootTarget=NMP::Vector3(0,0,0));
  void setExpectedRootForTarget(const NMP::Matrix34& rootTarget, float rootTargetWeight, const NMP::Vector3& velocityOfRootTarget=NMP::Vector3(0,0,0), const NMP::Vector3& angularVelocityOfRootTarget=NMP::Vector3(0,0,0));
  void setEndSupportAmount(float endSupportAmount);
  void setRootSupportedAmount(float rootSupportedAmount);
  /// Implicit stiffness is in 0 to 1 range, 1 is normal/correct implicit stiffness, 0 means there is no stiffness from the internal drive strength/damping on external impacts
  void setImplicitStiffness(float implicitStiffness);
  /// This sets the skin width of all the parts in the limb to the original authored value plus skinWidthIncrease
  void setSkinWidthIncrease(float skinWidthIncrease);
  /// This scales the gravity compensation clamp
  void setGravityCompensation(float gravityCompensation);
  /// This sets the amount by which the strength should be increased when the limb is marked as supporting. With a value
  /// of 1 (the default), then the strength is increased so that the root will have the same acceleration control as the
  /// end would normally have. With a value of 0 then the strength is not modified.
  void setSupportForceMultiplier(float supportForceMultiplier);
  void setSoftLimitStiffnessScale(float softLimitStiffnessScale);
  void setSwivelAmount(float swivelAmount);
  void setDamping(float damping);
  void setEndFrictionScale(float endFrictionScale);
  /// Small values will fully control the limb. 1.0 will result in zero strength at the last joint. 
  /// Larger values will confine strength to just the rootward joints.
  void setStrengthReductionTowardsEnd(float reduction);
  void setUseIncrementalIK(bool useIncrementalIK);
  NMP::Vector3 getTargetPos() const;
  NMP::Matrix34 getExpectedRootForTarget() const;
  NMP::Vector3 getExpectedRootPosForTarget() const;
  NMP::Quat getExpectedRootOrientationForTarget() const;
  NMP::Quat getTargetOrientation() const;
  NMP::Vector3 getTargetNormal() const;
  NMP::Vector3 getLocalNormalVector() const;
  NMP::Vector3 getTargetsVelocity() const;
  NMP::Vector3 getTargetsAngularVelocity() const;
  NMP::Vector3 getRootsVelocity() const;
  NMP::Vector3 getRootsAngularVelocity() const;
  float getPositionWeight() const;
  float getRootPositionWeight() const;
  float getRootOrientationWeight() const;
  float getNormalWeight() const;
  float getOrientationWeight() const;
  float getStiffness() const;
  float getDampingRatio() const;
  float getStrength() const;
  float getDamping() const;
  float getDriveCompensation() const;
  float getEndSupportAmount() const;
  float getRootSupportedAmount() const;
  bool getStrongFrictionDisabled() const;
  float getEndFrictionScale() const;
  float getImplicitStiffness() const;
  float getSkinWidthIncrease() const;
  float getGravityCompensation() const;
  float getSupportForceMultiplier() const;
  float getSoftLimitStiffnessScale() const;
  float getSwivelAmount() const;
  float getStrengthReductionTowardsEnd() const;
  bool getUseIncrementalIK() const;
  void setTargetDeltaStep(float w);
  float getTargetDeltaStep() const;
  void setCollisionGroupIndex(float index);
  float getCollisionGroupIndex() const;
  void setControlAmount(uint32_t controlType, float amount);
  const ER::LimbControlAmounts* getControlAmounts() const;
  void reinit(float stiffness, float dampingRatio, float driveCompensation);

private:


  NMP::Vector3 m_weightedRootTargetPosition;  ///< weighted by m_weightedRootPositionWeight

  NMP::Quat m_weightedRootTargetOrientation;  ///< weighted by m_weightedRootOrientationWeight

  NMP::Quat m_weightedTargetOrientation;  ///< weighted by m_weightedOrientationWeight

  NMP::Vector3 m_weightedTargetPosition;  ///< weighted by m_weightedPositionWeight

  NMP::Vector3 m_weightedTargetNormal;  ///< weighted by m_weightedNormalWeight

  NMP::Vector3 m_weightedLocalNormal;  ///< weighted by m_weightedNormalWeight

  NMP::Vector3 m_weightedRootTargetVelocity;  ///< weighted by m_weightedRootPositionWeight

  NMP::Vector3 m_weightedRootTargetAngularVelocity;  ///< weighted by m_weightedRootOrientationWeight

  NMP::Vector3 m_weightedTargetVelocity;  ///< weighted by m_weightedPositionWeight

  NMP::Vector3 m_weightedTargetAngularVelocity;  ///< weighted by m_weightedNormalWeight or m_weightedOrientationWeight depending  

#if defined(MR_OUTPUT_DEBUGGING)
  ER::LimbControlAmounts m_debugControlAmounts;  ///< (LimbDebugControlAmounts)
#endif // MR_OUTPUT_DEBUGGING

  float m_strength;  // Values weighted by the importance only  (StrengthValue)

  float m_damping;                           ///< (DampingValue)

  float m_driveCompensation;  ///< we store the square value so add/merges act like they do on stiffness  (Compensation)

  float m_endSupportAmount;                  ///< (Multiplier)

  float m_rootSupportedAmount;               ///< (Multiplier)

  float m_implicitStiffness;                 ///< (Multiplier)

  float m_endFrictionScale;                  ///< (Multiplier)

  float m_skinWidthIncrease;  ///< Amount the skin width should be greater than the authored value  (Length)

  float m_strengthReductionTowardsEnd;       ///< (Weight)

  float m_targetDeltaStep;  // AdvancedIK stepsize between 0 and 1  (Weight)

  float m_useIncrementalIK;  // when this value is 0 incremental IK is disabled and the IK solution is reset to the guide pose before each solve.  (Weight)

  float m_swivelAmount;  // Guide pose system swivel weight between -1 and 1  (Multiplier)

  float m_collisionGroupIndex;  // The collision set to enable - note that this will only be valid if the importance of the result
  // is 1 - i.e. it came from a single output. Float so that it goes through the blending junctions.

  float m_weightedRootPositionWeight;  // Singly weighted values - weighted by m_strength  (Weight)

  float m_weightedRootOrientationWeight;     ///< (Weight)

  float m_weightedPositionWeight;            ///< (Weight)

  float m_weightedNormalWeight;              ///< (Weight)

  float m_weightedOrientationWeight;         ///< (Weight)

  float m_weightedGravityCompensation;       ///< (Multiplier)

  float m_weightedSupportForceMultipler;     ///< (Multiplier)

  float m_softLimitStiffnessScale;           ///< (Multiplier)


  // functions

  LimbControl();

public:


  NM_INLINE void operator *= (const float fVal)
  {
    m_weightedRootTargetPosition *= fVal;
    m_weightedRootTargetOrientation *= fVal;
    m_weightedTargetOrientation *= fVal;
    m_weightedTargetPosition *= fVal;
    m_weightedTargetNormal *= fVal;
    m_weightedLocalNormal *= fVal;
    m_weightedRootTargetVelocity *= fVal;
    m_weightedRootTargetAngularVelocity *= fVal;
    m_weightedTargetVelocity *= fVal;
    m_weightedTargetAngularVelocity *= fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    m_debugControlAmounts *= fVal;
#endif // MR_OUTPUT_DEBUGGING
    m_strength *= fVal;
    m_damping *= fVal;
    m_driveCompensation *= fVal;
    m_endSupportAmount *= fVal;
    m_rootSupportedAmount *= fVal;
    m_implicitStiffness *= fVal;
    m_endFrictionScale *= fVal;
    m_skinWidthIncrease *= fVal;
    m_strengthReductionTowardsEnd *= fVal;
    m_targetDeltaStep *= fVal;
    m_useIncrementalIK *= fVal;
    m_swivelAmount *= fVal;
    m_collisionGroupIndex *= fVal;
    m_weightedRootPositionWeight *= fVal;
    m_weightedRootOrientationWeight *= fVal;
    m_weightedPositionWeight *= fVal;
    m_weightedNormalWeight *= fVal;
    m_weightedOrientationWeight *= fVal;
    m_weightedGravityCompensation *= fVal;
    m_weightedSupportForceMultipler *= fVal;
    m_softLimitStiffnessScale *= fVal;
  }

  NM_INLINE LimbControl operator * (const float fVal) const
  {
    LimbControl result;
    result.m_weightedRootTargetPosition = m_weightedRootTargetPosition * fVal;
    result.m_weightedRootTargetOrientation = m_weightedRootTargetOrientation * fVal;
    result.m_weightedTargetOrientation = m_weightedTargetOrientation * fVal;
    result.m_weightedTargetPosition = m_weightedTargetPosition * fVal;
    result.m_weightedTargetNormal = m_weightedTargetNormal * fVal;
    result.m_weightedLocalNormal = m_weightedLocalNormal * fVal;
    result.m_weightedRootTargetVelocity = m_weightedRootTargetVelocity * fVal;
    result.m_weightedRootTargetAngularVelocity = m_weightedRootTargetAngularVelocity * fVal;
    result.m_weightedTargetVelocity = m_weightedTargetVelocity * fVal;
    result.m_weightedTargetAngularVelocity = m_weightedTargetAngularVelocity * fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    result.m_debugControlAmounts = m_debugControlAmounts * fVal;
#endif // MR_OUTPUT_DEBUGGING
    result.m_strength = m_strength * fVal;
    result.m_damping = m_damping * fVal;
    result.m_driveCompensation = m_driveCompensation * fVal;
    result.m_endSupportAmount = m_endSupportAmount * fVal;
    result.m_rootSupportedAmount = m_rootSupportedAmount * fVal;
    result.m_implicitStiffness = m_implicitStiffness * fVal;
    result.m_endFrictionScale = m_endFrictionScale * fVal;
    result.m_skinWidthIncrease = m_skinWidthIncrease * fVal;
    result.m_strengthReductionTowardsEnd = m_strengthReductionTowardsEnd * fVal;
    result.m_targetDeltaStep = m_targetDeltaStep * fVal;
    result.m_useIncrementalIK = m_useIncrementalIK * fVal;
    result.m_swivelAmount = m_swivelAmount * fVal;
    result.m_collisionGroupIndex = m_collisionGroupIndex * fVal;
    result.m_weightedRootPositionWeight = m_weightedRootPositionWeight * fVal;
    result.m_weightedRootOrientationWeight = m_weightedRootOrientationWeight * fVal;
    result.m_weightedPositionWeight = m_weightedPositionWeight * fVal;
    result.m_weightedNormalWeight = m_weightedNormalWeight * fVal;
    result.m_weightedOrientationWeight = m_weightedOrientationWeight * fVal;
    result.m_weightedGravityCompensation = m_weightedGravityCompensation * fVal;
    result.m_weightedSupportForceMultipler = m_weightedSupportForceMultipler * fVal;
    result.m_softLimitStiffnessScale = m_softLimitStiffnessScale * fVal;
    return result;
  }

  NM_INLINE void operator += (const LimbControl& rhs)
  {
    m_weightedRootTargetPosition += rhs.m_weightedRootTargetPosition;
    m_weightedRootTargetOrientation += rhs.m_weightedRootTargetOrientation;
    m_weightedTargetOrientation += rhs.m_weightedTargetOrientation;
    m_weightedTargetPosition += rhs.m_weightedTargetPosition;
    m_weightedTargetNormal += rhs.m_weightedTargetNormal;
    m_weightedLocalNormal += rhs.m_weightedLocalNormal;
    m_weightedRootTargetVelocity += rhs.m_weightedRootTargetVelocity;
    m_weightedRootTargetAngularVelocity += rhs.m_weightedRootTargetAngularVelocity;
    m_weightedTargetVelocity += rhs.m_weightedTargetVelocity;
    m_weightedTargetAngularVelocity += rhs.m_weightedTargetAngularVelocity;
#if defined(MR_OUTPUT_DEBUGGING)
    m_debugControlAmounts += rhs.m_debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    m_strength += rhs.m_strength;
    m_damping += rhs.m_damping;
    m_driveCompensation += rhs.m_driveCompensation;
    m_endSupportAmount += rhs.m_endSupportAmount;
    m_rootSupportedAmount += rhs.m_rootSupportedAmount;
    m_implicitStiffness += rhs.m_implicitStiffness;
    m_endFrictionScale += rhs.m_endFrictionScale;
    m_skinWidthIncrease += rhs.m_skinWidthIncrease;
    m_strengthReductionTowardsEnd += rhs.m_strengthReductionTowardsEnd;
    m_targetDeltaStep += rhs.m_targetDeltaStep;
    m_useIncrementalIK += rhs.m_useIncrementalIK;
    m_swivelAmount += rhs.m_swivelAmount;
    m_collisionGroupIndex += rhs.m_collisionGroupIndex;
    m_weightedRootPositionWeight += rhs.m_weightedRootPositionWeight;
    m_weightedRootOrientationWeight += rhs.m_weightedRootOrientationWeight;
    m_weightedPositionWeight += rhs.m_weightedPositionWeight;
    m_weightedNormalWeight += rhs.m_weightedNormalWeight;
    m_weightedOrientationWeight += rhs.m_weightedOrientationWeight;
    m_weightedGravityCompensation += rhs.m_weightedGravityCompensation;
    m_weightedSupportForceMultipler += rhs.m_weightedSupportForceMultipler;
    m_softLimitStiffnessScale += rhs.m_softLimitStiffnessScale;
  }

  NM_INLINE LimbControl operator + (const LimbControl& rhs) const
  {
    LimbControl result;
    result.m_weightedRootTargetPosition = m_weightedRootTargetPosition + rhs.m_weightedRootTargetPosition;
    result.m_weightedRootTargetOrientation = m_weightedRootTargetOrientation + rhs.m_weightedRootTargetOrientation;
    result.m_weightedTargetOrientation = m_weightedTargetOrientation + rhs.m_weightedTargetOrientation;
    result.m_weightedTargetPosition = m_weightedTargetPosition + rhs.m_weightedTargetPosition;
    result.m_weightedTargetNormal = m_weightedTargetNormal + rhs.m_weightedTargetNormal;
    result.m_weightedLocalNormal = m_weightedLocalNormal + rhs.m_weightedLocalNormal;
    result.m_weightedRootTargetVelocity = m_weightedRootTargetVelocity + rhs.m_weightedRootTargetVelocity;
    result.m_weightedRootTargetAngularVelocity = m_weightedRootTargetAngularVelocity + rhs.m_weightedRootTargetAngularVelocity;
    result.m_weightedTargetVelocity = m_weightedTargetVelocity + rhs.m_weightedTargetVelocity;
    result.m_weightedTargetAngularVelocity = m_weightedTargetAngularVelocity + rhs.m_weightedTargetAngularVelocity;
#if defined(MR_OUTPUT_DEBUGGING)
    result.m_debugControlAmounts = m_debugControlAmounts + rhs.m_debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    result.m_strength = m_strength + rhs.m_strength;
    result.m_damping = m_damping + rhs.m_damping;
    result.m_driveCompensation = m_driveCompensation + rhs.m_driveCompensation;
    result.m_endSupportAmount = m_endSupportAmount + rhs.m_endSupportAmount;
    result.m_rootSupportedAmount = m_rootSupportedAmount + rhs.m_rootSupportedAmount;
    result.m_implicitStiffness = m_implicitStiffness + rhs.m_implicitStiffness;
    result.m_endFrictionScale = m_endFrictionScale + rhs.m_endFrictionScale;
    result.m_skinWidthIncrease = m_skinWidthIncrease + rhs.m_skinWidthIncrease;
    result.m_strengthReductionTowardsEnd = m_strengthReductionTowardsEnd + rhs.m_strengthReductionTowardsEnd;
    result.m_targetDeltaStep = m_targetDeltaStep + rhs.m_targetDeltaStep;
    result.m_useIncrementalIK = m_useIncrementalIK + rhs.m_useIncrementalIK;
    result.m_swivelAmount = m_swivelAmount + rhs.m_swivelAmount;
    result.m_collisionGroupIndex = m_collisionGroupIndex + rhs.m_collisionGroupIndex;
    result.m_weightedRootPositionWeight = m_weightedRootPositionWeight + rhs.m_weightedRootPositionWeight;
    result.m_weightedRootOrientationWeight = m_weightedRootOrientationWeight + rhs.m_weightedRootOrientationWeight;
    result.m_weightedPositionWeight = m_weightedPositionWeight + rhs.m_weightedPositionWeight;
    result.m_weightedNormalWeight = m_weightedNormalWeight + rhs.m_weightedNormalWeight;
    result.m_weightedOrientationWeight = m_weightedOrientationWeight + rhs.m_weightedOrientationWeight;
    result.m_weightedGravityCompensation = m_weightedGravityCompensation + rhs.m_weightedGravityCompensation;
    result.m_weightedSupportForceMultipler = m_weightedSupportForceMultipler + rhs.m_weightedSupportForceMultipler;
    result.m_softLimitStiffnessScale = m_softLimitStiffnessScale + rhs.m_softLimitStiffnessScale;
    return result;
  }


  NM_INLINE void doQuatFlip(const LimbControl &acc)
  {
    if (m_weightedRootTargetOrientation.dot(acc.m_weightedRootTargetOrientation) < 0.f)
      m_weightedRootTargetOrientation.negate();
    if (m_weightedTargetOrientation.dot(acc.m_weightedTargetOrientation) < 0.f)
      m_weightedTargetOrientation.negate();
  }

  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(m_weightedRootTargetPosition), "m_weightedRootTargetPosition");
    NM_VALIDATOR(Vector3Valid(m_weightedTargetPosition), "m_weightedTargetPosition");
    NM_VALIDATOR(Vector3Valid(m_weightedTargetNormal), "m_weightedTargetNormal");
    NM_VALIDATOR(Vector3Valid(m_weightedLocalNormal), "m_weightedLocalNormal");
    NM_VALIDATOR(Vector3Valid(m_weightedRootTargetVelocity), "m_weightedRootTargetVelocity");
    NM_VALIDATOR(Vector3Valid(m_weightedRootTargetAngularVelocity), "m_weightedRootTargetAngularVelocity");
    NM_VALIDATOR(Vector3Valid(m_weightedTargetVelocity), "m_weightedTargetVelocity");
    NM_VALIDATOR(Vector3Valid(m_weightedTargetAngularVelocity), "m_weightedTargetAngularVelocity");
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
    NM_VALIDATOR(FloatNonNegative(m_strength), "m_strength");
    NM_VALIDATOR(FloatNonNegative(m_damping), "m_damping");
    NM_VALIDATOR(FloatNonNegative(m_driveCompensation), "m_driveCompensation");
    NM_VALIDATOR(FloatValid(m_endSupportAmount), "m_endSupportAmount");
    NM_VALIDATOR(FloatValid(m_rootSupportedAmount), "m_rootSupportedAmount");
    NM_VALIDATOR(FloatValid(m_implicitStiffness), "m_implicitStiffness");
    NM_VALIDATOR(FloatValid(m_endFrictionScale), "m_endFrictionScale");
    NM_VALIDATOR(FloatNonNegative(m_skinWidthIncrease), "m_skinWidthIncrease");
    NM_VALIDATOR(FloatNonNegative(m_strengthReductionTowardsEnd), "m_strengthReductionTowardsEnd");
    NM_VALIDATOR(FloatNonNegative(m_targetDeltaStep), "m_targetDeltaStep");
    NM_VALIDATOR(FloatNonNegative(m_useIncrementalIK), "m_useIncrementalIK");
    NM_VALIDATOR(FloatValid(m_swivelAmount), "m_swivelAmount");
    NM_VALIDATOR(FloatValid(m_collisionGroupIndex), "m_collisionGroupIndex");
    NM_VALIDATOR(FloatNonNegative(m_weightedRootPositionWeight), "m_weightedRootPositionWeight");
    NM_VALIDATOR(FloatNonNegative(m_weightedRootOrientationWeight), "m_weightedRootOrientationWeight");
    NM_VALIDATOR(FloatNonNegative(m_weightedPositionWeight), "m_weightedPositionWeight");
    NM_VALIDATOR(FloatNonNegative(m_weightedNormalWeight), "m_weightedNormalWeight");
    NM_VALIDATOR(FloatNonNegative(m_weightedOrientationWeight), "m_weightedOrientationWeight");
    NM_VALIDATOR(FloatValid(m_weightedGravityCompensation), "m_weightedGravityCompensation");
    NM_VALIDATOR(FloatValid(m_weightedSupportForceMultipler), "m_weightedSupportForceMultipler");
    NM_VALIDATOR(FloatValid(m_softLimitStiffnessScale), "m_softLimitStiffnessScale");
#endif // NM_CALL_VALIDATORS
  }

}; // struct LimbControl


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

namespace ER
{
  template<>
  NM_INLINE void quatFlip(NM_BEHAVIOUR_LIB_NAMESPACE::LimbControl &val, const NM_BEHAVIOUR_LIB_NAMESPACE::LimbControl &acc)
  {
    val.doQuatFlip(acc);
  }
}
#endif // NM_MDF_TYPE_LIMBCONTROL_H

