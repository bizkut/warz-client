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

#ifndef NM_MDF_SPINEREACHREACTION_DATA_H
#define NM_MDF_SPINEREACHREACTION_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineReachReaction.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/ReachTarget.h"
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
 * \class SpineReachReactionInputs
 * \ingroup SpineReachReaction
 * \brief Inputs looks at the specified target
 *
 * Data packet definition (80 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineReachReactionInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineReachReactionInputs));
  }

  NM_INLINE void setReachTarget(const ReachTarget& _reachTarget, float reachTarget_importance = 1.f)
  {
    reachTarget = _reachTarget;
    postAssignReachTarget(reachTarget);
    NMP_ASSERT(reachTarget_importance >= 0.0f && reachTarget_importance <= 1.0f);
    m_reachTargetImportance = reachTarget_importance;
  }
  NM_INLINE float getReachTargetImportance() const { return m_reachTargetImportance; }
  NM_INLINE const float& getReachTargetImportanceRef() const { return m_reachTargetImportance; }
  NM_INLINE const ReachTarget& getReachTarget() const { return reachTarget; }

  NM_INLINE void setAvgReachingHandPos(const NMP::Vector3& _avgReachingHandPos, float avgReachingHandPos_importance = 1.f)
  {
    avgReachingHandPos = _avgReachingHandPos;
    postAssignAvgReachingHandPos(avgReachingHandPos);
    NMP_ASSERT(avgReachingHandPos_importance >= 0.0f && avgReachingHandPos_importance <= 1.0f);
    m_avgReachingHandPosImportance = avgReachingHandPos_importance;
  }
  NM_INLINE float getAvgReachingHandPosImportance() const { return m_avgReachingHandPosImportance; }
  NM_INLINE const float& getAvgReachingHandPosImportanceRef() const { return m_avgReachingHandPosImportance; }
  NM_INLINE const NMP::Vector3& getAvgReachingHandPos() const { return avgReachingHandPos; }

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

  ReachTarget reachTarget;
  NMP::Vector3 avgReachingHandPos;           ///< (Position)
  float supportAmount;                       ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignReachTarget(const ReachTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachingHandPos(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "avgReachingHandPos");
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
     m_reachTargetImportance, 
     m_avgReachingHandPosImportance, 
     m_supportAmountImportance;

  friend class SpineReachReaction_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getReachTargetImportance() > 0.0f)
    {
      reachTarget.validate();
    }
    if (getAvgReachingHandPosImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(avgReachingHandPos), "avgReachingHandPos");
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
 * \class SpineReachReactionOutputs
 * \ingroup SpineReachReaction
 * \brief Outputs looks at the specified target
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineReachReactionOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineReachReactionOutputs));
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

  friend class SpineReachReaction_Con;

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

#endif // NM_MDF_SPINEREACHREACTION_DATA_H

