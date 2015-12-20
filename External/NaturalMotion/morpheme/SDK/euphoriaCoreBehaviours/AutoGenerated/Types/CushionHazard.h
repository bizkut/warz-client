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

#ifndef NM_MDF_TYPE_CUSHIONHAZARD_H
#define NM_MDF_TYPE_CUSHIONHAZARD_H

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
struct CushionHazard
{

  NMP::Vector3 hazardPosition;               ///< (Position)

  NMP::Vector3 hazardVelocity;               ///< (Velocity)

  NMP::Vector3 protectPoint;                 ///< (Position)

  NMP::Vector3 protectVelocity;              ///< (Velocity)

  float imminence;                           ///< (Imminence)



  NM_INLINE void operator *= (const float fVal)
  {
    hazardPosition *= fVal;
    hazardVelocity *= fVal;
    protectPoint *= fVal;
    protectVelocity *= fVal;
    imminence *= fVal;
  }

  NM_INLINE CushionHazard operator * (const float fVal) const
  {
    CushionHazard result;
    result.hazardPosition = hazardPosition * fVal;
    result.hazardVelocity = hazardVelocity * fVal;
    result.protectPoint = protectPoint * fVal;
    result.protectVelocity = protectVelocity * fVal;
    result.imminence = imminence * fVal;
    return result;
  }

  NM_INLINE void operator += (const CushionHazard& rhs)
  {
    hazardPosition += rhs.hazardPosition;
    hazardVelocity += rhs.hazardVelocity;
    protectPoint += rhs.protectPoint;
    protectVelocity += rhs.protectVelocity;
    imminence += rhs.imminence;
  }

  NM_INLINE CushionHazard operator + (const CushionHazard& rhs) const
  {
    CushionHazard result;
    result.hazardPosition = hazardPosition + rhs.hazardPosition;
    result.hazardVelocity = hazardVelocity + rhs.hazardVelocity;
    result.protectPoint = protectPoint + rhs.protectPoint;
    result.protectVelocity = protectVelocity + rhs.protectVelocity;
    result.imminence = imminence + rhs.imminence;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(hazardPosition), "hazardPosition");
    NM_VALIDATOR(Vector3Valid(hazardVelocity), "hazardVelocity");
    NM_VALIDATOR(Vector3Valid(protectPoint), "protectPoint");
    NM_VALIDATOR(Vector3Valid(protectVelocity), "protectVelocity");
    NM_VALIDATOR(FloatValid(imminence), "imminence");
#endif // NM_CALL_VALIDATORS
  }

}; // struct CushionHazard


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_CUSHIONHAZARD_H

