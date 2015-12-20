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

#ifndef NM_MDF_HAZARDRESPONSE_DATA_H
#define NM_MDF_HAZARDRESPONSE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HazardResponse.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/ShieldParameters.h"
#include "Types/Hazard.h"
#include "Types/SpatialTarget.h"
#include "Types/BraceHazard.h"
#include "Types/DodgeHazard.h"

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
 * \class HazardResponseData
 * \ingroup HazardResponse
 * \brief Data respond to hazard alerts by calling for brace and avoid modules to activate, and specifying how strongly
 *  to shield if there is a high danger due to a high angular velocity of the COM, a high momentum hazards
 *  or a high tangential velocity along a big object. If shieldHazard is set, the others hazard are not.
 *
 * Data packet definition (53 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardResponseData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardResponseData));
  }

  NMP::Vector3 lastImpactPos;                ///< (Position)
  NMP::Vector3 lastHazardVel;                ///< (Velocity)
  int64_t lastHazardID;
  float lastHazardDanger;                    ///< (Weight)
  float lastTimeToImpact;                    ///< (TimePeriod)
  float placementImportance;  ///< Used for hysteresis on enabling the arms placement  (Weight)
  bool wasDoingShieldInUrgency;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(lastImpactPos), "lastImpactPos");
    NM_VALIDATOR(Vector3Valid(lastHazardVel), "lastHazardVel");
    NM_VALIDATOR(FloatNonNegative(lastHazardDanger), "lastHazardDanger");
    NM_VALIDATOR(FloatValid(lastTimeToImpact), "lastTimeToImpact");
    NM_VALIDATOR(FloatNonNegative(placementImportance), "placementImportance");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardResponseInputs
 * \ingroup HazardResponse
 * \brief Inputs respond to hazard alerts by calling for brace and avoid modules to activate, and specifying how strongly
 *  to shield if there is a high danger due to a high angular velocity of the COM, a high momentum hazards
 *  or a high tangential velocity along a big object. If shieldHazard is set, the others hazard are not.
 *
 * Data packet definition (48 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardResponseInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardResponseInputs));
  }

  NM_INLINE void setShieldParameters(const ShieldParameters& _shieldParameters, float shieldParameters_importance = 1.f)
  {
    shieldParameters = _shieldParameters;
    postAssignShieldParameters(shieldParameters);
    NMP_ASSERT(shieldParameters_importance >= 0.0f && shieldParameters_importance <= 1.0f);
    m_shieldParametersImportance = shieldParameters_importance;
  }
  NM_INLINE float getShieldParametersImportance() const { return m_shieldParametersImportance; }
  NM_INLINE const float& getShieldParametersImportanceRef() const { return m_shieldParametersImportance; }
  NM_INLINE const ShieldParameters& getShieldParameters() const { return shieldParameters; }

  NM_INLINE void setHeadAvoidWeight(const float& _headAvoidWeight, float headAvoidWeight_importance = 1.f)
  {
    headAvoidWeight = _headAvoidWeight;
    postAssignHeadAvoidWeight(headAvoidWeight);
    NMP_ASSERT(headAvoidWeight_importance >= 0.0f && headAvoidWeight_importance <= 1.0f);
    m_headAvoidWeightImportance = headAvoidWeight_importance;
  }
  NM_INLINE float getHeadAvoidWeightImportance() const { return m_headAvoidWeightImportance; }
  NM_INLINE const float& getHeadAvoidWeightImportanceRef() const { return m_headAvoidWeightImportance; }
  NM_INLINE const float& getHeadAvoidWeight() const { return headAvoidWeight; }

  NM_INLINE void setHeadDodgeWeight(const float& _headDodgeWeight, float headDodgeWeight_importance = 1.f)
  {
    headDodgeWeight = _headDodgeWeight;
    postAssignHeadDodgeWeight(headDodgeWeight);
    NMP_ASSERT(headDodgeWeight_importance >= 0.0f && headDodgeWeight_importance <= 1.0f);
    m_headDodgeWeightImportance = headDodgeWeight_importance;
  }
  NM_INLINE float getHeadDodgeWeightImportance() const { return m_headDodgeWeightImportance; }
  NM_INLINE const float& getHeadDodgeWeightImportanceRef() const { return m_headDodgeWeightImportance; }
  NM_INLINE const float& getHeadDodgeWeight() const { return headDodgeWeight; }

  NM_INLINE void setBraceWeight(const float& _braceWeight, float braceWeight_importance = 1.f)
  {
    braceWeight = _braceWeight;
    postAssignBraceWeight(braceWeight);
    NMP_ASSERT(braceWeight_importance >= 0.0f && braceWeight_importance <= 1.0f);
    m_braceWeightImportance = braceWeight_importance;
  }
  NM_INLINE float getBraceWeightImportance() const { return m_braceWeightImportance; }
  NM_INLINE const float& getBraceWeightImportanceRef() const { return m_braceWeightImportance; }
  NM_INLINE const float& getBraceWeight() const { return braceWeight; }

  NM_INLINE void setArmsPlacementWeight(const float& _armsPlacementWeight, float armsPlacementWeight_importance = 1.f)
  {
    armsPlacementWeight = _armsPlacementWeight;
    postAssignArmsPlacementWeight(armsPlacementWeight);
    NMP_ASSERT(armsPlacementWeight_importance >= 0.0f && armsPlacementWeight_importance <= 1.0f);
    m_armsPlacementWeightImportance = armsPlacementWeight_importance;
  }
  NM_INLINE float getArmsPlacementWeightImportance() const { return m_armsPlacementWeightImportance; }
  NM_INLINE const float& getArmsPlacementWeightImportanceRef() const { return m_armsPlacementWeightImportance; }
  NM_INLINE const float& getArmsPlacementWeight() const { return armsPlacementWeight; }

protected:

  ShieldParameters shieldParameters;
  float headAvoidWeight;  // Flags for which sub modules to weight  (Weight)
  float headDodgeWeight;                     ///< (Weight)
  float braceWeight;                         ///< (Weight)
  float armsPlacementWeight;                 ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignShieldParameters(const ShieldParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadAvoidWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "headAvoidWeight");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadDodgeWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "headDodgeWeight");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBraceWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "braceWeight");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmsPlacementWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armsPlacementWeight");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_shieldParametersImportance, 
     m_headAvoidWeightImportance, 
     m_headDodgeWeightImportance, 
     m_braceWeightImportance, 
     m_armsPlacementWeightImportance;

  friend class HazardResponse_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getShieldParametersImportance() > 0.0f)
    {
      shieldParameters.validate();
    }
    if (getHeadAvoidWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(headAvoidWeight), "headAvoidWeight");
    }
    if (getHeadDodgeWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(headDodgeWeight), "headDodgeWeight");
    }
    if (getBraceWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(braceWeight), "braceWeight");
    }
    if (getArmsPlacementWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(armsPlacementWeight), "armsPlacementWeight");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardResponseOutputs
 * \ingroup HazardResponse
 * \brief Outputs respond to hazard alerts by calling for brace and avoid modules to activate, and specifying how strongly
 *  to shield if there is a high danger due to a high angular velocity of the COM, a high momentum hazards
 *  or a high tangential velocity along a big object. If shieldHazard is set, the others hazard are not.
 *
 * Data packet definition (476 bytes, 480 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardResponseOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardResponseOutputs));
  }

  NM_INLINE void setBraceHazard(const BraceHazard& _braceHazard, float braceHazard_importance = 1.f)
  {
    braceHazard = _braceHazard;
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE BraceHazard& startBraceHazardModification()
  {
    m_braceHazardImportance = -1.0f; // set invalid until stopBraceHazardModification()
    return braceHazard;
  }
  NM_INLINE void stopBraceHazardModification(float braceHazard_importance = 1.f)
  {
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE float getBraceHazardImportance() const { return m_braceHazardImportance; }
  NM_INLINE const float& getBraceHazardImportanceRef() const { return m_braceHazardImportance; }
  NM_INLINE const BraceHazard& getBraceHazard() const { return braceHazard; }

  NM_INLINE void setDodgeHazard(const DodgeHazard& _dodgeHazard, float dodgeHazard_importance = 1.f)
  {
    dodgeHazard = _dodgeHazard;
    postAssignDodgeHazard(dodgeHazard);
    NMP_ASSERT(dodgeHazard_importance >= 0.0f && dodgeHazard_importance <= 1.0f);
    m_dodgeHazardImportance = dodgeHazard_importance;
  }
  NM_INLINE float getDodgeHazardImportance() const { return m_dodgeHazardImportance; }
  NM_INLINE const float& getDodgeHazardImportanceRef() const { return m_dodgeHazardImportance; }
  NM_INLINE const DodgeHazard& getDodgeHazard() const { return dodgeHazard; }

  NM_INLINE void setAvoidTarget(const SpatialTarget& _avoidTarget, float avoidTarget_importance = 1.f)
  {
    avoidTarget = _avoidTarget;
    postAssignAvoidTarget(avoidTarget);
    NMP_ASSERT(avoidTarget_importance >= 0.0f && avoidTarget_importance <= 1.0f);
    m_avoidTargetImportance = avoidTarget_importance;
  }
  NM_INLINE float getAvoidTargetImportance() const { return m_avoidTargetImportance; }
  NM_INLINE const float& getAvoidTargetImportanceRef() const { return m_avoidTargetImportance; }
  NM_INLINE const SpatialTarget& getAvoidTarget() const { return avoidTarget; }

  NM_INLINE void setPlacementTarget(const SpatialTarget& _placementTarget, float placementTarget_importance = 1.f)
  {
    placementTarget = _placementTarget;
    postAssignPlacementTarget(placementTarget);
    NMP_ASSERT(placementTarget_importance >= 0.0f && placementTarget_importance <= 1.0f);
    m_placementTargetImportance = placementTarget_importance;
  }
  NM_INLINE float getPlacementTargetImportance() const { return m_placementTargetImportance; }
  NM_INLINE const float& getPlacementTargetImportanceRef() const { return m_placementTargetImportance; }
  NM_INLINE const SpatialTarget& getPlacementTarget() const { return placementTarget; }

  NM_INLINE void setShieldDirectionToHazard(const NMP::Vector3& _shieldDirectionToHazard, float shieldDirectionToHazard_importance = 1.f)
  {
    shieldDirectionToHazard = _shieldDirectionToHazard;
    postAssignShieldDirectionToHazard(shieldDirectionToHazard);
    NMP_ASSERT(shieldDirectionToHazard_importance >= 0.0f && shieldDirectionToHazard_importance <= 1.0f);
    m_shieldDirectionToHazardImportance = shieldDirectionToHazard_importance;
  }
  NM_INLINE float getShieldDirectionToHazardImportance() const { return m_shieldDirectionToHazardImportance; }
  NM_INLINE const float& getShieldDirectionToHazardImportanceRef() const { return m_shieldDirectionToHazardImportance; }
  NM_INLINE const NMP::Vector3& getShieldDirectionToHazard() const { return shieldDirectionToHazard; }

  NM_INLINE void setInstabilityPreparation(const float& _instabilityPreparation, float instabilityPreparation_importance = 1.f)
  {
    instabilityPreparation = _instabilityPreparation;
    postAssignInstabilityPreparation(instabilityPreparation);
    NMP_ASSERT(instabilityPreparation_importance >= 0.0f && instabilityPreparation_importance <= 1.0f);
    m_instabilityPreparationImportance = instabilityPreparation_importance;
  }
  NM_INLINE float getInstabilityPreparationImportance() const { return m_instabilityPreparationImportance; }
  NM_INLINE const float& getInstabilityPreparationImportanceRef() const { return m_instabilityPreparationImportance; }
  NM_INLINE const float& getInstabilityPreparation() const { return instabilityPreparation; }

protected:

  BraceHazard braceHazard;  ///< Control the bracing and cushioning
  DodgeHazard dodgeHazard;  ///< Control the head dodging laterally
  SpatialTarget avoidTarget;  ///< Control head's avoidance of nearby objects that are potential dangers
  SpatialTarget placementTarget;  ///< Controls armsPlacement on nearby moving objects. Without this the brace has no persistence
  NMP::Vector3 shieldDirectionToHazard;  ///< in world space  (Direction)
  float instabilityPreparation;  ///< This value is the magnitude of an instability, and the importance is 1/(1+ time to instability)  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignBraceHazard(const BraceHazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDodgeHazard(const DodgeHazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvoidTarget(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPlacementTarget(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignShieldDirectionToHazard(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "shieldDirectionToHazard");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignInstabilityPreparation(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "instabilityPreparation");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_braceHazardImportance, 
     m_dodgeHazardImportance, 
     m_avoidTargetImportance, 
     m_placementTargetImportance, 
     m_shieldDirectionToHazardImportance, 
     m_instabilityPreparationImportance;

  friend class HazardResponse_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBraceHazardImportance() > 0.0f)
    {
      braceHazard.validate();
    }
    if (getDodgeHazardImportance() > 0.0f)
    {
      dodgeHazard.validate();
    }
    if (getAvoidTargetImportance() > 0.0f)
    {
      avoidTarget.validate();
    }
    if (getPlacementTargetImportance() > 0.0f)
    {
      placementTarget.validate();
    }
    if (getShieldDirectionToHazardImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(shieldDirectionToHazard), "shieldDirectionToHazard");
    }
    if (getInstabilityPreparationImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(instabilityPreparation), "instabilityPreparation");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardResponseFeedbackInputs
 * \ingroup HazardResponse
 * \brief FeedbackInputs respond to hazard alerts by calling for brace and avoid modules to activate, and specifying how strongly
 *  to shield if there is a high danger due to a high angular velocity of the COM, a high momentum hazards
 *  or a high tangential velocity along a big object. If shieldHazard is set, the others hazard are not.
 *
 * Data packet definition (429 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardResponseFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardResponseFeedbackInputs));
  }

  NM_INLINE void setHazard(const Hazard& _hazard, float hazard_importance = 1.f)
  {
    hazard = _hazard;
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE Hazard& startHazardModification()
  {
    m_hazardImportance = -1.0f; // set invalid until stopHazardModification()
    return hazard;
  }
  NM_INLINE void stopHazardModification(float hazard_importance = 1.f)
  {
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE float getHazardImportance() const { return m_hazardImportance; }
  NM_INLINE const float& getHazardImportanceRef() const { return m_hazardImportance; }
  NM_INLINE const Hazard& getHazard() const { return hazard; }

  NM_INLINE void setPossibleFutureHazard(const SpatialTarget& _possibleFutureHazard, float possibleFutureHazard_importance = 1.f)
  {
    possibleFutureHazard = _possibleFutureHazard;
    postAssignPossibleFutureHazard(possibleFutureHazard);
    NMP_ASSERT(possibleFutureHazard_importance >= 0.0f && possibleFutureHazard_importance <= 1.0f);
    m_possibleFutureHazardImportance = possibleFutureHazard_importance;
  }
  NM_INLINE float getPossibleFutureHazardImportance() const { return m_possibleFutureHazardImportance; }
  NM_INLINE const float& getPossibleFutureHazardImportanceRef() const { return m_possibleFutureHazardImportance; }
  NM_INLINE const SpatialTarget& getPossibleFutureHazard() const { return possibleFutureHazard; }

  NM_INLINE void setHazardsNotBraceable(const bool& _hazardsNotBraceable, float hazardsNotBraceable_importance = 1.f)
  {
    hazardsNotBraceable = _hazardsNotBraceable;
    NMP_ASSERT(hazardsNotBraceable_importance >= 0.0f && hazardsNotBraceable_importance <= 1.0f);
    m_hazardsNotBraceableImportance = hazardsNotBraceable_importance;
  }
  NM_INLINE float getHazardsNotBraceableImportance() const { return m_hazardsNotBraceableImportance; }
  NM_INLINE const float& getHazardsNotBraceableImportanceRef() const { return m_hazardsNotBraceableImportance; }
  NM_INLINE const bool& getHazardsNotBraceable() const { return hazardsNotBraceable; }

protected:

  Hazard hazard;  ///< The predicted incoming hazard
  SpatialTarget possibleFutureHazard;  ///< Used by headAvoid and armsPlacement, i.e. hazards not on a collision course
  bool hazardsNotBraceable;  ///< Used by shield

  // post-assignment stubs
  NM_INLINE void postAssignHazard(const Hazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPossibleFutureHazard(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_hazardImportance, 
     m_possibleFutureHazardImportance, 
     m_hazardsNotBraceableImportance;

  friend class HazardResponse_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHazardImportance() > 0.0f)
    {
      hazard.validate();
    }
    if (getPossibleFutureHazardImportance() > 0.0f)
    {
      possibleFutureHazard.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HAZARDRESPONSE_DATA_H

