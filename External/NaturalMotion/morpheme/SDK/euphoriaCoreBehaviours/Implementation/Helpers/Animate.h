#pragma once

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

#ifndef NM_ANIMATE_H
#define NM_ANIMATE_H

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

namespace Animate
{

//----------------------------------------------------------------------------------------------------------------------
inline void cachePose(
  PoseData& poseData,
  float& weight,
  const BodyPoseData& bp,
  const float driveCompensationScale,
  const float stiffnessScale,
  const uint32_t index)
{
  new (&poseData) PoseData;
  poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
  poseData.driveCompensationScale = driveCompensationScale;
  poseData.stiffnessScale = stiffnessScale;
  weight = bp.m_poseWeights[index];
}

//----------------------------------------------------------------------------------------------------------------------
// Note that if cached, then it must be a single pose so the velocity is zero
template<typename NetworkRoot>
inline void cacheLimbTargets(
  PoseData* cachedPose,
  float* cachedPoseImportance,
  const BodyPoseData& bp,
  const float driveCompensationScale,
  const float stiffnessScale,
  const NetworkRoot& networkRoot)
{
  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  uint32_t index = 0;
  for (uint32_t i = 0 ; i < networkRoot->data->numArms ; ++i, ++index)
    cachePose(cachedPose[index], cachedPoseImportance[index], bp, driveCompensationScale, stiffnessScale, index);

  index = NetworkConstants::networkMaxNumArms;
  for (uint32_t i = 0 ; i < networkRoot->data->numHeads ; ++i, ++index)
    cachePose(cachedPose[index], cachedPoseImportance[index], bp, driveCompensationScale, stiffnessScale, index);

  index = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
  for (uint32_t i = 0 ; i < networkRoot->data->numLegs ; ++i, ++index)
    cachePose(cachedPose[index], cachedPoseImportance[index], bp, driveCompensationScale, stiffnessScale, index);

  index =
    NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
  for (uint32_t i = 0 ; i < networkRoot->data->numSpines ; ++i, ++index)
    cachePose(cachedPose[index], cachedPoseImportance[index], bp, driveCompensationScale, stiffnessScale, index);
}

}

}

#endif