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

#ifndef NM_MDF_HEADDODGE_DATA_H
#define NM_MDF_HEADDODGE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadDodge.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/ProcessRequest.h"
#include "Types/DodgeHazard.h"
#include "Types/LimbControl.h"
#include "Types/TranslationRequest.h"
#include "Types/RotationRequest.h"

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
 * \class HeadDodgeData
 * \ingroup HeadDodge
 * \brief Data this module predicts the path of a moving hazard and moves laterally out of the way, either leaning or ducking
 *
 * Data packet definition (40 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadDodgeData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadDodgeData));
  }

  NMP::Vector3 headShift;  ///< Lateral shift in order to dodge  (PositionDelta)
  ProcessRequest process;  ///< Structure for processed request, to use update info in the feedback
  float stiffnessScale;  ///< 1 means use normal stiffness in the head dodge  (Multiplier)
  float dodgeable;  ///< Can object be dodged, if it is too big then shifting the head laterally would not help at all  (Weight)
  float importance;  ///< Used in the output control  (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(headShift), "headShift");
    process.validate();
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatNonNegative(dodgeable), "dodgeable");
    NM_VALIDATOR(FloatNonNegative(importance), "importance");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadDodgeInputs
 * \ingroup HeadDodge
 * \brief Inputs this module predicts the path of a moving hazard and moves laterally out of the way, either leaning or ducking
 *
 * Data packet definition (52 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadDodgeInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadDodgeInputs));
  }

  NM_INLINE void setDodgeHazard(const DodgeHazard& _dodgeHazard, float dodgeHazard_importance = 1.f)
  {
    dodgeHazard = _dodgeHazard;
    postAssignDodgeHazard(dodgeHazard);
    NMP_ASSERT(dodgeHazard_importance >= 0.0f && dodgeHazard_importance <= 1.0f);
    m_dodgeHazardImportance = dodgeHazard_importance;
  }
  NM_INLINE float getDodgeHazardImportance() const { return m_dodgeHazardImportance; }
  NM_INLINE const float& getDodgeHazardImportanceRef() const { return m_dodgeHazardImportance; }
  NM_INLINE const DodgeHazard& getDodgeHazard() const { return dodgeHazard; }

protected:

  DodgeHazard dodgeHazard;  ///< The hazard to dodge out the way of

  // post-assignment stubs
  NM_INLINE void postAssignDodgeHazard(const DodgeHazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_dodgeHazardImportance;

  friend class HeadDodge_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getDodgeHazardImportance() > 0.0f)
    {
      dodgeHazard.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadDodgeOutputs
 * \ingroup HeadDodge
 * \brief Outputs this module predicts the path of a moving hazard and moves laterally out of the way, either leaning or ducking
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadDodgeOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadDodgeOutputs));
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

  LimbControl control;  ///< IK head control

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

  friend class HeadDodge_Con;

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
 * \class HeadDodgeFeedbackOutputs
 * \ingroup HeadDodge
 * \brief FeedbackOutputs this module predicts the path of a moving hazard and moves laterally out of the way, either leaning or ducking
 *
 * Data packet definition (328 bytes, 352 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadDodgeFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadDodgeFeedbackOutputs));
  }

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

  TranslationRequest rootTranslationRequest;  // Request motion from the chest to aid in dodging
  RotationRequest rootRotationRequest;

  // post-assignment stubs
  NM_INLINE void postAssignRootTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
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

  // importance values
  float
     m_rootTranslationRequestImportance, 
     m_rootRotationRequestImportance;

  friend class HeadDodge_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRootTranslationRequestImportance() > 0.0f)
    {
      rootTranslationRequest.validate();
    }
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

#endif // NM_MDF_HEADDODGE_DATA_H

