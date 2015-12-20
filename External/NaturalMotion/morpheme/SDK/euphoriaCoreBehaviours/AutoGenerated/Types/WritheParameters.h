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

#ifndef NM_MDF_TYPE_WRITHEPARAMETERS_H
#define NM_MDF_TYPE_WRITHEPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMMatrix34.h"

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
// Data Payload: 96 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  
struct WritheParameters
{

  NMP::Matrix34 defaultPoseEndRelativeToRoot;  ///< Transform of the end relative to the root  (Transform)

  float amplitudeScale;  ///< Proportion of limb length  (Weight)

  float resetTargetTP;  ///< Time period after which the writhe targets are recalculated  (TimePeriod)

  float stiffnessScale;  ///< Multiplier used to multiply the normalStiffness.  (Weight)

  float driveCompensationScale;  ///< Multiplier used to multiply the normalDriveCompensation.  (Weight)

  bool basedOnDefaultPose;  ///< If false will be based on the current endTM.



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndRelativeToRoot), "defaultPoseEndRelativeToRoot");
    NM_VALIDATOR(FloatNonNegative(amplitudeScale), "amplitudeScale");
    NM_VALIDATOR(FloatValid(resetTargetTP), "resetTargetTP");
    NM_VALIDATOR(FloatNonNegative(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatNonNegative(driveCompensationScale), "driveCompensationScale");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Matrix34Orthonormalise(defaultPoseEndRelativeToRoot);
    validate();
  }

}; // struct WritheParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_WRITHEPARAMETERS_H

