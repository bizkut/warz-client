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

#ifndef NM_MDF_LEG_DATA_H
#define NM_MDF_LEG_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/Leg.module"

// external types
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/LimbControl.h"
#include "Types/EndConstraintControl.h"

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
 * \class LegData
 * \ingroup Leg
 * \brief Data All controllers and sensors that are specific to just the legs
 *
 * Data packet definition (60 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegData));
  }

  ER::DimensionalScaling dimensionalScaling;
  int32_t limbRigIndex;  ///< index into all the rig's limbs
  int32_t limbNetworkIndex;  ///< index into all the network's limbs
  int32_t childIndex;  ///< index into network's array of legs
  float length;  /// _Current_ leg length as a sum of all the joint-joint distances, ignoring the offset from the root joint  (Length)
  float collidingTime;  ///< +ve for time we've been colliding, -ve for time we've been not colliding  (TimePeriod)
  float endGroundCollidingTime;  /// +ve for time the foot has been colliding with ground, -ve for time we've been not colliding  (TimePeriod)
  float endOnGroundAmount;  /// Amount the foot is on the ground - stays positive for a short while after the foot leaves the ground  (Weight)
  float normalStiffness;  ///< Usual stiffness, e.g. for lifting and placing the foot  (Stiffness)
  float normalDampingRatio;  ///< 1 = critical damping  (DampingRatio)
  float normalDriveCompensation;  ///< Larger is more controlled, less spongey  (Compensation)
  float previousExternalSupportAmount;  ///< Used for getting the collided flag correct on first frame  (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(length), "length");
    NM_VALIDATOR(FloatValid(collidingTime), "collidingTime");
    NM_VALIDATOR(FloatValid(endGroundCollidingTime), "endGroundCollidingTime");
    NM_VALIDATOR(FloatNonNegative(endOnGroundAmount), "endOnGroundAmount");
    NM_VALIDATOR(FloatNonNegative(normalStiffness), "normalStiffness");
    NM_VALIDATOR(FloatNonNegative(normalDampingRatio), "normalDampingRatio");
    NM_VALIDATOR(FloatNonNegative(normalDriveCompensation), "normalDriveCompensation");
    NM_VALIDATOR(FloatNonNegative(previousExternalSupportAmount), "previousExternalSupportAmount");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegInputs
 * \ingroup Leg
 * \brief Inputs All controllers and sensors that are specific to just the legs
 *
 * Data packet definition (74 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegInputs));
  }

  NM_INLINE void setCentreOfHips(const NMP::Vector3& _centreOfHips, float centreOfHips_importance = 1.f)
  {
    centreOfHips = _centreOfHips;
    postAssignCentreOfHips(centreOfHips);
    NMP_ASSERT(centreOfHips_importance >= 0.0f && centreOfHips_importance <= 1.0f);
    m_centreOfHipsImportance = centreOfHips_importance;
  }
  NM_INLINE float getCentreOfHipsImportance() const { return m_centreOfHipsImportance; }
  NM_INLINE const float& getCentreOfHipsImportanceRef() const { return m_centreOfHipsImportance; }
  NM_INLINE const NMP::Vector3& getCentreOfHips() const { return centreOfHips; }

  NM_INLINE void setCentreOfSupport(const NMP::Vector3& _centreOfSupport, float centreOfSupport_importance = 1.f)
  {
    centreOfSupport = _centreOfSupport;
    postAssignCentreOfSupport(centreOfSupport);
    NMP_ASSERT(centreOfSupport_importance >= 0.0f && centreOfSupport_importance <= 1.0f);
    m_centreOfSupportImportance = centreOfSupport_importance;
  }
  NM_INLINE float getCentreOfSupportImportance() const { return m_centreOfSupportImportance; }
  NM_INLINE const float& getCentreOfSupportImportanceRef() const { return m_centreOfSupportImportance; }
  NM_INLINE const NMP::Vector3& getCentreOfSupport() const { return centreOfSupport; }

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

  NM_INLINE void setShouldBrace(const bool& _shouldBrace, float shouldBrace_importance = 1.f)
  {
    shouldBrace = _shouldBrace;
    NMP_ASSERT(shouldBrace_importance >= 0.0f && shouldBrace_importance <= 1.0f);
    m_shouldBraceImportance = shouldBrace_importance;
  }
  NM_INLINE float getShouldBraceImportance() const { return m_shouldBraceImportance; }
  NM_INLINE const float& getShouldBraceImportanceRef() const { return m_shouldBraceImportance; }
  NM_INLINE const bool& getShouldBrace() const { return shouldBrace; }

  NM_INLINE void setIsBodyBarFeetContacting(const bool& _isBodyBarFeetContacting, float isBodyBarFeetContacting_importance = 1.f)
  {
    isBodyBarFeetContacting = _isBodyBarFeetContacting;
    NMP_ASSERT(isBodyBarFeetContacting_importance >= 0.0f && isBodyBarFeetContacting_importance <= 1.0f);
    m_isBodyBarFeetContactingImportance = isBodyBarFeetContacting_importance;
  }
  NM_INLINE float getIsBodyBarFeetContactingImportance() const { return m_isBodyBarFeetContactingImportance; }
  NM_INLINE const float& getIsBodyBarFeetContactingImportanceRef() const { return m_isBodyBarFeetContactingImportance; }
  NM_INLINE const bool& getIsBodyBarFeetContacting() const { return isBodyBarFeetContacting; }

protected:

  NMP::Vector3 centreOfHips;  ///< Calculated from averaging the feedOut.hipPositions  (Position)
  NMP::Vector3 centreOfSupport;  ///< Average of feet weighted by support amount  (Position)
  float limbStiffnessReduction;  // 1 is full reduction, so no stiffness  (Multiplier)
  float limbControlCompensationReduction;    ///< (Multiplier)
  float limbDampingRatioReduction;           ///< (Multiplier)
  bool shouldBrace;  ///< Calculated from the BraceChooser, to choose which legs are best for bracing with
  bool isBodyBarFeetContacting;  ///< Any body part apart from the feet, contacting an external object

  // post-assignment stubs
  NM_INLINE void postAssignCentreOfHips(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "centreOfHips");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCentreOfSupport(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "centreOfSupport");
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
     m_centreOfHipsImportance, 
     m_centreOfSupportImportance, 
     m_limbStiffnessReductionImportance, 
     m_limbControlCompensationReductionImportance, 
     m_limbDampingRatioReductionImportance, 
     m_shouldBraceImportance, 
     m_isBodyBarFeetContactingImportance;

  friend class Leg_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCentreOfHipsImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(centreOfHips), "centreOfHips");
    }
    if (getCentreOfSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(centreOfSupport), "centreOfSupport");
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
 * \class LegOutputs
 * \ingroup Leg
 * \brief Outputs All controllers and sensors that are specific to just the legs
 *
 * Data packet definition (881 bytes, 896 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegOutputs));
  }

  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

  NM_INLINE float getWinnerTakesAllImportance() const { return m_winnerTakesAllImportance; }
  NM_INLINE const float& getWinnerTakesAllImportanceRef() const { return m_winnerTakesAllImportance; }
  NM_INLINE const LimbControl& getWinnerTakesAll() const { return winnerTakesAll; }

  NM_INLINE void setEndConstraint(const EndConstraintControl& _endConstraint, float endConstraint_importance = 1.f)
  {
    endConstraint = _endConstraint;
    postAssignEndConstraint(endConstraint);
    NMP_ASSERT(endConstraint_importance >= 0.0f && endConstraint_importance <= 1.0f);
    m_endConstraintImportance = endConstraint_importance;
  }
  NM_INLINE EndConstraintControl& startEndConstraintModification()
  {
    m_endConstraintImportance = -1.0f; // set invalid until stopEndConstraintModification()
    return endConstraint;
  }
  NM_INLINE void stopEndConstraintModification(float endConstraint_importance = 1.f)
  {
    postAssignEndConstraint(endConstraint);
    NMP_ASSERT(endConstraint_importance >= 0.0f && endConstraint_importance <= 1.0f);
    m_endConstraintImportance = endConstraint_importance;
  }
  NM_INLINE float getEndConstraintImportance() const { return m_endConstraintImportance; }
  NM_INLINE const float& getEndConstraintImportanceRef() const { return m_endConstraintImportance; }
  NM_INLINE const EndConstraintControl& getEndConstraint() const { return endConstraint; }

  NM_INLINE float getUseFullAnkleRangeImportance() const { return m_useFullAnkleRangeImportance; }
  NM_INLINE const float& getUseFullAnkleRangeImportanceRef() const { return m_useFullAnkleRangeImportance; }
  NM_INLINE const bool& getUseFullAnkleRange() const { return useFullAnkleRange; }

protected:

  LimbControl control;  ///< IK control of the leg
  LimbControl winnerTakesAll;  ///< This is a temporary output to get rid of junctions feeding into junctions
  EndConstraintControl endConstraint;  ///< Controls grabbing of the foot onto objects
  bool useFullAnkleRange;  ///< Makes ankles only rotate when from specified modules, such as step and support

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

  NM_INLINE void postAssignEndConstraint(const EndConstraintControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_winnerTakesAllImportance, 
     m_endConstraintImportance, 
     m_useFullAnkleRangeImportance;

  friend class Leg_Con;

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
    if (getEndConstraintImportance() > 0.0f)
    {
      endConstraint.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegFeedbackOutputs
 * \ingroup Leg
 * \brief FeedbackOutputs All controllers and sensors that are specific to just the legs
 *
 * Data packet definition (51 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegFeedbackOutputs));
  }

  NM_INLINE void setHipPosition(const NMP::Vector3& _hipPosition, float hipPosition_importance = 1.f)
  {
    hipPosition = _hipPosition;
    postAssignHipPosition(hipPosition);
    NMP_ASSERT(hipPosition_importance >= 0.0f && hipPosition_importance <= 1.0f);
    m_hipPositionImportance = hipPosition_importance;
  }
  NM_INLINE float getHipPositionImportance() const { return m_hipPositionImportance; }
  NM_INLINE const float& getHipPositionImportanceRef() const { return m_hipPositionImportance; }
  NM_INLINE const NMP::Vector3& getHipPosition() const { return hipPosition; }

  NM_INLINE void setCollidingTime(const float& _collidingTime, float collidingTime_importance = 1.f)
  {
    collidingTime = _collidingTime;
    postAssignCollidingTime(collidingTime);
    NMP_ASSERT(collidingTime_importance >= 0.0f && collidingTime_importance <= 1.0f);
    m_collidingTimeImportance = collidingTime_importance;
  }
  NM_INLINE float getCollidingTimeImportance() const { return m_collidingTimeImportance; }
  NM_INLINE const float& getCollidingTimeImportanceRef() const { return m_collidingTimeImportance; }
  NM_INLINE const float& getCollidingTime() const { return collidingTime; }

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

  NM_INLINE void setLimbBarEndIsColliding(const bool& _limbBarEndIsColliding, float limbBarEndIsColliding_importance = 1.f)
  {
    limbBarEndIsColliding = _limbBarEndIsColliding;
    NMP_ASSERT(limbBarEndIsColliding_importance >= 0.0f && limbBarEndIsColliding_importance <= 1.0f);
    m_limbBarEndIsCollidingImportance = limbBarEndIsColliding_importance;
  }
  NM_INLINE float getLimbBarEndIsCollidingImportance() const { return m_limbBarEndIsCollidingImportance; }
  NM_INLINE const float& getLimbBarEndIsCollidingImportanceRef() const { return m_limbBarEndIsCollidingImportance; }
  NM_INLINE const bool& getLimbBarEndIsColliding() const { return limbBarEndIsColliding; }

protected:

  NMP::Vector3 hipPosition;  ///< Used for calculating an average of hips  (Position)
  float collidingTime;  ///< Time any part of the leg is in contact  (TimePeriod)
  float endOnGroundAmount;  ///< Timed ramp of whether foot is contacting an external object  (Weight)
  bool inContact;  ///< Whether any part of the whole leg is in contact
  bool supportedByConstraint;  ///< Whether any part of the whole leg is supported by a constraint
  bool limbBarEndIsColliding;  ///< Whether any part of the whole leg is except the foot is in contact

  // post-assignment stubs
  NM_INLINE void postAssignHipPosition(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "hipPosition");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCollidingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "collidingTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "endOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_hipPositionImportance, 
     m_collidingTimeImportance, 
     m_endOnGroundAmountImportance, 
     m_inContactImportance, 
     m_supportedByConstraintImportance, 
     m_limbBarEndIsCollidingImportance;

  friend class Leg_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHipPositionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(hipPosition), "hipPosition");
    }
    if (getCollidingTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(collidingTime), "collidingTime");
    }
    if (getEndOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(endOnGroundAmount), "endOnGroundAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEG_DATA_H

