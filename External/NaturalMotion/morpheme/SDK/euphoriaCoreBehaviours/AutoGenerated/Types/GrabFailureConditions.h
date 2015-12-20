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

#ifndef NM_MDF_TYPE_GRABFAILURECONDITIONS_H
#define NM_MDF_TYPE_GRABFAILURECONDITIONS_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace ER { class DimensionalScaling; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Hold.types'
// Data Payload: 12 Bytes

//----------------------------------------------------------------------------------------------------------------------
// Determines conditions for aborting a hold attempt.
//----------------------------------------------------------------------------------------------------------------------

struct GrabFailureConditions
{

  float maxHandsBehindBackPeriod;  ///< If hands are constrained behind the back for this long.  (TimePeriod)

  float maxReachAttemptPeriod;  ///< If character has tried but failed to reach an edge for this long.  (TimePeriod)

  float minReachRecoveryPeriod;  ///< After a failed attempt, a new attempt isn't started for this long.  (TimePeriod)


  // functions

  void initialise(const ER::DimensionalScaling& scaling);


  NM_INLINE void operator *= (const float fVal)
  {
    maxHandsBehindBackPeriod *= fVal;
    maxReachAttemptPeriod *= fVal;
    minReachRecoveryPeriod *= fVal;
  }

  NM_INLINE GrabFailureConditions operator * (const float fVal) const
  {
    GrabFailureConditions result;
    result.maxHandsBehindBackPeriod = maxHandsBehindBackPeriod * fVal;
    result.maxReachAttemptPeriod = maxReachAttemptPeriod * fVal;
    result.minReachRecoveryPeriod = minReachRecoveryPeriod * fVal;
    return result;
  }

  NM_INLINE void operator += (const GrabFailureConditions& rhs)
  {
    maxHandsBehindBackPeriod += rhs.maxHandsBehindBackPeriod;
    maxReachAttemptPeriod += rhs.maxReachAttemptPeriod;
    minReachRecoveryPeriod += rhs.minReachRecoveryPeriod;
  }

  NM_INLINE GrabFailureConditions operator + (const GrabFailureConditions& rhs) const
  {
    GrabFailureConditions result;
    result.maxHandsBehindBackPeriod = maxHandsBehindBackPeriod + rhs.maxHandsBehindBackPeriod;
    result.maxReachAttemptPeriod = maxReachAttemptPeriod + rhs.maxReachAttemptPeriod;
    result.minReachRecoveryPeriod = minReachRecoveryPeriod + rhs.minReachRecoveryPeriod;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(maxHandsBehindBackPeriod), "maxHandsBehindBackPeriod");
    NM_VALIDATOR(FloatValid(maxReachAttemptPeriod), "maxReachAttemptPeriod");
    NM_VALIDATOR(FloatValid(minReachRecoveryPeriod), "minReachRecoveryPeriod");
#endif // NM_CALL_VALIDATORS
  }

}; // struct GrabFailureConditions


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_GRABFAILURECONDITIONS_H

