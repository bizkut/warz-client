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

#ifndef NM_MDF_ARMHOLD_DATA_H
#define NM_MDF_ARMHOLD_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmHold.module"

// external types
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/HoldTimer.h"
#include "Types/EndConstraintControl.h"

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
 * \class ArmHoldData
 * \ingroup ArmHold
 * \brief Data hold on to another actor with the end-effector
 *
 * Data packet definition (116 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldData));
  }

  EndConstraintControl lastRequestedConstraint;
  HoldTimer timer;  ///< in data section because both update and feedback need access
  float timeHeld;                            ///< (TimePeriod)
  float timeNotHeld;                         ///< (TimePeriod)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    lastRequestedConstraint.validate();
    timer.validate();
    NM_VALIDATOR(FloatValid(timeHeld), "timeHeld");
    NM_VALIDATOR(FloatValid(timeNotHeld), "timeNotHeld");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmHoldInputs
 * \ingroup ArmHold
 * \brief Inputs hold on to another actor with the end-effector
 *
 * Data packet definition (116 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldInputs));
  }

  NM_INLINE void setEndConstraint(const EndConstraintControl& _endConstraint, float endConstraint_importance = 1.f)
  {
    endConstraint = _endConstraint;
    postAssignEndConstraint(endConstraint);
    NMP_ASSERT(endConstraint_importance >= 0.0f && endConstraint_importance <= 1.0f);
    m_endConstraintImportance = endConstraint_importance;
  }
  NM_INLINE EndConstraintControl& startEndConstraintModification()
  {
    m_endConstraintImportance = -1.0f; // set invalid until stopEndConstraintModification()
    return endConstraint;
  }
  NM_INLINE void stopEndConstraintModification(float endConstraint_importance = 1.f)
  {
    postAssignEndConstraint(endConstraint);
    NMP_ASSERT(endConstraint_importance >= 0.0f && endConstraint_importance <= 1.0f);
    m_endConstraintImportance = endConstraint_importance;
  }
  NM_INLINE float getEndConstraintImportance() const { return m_endConstraintImportance; }
  NM_INLINE const float& getEndConstraintImportanceRef() const { return m_endConstraintImportance; }
  NM_INLINE const EndConstraintControl& getEndConstraint() const { return endConstraint; }

  NM_INLINE void setTimer(const HoldTimer& _timer, float timer_importance = 1.f)
  {
    timer = _timer;
    postAssignTimer(timer);
    NMP_ASSERT(timer_importance >= 0.0f && timer_importance <= 1.0f);
    m_timerImportance = timer_importance;
  }
  NM_INLINE float getTimerImportance() const { return m_timerImportance; }
  NM_INLINE const float& getTimerImportanceRef() const { return m_timerImportance; }
  NM_INLINE const HoldTimer& getTimer() const { return timer; }

protected:

  EndConstraintControl endConstraint;
  HoldTimer timer;

  // post-assignment stubs
  NM_INLINE void postAssignEndConstraint(const EndConstraintControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTimer(const HoldTimer& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_endConstraintImportance, 
     m_timerImportance;

  friend class ArmHold_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEndConstraintImportance() > 0.0f)
    {
      endConstraint.validate();
    }
    if (getTimerImportance() > 0.0f)
    {
      timer.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmHoldOutputs
 * \ingroup ArmHold
 * \brief Outputs hold on to another actor with the end-effector
 *
 * Data packet definition (168 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldOutputs));
  }

  NM_INLINE void setEndConstraint(const EndConstraintControl& _endConstraint, float endConstraint_importance = 1.f)
  {
    endConstraint = _endConstraint;
    postAssignEndConstraint(endConstraint);
    NMP_ASSERT(endConstraint_importance >= 0.0f && endConstraint_importance <= 1.0f);
    m_endConstraintImportance = endConstraint_importance;
  }
  NM_INLINE EndConstraintControl& startEndConstraintModification()
  {
    m_endConstraintImportance = -1.0f; // set invalid until stopEndConstraintModification()
    return endConstraint;
  }
  NM_INLINE void stopEndConstraintModification(float endConstraint_importance = 1.f)
  {
    postAssignEndConstraint(endConstraint);
    NMP_ASSERT(endConstraint_importance >= 0.0f && endConstraint_importance <= 1.0f);
    m_endConstraintImportance = endConstraint_importance;
  }
  NM_INLINE float getEndConstraintImportance() const { return m_endConstraintImportance; }
  NM_INLINE const float& getEndConstraintImportanceRef() const { return m_endConstraintImportance; }
  NM_INLINE const EndConstraintControl& getEndConstraint() const { return endConstraint; }

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

protected:

  EndConstraintControl endConstraint;
  ER::HandFootTransform endDesiredTM;

  // post-assignment stubs
  NM_INLINE void postAssignEndConstraint(const EndConstraintControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_endConstraintImportance, 
     m_endDesiredTMImportance;

  friend class ArmHold_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEndConstraintImportance() > 0.0f)
    {
      endConstraint.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmHoldFeedbackOutputs
 * \ingroup ArmHold
 * \brief FeedbackOutputs hold on to another actor with the end-effector
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmHoldFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmHoldFeedbackOutputs));
  }

  NM_INLINE void setTimeHeldProportion(const float& _timeHeldProportion, float timeHeldProportion_importance = 1.f)
  {
    timeHeldProportion = _timeHeldProportion;
    postAssignTimeHeldProportion(timeHeldProportion);
    NMP_ASSERT(timeHeldProportion_importance >= 0.0f && timeHeldProportion_importance <= 1.0f);
    m_timeHeldProportionImportance = timeHeldProportion_importance;
  }
  NM_INLINE float getTimeHeldProportionImportance() const { return m_timeHeldProportionImportance; }
  NM_INLINE const float& getTimeHeldProportionImportanceRef() const { return m_timeHeldProportionImportance; }
  NM_INLINE const float& getTimeHeldProportion() const { return timeHeldProportion; }

protected:

  float timeHeldProportion;                  ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignTimeHeldProportion(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "timeHeldProportion");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_timeHeldProportionImportance;

  friend class ArmHold_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTimeHeldProportionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(timeHeldProportion), "timeHeldProportion");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMHOLD_DATA_H

