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

#ifndef NM_MDF_TYPE_STEPPINGBALANCEPARAMETERS_H
#define NM_MDF_TYPE_STEPPINGBALANCEPARAMETERS_H

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
// Data Payload: 37 Bytes
// Modifiers: __no_combiner__  
struct SteppingBalanceParameters
{

  float armSwingStrengthScaleWhenStepping;   ///< (Weight)

  int32_t stepsBeforeLooking;

  float wholeBodyLook;                       ///< (Weight)

  float lookDownAngle;                       ///< (Angle)

  float stopLookingTime;                     ///< (TimePeriod)

  float lookWeight;                          ///< (Weight)

  float stepCountResetTime;                  ///< (TimePeriod)

  float minSpeedForLooking;                  ///< (Speed)

  float stepPoseArmDisplacementTime;         ///< (TimePeriod)

  bool lookInStepDirection;


  // functions

  SteppingBalanceParameters();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(armSwingStrengthScaleWhenStepping), "armSwingStrengthScaleWhenStepping");
    NM_VALIDATOR(FloatNonNegative(wholeBodyLook), "wholeBodyLook");
    NM_VALIDATOR(FloatValid(lookDownAngle), "lookDownAngle");
    NM_VALIDATOR(FloatValid(stopLookingTime), "stopLookingTime");
    NM_VALIDATOR(FloatNonNegative(lookWeight), "lookWeight");
    NM_VALIDATOR(FloatValid(stepCountResetTime), "stepCountResetTime");
    NM_VALIDATOR(FloatValid(minSpeedForLooking), "minSpeedForLooking");
    NM_VALIDATOR(FloatValid(stepPoseArmDisplacementTime), "stepPoseArmDisplacementTime");
#endif // NM_CALL_VALIDATORS
  }

}; // struct SteppingBalanceParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_STEPPINGBALANCEPARAMETERS_H

