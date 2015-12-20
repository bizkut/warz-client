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

#ifndef NM_MDF_ARMPLACEMENT_DATA_H
#define NM_MDF_ARMPLACEMENT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmPlacement.module"

// known types
#include "Types/SpatialTarget.h"
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
 * \class ArmPlacementData
 * \ingroup ArmPlacement
 * \brief Data Place the arms in front of the target, to minimise danger of object and head/chest colliding
 *
 * Data packet definition (2 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmPlacementData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmPlacementData));
  }

  bool targetWithinCone;
  bool isInRange;  ///< Used for hysteresis.

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
 * \class ArmPlacementInputs
 * \ingroup ArmPlacement
 * \brief Inputs Place the arms in front of the target, to minimise danger of object and head/chest colliding
 *
 * Data packet definition (76 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmPlacementInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmPlacementInputs));
  }

  NM_INLINE void setTarget(const SpatialTarget& _target, float target_importance = 1.f)
  {
    target = _target;
    postAssignTarget(target);
    NMP_ASSERT(target_importance >= 0.0f && target_importance <= 1.0f);
    m_targetImportance = target_importance;
  }
  NM_INLINE float getTargetImportance() const { return m_targetImportance; }
  NM_INLINE const float& getTargetImportanceRef() const { return m_targetImportance; }
  NM_INLINE const SpatialTarget& getTarget() const { return target; }

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

protected:

  SpatialTarget target;  ///< pos, vel and normal of the target to place the hand against.
  float maxArmExtensionScale;  ///< 1 is full extension of the arm.  (Weight)
  float swivelAmount;  ///< Usually -1 to 1 refers to elbows in vs elbow out or visa versa.  (Multiplier)
  float handOffsetMultiplier;  /// Affects how far apart the hands are, default of 1 will place the hands at the same span as the shoulders.  (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignTarget(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxArmExtensionScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "maxArmExtensionScale");
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

  // importance values
  float
     m_targetImportance, 
     m_maxArmExtensionScaleImportance, 
     m_swivelAmountImportance, 
     m_handOffsetMultiplierImportance;

  friend class ArmPlacement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTargetImportance() > 0.0f)
    {
      target.validate();
    }
    if (getMaxArmExtensionScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(maxArmExtensionScale), "maxArmExtensionScale");
    }
    if (getSwivelAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    }
    if (getHandOffsetMultiplierImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(handOffsetMultiplier), "handOffsetMultiplier");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmPlacementOutputs
 * \ingroup ArmPlacement
 * \brief Outputs Place the arms in front of the target, to minimise danger of object and head/chest colliding
 *
 * Data packet definition (396 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmPlacementOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmPlacementOutputs));
  }

  NM_INLINE void setControl(const LimbControl& _control, float control_importance = 1.f)
  {
    control = _control;
    postAssignControl(control);
    NMP_ASSERT(control_importance >= 0.0f && control_importance <= 1.0f);
    m_controlImportance = control_importance;
  }
  NM_INLINE LimbControl& startControlModification()
  {
    m_controlImportance = -1.0f; // set invalid until stopControlModification()
    return control;
  }
  NM_INLINE void stopControlModification(float control_importance = 1.f)
  {
    postAssignControl(control);
    NMP_ASSERT(control_importance >= 0.0f && control_importance <= 1.0f);
    m_controlImportance = control_importance;
  }
  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

  NM_INLINE void setWristLimitReduction(const float& _wristLimitReduction, float wristLimitReduction_importance = 1.f)
  {
    wristLimitReduction = _wristLimitReduction;
    postAssignWristLimitReduction(wristLimitReduction);
    NMP_ASSERT(wristLimitReduction_importance >= 0.0f && wristLimitReduction_importance <= 1.0f);
    m_wristLimitReductionImportance = wristLimitReduction_importance;
  }
  NM_INLINE float getWristLimitReductionImportance() const { return m_wristLimitReductionImportance; }
  NM_INLINE const float& getWristLimitReductionImportanceRef() const { return m_wristLimitReductionImportance; }
  NM_INLINE const float& getWristLimitReduction() const { return wristLimitReduction; }

protected:

  LimbControl control;  ///< IK control of arm.
  float wristLimitReduction;  /// Controls a reduction in the wrist joint limit to prevent poor wrist alignment on falling backwards.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWristLimitReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "wristLimitReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_wristLimitReductionImportance;

  friend class ArmPlacement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
    if (getWristLimitReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(wristLimitReduction), "wristLimitReduction");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMPLACEMENT_DATA_H

