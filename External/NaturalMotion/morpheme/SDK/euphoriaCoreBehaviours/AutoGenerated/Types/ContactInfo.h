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

#ifndef NM_MDF_TYPE_CONTACTINFO_H
#define NM_MDF_TYPE_CONTACTINFO_H

// include definition file to create project dependency
#include "./Definition/Types/Limb.types"

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
// Declaration from 'Limb.types'
// Data Payload: 80 Bytes
// Alignment: 16
struct ContactInfo
{

  NMP::Vector3 lastVelocity;  ///< velocity of contacted part at contact, last observed velocity if contact has parted  (Velocity)

  NMP::Vector3 lastPosition;  ///< position of contacted part at contact, last observed position if contact has parted  (Position)

  NMP::Vector3 lastNormal;  ///< normal of contacted part at contact, last observed normal if contact has parted  (Direction)

  NMP::Vector3 lastForce;  ///< relative force at contact, last observed if contact has parted  (Force)

  bool inContact;  ///< True if currently in contact



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(lastVelocity), "lastVelocity");
    NM_VALIDATOR(Vector3Valid(lastPosition), "lastPosition");
    NM_VALIDATOR(Vector3Normalised(lastNormal), "lastNormal");
    NM_VALIDATOR(Vector3Valid(lastForce), "lastForce");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(lastNormal);
    validate();
  }

}; // struct ContactInfo


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_CONTACTINFO_H

