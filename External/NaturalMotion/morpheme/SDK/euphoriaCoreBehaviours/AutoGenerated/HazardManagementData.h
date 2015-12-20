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

#ifndef NM_MDF_HAZARDMANAGEMENT_DATA_H
#define NM_MDF_HAZARDMANAGEMENT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HazardManagement.module"

// known types
#include "Types/BodyState.h"

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
 * \class HazardManagementOutputs
 * \ingroup HazardManagement
 * \brief Outputs module containing modules which deal with detecting and managing hazards
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardManagementOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardManagementOutputs));
  }

  NM_INLINE float getInstabilityPreparationImportance() const { return m_instabilityPreparationImportance; }
  NM_INLINE const float& getInstabilityPreparationImportanceRef() const { return m_instabilityPreparationImportance; }
  NM_INLINE const float& getInstabilityPreparation() const { return instabilityPreparation; }

protected:

  float instabilityPreparation;  ///< This value is the magnitude of the instability, and the importance is 1/(1+ time to instability)  (Weight)

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

  friend class HazardManagement_Con;

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
 * \class HazardManagementFeedbackInputs
 * \ingroup HazardManagement
 * \brief FeedbackInputs module containing modules which deal with detecting and managing hazards
 *
 * Data packet definition (76 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardManagementFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardManagementFeedbackInputs));
  }

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

protected:

  BodyState chestState;  ///< Current pos, vel etc of chest
  float supportAmount;  ///< How well the character is supported, affects character's acceleration  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignChestState(const BodyState& NMP_VALIDATOR_ARG(v)) const
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
     m_chestStateImportance, 
     m_supportAmountImportance;

  friend class HazardManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getChestStateImportance() > 0.0f)
    {
      chestState.validate();
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

#endif // NM_MDF_HAZARDMANAGEMENT_DATA_H

