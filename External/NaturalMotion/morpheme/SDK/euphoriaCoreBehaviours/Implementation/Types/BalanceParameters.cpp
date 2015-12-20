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

#include "Types/BalanceParameters.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

BalanceParameters::BalanceParameters()
{
  memset(&balanceWeaknessPerLeg[0], 0, sizeof(balanceWeaknessPerLeg));
  memset(&balanceWeaknessPerArm[0], 0, sizeof(balanceWeaknessPerArm));
  for (size_t i = 0 ; i < NetworkConstants::networkMaxNumLegs ; ++i)
    supportPointWeightingPerLeg[i] = 1.0f;
  for (size_t i = 0 ; i < NetworkConstants::networkMaxNumArms ; ++i)
    supportPointWeightingPerArm[i] = 1.0f;
  footBalanceAmount = 0.0f;
  decelerationAmount = 0.0f;
  maxAngle = 0.0f;
  footLength = 0.0f;
  spinAmount = 0.0f;
  spinThreshold = 0.25f;
  lowerPelvisDistanceWhenFootLifts = 0.0f;
  steppingDirectionThreshold = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

