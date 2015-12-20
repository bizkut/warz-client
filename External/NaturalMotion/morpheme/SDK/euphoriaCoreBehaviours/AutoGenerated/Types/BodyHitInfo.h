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

#ifndef NM_MDF_TYPE_BODYHITINFO_H
#define NM_MDF_TYPE_BODYHITINFO_H

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
// Data Payload: 80 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  
struct BodyHitInfo
{

  NMP::Vector3 point;  /// pos and normal of the hit point (coordinate frame maybe body local or world depending on context)  (Position)

  NMP::Vector3 normal;                       ///< (Direction)

  NMP::Vector3 hitDir;                       ///< (Direction)

  uint64_t actorId;  ///< uint repn of the physics system actor pointer

  int32_t limbRigIndex;  /// indexes to identify the limb and part thereof

  int32_t partIndex;

  float elapsedTime;  /// set to zero when the hit is first created/detected, can be incremented at each tick while it persists

  uint32_t hitCode;

  int32_t partType;


  // functions

  BodyHitInfo();
  bool isInvalid();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(point), "point");
    NM_VALIDATOR(Vector3Normalised(normal), "normal");
    NM_VALIDATOR(Vector3Normalised(hitDir), "hitDir");
    NM_VALIDATOR(FloatValid(elapsedTime), "elapsedTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(normal);
    PostCombiners::Vector3Normalise(hitDir);
    validate();
  }

}; // struct BodyHitInfo


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BODYHITINFO_H

