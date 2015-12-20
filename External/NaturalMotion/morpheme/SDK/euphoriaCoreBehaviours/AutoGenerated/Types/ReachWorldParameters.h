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

#ifndef NM_MDF_TYPE_REACHWORLDPARAMETERS_H
#define NM_MDF_TYPE_REACHWORLDPARAMETERS_H

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

namespace ER { class DimensionalScaling; } 

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
struct ReachWorldParameters
{

  enum Strategies
  {
    /*  0 */ disable,  ///< To disable self avoidance.
    /*  1 */ automatic,  ///< Will decide between the four others (done in decideStrategy).
    /*  2 */ shoulderSide,  ///< Path (i.e. from hand to target via the temporary target) will always be on the shoulder side (shoulder side is defined by the direction 'root to base' of the arm).
    /*  3 */ targetSide,  ///< Path will always pass on the target side (target side is either shoulder side or the opposite).
    /*  4 */ shortest,  ///< Path will be the shortest possible.
  };

  NMP::Vector3 controlColour;

  int32_t swivelMode;  ///< A code to direct how reach behaviours should apply swivel.

  float swivelAmount;  ///< User specfied swivel amount in range [-1,1] will only be used if swivelMode is set to REACH_SWIVEL_SPECIFIED  (Multiplier)

  float chestRotationScale;  ///< To allow rotation of the chest towards the target.  (Multiplier)

  float pelvisRotationScale;  ///< To allow rotation of the pelvis towards the target.  (Multiplier)

  float maxChestTranslation;  ///< To clamp the translation required from the chest to reach the target.  (Multiplier)

  float maxPelvisTranslation;  ///< To clamp the translation required from the pelvis to reach the target.  (Multiplier)

  float unreachableTargetImportanceScale;  ///< How much should the character reach for targets that out of range. 0 = not at all, 1 = try as hard as possible.   (Weight)

  float torsoRadiusMultiplier;  ///< Positive multiplier for torso radius (self-avoidance input)  (Weight)

  float reachImminence;                      ///< (Imminence)

  float maxReachScale;  ///< Will clamp the distance of the IKTarget. Relative to the length of the arm.  (Multiplier)

  float ikSubstepSize;  ///< Proportion of whole step in "advanced IK" substepping, 1 corresponds to "no substepping".  (ClampedWeight)

  int32_t selfAvoidanceStrategy;  ///< Which strategy to apply.

  float selfAvoidanceSlackAngle;  ///< Deadzone in which direction of winding follows current effector position.

  bool slideAlongNormal;

  bool avoidLegsArea;  ///< Will move up the hand if it is below the pelvis.


  // functions

  void setToDefaults(const ER::DimensionalScaling& scaling);


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(controlColour), "controlColour");
    NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    NM_VALIDATOR(FloatValid(chestRotationScale), "chestRotationScale");
    NM_VALIDATOR(FloatValid(pelvisRotationScale), "pelvisRotationScale");
    NM_VALIDATOR(FloatValid(maxChestTranslation), "maxChestTranslation");
    NM_VALIDATOR(FloatValid(maxPelvisTranslation), "maxPelvisTranslation");
    NM_VALIDATOR(FloatNonNegative(unreachableTargetImportanceScale), "unreachableTargetImportanceScale");
    NM_VALIDATOR(FloatNonNegative(torsoRadiusMultiplier), "torsoRadiusMultiplier");
    NM_VALIDATOR(FloatValid(reachImminence), "reachImminence");
    NM_VALIDATOR(FloatValid(maxReachScale), "maxReachScale");
    NM_VALIDATOR(Float0to1(ikSubstepSize), "ikSubstepSize");
    NM_VALIDATOR(FloatValid(selfAvoidanceSlackAngle), "selfAvoidanceSlackAngle");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ReachWorldParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_REACHWORLDPARAMETERS_H

