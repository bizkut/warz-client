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

#ifndef NM_MDF_TYPE_BALANCEPOSECLAMPING_H
#define NM_MDF_TYPE_BALANCEPOSECLAMPING_H

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
// Data Payload: 42 Bytes
// Modifiers: __no_combiner__  
struct BalancePoseClamping
{

  float minPelvisPitch;                      ///< (Angle)

  float maxPelvisPitch;                      ///< (Angle)

  float minPelvisRoll;                       ///< (Angle)

  float maxPelvisRoll;                       ///< (Angle)

  float minPelvisYaw;                        ///< (Angle)

  float maxPelvisYaw;                        ///< (Angle)

  float minPelvisHeight;                     ///< (Distance)

  float maxPelvisHeight;                     ///< (Distance)

  float pelvisPositionChangeTimescale;       ///< (TimePeriod)

  float pelvisOrientationChangeTimescale;    ///< (TimePeriod)

  bool enablePoseClamping;

  bool dynamicClamping;


  // functions

  BalancePoseClamping();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(minPelvisPitch), "minPelvisPitch");
    NM_VALIDATOR(FloatValid(maxPelvisPitch), "maxPelvisPitch");
    NM_VALIDATOR(FloatValid(minPelvisRoll), "minPelvisRoll");
    NM_VALIDATOR(FloatValid(maxPelvisRoll), "maxPelvisRoll");
    NM_VALIDATOR(FloatValid(minPelvisYaw), "minPelvisYaw");
    NM_VALIDATOR(FloatValid(maxPelvisYaw), "maxPelvisYaw");
    NM_VALIDATOR(FloatValid(minPelvisHeight), "minPelvisHeight");
    NM_VALIDATOR(FloatValid(maxPelvisHeight), "maxPelvisHeight");
    NM_VALIDATOR(FloatValid(pelvisPositionChangeTimescale), "pelvisPositionChangeTimescale");
    NM_VALIDATOR(FloatValid(pelvisOrientationChangeTimescale), "pelvisOrientationChangeTimescale");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BalancePoseClamping


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BALANCEPOSECLAMPING_H

