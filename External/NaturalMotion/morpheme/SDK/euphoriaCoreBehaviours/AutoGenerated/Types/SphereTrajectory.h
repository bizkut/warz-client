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

#ifndef NM_MDF_TYPE_SPHERETRAJECTORY_H
#define NM_MDF_TYPE_SPHERETRAJECTORY_H

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

namespace MR { class InstanceDebugInterface; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Externs.types'
// Data Payload: 64 Bytes
// Alignment: 16
struct SphereTrajectory
{

  NMP::Vector3 position;                     ///< (Position)

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 acceleration;                 ///< (Acceleration)

  float radius;                              ///< (Length)

  float extraSearchRadius;  ///< this uses idle time to search to see if the object is nearby (within this radius) if it isn't approaching  (Length)

  float maxTime;                             ///< (TimePeriod)


  // functions

  void set(const NMP::Vector3& pos, const NMP::Vector3& vel, const NMP::Vector3& acc, float rad, float extraSearchRad, float maxT);
  static float getCollidePlaneTime(float startPos, float velocity, float acceleration, float& modifiedRadius);
  float getCollidePlaneTime(const NMP::Vector3& point, const NMP::Vector3& normal);
  NMP::Vector3 getPoint(float t) const;
  NMP::Vector3 getVelocity(float t) const;
  float getCollidePointTime(const NMP::Vector3& point);
  void debugDraw(const NMP::Vector3& startColour, const NMP::Vector3& endColour, float maximumTime, MR::InstanceDebugInterface* pDebugDrawInst);


  NM_INLINE void operator *= (const float fVal)
  {
    position *= fVal;
    velocity *= fVal;
    acceleration *= fVal;
    radius *= fVal;
    extraSearchRadius *= fVal;
    maxTime *= fVal;
  }

  NM_INLINE SphereTrajectory operator * (const float fVal) const
  {
    SphereTrajectory result;
    result.position = position * fVal;
    result.velocity = velocity * fVal;
    result.acceleration = acceleration * fVal;
    result.radius = radius * fVal;
    result.extraSearchRadius = extraSearchRadius * fVal;
    result.maxTime = maxTime * fVal;
    return result;
  }

  NM_INLINE void operator += (const SphereTrajectory& rhs)
  {
    position += rhs.position;
    velocity += rhs.velocity;
    acceleration += rhs.acceleration;
    radius += rhs.radius;
    extraSearchRadius += rhs.extraSearchRadius;
    maxTime += rhs.maxTime;
  }

  NM_INLINE SphereTrajectory operator + (const SphereTrajectory& rhs) const
  {
    SphereTrajectory result;
    result.position = position + rhs.position;
    result.velocity = velocity + rhs.velocity;
    result.acceleration = acceleration + rhs.acceleration;
    result.radius = radius + rhs.radius;
    result.extraSearchRadius = extraSearchRadius + rhs.extraSearchRadius;
    result.maxTime = maxTime + rhs.maxTime;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(position), "position");
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Valid(acceleration), "acceleration");
    NM_VALIDATOR(FloatNonNegative(radius), "radius");
    NM_VALIDATOR(FloatNonNegative(extraSearchRadius), "extraSearchRadius");
    NM_VALIDATOR(FloatValid(maxTime), "maxTime");
#endif // NM_CALL_VALIDATORS
  }

}; // struct SphereTrajectory


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_SPHERETRAJECTORY_H

