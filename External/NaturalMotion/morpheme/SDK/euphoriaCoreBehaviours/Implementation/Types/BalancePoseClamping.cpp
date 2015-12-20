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

#include "Types/BalancePoseClamping.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

BalancePoseClamping::BalancePoseClamping()
{
  enablePoseClamping = false;
  dynamicClamping = false;
  minPelvisPitch = 0.0f;
  maxPelvisPitch = 0.0f;
  minPelvisRoll = 0.0f;
  maxPelvisRoll = 0.0f;
  minPelvisYaw = 0.0f;
  maxPelvisYaw = 0.0f;
  minPelvisHeight = 0.0f;
  maxPelvisHeight = 0.0f;
  pelvisPositionChangeTimescale = 0.0f;
  pelvisOrientationChangeTimescale = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

