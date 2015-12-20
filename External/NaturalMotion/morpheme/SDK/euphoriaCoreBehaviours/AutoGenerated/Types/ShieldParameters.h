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

#ifndef NM_MDF_TYPE_SHIELDPARAMETERS_H
#define NM_MDF_TYPE_SHIELDPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

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
// Data Payload: 12 Bytes
struct ShieldParameters
{

  float impactResponseSpeedForShield;  ///< Shield will be triggered if the character will change speed by more than this on impact, based on the relative masses.  (Speed)

  float angularSpeedForShield;  ///< The character will shield if he predicts an impact with a hazard and he is rotating faster than this.   (AngularSpeed)

  float tangentialSpeedForShield;  ///< The character will shield if his predicted tangential velocity at the time of impact with a hazard is greater than this.  (Speed)



  NM_INLINE void operator *= (const float fVal)
  {
    impactResponseSpeedForShield *= fVal;
    angularSpeedForShield *= fVal;
    tangentialSpeedForShield *= fVal;
  }

  NM_INLINE ShieldParameters operator * (const float fVal) const
  {
    ShieldParameters result;
    result.impactResponseSpeedForShield = impactResponseSpeedForShield * fVal;
    result.angularSpeedForShield = angularSpeedForShield * fVal;
    result.tangentialSpeedForShield = tangentialSpeedForShield * fVal;
    return result;
  }

  NM_INLINE void operator += (const ShieldParameters& rhs)
  {
    impactResponseSpeedForShield += rhs.impactResponseSpeedForShield;
    angularSpeedForShield += rhs.angularSpeedForShield;
    tangentialSpeedForShield += rhs.tangentialSpeedForShield;
  }

  NM_INLINE ShieldParameters operator + (const ShieldParameters& rhs) const
  {
    ShieldParameters result;
    result.impactResponseSpeedForShield = impactResponseSpeedForShield + rhs.impactResponseSpeedForShield;
    result.angularSpeedForShield = angularSpeedForShield + rhs.angularSpeedForShield;
    result.tangentialSpeedForShield = tangentialSpeedForShield + rhs.tangentialSpeedForShield;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(impactResponseSpeedForShield), "impactResponseSpeedForShield");
    NM_VALIDATOR(FloatValid(angularSpeedForShield), "angularSpeedForShield");
    NM_VALIDATOR(FloatValid(tangentialSpeedForShield), "tangentialSpeedForShield");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ShieldParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_SHIELDPARAMETERS_H

