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

#ifndef NM_MDF_TYPE_GRABENABLECONDITIONS_H
#define NM_MDF_TYPE_GRABENABLECONDITIONS_H

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
// Data Payload: 20 Bytes

//----------------------------------------------------------------------------------------------------------------------
// Determines under which conditions the character will attempt to hold.
//----------------------------------------------------------------------------------------------------------------------

struct GrabEnableConditions
{

  float minSupportSlope;  ///< Ground normals more upright are considered supportive, so no hold required.  (Angle)

  float verticalSpeedToStart;  ///< Falling speed needs to be bigger to attempt a hold.  (Speed)

  float verticalSpeedToStop;  ///< Falling speed needs to be smaller to abort the hold attempt.  (Speed)

  float unbalancedAmount;  ///< Amounts greater than this for a period of time are considered supported (no grab).  (Weight)

  float minUnbalancedPeriod;  ///< See minBalanceAmount.  (TimePeriod)


  // functions

  void initialise(const ER::DimensionalScaling& scaling);


  NM_INLINE void operator *= (const float fVal)
  {
    minSupportSlope *= fVal;
    verticalSpeedToStart *= fVal;
    verticalSpeedToStop *= fVal;
    unbalancedAmount *= fVal;
    minUnbalancedPeriod *= fVal;
  }

  NM_INLINE GrabEnableConditions operator * (const float fVal) const
  {
    GrabEnableConditions result;
    result.minSupportSlope = minSupportSlope * fVal;
    result.verticalSpeedToStart = verticalSpeedToStart * fVal;
    result.verticalSpeedToStop = verticalSpeedToStop * fVal;
    result.unbalancedAmount = unbalancedAmount * fVal;
    result.minUnbalancedPeriod = minUnbalancedPeriod * fVal;
    return result;
  }

  NM_INLINE void operator += (const GrabEnableConditions& rhs)
  {
    minSupportSlope += rhs.minSupportSlope;
    verticalSpeedToStart += rhs.verticalSpeedToStart;
    verticalSpeedToStop += rhs.verticalSpeedToStop;
    unbalancedAmount += rhs.unbalancedAmount;
    minUnbalancedPeriod += rhs.minUnbalancedPeriod;
  }

  NM_INLINE GrabEnableConditions operator + (const GrabEnableConditions& rhs) const
  {
    GrabEnableConditions result;
    result.minSupportSlope = minSupportSlope + rhs.minSupportSlope;
    result.verticalSpeedToStart = verticalSpeedToStart + rhs.verticalSpeedToStart;
    result.verticalSpeedToStop = verticalSpeedToStop + rhs.verticalSpeedToStop;
    result.unbalancedAmount = unbalancedAmount + rhs.unbalancedAmount;
    result.minUnbalancedPeriod = minUnbalancedPeriod + rhs.minUnbalancedPeriod;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(minSupportSlope), "minSupportSlope");
    NM_VALIDATOR(FloatValid(verticalSpeedToStart), "verticalSpeedToStart");
    NM_VALIDATOR(FloatValid(verticalSpeedToStop), "verticalSpeedToStop");
    NM_VALIDATOR(FloatNonNegative(unbalancedAmount), "unbalancedAmount");
    NM_VALIDATOR(FloatValid(minUnbalancedPeriod), "minUnbalancedPeriod");
#endif // NM_CALL_VALIDATORS
  }

}; // struct GrabEnableConditions


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_GRABENABLECONDITIONS_H

