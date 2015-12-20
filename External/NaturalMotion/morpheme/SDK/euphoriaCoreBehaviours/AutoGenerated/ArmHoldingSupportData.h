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

#ifndef NM_MDF_ARMHOLDINGSUPPORT_DATA_H
#define NM_MDF_ARMHOLDINGSUPPORT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmHoldingSupport.module"

// external types
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
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
 * \class ArmHoldingSupportData
 * \ingroup ArmHoldingSupport
 * \brief Data Drives the position of the chest using 1 or more constrained arms.
 *
 * Data packet definition (1 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldingSupportData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldingSupportData));
  }

  bool hasFeedbackRun;  /// Used to make support more keen to claim that it is supporting on the first update/feedback - so
  /// that there isn't one frame of unsupported (which would result in other behaviours trying to run).

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
 * \class ArmHoldingSupportInputs
 * \ingroup ArmHoldingSupport
 * \brief Inputs Drives the position of the chest using 1 or more constrained arms.
 *
 * Data packet definition (472 bytes, 480 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldingSupportInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldingSupportInputs));
  }

  NM_INLINE void setRotationRequest(const RotationRequest& _rotationRequest, float rotationRequest_importance = 1.f)
  {
    rotationRequest = _rotationRequest;
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE RotationRequest& startRotationRequestModification()
  {
    m_rotationRequestImportance = -1.0f; // set invalid until stopRotationRequestModification()
    return rotationRequest;
  }
  NM_INLINE void stopRotationRequestModification(float rotationRequest_importance = 1.f)
  {
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE float getRotationRequestImportance() const { return m_rotationRequestImportance; }
  NM_INLINE const float& getRotationRequestImportanceRef() const { return m_rotationRequestImportance; }
  NM_INLINE const RotationRequest& getRotationRequest() const { return rotationRequest; }

  NM_INLINE void setTranslationRequest(const TranslationRequest& _translationRequest, float translationRequest_importance = 1.f)
  {
    translationRequest = _translationRequest;
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE TranslationRequest& startTranslationRequestModification()
  {
    m_translationRequestImportance = -1.0f; // set invalid until stopTranslationRequestModification()
    return translationRequest;
  }
  NM_INLINE void stopTranslationRequestModification(float translationRequest_importance = 1.f)
  {
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE float getTranslationRequestImportance() const { return m_translationRequestImportance; }
  NM_INLINE const float& getTranslationRequestImportanceRef() const { return m_translationRequestImportance; }
  NM_INLINE const TranslationRequest& getTranslationRequest() const { return translationRequest; }

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

protected:

  RotationRequest rotationRequest;  /// Used to control the support when it is hanging/holding. This allows the hold to override conflicting requests such
  /// as from headAvoid etc.
  TranslationRequest translationRequest;
  ER::LimbTransform rootDesiredTM;  ///< Desired transform (world space) for the root of the limb.
  ER::HandFootTransform endDesiredTM;  ///< Desired transform (world space) for the end of the limb.
  float supportStrengthScale;  /// Scaling on the strengths so that modules coordinating support etc can indicate if the
  /// actions need to be stronger/weaker depending on how many limbs are acting.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
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
     m_rotationRequestImportance, 
     m_translationRequestImportance, 
     m_rootDesiredTMImportance, 
     m_endDesiredTMImportance, 
     m_supportStrengthScaleImportance;

  friend class ArmHoldingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
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
 * \class ArmHoldingSupportOutputs
 * \ingroup ArmHoldingSupport
 * \brief Outputs Drives the position of the chest using 1 or more constrained arms.
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldingSupportOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldingSupportOutputs));
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

protected:

  LimbControl control;

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance;

  friend class ArmHoldingSupport_Con;

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
 * \class ArmHoldingSupportFeedbackOutputs
 * \ingroup ArmHoldingSupport
 * \brief FeedbackOutputs Drives the position of the chest using 1 or more constrained arms.
 *
 * Data packet definition (185 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldingSupportFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldingSupportFeedbackOutputs));
  }

  NM_INLINE void setRootRotationRequest(const RotationRequest& _rootRotationRequest, float rootRotationRequest_importance = 1.f)
  {
    rootRotationRequest = _rootRotationRequest;
    postAssignRootRotationRequest(rootRotationRequest);
    NMP_ASSERT(rootRotationRequest_importance >= 0.0f && rootRotationRequest_importance <= 1.0f);
    m_rootRotationRequestImportance = rootRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startRootRotationRequestModification()
  {
    m_rootRotationRequestImportance = -1.0f; // set invalid until stopRootRotationRequestModification()
    return rootRotationRequest;
  }
  NM_INLINE void stopRootRotationRequestModification(float rootRotationRequest_importance = 1.f)
  {
    postAssignRootRotationRequest(rootRotationRequest);
    NMP_ASSERT(rootRotationRequest_importance >= 0.0f && rootRotationRequest_importance <= 1.0f);
    m_rootRotationRequestImportance = rootRotationRequest_importance;
  }
  NM_INLINE float getRootRotationRequestImportance() const { return m_rootRotationRequestImportance; }
  NM_INLINE const float& getRootRotationRequestImportanceRef() const { return m_rootRotationRequestImportance; }
  NM_INLINE const RotationRequest& getRootRotationRequest() const { return rootRotationRequest; }

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

  RotationRequest rootRotationRequest;
  float supportAmount;  /// How much the armSupport is operating, 1 if armSupport is operating, even if it has only loose control of chest.  (Weight)
  float chestControlAmount;  /// How much chest control due to arm strength, 0 up to 1 if arm can control chest with normal stiffness.  (WeightWithClamp)
  bool supportedByConstraint;

  // post-assignment stubs
  NM_INLINE void postAssignRootRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignChestControlAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "chestControlAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rootRotationRequestImportance, 
     m_supportAmountImportance, 
     m_chestControlAmountImportance, 
     m_supportedByConstraintImportance;

  friend class ArmHoldingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRootRotationRequestImportance() > 0.0f)
    {
      rootRotationRequest.validate();
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
    if (getChestControlAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(chestControlAmount), "chestControlAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMHOLDINGSUPPORT_DATA_H

