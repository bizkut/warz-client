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

#ifndef NM_MDF_TYPE_FREEFALLPARAMETERS_H
#define NM_MDF_TYPE_FREEFALLPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMQuat.h"

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
struct FreefallParameters
{

  NMP::Quat orientation;  ///< Target character orientation, relative to the base orientation.

  float assistanceAmount;  ///< Unitless value to keep character landing on its feet through pelvis torque  (Weight)

  float dampingAmount;  ///< Unitless value to stop character tumbling by applying pelvis torque  (Weight)

  float angleLandingAmount;  ///< 0 is no angle to landing, 1 is match impact velocity  (Weight)

  float rotationTime;  ///< The amount of time in which to achieve the rotation, in seconds on the standard character.

  float armsAmount;  ///< Multiplier on the arm movement

  float legsAmount;  ///< Multiplier on the leg movement

  float startOrientationTime;  ///< The time to start driving the character's orientation, in seconds on the standard character.  (TimePeriod)

  float stopOrientationTime;  ///< The time to stop driving the character's orientation, in seconds on the standard character.  (TimePeriod)

  float weight;  ///< Weight determines how strongly the character is driven towards the target orientation.

  int32_t characterAxis[2];  ///< Id of an axis in character space. Character and target axis coincide in the base orientation.

  int32_t targetAxis[2];  ///< Id of an axis in world space. Character and target axis coincide in the base orientation.

  float secondaryDirectionThreshold;  ///< If 1, then the secondary axis will always be used, even if it is parallel to the primary axis (e.g. if primary/secondary are set to up/velocity, and the character is falling vertically). As this value is decreased down to zero then the secondary axis is used only if it is increasingly different from the primary axis.

  bool startOrientationAtTimeBeforeImpact;  ///< The StartOrientationTime will be interpreted as time before impact if this is true, else it is interpreted as time after entering freefall.

  bool stopOrientationAtTimeBeforeImpact;  ///< The StopOrientationTime will be interpreted as time before impact if this is true, else it is interpreted as time after entering freefall.

  bool disableWhenInContact;  ///< Freefall does not drive in the presence of contacts when this flag is set.



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(assistanceAmount), "assistanceAmount");
    NM_VALIDATOR(FloatNonNegative(dampingAmount), "dampingAmount");
    NM_VALIDATOR(FloatNonNegative(angleLandingAmount), "angleLandingAmount");
    NM_VALIDATOR(FloatValid(rotationTime), "rotationTime");
    NM_VALIDATOR(FloatValid(armsAmount), "armsAmount");
    NM_VALIDATOR(FloatValid(legsAmount), "legsAmount");
    NM_VALIDATOR(FloatValid(startOrientationTime), "startOrientationTime");
    NM_VALIDATOR(FloatValid(stopOrientationTime), "stopOrientationTime");
    NM_VALIDATOR(FloatValid(weight), "weight");
    NM_VALIDATOR(FloatValid(secondaryDirectionThreshold), "secondaryDirectionThreshold");
#endif // NM_CALL_VALIDATORS
  }

}; // struct FreefallParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_FREEFALLPARAMETERS_H

