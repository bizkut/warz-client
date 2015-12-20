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

#ifndef NM_MDF_ARMSPLACEMENTBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_ARMSPLACEMENTBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ArmsPlacementBehaviourInterface.module"

// known types
#include "Types/SpatialTarget.h"

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
 * \class ArmsPlacementBehaviourInterfaceOutputs
 * \ingroup ArmsPlacementBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (76 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmsPlacementBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmsPlacementBehaviourInterfaceOutputs));
  }

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

  NM_INLINE void setSwivelAmount(const float& _swivelAmount, float swivelAmount_importance = 1.f)
  {
    swivelAmount = _swivelAmount;
    postAssignSwivelAmount(swivelAmount);
    NMP_ASSERT(swivelAmount_importance >= 0.0f && swivelAmount_importance <= 1.0f);
    m_swivelAmountImportance = swivelAmount_importance;
  }
  NM_INLINE float getSwivelAmountImportance() const { return m_swivelAmountImportance; }
  NM_INLINE const float& getSwivelAmountImportanceRef() const { return m_swivelAmountImportance; }
  NM_INLINE const float& getSwivelAmount() const { return swivelAmount; }

  NM_INLINE void setHandOffsetMultiplier(const float& _handOffsetMultiplier, float handOffsetMultiplier_importance = 1.f)
  {
    handOffsetMultiplier = _handOffsetMultiplier;
    postAssignHandOffsetMultiplier(handOffsetMultiplier);
    NMP_ASSERT(handOffsetMultiplier_importance >= 0.0f && handOffsetMultiplier_importance <= 1.0f);
    m_handOffsetMultiplierImportance = handOffsetMultiplier_importance;
  }
  NM_INLINE float getHandOffsetMultiplierImportance() const { return m_handOffsetMultiplierImportance; }
  NM_INLINE const float& getHandOffsetMultiplierImportanceRef() const { return m_handOffsetMultiplierImportance; }
  NM_INLINE const float& getHandOffsetMultiplier() const { return handOffsetMultiplier; }

  NM_INLINE void setMaxArmExtensionScale(const float& _maxArmExtensionScale, float maxArmExtensionScale_importance = 1.f)
  {
    maxArmExtensionScale = _maxArmExtensionScale;
    postAssignMaxArmExtensionScale(maxArmExtensionScale);
    NMP_ASSERT(maxArmExtensionScale_importance >= 0.0f && maxArmExtensionScale_importance <= 1.0f);
    m_maxArmExtensionScaleImportance = maxArmExtensionScale_importance;
  }
  NM_INLINE float getMaxArmExtensionScaleImportance() const { return m_maxArmExtensionScaleImportance; }
  NM_INLINE const float& getMaxArmExtensionScaleImportanceRef() const { return m_maxArmExtensionScaleImportance; }
  NM_INLINE const float& getMaxArmExtensionScale() const { return maxArmExtensionScale; }

protected:

  SpatialTarget placementTarget;  ///< Choice of output defines whether it has priority or not when competing
  float swivelAmount;  ///< Elbows in or out  (Multiplier)
  float handOffsetMultiplier;  ///< The span while placing, 1 means keep hands apart by shoulder distance  (Multiplier)
  float maxArmExtensionScale;  ///< 1 is fully extended arms  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignPlacementTarget(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSwivelAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "swivelAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHandOffsetMultiplier(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "handOffsetMultiplier");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxArmExtensionScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "maxArmExtensionScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_placementTargetImportance, 
     m_swivelAmountImportance, 
     m_handOffsetMultiplierImportance, 
     m_maxArmExtensionScaleImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPlacementTargetImportance() > 0.0f)
    {
      placementTarget.validate();
    }
    if (getSwivelAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    }
    if (getHandOffsetMultiplierImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(handOffsetMultiplier), "handOffsetMultiplier");
    }
    if (getMaxArmExtensionScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(maxArmExtensionScale), "maxArmExtensionScale");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSPLACEMENTBEHAVIOURINTERFACE_DATA_H

