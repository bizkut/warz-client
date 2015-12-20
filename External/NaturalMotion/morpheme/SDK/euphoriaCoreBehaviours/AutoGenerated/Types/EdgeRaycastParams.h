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

#ifndef NM_MDF_TYPE_EDGERAYCASTPARAMS_H
#define NM_MDF_TYPE_EDGERAYCASTPARAMS_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

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
// Declaration from 'Hold.types'
// Data Payload: 13 Bytes

//----------------------------------------------------------------------------------------------------------------------
// Ray-cast params.
//----------------------------------------------------------------------------------------------------------------------

struct EdgeRaycastParams
{

  float raycastEdgeOffset;                   ///< (Length)

  float raycastLength;                       ///< (Length)

  float raycastHitAngle;                     ///< (Angle)

  bool enableRaycast;


  // functions

  void initialise(const ER::DimensionalScaling& scaling);


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(raycastEdgeOffset), "raycastEdgeOffset");
    NM_VALIDATOR(FloatNonNegative(raycastLength), "raycastLength");
    NM_VALIDATOR(FloatValid(raycastHitAngle), "raycastHitAngle");
#endif // NM_CALL_VALIDATORS
  }

}; // struct EdgeRaycastParams


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_EDGERAYCASTPARAMS_H

