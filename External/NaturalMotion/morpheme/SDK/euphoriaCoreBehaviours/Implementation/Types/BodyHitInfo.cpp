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

#include "Types/BodyHitInfo.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

BodyHitInfo::BodyHitInfo()
{
  point.setToZero();
  normal.setToZero();
  hitDir.setToZero();
  limbRigIndex = -1;
  partIndex = -1;
  elapsedTime = 0.0f;
  hitCode = 0;
  actorId = 0;
}

bool BodyHitInfo::isInvalid()
{
  return actorId == 0;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

