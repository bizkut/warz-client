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

#ifndef NM_MDF_TYPE_PROTECTPARAMETERS_H
#define NM_MDF_TYPE_PROTECTPARAMETERS_H

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
struct ProtectParameters
{

  NMP::Vector3 offset;                       ///< (PositionDelta)

  float headLookAmount;                      ///< (Weight)

  float sensitivityToCloseMovements;         ///< (Multiplier)

  float supportIgnoreRadius;                 ///< (Length)

  float objectTrackingRadius;                ///< (Length)

  float hazardLookTime;                      ///< (TimePeriod)



  NM_INLINE void operator *= (const float fVal)
  {
    offset *= fVal;
    headLookAmount *= fVal;
    sensitivityToCloseMovements *= fVal;
    supportIgnoreRadius *= fVal;
    objectTrackingRadius *= fVal;
    hazardLookTime *= fVal;
  }

  NM_INLINE ProtectParameters operator * (const float fVal) const
  {
    ProtectParameters result;
    result.offset = offset * fVal;
    result.headLookAmount = headLookAmount * fVal;
    result.sensitivityToCloseMovements = sensitivityToCloseMovements * fVal;
    result.supportIgnoreRadius = supportIgnoreRadius * fVal;
    result.objectTrackingRadius = objectTrackingRadius * fVal;
    result.hazardLookTime = hazardLookTime * fVal;
    return result;
  }

  NM_INLINE void operator += (const ProtectParameters& rhs)
  {
    offset += rhs.offset;
    headLookAmount += rhs.headLookAmount;
    sensitivityToCloseMovements += rhs.sensitivityToCloseMovements;
    supportIgnoreRadius += rhs.supportIgnoreRadius;
    objectTrackingRadius += rhs.objectTrackingRadius;
    hazardLookTime += rhs.hazardLookTime;
  }

  NM_INLINE ProtectParameters operator + (const ProtectParameters& rhs) const
  {
    ProtectParameters result;
    result.offset = offset + rhs.offset;
    result.headLookAmount = headLookAmount + rhs.headLookAmount;
    result.sensitivityToCloseMovements = sensitivityToCloseMovements + rhs.sensitivityToCloseMovements;
    result.supportIgnoreRadius = supportIgnoreRadius + rhs.supportIgnoreRadius;
    result.objectTrackingRadius = objectTrackingRadius + rhs.objectTrackingRadius;
    result.hazardLookTime = hazardLookTime + rhs.hazardLookTime;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(offset), "offset");
    NM_VALIDATOR(FloatNonNegative(headLookAmount), "headLookAmount");
    NM_VALIDATOR(FloatValid(sensitivityToCloseMovements), "sensitivityToCloseMovements");
    NM_VALIDATOR(FloatNonNegative(supportIgnoreRadius), "supportIgnoreRadius");
    NM_VALIDATOR(FloatNonNegative(objectTrackingRadius), "objectTrackingRadius");
    NM_VALIDATOR(FloatValid(hazardLookTime), "hazardLookTime");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ProtectParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_PROTECTPARAMETERS_H

