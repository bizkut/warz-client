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

#ifndef NM_MDF_ARMSPIN_DATA_H
#define NM_MDF_ARMSPIN_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmSpin.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/SpinParameters.h"
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
 * \class ArmSpinData
 * \ingroup ArmSpin
 * \brief Data a long term spin motion in order to control the orientation of the body
 *
 * Data packet definition (32 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSpinData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSpinData));
  }

  NMP::Vector3 targetTorque;                 ///< (Torque)
  NMP::Vector3 targetDirection;              ///< (Position)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(targetTorque), "targetTorque");
    NM_VALIDATOR(Vector3Valid(targetDirection), "targetDirection");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmSpinInputs
 * \ingroup ArmSpin
 * \brief Inputs a long term spin motion in order to control the orientation of the body
 *
 * Data packet definition (251 bytes, 256 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSpinInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSpinInputs));
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

  NM_INLINE void setCombinedTargetDirection(const NMP::Vector3& _combinedTargetDirection, float combinedTargetDirection_importance = 1.f)
  {
    combinedTargetDirection = _combinedTargetDirection;
    postAssignCombinedTargetDirection(combinedTargetDirection);
    NMP_ASSERT(combinedTargetDirection_importance >= 0.0f && combinedTargetDirection_importance <= 1.0f);
    m_combinedTargetDirectionImportance = combinedTargetDirection_importance;
  }
  NM_INLINE float getCombinedTargetDirectionImportance() const { return m_combinedTargetDirectionImportance; }
  NM_INLINE const float& getCombinedTargetDirectionImportanceRef() const { return m_combinedTargetDirectionImportance; }
  NM_INLINE const NMP::Vector3& getCombinedTargetDirection() const { return combinedTargetDirection; }

  NM_INLINE void setSpinParams(const SpinParameters& _spinParams, float spinParams_importance = 1.f)
  {
    spinParams = _spinParams;
    postAssignSpinParams(spinParams);
    NMP_ASSERT(spinParams_importance >= 0.0f && spinParams_importance <= 1.0f);
    m_spinParamsImportance = spinParams_importance;
  }
  NM_INLINE float getSpinParamsImportance() const { return m_spinParamsImportance; }
  NM_INLINE const float& getSpinParamsImportanceRef() const { return m_spinParamsImportance; }
  NM_INLINE const SpinParameters& getSpinParams() const { return spinParams; }

  NM_INLINE void setSpinAmount(const float& _spinAmount, float spinAmount_importance = 1.f)
  {
    spinAmount = _spinAmount;
    postAssignSpinAmount(spinAmount);
    NMP_ASSERT(spinAmount_importance >= 0.0f && spinAmount_importance <= 1.0f);
    m_spinAmountImportance = spinAmount_importance;
  }
  NM_INLINE float getSpinAmountImportance() const { return m_spinAmountImportance; }
  NM_INLINE const float& getSpinAmountImportanceRef() const { return m_spinAmountImportance; }
  NM_INLINE const float& getSpinAmount() const { return spinAmount; }

  NM_INLINE void setArmsSpinInPhase(const bool& _armsSpinInPhase, float armsSpinInPhase_importance = 1.f)
  {
    armsSpinInPhase = _armsSpinInPhase;
    NMP_ASSERT(armsSpinInPhase_importance >= 0.0f && armsSpinInPhase_importance <= 1.0f);
    m_armsSpinInPhaseImportance = armsSpinInPhase_importance;
  }
  NM_INLINE float getArmsSpinInPhaseImportance() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const float& getArmsSpinInPhaseImportanceRef() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const bool& getArmsSpinInPhase() const { return armsSpinInPhase; }

  NM_INLINE void setIgnoreContacts(const bool& _ignoreContacts, float ignoreContacts_importance = 1.f)
  {
    ignoreContacts = _ignoreContacts;
    NMP_ASSERT(ignoreContacts_importance >= 0.0f && ignoreContacts_importance <= 1.0f);
    m_ignoreContactsImportance = ignoreContacts_importance;
  }
  NM_INLINE float getIgnoreContactsImportance() const { return m_ignoreContactsImportance; }
  NM_INLINE const float& getIgnoreContactsImportanceRef() const { return m_ignoreContactsImportance; }
  NM_INLINE const bool& getIgnoreContacts() const { return ignoreContacts; }

protected:

  RotationRequest rotationRequest;  ///< Rotation this limb will try to achieve by spinning (in world space).
  NMP::Vector3 combinedTargetDirection;      ///< (Direction)
  SpinParameters spinParams;
  float spinAmount;                          ///< (Multiplier)
  bool armsSpinInPhase;
  bool ignoreContacts;  ///< spin action is supressed when rig is in contact unless this flag is set

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCombinedTargetDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "combinedTargetDirection");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinParams(const SpinParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spinAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance, 
     m_combinedTargetDirectionImportance, 
     m_spinParamsImportance, 
     m_spinAmountImportance, 
     m_armsSpinInPhaseImportance, 
     m_ignoreContactsImportance;

  friend class ArmSpin_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getCombinedTargetDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(combinedTargetDirection), "combinedTargetDirection");
    }
    if (getSpinParamsImportance() > 0.0f)
    {
      spinParams.validate();
    }
    if (getSpinAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(spinAmount), "spinAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmSpinOutputs
 * \ingroup ArmSpin
 * \brief Outputs a long term spin motion in order to control the orientation of the body
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSpinOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSpinOutputs));
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

  friend class ArmSpin_Con;

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
 * \class ArmSpinFeedbackOutputs
 * \ingroup ArmSpin
 * \brief FeedbackOutputs a long term spin motion in order to control the orientation of the body
 *
 * Data packet definition (20 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSpinFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSpinFeedbackOutputs));
  }

  NM_INLINE void setTargetDirection(const NMP::Vector3& _targetDirection, float targetDirection_importance = 1.f)
  {
    targetDirection = _targetDirection;
    postAssignTargetDirection(targetDirection);
    NMP_ASSERT(targetDirection_importance >= 0.0f && targetDirection_importance <= 1.0f);
    m_targetDirectionImportance = targetDirection_importance;
  }
  NM_INLINE float getTargetDirectionImportance() const { return m_targetDirectionImportance; }
  NM_INLINE const float& getTargetDirectionImportanceRef() const { return m_targetDirectionImportance; }
  NM_INLINE const NMP::Vector3& getTargetDirection() const { return targetDirection; }

protected:

  NMP::Vector3 targetDirection;  ///< Used for coordinating control between limbs  (Direction)

  // post-assignment stubs
  NM_INLINE void postAssignTargetDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "targetDirection");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_targetDirectionImportance;

  friend class ArmSpin_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTargetDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(targetDirection), "targetDirection");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSPIN_DATA_H

