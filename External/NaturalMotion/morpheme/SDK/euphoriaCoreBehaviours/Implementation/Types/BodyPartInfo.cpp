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

#include "Types/BodyPartInfo.h"
#include "euphoria/erSharedEnums.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

BodyPartInfo::BodyPartInfo()
{
  limbRigIndex = -1;
  partIndex = -1;
  actorId = 0;
  type = ER::BodyPartTypeEnum::NumPartTypes;
}

bool BodyPartInfo::isInvalid()
{
  return actorId == 0;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

