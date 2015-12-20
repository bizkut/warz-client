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

#ifndef NM_MDF_TYPE_BRACESTATE_H
#define NM_MDF_TYPE_BRACESTATE_H

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
struct BraceState
{

  NMP::Vector3 error;                        ///< (Position)

  float mass;



  NM_INLINE void operator *= (const float fVal)
  {
    error *= fVal;
    mass *= fVal;
  }

  NM_INLINE BraceState operator * (const float fVal) const
  {
    BraceState result;
    result.error = error * fVal;
    result.mass = mass * fVal;
    return result;
  }

  NM_INLINE void operator += (const BraceState& rhs)
  {
    error += rhs.error;
    mass += rhs.mass;
  }

  NM_INLINE BraceState operator + (const BraceState& rhs) const
  {
    BraceState result;
    result.error = error + rhs.error;
    result.mass = mass + rhs.mass;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(error), "error");
    NM_VALIDATOR(FloatValid(mass), "mass");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BraceState


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BRACESTATE_H

