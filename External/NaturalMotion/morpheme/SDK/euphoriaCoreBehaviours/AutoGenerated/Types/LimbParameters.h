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

#ifndef NM_MDF_TYPE_LIMBPARAMETERS_H
#define NM_MDF_TYPE_LIMBPARAMETERS_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

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
// Data Payload: 12 Bytes
struct LimbParameters
{

  float stiffnessScale;  ///< Multiplier used to multiply the normalStiffness of the limb.  (Multiplier)

  float dampingScale;  ///< Multiplier used to multiply the normalDampingRatio of the limb.  (Multiplier)

  float driveCompensationScale;  ///< Multiplier used to multiply the normalDriveCompensation of the limb.  (Multiplier)



  NM_INLINE void operator *= (const float fVal)
  {
    stiffnessScale *= fVal;
    dampingScale *= fVal;
    driveCompensationScale *= fVal;
  }

  NM_INLINE LimbParameters operator * (const float fVal) const
  {
    LimbParameters result;
    result.stiffnessScale = stiffnessScale * fVal;
    result.dampingScale = dampingScale * fVal;
    result.driveCompensationScale = driveCompensationScale * fVal;
    return result;
  }

  NM_INLINE void operator += (const LimbParameters& rhs)
  {
    stiffnessScale += rhs.stiffnessScale;
    dampingScale += rhs.dampingScale;
    driveCompensationScale += rhs.driveCompensationScale;
  }

  NM_INLINE LimbParameters operator + (const LimbParameters& rhs) const
  {
    LimbParameters result;
    result.stiffnessScale = stiffnessScale + rhs.stiffnessScale;
    result.dampingScale = dampingScale + rhs.dampingScale;
    result.driveCompensationScale = driveCompensationScale + rhs.driveCompensationScale;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
    NM_VALIDATOR(FloatValid(dampingScale), "dampingScale");
    NM_VALIDATOR(FloatValid(driveCompensationScale), "driveCompensationScale");
#endif // NM_CALL_VALIDATORS
  }

}; // struct LimbParameters


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_LIMBPARAMETERS_H

