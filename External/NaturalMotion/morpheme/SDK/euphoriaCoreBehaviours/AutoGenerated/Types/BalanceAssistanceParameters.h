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

#ifndef NM_MDF_TYPE_BALANCEASSISTANCEPARAMETERS_H
#define NM_MDF_TYPE_BALANCEASSISTANCEPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMVector3.h"

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
// Data Payload: 48 Bytes
// Alignment: 16
struct BalanceAssistanceParameters
{

  NMP::Vector3 targetVelocity;               ///< (Velocity)

  float reduceAssistanceWhenFalling;         ///< (Weight)

  float orientationAssistanceAmount;         ///< (Weight)

  float positionAssistanceAmount;            ///< (Weight)

  float velocityAssistanceAmount;            ///< (Weight)

  float maxAngularAccelerationAssistance;    ///< (Weight)

  float maxLinearAccelerationAssistance;     ///< (Weight)

  float chestToPelvisRatio;                  ///< (Weight)

  bool useCounterForceOnFeet;



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(targetVelocity), "targetVelocity");
    NM_VALIDATOR(FloatNonNegative(reduceAssistanceWhenFalling), "reduceAssistanceWhenFalling");
    NM_VALIDATOR(FloatNonNegative(orientationAssistanceAmount), "orientationAssistanceAmount");
    NM_VALIDATOR(FloatNonNegative(positionAssistanceAmount), "positionAssistanceAmount");
    NM_VALIDATOR(FloatNonNegative(velocityAssistanceAmount), "velocityAssistanceAmount");
    NM_VALIDATOR(FloatNonNegative(maxAngularAccelerationAssistance), "maxAngularAccelerationAssistance");
    NM_VALIDATOR(FloatNonNegative(maxLinearAccelerationAssistance), "maxLinearAccelerationAssistance");
    NM_VALIDATOR(FloatNonNegative(chestToPelvisRatio), "chestToPelvisRatio");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BalanceAssistanceParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BALANCEASSISTANCEPARAMETERS_H

