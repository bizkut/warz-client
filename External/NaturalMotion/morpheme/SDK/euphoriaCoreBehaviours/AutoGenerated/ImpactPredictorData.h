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

#ifndef NM_MDF_IMPACTPREDICTOR_DATA_H
#define NM_MDF_IMPACTPREDICTOR_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ImpactPredictor.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/Hazard.h"
#include "Types/SphereTrajectory.h"
#include "Types/ProtectParameters.h"
#include "Types/ObjectFiltering.h"
#include "Types/PatchSet.h"
#include "Types/SpatialTarget.h"
#include "Types/TargetRequest.h"

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
 * \class ImpactPredictorData
 * \ingroup ImpactPredictor
 * \brief Data the job of this module is to alert of (pass up) the single most dangerous hazard (if any) that will affect a defined protect sphere
 *
 * Data packet definition (456 bytes, 480 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ImpactPredictorData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ImpactPredictorData));
  }

  Hazard hazard;  ///< So the update can access the hazard calculated in the feedback
  SphereTrajectory protectSphere;  ///< Pos, vel, acc of protection sphere around the chest
  NMP::Vector3 firstSeenPos;  ///< First seen position of hazard when became aware of it  (Velocity)
  float hazardUrgency;  ///< Unitless 1/time to impact
  float lookTime;  ///< So character doesn't look for too long at a former hazard  (TimePeriod)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    hazard.validate();
    protectSphere.validate();
    NM_VALIDATOR(Vector3Valid(firstSeenPos), "firstSeenPos");
    NM_VALIDATOR(FloatValid(hazardUrgency), "hazardUrgency");
    NM_VALIDATOR(FloatValid(lookTime), "lookTime");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ImpactPredictorInputs
 * \ingroup ImpactPredictor
 * \brief Inputs the job of this module is to alert of (pass up) the single most dangerous hazard (if any) that will affect a defined protect sphere
 *
 * Data packet definition (838 bytes, 864 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ImpactPredictorInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ImpactPredictorInputs));
  }

  NM_INLINE void setPotentialUserHazards(const PatchSet& _potentialUserHazards, float potentialUserHazards_importance = 1.f)
  {
    potentialUserHazards = _potentialUserHazards;
    NMP_ASSERT(potentialUserHazards_importance >= 0.0f && potentialUserHazards_importance <= 1.0f);
    m_potentialUserHazardsImportance = potentialUserHazards_importance;
  }
  NM_INLINE PatchSet& startPotentialUserHazardsModification()
  {
    m_potentialUserHazardsImportance = -1.0f; // set invalid until stopPotentialUserHazardsModification()
    return potentialUserHazards;
  }
  NM_INLINE void stopPotentialUserHazardsModification(float potentialUserHazards_importance = 1.f)
  {
    NMP_ASSERT(potentialUserHazards_importance >= 0.0f && potentialUserHazards_importance <= 1.0f);
    m_potentialUserHazardsImportance = potentialUserHazards_importance;
  }
  NM_INLINE float getPotentialUserHazardsImportance() const { return m_potentialUserHazardsImportance; }
  NM_INLINE const float& getPotentialUserHazardsImportanceRef() const { return m_potentialUserHazardsImportance; }
  NM_INLINE const PatchSet& getPotentialUserHazards() const { return potentialUserHazards; }

  NM_INLINE void setProtectParameters(const ProtectParameters& _protectParameters, float protectParameters_importance = 1.f)
  {
    protectParameters = _protectParameters;
    postAssignProtectParameters(protectParameters);
    NMP_ASSERT(protectParameters_importance >= 0.0f && protectParameters_importance <= 1.0f);
    m_protectParametersImportance = protectParameters_importance;
  }
  NM_INLINE float getProtectParametersImportance() const { return m_protectParametersImportance; }
  NM_INLINE const float& getProtectParametersImportanceRef() const { return m_protectParametersImportance; }
  NM_INLINE const ProtectParameters& getProtectParameters() const { return protectParameters; }

  NM_INLINE void setProtectHazardFiltering(const ObjectFiltering& _protectHazardFiltering, float protectHazardFiltering_importance = 1.f)
  {
    protectHazardFiltering = _protectHazardFiltering;
    NMP_ASSERT(protectHazardFiltering_importance >= 0.0f && protectHazardFiltering_importance <= 1.0f);
    m_protectHazardFilteringImportance = protectHazardFiltering_importance;
  }
  NM_INLINE float getProtectHazardFilteringImportance() const { return m_protectHazardFilteringImportance; }
  NM_INLINE const float& getProtectHazardFilteringImportanceRef() const { return m_protectHazardFilteringImportance; }
  NM_INLINE const ObjectFiltering& getProtectHazardFiltering() const { return protectHazardFiltering; }

  NM_INLINE void setIgnoreVerticalPredictionAmount(const float& _ignoreVerticalPredictionAmount, float ignoreVerticalPredictionAmount_importance = 1.f)
  {
    ignoreVerticalPredictionAmount = _ignoreVerticalPredictionAmount;
    postAssignIgnoreVerticalPredictionAmount(ignoreVerticalPredictionAmount);
    NMP_ASSERT(ignoreVerticalPredictionAmount_importance >= 0.0f && ignoreVerticalPredictionAmount_importance <= 1.0f);
    m_ignoreVerticalPredictionAmountImportance = ignoreVerticalPredictionAmount_importance;
  }
  NM_INLINE float getIgnoreVerticalPredictionAmountImportance() const { return m_ignoreVerticalPredictionAmountImportance; }
  NM_INLINE const float& getIgnoreVerticalPredictionAmountImportanceRef() const { return m_ignoreVerticalPredictionAmountImportance; }
  NM_INLINE const float& getIgnoreVerticalPredictionAmount() const { return ignoreVerticalPredictionAmount; }

  NM_INLINE void setProbeRadius(const float& _probeRadius, float probeRadius_importance = 1.f)
  {
    probeRadius = _probeRadius;
    postAssignProbeRadius(probeRadius);
    NMP_ASSERT(probeRadius_importance >= 0.0f && probeRadius_importance <= 1.0f);
    m_probeRadiusImportance = probeRadius_importance;
  }
  NM_INLINE float getProbeRadiusImportance() const { return m_probeRadiusImportance; }
  NM_INLINE const float& getProbeRadiusImportanceRef() const { return m_probeRadiusImportance; }
  NM_INLINE const float& getProbeRadius() const { return probeRadius; }

  NM_INLINE void setVelocityIsControlled(const bool& _velocityIsControlled, float velocityIsControlled_importance = 1.f)
  {
    velocityIsControlled = _velocityIsControlled;
    NMP_ASSERT(velocityIsControlled_importance >= 0.0f && velocityIsControlled_importance <= 1.0f);
    m_velocityIsControlledImportance = velocityIsControlled_importance;
  }
  NM_INLINE float getVelocityIsControlledImportance() const { return m_velocityIsControlledImportance; }
  NM_INLINE const float& getVelocityIsControlledImportanceRef() const { return m_velocityIsControlledImportance; }
  NM_INLINE const bool& getVelocityIsControlled() const { return velocityIsControlled; }

  NM_INLINE void setIgnoreNonUserHazards(const bool& _ignoreNonUserHazards, float ignoreNonUserHazards_importance = 1.f)
  {
    ignoreNonUserHazards = _ignoreNonUserHazards;
    NMP_ASSERT(ignoreNonUserHazards_importance >= 0.0f && ignoreNonUserHazards_importance <= 1.0f);
    m_ignoreNonUserHazardsImportance = ignoreNonUserHazards_importance;
  }
  NM_INLINE float getIgnoreNonUserHazardsImportance() const { return m_ignoreNonUserHazardsImportance; }
  NM_INLINE const float& getIgnoreNonUserHazardsImportanceRef() const { return m_ignoreNonUserHazardsImportance; }
  NM_INLINE const bool& getIgnoreNonUserHazards() const { return ignoreNonUserHazards; }

protected:

  PatchSet potentialUserHazards;  ///< Potential hazards provided by the UserHazard module.
  ProtectParameters protectParameters;  ///< How to protect
  ObjectFiltering protectHazardFiltering;  ///< Protect hazard filtering that uses additional information associated with a hazard to evaluate whether it should be considered or not.
  float ignoreVerticalPredictionAmount;  ///< Weighs how much to ignore vertical motion in the trajectory predictions  (Weight)
  float probeRadius;  ///< Radius of probe, personal space  (Length)
  bool velocityIsControlled;  ///< If true then the character's velocity is not considered a hazard
  bool ignoreNonUserHazards;  ///< Only user hazards will be considered if this is true.

  // post-assignment stubs
  NM_INLINE void postAssignProtectParameters(const ProtectParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignIgnoreVerticalPredictionAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "ignoreVerticalPredictionAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignProbeRadius(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "probeRadius");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_potentialUserHazardsImportance, 
     m_protectParametersImportance, 
     m_protectHazardFilteringImportance, 
     m_ignoreVerticalPredictionAmountImportance, 
     m_probeRadiusImportance, 
     m_velocityIsControlledImportance, 
     m_ignoreNonUserHazardsImportance;

  friend class ImpactPredictor_chestCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getProtectParametersImportance() > 0.0f)
    {
      protectParameters.validate();
    }
    if (getIgnoreVerticalPredictionAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(ignoreVerticalPredictionAmount), "ignoreVerticalPredictionAmount");
    }
    if (getProbeRadiusImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(probeRadius), "probeRadius");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ImpactPredictorOutputs
 * \ingroup ImpactPredictor
 * \brief Outputs the job of this module is to alert of (pass up) the single most dangerous hazard (if any) that will affect a defined protect sphere
 *
 * Data packet definition (232 bytes, 256 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ImpactPredictorOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ImpactPredictorOutputs));
  }

  NM_INLINE void setFocalCentre(const TargetRequest& _focalCentre, float focalCentre_importance = 1.f)
  {
    focalCentre = _focalCentre;
    postAssignFocalCentre(focalCentre);
    NMP_ASSERT(focalCentre_importance >= 0.0f && focalCentre_importance <= 1.0f);
    m_focalCentreImportance = focalCentre_importance;
  }
  NM_INLINE TargetRequest& startFocalCentreModification()
  {
    m_focalCentreImportance = -1.0f; // set invalid until stopFocalCentreModification()
    return focalCentre;
  }
  NM_INLINE void stopFocalCentreModification(float focalCentre_importance = 1.f)
  {
    postAssignFocalCentre(focalCentre);
    NMP_ASSERT(focalCentre_importance >= 0.0f && focalCentre_importance <= 1.0f);
    m_focalCentreImportance = focalCentre_importance;
  }
  NM_INLINE float getFocalCentreImportance() const { return m_focalCentreImportance; }
  NM_INLINE const float& getFocalCentreImportanceRef() const { return m_focalCentreImportance; }
  NM_INLINE const TargetRequest& getFocalCentre() const { return focalCentre; }

  NM_INLINE void setSphereProjection(const SphereTrajectory& _sphereProjection, float sphereProjection_importance = 1.f)
  {
    sphereProjection = _sphereProjection;
    postAssignSphereProjection(sphereProjection);
    NMP_ASSERT(sphereProjection_importance >= 0.0f && sphereProjection_importance <= 1.0f);
    m_sphereProjectionImportance = sphereProjection_importance;
  }
  NM_INLINE SphereTrajectory& startSphereProjectionModification()
  {
    m_sphereProjectionImportance = -1.0f; // set invalid until stopSphereProjectionModification()
    return sphereProjection;
  }
  NM_INLINE void stopSphereProjectionModification(float sphereProjection_importance = 1.f)
  {
    postAssignSphereProjection(sphereProjection);
    NMP_ASSERT(sphereProjection_importance >= 0.0f && sphereProjection_importance <= 1.0f);
    m_sphereProjectionImportance = sphereProjection_importance;
  }
  NM_INLINE float getSphereProjectionImportance() const { return m_sphereProjectionImportance; }
  NM_INLINE const float& getSphereProjectionImportanceRef() const { return m_sphereProjectionImportance; }
  NM_INLINE const SphereTrajectory& getSphereProjection() const { return sphereProjection; }

protected:

  TargetRequest focalCentre;  ///< Signal to look at the hazard
  SphereTrajectory sphereProjection;  ///< Passed to EnvironmentAwareness in order to receive potentialHazards set

  // post-assignment stubs
  NM_INLINE void postAssignFocalCentre(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSphereProjection(const SphereTrajectory& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_focalCentreImportance, 
     m_sphereProjectionImportance;

  friend class ImpactPredictor_chestCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFocalCentreImportance() > 0.0f)
    {
      focalCentre.validate();
    }
    if (getSphereProjectionImportance() > 0.0f)
    {
      sphereProjection.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ImpactPredictorFeedbackInputs
 * \ingroup ImpactPredictor
 * \brief FeedbackInputs the job of this module is to alert of (pass up) the single most dangerous hazard (if any) that will affect a defined protect sphere
 *
 * Data packet definition (828 bytes, 832 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ImpactPredictorFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ImpactPredictorFeedbackInputs));
  }

  NM_INLINE void setPotentialEnvironmentHazards(const PatchSet& _potentialEnvironmentHazards, float potentialEnvironmentHazards_importance = 1.f)
  {
    potentialEnvironmentHazards = _potentialEnvironmentHazards;
    NMP_ASSERT(potentialEnvironmentHazards_importance >= 0.0f && potentialEnvironmentHazards_importance <= 1.0f);
    m_potentialEnvironmentHazardsImportance = potentialEnvironmentHazards_importance;
  }
  NM_INLINE PatchSet& startPotentialEnvironmentHazardsModification()
  {
    m_potentialEnvironmentHazardsImportance = -1.0f; // set invalid until stopPotentialEnvironmentHazardsModification()
    return potentialEnvironmentHazards;
  }
  NM_INLINE void stopPotentialEnvironmentHazardsModification(float potentialEnvironmentHazards_importance = 1.f)
  {
    NMP_ASSERT(potentialEnvironmentHazards_importance >= 0.0f && potentialEnvironmentHazards_importance <= 1.0f);
    m_potentialEnvironmentHazardsImportance = potentialEnvironmentHazards_importance;
  }
  NM_INLINE float getPotentialEnvironmentHazardsImportance() const { return m_potentialEnvironmentHazardsImportance; }
  NM_INLINE const float& getPotentialEnvironmentHazardsImportanceRef() const { return m_potentialEnvironmentHazardsImportance; }
  NM_INLINE const PatchSet& getPotentialEnvironmentHazards() const { return potentialEnvironmentHazards; }

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

  NM_INLINE void setLeanVector(const NMP::Vector3& _leanVector, float leanVector_importance = 1.f)
  {
    leanVector = _leanVector;
    postAssignLeanVector(leanVector);
    NMP_ASSERT(leanVector_importance >= 0.0f && leanVector_importance <= 1.0f);
    m_leanVectorImportance = leanVector_importance;
  }
  NM_INLINE float getLeanVectorImportance() const { return m_leanVectorImportance; }
  NM_INLINE const float& getLeanVectorImportanceRef() const { return m_leanVectorImportance; }
  NM_INLINE const NMP::Vector3& getLeanVector() const { return leanVector; }

protected:

  PatchSet potentialEnvironmentHazards;  ///< Set of objects which might be hazards, that means collided patches and some previously collided patches
  ER::LimbTransform supportTM;
  NMP::Vector3 leanVector;  ///< How much the character is leaning, it affects character's computed acceleration  (PositionDelta)

  // post-assignment stubs
  NM_INLINE void postAssignLeanVector(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "leanVector");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_potentialEnvironmentHazardsImportance, 
     m_supportTMImportance, 
     m_leanVectorImportance;

  friend class ImpactPredictor_chestCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLeanVectorImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(leanVector), "leanVector");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ImpactPredictorFeedbackOutputs
 * \ingroup ImpactPredictor
 * \brief FeedbackOutputs the job of this module is to alert of (pass up) the single most dangerous hazard (if any) that will affect a defined protect sphere
 *
 * Data packet definition (429 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ImpactPredictorFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ImpactPredictorFeedbackOutputs));
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

  Hazard hazard;  ///< Pass up the discovered hazard
  SpatialTarget possibleFutureHazard;  ///< Used for placement and head avoid, for close obejcts that have been moving a lot, relative to character
  bool hazardsNotBraceable;  ///< More than one significant hazard will hit the character soon and are not braceable.

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

  friend class ImpactPredictor_chestCon;

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

#endif // NM_MDF_IMPACTPREDICTOR_DATA_H

