/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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

#include "Types/ProcessRequest.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

// This value is calculated to be the optimal value for an interception with a target
// it should not need to change
const float interceptUrgencyScale = sqrtf(6.0f);

//----------------------------------------------------------------------------------------------------------------------
// ProcessRequest
//----------------------------------------------------------------------------------------------------------------------
float ProcessRequest::processRequest(
  float imp,
  float imm,
  float controlStiffness,
  float passOnAmount,
  float minStiffness,
  float maxStiffness)
{
  NMP_ASSERT(imm >= 0.0f);
  const float epsilon = 1e-10f;
  imminence = imm;
  // to be overridden later in most cases
  importance = imp;

  // by accruing the urgency at each stage, we know how much left of the motion
  // we need to apply to the next stage down
  float accruedUrgency = (1.0f - imp) / (imp + epsilon);
  // The specified stiffness is never really reached in the physics engine, due to lack of coordinated motion
  // so, from tests, a scale value below works better than using a value of 1.
  const float controlStiffnessScale = 0.5f;
  float stiffness = controlStiffness * controlStiffnessScale;
  float urgencyUnclamped = interceptUrgencyScale * imm / (epsilon + stiffness);

  // if an object can't reach the target in time, it is given full urgency...
  // but we also provide a stiffnessScale here that you can use to increase the
  // parts stiffness, so it will get there in time the motion of all connected
  // objects does not need to change, regardless of if this stiffness scale is
  // applied or not, since it doesn't affect localUrgency value
  stiffnessScale = NMP::minimum(NMP::maximum(1.0f, urgencyUnclamped), maxStiffness / controlStiffness);
  if (urgencyUnclamped* stiffness <= minStiffness)
  {
    return 0.0f;
  }

  // weigh the urgency against the urgency already 'used up' higher up this pipeline
  float localUrgency = NMP::minimum(urgencyUnclamped, 1.0f);
  float importanceResult = localUrgency * localUrgency / (accruedUrgency + localUrgency + epsilon);

  // since accruedUrgency increases at each stage down, the importance reduces
  // below is same as imp = 1/(acr + 1) where acr = accruedUrgency + localUrgency/passOnAmount
  importance = passOnAmount / (epsilon + localUrgency + passOnAmount * (1.0f + accruedUrgency));

  return importanceResult;
}

}

// namespace NM_BEHAVIOUR_LIB_NAMESPACE

