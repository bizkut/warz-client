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

#ifndef NM_MDF_HEADPOINT_DATA_H
#define NM_MDF_HEADPOINT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadPoint.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/ProcessRequest.h"
#include "Types/TargetRequest.h"
#include "Types/RotationRequest.h"
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
 * \class HeadPointData
 * \ingroup HeadPoint
 * \brief Data looks at the specified target
 *
 * Data packet definition (112 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadPointData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadPointData));
  }

  ER::LimbTransform headTM;  ///< Smoothed transform
  NMP::Vector3 lastTarget;  ///< Used for persistence for smoothing  (Position)
  NMP::Vector3 targetRotation;  ///< Required rotation of head  (RotationVector)
  ProcessRequest process;  ///< Processed stiffness information from the request
  float lastTargetImportance;  ///< So it doesn't smooth when suddenly changing to a new target  (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(lastTarget), "lastTarget");
    NM_VALIDATOR(Vector3Valid(targetRotation), "targetRotation");
    process.validate();
    NM_VALIDATOR(FloatNonNegative(lastTargetImportance), "lastTargetImportance");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadPointInputs
 * \ingroup HeadPoint
 * \brief Inputs looks at the specified target
 *
 * Data packet definition (172 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadPointInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadPointInputs));
  }

  NM_INLINE void setTarget(const TargetRequest& _target, float target_importance = 1.f)
  {
    target = _target;
    postAssignTarget(target);
    NMP_ASSERT(target_importance >= 0.0f && target_importance <= 1.0f);
    m_targetImportance = target_importance;
  }
  NM_INLINE TargetRequest& startTargetModification()
  {
    m_targetImportance = -1.0f; // set invalid until stopTargetModification()
    return target;
  }
  NM_INLINE void stopTargetModification(float target_importance = 1.f)
  {
    postAssignTarget(target);
    NMP_ASSERT(target_importance >= 0.0f && target_importance <= 1.0f);
    m_targetImportance = target_importance;
  }
  NM_INLINE float getTargetImportance() const { return m_targetImportance; }
  NM_INLINE const float& getTargetImportanceRef() const { return m_targetImportance; }
  NM_INLINE const TargetRequest& getTarget() const { return target; }

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

  TargetRequest target;  ///< Requested look position
  float supportAmount;  ///< Is chest supported in any way  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
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
     m_targetImportance, 
     m_supportAmountImportance;

  friend class HeadPoint_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTargetImportance() > 0.0f)
    {
      target.validate();
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadPointOutputs
 * \ingroup HeadPoint
 * \brief Outputs looks at the specified target
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadPointOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadPointOutputs));
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

  LimbControl control;  ///< IK control of neck joints

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

  friend class HeadPoint_Con;

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
 * \class HeadPointFeedbackOutputs
 * \ingroup HeadPoint
 * \brief FeedbackOutputs looks at the specified target
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadPointFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadPointFeedbackOutputs));
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

protected:

  RotationRequest rootRotationRequest;  /// How much we want our root to be rotated to help us look

  // post-assignment stubs
  NM_INLINE void postAssignRootRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rootRotationRequestImportance;

  friend class HeadPoint_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRootRotationRequestImportance() > 0.0f)
    {
      rootRotationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADPOINT_DATA_H

