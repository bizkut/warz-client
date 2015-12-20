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

#include "Types/ActorImpulse.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

ActorImpulse::ActorImpulse()
{
  useVelChange = false;
  positionFrameWorld = false;
  point.setToZero();
  hitDir.setToZero();
  impulseMagnitude = 0.0f;
  torqueMultiplier = 1.0f;
  limbIndex  = 0;
  partIndex  = 0;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

