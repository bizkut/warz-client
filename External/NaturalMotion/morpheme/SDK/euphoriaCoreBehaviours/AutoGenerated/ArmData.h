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

#ifndef NM_MDF_ARM_DATA_H
#define NM_MDF_ARM_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/Arm.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erDimensionalScaling.h"

// known types
#include "Types/LimbControl.h"
#include "Types/EndConstraintControl.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

// constants
#include "NetworkConstants.h"

#include <string.h>


namespace MR { class InstanceDebugInterface; } 
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
 * \class ArmData
 * \ingroup Arm
 * \brief Data All controllers and sensors that are specific to just the arms.
 *
 * Data packet definition (56 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmData));
  }

  ER::DimensionalScaling dimensionalScaling;
  int32_t limbRigIndex;  ///< index into all the rig's limbs.
  int32_t limbNetworkIndex;  ///< index into all the network's limbs.
  int32_t childIndex;  ///< index into network's array of arms.
  float length;  ///< Length of arm when extended.  (Length)
  float normalStiffness;  ///< Usual value, e.g. for pointing or lifting a ball.  (Stiffness)
  float normalDampingRatio;  ///< 1 is critical damping.  (DampingRatio)
  float normalDriveCompensation;  ///< Greater gives more internal control / less wobbly.  (Compensation)
  float collidingTime;  /// +ve for time we've been colliding, -ve for time we've been not colliding.  (TimePeriod)
  float endGroundCollidingTime;  /// +ve for time the hand has been colliding with ground, -ve for time we've been not colliding.  (TimePeriod)
  float endOnGroundAmount;  /// Amount the hand is on the ground - stays positive for a short while after the hand leaves the ground.  (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(length), "length");
    NM_VALIDATOR(FloatNonNegative(normalStiffness), "normalStiffness");
    NM_VALIDATOR(FloatNonNegative(normalDampingRatio), "normalDampingRatio");
    NM_VALIDATOR(FloatNonNegative(normalDriveCompensation), "normalDriveCompensation");
    NM_VALIDATOR(FloatValid(collidingTime), "collidingTime");
    NM_VALIDATOR(FloatValid(endGroundCollidingTime), "endGroundCollidingTime");
    NM_VALIDATOR(FloatNonNegative(endOnGroundAmount), "endOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmInputs
 * \ingroup Arm
 * \brief Inputs All controllers and sensors that are specific to just the arms.
 *
 * Data packet definition (54 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmInputs));
  }

  NM_INLINE void setCentreOfShoulders(const NMP::Vector3& _centreOfShoulders, float centreOfShoulders_importance = 1.f)
  {
    centreOfShoulders = _centreOfShoulders;
    postAssignCentreOfShoulders(centreOfShoulders);
    NMP_ASSERT(centreOfShoulders_importance >= 0.0f && centreOfShoulders_importance <= 1.0f);
    m_centreOfShouldersImportance = centreOfShoulders_importance;
  }
  NM_INLINE float getCentreOfShouldersImportance() const { return m_centreOfShouldersImportance; }
  NM_INLINE const float& getCentreOfShouldersImportanceRef() const { return m_centreOfShouldersImportance; }
  NM_INLINE const NMP::Vector3& getCentreOfShoulders() const { return centreOfShoulders; }

  NM_INLINE void setLimbStiffnessReduction(const float& _limbStiffnessReduction, float limbStiffnessReduction_importance = 1.f)
  {
    limbStiffnessReduction = _limbStiffnessReduction;
    postAssignLimbStiffnessReduction(limbStiffnessReduction);
    NMP_ASSERT(limbStiffnessReduction_importance >= 0.0f && limbStiffnessReduction_importance <= 1.0f);
    m_limbStiffnessReductionImportance = limbStiffnessReduction_importance;
  }
  NM_INLINE float getLimbStiffnessReductionImportance() const { return m_limbStiffnessReductionImportance; }
  NM_INLINE const float& getLimbStiffnessReductionImportanceRef() const { return m_limbStiffnessReductionImportance; }
  NM_INLINE const float& getLimbStiffnessReduction() const { return limbStiffnessReduction; }

  NM_INLINE void setLimbControlCompensationReduction(const float& _limbControlCompensationReduction, float limbControlCompensationReduction_importance = 1.f)
  {
    limbControlCompensationReduction = _limbControlCompensationReduction;
    postAssignLimbControlCompensationReduction(limbControlCompensationReduction);
    NMP_ASSERT(limbControlCompensationReduction_importance >= 0.0f && limbControlCompensationReduction_importance <= 1.0f);
    m_limbControlCompensationReductionImportance = limbControlCompensationReduction_importance;
  }
  NM_INLINE float getLimbControlCompensationReductionImportance() const { return m_limbControlCompensationReductionImportance; }
  NM_INLINE const float& getLimbControlCompensationReductionImportanceRef() const { return m_limbControlCompensationReductionImportance; }
  NM_INLINE const float& getLimbControlCompensationReduction() const { return limbControlCompensationReduction; }

  NM_INLINE void setLimbDampingRatioReduction(const float& _limbDampingRatioReduction, float limbDampingRatioReduction_importance = 1.f)
  {
    limbDampingRatioReduction = _limbDampingRatioReduction;
    postAssignLimbDampingRatioReduction(limbDampingRatioReduction);
    NMP_ASSERT(limbDampingRatioReduction_importance >= 0.0f && limbDampingRatioReduction_importance <= 1.0f);
    m_limbDampingRatioReductionImportance = limbDampingRatioReduction_importance;
  }
  NM_INLINE float getLimbDampingRatioReductionImportance() const { return m_limbDampingRatioReductionImportance; }
  NM_INLINE const float& getLimbDampingRatioReductionImportanceRef() const { return m_limbDampingRatioReductionImportance; }
  NM_INLINE const float& getLimbDampingRatioReduction() const { return limbDampingRatioReduction; }

  NM_INLINE void setIsBodyBarFeetContacting(const bool& _isBodyBarFeetContacting, float isBodyBarFeetContacting_importance = 1.f)
  {
    isBodyBarFeetContacting = _isBodyBarFeetContacting;
    NMP_ASSERT(isBodyBarFeetContacting_importance >= 0.0f && isBodyBarFeetContacting_importance <= 1.0f);
    m_isBodyBarFeetContactingImportance = isBodyBarFeetContacting_importance;
  }
  NM_INLINE float getIsBodyBarFeetContactingImportance() const { return m_isBodyBarFeetContactingImportance; }
  NM_INLINE const float& getIsBodyBarFeetContactingImportanceRef() const { return m_isBodyBarFeetContactingImportance; }
  NM_INLINE const bool& getIsBodyBarFeetContacting() const { return isBodyBarFeetContacting; }

  NM_INLINE void setShouldBrace(const bool& _shouldBrace, float shouldBrace_importance = 1.f)
  {
    shouldBrace = _shouldBrace;
    NMP_ASSERT(shouldBrace_importance >= 0.0f && shouldBrace_importance <= 1.0f);
    m_shouldBraceImportance = shouldBrace_importance;
  }
  NM_INLINE float getShouldBraceImportance() const { return m_shouldBraceImportance; }
  NM_INLINE const float& getShouldBraceImportanceRef() const { return m_shouldBraceImportance; }
  NM_INLINE const bool& getShouldBrace() const { return shouldBrace; }

protected:

  NMP::Vector3 centreOfShoulders;  ///< Passed in to provide a point to brace underneath.  (Position)
  float limbStiffnessReduction;  ///< 0 is no reduction, 1 is full reduction so no stiffness.  (Multiplier)
  float limbControlCompensationReduction;  ///< 0 is no reduction, 1 is full reduction.  (Multiplier)
  float limbDampingRatioReduction;  ///< 0 is no reduction, 1 is full reduction.  (Multiplier)
  bool isBodyBarFeetContacting;  ///< The swing needs to know if the body (excluding feet) is in contact.
  bool shouldBrace;  ///< Will only brace if this limb is chosen to do so here.

  // post-assignment stubs
  NM_INLINE void postAssignCentreOfShoulders(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "centreOfShoulders");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "limbStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "limbControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "limbDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_centreOfShouldersImportance, 
     m_limbStiffnessReductionImportance, 
     m_limbControlCompensationReductionImportance, 
     m_limbDampingRatioReductionImportance, 
     m_isBodyBarFeetContactingImportance, 
     m_shouldBraceImportance;

  friend class Arm_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCentreOfShouldersImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(centreOfShoulders), "centreOfShoulders");
    }
    if (getLimbStiffnessReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(limbStiffnessReduction), "limbStiffnessReduction");
    }
    if (getLimbControlCompensationReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(limbControlCompensationReduction), "limbControlCompensationReduction");
    }
    if (getLimbDampingRatioReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(limbDampingRatioReduction), "limbDampingRatioReduction");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmOutputs
 * \ingroup Arm
 * \brief Outputs All controllers and sensors that are specific to just the arms.
 *
 * Data packet definition (1668 bytes, 1696 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmOutputs));
  }

  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

  NM_INLINE float getWinnerTakesAllImportance() const { return m_winnerTakesAllImportance; }
  NM_INLINE const float& getWinnerTakesAllImportanceRef() const { return m_winnerTakesAllImportance; }
  NM_INLINE const LimbControl& getWinnerTakesAll() const { return winnerTakesAll; }

  NM_INLINE float getWtaBracePlacementSittingImportance() const { return m_wtaBracePlacementSittingImportance; }
  NM_INLINE const float& getWtaBracePlacementSittingImportanceRef() const { return m_wtaBracePlacementSittingImportance; }
  NM_INLINE const LimbControl& getWtaBracePlacementSitting() const { return wtaBracePlacementSitting; }

  NM_INLINE float getWtaSittingStepStandingSupportImportance() const { return m_wtaSittingStepStandingSupportImportance; }
  NM_INLINE const float& getWtaSittingStepStandingSupportImportanceRef() const { return m_wtaSittingStepStandingSupportImportance; }
  NM_INLINE const LimbControl& getWtaSittingStepStandingSupport() const { return wtaSittingStepStandingSupport; }

  NM_INLINE float getEndConstraintImportance() const { return m_endConstraintImportance; }
  NM_INLINE const float& getEndConstraintImportanceRef() const { return m_endConstraintImportance; }
  NM_INLINE const EndConstraintControl& getEndConstraint() const { return endConstraint; }

  NM_INLINE float getHoldBreakageOpportunityImportance() const { return m_holdBreakageOpportunityImportance; }
  NM_INLINE const float& getHoldBreakageOpportunityImportanceRef() const { return m_holdBreakageOpportunityImportance; }
  NM_INLINE const float& getHoldBreakageOpportunity() const { return holdBreakageOpportunity; }

  NM_INLINE float getWristLimitReductionImportance() const { return m_wristLimitReductionImportance; }
  NM_INLINE const float& getWristLimitReductionImportanceRef() const { return m_wristLimitReductionImportance; }
  NM_INLINE const float& getWristLimitReduction() const { return wristLimitReduction; }

protected:

  LimbControl control;  ///< Default control of the arm.
  LimbControl winnerTakesAll;  ///< This is a temporary output to get rid of junctions feeding into junctions.
  LimbControl wtaBracePlacementSitting;
  LimbControl wtaSittingStepStandingSupport;
  EndConstraintControl endConstraint;  ///< Controls how to pin the hand to an object.
  float holdBreakageOpportunity;  // The higher this is, the more probably that a constraint can be broken as long as other arms are still holding on.  (Weight)
  float wristLimitReduction;  ///< If its 1 then IK cannot bend the wrist.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWinnerTakesAll(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWtaBracePlacementSitting(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWtaSittingStepStandingSupport(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndConstraint(const EndConstraintControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHoldBreakageOpportunity(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "holdBreakageOpportunity");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWristLimitReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "wristLimitReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_winnerTakesAllImportance, 
     m_wtaBracePlacementSittingImportance, 
     m_wtaSittingStepStandingSupportImportance, 
     m_endConstraintImportance, 
     m_holdBreakageOpportunityImportance, 
     m_wristLimitReductionImportance;

  friend class Arm_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
    if (getWinnerTakesAllImportance() > 0.0f)
    {
      winnerTakesAll.validate();
    }
    if (getWtaBracePlacementSittingImportance() > 0.0f)
    {
      wtaBracePlacementSitting.validate();
    }
    if (getWtaSittingStepStandingSupportImportance() > 0.0f)
    {
      wtaSittingStepStandingSupport.validate();
    }
    if (getEndConstraintImportance() > 0.0f)
    {
      endConstraint.validate();
    }
    if (getHoldBreakageOpportunityImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(holdBreakageOpportunity), "holdBreakageOpportunity");
    }
    if (getWristLimitReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(wristLimitReduction), "wristLimitReduction");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmFeedbackOutputs
 * \ingroup Arm
 * \brief FeedbackOutputs All controllers and sensors that are specific to just the arms.
 *
 * Data packet definition (46 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmFeedbackOutputs));
  }

  NM_INLINE void setBasePosition(const NMP::Vector3& _basePosition, float basePosition_importance = 1.f)
  {
    basePosition = _basePosition;
    postAssignBasePosition(basePosition);
    NMP_ASSERT(basePosition_importance >= 0.0f && basePosition_importance <= 1.0f);
    m_basePositionImportance = basePosition_importance;
  }
  NM_INLINE float getBasePositionImportance() const { return m_basePositionImportance; }
  NM_INLINE const float& getBasePositionImportanceRef() const { return m_basePositionImportance; }
  NM_INLINE const NMP::Vector3& getBasePosition() const { return basePosition; }

  NM_INLINE void setEndOnGroundAmount(const float& _endOnGroundAmount, float endOnGroundAmount_importance = 1.f)
  {
    endOnGroundAmount = _endOnGroundAmount;
    postAssignEndOnGroundAmount(endOnGroundAmount);
    NMP_ASSERT(endOnGroundAmount_importance >= 0.0f && endOnGroundAmount_importance <= 1.0f);
    m_endOnGroundAmountImportance = endOnGroundAmount_importance;
  }
  NM_INLINE float getEndOnGroundAmountImportance() const { return m_endOnGroundAmountImportance; }
  NM_INLINE const float& getEndOnGroundAmountImportanceRef() const { return m_endOnGroundAmountImportance; }
  NM_INLINE const float& getEndOnGroundAmount() const { return endOnGroundAmount; }

  NM_INLINE void setEndGroundCollidingTime(const float& _endGroundCollidingTime, float endGroundCollidingTime_importance = 1.f)
  {
    endGroundCollidingTime = _endGroundCollidingTime;
    postAssignEndGroundCollidingTime(endGroundCollidingTime);
    NMP_ASSERT(endGroundCollidingTime_importance >= 0.0f && endGroundCollidingTime_importance <= 1.0f);
    m_endGroundCollidingTimeImportance = endGroundCollidingTime_importance;
  }
  NM_INLINE float getEndGroundCollidingTimeImportance() const { return m_endGroundCollidingTimeImportance; }
  NM_INLINE const float& getEndGroundCollidingTimeImportanceRef() const { return m_endGroundCollidingTimeImportance; }
  NM_INLINE const float& getEndGroundCollidingTime() const { return endGroundCollidingTime; }

  NM_INLINE void setInContact(const bool& _inContact, float inContact_importance = 1.f)
  {
    inContact = _inContact;
    NMP_ASSERT(inContact_importance >= 0.0f && inContact_importance <= 1.0f);
    m_inContactImportance = inContact_importance;
  }
  NM_INLINE float getInContactImportance() const { return m_inContactImportance; }
  NM_INLINE const float& getInContactImportanceRef() const { return m_inContactImportance; }
  NM_INLINE const bool& getInContact() const { return inContact; }

  NM_INLINE float getSupportedByConstraintImportance() const { return m_supportedByConstraintImportance; }
  NM_INLINE const float& getSupportedByConstraintImportanceRef() const { return m_supportedByConstraintImportance; }
  NM_INLINE const bool& getSupportedByConstraint() const { return supportedByConstraint; }

protected:

  NMP::Vector3 basePosition;  ///< Roughly the shoulder.  (Position)
  float endOnGroundAmount;  ///< Ramps up and down over time.  (Weight)
  float endGroundCollidingTime;  /// +ve for time the hand has been colliding with ground, -ve for time we've been not colliding.  (TimePeriod)
  bool inContact;
  bool supportedByConstraint;  ///< Whether any part of the whole arm is supported by a constraint.

  // post-assignment stubs
  NM_INLINE void postAssignBasePosition(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "basePosition");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "endOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndGroundCollidingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "endGroundCollidingTime");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_basePositionImportance, 
     m_endOnGroundAmountImportance, 
     m_endGroundCollidingTimeImportance, 
     m_inContactImportance, 
     m_supportedByConstraintImportance;

  friend class Arm_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBasePositionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(basePosition), "basePosition");
    }
    if (getEndOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(endOnGroundAmount), "endOnGroundAmount");
    }
    if (getEndGroundCollidingTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(endGroundCollidingTime), "endGroundCollidingTime");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARM_DATA_H

