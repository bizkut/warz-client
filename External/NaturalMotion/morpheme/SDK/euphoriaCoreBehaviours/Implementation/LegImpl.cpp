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
#include "MyNetwork.h"
#include "LegPackaging.h"
#include "Leg.h"
#include "euphoria/erEuphoriaLogger.h"
#include "Helpers/LimbContact.h"

#define SCALING_SOURCE data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
bool LegAPIBase::canReachPoint(const NMP::Vector3& position, bool& canReach) const
{
  const ArmAndLegLimbSharedState& legLimbState = owner->data->legLimbSharedStates[data->childIndex];
  
  const ER::ReachLimit& reachLimit = legLimbState.m_reachLimit;
  NMP::Vector3 reachDirection = NMP::vNormalise(position - reachLimit.m_transform.translation());
  float reachDirProj = reachDirection.dot(legLimbState.m_reachLimit.getReachConeDir());
  // hysteresis
  const float hysteresisMax = 1.4f;
  float maxBraceAngle = canReach ? legLimbState.m_reachLimit.m_angle * hysteresisMax :
    legLimbState.m_reachLimit.m_angle;
  if (reachDirProj > cosf(maxBraceAngle))
  {
    canReach = true;
  }
  else
  {
    canReach = false;
  }
  return canReach;
}

//----------------------------------------------------------------------------------------------------------------------
void LegUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Hierarchically define stiffness parameters based on local data and on total stiffness of the
  // larger entity (the network)
  const float stiffnessMult = 1.0f - in->getLimbStiffnessReduction();
  const float dampingRatioMult = 1.0f - in->getLimbDampingRatioReduction();
  const float driveCompensationMult = 1.0f - in->getLimbControlCompensationReduction();
  data->normalStiffness = stiffnessMult * owner->data->normalStiffness;
  data->normalDampingRatio = dampingRatioMult * owner->data->normalDampingRatio;
  // Compensation used to make balance a bit worse so it might be worth checking if this is better
  // at an intermediate value like 0.5. However, pelvis control during stepping should be better
  // with it higher.
  data->normalDriveCompensation = driveCompensationMult * owner->data->normalDriveCompensation;

  // Set default (disabled) endConstraint
  EndConstraintControl constraint;
  constraint.initialise(SCALING_SOURCE);
  out->setEndConstraint(constraint, 0.0f);
#if defined(MR_OUTPUT_DEBUGGING)
  // Debug drawing
  const ArmAndLegLimbSharedState& legLimbState = owner->data->legLimbSharedStates[data->childIndex];
  NMP::Matrix34 tm = legLimbState.m_endTM;
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, tm, SCALE_DIST(0.2f));
  tm = legLimbState.m_rootTM;
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, tm, SCALE_DIST(0.2f));
  MR_DEBUG_DRAW_CONE_AND_DIAL(
    pDebugDrawInst,
    legLimbState.m_reachLimit.m_transform.translation(),
    legLimbState.m_reachLimit.getReachConeDir(),
    legLimbState.m_reachLimit.m_angle,
    legLimbState.m_reachLimit.m_distance,
    (legLimbState.m_endTM.translation() - legLimbState.m_basePosition).getNormalised(),
    NMP::Colour::RED);
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void LegFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& legLimbState = owner->data->legLimbSharedStates[data->childIndex];
  
  // Update and output the contact/colliding time feedouts
  //
  bool isColliding = legLimbState.m_mainPartIsColliding;
  // If we were previously supported externally (e.g. using partial/full body HK) then we probably
  // weren't receiving any collision info. That would mean on the first update that follows we
  // wouldn't know about the ground, and would do a one-frame swing, which looks terrible. It's
  // better to assume that we are on the ground, which at its worst will enable the balancer and
  // prevent immediate swing etc.
  if (data->previousExternalSupportAmount > 0.0f)
    isColliding = true;
  data->previousExternalSupportAmount = legLimbState.m_externalSupportAmount;;
  feedOut->setInContact(isColliding);
  updateLimbCollidingTime(isColliding, timeStep, data->collidingTime);
  feedOut->setCollidingTime(data->collidingTime);
  feedOut->setLimbBarEndIsColliding(legLimbState.m_limbBarEndIsColliding);
  
  updateLimbEndContactFeedback(
    *this,
    data->previousExternalSupportAmount > 0.0f,
    legLimbState,
    timeStep,
    owner->data->collidingSupportTime,
    pDebugDrawInst);

  // If we're not touching the ground then reduce the ground contact amount if we're moving away
  // from the ground
  if (data->endGroundCollidingTime <= 0.0f)
  {
    // Note that ideally we'd use the EA sweep info to find out what the real ground velocity is.
    // However, that's a huge overhead for very little benefit above just assuming it's stationary!
    // We need to reduce the on ground amount very quickly when we're flying away from the ground so
    // that things like fly-through-air kick in quickly. Reduce the on ground amount if we're off
    // the ground and going up so fast that gravity won't bring us back
    const float upwardsRootVelRelGround = legLimbState.m_rootData.velocity.dot(owner->data->up);
    // If the relative velocity is such that gravity would take longer than this to stop the
    // separation, then we would start to say we're not even near the floor
    const float speedForReducingOutput = 0.5f * owner->data->gravityMagnitude * owner->data->collidingSupportTime;
    const float speedForZeroOutput = owner->data->gravityMagnitude * owner->data->collidingSupportTime;
    if (speedForZeroOutput > speedForReducingOutput && upwardsRootVelRelGround > speedForReducingOutput)
    {
      float reduction = (upwardsRootVelRelGround - speedForReducingOutput) / (speedForZeroOutput - speedForReducingOutput);
      data->endOnGroundAmount = NMP::clampValue(data->endOnGroundAmount - reduction, 0.0f, 1.0f);
    }
  }

  feedOut->setEndOnGroundAmount(data->endOnGroundAmount);

  // Limb geometry
  data->length = owner->data->baseToEndLength[data->limbNetworkIndex];
  feedOut->setHipPosition(legLimbState.m_basePosition);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, legLimbState.m_dynamicState.getPosition(), SCALE_DIST(0.5f), NMP::Colour::YELLOW);
}

//----------------------------------------------------------------------------------------------------------------------
void Leg::entry()
{
  data->limbNetworkIndex = MyNetworkData::firstLegNetworkIndex + getChildIndex();
  data->limbRigIndex = owner->data->firstLegRigIndex + getChildIndex();
  data->childIndex = getChildIndex();
  data->dimensionalScaling = owner->data->dimensionalScaling;
  data->endOnGroundAmount = 0.0f;
  data->collidingTime = SCALE_TIME(-100000.0f);
  data->endGroundCollidingTime = 0.0f;
  data->previousExternalSupportAmount = 0.0f;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

