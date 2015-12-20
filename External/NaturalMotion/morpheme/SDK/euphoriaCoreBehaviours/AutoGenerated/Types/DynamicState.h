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

#ifndef NM_MDF_TYPE_DYNAMICSTATE_H
#define NM_MDF_TYPE_DYNAMICSTATE_H

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

// This is a low memory structure containing the most important features of the state of a (roughly spherical) body

struct DynamicState
{

  NMP::Vector3 positionTimesMass;

  NMP::Vector3 momentum;                     ///< (Momentum)

  NMP::Vector3 angularMomentum;  ///< this value is approximate since it scales by mass, so assumes fixed width  (AngularMomentum)

  float mass;                                ///< (Mass)


  // functions

  DynamicState(float mass=1);
  void setPosition(const NMP::Vector3& position);
  void setVelocity(const NMP::Vector3& velocity);
  void setAngularVelocity(const NMP::Vector3& angularVelocity);
  NMP::Vector3 getPosition() const;
  NMP::Vector3 getVelocity() const;
  NMP::Vector3 getAngularVelocity() const;


  NM_INLINE void operator *= (const float fVal)
  {
    positionTimesMass *= fVal;
    momentum *= fVal;
    angularMomentum *= fVal;
    mass *= fVal;
  }

  NM_INLINE DynamicState operator * (const float fVal) const
  {
    DynamicState result;
    result.positionTimesMass = positionTimesMass * fVal;
    result.momentum = momentum * fVal;
    result.angularMomentum = angularMomentum * fVal;
    result.mass = mass * fVal;
    return result;
  }

  NM_INLINE void operator += (const DynamicState& rhs)
  {
    positionTimesMass += rhs.positionTimesMass;
    momentum += rhs.momentum;
    angularMomentum += rhs.angularMomentum;
    mass += rhs.mass;
  }

  NM_INLINE DynamicState operator + (const DynamicState& rhs) const
  {
    DynamicState result;
    result.positionTimesMass = positionTimesMass + rhs.positionTimesMass;
    result.momentum = momentum + rhs.momentum;
    result.angularMomentum = angularMomentum + rhs.angularMomentum;
    result.mass = mass + rhs.mass;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(positionTimesMass), "positionTimesMass");
    NM_VALIDATOR(Vector3Valid(momentum), "momentum");
    NM_VALIDATOR(Vector3Valid(angularMomentum), "angularMomentum");
    NM_VALIDATOR(FloatNonNegative(mass), "mass");
#endif // NM_CALL_VALIDATORS
  }

}; // struct DynamicState


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_DYNAMICSTATE_H

