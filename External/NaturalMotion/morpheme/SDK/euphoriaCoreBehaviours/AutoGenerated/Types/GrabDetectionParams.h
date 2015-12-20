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

#ifndef NM_MDF_TYPE_GRABDETECTIONPARAMS_H
#define NM_MDF_TYPE_GRABDETECTIONPARAMS_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace ER { class DimensionalScaling; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Hold.types'
// Data Payload: 28 Bytes

//----------------------------------------------------------------------------------------------------------------------
// Describes what kind of edges the Hold behaviour should be allowed to grab.
//----------------------------------------------------------------------------------------------------------------------

struct GrabDetectionParams
{

  float maxReachDistance;  ///< Edge must be closer than this distance from the characters chest.

  float minEdgeLength;  ///< Shorter edges are not allowed (dimensionally scaled).  (Length)

  float minAngleBetweenNormals;  ///< If two surfaces are too level the joining edge is not allowed.  (Angle)

  float maxSlope;  ///< If slope of the more upright surface is too steep, edge is not allowed.  (Angle)

  float minMass;  ///< Only heavier objects are allowed (dimensionally scaled).  (Mass)

  float minVolume;  ///< Only bigger objects are allowed (dimensionally scaled).  (Volume)

  float minEdgeQuality;  ///< Filter out low quality edges, if desired.  (Weight)


  // functions

  void initialise(const ER::DimensionalScaling& scaling);


  NM_INLINE void operator *= (const float fVal)
  {
    maxReachDistance *= fVal;
    minEdgeLength *= fVal;
    minAngleBetweenNormals *= fVal;
    maxSlope *= fVal;
    minMass *= fVal;
    minVolume *= fVal;
    minEdgeQuality *= fVal;
  }

  NM_INLINE GrabDetectionParams operator * (const float fVal) const
  {
    GrabDetectionParams result;
    result.maxReachDistance = maxReachDistance * fVal;
    result.minEdgeLength = minEdgeLength * fVal;
    result.minAngleBetweenNormals = minAngleBetweenNormals * fVal;
    result.maxSlope = maxSlope * fVal;
    result.minMass = minMass * fVal;
    result.minVolume = minVolume * fVal;
    result.minEdgeQuality = minEdgeQuality * fVal;
    return result;
  }

  NM_INLINE void operator += (const GrabDetectionParams& rhs)
  {
    maxReachDistance += rhs.maxReachDistance;
    minEdgeLength += rhs.minEdgeLength;
    minAngleBetweenNormals += rhs.minAngleBetweenNormals;
    maxSlope += rhs.maxSlope;
    minMass += rhs.minMass;
    minVolume += rhs.minVolume;
    minEdgeQuality += rhs.minEdgeQuality;
  }

  NM_INLINE GrabDetectionParams operator + (const GrabDetectionParams& rhs) const
  {
    GrabDetectionParams result;
    result.maxReachDistance = maxReachDistance + rhs.maxReachDistance;
    result.minEdgeLength = minEdgeLength + rhs.minEdgeLength;
    result.minAngleBetweenNormals = minAngleBetweenNormals + rhs.minAngleBetweenNormals;
    result.maxSlope = maxSlope + rhs.maxSlope;
    result.minMass = minMass + rhs.minMass;
    result.minVolume = minVolume + rhs.minVolume;
    result.minEdgeQuality = minEdgeQuality + rhs.minEdgeQuality;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(maxReachDistance), "maxReachDistance");
    NM_VALIDATOR(FloatNonNegative(minEdgeLength), "minEdgeLength");
    NM_VALIDATOR(FloatValid(minAngleBetweenNormals), "minAngleBetweenNormals");
    NM_VALIDATOR(FloatValid(maxSlope), "maxSlope");
    NM_VALIDATOR(FloatNonNegative(minMass), "minMass");
    NM_VALIDATOR(FloatNonNegative(minVolume), "minVolume");
    NM_VALIDATOR(FloatNonNegative(minEdgeQuality), "minEdgeQuality");
#endif // NM_CALL_VALIDATORS
  }

}; // struct GrabDetectionParams


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_GRABDETECTIONPARAMS_H

