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

#ifndef NM_MDF_TYPE_ROTATIONREQUEST_H
#define NM_MDF_TYPE_ROTATIONREQUEST_H

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

/// Request for a rotation (ie a delta from the current orientation), with an optional imminence.
/// The imminence is the reciprocal of the time period in which the request needs to get done, and affects where 
/// the motion occurs, high is higher in the body

struct RotationRequest
{

  NMP::Vector3 rotation;  ///< Rotation (length is radians) is in world space  (RotationVector)

#if defined(MR_OUTPUT_DEBUGGING)
  ER::LimbControlAmounts debugControlAmounts;  ///< (LimbDebugControlAmounts)
#endif // MR_OUTPUT_DEBUGGING

  float imminence;                           ///< (Imminence)

  float stiffnessScale;                      ///< (Multiplier)

  float passOnAmount;  ///< 0 means receiver passes none futher, 1 means pass default amount down (even distribution), inf means pass all down  (Multiplier)


  // functions

  /// Note that the imminence must be set prior to use
  RotationRequest();
  RotationRequest(const NMP::Vector3& rotation, const ER::DimensionalScaling dimensionalScaling);


  NM_INLINE void operator *= (const float fVal)
  {
    rotation *= fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    debugControlAmounts *= fVal;
#endif // MR_OUTPUT_DEBUGGING
    imminence *= fVal;
    stiffnessScale *= fVal;
    passOnAmount *= fVal;
  }

  NM_INLINE RotationRequest operator * (const float fVal) const
  {
    RotationRequest result;
    result.rotation = rotation * fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    result.debugControlAmounts = debugControlAmounts * fVal;
#endif // MR_OUTPUT_DEBUGGING
    result.imminence = imminence * fVal;
    result.stiffnessScale = stiffnessScale * fVal;
    result.passOnAmount = passOnAmount * fVal;
    return result;
  }

  NM_INLINE void operator += (const RotationRequest& rhs)
  {
    rotation += rhs.rotation;
#if defined(MR_OUTPUT_DEBUGGING)
    debugControlAmounts += rhs.debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    imminence += rhs.imminence;
    stiffnessScale += rhs.stiffnessScale;
    passOnAmount += rhs.passOnAmount;
  }

  NM_INLINE RotationRequest operator + (const RotationRequest& rhs) const
  {
    RotationRequest result;
    result.rotation = rotation + rhs.rotation;
#if defined(MR_OUTPUT_DEBUGGING)
    result.debugControlAmounts = debugControlAmounts + rhs.debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    result.imminence = imminence + rhs.imminence;
    result.stiffnessScale = stiffnessScale + rhs.stiffnessScale;
    result.passOnAmount = passOnAmount + rhs.passOnAmount;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(rotation), "rotation");
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
    NM_VALIDATOR(FloatValid(imminence), "imminence");
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatValid(passOnAmount), "passOnAmount");
#endif // NM_CALL_VALIDATORS
  }

}; // struct RotationRequest


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ROTATIONREQUEST_H

