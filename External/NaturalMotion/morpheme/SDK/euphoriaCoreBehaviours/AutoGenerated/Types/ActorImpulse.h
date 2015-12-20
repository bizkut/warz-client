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

#ifndef NM_MDF_TYPE_ACTORIMPULSE_H
#define NM_MDF_TYPE_ACTORIMPULSE_H

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
// Modifiers: __no_combiner__  
struct ActorImpulse
{

  NMP::Vector3 point;  /// pos and direction of the impulse (coords wrt body or world depending on the flag)  (Position)

  NMP::Vector3 hitDir;                       ///< (Direction)

  float impulseMagnitude;  /// magnitude of impulse

  float torqueMultiplier;  ///< gives a means to exaggerate/tone down or even reverse the torque associated with the impulse

  int32_t limbIndex;  ///< If negative will be applied to all limbs of the character.

  int32_t partIndex;  ///< If negative will be applied to all parts of the limb (or limbs).

  bool useVelChange;

  bool positionFrameWorld;  /// To specify coordinate frame

  bool directionFrameWorld;


  // functions

  ActorImpulse();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(point), "point");
    NM_VALIDATOR(Vector3Normalised(hitDir), "hitDir");
    NM_VALIDATOR(FloatValid(impulseMagnitude), "impulseMagnitude");
    NM_VALIDATOR(FloatValid(torqueMultiplier), "torqueMultiplier");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(hitDir);
    validate();
  }

}; // struct ActorImpulse


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ACTORIMPULSE_H

