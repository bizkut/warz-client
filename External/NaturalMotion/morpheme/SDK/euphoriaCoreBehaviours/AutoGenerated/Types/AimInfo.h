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

#ifndef NM_MDF_TYPE_AIMINFO_H
#define NM_MDF_TYPE_AIMINFO_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "euphoria/erDebugDraw.h"

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
// Data Payload: 128 Bytes
// Alignment: 16
struct AimInfo
{

  NMP::Matrix34 poseEndRelativeToRoot;       ///< (Transform)

  float gravityCompensation;                 ///< (Multiplier)

  float dampingScale;                        ///< (Multiplier)

  float strengthScale;                       ///< (Multiplier)

  float swivelAmount;                        ///< (Multiplier)

  float limbControlPositionWeight;           ///< (Weight)

  float limbControlOrientationWeight;        ///< (Weight)

  float limbControlDeltaStep;                ///< (Multiplier)

  float poseTwistWeight;  // Advanced IK substepping parameter.  (Weight)

  float poseSwingWeight;  // How much the limb should twist when aiming.  (Weight)

  float rootRotationCompensation;  // How much the limb should swing when aiming.  (Multiplier)

#if defined(MR_OUTPUT_DEBUGGING)
  float aimDirection[3];                     ///< (DebugDrawFloat)
#endif // MR_OUTPUT_DEBUGGING

#if defined(MR_OUTPUT_DEBUGGING)
  float aimingLimbIndex;                     ///< (DebugDrawFloat)
#endif // MR_OUTPUT_DEBUGGING

  bool useIncrementalIK;  // How much should the limb target be modified to acount for the rotation of the root.



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(poseEndRelativeToRoot), "poseEndRelativeToRoot");
    NM_VALIDATOR(FloatValid(gravityCompensation), "gravityCompensation");
    NM_VALIDATOR(FloatValid(dampingScale), "dampingScale");
    NM_VALIDATOR(FloatValid(strengthScale), "strengthScale");
    NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    NM_VALIDATOR(FloatNonNegative(limbControlPositionWeight), "limbControlPositionWeight");
    NM_VALIDATOR(FloatNonNegative(limbControlOrientationWeight), "limbControlOrientationWeight");
    NM_VALIDATOR(FloatValid(limbControlDeltaStep), "limbControlDeltaStep");
    NM_VALIDATOR(FloatNonNegative(poseTwistWeight), "poseTwistWeight");
    NM_VALIDATOR(FloatNonNegative(poseSwingWeight), "poseSwingWeight");
    NM_VALIDATOR(FloatValid(rootRotationCompensation), "rootRotationCompensation");
#if defined(MR_OUTPUT_DEBUGGING)
    NM_VALIDATOR(FloatValid(aimDirection[0]), "aimDirection[0]");
    NM_VALIDATOR(FloatValid(aimDirection[1]), "aimDirection[1]");
    NM_VALIDATOR(FloatValid(aimDirection[2]), "aimDirection[2]");
#endif // MR_OUTPUT_DEBUGGING
#if defined(MR_OUTPUT_DEBUGGING)
    NM_VALIDATOR(FloatValid(aimingLimbIndex), "aimingLimbIndex");
#endif // MR_OUTPUT_DEBUGGING
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Matrix34Orthonormalise(poseEndRelativeToRoot);
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
#if defined(MR_OUTPUT_DEBUGGING)
#endif // MR_OUTPUT_DEBUGGING
    validate();
  }

}; // struct AimInfo


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_AIMINFO_H

