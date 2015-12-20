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

#ifndef NM_MDF_TYPE_LIMBHANDLEDATA_H
#define NM_MDF_TYPE_LIMBHANDLEDATA_H

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
// Modifiers: __no_combiner__  

///< used for each limb handle, e.g. the end, the root and the base (eg shoulder)
struct LimbHandleData
{

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 angularVelocity;              ///< (AngularVelocity)

  int64_t lastCollisionID;  ///< Cast from pointer

  bool isCollidingWithWorld;


  // functions

  LimbHandleData();
  LimbHandleData(const NMP::Vector3& v, const NMP::Vector3& angVel, bool colliding, int64_t collisionId);


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Valid(angularVelocity), "angularVelocity");
#endif // NM_CALL_VALIDATORS
  }

}; // struct LimbHandleData


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_LIMBHANDLEDATA_H

