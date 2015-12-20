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

#include "Types/ReachWorldParameters.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erDimensionalScaling.h"
#include "euphoria/erSharedEnums.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

void ReachWorldParameters::setToDefaults(const ER::DimensionalScaling &scaling)
{
  // By default, none of the optional features are used.
  controlColour.setToZero();
  swivelMode = ER::REACH_SWIVEL_NONE;
  swivelAmount = 0.0f;
  unreachableTargetImportanceScale = 1.0f;
  torsoRadiusMultiplier = 1.0f;
  slideAlongNormal = false;
  chestRotationScale = 0.0f;
  pelvisRotationScale = 0.0f;
  maxChestTranslation = 0.0f;
  maxPelvisTranslation = 0.0f;
  selfAvoidanceStrategy = disable;
  ikSubstepSize = 1.0f;
  avoidLegsArea = false;
  maxReachScale = 1.0f;
  reachImminence = scaling.scaleImminence(1.0f);
  selfAvoidanceSlackAngle = NM_PI / 60.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

