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

#ifndef NM_MDF_TYPE_WRENCH_H
#define NM_MDF_TYPE_WRENCH_H

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

// The 6d vector of force and torque is called a 'wrench', this name comes from screw theory which is applicable to any 3d dynamics

struct Wrench
{

  NMP::Vector3 force;                        ///< (Force)

  NMP::Vector3 torque;                       ///< (Torque)


  // functions

  Wrench();
  void setToZero();


  NM_INLINE void operator *= (const float fVal)
  {
    force *= fVal;
    torque *= fVal;
  }

  NM_INLINE Wrench operator * (const float fVal) const
  {
    Wrench result;
    result.force = force * fVal;
    result.torque = torque * fVal;
    return result;
  }

  NM_INLINE void operator += (const Wrench& rhs)
  {
    force += rhs.force;
    torque += rhs.torque;
  }

  NM_INLINE Wrench operator + (const Wrench& rhs) const
  {
    Wrench result;
    result.force = force + rhs.force;
    result.torque = torque + rhs.torque;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(force), "force");
    NM_VALIDATOR(Vector3Valid(torque), "torque");
#endif // NM_CALL_VALIDATORS
  }

}; // struct Wrench


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_WRENCH_H

