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

#ifndef NM_MDF_TYPE_TARGETREQUEST_H
#define NM_MDF_TYPE_TARGETREQUEST_H

// include definition file to create project dependency
#include "./Definition/Types/PositionAndOrientation.types"

// external types
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMVector3.h"
#include "Euphoria/erDebugDraw.h"

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
// Declaration from 'PositionAndOrientation.types'
// Data Payload: 160 Bytes
// Alignment: 16

/// Request an orientation change towards a (look) target, and an optional imminence.
/// The imminence is the reciprocal of the time period in which the request needs to get done, and affects 
/// where the motion occurs, high is higher in the body.

struct TargetRequest
{

  NMP::Vector3 target;  /// the target position for the area you are requesting (e.g. to the head point)  (Position)

#if defined(MR_OUTPUT_DEBUGGING)
  ER::LimbControlAmounts debugControlAmounts;  ///< (LimbDebugControlAmounts)
#endif // MR_OUTPUT_DEBUGGING

  float imminence;  /// 1 divided by the time period in which the request needs to be fulfilled  (Imminence)

  float stiffnessScale;                      ///< (Multiplier)

  float passOnAmount;  ///< 0 means receiver passes none futher, 1 means pass default amount down (even distribution), inf means pass all down  (Multiplier)

  float lookOutOfRangeWeight;  ///< 0 means he won't try and look at targets out of range, 1 means he will try and look at targets over the full 4pi square-radians  (Weight)


  // functions

  /// Note that the imminence must be set prior to use
  TargetRequest();
  TargetRequest(const NMP::Vector3& target, const ER::DimensionalScaling dimensionalScaling);


  NM_INLINE void operator *= (const float fVal)
  {
    target *= fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    debugControlAmounts *= fVal;
#endif // MR_OUTPUT_DEBUGGING
    imminence *= fVal;
    stiffnessScale *= fVal;
    passOnAmount *= fVal;
    lookOutOfRangeWeight *= fVal;
  }

  NM_INLINE TargetRequest operator * (const float fVal) const
  {
    TargetRequest result;
    result.target = target * fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    result.debugControlAmounts = debugControlAmounts * fVal;
#endif // MR_OUTPUT_DEBUGGING
    result.imminence = imminence * fVal;
    result.stiffnessScale = stiffnessScale * fVal;
    result.passOnAmount = passOnAmount * fVal;
    result.lookOutOfRangeWeight = lookOutOfRangeWeight * fVal;
    return result;
  }

  NM_INLINE void operator += (const TargetRequest& rhs)
  {
    target += rhs.target;
#if defined(MR_OUTPUT_DEBUGGING)
    debugControlAmounts += rhs.debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    imminence += rhs.imminence;
    stiffnessScale += rhs.stiffnessScale;
    passOnAmount += rhs.passOnAmount;
    lookOutOfRangeWeight += rhs.lookOutOfRangeWeight;
  }

  NM_INLINE TargetRequest operator + (const TargetRequest& rhs) const
  {
    TargetRequest result;
    result.target = target + rhs.target;
#if defined(MR_OUTPUT_DEBUGGING)
    result.debugControlAmounts = debugControlAmounts + rhs.debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    result.imminence = imminence + rhs.imminence;
    result.stiffnessScale = stiffnessScale + rhs.stiffnessScale;
    result.passOnAmount = passOnAmount + rhs.passOnAmount;
    result.lookOutOfRangeWeight = lookOutOfRangeWeight + rhs.lookOutOfRangeWeight;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(target), "target");
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
    NM_VALIDATOR(FloatValid(imminence), "imminence");
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatValid(passOnAmount), "passOnAmount");
    NM_VALIDATOR(FloatNonNegative(lookOutOfRangeWeight), "lookOutOfRangeWeight");
#endif // NM_CALL_VALIDATORS
  }

}; // struct TargetRequest


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_TARGETREQUEST_H

