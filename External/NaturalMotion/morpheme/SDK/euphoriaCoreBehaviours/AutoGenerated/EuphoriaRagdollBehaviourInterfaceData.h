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

#ifndef NM_MDF_EUPHORIARAGDOLLBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_EUPHORIARAGDOLLBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/EuphoriaRagdollBehaviourInterface.module"

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
 * \class EuphoriaRagdollBehaviourInterfaceOutputs
 * \ingroup EuphoriaRagdollBehaviourInterface
 * \brief Outputs This behaviour currently is just used to retrieve the eye vector or position, which can then be set in the animation
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EuphoriaRagdollBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EuphoriaRagdollBehaviourInterfaceOutputs));
  }

  NM_INLINE void setSleepingDisabled(const bool& _sleepingDisabled, float sleepingDisabled_importance = 1.f)
  {
    sleepingDisabled = _sleepingDisabled;
    NMP_ASSERT(sleepingDisabled_importance >= 0.0f && sleepingDisabled_importance <= 1.0f);
    m_sleepingDisabledImportance = sleepingDisabled_importance;
  }
  NM_INLINE float getSleepingDisabledImportance() const { return m_sleepingDisabledImportance; }
  NM_INLINE const float& getSleepingDisabledImportanceRef() const { return m_sleepingDisabledImportance; }
  NM_INLINE const bool& getSleepingDisabled() const { return sleepingDisabled; }

protected:

  bool sleepingDisabled;  ///< Is sleeping disabled when stationary

  // importance values
  float
     m_sleepingDisabledImportance;


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

#endif // NM_MDF_EUPHORIARAGDOLLBEHAVIOURINTERFACE_DATA_H

