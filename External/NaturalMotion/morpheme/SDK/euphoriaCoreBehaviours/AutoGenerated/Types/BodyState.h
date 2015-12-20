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

#ifndef NM_MDF_TYPE_BODYSTATE_H
#define NM_MDF_TYPE_BODYSTATE_H

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
// Data Payload: 64 Bytes
// Alignment: 16
struct BodyState
{

  NMP::Vector3 position;                     ///< (Position)

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 acceleration;                 ///< (Acceleration)

  float mass;                                ///< (Mass)

  float accSqr;                              ///< (AccelerationSquared)



  NM_INLINE void operator *= (const float fVal)
  {
    position *= fVal;
    velocity *= fVal;
    acceleration *= fVal;
    mass *= fVal;
    accSqr *= fVal;
  }

  NM_INLINE BodyState operator * (const float fVal) const
  {
    BodyState result;
    result.position = position * fVal;
    result.velocity = velocity * fVal;
    result.acceleration = acceleration * fVal;
    result.mass = mass * fVal;
    result.accSqr = accSqr * fVal;
    return result;
  }

  NM_INLINE void operator += (const BodyState& rhs)
  {
    position += rhs.position;
    velocity += rhs.velocity;
    acceleration += rhs.acceleration;
    mass += rhs.mass;
    accSqr += rhs.accSqr;
  }

  NM_INLINE BodyState operator + (const BodyState& rhs) const
  {
    BodyState result;
    result.position = position + rhs.position;
    result.velocity = velocity + rhs.velocity;
    result.acceleration = acceleration + rhs.acceleration;
    result.mass = mass + rhs.mass;
    result.accSqr = accSqr + rhs.accSqr;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(position), "position");
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Valid(acceleration), "acceleration");
    NM_VALIDATOR(FloatNonNegative(mass), "mass");
    NM_VALIDATOR(FloatValid(accSqr), "accSqr");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BodyState


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BODYSTATE_H

