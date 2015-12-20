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

#ifndef NM_MDF_ARMSBRACEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_ARMSBRACEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ArmsBraceBehaviourInterface.module"

// known types
#include "Types/BraceHazard.h"

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
 * \class ArmsBraceBehaviourInterfaceOutputs
 * \ingroup ArmsBraceBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (308 bytes, 320 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmsBraceBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmsBraceBehaviourInterfaceOutputs));
  }

  NM_INLINE void setHazard(const BraceHazard& _hazard, float hazard_importance = 1.f)
  {
    hazard = _hazard;
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE BraceHazard& startHazardModification()
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
  NM_INLINE const BraceHazard& getHazard() const { return hazard; }

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

  BraceHazard hazard;  ///< User defined hazard to brace against
  float swivelAmount;  ///< Elbows in or out  (Multiplier)
  float maxArmExtensionScale;  ///< 1 is fully extended arms on bracing  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignHazard(const BraceHazard& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignMaxArmExtensionScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "maxArmExtensionScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_hazardImportance, 
     m_swivelAmountImportance, 
     m_maxArmExtensionScaleImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHazardImportance() > 0.0f)
    {
      hazard.validate();
    }
    if (getSwivelAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
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

#endif // NM_MDF_ARMSBRACEBEHAVIOURINTERFACE_DATA_H

