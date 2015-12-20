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

#ifndef NM_MDF_TYPE_DODGEHAZARD_H
#define NM_MDF_TYPE_DODGEHAZARD_H

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
struct DodgeHazard
{

  NMP::Vector3 position;                     ///< (Position)

  NMP::Vector3 velocity;                     ///< (Velocity)

  float radius;                              ///< (Length)

  float dodgeableAmount;  ///< Will normally be set to 0 or 1, but when hazards are combined can be fractional  (Weight)

  float rampDownDurationScale;               ///< (TimePeriod)



  NM_INLINE void operator *= (const float fVal)
  {
    position *= fVal;
    velocity *= fVal;
    radius *= fVal;
    dodgeableAmount *= fVal;
    rampDownDurationScale *= fVal;
  }

  NM_INLINE DodgeHazard operator * (const float fVal) const
  {
    DodgeHazard result;
    result.position = position * fVal;
    result.velocity = velocity * fVal;
    result.radius = radius * fVal;
    result.dodgeableAmount = dodgeableAmount * fVal;
    result.rampDownDurationScale = rampDownDurationScale * fVal;
    return result;
  }

  NM_INLINE void operator += (const DodgeHazard& rhs)
  {
    position += rhs.position;
    velocity += rhs.velocity;
    radius += rhs.radius;
    dodgeableAmount += rhs.dodgeableAmount;
    rampDownDurationScale += rhs.rampDownDurationScale;
  }

  NM_INLINE DodgeHazard operator + (const DodgeHazard& rhs) const
  {
    DodgeHazard result;
    result.position = position + rhs.position;
    result.velocity = velocity + rhs.velocity;
    result.radius = radius + rhs.radius;
    result.dodgeableAmount = dodgeableAmount + rhs.dodgeableAmount;
    result.rampDownDurationScale = rampDownDurationScale + rhs.rampDownDurationScale;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(position), "position");
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(FloatNonNegative(radius), "radius");
    NM_VALIDATOR(FloatNonNegative(dodgeableAmount), "dodgeableAmount");
    NM_VALIDATOR(FloatValid(rampDownDurationScale), "rampDownDurationScale");
#endif // NM_CALL_VALIDATORS
  }

}; // struct DodgeHazard


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_DODGEHAZARD_H

