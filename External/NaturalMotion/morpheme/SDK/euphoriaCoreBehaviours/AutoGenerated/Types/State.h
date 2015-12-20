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

#ifndef NM_MDF_TYPE_STATE_H
#define NM_MDF_TYPE_STATE_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMMatrix34.h"
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
// Data Payload: 96 Bytes
// Alignment: 16

// this is a quite pure meaning of the state of a rigid body, ie its position, orientation and their differentials

struct State
{

  NMP::Matrix34 matrix;

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 angularVelocity;              ///< (AngularVelocity)



  NM_INLINE void operator *= (const float fVal)
  {
    matrix *= fVal;
    velocity *= fVal;
    angularVelocity *= fVal;
  }

  NM_INLINE State operator * (const float fVal) const
  {
    State result;
    result.matrix = matrix * fVal;
    result.velocity = velocity * fVal;
    result.angularVelocity = angularVelocity * fVal;
    return result;
  }

  NM_INLINE void operator += (const State& rhs)
  {
    matrix += rhs.matrix;
    velocity += rhs.velocity;
    angularVelocity += rhs.angularVelocity;
  }

  NM_INLINE State operator + (const State& rhs) const
  {
    State result;
    result.matrix = matrix + rhs.matrix;
    result.velocity = velocity + rhs.velocity;
    result.angularVelocity = angularVelocity + rhs.angularVelocity;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Valid(angularVelocity), "angularVelocity");
#endif // NM_CALL_VALIDATORS
  }

}; // struct State


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_STATE_H

