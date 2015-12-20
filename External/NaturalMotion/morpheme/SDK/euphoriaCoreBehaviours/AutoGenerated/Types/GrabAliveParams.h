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

#ifndef NM_MDF_TYPE_GRABALIVEPARAMS_H
#define NM_MDF_TYPE_GRABALIVEPARAMS_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

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
// Declaration from 'Hold.types'
// Data Payload: 8 Bytes

//----------------------------------------------------------------------------------------------------------------------
// Describes how the character behaves while stably hanging from an edge.
//----------------------------------------------------------------------------------------------------------------------

struct GrabAliveParams
{

  float pullUpAmount;  //< How high up the character tries to pull himself. 1: to chest level, 0: no pulling.  (Weight)

  float pullUpStrengthScale;  //< How strong the arms will try to pull up.  (Weight)


  // functions

  void initialise();


  NM_INLINE void operator *= (const float fVal)
  {
    pullUpAmount *= fVal;
    pullUpStrengthScale *= fVal;
  }

  NM_INLINE GrabAliveParams operator * (const float fVal) const
  {
    GrabAliveParams result;
    result.pullUpAmount = pullUpAmount * fVal;
    result.pullUpStrengthScale = pullUpStrengthScale * fVal;
    return result;
  }

  NM_INLINE void operator += (const GrabAliveParams& rhs)
  {
    pullUpAmount += rhs.pullUpAmount;
    pullUpStrengthScale += rhs.pullUpStrengthScale;
  }

  NM_INLINE GrabAliveParams operator + (const GrabAliveParams& rhs) const
  {
    GrabAliveParams result;
    result.pullUpAmount = pullUpAmount + rhs.pullUpAmount;
    result.pullUpStrengthScale = pullUpStrengthScale + rhs.pullUpStrengthScale;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(pullUpAmount), "pullUpAmount");
    NM_VALIDATOR(FloatNonNegative(pullUpStrengthScale), "pullUpStrengthScale");
#endif // NM_CALL_VALIDATORS
  }

}; // struct GrabAliveParams


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_GRABALIVEPARAMS_H

