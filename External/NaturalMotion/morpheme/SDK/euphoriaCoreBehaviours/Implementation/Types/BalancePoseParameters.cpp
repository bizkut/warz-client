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

#include "Types/BalancePoseParameters.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

BalancePoseParameters::BalancePoseParameters()
{
  yaw = 0.0f;
  pitch = 0.0f;
  roll = 0.0f;
  pelvisMultiplier = 0.0f;
  spineMultiplier = 0.0f;
  headMultiplier = 0.0f;
  crouchAmount = 0.0f;
  chestTranslation.setToZero();
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

