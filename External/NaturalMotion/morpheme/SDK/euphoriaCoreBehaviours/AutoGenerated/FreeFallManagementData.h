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

#ifndef NM_MDF_FREEFALLMANAGEMENT_DATA_H
#define NM_MDF_FREEFALLMANAGEMENT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/FreeFallManagement.module"

// known types
#include "Types/Wrench.h"
#include "Types/Hazard.h"
#include "Types/FreefallParameters.h"
#include "Types/RotationRequest.h"
#include "Types/TargetRequest.h"
#include "Types/PoseData.h"

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
 * \class FreeFallManagementData
 * \ingroup FreeFallManagement
 * \brief Data Compute the desired orientation and send rotation requests and cheating torque to try to keep the 
 *  body matching the desired orientation during freefall.
 * 
 *  If the character is not in contact and is not supported or has no ground below him or is already in freefall
 *  he will be considered in freefall :
 *   - if he is already in freefall until the nextImpact.
 *   - if the next predicted impact is later than the freefallParams.minTimeBeforeImpactToStartToSpin.
 *
 * Data packet definition (45 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallManagementData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallManagementData));
  }

  Wrench wrench;  ///< Torque to rotate character towards the desired orientation.
  float isInFreefallTimer;                   ///< (TimePeriod)
  int32_t noHazardDetected;  ///< Number of frames that no hazard has been detected
  float orientationError;  ///< Angle between current and target orientation, in radians.  (Angle)
  bool isInFreefall;  ///< Means character is in freefall but doesn't mean is doing fly behaviour.

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    wrench.validate();
    NM_VALIDATOR(FloatValid(isInFreefallTimer), "isInFreefallTimer");
    NM_VALIDATOR(FloatValid(orientationError), "orientationError");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class FreeFallManagementInputs
 * \ingroup FreeFallManagement
 * \brief Inputs Compute the desired orientation and send rotation requests and cheating torque to try to keep the 
 *  body matching the desired orientation during freefall.
 * 
 *  If the character is not in contact and is not supported or has no ground below him or is already in freefall
 *  he will be considered in freefall :
 *   - if he is already in freefall until the nextImpact.
 *   - if the next predicted impact is later than the freefallParams.minTimeBeforeImpactToStartToSpin.
 *
 * Data packet definition (84 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallManagementInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallManagementInputs));
  }

  NM_INLINE void setFreefallParams(const FreefallParameters& _freefallParams, float freefallParams_importance = 1.f)
  {
    freefallParams = _freefallParams;
    postAssignFreefallParams(freefallParams);
    NMP_ASSERT(freefallParams_importance >= 0.0f && freefallParams_importance <= 1.0f);
    m_freefallParamsImportance = freefallParams_importance;
  }
  NM_INLINE FreefallParameters& startFreefallParamsModification()
  {
    m_freefallParamsImportance = -1.0f; // set invalid until stopFreefallParamsModification()
    return freefallParams;
  }
  NM_INLINE void stopFreefallParamsModification(float freefallParams_importance = 1.f)
  {
    postAssignFreefallParams(freefallParams);
    NMP_ASSERT(freefallParams_importance >= 0.0f && freefallParams_importance <= 1.0f);
    m_freefallParamsImportance = freefallParams_importance;
  }
  NM_INLINE float getFreefallParamsImportance() const { return m_freefallParamsImportance; }
  NM_INLINE const float& getFreefallParamsImportanceRef() const { return m_freefallParamsImportance; }
  NM_INLINE const FreefallParameters& getFreefallParams() const { return freefallParams; }

protected:

  FreefallParameters freefallParams;

  // post-assignment stubs
  NM_INLINE void postAssignFreefallParams(const FreefallParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_freefallParamsImportance;

  friend class FreeFallManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFreefallParamsImportance() > 0.0f)
    {
      freefallParams.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class FreeFallManagementOutputs
 * \ingroup FreeFallManagement
 * \brief Outputs Compute the desired orientation and send rotation requests and cheating torque to try to keep the 
 *  body matching the desired orientation during freefall.
 * 
 *  If the character is not in contact and is not supported or has no ground below him or is already in freefall
 *  he will be considered in freefall :
 *   - if he is already in freefall until the nextImpact.
 *   - if the next predicted impact is later than the freefallParams.minTimeBeforeImpactToStartToSpin.
 *
 * Data packet definition (637 bytes, 640 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallManagementOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallManagementOutputs));
  }

  NM_INLINE void setArmsSpinRotationRequest(const RotationRequest& _armsSpinRotationRequest, float armsSpinRotationRequest_importance = 1.f)
  {
    armsSpinRotationRequest = _armsSpinRotationRequest;
    postAssignArmsSpinRotationRequest(armsSpinRotationRequest);
    NMP_ASSERT(armsSpinRotationRequest_importance >= 0.0f && armsSpinRotationRequest_importance <= 1.0f);
    m_armsSpinRotationRequestImportance = armsSpinRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startArmsSpinRotationRequestModification()
  {
    m_armsSpinRotationRequestImportance = -1.0f; // set invalid until stopArmsSpinRotationRequestModification()
    return armsSpinRotationRequest;
  }
  NM_INLINE void stopArmsSpinRotationRequestModification(float armsSpinRotationRequest_importance = 1.f)
  {
    postAssignArmsSpinRotationRequest(armsSpinRotationRequest);
    NMP_ASSERT(armsSpinRotationRequest_importance >= 0.0f && armsSpinRotationRequest_importance <= 1.0f);
    m_armsSpinRotationRequestImportance = armsSpinRotationRequest_importance;
  }
  NM_INLINE float getArmsSpinRotationRequestImportance() const { return m_armsSpinRotationRequestImportance; }
  NM_INLINE const float& getArmsSpinRotationRequestImportanceRef() const { return m_armsSpinRotationRequestImportance; }
  NM_INLINE const RotationRequest& getArmsSpinRotationRequest() const { return armsSpinRotationRequest; }

  NM_INLINE void setLegsSpinRotationRequest(const RotationRequest& _legsSpinRotationRequest, float legsSpinRotationRequest_importance = 1.f)
  {
    legsSpinRotationRequest = _legsSpinRotationRequest;
    postAssignLegsSpinRotationRequest(legsSpinRotationRequest);
    NMP_ASSERT(legsSpinRotationRequest_importance >= 0.0f && legsSpinRotationRequest_importance <= 1.0f);
    m_legsSpinRotationRequestImportance = legsSpinRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startLegsSpinRotationRequestModification()
  {
    m_legsSpinRotationRequestImportance = -1.0f; // set invalid until stopLegsSpinRotationRequestModification()
    return legsSpinRotationRequest;
  }
  NM_INLINE void stopLegsSpinRotationRequestModification(float legsSpinRotationRequest_importance = 1.f)
  {
    postAssignLegsSpinRotationRequest(legsSpinRotationRequest);
    NMP_ASSERT(legsSpinRotationRequest_importance >= 0.0f && legsSpinRotationRequest_importance <= 1.0f);
    m_legsSpinRotationRequestImportance = legsSpinRotationRequest_importance;
  }
  NM_INLINE float getLegsSpinRotationRequestImportance() const { return m_legsSpinRotationRequestImportance; }
  NM_INLINE const float& getLegsSpinRotationRequestImportanceRef() const { return m_legsSpinRotationRequestImportance; }
  NM_INLINE const RotationRequest& getLegsSpinRotationRequest() const { return legsSpinRotationRequest; }

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

  NM_INLINE void setSpinePose(const PoseData& _spinePose, float spinePose_importance = 1.f)
  {
    spinePose = _spinePose;
    postAssignSpinePose(spinePose);
    NMP_ASSERT(spinePose_importance >= 0.0f && spinePose_importance <= 1.0f);
    m_spinePoseImportance = spinePose_importance;
  }
  NM_INLINE PoseData& startSpinePoseModification()
  {
    m_spinePoseImportance = -1.0f; // set invalid until stopSpinePoseModification()
    return spinePose;
  }
  NM_INLINE void stopSpinePoseModification(float spinePose_importance = 1.f)
  {
    postAssignSpinePose(spinePose);
    NMP_ASSERT(spinePose_importance >= 0.0f && spinePose_importance <= 1.0f);
    m_spinePoseImportance = spinePose_importance;
  }
  NM_INLINE float getSpinePoseImportance() const { return m_spinePoseImportance; }
  NM_INLINE const float& getSpinePoseImportanceRef() const { return m_spinePoseImportance; }
  NM_INLINE const PoseData& getSpinePose() const { return spinePose; }

  NM_INLINE void setOrientationError(const float& _orientationError, float orientationError_importance = 1.f)
  {
    orientationError = _orientationError;
    postAssignOrientationError(orientationError);
    NMP_ASSERT(orientationError_importance >= 0.0f && orientationError_importance <= 1.0f);
    m_orientationErrorImportance = orientationError_importance;
  }
  NM_INLINE float getOrientationErrorImportance() const { return m_orientationErrorImportance; }
  NM_INLINE const float& getOrientationErrorImportanceRef() const { return m_orientationErrorImportance; }
  NM_INLINE const float& getOrientationError() const { return orientationError; }

  NM_INLINE void setIsInFreefall(const bool& _isInFreefall, float isInFreefall_importance = 1.f)
  {
    isInFreefall = _isInFreefall;
    NMP_ASSERT(isInFreefall_importance >= 0.0f && isInFreefall_importance <= 1.0f);
    m_isInFreefallImportance = isInFreefall_importance;
  }
  NM_INLINE float getIsInFreefallImportance() const { return m_isInFreefallImportance; }
  NM_INLINE const float& getIsInFreefallImportanceRef() const { return m_isInFreefallImportance; }
  NM_INLINE const bool& getIsInFreefall() const { return isInFreefall; }

protected:

  RotationRequest armsSpinRotationRequest;
  RotationRequest legsSpinRotationRequest;
  TargetRequest focalCentre;
  PoseData spinePose;
  float orientationError;  ///< Angle between current and target orientation, in radians.  (Angle)
  bool isInFreefall;  ///< Orientation control is active.

  // post-assignment stubs
  NM_INLINE void postAssignArmsSpinRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegsSpinRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFocalCentre(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOrientationError(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "orientationError");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armsSpinRotationRequestImportance, 
     m_legsSpinRotationRequestImportance, 
     m_focalCentreImportance, 
     m_spinePoseImportance, 
     m_orientationErrorImportance, 
     m_isInFreefallImportance;

  friend class FreeFallManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getArmsSpinRotationRequestImportance() > 0.0f)
    {
      armsSpinRotationRequest.validate();
    }
    if (getLegsSpinRotationRequestImportance() > 0.0f)
    {
      legsSpinRotationRequest.validate();
    }
    if (getFocalCentreImportance() > 0.0f)
    {
      focalCentre.validate();
    }
    if (getSpinePoseImportance() > 0.0f)
    {
      spinePose.validate();
    }
    if (getOrientationErrorImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(orientationError), "orientationError");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class FreeFallManagementFeedbackInputs
 * \ingroup FreeFallManagement
 * \brief FeedbackInputs Compute the desired orientation and send rotation requests and cheating torque to try to keep the 
 *  body matching the desired orientation during freefall.
 * 
 *  If the character is not in contact and is not supported or has no ground below him or is already in freefall
 *  he will be considered in freefall :
 *   - if he is already in freefall until the nextImpact.
 *   - if the next predicted impact is later than the freefallParams.minTimeBeforeImpactToStartToSpin.
 *
 * Data packet definition (385 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallManagementFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallManagementFeedbackInputs));
  }

  NM_INLINE void setChestHazard(const Hazard& _chestHazard, float chestHazard_importance = 1.f)
  {
    chestHazard = _chestHazard;
    postAssignChestHazard(chestHazard);
    NMP_ASSERT(chestHazard_importance >= 0.0f && chestHazard_importance <= 1.0f);
    m_chestHazardImportance = chestHazard_importance;
  }
  NM_INLINE Hazard& startChestHazardModification()
  {
    m_chestHazardImportance = -1.0f; // set invalid until stopChestHazardModification()
    return chestHazard;
  }
  NM_INLINE void stopChestHazardModification(float chestHazard_importance = 1.f)
  {
    postAssignChestHazard(chestHazard);
    NMP_ASSERT(chestHazard_importance >= 0.0f && chestHazard_importance <= 1.0f);
    m_chestHazardImportance = chestHazard_importance;
  }
  NM_INLINE float getChestHazardImportance() const { return m_chestHazardImportance; }
  NM_INLINE const float& getChestHazardImportanceRef() const { return m_chestHazardImportance; }
  NM_INLINE const Hazard& getChestHazard() const { return chestHazard; }

  NM_INLINE void setSupportAmount(const float& _supportAmount, float supportAmount_importance = 1.f)
  {
    supportAmount = _supportAmount;
    postAssignSupportAmount(supportAmount);
    NMP_ASSERT(supportAmount_importance >= 0.0f && supportAmount_importance <= 1.0f);
    m_supportAmountImportance = supportAmount_importance;
  }
  NM_INLINE float getSupportAmountImportance() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmountImportanceRef() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmount() const { return supportAmount; }

  NM_INLINE void setIsInContact(const bool& _isInContact, float isInContact_importance = 1.f)
  {
    isInContact = _isInContact;
    NMP_ASSERT(isInContact_importance >= 0.0f && isInContact_importance <= 1.0f);
    m_isInContactImportance = isInContact_importance;
  }
  NM_INLINE float getIsInContactImportance() const { return m_isInContactImportance; }
  NM_INLINE const float& getIsInContactImportanceRef() const { return m_isInContactImportance; }
  NM_INLINE const bool& getIsInContact() const { return isInContact; }

protected:

  Hazard chestHazard;
  float supportAmount;                       ///< (Weight)
  bool isInContact;

  // post-assignment stubs
  NM_INLINE void postAssignChestHazard(const Hazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_chestHazardImportance, 
     m_supportAmountImportance, 
     m_isInContactImportance;

  friend class FreeFallManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getChestHazardImportance() > 0.0f)
    {
      chestHazard.validate();
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_FREEFALLMANAGEMENT_DATA_H

