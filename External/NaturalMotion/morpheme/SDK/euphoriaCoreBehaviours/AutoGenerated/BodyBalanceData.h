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

#ifndef NM_MDF_BODYBALANCE_DATA_H
#define NM_MDF_BODYBALANCE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BodyBalance.module"

// external types
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/BalanceParameters.h"
#include "Types/BalancePoseClamping.h"
#include "Types/StepRecoveryParameters.h"
#include "Types/Environment_SupportPoly.h"
#include "Types/RotationRequest.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

// constants
#include "NetworkConstants.h"

#include <string.h>

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4324) // structure was padded due to __declspec(align()) 
#endif // _MSC_VER

/** 
 * \class BodyBalanceData
 * \ingroup BodyBalance
 * \brief Data Overall module dealing with applying overall body balance
 *  Note that this deals with balancing on legs and arms. Where
 *  legs and arms are treated identically then legs will be stored
 *  first, and the leg/arm is called a limb.
 *
 * Data packet definition (581 bytes, 608 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyBalanceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyBalanceData));
  }

  enum Constants
  {
    /*  4 */ rigMaxNumSupports = (NetworkConstants::networkMaxNumLegs + NetworkConstants::networkMaxNumArms),  
  };

  ER::LimbTransform supportTM;  /// The time-smoothed centre of support position - at the moment average of the feet - weighted by the amount each
  /// foot (or whatever) is actually supporting. Orientation gives yAxis "up" (against the effective gravity), 
  /// xAxis "forward"
  ER::LimbTransform horizontalSupportTM;  /// horizontal version of supportTM
  NMP::Matrix34 targetPelvisRelSupport;      ///< (Transform)
  NMP::Vector3 effectiveGravityDir;  /// Direction of the effective gravity (adjusted if the floor is accelerating)  (Direction)
  NMP::Vector3 averageSupportingLimbContactNormal;  /// Average of all the supporting limb's contact normals  (Direction)
  NMP::Vector3 groundContactPoint;  /// This is the position on the ground around which balancing is done. It won't necessarily be the
  /// same as the supportTM position as that is the intended support position - i.e. it won't shift
  /// if a foot is lifted, but is still considered to be part of the balance.
  NMP::Vector3 supportForwardRate;  /// Rate of change of the support forward direction (units of /s)
  NMP::Vector3 groundVel;  /// The current (smoothed) ground velocity (averaged over all supporting limbs)  (Velocity)
  NMP::Vector3 groundVelRate;  /// Rate of change of the ground velocity so we can smooth it  (Acceleration)
  NMP::Vector3 lastGroundVel;  /// Store the old ground velocity so we can estimate the acceleration  (Velocity)
  NMP::Vector3 groundAccel;  /// (Smoothed) Acceleration of the feet/standing surface  (Acceleration)
  NMP::Vector3 groundAccelRate;  /// Rate of change of the acceleration of the standing surface, used for smoothing  (Jerk)
  NMP::Vector3 COMVel;  /// velocity of the CoM  (Velocity)
  NMP::Vector3 horCOMVelRelGround;  /// Horizontal (in support plane) COM velocity relative to the ground (depends on a foot touching the ground)  (Velocity)
  NMP::Vector3 targetPelvisRelSupportPosition;  // Used to adjust/correct the incoming desired pose  (PositionDelta)
  NMP::Vector3 targetPelvisRelSupportOrientation;  ///< (RotationVector)
  NMP::Vector3 targetPelvisRelSupportPositionRate;  ///< (Velocity)
  NMP::Vector3 lastTargetPelvisRelSupportPosition;  ///< used in stabilise_method 3  (Position)
  NMP::Vector3 targetPelvisRelSupportOrientationRate;  ///< (AngularVelocity)
  float balanceAmount;  ///< From 0 to 1  (Weight)
  uint32_t numStepsTaken;
  float timeSinceLastStep;                   ///< (TimePeriod)
  float enableSupportOnLimbs[BodyBalanceData::rigMaxNumSupports];  /// Use this to "dot product" against arrays of all arms/legs  (Weight)
  float enableStepOnLimbs[BodyBalanceData::rigMaxNumSupports];  ///< (Weight)
  float smoothedArmStrengthRate[NetworkConstants::networkMaxNumArms];
  float continueArmsTimer;  /// When we start swinging our arms to balance we use this timer to prevent them stopping too soon.  (TimePeriod)
  float sidewaysPosErrorFrac;  // The COM error from the desired position, normalised to the support dimensions (but then squared
  // and scaled)  (Multiplier)
  float forwardsPosErrorFrac;                ///< (Multiplier)
  float limbWeights[BodyBalanceData::rigMaxNumSupports];  ///< weights per supporting limb claculated and stored in feedback for use in update  (Weight)
  float supportPointWeightingPerLimb[BodyBalanceData::rigMaxNumSupports];  ///< (Multiplier)
  float smoothedLegIKOrientationError;  // Used when clamping the desired pose to smooth the IK error and use that to limit the deviations  (Distance)
  float smoothedLegIKOrientationErrorRate;   ///< (Speed)
  float smoothedLegStrength[NetworkConstants::networkMaxNumLegs];
  float smoothedArmStrength[NetworkConstants::networkMaxNumArms];
  float smoothedLegStrengthRate[NetworkConstants::networkMaxNumLegs];
  bool needToInitialiseSupportTM;
  bool initialisedTargetPelvisRelSupport;
  bool needToStep;
  bool isFirstUpdate;  /// True only right at the start of the update (and then cleared).
  bool isStepping;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(targetPelvisRelSupport), "targetPelvisRelSupport");
    NM_VALIDATOR(Vector3Normalised(effectiveGravityDir), "effectiveGravityDir");
    NM_VALIDATOR(Vector3Normalised(averageSupportingLimbContactNormal), "averageSupportingLimbContactNormal");
    NM_VALIDATOR(Vector3Valid(groundContactPoint), "groundContactPoint");
    NM_VALIDATOR(Vector3Valid(supportForwardRate), "supportForwardRate");
    NM_VALIDATOR(Vector3Valid(groundVel), "groundVel");
    NM_VALIDATOR(Vector3Valid(groundVelRate), "groundVelRate");
    NM_VALIDATOR(Vector3Valid(lastGroundVel), "lastGroundVel");
    NM_VALIDATOR(Vector3Valid(groundAccel), "groundAccel");
    NM_VALIDATOR(Vector3Valid(groundAccelRate), "groundAccelRate");
    NM_VALIDATOR(Vector3Valid(COMVel), "COMVel");
    NM_VALIDATOR(Vector3Valid(horCOMVelRelGround), "horCOMVelRelGround");
    NM_VALIDATOR(Vector3Valid(targetPelvisRelSupportPosition), "targetPelvisRelSupportPosition");
    NM_VALIDATOR(Vector3Valid(targetPelvisRelSupportOrientation), "targetPelvisRelSupportOrientation");
    NM_VALIDATOR(Vector3Valid(targetPelvisRelSupportPositionRate), "targetPelvisRelSupportPositionRate");
    NM_VALIDATOR(Vector3Valid(lastTargetPelvisRelSupportPosition), "lastTargetPelvisRelSupportPosition");
    NM_VALIDATOR(Vector3Valid(targetPelvisRelSupportOrientationRate), "targetPelvisRelSupportOrientationRate");
    NM_VALIDATOR(FloatNonNegative(balanceAmount), "balanceAmount");
    NM_VALIDATOR(FloatValid(timeSinceLastStep), "timeSinceLastStep");
    NM_VALIDATOR(FloatNonNegative(enableSupportOnLimbs[0]), "enableSupportOnLimbs[0]");
    NM_VALIDATOR(FloatNonNegative(enableSupportOnLimbs[1]), "enableSupportOnLimbs[1]");
    NM_VALIDATOR(FloatNonNegative(enableSupportOnLimbs[2]), "enableSupportOnLimbs[2]");
    NM_VALIDATOR(FloatNonNegative(enableSupportOnLimbs[3]), "enableSupportOnLimbs[3]");
    NM_VALIDATOR(FloatNonNegative(enableStepOnLimbs[0]), "enableStepOnLimbs[0]");
    NM_VALIDATOR(FloatNonNegative(enableStepOnLimbs[1]), "enableStepOnLimbs[1]");
    NM_VALIDATOR(FloatNonNegative(enableStepOnLimbs[2]), "enableStepOnLimbs[2]");
    NM_VALIDATOR(FloatNonNegative(enableStepOnLimbs[3]), "enableStepOnLimbs[3]");
    NM_VALIDATOR(FloatValid(smoothedArmStrengthRate[0]), "smoothedArmStrengthRate[0]");
    NM_VALIDATOR(FloatValid(smoothedArmStrengthRate[1]), "smoothedArmStrengthRate[1]");
    NM_VALIDATOR(FloatValid(continueArmsTimer), "continueArmsTimer");
    NM_VALIDATOR(FloatValid(sidewaysPosErrorFrac), "sidewaysPosErrorFrac");
    NM_VALIDATOR(FloatValid(forwardsPosErrorFrac), "forwardsPosErrorFrac");
    NM_VALIDATOR(FloatNonNegative(limbWeights[0]), "limbWeights[0]");
    NM_VALIDATOR(FloatNonNegative(limbWeights[1]), "limbWeights[1]");
    NM_VALIDATOR(FloatNonNegative(limbWeights[2]), "limbWeights[2]");
    NM_VALIDATOR(FloatNonNegative(limbWeights[3]), "limbWeights[3]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerLimb[0]), "supportPointWeightingPerLimb[0]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerLimb[1]), "supportPointWeightingPerLimb[1]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerLimb[2]), "supportPointWeightingPerLimb[2]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerLimb[3]), "supportPointWeightingPerLimb[3]");
    NM_VALIDATOR(FloatValid(smoothedLegIKOrientationError), "smoothedLegIKOrientationError");
    NM_VALIDATOR(FloatValid(smoothedLegIKOrientationErrorRate), "smoothedLegIKOrientationErrorRate");
    NM_VALIDATOR(FloatValid(smoothedLegStrength[0]), "smoothedLegStrength[0]");
    NM_VALIDATOR(FloatValid(smoothedLegStrength[1]), "smoothedLegStrength[1]");
    NM_VALIDATOR(FloatValid(smoothedArmStrength[0]), "smoothedArmStrength[0]");
    NM_VALIDATOR(FloatValid(smoothedArmStrength[1]), "smoothedArmStrength[1]");
    NM_VALIDATOR(FloatValid(smoothedLegStrengthRate[0]), "smoothedLegStrengthRate[0]");
    NM_VALIDATOR(FloatValid(smoothedLegStrengthRate[1]), "smoothedLegStrengthRate[1]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyBalanceInputs
 * \ingroup BodyBalance
 * \brief Inputs Overall module dealing with applying overall body balance
 *  Note that this deals with balancing on legs and arms. Where
 *  legs and arms are treated identically then legs will be stored
 *  first, and the leg/arm is called a limb.
 *
 * Data packet definition (987 bytes, 992 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyBalanceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyBalanceInputs));
  }

  NM_INLINE void setSupportPolygon(const Environment::SupportPoly& _supportPolygon, float supportPolygon_importance = 1.f)
  {
    supportPolygon = _supportPolygon;
    postAssignSupportPolygon(supportPolygon);
    NMP_ASSERT(supportPolygon_importance >= 0.0f && supportPolygon_importance <= 1.0f);
    m_supportPolygonImportance = supportPolygon_importance;
  }
  NM_INLINE Environment::SupportPoly& startSupportPolygonModification()
  {
    m_supportPolygonImportance = -1.0f; // set invalid until stopSupportPolygonModification()
    return supportPolygon;
  }
  NM_INLINE void stopSupportPolygonModification(float supportPolygon_importance = 1.f)
  {
    postAssignSupportPolygon(supportPolygon);
    NMP_ASSERT(supportPolygon_importance >= 0.0f && supportPolygon_importance <= 1.0f);
    m_supportPolygonImportance = supportPolygon_importance;
  }
  NM_INLINE float getSupportPolygonImportance() const { return m_supportPolygonImportance; }
  NM_INLINE const float& getSupportPolygonImportanceRef() const { return m_supportPolygonImportance; }
  NM_INLINE const Environment::SupportPoly& getSupportPolygon() const { return supportPolygon; }

  NM_INLINE void setPelvisRelSupport(const NMP::Matrix34& _pelvisRelSupport, float pelvisRelSupport_importance = 1.f)
  {
    pelvisRelSupport = _pelvisRelSupport;
    postAssignPelvisRelSupport(pelvisRelSupport);
    NMP_ASSERT(pelvisRelSupport_importance >= 0.0f && pelvisRelSupport_importance <= 1.0f);
    m_pelvisRelSupportImportance = pelvisRelSupport_importance;
  }
  NM_INLINE float getPelvisRelSupportImportance() const { return m_pelvisRelSupportImportance; }
  NM_INLINE const float& getPelvisRelSupportImportanceRef() const { return m_pelvisRelSupportImportance; }
  NM_INLINE const NMP::Matrix34& getPelvisRelSupport() const { return pelvisRelSupport; }

  enum { maxSpinePoseEndRelativeToRoot = 1 };
  NM_INLINE unsigned int getMaxSpinePoseEndRelativeToRoot() const { return maxSpinePoseEndRelativeToRoot; }
  NM_INLINE void setSpinePoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _spinePoseEndRelativeToRoot, float spinePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinePoseEndRelativeToRoot);
    NMP_ASSERT(spinePoseEndRelativeToRoot_importance >= 0.0f && spinePoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinePoseEndRelativeToRoot[i] = _spinePoseEndRelativeToRoot[i];
      postAssignSpinePoseEndRelativeToRoot(spinePoseEndRelativeToRoot[i]);
      m_spinePoseEndRelativeToRootImportance[i] = spinePoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setSpinePoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _spinePoseEndRelativeToRoot, float spinePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinePoseEndRelativeToRoot);
    spinePoseEndRelativeToRoot[index] = _spinePoseEndRelativeToRoot;
    NMP_ASSERT(spinePoseEndRelativeToRoot_importance >= 0.0f && spinePoseEndRelativeToRoot_importance <= 1.0f);
    postAssignSpinePoseEndRelativeToRoot(spinePoseEndRelativeToRoot[index]);
    m_spinePoseEndRelativeToRootImportance[index] = spinePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getSpinePoseEndRelativeToRootImportance(int index) const { return m_spinePoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getSpinePoseEndRelativeToRootImportanceRef(int index) const { return m_spinePoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinePoseEndRelativeToRoot() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spinePoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getSpinePoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxSpinePoseEndRelativeToRoot); return spinePoseEndRelativeToRoot[index]; }

  enum { maxLegEndRelativeToRoot = 2 };
  NM_INLINE unsigned int getMaxLegEndRelativeToRoot() const { return maxLegEndRelativeToRoot; }
  NM_INLINE void setLegEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _legEndRelativeToRoot, float legEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegEndRelativeToRoot);
    NMP_ASSERT(legEndRelativeToRoot_importance >= 0.0f && legEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legEndRelativeToRoot[i] = _legEndRelativeToRoot[i];
      postAssignLegEndRelativeToRoot(legEndRelativeToRoot[i]);
      m_legEndRelativeToRootImportance[i] = legEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setLegEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _legEndRelativeToRoot, float legEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegEndRelativeToRoot);
    legEndRelativeToRoot[index] = _legEndRelativeToRoot;
    NMP_ASSERT(legEndRelativeToRoot_importance >= 0.0f && legEndRelativeToRoot_importance <= 1.0f);
    postAssignLegEndRelativeToRoot(legEndRelativeToRoot[index]);
    m_legEndRelativeToRootImportance[index] = legEndRelativeToRoot_importance;
  }
  NM_INLINE float getLegEndRelativeToRootImportance(int index) const { return m_legEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getLegEndRelativeToRootImportanceRef(int index) const { return m_legEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumLegEndRelativeToRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getLegEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxLegEndRelativeToRoot); return legEndRelativeToRoot[index]; }

  NM_INLINE void setBalanceParameters(const BalanceParameters& _balanceParameters, float balanceParameters_importance = 1.f)
  {
    balanceParameters = _balanceParameters;
    postAssignBalanceParameters(balanceParameters);
    NMP_ASSERT(balanceParameters_importance >= 0.0f && balanceParameters_importance <= 1.0f);
    m_balanceParametersImportance = balanceParameters_importance;
  }
  NM_INLINE BalanceParameters& startBalanceParametersModification()
  {
    m_balanceParametersImportance = -1.0f; // set invalid until stopBalanceParametersModification()
    return balanceParameters;
  }
  NM_INLINE void stopBalanceParametersModification(float balanceParameters_importance = 1.f)
  {
    postAssignBalanceParameters(balanceParameters);
    NMP_ASSERT(balanceParameters_importance >= 0.0f && balanceParameters_importance <= 1.0f);
    m_balanceParametersImportance = balanceParameters_importance;
  }
  NM_INLINE float getBalanceParametersImportance() const { return m_balanceParametersImportance; }
  NM_INLINE const float& getBalanceParametersImportanceRef() const { return m_balanceParametersImportance; }
  NM_INLINE const BalanceParameters& getBalanceParameters() const { return balanceParameters; }

  NM_INLINE void setBalancePoseClamping(const BalancePoseClamping& _balancePoseClamping, float balancePoseClamping_importance = 1.f)
  {
    balancePoseClamping = _balancePoseClamping;
    postAssignBalancePoseClamping(balancePoseClamping);
    NMP_ASSERT(balancePoseClamping_importance >= 0.0f && balancePoseClamping_importance <= 1.0f);
    m_balancePoseClampingImportance = balancePoseClamping_importance;
  }
  NM_INLINE float getBalancePoseClampingImportance() const { return m_balancePoseClampingImportance; }
  NM_INLINE const float& getBalancePoseClampingImportanceRef() const { return m_balancePoseClampingImportance; }
  NM_INLINE const BalancePoseClamping& getBalancePoseClamping() const { return balancePoseClamping; }

  NM_INLINE void setStepRecoveryParameters(const StepRecoveryParameters& _stepRecoveryParameters, float stepRecoveryParameters_importance = 1.f)
  {
    stepRecoveryParameters = _stepRecoveryParameters;
    postAssignStepRecoveryParameters(stepRecoveryParameters);
    NMP_ASSERT(stepRecoveryParameters_importance >= 0.0f && stepRecoveryParameters_importance <= 1.0f);
    m_stepRecoveryParametersImportance = stepRecoveryParameters_importance;
  }
  NM_INLINE float getStepRecoveryParametersImportance() const { return m_stepRecoveryParametersImportance; }
  NM_INLINE const float& getStepRecoveryParametersImportanceRef() const { return m_stepRecoveryParametersImportance; }
  NM_INLINE const StepRecoveryParameters& getStepRecoveryParameters() const { return stepRecoveryParameters; }

  NM_INLINE void setBalanceUsingAnimationHorizontalPelvisOffsetAmount(const float& _balanceUsingAnimationHorizontalPelvisOffsetAmount, float balanceUsingAnimationHorizontalPelvisOffsetAmount_importance = 1.f)
  {
    balanceUsingAnimationHorizontalPelvisOffsetAmount = _balanceUsingAnimationHorizontalPelvisOffsetAmount;
    postAssignBalanceUsingAnimationHorizontalPelvisOffsetAmount(balanceUsingAnimationHorizontalPelvisOffsetAmount);
    NMP_ASSERT(balanceUsingAnimationHorizontalPelvisOffsetAmount_importance >= 0.0f && balanceUsingAnimationHorizontalPelvisOffsetAmount_importance <= 1.0f);
    m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance = balanceUsingAnimationHorizontalPelvisOffsetAmount_importance;
  }
  NM_INLINE float getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportance() const { return m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance; }
  NM_INLINE const float& getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportanceRef() const { return m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance; }
  NM_INLINE const float& getBalanceUsingAnimationHorizontalPelvisOffsetAmount() const { return balanceUsingAnimationHorizontalPelvisOffsetAmount; }

  NM_INLINE void setEnableSupportOnLegs(const bool& _enableSupportOnLegs, float enableSupportOnLegs_importance = 1.f)
  {
    enableSupportOnLegs = _enableSupportOnLegs;
    NMP_ASSERT(enableSupportOnLegs_importance >= 0.0f && enableSupportOnLegs_importance <= 1.0f);
    m_enableSupportOnLegsImportance = enableSupportOnLegs_importance;
  }
  NM_INLINE float getEnableSupportOnLegsImportance() const { return m_enableSupportOnLegsImportance; }
  NM_INLINE const float& getEnableSupportOnLegsImportanceRef() const { return m_enableSupportOnLegsImportance; }
  NM_INLINE const bool& getEnableSupportOnLegs() const { return enableSupportOnLegs; }

  NM_INLINE void setEnableSupportOnArms(const bool& _enableSupportOnArms, float enableSupportOnArms_importance = 1.f)
  {
    enableSupportOnArms = _enableSupportOnArms;
    NMP_ASSERT(enableSupportOnArms_importance >= 0.0f && enableSupportOnArms_importance <= 1.0f);
    m_enableSupportOnArmsImportance = enableSupportOnArms_importance;
  }
  NM_INLINE float getEnableSupportOnArmsImportance() const { return m_enableSupportOnArmsImportance; }
  NM_INLINE const float& getEnableSupportOnArmsImportanceRef() const { return m_enableSupportOnArmsImportance; }
  NM_INLINE const bool& getEnableSupportOnArms() const { return enableSupportOnArms; }

  NM_INLINE void setEnableStepOnLegs(const bool& _enableStepOnLegs, float enableStepOnLegs_importance = 1.f)
  {
    enableStepOnLegs = _enableStepOnLegs;
    NMP_ASSERT(enableStepOnLegs_importance >= 0.0f && enableStepOnLegs_importance <= 1.0f);
    m_enableStepOnLegsImportance = enableStepOnLegs_importance;
  }
  NM_INLINE float getEnableStepOnLegsImportance() const { return m_enableStepOnLegsImportance; }
  NM_INLINE const float& getEnableStepOnLegsImportanceRef() const { return m_enableStepOnLegsImportance; }
  NM_INLINE const bool& getEnableStepOnLegs() const { return enableStepOnLegs; }

  NM_INLINE void setEnableStepOnArms(const bool& _enableStepOnArms, float enableStepOnArms_importance = 1.f)
  {
    enableStepOnArms = _enableStepOnArms;
    NMP_ASSERT(enableStepOnArms_importance >= 0.0f && enableStepOnArms_importance <= 1.0f);
    m_enableStepOnArmsImportance = enableStepOnArms_importance;
  }
  NM_INLINE float getEnableStepOnArmsImportance() const { return m_enableStepOnArmsImportance; }
  NM_INLINE const float& getEnableStepOnArmsImportanceRef() const { return m_enableStepOnArmsImportance; }
  NM_INLINE const bool& getEnableStepOnArms() const { return enableStepOnArms; }

protected:

  Environment::SupportPoly supportPolygon;
  NMP::Matrix34 pelvisRelSupport;  /// The desired rotation and offset of the pelvis relative to the centre and direction of support.
  /// The support coordinate from has the same convention as the spine etc - i.e. y up, x fwd
  /// The horizontal component is used as a desired offset from the centre of support - so for
  /// normal balancing, make sure this is zeroed!  (Transform)
  NMP::Matrix34 spinePoseEndRelativeToRoot[NetworkConstants::networkMaxNumSpines];  /// Spine balance pose  (Transform)
  NMP::Matrix34 legEndRelativeToRoot[NetworkConstants::networkMaxNumLegs];  /// Leg balance pose  (Transform)
  BalanceParameters balanceParameters;
  BalancePoseClamping balancePoseClamping;
  StepRecoveryParameters stepRecoveryParameters;
  float balanceUsingAnimationHorizontalPelvisOffsetAmount;  /// If 1 then the incoming horizontal pelvis offset from support should be kept. If 0 then the CoM will be used
  /// balance.   (ClampedWeight)
  bool enableSupportOnLegs;
  bool enableSupportOnArms;
  bool enableStepOnLegs;
  bool enableStepOnArms;

  // post-assignment stubs
  NM_INLINE void postAssignSupportPolygon(const Environment::SupportPoly& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "pelvisRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "spinePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "legEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceParameters(const BalanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalancePoseClamping(const BalancePoseClamping& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepRecoveryParameters(const StepRecoveryParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceUsingAnimationHorizontalPelvisOffsetAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "balanceUsingAnimationHorizontalPelvisOffsetAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_supportPolygonImportance, 
     m_pelvisRelSupportImportance, 
     m_spinePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumSpines], 
     m_legEndRelativeToRootImportance[NetworkConstants::networkMaxNumLegs], 
     m_balanceParametersImportance, 
     m_balancePoseClampingImportance, 
     m_stepRecoveryParametersImportance, 
     m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance, 
     m_enableSupportOnLegsImportance, 
     m_enableSupportOnArmsImportance, 
     m_enableStepOnLegsImportance, 
     m_enableStepOnArmsImportance;

  friend class BodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSupportPolygonImportance() > 0.0f)
    {
      supportPolygon.validate();
    }
    if (getPelvisRelSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(pelvisRelSupport), "pelvisRelSupport");
    }
    {
      uint32_t numSpinePoseEndRelativeToRoot = calculateNumSpinePoseEndRelativeToRoot();
      for (uint32_t i=0; i<numSpinePoseEndRelativeToRoot; i++)
      {
        if (getSpinePoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(spinePoseEndRelativeToRoot[i]), "spinePoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numLegEndRelativeToRoot = calculateNumLegEndRelativeToRoot();
      for (uint32_t i=0; i<numLegEndRelativeToRoot; i++)
      {
        if (getLegEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(legEndRelativeToRoot[i]), "legEndRelativeToRoot");
        }
      }
    }
    if (getBalanceParametersImportance() > 0.0f)
    {
      balanceParameters.validate();
    }
    if (getBalancePoseClampingImportance() > 0.0f)
    {
      balancePoseClamping.validate();
    }
    if (getStepRecoveryParametersImportance() > 0.0f)
    {
      stepRecoveryParameters.validate();
    }
    if (getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(balanceUsingAnimationHorizontalPelvisOffsetAmount), "balanceUsingAnimationHorizontalPelvisOffsetAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyBalanceOutputs
 * \ingroup BodyBalance
 * \brief Outputs Overall module dealing with applying overall body balance
 *  Note that this deals with balancing on legs and arms. Where
 *  legs and arms are treated identically then legs will be stored
 *  first, and the leg/arm is called a limb.
 *
 * Data packet definition (839 bytes, 864 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyBalanceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyBalanceOutputs));
  }

  NM_INLINE void setPelvisRotationRequest(const RotationRequest& _pelvisRotationRequest, float pelvisRotationRequest_importance = 1.f)
  {
    pelvisRotationRequest = _pelvisRotationRequest;
    postAssignPelvisRotationRequest(pelvisRotationRequest);
    NMP_ASSERT(pelvisRotationRequest_importance >= 0.0f && pelvisRotationRequest_importance <= 1.0f);
    m_pelvisRotationRequestImportance = pelvisRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startPelvisRotationRequestModification()
  {
    m_pelvisRotationRequestImportance = -1.0f; // set invalid until stopPelvisRotationRequestModification()
    return pelvisRotationRequest;
  }
  NM_INLINE void stopPelvisRotationRequestModification(float pelvisRotationRequest_importance = 1.f)
  {
    postAssignPelvisRotationRequest(pelvisRotationRequest);
    NMP_ASSERT(pelvisRotationRequest_importance >= 0.0f && pelvisRotationRequest_importance <= 1.0f);
    m_pelvisRotationRequestImportance = pelvisRotationRequest_importance;
  }
  NM_INLINE float getPelvisRotationRequestImportance() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const float& getPelvisRotationRequestImportanceRef() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const RotationRequest& getPelvisRotationRequest() const { return pelvisRotationRequest; }

  NM_INLINE void setDesiredPelvisTM(const ER::LimbTransform& _desiredPelvisTM, float desiredPelvisTM_importance = 1.f)
  {
    desiredPelvisTM = _desiredPelvisTM;
    NMP_ASSERT(desiredPelvisTM_importance >= 0.0f && desiredPelvisTM_importance <= 1.0f);
    m_desiredPelvisTMImportance = desiredPelvisTM_importance;
  }
  NM_INLINE ER::LimbTransform& startDesiredPelvisTMModification()
  {
    m_desiredPelvisTMImportance = -1.0f; // set invalid until stopDesiredPelvisTMModification()
    return desiredPelvisTM;
  }
  NM_INLINE void stopDesiredPelvisTMModification(float desiredPelvisTM_importance = 1.f)
  {
    NMP_ASSERT(desiredPelvisTM_importance >= 0.0f && desiredPelvisTM_importance <= 1.0f);
    m_desiredPelvisTMImportance = desiredPelvisTM_importance;
  }
  NM_INLINE float getDesiredPelvisTMImportance() const { return m_desiredPelvisTMImportance; }
  NM_INLINE const float& getDesiredPelvisTMImportanceRef() const { return m_desiredPelvisTMImportance; }
  NM_INLINE const ER::LimbTransform& getDesiredPelvisTM() const { return desiredPelvisTM; }

  NM_INLINE void setDesiredChestTM(const ER::LimbTransform& _desiredChestTM, float desiredChestTM_importance = 1.f)
  {
    desiredChestTM = _desiredChestTM;
    NMP_ASSERT(desiredChestTM_importance >= 0.0f && desiredChestTM_importance <= 1.0f);
    m_desiredChestTMImportance = desiredChestTM_importance;
  }
  NM_INLINE ER::LimbTransform& startDesiredChestTMModification()
  {
    m_desiredChestTMImportance = -1.0f; // set invalid until stopDesiredChestTMModification()
    return desiredChestTM;
  }
  NM_INLINE void stopDesiredChestTMModification(float desiredChestTM_importance = 1.f)
  {
    NMP_ASSERT(desiredChestTM_importance >= 0.0f && desiredChestTM_importance <= 1.0f);
    m_desiredChestTMImportance = desiredChestTM_importance;
  }
  NM_INLINE float getDesiredChestTMImportance() const { return m_desiredChestTMImportance; }
  NM_INLINE const float& getDesiredChestTMImportanceRef() const { return m_desiredChestTMImportance; }
  NM_INLINE const ER::LimbTransform& getDesiredChestTM() const { return desiredChestTM; }

  enum { maxRecoveryLegStepTarget = 2 };
  NM_INLINE unsigned int getMaxRecoveryLegStepTarget() const { return maxRecoveryLegStepTarget; }
  NM_INLINE void setRecoveryLegStepTarget(unsigned int number, const NMP::Matrix34* _recoveryLegStepTarget, float recoveryLegStepTarget_importance = 1.f)
  {
    NMP_ASSERT(number <= maxRecoveryLegStepTarget);
    NMP_ASSERT(recoveryLegStepTarget_importance >= 0.0f && recoveryLegStepTarget_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      recoveryLegStepTarget[i] = _recoveryLegStepTarget[i];
      postAssignRecoveryLegStepTarget(recoveryLegStepTarget[i]);
      m_recoveryLegStepTargetImportance[i] = recoveryLegStepTarget_importance;
    }
  }
  NM_INLINE void setRecoveryLegStepTargetAt(unsigned int index, const NMP::Matrix34& _recoveryLegStepTarget, float recoveryLegStepTarget_importance = 1.f)
  {
    NMP_ASSERT(index < maxRecoveryLegStepTarget);
    recoveryLegStepTarget[index] = _recoveryLegStepTarget;
    NMP_ASSERT(recoveryLegStepTarget_importance >= 0.0f && recoveryLegStepTarget_importance <= 1.0f);
    postAssignRecoveryLegStepTarget(recoveryLegStepTarget[index]);
    m_recoveryLegStepTargetImportance[index] = recoveryLegStepTarget_importance;
  }
  NM_INLINE float getRecoveryLegStepTargetImportance(int index) const { return m_recoveryLegStepTargetImportance[index]; }
  NM_INLINE const float& getRecoveryLegStepTargetImportanceRef(int index) const { return m_recoveryLegStepTargetImportance[index]; }
  NM_INLINE unsigned int calculateNumRecoveryLegStepTarget() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_recoveryLegStepTargetImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getRecoveryLegStepTarget(unsigned int index) const { NMP_ASSERT(index <= maxRecoveryLegStepTarget); return recoveryLegStepTarget[index]; }

  enum { maxRecoveryArmStepTarget = 2 };
  NM_INLINE unsigned int getMaxRecoveryArmStepTarget() const { return maxRecoveryArmStepTarget; }
  NM_INLINE void setRecoveryArmStepTarget(unsigned int number, const NMP::Matrix34* _recoveryArmStepTarget, float recoveryArmStepTarget_importance = 1.f)
  {
    NMP_ASSERT(number <= maxRecoveryArmStepTarget);
    NMP_ASSERT(recoveryArmStepTarget_importance >= 0.0f && recoveryArmStepTarget_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      recoveryArmStepTarget[i] = _recoveryArmStepTarget[i];
      postAssignRecoveryArmStepTarget(recoveryArmStepTarget[i]);
      m_recoveryArmStepTargetImportance[i] = recoveryArmStepTarget_importance;
    }
  }
  NM_INLINE void setRecoveryArmStepTargetAt(unsigned int index, const NMP::Matrix34& _recoveryArmStepTarget, float recoveryArmStepTarget_importance = 1.f)
  {
    NMP_ASSERT(index < maxRecoveryArmStepTarget);
    recoveryArmStepTarget[index] = _recoveryArmStepTarget;
    NMP_ASSERT(recoveryArmStepTarget_importance >= 0.0f && recoveryArmStepTarget_importance <= 1.0f);
    postAssignRecoveryArmStepTarget(recoveryArmStepTarget[index]);
    m_recoveryArmStepTargetImportance[index] = recoveryArmStepTarget_importance;
  }
  NM_INLINE float getRecoveryArmStepTargetImportance(int index) const { return m_recoveryArmStepTargetImportance[index]; }
  NM_INLINE const float& getRecoveryArmStepTargetImportanceRef(int index) const { return m_recoveryArmStepTargetImportance[index]; }
  NM_INLINE unsigned int calculateNumRecoveryArmStepTarget() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_recoveryArmStepTargetImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getRecoveryArmStepTarget(unsigned int index) const { NMP_ASSERT(index <= maxRecoveryArmStepTarget); return recoveryArmStepTarget[index]; }

  NM_INLINE void setSupportTM(const ER::LimbTransform& _supportTM, float supportTM_importance = 1.f)
  {
    supportTM = _supportTM;
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE ER::LimbTransform& startSupportTMModification()
  {
    m_supportTMImportance = -1.0f; // set invalid until stopSupportTMModification()
    return supportTM;
  }
  NM_INLINE void stopSupportTMModification(float supportTM_importance = 1.f)
  {
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE float getSupportTMImportance() const { return m_supportTMImportance; }
  NM_INLINE const float& getSupportTMImportanceRef() const { return m_supportTMImportance; }
  NM_INLINE const ER::LimbTransform& getSupportTM() const { return supportTM; }

  NM_INLINE void setTargetPelvisTM(const ER::LimbTransform& _targetPelvisTM, float targetPelvisTM_importance = 1.f)
  {
    targetPelvisTM = _targetPelvisTM;
    NMP_ASSERT(targetPelvisTM_importance >= 0.0f && targetPelvisTM_importance <= 1.0f);
    m_targetPelvisTMImportance = targetPelvisTM_importance;
  }
  NM_INLINE ER::LimbTransform& startTargetPelvisTMModification()
  {
    m_targetPelvisTMImportance = -1.0f; // set invalid until stopTargetPelvisTMModification()
    return targetPelvisTM;
  }
  NM_INLINE void stopTargetPelvisTMModification(float targetPelvisTM_importance = 1.f)
  {
    NMP_ASSERT(targetPelvisTM_importance >= 0.0f && targetPelvisTM_importance <= 1.0f);
    m_targetPelvisTMImportance = targetPelvisTM_importance;
  }
  NM_INLINE float getTargetPelvisTMImportance() const { return m_targetPelvisTMImportance; }
  NM_INLINE const float& getTargetPelvisTMImportanceRef() const { return m_targetPelvisTMImportance; }
  NM_INLINE const ER::LimbTransform& getTargetPelvisTM() const { return targetPelvisTM; }

  NM_INLINE void setGroundVelocity(const NMP::Vector3& _groundVelocity, float groundVelocity_importance = 1.f)
  {
    groundVelocity = _groundVelocity;
    postAssignGroundVelocity(groundVelocity);
    NMP_ASSERT(groundVelocity_importance >= 0.0f && groundVelocity_importance <= 1.0f);
    m_groundVelocityImportance = groundVelocity_importance;
  }
  NM_INLINE float getGroundVelocityImportance() const { return m_groundVelocityImportance; }
  NM_INLINE const float& getGroundVelocityImportanceRef() const { return m_groundVelocityImportance; }
  NM_INLINE const NMP::Vector3& getGroundVelocity() const { return groundVelocity; }

  NM_INLINE void setAverageSupportingLimbContactNormal(const NMP::Vector3& _averageSupportingLimbContactNormal, float averageSupportingLimbContactNormal_importance = 1.f)
  {
    averageSupportingLimbContactNormal = _averageSupportingLimbContactNormal;
    postAssignAverageSupportingLimbContactNormal(averageSupportingLimbContactNormal);
    NMP_ASSERT(averageSupportingLimbContactNormal_importance >= 0.0f && averageSupportingLimbContactNormal_importance <= 1.0f);
    m_averageSupportingLimbContactNormalImportance = averageSupportingLimbContactNormal_importance;
  }
  NM_INLINE float getAverageSupportingLimbContactNormalImportance() const { return m_averageSupportingLimbContactNormalImportance; }
  NM_INLINE const float& getAverageSupportingLimbContactNormalImportanceRef() const { return m_averageSupportingLimbContactNormalImportance; }
  NM_INLINE const NMP::Vector3& getAverageSupportingLimbContactNormal() const { return averageSupportingLimbContactNormal; }

  NM_INLINE void setStabilisePelvisAmount(const float& _stabilisePelvisAmount, float stabilisePelvisAmount_importance = 1.f)
  {
    stabilisePelvisAmount = _stabilisePelvisAmount;
    postAssignStabilisePelvisAmount(stabilisePelvisAmount);
    NMP_ASSERT(stabilisePelvisAmount_importance >= 0.0f && stabilisePelvisAmount_importance <= 1.0f);
    m_stabilisePelvisAmountImportance = stabilisePelvisAmount_importance;
  }
  NM_INLINE float getStabilisePelvisAmountImportance() const { return m_stabilisePelvisAmountImportance; }
  NM_INLINE const float& getStabilisePelvisAmountImportanceRef() const { return m_stabilisePelvisAmountImportance; }
  NM_INLINE const float& getStabilisePelvisAmount() const { return stabilisePelvisAmount; }

  NM_INLINE void setStabiliseChestAmount(const float& _stabiliseChestAmount, float stabiliseChestAmount_importance = 1.f)
  {
    stabiliseChestAmount = _stabiliseChestAmount;
    postAssignStabiliseChestAmount(stabiliseChestAmount);
    NMP_ASSERT(stabiliseChestAmount_importance >= 0.0f && stabiliseChestAmount_importance <= 1.0f);
    m_stabiliseChestAmountImportance = stabiliseChestAmount_importance;
  }
  NM_INLINE float getStabiliseChestAmountImportance() const { return m_stabiliseChestAmountImportance; }
  NM_INLINE const float& getStabiliseChestAmountImportanceRef() const { return m_stabiliseChestAmountImportance; }
  NM_INLINE const float& getStabiliseChestAmount() const { return stabiliseChestAmount; }

  enum { maxLegSupportStrengthScales = 2 };
  NM_INLINE unsigned int getMaxLegSupportStrengthScales() const { return maxLegSupportStrengthScales; }
  NM_INLINE void setLegSupportStrengthScales(unsigned int number, const float* _legSupportStrengthScales, float legSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegSupportStrengthScales);
    NMP_ASSERT(legSupportStrengthScales_importance >= 0.0f && legSupportStrengthScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legSupportStrengthScales[i] = _legSupportStrengthScales[i];
      postAssignLegSupportStrengthScales(legSupportStrengthScales[i]);
      m_legSupportStrengthScalesImportance[i] = legSupportStrengthScales_importance;
    }
  }
  NM_INLINE void setLegSupportStrengthScalesAt(unsigned int index, const float& _legSupportStrengthScales, float legSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegSupportStrengthScales);
    legSupportStrengthScales[index] = _legSupportStrengthScales;
    NMP_ASSERT(legSupportStrengthScales_importance >= 0.0f && legSupportStrengthScales_importance <= 1.0f);
    postAssignLegSupportStrengthScales(legSupportStrengthScales[index]);
    m_legSupportStrengthScalesImportance[index] = legSupportStrengthScales_importance;
  }
  NM_INLINE float getLegSupportStrengthScalesImportance(int index) const { return m_legSupportStrengthScalesImportance[index]; }
  NM_INLINE const float& getLegSupportStrengthScalesImportanceRef(int index) const { return m_legSupportStrengthScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumLegSupportStrengthScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legSupportStrengthScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegSupportStrengthScales(unsigned int index) const { NMP_ASSERT(index <= maxLegSupportStrengthScales); return legSupportStrengthScales[index]; }

  enum { maxArmSupportStrengthScales = 2 };
  NM_INLINE unsigned int getMaxArmSupportStrengthScales() const { return maxArmSupportStrengthScales; }
  NM_INLINE void setArmSupportStrengthScales(unsigned int number, const float* _armSupportStrengthScales, float armSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmSupportStrengthScales);
    NMP_ASSERT(armSupportStrengthScales_importance >= 0.0f && armSupportStrengthScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armSupportStrengthScales[i] = _armSupportStrengthScales[i];
      postAssignArmSupportStrengthScales(armSupportStrengthScales[i]);
      m_armSupportStrengthScalesImportance[i] = armSupportStrengthScales_importance;
    }
  }
  NM_INLINE void setArmSupportStrengthScalesAt(unsigned int index, const float& _armSupportStrengthScales, float armSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmSupportStrengthScales);
    armSupportStrengthScales[index] = _armSupportStrengthScales;
    NMP_ASSERT(armSupportStrengthScales_importance >= 0.0f && armSupportStrengthScales_importance <= 1.0f);
    postAssignArmSupportStrengthScales(armSupportStrengthScales[index]);
    m_armSupportStrengthScalesImportance[index] = armSupportStrengthScales_importance;
  }
  NM_INLINE float getArmSupportStrengthScalesImportance(int index) const { return m_armSupportStrengthScalesImportance[index]; }
  NM_INLINE const float& getArmSupportStrengthScalesImportanceRef(int index) const { return m_armSupportStrengthScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumArmSupportStrengthScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armSupportStrengthScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmSupportStrengthScales(unsigned int index) const { NMP_ASSERT(index <= maxArmSupportStrengthScales); return armSupportStrengthScales[index]; }

  NM_INLINE void setSpineDampingIncrease(const float& _spineDampingIncrease, float spineDampingIncrease_importance = 1.f)
  {
    spineDampingIncrease = _spineDampingIncrease;
    postAssignSpineDampingIncrease(spineDampingIncrease);
    NMP_ASSERT(spineDampingIncrease_importance >= 0.0f && spineDampingIncrease_importance <= 1.0f);
    m_spineDampingIncreaseImportance = spineDampingIncrease_importance;
  }
  NM_INLINE float getSpineDampingIncreaseImportance() const { return m_spineDampingIncreaseImportance; }
  NM_INLINE const float& getSpineDampingIncreaseImportanceRef() const { return m_spineDampingIncreaseImportance; }
  NM_INLINE const float& getSpineDampingIncrease() const { return spineDampingIncrease; }

  enum { maxAllowLegStep = 2 };
  NM_INLINE unsigned int getMaxAllowLegStep() const { return maxAllowLegStep; }
  NM_INLINE void setAllowLegStep(unsigned int number, const bool* _allowLegStep, float allowLegStep_importance = 1.f)
  {
    NMP_ASSERT(number <= maxAllowLegStep);
    NMP_ASSERT(allowLegStep_importance >= 0.0f && allowLegStep_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      allowLegStep[i] = _allowLegStep[i];
      m_allowLegStepImportance[i] = allowLegStep_importance;
    }
  }
  NM_INLINE void setAllowLegStepAt(unsigned int index, const bool& _allowLegStep, float allowLegStep_importance = 1.f)
  {
    NMP_ASSERT(index < maxAllowLegStep);
    allowLegStep[index] = _allowLegStep;
    NMP_ASSERT(allowLegStep_importance >= 0.0f && allowLegStep_importance <= 1.0f);
    m_allowLegStepImportance[index] = allowLegStep_importance;
  }
  NM_INLINE float getAllowLegStepImportance(int index) const { return m_allowLegStepImportance[index]; }
  NM_INLINE const float& getAllowLegStepImportanceRef(int index) const { return m_allowLegStepImportance[index]; }
  NM_INLINE unsigned int calculateNumAllowLegStep() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_allowLegStepImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getAllowLegStep(unsigned int index) const { NMP_ASSERT(index <= maxAllowLegStep); return allowLegStep[index]; }

  enum { maxAllowArmStep = 2 };
  NM_INLINE unsigned int getMaxAllowArmStep() const { return maxAllowArmStep; }
  NM_INLINE void setAllowArmStep(unsigned int number, const bool* _allowArmStep, float allowArmStep_importance = 1.f)
  {
    NMP_ASSERT(number <= maxAllowArmStep);
    NMP_ASSERT(allowArmStep_importance >= 0.0f && allowArmStep_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      allowArmStep[i] = _allowArmStep[i];
      m_allowArmStepImportance[i] = allowArmStep_importance;
    }
  }
  NM_INLINE void setAllowArmStepAt(unsigned int index, const bool& _allowArmStep, float allowArmStep_importance = 1.f)
  {
    NMP_ASSERT(index < maxAllowArmStep);
    allowArmStep[index] = _allowArmStep;
    NMP_ASSERT(allowArmStep_importance >= 0.0f && allowArmStep_importance <= 1.0f);
    m_allowArmStepImportance[index] = allowArmStep_importance;
  }
  NM_INLINE float getAllowArmStepImportance(int index) const { return m_allowArmStepImportance[index]; }
  NM_INLINE const float& getAllowArmStepImportanceRef(int index) const { return m_allowArmStepImportance[index]; }
  NM_INLINE unsigned int calculateNumAllowArmStep() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_allowArmStepImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getAllowArmStep(unsigned int index) const { NMP_ASSERT(index <= maxAllowArmStep); return allowArmStep[index]; }

  NM_INLINE void setEvaluateStep(const bool& _evaluateStep, float evaluateStep_importance = 1.f)
  {
    evaluateStep = _evaluateStep;
    NMP_ASSERT(evaluateStep_importance >= 0.0f && evaluateStep_importance <= 1.0f);
    m_evaluateStepImportance = evaluateStep_importance;
  }
  NM_INLINE float getEvaluateStepImportance() const { return m_evaluateStepImportance; }
  NM_INLINE const float& getEvaluateStepImportanceRef() const { return m_evaluateStepImportance; }
  NM_INLINE const bool& getEvaluateStep() const { return evaluateStep; }

  NM_INLINE void setArmsSwingOutwardsOnly(const bool& _armsSwingOutwardsOnly, float armsSwingOutwardsOnly_importance = 1.f)
  {
    armsSwingOutwardsOnly = _armsSwingOutwardsOnly;
    NMP_ASSERT(armsSwingOutwardsOnly_importance >= 0.0f && armsSwingOutwardsOnly_importance <= 1.0f);
    m_armsSwingOutwardsOnlyImportance = armsSwingOutwardsOnly_importance;
  }
  NM_INLINE float getArmsSwingOutwardsOnlyImportance() const { return m_armsSwingOutwardsOnlyImportance; }
  NM_INLINE const float& getArmsSwingOutwardsOnlyImportanceRef() const { return m_armsSwingOutwardsOnlyImportance; }
  NM_INLINE const bool& getArmsSwingOutwardsOnly() const { return armsSwingOutwardsOnly; }

  NM_INLINE void setArmsSpinInPhase(const bool& _armsSpinInPhase, float armsSpinInPhase_importance = 1.f)
  {
    armsSpinInPhase = _armsSpinInPhase;
    NMP_ASSERT(armsSpinInPhase_importance >= 0.0f && armsSpinInPhase_importance <= 1.0f);
    m_armsSpinInPhaseImportance = armsSpinInPhase_importance;
  }
  NM_INLINE float getArmsSpinInPhaseImportance() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const float& getArmsSpinInPhaseImportanceRef() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const bool& getArmsSpinInPhase() const { return armsSpinInPhase; }

protected:

  RotationRequest pelvisRotationRequest;  /// This request can be acted on by any part of the body (hint - arm swing!) to try to rotate the pelvis. 
  ER::LimbTransform desiredPelvisTM;  /// Desired spine root transform that support modules may try to reach for balance
  ER::LimbTransform desiredChestTM;  /// Desired spine end transform that support modules may try to reach for balance
  NMP::Matrix34 recoveryLegStepTarget[NetworkConstants::networkMaxNumLegs];  /// Indicate to stepping that it should try to step to recover the feet, and where to. When this
  /// gets called normally the balancer will be stable, and will have made the desired stepping leg
  /// redundant to the balance.  (Transform)
  NMP::Matrix34 recoveryArmStepTarget[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  ER::LimbTransform supportTM;  /// The centre of support position - at the moment average of the feet - weighted by the amount
  /// each foot (or whatever) is actually supporting. Orientation gives yAxis "up" (against the
  /// real gravity), xAxis "forward"
  /// Same as feedOut.supportTM
  ER::LimbTransform targetPelvisTM;  /// This is the target pelvis position that is given by the target pose offset from the 
  /// (horizontal) support TM. It has the CoM offset from the pelvis applied to it - so is 
  /// statically balanced
  NMP::Vector3 groundVelocity;  /// The velocity of the ground, if there is at least one limb supporting  (Velocity)
  NMP::Vector3 averageSupportingLimbContactNormal;  /// Average contact normal of the supporting limbs that are touching the ground  (Direction)
  float stabilisePelvisAmount;  /// How much the root of the spine should be stabilised by support (by counteracting undesired motion)  (Multiplier)
  float stabiliseChestAmount;  /// How much the end of the spine should be stabilised by support (by counteracting undesired motion)  (Multiplier)
  float legSupportStrengthScales[NetworkConstants::networkMaxNumLegs];  /// Strength scales for the arms/legs that are supporting (should also be used by stepping)  (Weight)
  float armSupportStrengthScales[NetworkConstants::networkMaxNumArms];  ///< (Weight)
  float spineDampingIncrease;  /// Increase the spine damping when balancing to reduce oscillations  (Weight)
  bool allowLegStep[NetworkConstants::networkMaxNumLegs];  /// Top-down allowing of stepping - needed to prevent both feet from stepping at once etc. 
  bool allowArmStep[NetworkConstants::networkMaxNumArms];
  bool evaluateStep;  /// Top-down evaluation of stepping - no need to evaluate if we're completely balanced, or if another 
  /// leg is already stepping
  bool armsSwingOutwardsOnly;  /// Control how the balance swing/spin the arms
  bool armsSpinInPhase;

  // post-assignment stubs
  NM_INLINE void postAssignPelvisRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRecoveryLegStepTarget(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "recoveryLegStepTarget");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRecoveryArmStepTarget(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "recoveryArmStepTarget");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGroundVelocity(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "groundVelocity");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageSupportingLimbContactNormal(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "averageSupportingLimbContactNormal");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStabilisePelvisAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "stabilisePelvisAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStabiliseChestAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "stabiliseChestAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegSupportStrengthScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "legSupportStrengthScales");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmSupportStrengthScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armSupportStrengthScales");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineDampingIncrease(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "spineDampingIncrease");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisRotationRequestImportance, 
     m_desiredPelvisTMImportance, 
     m_desiredChestTMImportance, 
     m_recoveryLegStepTargetImportance[NetworkConstants::networkMaxNumLegs], 
     m_recoveryArmStepTargetImportance[NetworkConstants::networkMaxNumArms], 
     m_supportTMImportance, 
     m_targetPelvisTMImportance, 
     m_groundVelocityImportance, 
     m_averageSupportingLimbContactNormalImportance, 
     m_stabilisePelvisAmountImportance, 
     m_stabiliseChestAmountImportance, 
     m_legSupportStrengthScalesImportance[NetworkConstants::networkMaxNumLegs], 
     m_armSupportStrengthScalesImportance[NetworkConstants::networkMaxNumArms], 
     m_spineDampingIncreaseImportance, 
     m_allowLegStepImportance[NetworkConstants::networkMaxNumLegs], 
     m_allowArmStepImportance[NetworkConstants::networkMaxNumArms], 
     m_evaluateStepImportance, 
     m_armsSwingOutwardsOnlyImportance, 
     m_armsSpinInPhaseImportance;

  friend class BodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisRotationRequestImportance() > 0.0f)
    {
      pelvisRotationRequest.validate();
    }
    {
      uint32_t numRecoveryLegStepTarget = calculateNumRecoveryLegStepTarget();
      for (uint32_t i=0; i<numRecoveryLegStepTarget; i++)
      {
        if (getRecoveryLegStepTargetImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(recoveryLegStepTarget[i]), "recoveryLegStepTarget");
        }
      }
    }
    {
      uint32_t numRecoveryArmStepTarget = calculateNumRecoveryArmStepTarget();
      for (uint32_t i=0; i<numRecoveryArmStepTarget; i++)
      {
        if (getRecoveryArmStepTargetImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(recoveryArmStepTarget[i]), "recoveryArmStepTarget");
        }
      }
    }
    if (getGroundVelocityImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(groundVelocity), "groundVelocity");
    }
    if (getAverageSupportingLimbContactNormalImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(averageSupportingLimbContactNormal), "averageSupportingLimbContactNormal");
    }
    if (getStabilisePelvisAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(stabilisePelvisAmount), "stabilisePelvisAmount");
    }
    if (getStabiliseChestAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(stabiliseChestAmount), "stabiliseChestAmount");
    }
    {
      uint32_t numLegSupportStrengthScales = calculateNumLegSupportStrengthScales();
      for (uint32_t i=0; i<numLegSupportStrengthScales; i++)
      {
        if (getLegSupportStrengthScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(legSupportStrengthScales[i]), "legSupportStrengthScales");
        }
      }
    }
    {
      uint32_t numArmSupportStrengthScales = calculateNumArmSupportStrengthScales();
      for (uint32_t i=0; i<numArmSupportStrengthScales; i++)
      {
        if (getArmSupportStrengthScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(armSupportStrengthScales[i]), "armSupportStrengthScales");
        }
      }
    }
    if (getSpineDampingIncreaseImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(spineDampingIncrease), "spineDampingIncrease");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyBalanceFeedbackInputs
 * \ingroup BodyBalance
 * \brief FeedbackInputs Overall module dealing with applying overall body balance
 *  Note that this deals with balancing on legs and arms. Where
 *  legs and arms are treated identically then legs will be stored
 *  first, and the leg/arm is called a limb.
 *
 * Data packet definition (169 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyBalanceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyBalanceFeedbackInputs));
  }

  NM_INLINE void setStepDir(const NMP::Vector3& _stepDir, float stepDir_importance = 1.f)
  {
    stepDir = _stepDir;
    postAssignStepDir(stepDir);
    NMP_ASSERT(stepDir_importance >= 0.0f && stepDir_importance <= 1.0f);
    m_stepDirImportance = stepDir_importance;
  }
  NM_INLINE float getStepDirImportance() const { return m_stepDirImportance; }
  NM_INLINE const float& getStepDirImportanceRef() const { return m_stepDirImportance; }
  NM_INLINE const NMP::Vector3& getStepDir() const { return stepDir; }

  enum { maxLimbSupportAmounts = 4 };
  NM_INLINE unsigned int getMaxLimbSupportAmounts() const { return maxLimbSupportAmounts; }
  NM_INLINE void setLimbSupportAmounts(unsigned int number, const float* _limbSupportAmounts, float limbSupportAmounts_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLimbSupportAmounts);
    NMP_ASSERT(limbSupportAmounts_importance >= 0.0f && limbSupportAmounts_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      limbSupportAmounts[i] = _limbSupportAmounts[i];
      postAssignLimbSupportAmounts(limbSupportAmounts[i]);
      m_limbSupportAmountsImportance[i] = limbSupportAmounts_importance;
    }
  }
  NM_INLINE void setLimbSupportAmountsAt(unsigned int index, const float& _limbSupportAmounts, float limbSupportAmounts_importance = 1.f)
  {
    NMP_ASSERT(index < maxLimbSupportAmounts);
    limbSupportAmounts[index] = _limbSupportAmounts;
    NMP_ASSERT(limbSupportAmounts_importance >= 0.0f && limbSupportAmounts_importance <= 1.0f);
    postAssignLimbSupportAmounts(limbSupportAmounts[index]);
    m_limbSupportAmountsImportance[index] = limbSupportAmounts_importance;
  }
  NM_INLINE float getLimbSupportAmountsImportance(int index) const { return m_limbSupportAmountsImportance[index]; }
  NM_INLINE const float& getLimbSupportAmountsImportanceRef(int index) const { return m_limbSupportAmountsImportance[index]; }
  NM_INLINE unsigned int calculateNumLimbSupportAmounts() const
  {
    for (int i=3; i>=0; --i)
    {
      if (m_limbSupportAmountsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLimbSupportAmounts(unsigned int index) const { NMP_ASSERT(index <= maxLimbSupportAmounts); return limbSupportAmounts[index]; }

  enum { maxLimbEndOnGroundAmounts = 4 };
  NM_INLINE unsigned int getMaxLimbEndOnGroundAmounts() const { return maxLimbEndOnGroundAmounts; }
  NM_INLINE void setLimbEndOnGroundAmounts(unsigned int number, const float* _limbEndOnGroundAmounts, float limbEndOnGroundAmounts_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLimbEndOnGroundAmounts);
    NMP_ASSERT(limbEndOnGroundAmounts_importance >= 0.0f && limbEndOnGroundAmounts_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      limbEndOnGroundAmounts[i] = _limbEndOnGroundAmounts[i];
      postAssignLimbEndOnGroundAmounts(limbEndOnGroundAmounts[i]);
      m_limbEndOnGroundAmountsImportance[i] = limbEndOnGroundAmounts_importance;
    }
  }
  NM_INLINE void setLimbEndOnGroundAmountsAt(unsigned int index, const float& _limbEndOnGroundAmounts, float limbEndOnGroundAmounts_importance = 1.f)
  {
    NMP_ASSERT(index < maxLimbEndOnGroundAmounts);
    limbEndOnGroundAmounts[index] = _limbEndOnGroundAmounts;
    NMP_ASSERT(limbEndOnGroundAmounts_importance >= 0.0f && limbEndOnGroundAmounts_importance <= 1.0f);
    postAssignLimbEndOnGroundAmounts(limbEndOnGroundAmounts[index]);
    m_limbEndOnGroundAmountsImportance[index] = limbEndOnGroundAmounts_importance;
  }
  NM_INLINE float getLimbEndOnGroundAmountsImportance(int index) const { return m_limbEndOnGroundAmountsImportance[index]; }
  NM_INLINE const float& getLimbEndOnGroundAmountsImportanceRef(int index) const { return m_limbEndOnGroundAmountsImportance[index]; }
  NM_INLINE unsigned int calculateNumLimbEndOnGroundAmounts() const
  {
    for (int i=3; i>=0; --i)
    {
      if (m_limbEndOnGroundAmountsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLimbEndOnGroundAmounts(unsigned int index) const { NMP_ASSERT(index <= maxLimbEndOnGroundAmounts); return limbEndOnGroundAmounts[index]; }

  enum { maxStepLengths = 4 };
  NM_INLINE unsigned int getMaxStepLengths() const { return maxStepLengths; }
  NM_INLINE void setStepLengths(unsigned int number, const float* _stepLengths, float stepLengths_importance = 1.f)
  {
    NMP_ASSERT(number <= maxStepLengths);
    NMP_ASSERT(stepLengths_importance >= 0.0f && stepLengths_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      stepLengths[i] = _stepLengths[i];
      postAssignStepLengths(stepLengths[i]);
      m_stepLengthsImportance[i] = stepLengths_importance;
    }
  }
  NM_INLINE void setStepLengthsAt(unsigned int index, const float& _stepLengths, float stepLengths_importance = 1.f)
  {
    NMP_ASSERT(index < maxStepLengths);
    stepLengths[index] = _stepLengths;
    NMP_ASSERT(stepLengths_importance >= 0.0f && stepLengths_importance <= 1.0f);
    postAssignStepLengths(stepLengths[index]);
    m_stepLengthsImportance[index] = stepLengths_importance;
  }
  NM_INLINE float getStepLengthsImportance(int index) const { return m_stepLengthsImportance[index]; }
  NM_INLINE const float& getStepLengthsImportanceRef(int index) const { return m_stepLengthsImportance[index]; }
  NM_INLINE unsigned int calculateNumStepLengths() const
  {
    for (int i=3; i>=0; --i)
    {
      if (m_stepLengthsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getStepLengths(unsigned int index) const { NMP_ASSERT(index <= maxStepLengths); return stepLengths[index]; }

  NM_INLINE void setLowerPelvisDistance(const float& _lowerPelvisDistance, float lowerPelvisDistance_importance = 1.f)
  {
    lowerPelvisDistance = _lowerPelvisDistance;
    postAssignLowerPelvisDistance(lowerPelvisDistance);
    NMP_ASSERT(lowerPelvisDistance_importance >= 0.0f && lowerPelvisDistance_importance <= 1.0f);
    m_lowerPelvisDistanceImportance = lowerPelvisDistance_importance;
  }
  NM_INLINE float getLowerPelvisDistanceImportance() const { return m_lowerPelvisDistanceImportance; }
  NM_INLINE const float& getLowerPelvisDistanceImportanceRef() const { return m_lowerPelvisDistanceImportance; }
  NM_INLINE const float& getLowerPelvisDistance() const { return lowerPelvisDistance; }

  NM_INLINE void setIsSupportedByConstraint(const bool& _isSupportedByConstraint, float isSupportedByConstraint_importance = 1.f)
  {
    isSupportedByConstraint = _isSupportedByConstraint;
    NMP_ASSERT(isSupportedByConstraint_importance >= 0.0f && isSupportedByConstraint_importance <= 1.0f);
    m_isSupportedByConstraintImportance = isSupportedByConstraint_importance;
  }
  NM_INLINE float getIsSupportedByConstraintImportance() const { return m_isSupportedByConstraintImportance; }
  NM_INLINE const float& getIsSupportedByConstraintImportanceRef() const { return m_isSupportedByConstraintImportance; }
  NM_INLINE const bool& getIsSupportedByConstraint() const { return isSupportedByConstraint; }

  enum { maxIsStepping = 4 };
  NM_INLINE unsigned int getMaxIsStepping() const { return maxIsStepping; }
  NM_INLINE void setIsStepping(unsigned int number, const bool* _isStepping, float isStepping_importance = 1.f)
  {
    NMP_ASSERT(number <= maxIsStepping);
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      isStepping[i] = _isStepping[i];
      m_isSteppingImportance[i] = isStepping_importance;
    }
  }
  NM_INLINE void setIsSteppingAt(unsigned int index, const bool& _isStepping, float isStepping_importance = 1.f)
  {
    NMP_ASSERT(index < maxIsStepping);
    isStepping[index] = _isStepping;
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    m_isSteppingImportance[index] = isStepping_importance;
  }
  NM_INLINE float getIsSteppingImportance(int index) const { return m_isSteppingImportance[index]; }
  NM_INLINE const float& getIsSteppingImportanceRef(int index) const { return m_isSteppingImportance[index]; }
  NM_INLINE unsigned int calculateNumIsStepping() const
  {
    for (int i=3; i>=0; --i)
    {
      if (m_isSteppingImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getIsStepping(unsigned int index) const { NMP_ASSERT(index <= maxIsStepping); return isStepping[index]; }

  NM_INLINE void setIsDoingRecoveryStep(const bool& _isDoingRecoveryStep, float isDoingRecoveryStep_importance = 1.f)
  {
    isDoingRecoveryStep = _isDoingRecoveryStep;
    NMP_ASSERT(isDoingRecoveryStep_importance >= 0.0f && isDoingRecoveryStep_importance <= 1.0f);
    m_isDoingRecoveryStepImportance = isDoingRecoveryStep_importance;
  }
  NM_INLINE float getIsDoingRecoveryStepImportance() const { return m_isDoingRecoveryStepImportance; }
  NM_INLINE const float& getIsDoingRecoveryStepImportanceRef() const { return m_isDoingRecoveryStepImportance; }
  NM_INLINE const bool& getIsDoingRecoveryStep() const { return isDoingRecoveryStep; }

  NM_INLINE void setIsSteppingBeingSuppressed(const bool& _isSteppingBeingSuppressed, float isSteppingBeingSuppressed_importance = 1.f)
  {
    isSteppingBeingSuppressed = _isSteppingBeingSuppressed;
    NMP_ASSERT(isSteppingBeingSuppressed_importance >= 0.0f && isSteppingBeingSuppressed_importance <= 1.0f);
    m_isSteppingBeingSuppressedImportance = isSteppingBeingSuppressed_importance;
  }
  NM_INLINE float getIsSteppingBeingSuppressedImportance() const { return m_isSteppingBeingSuppressedImportance; }
  NM_INLINE const float& getIsSteppingBeingSuppressedImportanceRef() const { return m_isSteppingBeingSuppressedImportance; }
  NM_INLINE const bool& getIsSteppingBeingSuppressed() const { return isSteppingBeingSuppressed; }

  NM_INLINE void setEnableSupportOnLegs(const bool& _enableSupportOnLegs, float enableSupportOnLegs_importance = 1.f)
  {
    enableSupportOnLegs = _enableSupportOnLegs;
    NMP_ASSERT(enableSupportOnLegs_importance >= 0.0f && enableSupportOnLegs_importance <= 1.0f);
    m_enableSupportOnLegsImportance = enableSupportOnLegs_importance;
  }
  NM_INLINE float getEnableSupportOnLegsImportance() const { return m_enableSupportOnLegsImportance; }
  NM_INLINE const float& getEnableSupportOnLegsImportanceRef() const { return m_enableSupportOnLegsImportance; }
  NM_INLINE const bool& getEnableSupportOnLegs() const { return enableSupportOnLegs; }

  NM_INLINE void setEnableSupportOnArms(const bool& _enableSupportOnArms, float enableSupportOnArms_importance = 1.f)
  {
    enableSupportOnArms = _enableSupportOnArms;
    NMP_ASSERT(enableSupportOnArms_importance >= 0.0f && enableSupportOnArms_importance <= 1.0f);
    m_enableSupportOnArmsImportance = enableSupportOnArms_importance;
  }
  NM_INLINE float getEnableSupportOnArmsImportance() const { return m_enableSupportOnArmsImportance; }
  NM_INLINE const float& getEnableSupportOnArmsImportanceRef() const { return m_enableSupportOnArmsImportance; }
  NM_INLINE const bool& getEnableSupportOnArms() const { return enableSupportOnArms; }

protected:

  NMP::Vector3 stepDir;                      ///< (Direction)
  float limbSupportAmounts[BodyBalanceData::rigMaxNumSupports];  ///< (Weight)
  float limbEndOnGroundAmounts[BodyBalanceData::rigMaxNumSupports];  ///< (Weight)
  float stepLengths[BodyBalanceData::rigMaxNumSupports];  ///< (Length)
  float lowerPelvisDistance;  /// Amount that we should lower the pelvis by to help the stepping  (Distance)
  bool isSupportedByConstraint;
  bool isStepping[BodyBalanceData::rigMaxNumSupports];  // What the stepping module is doing  
  bool isDoingRecoveryStep;  /// Indicates if any of the legs are stepping to recover pose
  bool isSteppingBeingSuppressed;
  bool enableSupportOnLegs;  /// Backup indications if we should support - needed for the first feedback pass
  bool enableSupportOnArms;

  // post-assignment stubs
  NM_INLINE void postAssignStepDir(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "stepDir");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbSupportAmounts(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "limbSupportAmounts");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbEndOnGroundAmounts(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "limbEndOnGroundAmounts");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepLengths(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "stepLengths");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerPelvisDistance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "lowerPelvisDistance");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_stepDirImportance, 
     m_limbSupportAmountsImportance[BodyBalanceData::rigMaxNumSupports], 
     m_limbEndOnGroundAmountsImportance[BodyBalanceData::rigMaxNumSupports], 
     m_stepLengthsImportance[BodyBalanceData::rigMaxNumSupports], 
     m_lowerPelvisDistanceImportance, 
     m_isSupportedByConstraintImportance, 
     m_isSteppingImportance[BodyBalanceData::rigMaxNumSupports], 
     m_isDoingRecoveryStepImportance, 
     m_isSteppingBeingSuppressedImportance, 
     m_enableSupportOnLegsImportance, 
     m_enableSupportOnArmsImportance;

  friend class BodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getStepDirImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(stepDir), "stepDir");
    }
    {
      uint32_t numLimbSupportAmounts = calculateNumLimbSupportAmounts();
      for (uint32_t i=0; i<numLimbSupportAmounts; i++)
      {
        if (getLimbSupportAmountsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(limbSupportAmounts[i]), "limbSupportAmounts");
        }
      }
    }
    {
      uint32_t numLimbEndOnGroundAmounts = calculateNumLimbEndOnGroundAmounts();
      for (uint32_t i=0; i<numLimbEndOnGroundAmounts; i++)
      {
        if (getLimbEndOnGroundAmountsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(limbEndOnGroundAmounts[i]), "limbEndOnGroundAmounts");
        }
      }
    }
    {
      uint32_t numStepLengths = calculateNumStepLengths();
      for (uint32_t i=0; i<numStepLengths; i++)
      {
        if (getStepLengthsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(stepLengths[i]), "stepLengths");
        }
      }
    }
    if (getLowerPelvisDistanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(lowerPelvisDistance), "lowerPelvisDistance");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyBalanceFeedbackOutputs
 * \ingroup BodyBalance
 * \brief FeedbackOutputs Overall module dealing with applying overall body balance
 *  Note that this deals with balancing on legs and arms. Where
 *  legs and arms are treated identically then legs will be stored
 *  first, and the leg/arm is called a limb.
 *
 * Data packet definition (101 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyBalanceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyBalanceFeedbackOutputs));
  }

  NM_INLINE void setSupportTM(const ER::LimbTransform& _supportTM, float supportTM_importance = 1.f)
  {
    supportTM = _supportTM;
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE ER::LimbTransform& startSupportTMModification()
  {
    m_supportTMImportance = -1.0f; // set invalid until stopSupportTMModification()
    return supportTM;
  }
  NM_INLINE void stopSupportTMModification(float supportTM_importance = 1.f)
  {
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE float getSupportTMImportance() const { return m_supportTMImportance; }
  NM_INLINE const float& getSupportTMImportanceRef() const { return m_supportTMImportance; }
  NM_INLINE const ER::LimbTransform& getSupportTM() const { return supportTM; }

  NM_INLINE void setSupportVelocity(const NMP::Vector3& _supportVelocity, float supportVelocity_importance = 1.f)
  {
    supportVelocity = _supportVelocity;
    postAssignSupportVelocity(supportVelocity);
    NMP_ASSERT(supportVelocity_importance >= 0.0f && supportVelocity_importance <= 1.0f);
    m_supportVelocityImportance = supportVelocity_importance;
  }
  NM_INLINE float getSupportVelocityImportance() const { return m_supportVelocityImportance; }
  NM_INLINE const float& getSupportVelocityImportanceRef() const { return m_supportVelocityImportance; }
  NM_INLINE const NMP::Vector3& getSupportVelocity() const { return supportVelocity; }

  NM_INLINE void setBalanceAmount(const float& _balanceAmount, float balanceAmount_importance = 1.f)
  {
    balanceAmount = _balanceAmount;
    postAssignBalanceAmount(balanceAmount);
    NMP_ASSERT(balanceAmount_importance >= 0.0f && balanceAmount_importance <= 1.0f);
    m_balanceAmountImportance = balanceAmount_importance;
  }
  NM_INLINE float getBalanceAmountImportance() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmountImportanceRef() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmount() const { return balanceAmount; }

  NM_INLINE float getIsSteppingImportance() const { return m_isSteppingImportance; }
  NM_INLINE const float& getIsSteppingImportanceRef() const { return m_isSteppingImportance; }
  NM_INLINE const bool& getIsStepping() const { return isStepping; }

protected:

  ER::LimbTransform supportTM;  /// The centre of support position - at the moment average of the feet - weighted by the amount
  /// each foot (or whatever) is actually supporting. Orientation gives yAxis "up" (against the
  /// real gravity), xAxis "forward"
  NMP::Vector3 supportVelocity;  /// Velocity of the support poly - note that this is not the same as the gound velocity as we may
  /// have the feet off the floor (e.g. stepping).  (Velocity)
  float balanceAmount;  /// If the body as a whole is supported (includes standing or stepping). Will be 0 when fallen.  (Weight)
  bool isStepping;  /// This indicates if the legs are currently trying to step (though will be wrong if the step
  /// output is overridden)

  // post-assignment stubs
  NM_INLINE void postAssignSupportVelocity(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "supportVelocity");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "balanceAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_supportTMImportance, 
     m_supportVelocityImportance, 
     m_balanceAmountImportance, 
     m_isSteppingImportance;

  friend class BodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSupportVelocityImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(supportVelocity), "supportVelocity");
    }
    if (getBalanceAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(balanceAmount), "balanceAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYBALANCE_DATA_H

