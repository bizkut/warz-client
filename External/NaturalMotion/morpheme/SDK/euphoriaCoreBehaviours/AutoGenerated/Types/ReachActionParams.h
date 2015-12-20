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

#ifndef NM_MDF_TYPE_REACHACTIONPARAMS_H
#define NM_MDF_TYPE_REACHACTIONPARAMS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

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
// Declaration from 'Externs.types'
// Data Payload: 52 Bytes
// Modifiers: __no_combiner__  
struct ReachActionParams
{

  float outOfReachDistance;  ///< effector "close enough" threshold

  float outOfReachTimeout;  ///< failed to get "close enough" timeout  (TimePeriod)

  float withinReachTimeout;  ///< hold "close enough" timeout  (TimePeriod)

  float rampDownFailedDuration;  ///< rampdown after failing  (TimePeriod)

  float rampDownCompletedDuration;  ///< rampdown after successful reach  (TimePeriod)

  float strength;  ///< parameter that can be used to modulate reach strength  (Weight)

  float effectorSpeedLimit;  ///< parameter that can be used to slow effector progress to target

  int32_t swivelMode;  ///< A code to direct how reach behaviours should apply swivel.

  float swivelAmount;  ///< User specfied swivel amount in range [-1,1] will only be used if swivelMode is set to REACH_SWIVEL_SPECIFIED  (Multiplier)

  float maxLimbExtensionScale;  ///< Limit the maximum extension of the limb, expressed as a proportion of the total limb length, 1 is full arm extension  (Multiplier)

  float torsoRadiusMultiplier;  ///< Positive multiplier for torso radius (self-avoidance input)  (Weight)

  float penetrationAdjustment;  ///< a small offset below the surface to be applied wrt the surface normal  (Length)

  int32_t handsCollisionGroupIndex;  ///< collision group that can be enabled to disable hand/hand collisions


  // functions

  void setToDefaults(const ER::DimensionalScaling& scaling);


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(outOfReachDistance), "outOfReachDistance");
    NM_VALIDATOR(FloatValid(outOfReachTimeout), "outOfReachTimeout");
    NM_VALIDATOR(FloatValid(withinReachTimeout), "withinReachTimeout");
    NM_VALIDATOR(FloatValid(rampDownFailedDuration), "rampDownFailedDuration");
    NM_VALIDATOR(FloatValid(rampDownCompletedDuration), "rampDownCompletedDuration");
    NM_VALIDATOR(FloatNonNegative(strength), "strength");
    NM_VALIDATOR(FloatValid(effectorSpeedLimit), "effectorSpeedLimit");
    NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    NM_VALIDATOR(FloatValid(maxLimbExtensionScale), "maxLimbExtensionScale");
    NM_VALIDATOR(FloatNonNegative(torsoRadiusMultiplier), "torsoRadiusMultiplier");
    NM_VALIDATOR(FloatNonNegative(penetrationAdjustment), "penetrationAdjustment");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ReachActionParams


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_REACHACTIONPARAMS_H

