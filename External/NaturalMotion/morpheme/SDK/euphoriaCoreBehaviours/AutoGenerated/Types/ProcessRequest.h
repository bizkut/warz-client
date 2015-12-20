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

#ifndef NM_MDF_TYPE_PROCESSREQUEST_H
#define NM_MDF_TYPE_PROCESSREQUEST_H

// include definition file to create project dependency
#include "./Definition/Types/PositionAndOrientation.types"

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
// Declaration from 'PositionAndOrientation.types'
// Data Payload: 12 Bytes

// Used for choosing the weight to apply requests

struct ProcessRequest
{

  float imminence;  ///< same as input imminence, to be passed into any motion requests

  float importance;  ///< importance of any motion requests

  float stiffnessScale;  ///< scalar to optionally apply to the control stiffness for imminent targets


  // functions

  // Stores values to optionally use and returns importance for this controller
  // importance:       importance of inbound request, or 1 when generating new control 
  // imminence:        imminence of constraint (1/time to event)
  // controlStiffness: stiffness of control if controller used fully
  // passOnAmount:     0 means receiver passes none futher, 1 means pass default amount down (even distribution), inf means pass all down
  // minStiffness:     minimum stiffness, any constraint that requires less than this does nothing 
  // maxStiffness:     maximum stiffness, any constraint that requires more won’t be fully met
  float processRequest(float importance, float imminence, float controlStiffness, float passOnAmount=1, float minStiffness=0, float maxStiffness=10000000);


  NM_INLINE void operator *= (const float fVal)
  {
    imminence *= fVal;
    importance *= fVal;
    stiffnessScale *= fVal;
  }

  NM_INLINE ProcessRequest operator * (const float fVal) const
  {
    ProcessRequest result;
    result.imminence = imminence * fVal;
    result.importance = importance * fVal;
    result.stiffnessScale = stiffnessScale * fVal;
    return result;
  }

  NM_INLINE void operator += (const ProcessRequest& rhs)
  {
    imminence += rhs.imminence;
    importance += rhs.importance;
    stiffnessScale += rhs.stiffnessScale;
  }

  NM_INLINE ProcessRequest operator + (const ProcessRequest& rhs) const
  {
    ProcessRequest result;
    result.imminence = imminence + rhs.imminence;
    result.importance = importance + rhs.importance;
    result.stiffnessScale = stiffnessScale + rhs.stiffnessScale;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(imminence), "imminence");
    NM_VALIDATOR(FloatValid(importance), "importance");
    NM_VALIDATOR(FloatValid(stiffnessScale), "stiffnessScale");
#endif // NM_CALL_VALIDATORS
  }

}; // struct ProcessRequest


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_PROCESSREQUEST_H

