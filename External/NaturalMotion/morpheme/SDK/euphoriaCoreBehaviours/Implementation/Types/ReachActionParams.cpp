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

#include "Types/ReachActionParams.h"
#include "euphoria/erDimensionalScaling.h"
#include "euphoria/erSharedEnums.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

void ReachActionParams::setToDefaults(const ER::DimensionalScaling &scaling)
{
  outOfReachDistance = scaling.scaleDist(0.1f);       // to determine when effector is in some sense "close enough" to target
  outOfReachTimeout = scaling.scaleTime(1.0f);        // to determine how long to spend trying to get "close enough"
  withinReachTimeout = scaling.scaleTime(2.0f);       // to determine how long to spend at the target (i.e within "close enough" threshold)
  rampDownFailedDuration = scaling.scaleTime(1.0f);   // rampdown duration after not reaching target
  rampDownCompletedDuration = scaling.scaleTime(2.0f);// rampdown duration after reaching target
  swivelMode = ER::REACH_SWIVEL_NONE;                 // default to none and swivelAmount is just ignored
  swivelAmount = 0.0f;
  strength = 1.0f;
  penetrationAdjustment = scaling.scaleDist(0.0f);
  torsoRadiusMultiplier = 1.0f;
  handsCollisionGroupIndex = -1; // default to invalid group index
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

