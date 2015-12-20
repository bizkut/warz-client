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

#ifndef NM_MDF_TYPE_POSEDATA_H
#define NM_MDF_TYPE_POSEDATA_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMMatrix34.h"
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
// Data Payload: 128 Bytes
// Alignment: 16
struct PoseData
{

  NMP::Matrix34 poseEndRelativeToRoot;  ///< Transform of the end relative to the root  (Transform)

  NMP::Vector3 poseEndRelativeToRootVelocity;  ///< velocity of the end minus the velocity of the root  (Velocity)

  NMP::Vector3 poseEndRelativeToRootAngularVelocity;  ///< Angular velocity of the end minus the anular velocity of the root  (AngularVelocity)

  float stiffnessScale;  ///< Multiplier used to scale normalStiffness.  (Multiplier)

  float dampingRatioScale;  ///< Multiplier used to scale dampingRatio.  (Multiplier)

  float driveCompensationScale;  ///< Multiplier used to scale normalDriveCompensation.  (Multiplier)

  float gravityCompensation;  ///< Gravity compensation (defaults to 1)  (Multiplier)

  float strengthReductionTowardsEnd;  ///< Small values result in the end joint being as strong as the base joint. 1.0 will result in the end joint being completely loose. Larger values confine the strength to the joints near the limb base.  (Multiplier)


  // functions

  /// Constructor will set everything to sensible defaults or zero
  PoseData();


  NM_INLINE void operator *= (const float fVal)
  {
    poseEndRelativeToRoot *= fVal;
    poseEndRelativeToRootVelocity *= fVal;
    poseEndRelativeToRootAngularVelocity *= fVal;
    stiffnessScale *= fVal;
    dampingRatioScale *= fVal;
    driveCompensationScale *= fVal;
    gravityCompensation *= fVal;
    strengthReductionTowardsEnd *= fVal;
  }

  NM_INLINE PoseData operator * (const float fVal) const
  {
    PoseData result;
    result.poseEndRelativeToRoot = poseEndRelativeToRoot * fVal;
    result.poseEndRelativeToRootVelocity = poseEndRelativeToRootVelocity * fVal;
    result.poseEndRelativeToRootAngularVelocity = poseEndRelativeToRootAngularVelocity * fVal;
    result.stiffnessScale = stiffnessScale * fVal;
    result.dampingRatioScale = dampingRatioScale * fVal;
    result.driveCompensationScale = driveCompensationScale * fVal;
    result.gravityCompensation = gravityCompensation * fVal;
    result.strengthReductionTowardsEnd = strengthReductionTowardsEnd * fVal;
    return result;
  }

  NM_INLINE void operator += (const PoseData& rhs)
  {
    poseEndRelativeToRoot += rhs.poseEndRelativeToRoot;
    poseEndRelativeToRootVelocity += rhs.poseEndRelativeToRootVelocity;
    poseEndRelativeToRootAngularVelocity += rhs.poseEndRelativeToRootAngularVelocity;
    stiffnessScale += rhs.stiffnessScale;
    dampingRatioScale += rhs.dampingRatioScale;
    driveCompensationScale += rhs.driveCompensationScale;
    gravityCompensation += rhs.gravityCompensation;
    strengthReductionTowardsEnd += rhs.strengthReductionTowardsEnd;
  }

  NM_INLINE PoseData operator + (const PoseData& rhs) const
  {
    PoseData result;
    result.poseEndRelativeToRoot = poseEndRelativeToRoot + rhs.poseEndRelativeToRoot;
    result.poseEndRelativeToRootVelocity = poseEndRelativeToRootVelocity + rhs.poseEndRelativeToRootVelocity;
    result.poseEndRelativeToRootAngularVelocity = poseEndRelativeToRootAngularVelocity + rhs.poseEndRelativeToRootAngularVelocity;
    result.stiffnessScale = stiffnessScale + rhs.stiffnessScale;
    result.dampingRatioScale = dampingRatioScale + rhs.dampingRatioScale;
    result.driveCompensationScale = driveCompensationScale + rhs.driveCompensationScale;
    result.gravityCompensation = gravityCompensation + rhs.gravityCompensation;
    result.strengthReductionTowardsEnd = strengthReductionTowardsEnd + rhs.strengthReductionTowardsEnd;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(poseEndRelativeToRoot), "poseEndRelativeToRoot");
    NM_VALIDATOR(Vector3Valid(poseEndRelativeToRootVelocity), "poseEndRelativeToRootVelocity");
    NM_VALIDATOR(Vector3Valid(poseEndRelativeToRootAngularVelocity), "poseEndRelativeToRootAngularVelocity");
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatValid(dampingRatioScale), "dampingRatioScale");
    NM_VALIDATOR(FloatValid(driveCompensationScale), "driveCompensationScale");
    NM_VALIDATOR(FloatValid(gravityCompensation), "gravityCompensation");
    NM_VALIDATOR(FloatValid(strengthReductionTowardsEnd), "strengthReductionTowardsEnd");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Matrix34Orthonormalise(poseEndRelativeToRoot);
    validate();
  }

}; // struct PoseData


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_POSEDATA_H

