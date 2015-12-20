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

#ifndef NM_MDF_TYPE_ENVIRONMENT_OBJECTDATA_H
#define NM_MDF_TYPE_ENVIRONMENT_OBJECTDATA_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

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

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 80 Bytes
// Alignment: 16
struct ObjectData
{

  NMP::Vector3 lastVel;

  NMP::Vector3 secondLastVel;

  NMP::Vector3 smoothedAcc;

  NMP::Vector3 smoothedAngVel;

  int64_t shapeID;  ///< Will be cast from a pointer to the PxShape

  float smoothedAccSqr;

  int32_t inViewFrameCount;  ///< how many frames has it been in view



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(lastVel), "lastVel");
    NM_VALIDATOR(Vector3Valid(secondLastVel), "secondLastVel");
    NM_VALIDATOR(Vector3Valid(smoothedAcc), "smoothedAcc");
    NM_VALIDATOR(Vector3Valid(smoothedAngVel), "smoothedAngVel");
    NM_VALIDATOR(FloatValid(smoothedAccSqr), "smoothedAccSqr");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ObjectData

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_OBJECTDATA_H

