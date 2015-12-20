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

#ifndef NM_MDF_HEADAVOID_DATA_H
#define NM_MDF_HEADAVOID_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadAvoid.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/SpatialTarget.h"
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
 * \class HeadAvoidData
 * \ingroup HeadAvoid
 * \brief Data this module simply repositions the head to get out the way of a hazard
 *
 * Data packet definition (24 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadAvoidData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadAvoidData));
  }

  NMP::Vector3 headShift;  ///< Amount of translation required to avoid the target  (PositionDelta)
  float importance;  ///< Used on control  (Weight)
  float safeRadius;  ///< The radius at which no head movement is necessary, not the same as the equilibrium radius  (Length)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(headShift), "headShift");
    NM_VALIDATOR(FloatNonNegative(importance), "importance");
    NM_VALIDATOR(FloatNonNegative(safeRadius), "safeRadius");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadAvoidInputs
 * \ingroup HeadAvoid
 * \brief Inputs this module simply repositions the head to get out the way of a hazard
 *
 * Data packet definition (65 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadAvoidInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadAvoidInputs));
  }

  NM_INLINE void setAvoidTarget(const SpatialTarget& _avoidTarget, float avoidTarget_importance = 1.f)
  {
    avoidTarget = _avoidTarget;
    postAssignAvoidTarget(avoidTarget);
    NMP_ASSERT(avoidTarget_importance >= 0.0f && avoidTarget_importance <= 1.0f);
    m_avoidTargetImportance = avoidTarget_importance;
  }
  NM_INLINE float getAvoidTargetImportance() const { return m_avoidTargetImportance; }
  NM_INLINE const float& getAvoidTargetImportanceRef() const { return m_avoidTargetImportance; }
  NM_INLINE const SpatialTarget& getAvoidTarget() const { return avoidTarget; }

  NM_INLINE void setAvoidRadius(const float& _avoidRadius, float avoidRadius_importance = 1.f)
  {
    avoidRadius = _avoidRadius;
    postAssignAvoidRadius(avoidRadius);
    NMP_ASSERT(avoidRadius_importance >= 0.0f && avoidRadius_importance <= 1.0f);
    m_avoidRadiusImportance = avoidRadius_importance;
  }
  NM_INLINE float getAvoidRadiusImportance() const { return m_avoidRadiusImportance; }
  NM_INLINE const float& getAvoidRadiusImportanceRef() const { return m_avoidRadiusImportance; }
  NM_INLINE const float& getAvoidRadius() const { return avoidRadius; }

  NM_INLINE void setTwoSidedTarget(const bool& _twoSidedTarget, float twoSidedTarget_importance = 1.f)
  {
    twoSidedTarget = _twoSidedTarget;
    NMP_ASSERT(twoSidedTarget_importance >= 0.0f && twoSidedTarget_importance <= 1.0f);
    m_twoSidedTargetImportance = twoSidedTarget_importance;
  }
  NM_INLINE float getTwoSidedTargetImportance() const { return m_twoSidedTargetImportance; }
  NM_INLINE const float& getTwoSidedTargetImportanceRef() const { return m_twoSidedTargetImportance; }
  NM_INLINE const bool& getTwoSidedTarget() const { return twoSidedTarget; }

protected:

  SpatialTarget avoidTarget;  ///< Position, normal of target to avoid
  float avoidRadius;  ///< Max radius at which to avoid, equilibrium will be less than this radius   (Length)
  bool twoSidedTarget;  ///< Defaults to false. True means 2-sided plane

  // post-assignment stubs
  NM_INLINE void postAssignAvoidTarget(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvoidRadius(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "avoidRadius");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_avoidTargetImportance, 
     m_avoidRadiusImportance, 
     m_twoSidedTargetImportance;

  friend class HeadAvoid_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getAvoidTargetImportance() > 0.0f)
    {
      avoidTarget.validate();
    }
    if (getAvoidRadiusImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(avoidRadius), "avoidRadius");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadAvoidOutputs
 * \ingroup HeadAvoid
 * \brief Outputs this module simply repositions the head to get out the way of a hazard
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadAvoidOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadAvoidOutputs));
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

  LimbControl control;  ///< IK control of the neck joints

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

  friend class HeadAvoid_Con;

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
 * \class HeadAvoidFeedbackOutputs
 * \ingroup HeadAvoid
 * \brief FeedbackOutputs this module simply repositions the head to get out the way of a hazard
 *
 * Data packet definition (328 bytes, 352 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadAvoidFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadAvoidFeedbackOutputs));
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

  TranslationRequest rootTranslationRequest;  // Request motion from the chest, to aid in achieving the avoid task
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

  friend class HeadAvoid_Con;

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

#endif // NM_MDF_HEADAVOID_DATA_H

