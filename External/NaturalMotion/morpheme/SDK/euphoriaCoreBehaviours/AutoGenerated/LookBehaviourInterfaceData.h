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

#ifndef NM_MDF_LOOKBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_LOOKBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/LookBehaviourInterface.module"

// known types
#include "Types/TargetRequest.h"

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
 * \class LookBehaviourInterfaceOutputs
 * \ingroup LookBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LookBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LookBehaviourInterfaceOutputs));
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

protected:

  TargetRequest target;  ///< Basically the position to look at

  // post-assignment stubs
  NM_INLINE void postAssignTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_targetImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTargetImportance() > 0.0f)
    {
      target.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LOOKBEHAVIOURINTERFACE_DATA_H

