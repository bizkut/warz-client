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

#ifndef NM_MDF_TYPE_HOLDTIMER_H
#define NM_MDF_TYPE_HOLDTIMER_H

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
// Manages an EndConstraint over time.
//----------------------------------------------------------------------------------------------------------------------

struct HoldTimer
{

  float minHoldPeriod;  ///< The constraint will exist at least for this long.  (TimePeriod)

  float maxHoldPeriod;  ///< The constraint will exist no longer than this.  (TimePeriod)

  float noHoldPeriod;  ///< After a constraint is released, a new one can't be created during this period.  (TimePeriod)


  // functions

  void initialise(const ER::DimensionalScaling& scaling);


  NM_INLINE void operator *= (const float fVal)
  {
    minHoldPeriod *= fVal;
    maxHoldPeriod *= fVal;
    noHoldPeriod *= fVal;
  }

  NM_INLINE HoldTimer operator * (const float fVal) const
  {
    HoldTimer result;
    result.minHoldPeriod = minHoldPeriod * fVal;
    result.maxHoldPeriod = maxHoldPeriod * fVal;
    result.noHoldPeriod = noHoldPeriod * fVal;
    return result;
  }

  NM_INLINE void operator += (const HoldTimer& rhs)
  {
    minHoldPeriod += rhs.minHoldPeriod;
    maxHoldPeriod += rhs.maxHoldPeriod;
    noHoldPeriod += rhs.noHoldPeriod;
  }

  NM_INLINE HoldTimer operator + (const HoldTimer& rhs) const
  {
    HoldTimer result;
    result.minHoldPeriod = minHoldPeriod + rhs.minHoldPeriod;
    result.maxHoldPeriod = maxHoldPeriod + rhs.maxHoldPeriod;
    result.noHoldPeriod = noHoldPeriod + rhs.noHoldPeriod;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(minHoldPeriod), "minHoldPeriod");
    NM_VALIDATOR(FloatValid(maxHoldPeriod), "maxHoldPeriod");
    NM_VALIDATOR(FloatValid(noHoldPeriod), "noHoldPeriod");
#endif // NM_CALL_VALIDATORS
  }

}; // struct HoldTimer


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_HOLDTIMER_H

