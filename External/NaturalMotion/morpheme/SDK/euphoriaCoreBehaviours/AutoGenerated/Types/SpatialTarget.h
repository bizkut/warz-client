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

#ifndef NM_MDF_TYPE_SPATIALTARGET_H
#define NM_MDF_TYPE_SPATIALTARGET_H

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

// a simple target definition

struct SpatialTarget
{

  NMP::Vector3 position;                     ///< (Position)

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 normal;                       ///< (Direction)


  // functions

  SpatialTarget();
  SpatialTarget(const NMP::Vector3& pos, const NMP::Vector3& vel);


  NM_INLINE void operator *= (const float fVal)
  {
    position *= fVal;
    velocity *= fVal;
    normal *= fVal;
  }

  NM_INLINE SpatialTarget operator * (const float fVal) const
  {
    SpatialTarget result;
    result.position = position * fVal;
    result.velocity = velocity * fVal;
    result.normal = normal * fVal;
    return result;
  }

  NM_INLINE void operator += (const SpatialTarget& rhs)
  {
    position += rhs.position;
    velocity += rhs.velocity;
    normal += rhs.normal;
  }

  NM_INLINE SpatialTarget operator + (const SpatialTarget& rhs) const
  {
    SpatialTarget result;
    result.position = position + rhs.position;
    result.velocity = velocity + rhs.velocity;
    result.normal = normal + rhs.normal;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(position), "position");
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Normalised(normal), "normal");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(normal);
    validate();
  }

}; // struct SpatialTarget


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_SPATIALTARGET_H

