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

#ifndef NM_MDF_TYPE_ENVIRONMENT_COLLIDERESULT_H
#define NM_MDF_TYPE_ENVIRONMENT_COLLIDERESULT_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

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

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 80 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  
struct CollideResult
{

  NMP::Vector3 impactVel;  ///< actual full relative impact speed  (Velocity)

  NMP::Vector3 impactPosition;  ///< actual contact point where objects touch  (Position)

  NMP::Vector3 normal;  ///< normal on patch where collided  (Direction)

  float approachSpeed;  ///< how fast the objecta are moving towards eachother at the start  (Speed)

  float impactSpeed;  ///< how fast they move towards eachother at impact  (Speed)

  float time;  ///< time to impact  (TimePeriod)

  float distance;  ///< distance from start to impact  (Length)

  float timeMetric;  ///< time to impact, scaled by the confidence in that patch's accuracy, i.e. a common metric  (TimePeriod)

  bool clippedToBoundingBox;  ///< whether contact is on the actual patch geometry (false) or just clipping with the counding box (true)

  bool initialOverlap;  ///< true if objects are intersecting at time 0



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(impactVel), "impactVel");
    NM_VALIDATOR(Vector3Valid(impactPosition), "impactPosition");
    NM_VALIDATOR(Vector3Normalised(normal), "normal");
    NM_VALIDATOR(FloatValid(approachSpeed), "approachSpeed");
    NM_VALIDATOR(FloatValid(impactSpeed), "impactSpeed");
    NM_VALIDATOR(FloatValid(time), "time");
    NM_VALIDATOR(FloatNonNegative(distance), "distance");
    NM_VALIDATOR(FloatValid(timeMetric), "timeMetric");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(normal);
    validate();
  }

}; // struct CollideResult

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_COLLIDERESULT_H

