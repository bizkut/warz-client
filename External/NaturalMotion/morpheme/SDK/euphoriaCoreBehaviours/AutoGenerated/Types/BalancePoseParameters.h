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

#ifndef NM_MDF_TYPE_BALANCEPOSEPARAMETERS_H
#define NM_MDF_TYPE_BALANCEPOSEPARAMETERS_H

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
struct BalancePoseParameters
{

  NMP::Vector3 chestTranslation;             ///< (PositionDelta)

  float yaw;  ///< Around spine axis (yaw)  (Angle)

  float pitch;  ///< Lean fwd/bwd      (pitch)  (Angle)

  float roll;  ///< Lean left/right   (roll)  (Angle)

  float pelvisMultiplier;  ///< Used only for the rotations.  (Multiplier)

  float spineMultiplier;  ///< Used only for the rotations.  (Multiplier)

  float headMultiplier;  ///< Used only for the rotations.  (Multiplier)

  float crouchAmount;  ///< Fraction of the balance pose to crouch  (Multiplier)


  // functions

  BalancePoseParameters();


  NM_INLINE void operator *= (const float fVal)
  {
    chestTranslation *= fVal;
    yaw *= fVal;
    pitch *= fVal;
    roll *= fVal;
    pelvisMultiplier *= fVal;
    spineMultiplier *= fVal;
    headMultiplier *= fVal;
    crouchAmount *= fVal;
  }

  NM_INLINE BalancePoseParameters operator * (const float fVal) const
  {
    BalancePoseParameters result;
    result.chestTranslation = chestTranslation * fVal;
    result.yaw = yaw * fVal;
    result.pitch = pitch * fVal;
    result.roll = roll * fVal;
    result.pelvisMultiplier = pelvisMultiplier * fVal;
    result.spineMultiplier = spineMultiplier * fVal;
    result.headMultiplier = headMultiplier * fVal;
    result.crouchAmount = crouchAmount * fVal;
    return result;
  }

  NM_INLINE void operator += (const BalancePoseParameters& rhs)
  {
    chestTranslation += rhs.chestTranslation;
    yaw += rhs.yaw;
    pitch += rhs.pitch;
    roll += rhs.roll;
    pelvisMultiplier += rhs.pelvisMultiplier;
    spineMultiplier += rhs.spineMultiplier;
    headMultiplier += rhs.headMultiplier;
    crouchAmount += rhs.crouchAmount;
  }

  NM_INLINE BalancePoseParameters operator + (const BalancePoseParameters& rhs) const
  {
    BalancePoseParameters result;
    result.chestTranslation = chestTranslation + rhs.chestTranslation;
    result.yaw = yaw + rhs.yaw;
    result.pitch = pitch + rhs.pitch;
    result.roll = roll + rhs.roll;
    result.pelvisMultiplier = pelvisMultiplier + rhs.pelvisMultiplier;
    result.spineMultiplier = spineMultiplier + rhs.spineMultiplier;
    result.headMultiplier = headMultiplier + rhs.headMultiplier;
    result.crouchAmount = crouchAmount + rhs.crouchAmount;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(chestTranslation), "chestTranslation");
    NM_VALIDATOR(FloatValid(yaw), "yaw");
    NM_VALIDATOR(FloatValid(pitch), "pitch");
    NM_VALIDATOR(FloatValid(roll), "roll");
    NM_VALIDATOR(FloatValid(pelvisMultiplier), "pelvisMultiplier");
    NM_VALIDATOR(FloatValid(spineMultiplier), "spineMultiplier");
    NM_VALIDATOR(FloatValid(headMultiplier), "headMultiplier");
    NM_VALIDATOR(FloatValid(crouchAmount), "crouchAmount");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BalancePoseParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BALANCEPOSEPARAMETERS_H

