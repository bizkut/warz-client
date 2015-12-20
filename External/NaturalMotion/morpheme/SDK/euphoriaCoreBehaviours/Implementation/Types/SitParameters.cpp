/*
 * Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
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

#include "Types/SitParameters.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

SitParameters::SitParameters()
{
  minSitAmount = 0.0f;
  minStandingBalanceAmount = 0.0f;
  armStepHeight = 0.0f;
  armStepTargetExtrapolationTime = 0.0f;
  armStepTargetSeparationFromBody = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

