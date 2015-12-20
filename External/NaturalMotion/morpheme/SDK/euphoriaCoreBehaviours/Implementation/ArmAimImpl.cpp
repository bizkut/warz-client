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

//----------------------------------------------------------------------------------------------------------------------
#include "MyNetworkPackaging.h"
#include "MyNetwork.h"
#include "ArmAim.h"
#include "ArmAimPackaging.h"
#include "Helpers/Aim.h"
#include "NMGeomUtils/NMGeomUtils.h"

#define SCALING_SOURCE pkg.owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
template<> inline void aimEndTargetCorrection<ArmAimUpdatePackage>(
  ArmAimUpdatePackage& pkg,
  NMP::Matrix34& targetTM,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  float effectorToTargetAngleOffset = 0.0f;

  targetTM.translation() = pkg.owner->avoidSpineUsingAngleCoefficient(
    targetTM.translation(),
    pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation(),
    NM_PI / 6.0f,
    pDebugDrawInst,
    &effectorToTargetAngleOffset);

  // Ensure that the distance between the hand target and the shoulder position not greater than it is on the rig.
  const ArmAndLegLimbSharedState& armLimbState = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex];

  const NMP::Vector3 targetRelBasePosition = targetTM.translation() - armLimbState.m_basePosition;
  const float targetRelBaseSeparation      = targetRelBasePosition.magnitude();
  const float maxArmExtension              = armLimbState.m_reachLimit.m_distance;

  if (targetRelBaseSeparation > maxArmExtension)
  {
    // Move target position back along line from shoulder to hand s.t. it is within reach of the hand.
    targetTM.translation() -= targetRelBasePosition * (1.0f - (maxArmExtension / targetRelBaseSeparation));
  }

  // Limit angular separation between current and target end positions. 
  // 
  // The self avoidance code will move the hand target position along a curved path that avoids collisions with the
  // torso. However, the target moves much faster than the hand meaning that the hand often follows a straight line
  // rather than a curved path. This means that when the target position passes from the character's front to its back
  // the hand often goes under the arm and can get caught in an awkward position. The following tries to keep the target
  // position close to the hand following a curved arc of with radius of the reach length from the shoulder to prevent
  // the hand getting caught under the arms. This will encourage arm to remain straight when swinging around body.
  const NMP::Vector3 endRelBasePosition = armLimbState.m_endTM.translation() - armLimbState.m_basePosition;

  const float cosAngularSeparation =
    endRelBasePosition.dot(targetRelBasePosition) * 
    NMP::approxReciprocalSqrt(endRelBasePosition.magnitudeSquared() * targetRelBasePosition.magnitudeSquared());

  if (cosAngularSeparation < 0.0f)
  {
    // Angular separation must be between pi/2 and pi. Interpolate between current and target position in a way thats
    // proportional to 1 - cos(angular separation). This means that the further the target is from the current position,
    // the less influence it has over the final target.
    // 
    // note: Scaling the current position by 2 forces the arm to remain straight, which makes the character's pose look
    // better when transitioning between aim targets that are far appart.
    const float weight = NMP::clampValue(1.0f + (cosAngularSeparation * 0.5f), 0.0f, 1.0f);
    targetTM.translation().slerp(endRelBasePosition * 2.0f, targetRelBasePosition, weight);
    targetTM.translation() += armLimbState.m_basePosition;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmAimUpdatePackage::update(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& armLimbState = owner->owner->data->armLimbSharedStates[owner->data->childIndex];
  aimUpdate(*this, armLimbState, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmAim::entry()
{}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE