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

#ifndef NM_MDF_TYPE_SITPARAMETERS_H
#define NM_MDF_TYPE_SITPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Externs.types'
// Data Payload: 20 Bytes
struct SitParameters
{

  float minSitAmount;                        ///< (Weight)

  float minStandingBalanceAmount;  ///< Only attempt to sit when the standing balance amount is below this threshold.  (Weight)

  float armStepHeight;                       ///< (Length)

  float armStepTargetExtrapolationTime;  ///< How far into the future should the chest's velocity be extrapolated to determine the hand placement target position.  (TimePeriod)

  float armStepTargetSeparationFromBody;  ///< Arm target is placed at least this far away from the hips or shoulder.  (Length)


  // functions

  SitParameters();


  NM_INLINE void operator *= (const float fVal)
  {
    minSitAmount *= fVal;
    minStandingBalanceAmount *= fVal;
    armStepHeight *= fVal;
    armStepTargetExtrapolationTime *= fVal;
    armStepTargetSeparationFromBody *= fVal;
  }

  NM_INLINE SitParameters operator * (const float fVal) const
  {
    SitParameters result;
    result.minSitAmount = minSitAmount * fVal;
    result.minStandingBalanceAmount = minStandingBalanceAmount * fVal;
    result.armStepHeight = armStepHeight * fVal;
    result.armStepTargetExtrapolationTime = armStepTargetExtrapolationTime * fVal;
    result.armStepTargetSeparationFromBody = armStepTargetSeparationFromBody * fVal;
    return result;
  }

  NM_INLINE void operator += (const SitParameters& rhs)
  {
    minSitAmount += rhs.minSitAmount;
    minStandingBalanceAmount += rhs.minStandingBalanceAmount;
    armStepHeight += rhs.armStepHeight;
    armStepTargetExtrapolationTime += rhs.armStepTargetExtrapolationTime;
    armStepTargetSeparationFromBody += rhs.armStepTargetSeparationFromBody;
  }

  NM_INLINE SitParameters operator + (const SitParameters& rhs) const
  {
    SitParameters result;
    result.minSitAmount = minSitAmount + rhs.minSitAmount;
    result.minStandingBalanceAmount = minStandingBalanceAmount + rhs.minStandingBalanceAmount;
    result.armStepHeight = armStepHeight + rhs.armStepHeight;
    result.armStepTargetExtrapolationTime = armStepTargetExtrapolationTime + rhs.armStepTargetExtrapolationTime;
    result.armStepTargetSeparationFromBody = armStepTargetSeparationFromBody + rhs.armStepTargetSeparationFromBody;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(minSitAmount), "minSitAmount");
    NM_VALIDATOR(FloatNonNegative(minStandingBalanceAmount), "minStandingBalanceAmount");
    NM_VALIDATOR(FloatNonNegative(armStepHeight), "armStepHeight");
    NM_VALIDATOR(FloatValid(armStepTargetExtrapolationTime), "armStepTargetExtrapolationTime");
    NM_VALIDATOR(FloatNonNegative(armStepTargetSeparationFromBody), "armStepTargetSeparationFromBody");
#endif // NM_CALL_VALIDATORS
  }

}; // struct SitParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_SITPARAMETERS_H

