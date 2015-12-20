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

#ifndef NM_MDF_TYPE_STEPRECOVERYPARAMETERS_H
#define NM_MDF_TYPE_STEPRECOVERYPARAMETERS_H

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
// Data Payload: 17 Bytes
// Modifiers: __no_combiner__  
struct StepRecoveryParameters
{

  float fwdDistanceToTriggerStep;            ///< (Distance)

  float sidewaysDistanceToTriggerStep;       ///< (Distance)

  float timeBeforeShiftingWeight;            ///< (TimePeriod)

  float weightShiftingTime;                  ///< (TimePeriod)

  bool stepToRecoverPose;


  // functions

  StepRecoveryParameters();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(fwdDistanceToTriggerStep), "fwdDistanceToTriggerStep");
    NM_VALIDATOR(FloatValid(sidewaysDistanceToTriggerStep), "sidewaysDistanceToTriggerStep");
    NM_VALIDATOR(FloatValid(timeBeforeShiftingWeight), "timeBeforeShiftingWeight");
    NM_VALIDATOR(FloatValid(weightShiftingTime), "weightShiftingTime");
#endif // NM_CALL_VALIDATORS
  }

}; // struct StepRecoveryParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_STEPRECOVERYPARAMETERS_H

