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
#include "ArmSittingStep.h"
#include "ArmSittingStepPackaging.h"

#define SCALING_SOURCE pkg.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
static void debugDrawStepTarget(
  MR::InstanceDebugInterface* pDebugDrawInst,
  const ArmSittingStepUpdatePackage& pkg,
  const NMP::Matrix34& targetTM)
{
  const NMP::Vector3& groundNormal = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootContact.lastNormal;
  const NMP::Vector3& groundPosition = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootContact.lastPosition;

  // Draw target position.
  MR_DEBUG_DRAW_POINT(
    pDebugDrawInst,
    targetTM.translation(),
    SCALE_DIST(0.1f),
    NMP::Colour::DARK_RED);

  // Draw hand position, projected onto ground
  const NMP::Vector3 handPositionOnGround = groundPosition +
    (pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endLimbPartPosition
    - groundPosition).getComponentOrthogonalToDir(groundNormal);

  MR_DEBUG_DRAW_POINT(
    pDebugDrawInst,
    handPositionOnGround,
    SCALE_DIST(0.1f),
    NMP::Colour::DARK_ORANGE);
}
#endif // MR_OUTPUT_DEBUGGING


//----------------------------------------------------------------------------------------------------------------------
static NMP::Matrix34 calculateTargetHandTM(
  const ArmSittingStepUpdatePackage& pkg,
  const NMP::Vector3& armFacingDirection,
  const float armSupportWeight,
  const NMP::Vector3& leanNormal,
  const float leanAmount,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const ER::LimbTransform& chestTM = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM;
  const ER::LimbTransform& pelvisTM = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM;
  const NMP::Vector3& pelvisPosition = pelvisTM.translation();
  const NMP::Vector3& groundNormal = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootContact.lastNormal;
  const SitParameters& sitParameters = pkg.in->getSitParameters();

  // Find the offset from the pelvis position to its contact with the ground in direction of ground plane.
  const NMP::Vector3 pelvisPositionOnGround = pelvisPosition +
    groundNormal * (pkg.owner->owner->data->spineLimbSharedStates[0].m_rootContact.lastPosition
      - pelvisPosition).dot(groundNormal);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, pelvisPositionOnGround, 0.2f, NMP::Colour::DARK_TURQUOISE);

  // Find hand position relative to shoulder projected onto floor.
  NMP::Vector3 endRelativeToRoot = 
    (pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endLimbPartPosition - 
    pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_basePosition).getComponentOrthogonalToDir(groundNormal);

  // Find position of shoulder projected onto ground.
  NMP::Vector3 shoulderPositionOnGround = pelvisPositionOnGround +
    (pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_basePosition
    - pelvisPositionOnGround).getComponentOrthogonalToDir(groundNormal);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, pelvisPosition, pelvisPosition + endRelativeToRoot, NMP::Colour::DARK_PURPLE);

  // Calculate hand target position, being careful to avoid placing the target too near the pelvis.
  NMP::Vector3 targetHandPosition = shoulderPositionOnGround;

  const LimbHandleData& chestData  = pkg.owner->owner->data->spineLimbSharedStates[0].m_endData;
  const LimbHandleData& pelvisData = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootData;
  const float chestSpeedInLeanDirection = (chestData.velocity - pelvisData.velocity).dot(leanNormal);
  const float targetExtrapolationOffset =
    NMP::maximum( chestSpeedInLeanDirection * sitParameters.armStepTargetExtrapolationTime, 0.0f);

  const float pelvisAvoidanceRadius = pkg.owner->owner->data->selfAvoidanceRadius;

  NMP::Vector3 leanNormalInGroundPlane =
    (chestTM.translation() - pelvisPosition).getComponentOrthogonalToDir(groundNormal);
  leanNormalInGroundPlane.fastNormalise();
  const NMP::Vector3 targetDir = NMP::vLerp(armFacingDirection, leanNormalInGroundPlane, armSupportWeight);
  // Set hand target relative to shoulder or pelvis position, depending on whether the spine is leaning toward or
  // away from the current arm.
  if ((shoulderPositionOnGround - pelvisPositionOnGround).dot(targetDir) < pelvisAvoidanceRadius)
  {
    targetHandPosition = pelvisPositionOnGround + (targetDir * pelvisAvoidanceRadius);
  }

  targetHandPosition += targetDir *
    ((sitParameters.armStepTargetSeparationFromBody * (1.0f - (2.0f * leanAmount))) + targetExtrapolationOffset);

  // Calculate target hand transform.
  NMP::Vector3 pelvisToHandDir = targetHandPosition - pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation();
  pelvisToHandDir.fastNormalise();
  // Find target direction for fingers by interpolating between character forward and the line between the pelvis and
  // the hand.
  const float weight = NMP::clampValue(NMP::fastArccos(
    NMP::clampValue(armFacingDirection.dot(pelvisToHandDir), 0.0f, 1.0f)) * (4.0f / NM_PI), 0.0f, 1.0f);
  const NMP::Vector3 handForwardDir = 
    (weight * pelvisToHandDir) + ((1.0f - weight) * pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM.frontDirection());
  
  NMP::Matrix34 targetHandTM;
  targetHandTM.xAxis() = groundNormal;
  targetHandTM.yAxis() = NMRU::GeomUtils::calculateOrthogonalPlanarVector(groundNormal, handForwardDir);
  targetHandTM.yAxis().fastNormalise();
  targetHandTM.zAxis() = NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(groundNormal, handForwardDir);
  targetHandTM.zAxis().fastNormalise();
  targetHandTM.translation() = targetHandPosition;

  return targetHandTM;
}

//----------------------------------------------------------------------------------------------------------------------
#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling

void ArmSittingStepUpdatePackage::update(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const float supportImportance = feedIn->getSupportImportance();

  if (supportImportance <= 0.0f)
  {
    return;
  }

  const ER::LimbTransform& chestTM = owner->owner->data->spineLimbSharedStates[0].m_endTM;
  const ER::LimbTransform& pelvisTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
  const NMP::Vector3 worldUpDir = owner->owner->data->up;
  const float spineLength = owner->owner->data->defaultPoseLimbLengths[owner->owner->data->firstSpineRigIndex];

  // Find the direction in which the character's spine is leaning.
  NMP::Vector3 leanDirection =
    (chestTM.translation() - pelvisTM.translation()).getComponentOrthogonalToDir(worldUpDir);

  const float leanMagnitude = NMP::approxSqrt(leanDirection.magnitudeSquared());
  const NMP::Vector3 leanNormal = leanDirection / leanMagnitude;
  const float leanAmount = NMP::clampValue(leanMagnitude / spineLength, 0.0f, 1.0f);

  // Calculate arm support weight and importance.
  const ER::ReachLimit& reachLimit = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit;
  const float cosReachLimitAngle = cosf(reachLimit.m_angle);
  const float cosAngularSeparationBetweenLeanAndArm = leanNormal.dot(reachLimit.getReachConeDir());
  const float armSupportWeight = NMP::clampValue(
    ((cosAngularSeparationBetweenLeanAndArm - cosReachLimitAngle) / (1.01f - cosReachLimitAngle)),
    0.0f, 1.0f);

  const NMP::Vector3 groundNormal = owner->owner->data->spineLimbSharedStates[0].m_rootContact.lastNormal;

  // find direction from chest to base of arm - used to determine which side of the body the arm is on.
  NMP::Vector3 armFacingDirection = 
    (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition- chestTM.translation()).getComponentOrthogonalToDir(worldUpDir);
  armFacingDirection.fastNormalise();

  NMP::Matrix34 targetHandTM =
    calculateTargetHandTM(*this, armFacingDirection, armSupportWeight, leanNormal, leanAmount, pDebugDrawInst);

  // Blend with pose target position as support importance gets low
  const NMP::Matrix34 poseHandTM = in->getArmPoseEndRelativeToRoot()
      * owner->owner->data->spineLimbSharedStates[0].m_endTM;
  targetHandTM.interpolate(poseHandTM, 1.0f - supportImportance);

  // Measure error in hand position on ground.
  const NMP::Vector3 handTargetSeparation = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endLimbPartPosition
    - targetHandTM.translation();
  const float positionError =
    handTargetSeparation.getComponentOrthogonalToDir(groundNormal).magnitudeSquared() / SCALE_AREA(0.3f);

  // Measure error in hand normal
  const float orientationError = 1.0f - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.normalDirection().dot(targetHandTM.xAxis());

  float importance = 0.0f;

  const float counterWeightMaxArmSupportWeightToStart = 0.01f;
  const float counterWeightMinSupportImportance = 0.8f;

  if ((supportImportance > counterWeightMinSupportImportance) &&
      (armSupportWeight < counterWeightMaxArmSupportWeightToStart))
  {
    // Move hand away from body and ground in the opposite direction to the lean.
    const NMP::Vector3 counterBalanceTarget =
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition
      - (leanNormal * owner->data->length);
    targetHandTM.translation().lerp(counterBalanceTarget, supportImportance);

    importance = supportImportance;
  }
  else
  {
    if (positionError > 0.1f)
    {
      NMP::Vector3 targetOffset = groundNormal * NMP::minimum(positionError, in->getSitParameters().armStepHeight);
      targetHandTM.translation() += targetOffset;
    }
    else if (orientationError > 0.8f)
    {
      targetHandTM.translation()
        = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endLimbPartPosition
        + groundNormal * in->getSitParameters().armStepHeight;
    }

    const float minImportance = 0.1f;
    const float supportImportanceSq = NMP::sqr(supportImportance);
    importance = 4.0f * (supportImportanceSq - NMP::sqr(supportImportanceSq));
    importance = NMP::clampValue(importance + positionError, minImportance, 1.0f);
  }

#if defined(MR_OUTPUT_DEBUGGING)
  debugDrawStepTarget(pDebugDrawInst, *this, targetHandTM);
#endif // MR_OUTPUT_DEBUGGING

  LimbControl& control = out->startControlModification();
  control.reinit(
    owner->data->normalStiffness,
    owner->data->normalDampingRatio,
    owner->data->normalDriveCompensation);
  control.setTargetPos(targetHandTM.translation(), 1.0f);
  control.setTargetOrientation(targetHandTM, 1.0f);
  control.setSupportForceMultiplier(1.0f);
  control.setImplicitStiffness(16.0f);
  control.setTargetDeltaStep(0.5f); // IK sub stepping - should lift the hand off the ground when stepping.
  control.setGravityCompensation(1.0f);
  control.setControlAmount(ER::sitControl, 1.0f);
  out->stopControlModification(importance);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSittingStep::entry()
{}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

