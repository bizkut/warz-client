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

#ifndef NM_MDF_ARMSTANDINGSUPPORT_DATA_H
#define NM_MDF_ARMSTANDINGSUPPORT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmStandingSupport.module"

// external types
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/BalanceParameters.h"
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
 * \class ArmStandingSupportData
 * \ingroup ArmStandingSupport
 * \brief Data Maintain a raised stance of the body relative to the ground, this module controls the position of the chest.
 *
 * Data packet definition (2 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmStandingSupportData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmStandingSupportData));
  }

  bool hasFeedbackRun;  /// Used to make support more keen to claim that it is supporting on the first update/feedback - so 
  /// that there isn't one frame of unsupported (which would result in other behaviours trying to run)
  bool lostContactWithGround;  /// Used to differentiate between landing after a step and landing after a fall.

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
 * \class ArmStandingSupportInputs
 * \ingroup ArmStandingSupport
 * \brief Inputs Maintain a raised stance of the body relative to the ground, this module controls the position of the chest.
 *
 * Data packet definition (318 bytes, 320 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmStandingSupportInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmStandingSupportInputs));
  }

  NM_INLINE void setRootDesiredTM(const ER::LimbTransform& _rootDesiredTM, float rootDesiredTM_importance = 1.f)
  {
    rootDesiredTM = _rootDesiredTM;
    NMP_ASSERT(rootDesiredTM_importance >= 0.0f && rootDesiredTM_importance <= 1.0f);
    m_rootDesiredTMImportance = rootDesiredTM_importance;
  }
  NM_INLINE ER::LimbTransform& startRootDesiredTMModification()
  {
    m_rootDesiredTMImportance = -1.0f; // set invalid until stopRootDesiredTMModification()
    return rootDesiredTM;
  }
  NM_INLINE void stopRootDesiredTMModification(float rootDesiredTM_importance = 1.f)
  {
    NMP_ASSERT(rootDesiredTM_importance >= 0.0f && rootDesiredTM_importance <= 1.0f);
    m_rootDesiredTMImportance = rootDesiredTM_importance;
  }
  NM_INLINE float getRootDesiredTMImportance() const { return m_rootDesiredTMImportance; }
  NM_INLINE const float& getRootDesiredTMImportanceRef() const { return m_rootDesiredTMImportance; }
  NM_INLINE const ER::LimbTransform& getRootDesiredTM() const { return rootDesiredTM; }

  NM_INLINE void setEndDesiredTM(const ER::HandFootTransform& _endDesiredTM, float endDesiredTM_importance = 1.f)
  {
    endDesiredTM = _endDesiredTM;
    NMP_ASSERT(endDesiredTM_importance >= 0.0f && endDesiredTM_importance <= 1.0f);
    m_endDesiredTMImportance = endDesiredTM_importance;
  }
  NM_INLINE ER::HandFootTransform& startEndDesiredTMModification()
  {
    m_endDesiredTMImportance = -1.0f; // set invalid until stopEndDesiredTMModification()
    return endDesiredTM;
  }
  NM_INLINE void stopEndDesiredTMModification(float endDesiredTM_importance = 1.f)
  {
    NMP_ASSERT(endDesiredTM_importance >= 0.0f && endDesiredTM_importance <= 1.0f);
    m_endDesiredTMImportance = endDesiredTM_importance;
  }
  NM_INLINE float getEndDesiredTMImportance() const { return m_endDesiredTMImportance; }
  NM_INLINE const float& getEndDesiredTMImportanceRef() const { return m_endDesiredTMImportance; }
  NM_INLINE const ER::HandFootTransform& getEndDesiredTM() const { return endDesiredTM; }

  NM_INLINE void setBalancePoseEndRelativeToRoot(const NMP::Matrix34& _balancePoseEndRelativeToRoot, float balancePoseEndRelativeToRoot_importance = 1.f)
  {
    balancePoseEndRelativeToRoot = _balancePoseEndRelativeToRoot;
    postAssignBalancePoseEndRelativeToRoot(balancePoseEndRelativeToRoot);
    NMP_ASSERT(balancePoseEndRelativeToRoot_importance >= 0.0f && balancePoseEndRelativeToRoot_importance <= 1.0f);
    m_balancePoseEndRelativeToRootImportance = balancePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getBalancePoseEndRelativeToRootImportance() const { return m_balancePoseEndRelativeToRootImportance; }
  NM_INLINE const float& getBalancePoseEndRelativeToRootImportanceRef() const { return m_balancePoseEndRelativeToRootImportance; }
  NM_INLINE const NMP::Matrix34& getBalancePoseEndRelativeToRoot() const { return balancePoseEndRelativeToRoot; }

  NM_INLINE void setBalanceParameters(const BalanceParameters& _balanceParameters, float balanceParameters_importance = 1.f)
  {
    balanceParameters = _balanceParameters;
    postAssignBalanceParameters(balanceParameters);
    NMP_ASSERT(balanceParameters_importance >= 0.0f && balanceParameters_importance <= 1.0f);
    m_balanceParametersImportance = balanceParameters_importance;
  }
  NM_INLINE BalanceParameters& startBalanceParametersModification()
  {
    m_balanceParametersImportance = -1.0f; // set invalid until stopBalanceParametersModification()
    return balanceParameters;
  }
  NM_INLINE void stopBalanceParametersModification(float balanceParameters_importance = 1.f)
  {
    postAssignBalanceParameters(balanceParameters);
    NMP_ASSERT(balanceParameters_importance >= 0.0f && balanceParameters_importance <= 1.0f);
    m_balanceParametersImportance = balanceParameters_importance;
  }
  NM_INLINE float getBalanceParametersImportance() const { return m_balanceParametersImportance; }
  NM_INLINE const float& getBalanceParametersImportanceRef() const { return m_balanceParametersImportance; }
  NM_INLINE const BalanceParameters& getBalanceParameters() const { return balanceParameters; }

  NM_INLINE void setStabiliseRootAmount(const float& _stabiliseRootAmount, float stabiliseRootAmount_importance = 1.f)
  {
    stabiliseRootAmount = _stabiliseRootAmount;
    postAssignStabiliseRootAmount(stabiliseRootAmount);
    NMP_ASSERT(stabiliseRootAmount_importance >= 0.0f && stabiliseRootAmount_importance <= 1.0f);
    m_stabiliseRootAmountImportance = stabiliseRootAmount_importance;
  }
  NM_INLINE float getStabiliseRootAmountImportance() const { return m_stabiliseRootAmountImportance; }
  NM_INLINE const float& getStabiliseRootAmountImportanceRef() const { return m_stabiliseRootAmountImportance; }
  NM_INLINE const float& getStabiliseRootAmount() const { return stabiliseRootAmount; }

  NM_INLINE void setSupportStrengthScale(const float& _supportStrengthScale, float supportStrengthScale_importance = 1.f)
  {
    supportStrengthScale = _supportStrengthScale;
    postAssignSupportStrengthScale(supportStrengthScale);
    NMP_ASSERT(supportStrengthScale_importance >= 0.0f && supportStrengthScale_importance <= 1.0f);
    m_supportStrengthScaleImportance = supportStrengthScale_importance;
  }
  NM_INLINE float getSupportStrengthScaleImportance() const { return m_supportStrengthScaleImportance; }
  NM_INLINE const float& getSupportStrengthScaleImportanceRef() const { return m_supportStrengthScaleImportance; }
  NM_INLINE const float& getSupportStrengthScale() const { return supportStrengthScale; }

  NM_INLINE void setEnableExternalSupport(const bool& _enableExternalSupport, float enableExternalSupport_importance = 1.f)
  {
    enableExternalSupport = _enableExternalSupport;
    NMP_ASSERT(enableExternalSupport_importance >= 0.0f && enableExternalSupport_importance <= 1.0f);
    m_enableExternalSupportImportance = enableExternalSupport_importance;
  }
  NM_INLINE float getEnableExternalSupportImportance() const { return m_enableExternalSupportImportance; }
  NM_INLINE const float& getEnableExternalSupportImportanceRef() const { return m_enableExternalSupportImportance; }
  NM_INLINE const bool& getEnableExternalSupport() const { return enableExternalSupport; }

  NM_INLINE void setEnableRootSupport(const bool& _enableRootSupport, float enableRootSupport_importance = 1.f)
  {
    enableRootSupport = _enableRootSupport;
    NMP_ASSERT(enableRootSupport_importance >= 0.0f && enableRootSupport_importance <= 1.0f);
    m_enableRootSupportImportance = enableRootSupport_importance;
  }
  NM_INLINE float getEnableRootSupportImportance() const { return m_enableRootSupportImportance; }
  NM_INLINE const float& getEnableRootSupportImportanceRef() const { return m_enableRootSupportImportance; }
  NM_INLINE const bool& getEnableRootSupport() const { return enableRootSupport; }

protected:

  ER::LimbTransform rootDesiredTM;  ///< Desired transform (world space) for the root of the limb.
  ER::HandFootTransform endDesiredTM;  ///< Desired transform (world space) for the end of the limb.
  NMP::Matrix34 balancePoseEndRelativeToRoot;  ///< Balance pose  (Transform)
  BalanceParameters balanceParameters;
  float stabiliseRootAmount;  /// If the limb root should be stabilised (e.g. to provide additional compensation whilst stepping).
  /// Can be > 0 but large values will lead to instability  (Multiplier)
  float supportStrengthScale;  /// Scaling on the strengths so that modules coordinating support etc can indicate if the 
  /// actions need to be stronger/weaker depending on how many limbs are acting.  (Weight)
  bool enableExternalSupport;  /// If enabled, the support module will inform rest of network that it is externally supported (e.g. when constrained), 
  /// in which case the balancer e.g. will know that this helps with balancing
  bool enableRootSupport;  ///< Use the root of the limb (i.e. the chest in the case of arms) for support.

  // post-assignment stubs
  NM_INLINE void postAssignBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "balancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceParameters(const BalanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStabiliseRootAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "stabiliseRootAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportStrengthScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportStrengthScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rootDesiredTMImportance, 
     m_endDesiredTMImportance, 
     m_balancePoseEndRelativeToRootImportance, 
     m_balanceParametersImportance, 
     m_stabiliseRootAmountImportance, 
     m_supportStrengthScaleImportance, 
     m_enableExternalSupportImportance, 
     m_enableRootSupportImportance;

  friend class ArmStandingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBalancePoseEndRelativeToRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(balancePoseEndRelativeToRoot), "balancePoseEndRelativeToRoot");
    }
    if (getBalanceParametersImportance() > 0.0f)
    {
      balanceParameters.validate();
    }
    if (getStabiliseRootAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(stabiliseRootAmount), "stabiliseRootAmount");
    }
    if (getSupportStrengthScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportStrengthScale), "supportStrengthScale");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmStandingSupportOutputs
 * \ingroup ArmStandingSupport
 * \brief Outputs Maintain a raised stance of the body relative to the ground, this module controls the position of the chest.
 *
 * Data packet definition (393 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmStandingSupportOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmStandingSupportOutputs));
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

  NM_INLINE void setUseFullEndJointRange(const bool& _useFullEndJointRange, float useFullEndJointRange_importance = 1.f)
  {
    useFullEndJointRange = _useFullEndJointRange;
    NMP_ASSERT(useFullEndJointRange_importance >= 0.0f && useFullEndJointRange_importance <= 1.0f);
    m_useFullEndJointRangeImportance = useFullEndJointRange_importance;
  }
  NM_INLINE float getUseFullEndJointRangeImportance() const { return m_useFullEndJointRangeImportance; }
  NM_INLINE const float& getUseFullEndJointRangeImportanceRef() const { return m_useFullEndJointRangeImportance; }
  NM_INLINE const bool& getUseFullEndJointRange() const { return useFullEndJointRange; }

protected:

  LimbControl control;
  bool useFullEndJointRange;

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_useFullEndJointRangeImportance;

  friend class ArmStandingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmStandingSupportFeedbackOutputs
 * \ingroup ArmStandingSupport
 * \brief FeedbackOutputs Maintain a raised stance of the body relative to the ground, this module controls the position of the chest.
 *
 * Data packet definition (49 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmStandingSupportFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmStandingSupportFeedbackOutputs));
  }

  NM_INLINE void setEndSupportPoint(const NMP::Vector3& _endSupportPoint, float endSupportPoint_importance = 1.f)
  {
    endSupportPoint = _endSupportPoint;
    postAssignEndSupportPoint(endSupportPoint);
    NMP_ASSERT(endSupportPoint_importance >= 0.0f && endSupportPoint_importance <= 1.0f);
    m_endSupportPointImportance = endSupportPoint_importance;
  }
  NM_INLINE float getEndSupportPointImportance() const { return m_endSupportPointImportance; }
  NM_INLINE const float& getEndSupportPointImportanceRef() const { return m_endSupportPointImportance; }
  NM_INLINE const NMP::Vector3& getEndSupportPoint() const { return endSupportPoint; }

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

  NM_INLINE void setChestControlAmount(const float& _chestControlAmount, float chestControlAmount_importance = 1.f)
  {
    chestControlAmount = _chestControlAmount;
    postAssignChestControlAmount(chestControlAmount);
    NMP_ASSERT(chestControlAmount_importance >= 0.0f && chestControlAmount_importance <= 1.0f);
    m_chestControlAmountImportance = chestControlAmount_importance;
  }
  NM_INLINE float getChestControlAmountImportance() const { return m_chestControlAmountImportance; }
  NM_INLINE const float& getChestControlAmountImportanceRef() const { return m_chestControlAmountImportance; }
  NM_INLINE const float& getChestControlAmount() const { return chestControlAmount; }

  NM_INLINE void setLowerRootDistance(const float& _lowerRootDistance, float lowerRootDistance_importance = 1.f)
  {
    lowerRootDistance = _lowerRootDistance;
    postAssignLowerRootDistance(lowerRootDistance);
    NMP_ASSERT(lowerRootDistance_importance >= 0.0f && lowerRootDistance_importance <= 1.0f);
    m_lowerRootDistanceImportance = lowerRootDistance_importance;
  }
  NM_INLINE float getLowerRootDistanceImportance() const { return m_lowerRootDistanceImportance; }
  NM_INLINE const float& getLowerRootDistanceImportanceRef() const { return m_lowerRootDistanceImportance; }
  NM_INLINE const float& getLowerRootDistance() const { return lowerRootDistance; }

  NM_INLINE void setSupportedByConstraint(const bool& _supportedByConstraint, float supportedByConstraint_importance = 1.f)
  {
    supportedByConstraint = _supportedByConstraint;
    NMP_ASSERT(supportedByConstraint_importance >= 0.0f && supportedByConstraint_importance <= 1.0f);
    m_supportedByConstraintImportance = supportedByConstraint_importance;
  }
  NM_INLINE float getSupportedByConstraintImportance() const { return m_supportedByConstraintImportance; }
  NM_INLINE const float& getSupportedByConstraintImportanceRef() const { return m_supportedByConstraintImportance; }
  NM_INLINE const bool& getSupportedByConstraint() const { return supportedByConstraint; }

protected:

  NMP::Vector3 endSupportPoint;              ///< (Position)
  float supportAmount;  /// how much the armSupport is operating, 1 if armSupport is operating, even if it has only loose control of chest.  (Weight)
  float chestControlAmount;  /// how much chest control due to arm strength, 0 up to 1 if arm can control chest with normal stiffness.  (WeightWithClamp)
  float lowerRootDistance;                   ///< (Distance)
  bool supportedByConstraint;

  // post-assignment stubs
  NM_INLINE void postAssignEndSupportPoint(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "endSupportPoint");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestControlAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "chestControlAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerRootDistance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "lowerRootDistance");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_endSupportPointImportance, 
     m_supportAmountImportance, 
     m_chestControlAmountImportance, 
     m_lowerRootDistanceImportance, 
     m_supportedByConstraintImportance;

  friend class ArmStandingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEndSupportPointImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(endSupportPoint), "endSupportPoint");
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
    if (getChestControlAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(chestControlAmount), "chestControlAmount");
    }
    if (getLowerRootDistanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(lowerRootDistance), "lowerRootDistance");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSTANDINGSUPPORT_DATA_H

