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

#ifndef NM_MDF_TYPE_BALANCEPARAMETERS_H
#define NM_MDF_TYPE_BALANCEPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

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
// Data Payload: 84 Bytes
// Modifiers: __no_combiner__  
struct BalanceParameters
{

  float footBalanceAmount;                   ///< (Angle)

  float decelerationAmount;                  ///< (Multiplier)

  float maxAngle;                            ///< (Angle)

  float footLength;                          ///< (Distance)

  float lowerPelvisDistanceWhenFootLifts;    ///< (Distance)

  float balanceWeaknessPerLeg[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)

  float balanceWeaknessPerArm[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)

  float supportPointWeightingPerLeg[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)

  float supportPointWeightingPerArm[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)

  float supportFwdOffset;                    ///< (Multiplier)

  float supportRightOffset;                  ///< (Multiplier)

  float supportFwdRange;                     ///< (Multiplier)

  float supportSideRange;                    ///< (Multiplier)

  float supportRangeTimescale;               ///< (Multiplier)

  float steppingDirectionThreshold;          ///< (Multiplier)

  float spinAmount;                          ///< (Multiplier)

  float spinThreshold;                       ///< (Multiplier)


  // functions

  BalanceParameters();


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(footBalanceAmount), "footBalanceAmount");
    NM_VALIDATOR(FloatValid(decelerationAmount), "decelerationAmount");
    NM_VALIDATOR(FloatValid(maxAngle), "maxAngle");
    NM_VALIDATOR(FloatValid(footLength), "footLength");
    NM_VALIDATOR(FloatValid(lowerPelvisDistanceWhenFootLifts), "lowerPelvisDistanceWhenFootLifts");
    NM_VALIDATOR(FloatValid(balanceWeaknessPerLeg[0]), "balanceWeaknessPerLeg[0]");
    NM_VALIDATOR(FloatValid(balanceWeaknessPerLeg[1]), "balanceWeaknessPerLeg[1]");
    NM_VALIDATOR(FloatValid(balanceWeaknessPerArm[0]), "balanceWeaknessPerArm[0]");
    NM_VALIDATOR(FloatValid(balanceWeaknessPerArm[1]), "balanceWeaknessPerArm[1]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerLeg[0]), "supportPointWeightingPerLeg[0]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerLeg[1]), "supportPointWeightingPerLeg[1]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerArm[0]), "supportPointWeightingPerArm[0]");
    NM_VALIDATOR(FloatValid(supportPointWeightingPerArm[1]), "supportPointWeightingPerArm[1]");
    NM_VALIDATOR(FloatValid(supportFwdOffset), "supportFwdOffset");
    NM_VALIDATOR(FloatValid(supportRightOffset), "supportRightOffset");
    NM_VALIDATOR(FloatValid(supportFwdRange), "supportFwdRange");
    NM_VALIDATOR(FloatValid(supportSideRange), "supportSideRange");
    NM_VALIDATOR(FloatValid(supportRangeTimescale), "supportRangeTimescale");
    NM_VALIDATOR(FloatValid(steppingDirectionThreshold), "steppingDirectionThreshold");
    NM_VALIDATOR(FloatValid(spinAmount), "spinAmount");
    NM_VALIDATOR(FloatValid(spinThreshold), "spinThreshold");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BalanceParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BALANCEPARAMETERS_H

