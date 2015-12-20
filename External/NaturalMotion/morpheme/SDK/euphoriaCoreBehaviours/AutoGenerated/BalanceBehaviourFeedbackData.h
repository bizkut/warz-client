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

#ifndef NM_MDF_BALANCEBEHAVIOURFEEDBACK_DATA_H
#define NM_MDF_BALANCEBEHAVIOURFEEDBACK_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BalanceBehaviourFeedback.module"

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
 * \class BalanceBehaviourFeedbackFeedbackOutputs
 * \ingroup BalanceBehaviourFeedback
 * \brief FeedbackOutputs Informs modules about the balance abilities on the feedback pass
 *
 * Data packet definition (10 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceBehaviourFeedbackFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceBehaviourFeedbackFeedbackOutputs));
  }

  NM_INLINE void setSupportWithLegs(const bool& _supportWithLegs, float supportWithLegs_importance = 1.f)
  {
    supportWithLegs = _supportWithLegs;
    NMP_ASSERT(supportWithLegs_importance >= 0.0f && supportWithLegs_importance <= 1.0f);
    m_supportWithLegsImportance = supportWithLegs_importance;
  }
  NM_INLINE float getSupportWithLegsImportance() const { return m_supportWithLegsImportance; }
  NM_INLINE const float& getSupportWithLegsImportanceRef() const { return m_supportWithLegsImportance; }
  NM_INLINE const bool& getSupportWithLegs() const { return supportWithLegs; }

  NM_INLINE void setSupportWithArms(const bool& _supportWithArms, float supportWithArms_importance = 1.f)
  {
    supportWithArms = _supportWithArms;
    NMP_ASSERT(supportWithArms_importance >= 0.0f && supportWithArms_importance <= 1.0f);
    m_supportWithArmsImportance = supportWithArms_importance;
  }
  NM_INLINE float getSupportWithArmsImportance() const { return m_supportWithArmsImportance; }
  NM_INLINE const float& getSupportWithArmsImportanceRef() const { return m_supportWithArmsImportance; }
  NM_INLINE const bool& getSupportWithArms() const { return supportWithArms; }

protected:

  bool supportWithLegs;
  bool supportWithArms;

  // importance values
  float
     m_supportWithLegsImportance, 
     m_supportWithArmsImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEBEHAVIOURFEEDBACK_DATA_H

