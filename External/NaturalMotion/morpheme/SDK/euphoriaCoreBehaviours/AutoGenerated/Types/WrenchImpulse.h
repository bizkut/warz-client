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

#ifndef NM_MDF_TYPE_WRENCHIMPULSE_H
#define NM_MDF_TYPE_WRENCHIMPULSE_H

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
// Data Payload: 32 Bytes
// Alignment: 16

// The 6d vector of force and torque is called a 'wrench', so we can assume that wrenchImpulse is a good name for a vector of impulse and torqueImpulse

struct WrenchImpulse
{

  NMP::Vector3 impulse;                      ///< (Impulse)

  NMP::Vector3 torqueImpulse;                ///< (TorqueImpulse)


  // functions

  WrenchImpulse();
  void setToZero();


  NM_INLINE void operator *= (const float fVal)
  {
    impulse *= fVal;
    torqueImpulse *= fVal;
  }

  NM_INLINE WrenchImpulse operator * (const float fVal) const
  {
    WrenchImpulse result;
    result.impulse = impulse * fVal;
    result.torqueImpulse = torqueImpulse * fVal;
    return result;
  }

  NM_INLINE void operator += (const WrenchImpulse& rhs)
  {
    impulse += rhs.impulse;
    torqueImpulse += rhs.torqueImpulse;
  }

  NM_INLINE WrenchImpulse operator + (const WrenchImpulse& rhs) const
  {
    WrenchImpulse result;
    result.impulse = impulse + rhs.impulse;
    result.torqueImpulse = torqueImpulse + rhs.torqueImpulse;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(impulse), "impulse");
    NM_VALIDATOR(Vector3Valid(torqueImpulse), "torqueImpulse");
#endif // NM_CALL_VALIDATORS
  }

}; // struct WrenchImpulse


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_WRENCHIMPULSE_H

