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

#ifndef NM_MDF_BODYFRAME_DATA_H
#define NM_MDF_BODYFRAME_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BodyFrame.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/BodyState.h"
#include "Types/RotationRequest.h"
#include "Types/State.h"

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
 * \class BodyFrameData
 * \ingroup BodyFrame
 * \brief Data overall module dealing with applying overall body control, or passing up overall body state
 *
 * Data packet definition (64 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyFrameData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyFrameData));
  }

  BodyState chestState;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    chestState.validate();
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyFrameInputs
 * \ingroup BodyFrame
 * \brief Inputs overall module dealing with applying overall body control, or passing up overall body state
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyFrameInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyFrameInputs));
  }

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

  float instabilityPreparation;  ///< value is size of instability, importance is urgency  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignInstabilityPreparation(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "instabilityPreparation");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_instabilityPreparationImportance;

  friend class BodyFrame_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getInstabilityPreparationImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(instabilityPreparation), "instabilityPreparation");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyFrameOutputs
 * \ingroup BodyFrame
 * \brief Outputs overall module dealing with applying overall body control, or passing up overall body state
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyFrameOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyFrameOutputs));
  }

  NM_INLINE float getPelvisRotationRequestImportance() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const float& getPelvisRotationRequestImportanceRef() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const RotationRequest& getPelvisRotationRequest() const { return pelvisRotationRequest; }

protected:

  RotationRequest pelvisRotationRequest;

  // post-assignment stubs
  NM_INLINE void postAssignPelvisRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisRotationRequestImportance;

  friend class BodyFrame_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisRotationRequestImportance() > 0.0f)
    {
      pelvisRotationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyFrameFeedbackInputs
 * \ingroup BodyFrame
 * \brief FeedbackInputs overall module dealing with applying overall body control, or passing up overall body state
 *
 * Data packet definition (244 bytes, 256 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyFrameFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyFrameFeedbackInputs));
  }

  NM_INLINE void setChestState(const State& _chestState, float chestState_importance = 1.f)
  {
    chestState = _chestState;
    postAssignChestState(chestState);
    NMP_ASSERT(chestState_importance >= 0.0f && chestState_importance <= 1.0f);
    m_chestStateImportance = chestState_importance;
  }
  NM_INLINE State& startChestStateModification()
  {
    m_chestStateImportance = -1.0f; // set invalid until stopChestStateModification()
    return chestState;
  }
  NM_INLINE void stopChestStateModification(float chestState_importance = 1.f)
  {
    postAssignChestState(chestState);
    NMP_ASSERT(chestState_importance >= 0.0f && chestState_importance <= 1.0f);
    m_chestStateImportance = chestState_importance;
  }
  NM_INLINE float getChestStateImportance() const { return m_chestStateImportance; }
  NM_INLINE const float& getChestStateImportanceRef() const { return m_chestStateImportance; }
  NM_INLINE const State& getChestState() const { return chestState; }

  NM_INLINE void setAverageOfSupportingFeet(const NMP::Vector3& _averageOfSupportingFeet, float averageOfSupportingFeet_importance = 1.f)
  {
    averageOfSupportingFeet = _averageOfSupportingFeet;
    postAssignAverageOfSupportingFeet(averageOfSupportingFeet);
    NMP_ASSERT(averageOfSupportingFeet_importance >= 0.0f && averageOfSupportingFeet_importance <= 1.0f);
    m_averageOfSupportingFeetImportance = averageOfSupportingFeet_importance;
  }
  NM_INLINE float getAverageOfSupportingFeetImportance() const { return m_averageOfSupportingFeetImportance; }
  NM_INLINE const float& getAverageOfSupportingFeetImportanceRef() const { return m_averageOfSupportingFeetImportance; }
  NM_INLINE const NMP::Vector3& getAverageOfSupportingFeet() const { return averageOfSupportingFeet; }

  NM_INLINE void setAverageOfSupportingHands(const NMP::Vector3& _averageOfSupportingHands, float averageOfSupportingHands_importance = 1.f)
  {
    averageOfSupportingHands = _averageOfSupportingHands;
    postAssignAverageOfSupportingHands(averageOfSupportingHands);
    NMP_ASSERT(averageOfSupportingHands_importance >= 0.0f && averageOfSupportingHands_importance <= 1.0f);
    m_averageOfSupportingHandsImportance = averageOfSupportingHands_importance;
  }
  NM_INLINE float getAverageOfSupportingHandsImportance() const { return m_averageOfSupportingHandsImportance; }
  NM_INLINE const float& getAverageOfSupportingHandsImportanceRef() const { return m_averageOfSupportingHandsImportance; }
  NM_INLINE const NMP::Vector3& getAverageOfSupportingHands() const { return averageOfSupportingHands; }

  NM_INLINE void setAverageOfSupportingHandsAndFeet(const NMP::Vector3& _averageOfSupportingHandsAndFeet, float averageOfSupportingHandsAndFeet_importance = 1.f)
  {
    averageOfSupportingHandsAndFeet = _averageOfSupportingHandsAndFeet;
    postAssignAverageOfSupportingHandsAndFeet(averageOfSupportingHandsAndFeet);
    NMP_ASSERT(averageOfSupportingHandsAndFeet_importance >= 0.0f && averageOfSupportingHandsAndFeet_importance <= 1.0f);
    m_averageOfSupportingHandsAndFeetImportance = averageOfSupportingHandsAndFeet_importance;
  }
  NM_INLINE float getAverageOfSupportingHandsAndFeetImportance() const { return m_averageOfSupportingHandsAndFeetImportance; }
  NM_INLINE const float& getAverageOfSupportingHandsAndFeetImportanceRef() const { return m_averageOfSupportingHandsAndFeetImportance; }
  NM_INLINE const NMP::Vector3& getAverageOfSupportingHandsAndFeet() const { return averageOfSupportingHandsAndFeet; }

  NM_INLINE void setAverageShoulders(const NMP::Vector3& _averageShoulders, float averageShoulders_importance = 1.f)
  {
    averageShoulders = _averageShoulders;
    postAssignAverageShoulders(averageShoulders);
    NMP_ASSERT(averageShoulders_importance >= 0.0f && averageShoulders_importance <= 1.0f);
    m_averageShouldersImportance = averageShoulders_importance;
  }
  NM_INLINE float getAverageShouldersImportance() const { return m_averageShouldersImportance; }
  NM_INLINE const float& getAverageShouldersImportanceRef() const { return m_averageShouldersImportance; }
  NM_INLINE const NMP::Vector3& getAverageShoulders() const { return averageShoulders; }

  NM_INLINE void setAverageHips(const NMP::Vector3& _averageHips, float averageHips_importance = 1.f)
  {
    averageHips = _averageHips;
    postAssignAverageHips(averageHips);
    NMP_ASSERT(averageHips_importance >= 0.0f && averageHips_importance <= 1.0f);
    m_averageHipsImportance = averageHips_importance;
  }
  NM_INLINE float getAverageHipsImportance() const { return m_averageHipsImportance; }
  NM_INLINE const float& getAverageHipsImportanceRef() const { return m_averageHipsImportance; }
  NM_INLINE const NMP::Vector3& getAverageHips() const { return averageHips; }

  NM_INLINE void setExternalSpineSupportAmount(const float& _externalSpineSupportAmount, float externalSpineSupportAmount_importance = 1.f)
  {
    externalSpineSupportAmount = _externalSpineSupportAmount;
    postAssignExternalSpineSupportAmount(externalSpineSupportAmount);
    NMP_ASSERT(externalSpineSupportAmount_importance >= 0.0f && externalSpineSupportAmount_importance <= 1.0f);
    m_externalSpineSupportAmountImportance = externalSpineSupportAmount_importance;
  }
  NM_INLINE float getExternalSpineSupportAmountImportance() const { return m_externalSpineSupportAmountImportance; }
  NM_INLINE const float& getExternalSpineSupportAmountImportanceRef() const { return m_externalSpineSupportAmountImportance; }
  NM_INLINE const float& getExternalSpineSupportAmount() const { return externalSpineSupportAmount; }

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

  NM_INLINE void setMaxLegCollidingTime(const float& _maxLegCollidingTime, float maxLegCollidingTime_importance = 1.f)
  {
    maxLegCollidingTime = _maxLegCollidingTime;
    postAssignMaxLegCollidingTime(maxLegCollidingTime);
    NMP_ASSERT(maxLegCollidingTime_importance >= 0.0f && maxLegCollidingTime_importance <= 1.0f);
    m_maxLegCollidingTimeImportance = maxLegCollidingTime_importance;
  }
  NM_INLINE float getMaxLegCollidingTimeImportance() const { return m_maxLegCollidingTimeImportance; }
  NM_INLINE const float& getMaxLegCollidingTimeImportanceRef() const { return m_maxLegCollidingTimeImportance; }
  NM_INLINE const float& getMaxLegCollidingTime() const { return maxLegCollidingTime; }

  NM_INLINE void setLegsInContact(const bool& _legsInContact, float legsInContact_importance = 1.f)
  {
    legsInContact = _legsInContact;
    NMP_ASSERT(legsInContact_importance >= 0.0f && legsInContact_importance <= 1.0f);
    m_legsInContactImportance = legsInContact_importance;
  }
  NM_INLINE float getLegsInContactImportance() const { return m_legsInContactImportance; }
  NM_INLINE const float& getLegsInContactImportanceRef() const { return m_legsInContactImportance; }
  NM_INLINE const bool& getLegsInContact() const { return legsInContact; }

  NM_INLINE void setSpineInContact(const bool& _spineInContact, float spineInContact_importance = 1.f)
  {
    spineInContact = _spineInContact;
    NMP_ASSERT(spineInContact_importance >= 0.0f && spineInContact_importance <= 1.0f);
    m_spineInContactImportance = spineInContact_importance;
  }
  NM_INLINE float getSpineInContactImportance() const { return m_spineInContactImportance; }
  NM_INLINE const float& getSpineInContactImportanceRef() const { return m_spineInContactImportance; }
  NM_INLINE const bool& getSpineInContact() const { return spineInContact; }

  NM_INLINE void setIsBodyBarFeetContacting(const bool& _isBodyBarFeetContacting, float isBodyBarFeetContacting_importance = 1.f)
  {
    isBodyBarFeetContacting = _isBodyBarFeetContacting;
    NMP_ASSERT(isBodyBarFeetContacting_importance >= 0.0f && isBodyBarFeetContacting_importance <= 1.0f);
    m_isBodyBarFeetContactingImportance = isBodyBarFeetContacting_importance;
  }
  NM_INLINE float getIsBodyBarFeetContactingImportance() const { return m_isBodyBarFeetContactingImportance; }
  NM_INLINE const float& getIsBodyBarFeetContactingImportanceRef() const { return m_isBodyBarFeetContactingImportance; }
  NM_INLINE const bool& getIsBodyBarFeetContacting() const { return isBodyBarFeetContacting; }

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

  State chestState;
  NMP::Vector3 averageOfSupportingFeet;      ///< (Position)
  NMP::Vector3 averageOfSupportingHands;     ///< (Position)
  NMP::Vector3 averageOfSupportingHandsAndFeet;  ///< (Position)
  NMP::Vector3 averageShoulders;             ///< (Position)
  NMP::Vector3 averageHips;                  ///< (Position)
  float externalSpineSupportAmount;          ///< (Weight)
  float supportAmount;                       ///< (Weight)
  float maxLegCollidingTime;  ///< max collision time from all legs  (TimePeriod)
  bool legsInContact;
  bool spineInContact;
  bool isBodyBarFeetContacting;  ///< any of body in contact apart from feet
  bool isInContact;  ///< Any of body in contact.

  // post-assignment stubs
  NM_INLINE void postAssignChestState(const State& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageOfSupportingFeet(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "averageOfSupportingFeet");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageOfSupportingHands(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "averageOfSupportingHands");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageOfSupportingHandsAndFeet(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "averageOfSupportingHandsAndFeet");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageShoulders(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "averageShoulders");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageHips(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "averageHips");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignExternalSpineSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "externalSpineSupportAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxLegCollidingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "maxLegCollidingTime");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_chestStateImportance, 
     m_averageOfSupportingFeetImportance, 
     m_averageOfSupportingHandsImportance, 
     m_averageOfSupportingHandsAndFeetImportance, 
     m_averageShouldersImportance, 
     m_averageHipsImportance, 
     m_externalSpineSupportAmountImportance, 
     m_supportAmountImportance, 
     m_maxLegCollidingTimeImportance, 
     m_legsInContactImportance, 
     m_spineInContactImportance, 
     m_isBodyBarFeetContactingImportance, 
     m_isInContactImportance;

  friend class BodyFrame_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getChestStateImportance() > 0.0f)
    {
      chestState.validate();
    }
    if (getAverageOfSupportingFeetImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(averageOfSupportingFeet), "averageOfSupportingFeet");
    }
    if (getAverageOfSupportingHandsImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(averageOfSupportingHands), "averageOfSupportingHands");
    }
    if (getAverageOfSupportingHandsAndFeetImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(averageOfSupportingHandsAndFeet), "averageOfSupportingHandsAndFeet");
    }
    if (getAverageShouldersImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(averageShoulders), "averageShoulders");
    }
    if (getAverageHipsImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(averageHips), "averageHips");
    }
    if (getExternalSpineSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(externalSpineSupportAmount), "externalSpineSupportAmount");
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
    if (getMaxLegCollidingTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(maxLegCollidingTime), "maxLegCollidingTime");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodyFrameFeedbackOutputs
 * \ingroup BodyFrame
 * \brief FeedbackOutputs overall module dealing with applying overall body control, or passing up overall body state
 *
 * Data packet definition (200 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodyFrameFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodyFrameFeedbackOutputs));
  }

  NM_INLINE void setCentreOfMassBodyState(const BodyState& _centreOfMassBodyState, float centreOfMassBodyState_importance = 1.f)
  {
    centreOfMassBodyState = _centreOfMassBodyState;
    postAssignCentreOfMassBodyState(centreOfMassBodyState);
    NMP_ASSERT(centreOfMassBodyState_importance >= 0.0f && centreOfMassBodyState_importance <= 1.0f);
    m_centreOfMassBodyStateImportance = centreOfMassBodyState_importance;
  }
  NM_INLINE BodyState& startCentreOfMassBodyStateModification()
  {
    m_centreOfMassBodyStateImportance = -1.0f; // set invalid until stopCentreOfMassBodyStateModification()
    return centreOfMassBodyState;
  }
  NM_INLINE void stopCentreOfMassBodyStateModification(float centreOfMassBodyState_importance = 1.f)
  {
    postAssignCentreOfMassBodyState(centreOfMassBodyState);
    NMP_ASSERT(centreOfMassBodyState_importance >= 0.0f && centreOfMassBodyState_importance <= 1.0f);
    m_centreOfMassBodyStateImportance = centreOfMassBodyState_importance;
  }
  NM_INLINE float getCentreOfMassBodyStateImportance() const { return m_centreOfMassBodyStateImportance; }
  NM_INLINE const float& getCentreOfMassBodyStateImportanceRef() const { return m_centreOfMassBodyStateImportance; }
  NM_INLINE const BodyState& getCentreOfMassBodyState() const { return centreOfMassBodyState; }

  NM_INLINE void setChestState(const BodyState& _chestState, float chestState_importance = 1.f)
  {
    chestState = _chestState;
    postAssignChestState(chestState);
    NMP_ASSERT(chestState_importance >= 0.0f && chestState_importance <= 1.0f);
    m_chestStateImportance = chestState_importance;
  }
  NM_INLINE BodyState& startChestStateModification()
  {
    m_chestStateImportance = -1.0f; // set invalid until stopChestStateModification()
    return chestState;
  }
  NM_INLINE void stopChestStateModification(float chestState_importance = 1.f)
  {
    postAssignChestState(chestState);
    NMP_ASSERT(chestState_importance >= 0.0f && chestState_importance <= 1.0f);
    m_chestStateImportance = chestState_importance;
  }
  NM_INLINE float getChestStateImportance() const { return m_chestStateImportance; }
  NM_INLINE const float& getChestStateImportanceRef() const { return m_chestStateImportance; }
  NM_INLINE const BodyState& getChestState() const { return chestState; }

  NM_INLINE float getLeanVectorImportance() const { return m_leanVectorImportance; }
  NM_INLINE const float& getLeanVectorImportanceRef() const { return m_leanVectorImportance; }
  NM_INLINE const NMP::Vector3& getLeanVector() const { return leanVector; }

  NM_INLINE void setArmsSupportDirection(const NMP::Vector3& _armsSupportDirection, float armsSupportDirection_importance = 1.f)
  {
    armsSupportDirection = _armsSupportDirection;
    postAssignArmsSupportDirection(armsSupportDirection);
    NMP_ASSERT(armsSupportDirection_importance >= 0.0f && armsSupportDirection_importance <= 1.0f);
    m_armsSupportDirectionImportance = armsSupportDirection_importance;
  }
  NM_INLINE float getArmsSupportDirectionImportance() const { return m_armsSupportDirectionImportance; }
  NM_INLINE const float& getArmsSupportDirectionImportanceRef() const { return m_armsSupportDirectionImportance; }
  NM_INLINE const NMP::Vector3& getArmsSupportDirection() const { return armsSupportDirection; }

  NM_INLINE float getImpendingInstabilityImportance() const { return m_impendingInstabilityImportance; }
  NM_INLINE const float& getImpendingInstabilityImportanceRef() const { return m_impendingInstabilityImportance; }
  NM_INLINE const float& getImpendingInstability() const { return impendingInstability; }

  NM_INLINE float getSupportAmountImportance() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmountImportanceRef() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmount() const { return supportAmount; }

  NM_INLINE void setExternalSpineSupportAmount(const float& _externalSpineSupportAmount, float externalSpineSupportAmount_importance = 1.f)
  {
    externalSpineSupportAmount = _externalSpineSupportAmount;
    postAssignExternalSpineSupportAmount(externalSpineSupportAmount);
    NMP_ASSERT(externalSpineSupportAmount_importance >= 0.0f && externalSpineSupportAmount_importance <= 1.0f);
    m_externalSpineSupportAmountImportance = externalSpineSupportAmount_importance;
  }
  NM_INLINE float getExternalSpineSupportAmountImportance() const { return m_externalSpineSupportAmountImportance; }
  NM_INLINE const float& getExternalSpineSupportAmountImportanceRef() const { return m_externalSpineSupportAmountImportance; }
  NM_INLINE const float& getExternalSpineSupportAmount() const { return externalSpineSupportAmount; }

protected:

  BodyState centreOfMassBodyState;
  BodyState chestState;
  NMP::Vector3 leanVector;                   ///< (PositionDelta)
  NMP::Vector3 armsSupportDirection;         ///< (Direction)
  float impendingInstability;  ///< value is size of instability, importance is urgency  (Weight)
  float supportAmount;  ///< If the body as a whole is supported (includes standing or stepping or sitting etc)  (Weight)
  float externalSpineSupportAmount;          ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignCentreOfMassBodyState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLeanVector(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "leanVector");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmsSupportDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "armsSupportDirection");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignImpendingInstability(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "impendingInstability");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignExternalSpineSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "externalSpineSupportAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_centreOfMassBodyStateImportance, 
     m_chestStateImportance, 
     m_leanVectorImportance, 
     m_armsSupportDirectionImportance, 
     m_impendingInstabilityImportance, 
     m_supportAmountImportance, 
     m_externalSpineSupportAmountImportance;

  friend class BodyFrame_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCentreOfMassBodyStateImportance() > 0.0f)
    {
      centreOfMassBodyState.validate();
    }
    if (getChestStateImportance() > 0.0f)
    {
      chestState.validate();
    }
    if (getLeanVectorImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(leanVector), "leanVector");
    }
    if (getArmsSupportDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(armsSupportDirection), "armsSupportDirection");
    }
    if (getImpendingInstabilityImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(impendingInstability), "impendingInstability");
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
    if (getExternalSpineSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(externalSpineSupportAmount), "externalSpineSupportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYFRAME_DATA_H

