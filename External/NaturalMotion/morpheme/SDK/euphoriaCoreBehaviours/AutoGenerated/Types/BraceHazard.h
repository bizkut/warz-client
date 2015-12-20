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

#ifndef NM_MDF_TYPE_BRACEHAZARD_H
#define NM_MDF_TYPE_BRACEHAZARD_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Environment_Patch.h"

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
// Data Payload: 288 Bytes
// Alignment: 16
struct BraceHazard
{

  NMP::Vector3 position;  /// put some helper constructors here to allow other modules to call the brace  (Position)

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 normal;                       ///< (Direction)

  Environment::Patch patch;
 // 240 Bytes 


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(position), "position");
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Normalised(normal), "normal");
    patch.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(normal);
    validate();
  }

}; // struct BraceHazard


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BRACEHAZARD_H

