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

#ifndef NM_MDF_TYPE_OBJECTMETRIC_H
#define NM_MDF_TYPE_OBJECTMETRIC_H

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

namespace Environment { struct State; } 


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Externs.types'
// Data Payload: 80 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  

// defines an object by its speed, acceleration etc, used to search for bet objects

struct ObjectMetric
{

  NMP::Vector3 charactersPosition;           ///< (Position)

  NMP::Vector3 charactersVelocity;           ///< (Velocity)

  int64_t focusShapeID;  /// A shape to focus on

  float minMass;  /// absolute cutoffs  (Mass)

  float maxAverageSize;                      ///< (Length)

  float speedSquaredCoefficient;  /// coefficients which sum together  (Multiplier)

  float accelerationSquaredCoefficient;      ///< (Multiplier)

  float distanceSquaredCoefficient;          ///< (Multiplier)

  float shapeAdvantage;  ///< For value hysteresis

  float interestReductionRate;               ///< (Frequency)


  // functions

  float getMetric(const Environment::State& state, float timeStep, float& lastMetric) const;


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(charactersPosition), "charactersPosition");
    NM_VALIDATOR(Vector3Valid(charactersVelocity), "charactersVelocity");
    NM_VALIDATOR(FloatNonNegative(minMass), "minMass");
    NM_VALIDATOR(FloatNonNegative(maxAverageSize), "maxAverageSize");
    NM_VALIDATOR(FloatValid(speedSquaredCoefficient), "speedSquaredCoefficient");
    NM_VALIDATOR(FloatValid(accelerationSquaredCoefficient), "accelerationSquaredCoefficient");
    NM_VALIDATOR(FloatValid(distanceSquaredCoefficient), "distanceSquaredCoefficient");
    NM_VALIDATOR(FloatValid(shapeAdvantage), "shapeAdvantage");
    NM_VALIDATOR(FloatNonNegative(interestReductionRate), "interestReductionRate");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ObjectMetric


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_OBJECTMETRIC_H

