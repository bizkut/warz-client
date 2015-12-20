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

#ifndef NM_MDF_TYPE_REACHTARGETSOURCEINFO_H
#define NM_MDF_TYPE_REACHTARGETSOURCEINFO_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

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
// Data Payload: 112 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  

// used by ReachForWorldBehaviourInfo to store "reach for" info coming in from game/morpheme-side
//

struct ReachTargetSourceInfo
{

  NMP::Matrix34 objTM;  ///< coordinate frame of reference for point and normal  (Transform)

  NMP::Vector3 point;  ///< surface point  (Position)

  NMP::Vector3 normal;  ///< surface normal  (Direction)

  uint64_t actorId;  ///< from pointer to the physics obj if it exists, -1 if it doesn't, and 0 if the info is "invalid"

  float elapsedTime;  ///< updated each frame to show how long (in simulation time) we've known about this  (TimePeriod)


  // functions

  ReachTargetSourceInfo();
  bool isInvalid();
  void invalidate();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(objTM), "objTM");
    NM_VALIDATOR(Vector3Valid(point), "point");
    NM_VALIDATOR(Vector3Normalised(normal), "normal");
    NM_VALIDATOR(FloatValid(elapsedTime), "elapsedTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Matrix34Orthonormalise(objTM);
    PostCombiners::Vector3Normalise(normal);
    validate();
  }

}; // struct ReachTargetSourceInfo


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_REACHTARGETSOURCEINFO_H

