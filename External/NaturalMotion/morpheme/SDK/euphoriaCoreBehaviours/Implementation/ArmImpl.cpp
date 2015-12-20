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

//----------------------------------------------------------------------------------------------------------------------
#include "MyNetworkPackaging.h"
#include "ArmPackaging.h"
#include "Arm.h"
#include "MyNetwork.h"
#include "euphoria/erDebugDraw.h"
#include "Helpers/LimbContact.h"
#include "Helpers/Reach.h"

#define SCALING_SOURCE data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ArmAPIBase
//----------------------------------------------------------------------------------------------------------------------

// Draw a square on the palm of the hand to indicate which way round it is
//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
static void drawTopSurface(const ArmFeedbackPackage& pkg, MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& armLimbState = pkg.owner->data->armLimbSharedStates[pkg.data->childIndex];
  const float size  = 0.05f;
  NMP::Vector3 pointDir = armLimbState.m_endTM.pointDirection() * size;
  NMP::Vector3 leftDir = armLimbState.m_endTM.leftDirection() * size;
  NMP::Vector3 c (1.0f, 1.0f, 1.0f, 1.0f);
  NMP::Vector3 corner = armLimbState.m_endTM.translation() - (pointDir / 2.0f) - (leftDir / 2.0f);

  // white outside patch
  MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, corner, corner + pointDir, corner + leftDir, NMP::Colour(c));
  MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, corner + pointDir, corner + leftDir, corner + leftDir + pointDir, NMP::Colour(c));

  // dark inside patch
  corner += 0.0001f * armLimbState.m_endTM.normalDirection().getNormalised();
  c = NMP::Vector3(0.0f, 0.0f, 0.0f, 1.0f);
  MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, corner, corner + pointDir, corner + leftDir, NMP::Colour(c));
  MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, corner + pointDir, corner + leftDir, corner + leftDir + pointDir, NMP::Colour(c));
}
#endif

// An adjustment that moves the placement down when placing behind the character
float ArmAPIBase::lowerTargetWhenBehindBack(
  const NMP::Vector3& worldTarget, 
  NMP::Vector3& desiredPosition, 
  const NMP::Vector3& shoulderOffset,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
  const ArmAndLegLimbSharedState& armLimbState = owner->data->armLimbSharedStates[data->childIndex];

  // Find out how far behind the character the target is
  NMP::Vector3 targetToChestDir = NMP::vNormalise(in->getCentreOfShoulders() - worldTarget);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, worldTarget, targetToChestDir, NMP::Colour::GREEN);
  float behindness = targetToChestDir.dot(armLimbState.m_rootTM.frontDirection());

  // Ramp up to full drop when mainly trying to place behind
  float drop = NMP::clampValue(0.5f + behindness * 2.0f, 0.0f, 1.0f);
  desiredPosition -= 
    drop * SCALE_DIST(0.35f) * (armLimbState.m_rootTM.upDirection() - 0.15f * armLimbState.m_rootTM.frontDirection());

  // If behind then should make arms aim wider apart.
  desiredPosition += shoulderOffset * drop * 2.0f;
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, desiredPosition, SCALE_DIST(0.05f), NMP::Colour::DARK_BLUE);
  return drop;
}

// This only checks against the angular reach limit, not the distance.
//----------------------------------------------------------------------------------------------------------------------
bool ArmAPIBase::canReachPoint(
  const NMP::Vector3& position, bool& canReach, float lookOutOfRangeWeight,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
  const ArmAndLegLimbSharedState& armLimbState = owner->data->armLimbSharedStates[data->childIndex];
  const ER::ReachLimit& reachLimit = armLimbState.m_reachLimit;

  // get angular proximity of position to reach cone centre
  NMP::Vector3 reachLimitTranslation = reachLimit.m_transform.translation();
  NMP::Vector3 reachDirection = NMP::vNormalise(position - reachLimitTranslation);

  float reachDirProj = reachDirection.dot(reachLimit.getReachConeDir());
  float reachAngle = NMP::fastArccos(reachDirProj);

  // if lookOutOfRangeWeight is high he can try and reach further because far side brace angle reduces
  float maxBraceAngleFarSide = reachLimit.m_angle * (1.0f - lookOutOfRangeWeight);

  // hysteresis prevents bouncing between two states, in this case when dot is close to cos(maxBraceAngleFarSide)
  const float hysteresisMax = 1.4f;
  if ((canReach  && (reachAngle < maxBraceAngleFarSide)) ||
      (!canReach && (reachAngle < (maxBraceAngleFarSide * hysteresisMax))))
  {
    canReach = true;
  }
  else
  {
    canReach = false;
  }

#if defined(MR_OUTPUT_DEBUGGING)
  if (pDebugDrawInst)
  {
    NMP::Colour colour = canReach ? NMP::Colour::GREEN : NMP::Colour::RED;
    reachLimit.draw(pDebugDrawInst, maxBraceAngleFarSide, colour); // draw with limits we're actually using
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, position, SCALE_DIST(0.15f), colour);
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, reachLimitTranslation, reachLimitTranslation + reachDirection, NMP::Colour::YELLOW);
  }
#endif

  return canReach;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 ArmAPIBase::avoidSpineUsingAngleCoefficient(
  const NMP::Vector3& target, 
  const NMP::Vector3& pelvisPos, 
  float slackAngle,
  MR::InstanceDebugInterface* pDebugDrawInst,
  float* angularOffset,
  NMP::Vector3 *adjustedApproach) const
{
  const ArmAndLegLimbSharedState& armLimbState = owner->data->armLimbSharedStates[data->childIndex];
  
  // Compile inputs for target angle adjustment
  //
  NMP::Vector3 result;
  const NMP::Vector3 effector = armLimbState.m_endTM.translation();              // Current effector position.
  const NMP::Vector3 reachOrg = armLimbState.m_rootTM.translation();             // Origin (e.g. shoulder or chest).
  const NMP::Vector3 reachUp  = armLimbState.m_rootTM.translation() - pelvisPos; // Up direction (eg. spine or chest up).
  const NMP::Vector3 reachForward = armLimbState.m_reachLimit.getReachConeDir(); // Forward direction
  // Results
  NMP::Vector3 targetApproach; // New approach vector after angle adjustment
  float angularError;                  // Total angle between effector and target

  // Swings the target about the torso.
  result = calcAngleLimitedTarget(
    target, effector, 
    owner->data->selfAvoidanceRadius, slackAngle,
    targetApproach, angularError,
    reachOrg, reachUp, reachForward, pDebugDrawInst, SCALING_SOURCE);

  // Feed back these results if required by caller.
  //
  if (angularOffset != NULL)
  {
    *angularOffset = angularError;
  }
  if (adjustedApproach != NULL)
  {
    *adjustedApproach = targetApproach;
  }

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, target, SCALE_DIST(0.07f), NMP::Colour::WHITE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, target, result - target, NMP::Colour::YELLOW);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, result, SCALE_DIST(0.07f), NMP::Colour::YELLOW);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// ArmUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // hierarchically define stiffness parameters based on local data and on total stiffness of the
  // larger entity (the network)
  const float stiffnessMult = 1.0f - in->getLimbStiffnessReduction();
  const float dampingRatioMult = 1.0f - in->getLimbDampingRatioReduction();
  const float driveCompensationMult = 1.0f - in->getLimbControlCompensationReduction();
  data->normalStiffness         = stiffnessMult * owner->data->normalStiffness;
  data->normalDampingRatio      = dampingRatioMult * owner->data->normalDampingRatio;
  data->normalDriveCompensation = driveCompensationMult * owner->data->normalDriveCompensation;

#if defined(MR_OUTPUT_DEBUGGING)
  const ArmAndLegLimbSharedState armLimbState = owner->data->armLimbSharedStates[data->childIndex];
  // Debug drawing
  NMP::Matrix34 endTM = armLimbState.m_endTM;
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endTM, SCALE_DIST(0.2f));

  NMP::Matrix34 rootTM = armLimbState.m_rootTM;
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, rootTM, SCALE_DIST(0.2f));

  NMP::Vector3 toEndDir = (armLimbState.m_endTM.translation() - armLimbState.m_basePosition).getNormalised();
  MR_DEBUG_DRAW_CONE_AND_DIAL(
    pDebugDrawInst,
    armLimbState.m_reachLimit.m_transform.translation(),
    armLimbState.m_reachLimit.getReachConeDir(),
    armLimbState.m_reachLimit.m_angle,
    armLimbState.m_reachLimit.m_distance,
    toEndDir,
    NMP::Colour::RED);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// ArmFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void ArmFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& armLimbState = owner->data->armLimbSharedStates[data->childIndex];
#if defined(MR_OUTPUT_DEBUGGING)
  drawTopSurface(*this, pDebugDrawInst);
#endif

  // **TODO follow this and remove downstream references 
  feedOut->setBasePosition(armLimbState.m_basePosition);

  data->length = owner->data->baseToEndLength[data->limbNetworkIndex];

  const bool isColliding = armLimbState.m_mainPartIsColliding;

  updateLimbCollidingTime(isColliding, timeStep, data->collidingTime);
  updateLimbEndContactFeedback(*this, false, owner->data->armLimbSharedStates[data->childIndex],
    timeStep, owner->data->collidingSupportTime, pDebugDrawInst);

  feedOut->setInContact(isColliding);
  feedOut->setEndOnGroundAmount(data->endOnGroundAmount);
  feedOut->setEndGroundCollidingTime(data->endGroundCollidingTime);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, armLimbState.m_dynamicState.getPosition(), SCALE_DIST(0.5f), NMP::Colour::YELLOW);
}

//----------------------------------------------------------------------------------------------------------------------
void Arm::entry()
{
  data->limbNetworkIndex = MyNetworkData::firstArmNetworkIndex + getChildIndex();
  data->limbRigIndex = owner->data->firstArmRigIndex + getChildIndex();
  data->childIndex = getChildIndex();
  data->dimensionalScaling = owner->data->dimensionalScaling;
  data->endOnGroundAmount = 0.0f;
  data->collidingTime = SCALE_TIME(-100000.0f);
  data->endGroundCollidingTime = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
