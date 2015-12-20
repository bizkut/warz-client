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

#ifndef NM_MDF_TYPE_STEPPARAMETERS_H
#define NM_MDF_TYPE_STEPPARAMETERS_H

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
// Data Payload: 128 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  
struct StepParameters
{

  NMP::Vector3 exclusionPlane;  /// exclusion plane is dot(v, exclusionPlane.[xyz]) + exclusionPlane.w = 0, or +ve when in exclusion zone

  int32_t collisionGroupIndex;

  float stepPredictionTimeForward;           ///< (TimePeriod)

  float stepPredictionTimeBackward;          ///< (TimePeriod)

  float stepUpDistance;                      ///< (Length)

  float stepDownDistance;                    ///< (Length)

  float maxStepLength;                       ///< (Length)

  float stepDownSpeed;                       ///< (Speed)

  float footSeparationFraction;              ///< (Multiplier)

  float alignFootToFloorWeight;              ///< (Weight)

  float orientationWeight;                   ///< (Weight)

  float limbLengthToAbort;                   ///< (Length)

  float rootDownSpeedToAbort;                ///< (Speed)

  float steppingDirectionThreshold;          ///< (Multiplier)

  float steppingImplicitStiffness;           ///< (Weight)

  float lowerPelvisAmount;                   ///< (Multiplier)

  float IKSubstep;                           ///< (Multiplier)

  float gravityCompensation;                 ///< (Multiplier)

  float strengthScale;                       ///< (Multiplier)

  float dampingRatioScale;                   ///< (Multiplier)

  float stepWeaknessPerLeg[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)

  float stepWeaknessPerArm[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)

  float footCrossingOffset;                  ///< (Distance)

  bool suppressFootCrossing;


  // functions

  StepParameters();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(exclusionPlane), "exclusionPlane");
    NM_VALIDATOR(FloatValid(stepPredictionTimeForward), "stepPredictionTimeForward");
    NM_VALIDATOR(FloatValid(stepPredictionTimeBackward), "stepPredictionTimeBackward");
    NM_VALIDATOR(FloatNonNegative(stepUpDistance), "stepUpDistance");
    NM_VALIDATOR(FloatNonNegative(stepDownDistance), "stepDownDistance");
    NM_VALIDATOR(FloatNonNegative(maxStepLength), "maxStepLength");
    NM_VALIDATOR(FloatValid(stepDownSpeed), "stepDownSpeed");
    NM_VALIDATOR(FloatValid(footSeparationFraction), "footSeparationFraction");
    NM_VALIDATOR(FloatNonNegative(alignFootToFloorWeight), "alignFootToFloorWeight");
    NM_VALIDATOR(FloatNonNegative(orientationWeight), "orientationWeight");
    NM_VALIDATOR(FloatNonNegative(limbLengthToAbort), "limbLengthToAbort");
    NM_VALIDATOR(FloatValid(rootDownSpeedToAbort), "rootDownSpeedToAbort");
    NM_VALIDATOR(FloatValid(steppingDirectionThreshold), "steppingDirectionThreshold");
    NM_VALIDATOR(FloatNonNegative(steppingImplicitStiffness), "steppingImplicitStiffness");
    NM_VALIDATOR(FloatValid(lowerPelvisAmount), "lowerPelvisAmount");
    NM_VALIDATOR(FloatValid(IKSubstep), "IKSubstep");
    NM_VALIDATOR(FloatValid(gravityCompensation), "gravityCompensation");
    NM_VALIDATOR(FloatValid(strengthScale), "strengthScale");
    NM_VALIDATOR(FloatValid(dampingRatioScale), "dampingRatioScale");
    NM_VALIDATOR(FloatValid(stepWeaknessPerLeg[0]), "stepWeaknessPerLeg[0]");
    NM_VALIDATOR(FloatValid(stepWeaknessPerLeg[1]), "stepWeaknessPerLeg[1]");
    NM_VALIDATOR(FloatValid(stepWeaknessPerArm[0]), "stepWeaknessPerArm[0]");
    NM_VALIDATOR(FloatValid(stepWeaknessPerArm[1]), "stepWeaknessPerArm[1]");
    NM_VALIDATOR(FloatValid(footCrossingOffset), "footCrossingOffset");
#endif // NM_CALL_VALIDATORS
  }

}; // struct StepParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_STEPPARAMETERS_H

