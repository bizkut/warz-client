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

#ifndef NM_MDF_SPINECONTROL_DATA_H
#define NM_MDF_SPINECONTROL_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineControl.module"

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
 * \class SpineControlInputs
 * \ingroup SpineControl
 * \brief Inputs Basic control of the spine with local orientation and position offsets
 *
 * Data packet definition (664 bytes, 672 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineControlInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineControlInputs));
  }

  NM_INLINE void setEndRotationRequest(const RotationRequest& _endRotationRequest, float endRotationRequest_importance = 1.f)
  {
    endRotationRequest = _endRotationRequest;
    postAssignEndRotationRequest(endRotationRequest);
    NMP_ASSERT(endRotationRequest_importance >= 0.0f && endRotationRequest_importance <= 1.0f);
    m_endRotationRequestImportance = endRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startEndRotationRequestModification()
  {
    m_endRotationRequestImportance = -1.0f; // set invalid until stopEndRotationRequestModification()
    return endRotationRequest;
  }
  NM_INLINE void stopEndRotationRequestModification(float endRotationRequest_importance = 1.f)
  {
    postAssignEndRotationRequest(endRotationRequest);
    NMP_ASSERT(endRotationRequest_importance >= 0.0f && endRotationRequest_importance <= 1.0f);
    m_endRotationRequestImportance = endRotationRequest_importance;
  }
  NM_INLINE float getEndRotationRequestImportance() const { return m_endRotationRequestImportance; }
  NM_INLINE const float& getEndRotationRequestImportanceRef() const { return m_endRotationRequestImportance; }
  NM_INLINE const RotationRequest& getEndRotationRequest() const { return endRotationRequest; }

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

  NM_INLINE void setEndTranslationRequest(const TranslationRequest& _endTranslationRequest, float endTranslationRequest_importance = 1.f)
  {
    endTranslationRequest = _endTranslationRequest;
    postAssignEndTranslationRequest(endTranslationRequest);
    NMP_ASSERT(endTranslationRequest_importance >= 0.0f && endTranslationRequest_importance <= 1.0f);
    m_endTranslationRequestImportance = endTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startEndTranslationRequestModification()
  {
    m_endTranslationRequestImportance = -1.0f; // set invalid until stopEndTranslationRequestModification()
    return endTranslationRequest;
  }
  NM_INLINE void stopEndTranslationRequestModification(float endTranslationRequest_importance = 1.f)
  {
    postAssignEndTranslationRequest(endTranslationRequest);
    NMP_ASSERT(endTranslationRequest_importance >= 0.0f && endTranslationRequest_importance <= 1.0f);
    m_endTranslationRequestImportance = endTranslationRequest_importance;
  }
  NM_INLINE float getEndTranslationRequestImportance() const { return m_endTranslationRequestImportance; }
  NM_INLINE const float& getEndTranslationRequestImportanceRef() const { return m_endTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getEndTranslationRequest() const { return endTranslationRequest; }

  NM_INLINE void setRootTranslationRequest(const TranslationRequest& _rootTranslationRequest, float rootTranslationRequest_importance = 1.f)
  {
    rootTranslationRequest = _rootTranslationRequest;
    postAssignRootTranslationRequest(rootTranslationRequest);
    NMP_ASSERT(rootTranslationRequest_importance >= 0.0f && rootTranslationRequest_importance <= 1.0f);
    m_rootTranslationRequestImportance = rootTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startRootTranslationRequestModification()
  {
    m_rootTranslationRequestImportance = -1.0f; // set invalid until stopRootTranslationRequestModification()
    return rootTranslationRequest;
  }
  NM_INLINE void stopRootTranslationRequestModification(float rootTranslationRequest_importance = 1.f)
  {
    postAssignRootTranslationRequest(rootTranslationRequest);
    NMP_ASSERT(rootTranslationRequest_importance >= 0.0f && rootTranslationRequest_importance <= 1.0f);
    m_rootTranslationRequestImportance = rootTranslationRequest_importance;
  }
  NM_INLINE float getRootTranslationRequestImportance() const { return m_rootTranslationRequestImportance; }
  NM_INLINE const float& getRootTranslationRequestImportanceRef() const { return m_rootTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getRootTranslationRequest() const { return rootTranslationRequest; }

  NM_INLINE void setDampingIncrease(const float& _dampingIncrease, float dampingIncrease_importance = 1.f)
  {
    dampingIncrease = _dampingIncrease;
    postAssignDampingIncrease(dampingIncrease);
    NMP_ASSERT(dampingIncrease_importance >= 0.0f && dampingIncrease_importance <= 1.0f);
    m_dampingIncreaseImportance = dampingIncrease_importance;
  }
  NM_INLINE float getDampingIncreaseImportance() const { return m_dampingIncreaseImportance; }
  NM_INLINE const float& getDampingIncreaseImportanceRef() const { return m_dampingIncreaseImportance; }
  NM_INLINE const float& getDampingIncrease() const { return dampingIncrease; }

protected:

  RotationRequest endRotationRequest;  ///< Desired rotation of the chest
  RotationRequest rootRotationRequest;  ///< Desired rotation of the pelvis
  TranslationRequest endTranslationRequest;  ///< Desired translation of the chest
  TranslationRequest rootTranslationRequest;  ///< Desired translation of the pelvis
  float dampingIncrease;  ///< 0 is normal damping  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignEndRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRootRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRootTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDampingIncrease(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "dampingIncrease");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_endRotationRequestImportance, 
     m_rootRotationRequestImportance, 
     m_endTranslationRequestImportance, 
     m_rootTranslationRequestImportance, 
     m_dampingIncreaseImportance;

  friend class SpineControl_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEndRotationRequestImportance() > 0.0f)
    {
      endRotationRequest.validate();
    }
    if (getRootRotationRequestImportance() > 0.0f)
    {
      rootRotationRequest.validate();
    }
    if (getEndTranslationRequestImportance() > 0.0f)
    {
      endTranslationRequest.validate();
    }
    if (getRootTranslationRequestImportance() > 0.0f)
    {
      rootTranslationRequest.validate();
    }
    if (getDampingIncreaseImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(dampingIncrease), "dampingIncrease");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineControlOutputs
 * \ingroup SpineControl
 * \brief Outputs Basic control of the spine with local orientation and position offsets
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineControlOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineControlOutputs));
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

  LimbControl control;  ///< IK control of the spine

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

  friend class SpineControl_Con;

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

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINECONTROL_DATA_H

