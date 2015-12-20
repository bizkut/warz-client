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

#ifndef NM_MDF_TYPE_DIRECTIONREQUEST_H
#define NM_MDF_TYPE_DIRECTIONREQUEST_H

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
// Data Payload: 176 Bytes
// Alignment: 16

/// Request for a direction change that would align a reference direction with a world space direction, 
/// and an optional imminence.
/// The imminence is the reciprocal of the time period in which the request needs to get done, and affects 
/// where the motion occurs, high is higher in the body

struct DirectionRequest
{

  NMP::Vector3 controlDirLocal;  /// the direction in local coordinates of the body to be orienting (eg. (1,0,0) of the pelvis means the forwards direction of the pelvis)  (Direction)

  NMP::Vector3 referenceDirWorld;  /// the direction to be orienting to coordinates wrt the frame that the body matrix is expressed in (usually world)  (Direction)

#if defined(MR_OUTPUT_DEBUGGING)
  ER::LimbControlAmounts debugControlAmounts;  ///< (LimbDebugControlAmounts)
#endif // MR_OUTPUT_DEBUGGING

  float imminence;  /// 1 divided by the time period in which the request needs to be fulfilled  (Imminence)

  float stiffnessScale;                      ///< (Multiplier)

  float passOnAmount;  ///< 0 means receiver passes none futher, 1 means pass default amount down (even distribution), inf means pass all down  (Multiplier)


  // functions

  /// Note that the imminence must be set prior to use
  DirectionRequest();
  DirectionRequest(const NMP::Vector3& controlLocal, const NMP::Vector3& referenceWorld, const ER::DimensionalScaling dimensionalScaling);


  NM_INLINE void operator *= (const float fVal)
  {
    controlDirLocal *= fVal;
    referenceDirWorld *= fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    debugControlAmounts *= fVal;
#endif // MR_OUTPUT_DEBUGGING
    imminence *= fVal;
    stiffnessScale *= fVal;
    passOnAmount *= fVal;
  }

  NM_INLINE DirectionRequest operator * (const float fVal) const
  {
    DirectionRequest result;
    result.controlDirLocal = controlDirLocal * fVal;
    result.referenceDirWorld = referenceDirWorld * fVal;
#if defined(MR_OUTPUT_DEBUGGING)
    result.debugControlAmounts = debugControlAmounts * fVal;
#endif // MR_OUTPUT_DEBUGGING
    result.imminence = imminence * fVal;
    result.stiffnessScale = stiffnessScale * fVal;
    result.passOnAmount = passOnAmount * fVal;
    return result;
  }

  NM_INLINE void operator += (const DirectionRequest& rhs)
  {
    controlDirLocal += rhs.controlDirLocal;
    referenceDirWorld += rhs.referenceDirWorld;
#if defined(MR_OUTPUT_DEBUGGING)
    debugControlAmounts += rhs.debugControlAmounts;
#endif // MR_OUTPUT_DEBUGGING
    imminence += rhs.imminence;
    stiffnessScale += rhs.stiffnessScale;
    passOnAmount += rhs.passOnAmount;
  }

  NM_INLINE DirectionRequest operator + (const DirectionRequest& rhs) const
  {
    DirectionRequest result;
    result.controlDirLocal = controlDirLocal + rhs.controlDirLocal;
    result.referenceDirWorld = referenceDirWorld + rhs.referenceDirWorld;
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
    NM_VALIDATOR(Vector3Normalised(controlDirLocal), "controlDirLocal");
    NM_VALIDATOR(Vector3Normalised(referenceDirWorld), "referenceDirWorld");
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
    NM_VALIDATOR(FloatValid(imminence), "imminence");
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatValid(passOnAmount), "passOnAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(controlDirLocal);
    PostCombiners::Vector3Normalise(referenceDirWorld);
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
    validate();
  }

}; // struct DirectionRequest


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_DIRECTIONREQUEST_H

