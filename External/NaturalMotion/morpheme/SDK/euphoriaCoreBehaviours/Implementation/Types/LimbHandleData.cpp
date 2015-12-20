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

#include "Types/LimbHandleData.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
LimbHandleData::LimbHandleData() :
  velocity(NMP::Vector3::InitZero),
  angularVelocity(NMP::Vector3::InitZero),
  lastCollisionID(0),
  isCollidingWithWorld(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
LimbHandleData::LimbHandleData(
  const NMP::Vector3& vel,
  const NMP::Vector3& angVel,
  bool colliding,
  int64_t collisionId) :
  velocity(vel),
  angularVelocity(angVel),
  lastCollisionID(collisionId),
  isCollidingWithWorld(colliding)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
