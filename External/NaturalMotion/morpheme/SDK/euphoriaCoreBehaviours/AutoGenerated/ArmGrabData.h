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

#ifndef NM_MDF_ARMGRAB_DATA_H
#define NM_MDF_ARMGRAB_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmGrab.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Edge.h"
#include "Types/EndConstraintControl.h"
#include "Types/GrabFailureConditions.h"
#include "Types/GrabAliveParams.h"
#include "Types/ReachWorldParameters.h"
#include "Types/LimbControl.h"

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
 * \class ArmGrabData
 * \ingroup ArmGrab
 * \brief Data coordinates the reaching out for and holding on to an object in the environment
 *
 * Data packet definition (129 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmGrabData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmGrabData));
  }

  Edge lastGrabbedEdge;  ///< Cache the previous edge, in case we want to prevent grabbing the same edge twice (optional)
  NMP::Vector3 previousGrabPoint;  ///< This arm's hand target.  (Position)
  float grabAttemptPeriod;  ///< How long has this arm been trying to grab the edge.  (TimePeriod)
  float handBehindBodyPeriod;  ///< How long has this arm's hand been "stuck" behind the back.  (TimePeriod)
  float reachImportance;  ///< How important is it to reach for the edge (rather than protect for example).  (Weight)
  float cachedEdgeT;  ///< Cache the t value along the edge to avoid drift when constrained
  bool havePreviousGrabPoint;  ///< True if the previous grab point is valid

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    lastGrabbedEdge.validate();
    NM_VALIDATOR(Vector3Valid(previousGrabPoint), "previousGrabPoint");
    NM_VALIDATOR(FloatValid(grabAttemptPeriod), "grabAttemptPeriod");
    NM_VALIDATOR(FloatValid(handBehindBodyPeriod), "handBehindBodyPeriod");
    NM_VALIDATOR(FloatNonNegative(reachImportance), "reachImportance");
    NM_VALIDATOR(FloatValid(cachedEdgeT), "cachedEdgeT");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmGrabInputs
 * \ingroup ArmGrab
 * \brief Inputs coordinates the reaching out for and holding on to an object in the environment
 *
 * Data packet definition (277 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmGrabInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmGrabInputs));
  }

  NM_INLINE void setEdgeToGrab(const Edge& _edgeToGrab, float edgeToGrab_importance = 1.f)
  {
    edgeToGrab = _edgeToGrab;
    postAssignEdgeToGrab(edgeToGrab);
    NMP_ASSERT(edgeToGrab_importance >= 0.0f && edgeToGrab_importance <= 1.0f);
    m_edgeToGrabImportance = edgeToGrab_importance;
  }
  NM_INLINE Edge& startEdgeToGrabModification()
  {
    m_edgeToGrabImportance = -1.0f; // set invalid until stopEdgeToGrabModification()
    return edgeToGrab;
  }
  NM_INLINE void stopEdgeToGrabModification(float edgeToGrab_importance = 1.f)
  {
    postAssignEdgeToGrab(edgeToGrab);
    NMP_ASSERT(edgeToGrab_importance >= 0.0f && edgeToGrab_importance <= 1.0f);
    m_edgeToGrabImportance = edgeToGrab_importance;
  }
  NM_INLINE float getEdgeToGrabImportance() const { return m_edgeToGrabImportance; }
  NM_INLINE const float& getEdgeToGrabImportanceRef() const { return m_edgeToGrabImportance; }
  NM_INLINE const Edge& getEdgeToGrab() const { return edgeToGrab; }

  NM_INLINE void setEndConstraintOptions(const EndConstraintControl& _endConstraintOptions, float endConstraintOptions_importance = 1.f)
  {
    endConstraintOptions = _endConstraintOptions;
    postAssignEndConstraintOptions(endConstraintOptions);
    NMP_ASSERT(endConstraintOptions_importance >= 0.0f && endConstraintOptions_importance <= 1.0f);
    m_endConstraintOptionsImportance = endConstraintOptions_importance;
  }
  NM_INLINE EndConstraintControl& startEndConstraintOptionsModification()
  {
    m_endConstraintOptionsImportance = -1.0f; // set invalid until stopEndConstraintOptionsModification()
    return endConstraintOptions;
  }
  NM_INLINE void stopEndConstraintOptionsModification(float endConstraintOptions_importance = 1.f)
  {
    postAssignEndConstraintOptions(endConstraintOptions);
    NMP_ASSERT(endConstraintOptions_importance >= 0.0f && endConstraintOptions_importance <= 1.0f);
    m_endConstraintOptionsImportance = endConstraintOptions_importance;
  }
  NM_INLINE float getEndConstraintOptionsImportance() const { return m_endConstraintOptionsImportance; }
  NM_INLINE const float& getEndConstraintOptionsImportanceRef() const { return m_endConstraintOptionsImportance; }
  NM_INLINE const EndConstraintControl& getEndConstraintOptions() const { return endConstraintOptions; }

  NM_INLINE void setChestVelocity(const NMP::Vector3& _chestVelocity, float chestVelocity_importance = 1.f)
  {
    chestVelocity = _chestVelocity;
    postAssignChestVelocity(chestVelocity);
    NMP_ASSERT(chestVelocity_importance >= 0.0f && chestVelocity_importance <= 1.0f);
    m_chestVelocityImportance = chestVelocity_importance;
  }
  NM_INLINE float getChestVelocityImportance() const { return m_chestVelocityImportance; }
  NM_INLINE const float& getChestVelocityImportanceRef() const { return m_chestVelocityImportance; }
  NM_INLINE const NMP::Vector3& getChestVelocity() const { return chestVelocity; }

  NM_INLINE void setFailureConditions(const GrabFailureConditions& _failureConditions, float failureConditions_importance = 1.f)
  {
    failureConditions = _failureConditions;
    postAssignFailureConditions(failureConditions);
    NMP_ASSERT(failureConditions_importance >= 0.0f && failureConditions_importance <= 1.0f);
    m_failureConditionsImportance = failureConditions_importance;
  }
  NM_INLINE float getFailureConditionsImportance() const { return m_failureConditionsImportance; }
  NM_INLINE const float& getFailureConditionsImportanceRef() const { return m_failureConditionsImportance; }
  NM_INLINE const GrabFailureConditions& getFailureConditions() const { return failureConditions; }

  NM_INLINE void setAliveParams(const GrabAliveParams& _aliveParams, float aliveParams_importance = 1.f)
  {
    aliveParams = _aliveParams;
    postAssignAliveParams(aliveParams);
    NMP_ASSERT(aliveParams_importance >= 0.0f && aliveParams_importance <= 1.0f);
    m_aliveParamsImportance = aliveParams_importance;
  }
  NM_INLINE float getAliveParamsImportance() const { return m_aliveParamsImportance; }
  NM_INLINE const float& getAliveParamsImportanceRef() const { return m_aliveParamsImportance; }
  NM_INLINE const GrabAliveParams& getAliveParams() const { return aliveParams; }

  NM_INLINE void setImportance(const float& _importance, float importance_importance = 1.f)
  {
    importance = _importance;
    postAssignImportance(importance);
    NMP_ASSERT(importance_importance >= 0.0f && importance_importance <= 1.0f);
    m_importanceImportance = importance_importance;
  }
  NM_INLINE float getImportanceImportance() const { return m_importanceImportance; }
  NM_INLINE const float& getImportanceImportanceRef() const { return m_importanceImportance; }
  NM_INLINE const float& getImportance() const { return importance; }

  NM_INLINE void setNumConstrainedArms(const uint16_t& _numConstrainedArms, float numConstrainedArms_importance = 1.f)
  {
    numConstrainedArms = _numConstrainedArms;
    NMP_ASSERT(numConstrainedArms_importance >= 0.0f && numConstrainedArms_importance <= 1.0f);
    m_numConstrainedArmsImportance = numConstrainedArms_importance;
  }
  NM_INLINE float getNumConstrainedArmsImportance() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const float& getNumConstrainedArmsImportanceRef() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const uint16_t& getNumConstrainedArms() const { return numConstrainedArms; }

  NM_INLINE void setEdgeIsObstructed(const bool& _edgeIsObstructed, float edgeIsObstructed_importance = 1.f)
  {
    edgeIsObstructed = _edgeIsObstructed;
    NMP_ASSERT(edgeIsObstructed_importance >= 0.0f && edgeIsObstructed_importance <= 1.0f);
    m_edgeIsObstructedImportance = edgeIsObstructed_importance;
  }
  NM_INLINE float getEdgeIsObstructedImportance() const { return m_edgeIsObstructedImportance; }
  NM_INLINE const float& getEdgeIsObstructedImportanceRef() const { return m_edgeIsObstructedImportance; }
  NM_INLINE const bool& getEdgeIsObstructed() const { return edgeIsObstructed; }

  NM_INLINE void setHasTwistedAround(const bool& _hasTwistedAround, float hasTwistedAround_importance = 1.f)
  {
    hasTwistedAround = _hasTwistedAround;
    NMP_ASSERT(hasTwistedAround_importance >= 0.0f && hasTwistedAround_importance <= 1.0f);
    m_hasTwistedAroundImportance = hasTwistedAround_importance;
  }
  NM_INLINE float getHasTwistedAroundImportance() const { return m_hasTwistedAroundImportance; }
  NM_INLINE const float& getHasTwistedAroundImportanceRef() const { return m_hasTwistedAroundImportance; }
  NM_INLINE const bool& getHasTwistedAround() const { return hasTwistedAround; }

  NM_INLINE void setIsHanging(const bool& _isHanging, float isHanging_importance = 1.f)
  {
    isHanging = _isHanging;
    NMP_ASSERT(isHanging_importance >= 0.0f && isHanging_importance <= 1.0f);
    m_isHangingImportance = isHanging_importance;
  }
  NM_INLINE float getIsHangingImportance() const { return m_isHangingImportance; }
  NM_INLINE const float& getIsHangingImportanceRef() const { return m_isHangingImportance; }
  NM_INLINE const bool& getIsHanging() const { return isHanging; }

protected:

  Edge edgeToGrab;  ///< Edge passed in by Grab module.
  EndConstraintControl endConstraintOptions;  ///< Configures the physical constraint.
  NMP::Vector3 chestVelocity;  ///< Smoothed value already calculated by GrabDetection and passed in here  (Velocity)
  GrabFailureConditions failureConditions;  ///< Configures the conditions for aborting a hold attempt.
  GrabAliveParams aliveParams;  ///< Configures behaviour while hanging (e.g. "pullUps")
  float importance;  ///< Allows game to manipulate hold importance for individual arms.
  uint16_t numConstrainedArms;  ///< Total number of currently constrained hands.
  bool edgeIsObstructed;
  bool hasTwistedAround;  ///< Is the character not facing the edge anymore?
  bool isHanging;  ///< Is the character stably hanging (as opposed to still falling).

  // post-assignment stubs
  NM_INLINE void postAssignEdgeToGrab(const Edge& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndConstraintOptions(const EndConstraintControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestVelocity(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "chestVelocity");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFailureConditions(const GrabFailureConditions& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAliveParams(const GrabAliveParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignImportance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "importance");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_edgeToGrabImportance, 
     m_endConstraintOptionsImportance, 
     m_chestVelocityImportance, 
     m_failureConditionsImportance, 
     m_aliveParamsImportance, 
     m_importanceImportance, 
     m_numConstrainedArmsImportance, 
     m_edgeIsObstructedImportance, 
     m_hasTwistedAroundImportance, 
     m_isHangingImportance;

  friend class ArmGrab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEdgeToGrabImportance() > 0.0f)
    {
      edgeToGrab.validate();
    }
    if (getEndConstraintOptionsImportance() > 0.0f)
    {
      endConstraintOptions.validate();
    }
    if (getChestVelocityImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(chestVelocity), "chestVelocity");
    }
    if (getFailureConditionsImportance() > 0.0f)
    {
      failureConditions.validate();
    }
    if (getAliveParamsImportance() > 0.0f)
    {
      aliveParams.validate();
    }
    if (getImportanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(importance), "importance");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmGrabOutputs
 * \ingroup ArmGrab
 * \brief Outputs coordinates the reaching out for and holding on to an object in the environment
 *
 * Data packet definition (588 bytes, 608 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmGrabOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmGrabOutputs));
  }

  NM_INLINE void setLimbControl(const LimbControl& _limbControl, float limbControl_importance = 1.f)
  {
    limbControl = _limbControl;
    postAssignLimbControl(limbControl);
    NMP_ASSERT(limbControl_importance >= 0.0f && limbControl_importance <= 1.0f);
    m_limbControlImportance = limbControl_importance;
  }
  NM_INLINE LimbControl& startLimbControlModification()
  {
    m_limbControlImportance = -1.0f; // set invalid until stopLimbControlModification()
    return limbControl;
  }
  NM_INLINE void stopLimbControlModification(float limbControl_importance = 1.f)
  {
    postAssignLimbControl(limbControl);
    NMP_ASSERT(limbControl_importance >= 0.0f && limbControl_importance <= 1.0f);
    m_limbControlImportance = limbControl_importance;
  }
  NM_INLINE float getLimbControlImportance() const { return m_limbControlImportance; }
  NM_INLINE const float& getLimbControlImportanceRef() const { return m_limbControlImportance; }
  NM_INLINE const LimbControl& getLimbControl() const { return limbControl; }

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

  NM_INLINE void setReachParams(const ReachWorldParameters& _reachParams, float reachParams_importance = 1.f)
  {
    reachParams = _reachParams;
    postAssignReachParams(reachParams);
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    m_reachParamsImportance = reachParams_importance;
  }
  NM_INLINE ReachWorldParameters& startReachParamsModification()
  {
    m_reachParamsImportance = -1.0f; // set invalid until stopReachParamsModification()
    return reachParams;
  }
  NM_INLINE void stopReachParamsModification(float reachParams_importance = 1.f)
  {
    postAssignReachParams(reachParams);
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    m_reachParamsImportance = reachParams_importance;
  }
  NM_INLINE float getReachParamsImportance() const { return m_reachParamsImportance; }
  NM_INLINE const float& getReachParamsImportanceRef() const { return m_reachParamsImportance; }
  NM_INLINE const ReachWorldParameters& getReachParams() const { return reachParams; }

  NM_INLINE void setHoldStrengthScale(const float& _holdStrengthScale, float holdStrengthScale_importance = 1.f)
  {
    holdStrengthScale = _holdStrengthScale;
    postAssignHoldStrengthScale(holdStrengthScale);
    NMP_ASSERT(holdStrengthScale_importance >= 0.0f && holdStrengthScale_importance <= 1.0f);
    m_holdStrengthScaleImportance = holdStrengthScale_importance;
  }
  NM_INLINE float getHoldStrengthScaleImportance() const { return m_holdStrengthScaleImportance; }
  NM_INLINE const float& getHoldStrengthScaleImportanceRef() const { return m_holdStrengthScaleImportance; }
  NM_INLINE const float& getHoldStrengthScale() const { return holdStrengthScale; }

  NM_INLINE void setBreakageOpportunity(const float& _breakageOpportunity, float breakageOpportunity_importance = 1.f)
  {
    breakageOpportunity = _breakageOpportunity;
    postAssignBreakageOpportunity(breakageOpportunity);
    NMP_ASSERT(breakageOpportunity_importance >= 0.0f && breakageOpportunity_importance <= 1.0f);
    m_breakageOpportunityImportance = breakageOpportunity_importance;
  }
  NM_INLINE float getBreakageOpportunityImportance() const { return m_breakageOpportunityImportance; }
  NM_INLINE const float& getBreakageOpportunityImportanceRef() const { return m_breakageOpportunityImportance; }
  NM_INLINE const float& getBreakageOpportunity() const { return breakageOpportunity; }

protected:

  LimbControl limbControl;  ///< Specifies the hand target (for ArmReachForWorld).
  EndConstraintControl endConstraint;  ///< Configures the physical constraint.
  ReachWorldParameters reachParams;  ///< Configure the type of reach used by Hold.
  float holdStrengthScale;  /// Arm strength, incorporating fatigue. 1 when just constrained, 0 when maxHoldTime is reached, 
  /// ramping down linearly in between.  (Weight)
  float breakageOpportunity;  /// The higher this value, the more likely this arm's constraint is destroyed if other arms are still holding  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignLimbControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignReachParams(const ReachWorldParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHoldStrengthScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "holdStrengthScale");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBreakageOpportunity(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "breakageOpportunity");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_limbControlImportance, 
     m_endConstraintImportance, 
     m_reachParamsImportance, 
     m_holdStrengthScaleImportance, 
     m_breakageOpportunityImportance;

  friend class ArmGrab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLimbControlImportance() > 0.0f)
    {
      limbControl.validate();
    }
    if (getEndConstraintImportance() > 0.0f)
    {
      endConstraint.validate();
    }
    if (getReachParamsImportance() > 0.0f)
    {
      reachParams.validate();
    }
    if (getHoldStrengthScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(holdStrengthScale), "holdStrengthScale");
    }
    if (getBreakageOpportunityImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(breakageOpportunity), "breakageOpportunity");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmGrabFeedbackInputs
 * \ingroup ArmGrab
 * \brief FeedbackInputs coordinates the reaching out for and holding on to an object in the environment
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmGrabFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmGrabFeedbackInputs));
  }

  NM_INLINE void setTimeHeldProportion(const float& _timeHeldProportion, float timeHeldProportion_importance = 1.f)
  {
    timeHeldProportion = _timeHeldProportion;
    postAssignTimeHeldProportion(timeHeldProportion);
    NMP_ASSERT(timeHeldProportion_importance >= 0.0f && timeHeldProportion_importance <= 1.0f);
    m_timeHeldProportionImportance = timeHeldProportion_importance;
  }
  NM_INLINE float getTimeHeldProportionImportance() const { return m_timeHeldProportionImportance; }
  NM_INLINE const float& getTimeHeldProportionImportanceRef() const { return m_timeHeldProportionImportance; }
  NM_INLINE const float& getTimeHeldProportion() const { return timeHeldProportion; }

protected:

  float timeHeldProportion;  ///< Ratio of maxHoldTime that this arm has been constrained.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignTimeHeldProportion(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "timeHeldProportion");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_timeHeldProportionImportance;

  friend class ArmGrab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTimeHeldProportionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(timeHeldProportion), "timeHeldProportion");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmGrabFeedbackOutputs
 * \ingroup ArmGrab
 * \brief FeedbackOutputs coordinates the reaching out for and holding on to an object in the environment
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmGrabFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmGrabFeedbackOutputs));
  }

  NM_INLINE void setIsHighPriority(const bool& _isHighPriority, float isHighPriority_importance = 1.f)
  {
    isHighPriority = _isHighPriority;
    NMP_ASSERT(isHighPriority_importance >= 0.0f && isHighPriority_importance <= 1.0f);
    m_isHighPriorityImportance = isHighPriority_importance;
  }
  NM_INLINE float getIsHighPriorityImportance() const { return m_isHighPriorityImportance; }
  NM_INLINE const float& getIsHighPriorityImportanceRef() const { return m_isHighPriorityImportance; }
  NM_INLINE const bool& getIsHighPriority() const { return isHighPriority; }

protected:

  bool isHighPriority;  ///< If true, suppresses the shield behaviour.

  // importance values
  float
     m_isHighPriorityImportance;

  friend class ArmGrab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMGRAB_DATA_H

