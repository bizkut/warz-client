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

#include "Types/PoseData.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

// Note really the right place to put this, but it seems silly making a source file just for a few lines...
PoseData::PoseData()
{
  poseEndRelativeToRoot.identity();
  poseEndRelativeToRootVelocity.setToZero();
  poseEndRelativeToRootAngularVelocity.setToZero();
  stiffnessScale = driveCompensationScale = 0.0f;
  dampingRatioScale = 1.0f;
  gravityCompensation = 1.0f;           // Assume that we really want to reach the pose
  strengthReductionTowardsEnd = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

