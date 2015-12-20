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

#ifndef NM_MDF_TYPE_GRABCHESTPARAMS_H
#define NM_MDF_TYPE_GRABCHESTPARAMS_H

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
// Data Payload: 12 Bytes

//----------------------------------------------------------------------------------------------------------------------
// Settings for trying to position/orientate the chest when grabbing/holding
//----------------------------------------------------------------------------------------------------------------------

struct GrabChestParams
{

  float imminence;                           ///< (Imminence)

  float stiffnessScale;                      ///< (Weight)

  float passOnAmount;                        ///< (ClampedWeight)



  NM_INLINE void operator *= (const float fVal)
  {
    imminence *= fVal;
    stiffnessScale *= fVal;
    passOnAmount *= fVal;
  }

  NM_INLINE GrabChestParams operator * (const float fVal) const
  {
    GrabChestParams result;
    result.imminence = imminence * fVal;
    result.stiffnessScale = stiffnessScale * fVal;
    result.passOnAmount = passOnAmount * fVal;
    return result;
  }

  NM_INLINE void operator += (const GrabChestParams& rhs)
  {
    imminence += rhs.imminence;
    stiffnessScale += rhs.stiffnessScale;
    passOnAmount += rhs.passOnAmount;
  }

  NM_INLINE GrabChestParams operator + (const GrabChestParams& rhs) const
  {
    GrabChestParams result;
    result.imminence = imminence + rhs.imminence;
    result.stiffnessScale = stiffnessScale + rhs.stiffnessScale;
    result.passOnAmount = passOnAmount + rhs.passOnAmount;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(imminence), "imminence");
    NM_VALIDATOR(FloatNonNegative(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(Float0to1(passOnAmount), "passOnAmount");
#endif // NM_CALL_VALIDATORS
  }

}; // struct GrabChestParams


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_GRABCHESTPARAMS_H

