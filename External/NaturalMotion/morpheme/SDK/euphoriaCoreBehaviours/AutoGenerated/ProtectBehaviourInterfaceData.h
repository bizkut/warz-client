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

#ifndef NM_MDF_PROTECTBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_PROTECTBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ProtectBehaviourInterface.module"

// known types
#include "Types/ProtectParameters.h"
#include "Types/ObjectFiltering.h"

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
 * \class ProtectBehaviourInterfaceUserInputs
 * \ingroup ProtectBehaviourInterface
 * \brief UserInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (42 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ProtectBehaviourInterfaceUserInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ProtectBehaviourInterfaceUserInputs));
  }

  enum { maxFilterBitmaskWord = 4 };
  NM_INLINE unsigned int getMaxFilterBitmaskWord() const { return maxFilterBitmaskWord; }
  NM_INLINE void setFilterBitmaskWord(unsigned int number, const uint32_t* _filterBitmaskWord, float filterBitmaskWord_importance = 1.f)
  {
    NMP_ASSERT(number <= maxFilterBitmaskWord);
    NMP_ASSERT(filterBitmaskWord_importance >= 0.0f && filterBitmaskWord_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      filterBitmaskWord[i] = _filterBitmaskWord[i];
      m_filterBitmaskWordImportance[i] = filterBitmaskWord_importance;
    }
  }
  NM_INLINE void setFilterBitmaskWordAt(unsigned int index, const uint32_t& _filterBitmaskWord, float filterBitmaskWord_importance = 1.f)
  {
    NMP_ASSERT(index < maxFilterBitmaskWord);
    filterBitmaskWord[index] = _filterBitmaskWord;
    NMP_ASSERT(filterBitmaskWord_importance >= 0.0f && filterBitmaskWord_importance <= 1.0f);
    m_filterBitmaskWordImportance[index] = filterBitmaskWord_importance;
  }
  NM_INLINE float getFilterBitmaskWordImportance(int index) const { return m_filterBitmaskWordImportance[index]; }
  NM_INLINE const float& getFilterBitmaskWordImportanceRef(int index) const { return m_filterBitmaskWordImportance[index]; }
  NM_INLINE unsigned int calculateNumFilterBitmaskWord() const
  {
    for (int i=3; i>=0; --i)
    {
      if (m_filterBitmaskWordImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const uint32_t& getFilterBitmaskWord(unsigned int index) const { NMP_ASSERT(index <= maxFilterBitmaskWord); return filterBitmaskWord[index]; }

  NM_INLINE void setUseControlledVelocity(const bool& _useControlledVelocity, float useControlledVelocity_importance = 1.f)
  {
    useControlledVelocity = _useControlledVelocity;
    NMP_ASSERT(useControlledVelocity_importance >= 0.0f && useControlledVelocity_importance <= 1.0f);
    m_useControlledVelocityImportance = useControlledVelocity_importance;
  }
  NM_INLINE float getUseControlledVelocityImportance() const { return m_useControlledVelocityImportance; }
  NM_INLINE const float& getUseControlledVelocityImportanceRef() const { return m_useControlledVelocityImportance; }
  NM_INLINE const bool& getUseControlledVelocity() const { return useControlledVelocity; }

  NM_INLINE void setUseFiltering(const bool& _useFiltering, float useFiltering_importance = 1.f)
  {
    useFiltering = _useFiltering;
    NMP_ASSERT(useFiltering_importance >= 0.0f && useFiltering_importance <= 1.0f);
    m_useFilteringImportance = useFiltering_importance;
  }
  NM_INLINE float getUseFilteringImportance() const { return m_useFilteringImportance; }
  NM_INLINE const float& getUseFilteringImportanceRef() const { return m_useFilteringImportance; }
  NM_INLINE const bool& getUseFiltering() const { return useFiltering; }

protected:

  uint32_t filterBitmaskWord[4];  /// Bitmask used by filter shader function.
  bool useControlledVelocity;  ///< means character's velocity is not considered capable of being a hazard
  bool useFiltering;  /// Toggles hazard filtering that uses additional information associated with a hazard to evaluate whether it should be considered or not.

  // importance values
  float
     m_filterBitmaskWordImportance[4], 
     m_useControlledVelocityImportance, 
     m_useFilteringImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ProtectBehaviourInterfaceOutputs
 * \ingroup ProtectBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (152 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ProtectBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ProtectBehaviourInterfaceOutputs));
  }

  NM_INLINE void setToImpactPredictorParameters(const ProtectParameters& _toImpactPredictorParameters, float toImpactPredictorParameters_importance = 1.f)
  {
    toImpactPredictorParameters = _toImpactPredictorParameters;
    postAssignToImpactPredictorParameters(toImpactPredictorParameters);
    NMP_ASSERT(toImpactPredictorParameters_importance >= 0.0f && toImpactPredictorParameters_importance <= 1.0f);
    m_toImpactPredictorParametersImportance = toImpactPredictorParameters_importance;
  }
  NM_INLINE float getToImpactPredictorParametersImportance() const { return m_toImpactPredictorParametersImportance; }
  NM_INLINE const float& getToImpactPredictorParametersImportanceRef() const { return m_toImpactPredictorParametersImportance; }
  NM_INLINE const ProtectParameters& getToImpactPredictorParameters() const { return toImpactPredictorParameters; }

  NM_INLINE void setToImpactPredictorHazardFiltering(const ObjectFiltering& _toImpactPredictorHazardFiltering, float toImpactPredictorHazardFiltering_importance = 1.f)
  {
    toImpactPredictorHazardFiltering = _toImpactPredictorHazardFiltering;
    NMP_ASSERT(toImpactPredictorHazardFiltering_importance >= 0.0f && toImpactPredictorHazardFiltering_importance <= 1.0f);
    m_toImpactPredictorHazardFilteringImportance = toImpactPredictorHazardFiltering_importance;
  }
  NM_INLINE float getToImpactPredictorHazardFilteringImportance() const { return m_toImpactPredictorHazardFilteringImportance; }
  NM_INLINE const float& getToImpactPredictorHazardFilteringImportanceRef() const { return m_toImpactPredictorHazardFilteringImportance; }
  NM_INLINE const ObjectFiltering& getToImpactPredictorHazardFiltering() const { return toImpactPredictorHazardFiltering; }

  NM_INLINE void setCrouchDownAmount(const float& _crouchDownAmount, float crouchDownAmount_importance = 1.f)
  {
    crouchDownAmount = _crouchDownAmount;
    postAssignCrouchDownAmount(crouchDownAmount);
    NMP_ASSERT(crouchDownAmount_importance >= 0.0f && crouchDownAmount_importance <= 1.0f);
    m_crouchDownAmountImportance = crouchDownAmount_importance;
  }
  NM_INLINE float getCrouchDownAmountImportance() const { return m_crouchDownAmountImportance; }
  NM_INLINE const float& getCrouchDownAmountImportanceRef() const { return m_crouchDownAmountImportance; }
  NM_INLINE const float& getCrouchDownAmount() const { return crouchDownAmount; }

  NM_INLINE void setCrouchPitchAmount(const float& _crouchPitchAmount, float crouchPitchAmount_importance = 1.f)
  {
    crouchPitchAmount = _crouchPitchAmount;
    postAssignCrouchPitchAmount(crouchPitchAmount);
    NMP_ASSERT(crouchPitchAmount_importance >= 0.0f && crouchPitchAmount_importance <= 1.0f);
    m_crouchPitchAmountImportance = crouchPitchAmount_importance;
  }
  NM_INLINE float getCrouchPitchAmountImportance() const { return m_crouchPitchAmountImportance; }
  NM_INLINE const float& getCrouchPitchAmountImportanceRef() const { return m_crouchPitchAmountImportance; }
  NM_INLINE const float& getCrouchPitchAmount() const { return crouchPitchAmount; }

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

  NM_INLINE void setArmsSwingWeight(const float& _armsSwingWeight, float armsSwingWeight_importance = 1.f)
  {
    armsSwingWeight = _armsSwingWeight;
    postAssignArmsSwingWeight(armsSwingWeight);
    NMP_ASSERT(armsSwingWeight_importance >= 0.0f && armsSwingWeight_importance <= 1.0f);
    m_armsSwingWeightImportance = armsSwingWeight_importance;
  }
  NM_INLINE float getArmsSwingWeightImportance() const { return m_armsSwingWeightImportance; }
  NM_INLINE const float& getArmsSwingWeightImportanceRef() const { return m_armsSwingWeightImportance; }
  NM_INLINE const float& getArmsSwingWeight() const { return armsSwingWeight; }

  NM_INLINE void setLegsSwingWeight(const float& _legsSwingWeight, float legsSwingWeight_importance = 1.f)
  {
    legsSwingWeight = _legsSwingWeight;
    postAssignLegsSwingWeight(legsSwingWeight);
    NMP_ASSERT(legsSwingWeight_importance >= 0.0f && legsSwingWeight_importance <= 1.0f);
    m_legsSwingWeightImportance = legsSwingWeight_importance;
  }
  NM_INLINE float getLegsSwingWeightImportance() const { return m_legsSwingWeightImportance; }
  NM_INLINE const float& getLegsSwingWeightImportanceRef() const { return m_legsSwingWeightImportance; }
  NM_INLINE const float& getLegsSwingWeight() const { return legsSwingWeight; }

protected:

  ProtectParameters toImpactPredictorParameters;
  ObjectFiltering toImpactPredictorHazardFiltering;  ///< Protect hazard filtering that uses additional information associated with a hazard to evaluate whether it should be considered or not.
  float crouchDownAmount;  ///< Larger crouches down more when preparing for an incoming hazard  (Multiplier)
  float crouchPitchAmount;  ///< Larger pitches forwards more when preparing for an incoming hazard  (Multiplier)
  float swivelAmount;  ///< Elbows in or out  (Multiplier)
  float maxArmExtensionScale;  ///< 1 is full arm extension  (Weight)
  float headAvoidWeight;                     ///< (Weight)
  float headDodgeWeight;                     ///< (Weight)
  float braceWeight;                         ///< (Weight)
  float armsPlacementWeight;                 ///< (Weight)
  float armsSwingWeight;                     ///< (Multiplier)
  float legsSwingWeight;                     ///< (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignToImpactPredictorParameters(const ProtectParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCrouchDownAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "crouchDownAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCrouchPitchAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "crouchPitchAmount");
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

  NM_INLINE void postAssignArmsSwingWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armsSwingWeight");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegsSwingWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legsSwingWeight");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_toImpactPredictorParametersImportance, 
     m_toImpactPredictorHazardFilteringImportance, 
     m_crouchDownAmountImportance, 
     m_crouchPitchAmountImportance, 
     m_swivelAmountImportance, 
     m_maxArmExtensionScaleImportance, 
     m_headAvoidWeightImportance, 
     m_headDodgeWeightImportance, 
     m_braceWeightImportance, 
     m_armsPlacementWeightImportance, 
     m_armsSwingWeightImportance, 
     m_legsSwingWeightImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getToImpactPredictorParametersImportance() > 0.0f)
    {
      toImpactPredictorParameters.validate();
    }
    if (getCrouchDownAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(crouchDownAmount), "crouchDownAmount");
    }
    if (getCrouchPitchAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(crouchPitchAmount), "crouchPitchAmount");
    }
    if (getSwivelAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    }
    if (getMaxArmExtensionScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(maxArmExtensionScale), "maxArmExtensionScale");
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
    if (getArmsSwingWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(armsSwingWeight), "armsSwingWeight");
    }
    if (getLegsSwingWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(legsSwingWeight), "legsSwingWeight");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_PROTECTBEHAVIOURINTERFACE_DATA_H

