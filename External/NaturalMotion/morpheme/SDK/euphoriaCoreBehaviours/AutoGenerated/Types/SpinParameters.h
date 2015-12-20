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

#ifndef NM_MDF_TYPE_SPINPARAMETERS_H
#define NM_MDF_TYPE_SPINPARAMETERS_H

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
// Data Payload: 45 Bytes
// Modifiers: __no_combiner__  
struct SpinParameters
{

  float maxRadius;                           ///< (Length)

  float maxAngSpeed;                         ///< (AngularSpeed)

  float spinWeightLateral;  /// To control which direction he can spin in   (Weight)

  float spinWeightUp;                        ///< (Weight)

  float spinWeightForward;                   ///< (Weight)

  float spinCentreLateral;  /// These are just used for arms spin  (Distance)

  float spinCentreUp;                        ///< (Distance)

  float spinCentreForward;                   ///< (Distance)

  float spinOutwardsDistanceWhenBehind;      ///< (Distance)

  float spinArmControlCompensationScale;     ///< (Weight)

  float strengthReductionTowardsEnd;         ///< (Weight)

  bool synchronised;  ///< Synchronization between limbs.


  // functions

  /// Constructor will set everything to sensible defaults or zero.
  SpinParameters();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(maxRadius), "maxRadius");
    NM_VALIDATOR(FloatValid(maxAngSpeed), "maxAngSpeed");
    NM_VALIDATOR(FloatNonNegative(spinWeightLateral), "spinWeightLateral");
    NM_VALIDATOR(FloatNonNegative(spinWeightUp), "spinWeightUp");
    NM_VALIDATOR(FloatNonNegative(spinWeightForward), "spinWeightForward");
    NM_VALIDATOR(FloatValid(spinCentreLateral), "spinCentreLateral");
    NM_VALIDATOR(FloatValid(spinCentreUp), "spinCentreUp");
    NM_VALIDATOR(FloatValid(spinCentreForward), "spinCentreForward");
    NM_VALIDATOR(FloatValid(spinOutwardsDistanceWhenBehind), "spinOutwardsDistanceWhenBehind");
    NM_VALIDATOR(FloatNonNegative(spinArmControlCompensationScale), "spinArmControlCompensationScale");
    NM_VALIDATOR(FloatNonNegative(strengthReductionTowardsEnd), "strengthReductionTowardsEnd");
#endif // NM_CALL_VALIDATORS
  }

}; // struct SpinParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_SPINPARAMETERS_H

