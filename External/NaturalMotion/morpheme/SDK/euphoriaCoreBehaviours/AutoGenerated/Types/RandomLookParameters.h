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

#ifndef NM_MDF_TYPE_RANDOMLOOKPARAMETERS_H
#define NM_MDF_TYPE_RANDOMLOOKPARAMETERS_H

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
// Data Payload: 32 Bytes
struct RandomLookParameters
{

  float lookTimescale;  ///< Scale time used to modify the look targets  (TimePeriod)

  float lookAmount;                          ///< (Weight)

  float lookRangeUpDown;                     ///< (Angle)

  float lookRangeSideways;                   ///< (Angle)

  float lookWithWholeBody;                   ///< (Weight)

  float lookFocusDistance;                   ///< (Length)

  float lookVerticalOffset;                  ///< (Distance)

  float lookTransitionTime;                  ///< (TimePeriod)



  NM_INLINE void operator *= (const float fVal)
  {
    lookTimescale *= fVal;
    lookAmount *= fVal;
    lookRangeUpDown *= fVal;
    lookRangeSideways *= fVal;
    lookWithWholeBody *= fVal;
    lookFocusDistance *= fVal;
    lookVerticalOffset *= fVal;
    lookTransitionTime *= fVal;
  }

  NM_INLINE RandomLookParameters operator * (const float fVal) const
  {
    RandomLookParameters result;
    result.lookTimescale = lookTimescale * fVal;
    result.lookAmount = lookAmount * fVal;
    result.lookRangeUpDown = lookRangeUpDown * fVal;
    result.lookRangeSideways = lookRangeSideways * fVal;
    result.lookWithWholeBody = lookWithWholeBody * fVal;
    result.lookFocusDistance = lookFocusDistance * fVal;
    result.lookVerticalOffset = lookVerticalOffset * fVal;
    result.lookTransitionTime = lookTransitionTime * fVal;
    return result;
  }

  NM_INLINE void operator += (const RandomLookParameters& rhs)
  {
    lookTimescale += rhs.lookTimescale;
    lookAmount += rhs.lookAmount;
    lookRangeUpDown += rhs.lookRangeUpDown;
    lookRangeSideways += rhs.lookRangeSideways;
    lookWithWholeBody += rhs.lookWithWholeBody;
    lookFocusDistance += rhs.lookFocusDistance;
    lookVerticalOffset += rhs.lookVerticalOffset;
    lookTransitionTime += rhs.lookTransitionTime;
  }

  NM_INLINE RandomLookParameters operator + (const RandomLookParameters& rhs) const
  {
    RandomLookParameters result;
    result.lookTimescale = lookTimescale + rhs.lookTimescale;
    result.lookAmount = lookAmount + rhs.lookAmount;
    result.lookRangeUpDown = lookRangeUpDown + rhs.lookRangeUpDown;
    result.lookRangeSideways = lookRangeSideways + rhs.lookRangeSideways;
    result.lookWithWholeBody = lookWithWholeBody + rhs.lookWithWholeBody;
    result.lookFocusDistance = lookFocusDistance + rhs.lookFocusDistance;
    result.lookVerticalOffset = lookVerticalOffset + rhs.lookVerticalOffset;
    result.lookTransitionTime = lookTransitionTime + rhs.lookTransitionTime;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(lookTimescale), "lookTimescale");
    NM_VALIDATOR(FloatNonNegative(lookAmount), "lookAmount");
    NM_VALIDATOR(FloatValid(lookRangeUpDown), "lookRangeUpDown");
    NM_VALIDATOR(FloatValid(lookRangeSideways), "lookRangeSideways");
    NM_VALIDATOR(FloatNonNegative(lookWithWholeBody), "lookWithWholeBody");
    NM_VALIDATOR(FloatNonNegative(lookFocusDistance), "lookFocusDistance");
    NM_VALIDATOR(FloatValid(lookVerticalOffset), "lookVerticalOffset");
    NM_VALIDATOR(FloatValid(lookTransitionTime), "lookTransitionTime");
#endif // NM_CALL_VALIDATORS
  }

}; // struct RandomLookParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_RANDOMLOOKPARAMETERS_H

