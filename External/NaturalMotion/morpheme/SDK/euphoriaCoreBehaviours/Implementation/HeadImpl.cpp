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
#include "HeadPackaging.h"
#include "Head.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// HeadAPIBase
//----------------------------------------------------------------------------------------------------------------------
bool HeadAPIBase::canReachPointTarget(const NMP::Vector3& position, bool& canReach, float lookOutOfRangeWeight) const
{
  // returns if the head can reach to look at the specified position
  // it uses hysteresis to prevent flip-flopping
  const HeadLimbSharedState& headLimbState = owner->data->headLimbSharedStates[data->childIndex];
  NMP::Vector3 reachDirection = vNormalise(position - headLimbState.m_endTM.translation());
  float cosReachDirection = -reachDirection.dot(headLimbState.m_reachLimit.getReachConeDir());
  float maxBraceAngleFarSide = headLimbState.m_reachLimit.m_angle * (1.0f - lookOutOfRangeWeight);
  // this simply compares the outside angle to maxBraceFarSide, using hysteresis
  const float hysteresisAngleScale = 1.4f;
  if ((canReach && cosReachDirection < cosf(maxBraceAngleFarSide)) ||
      (!canReach && cosReachDirection < cosf(maxBraceAngleFarSide * hysteresisAngleScale)))
    canReach = true;
  else
    canReach = false;
  return canReach;
}

//----------------------------------------------------------------------------------------------------------------------
// HeadUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void HeadUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // hierarchically define stiffness parameters based on local data and on total stiffness of the
  // larger entity (the network)
  const float stiffnessMult = 1.0f - in->getLimbStiffnessReduction();
  const float dampingRatioMult = 1.0f - in->getLimbDampingRatioReduction();
  const float driveCompensationMult = 1.0f - in->getLimbControlCompensationReduction();
  data->normalStiffness = stiffnessMult * owner->data->normalStiffness;
  data->normalDampingRatio = dampingRatioMult * owner->data->normalDampingRatio;
  data->normalDriveCompensation = driveCompensationMult * owner->data->normalDriveCompensation;

#ifdef MR_OUTPUT_DEBUGGING
  const HeadLimbSharedState& headLimbState = owner->data->headLimbSharedStates[data->childIndex];
  MR_DEBUG_DRAW_CONE_AND_DIAL(
    pDebugDrawInst,
    headLimbState.m_reachLimit.m_transform.translation(),
    headLimbState.m_reachLimit.getReachConeDir(),
    headLimbState.m_reachLimit.m_angle,
    headLimbState.m_reachLimit.m_distance,
    (headLimbState.m_endTM.translation() - headLimbState.m_basePosition).getNormalised(),
    NMP::Colour::RED);
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, headLimbState.m_rootTM, SCALE_DIST(0.1f));
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, headLimbState.m_endTM, SCALE_DIST(0.1f));
#endif

}

//----------------------------------------------------------------------------------------------------------------------
// HeadFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void HeadFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep), 
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const HeadLimbSharedState& headLimbState = owner->data->headLimbSharedStates[data->childIndex];

  data->contactForceScalarSqr = headLimbState.m_endContact.lastForce.magnitudeSquared();
  data->headInContact = headLimbState.m_endContact.inContact;
  data->chestInContact = headLimbState.m_limbBarEndIsColliding;

  // (TODO** remove these following three - this may not be entirely trivial depending on what dependencies and
  // and processing is happening downstream)
  //
  // feedIn to feedOut pass through 
  feedOut->setCurrEndPartPosition(headLimbState.m_endTM.translation());
  // get length of limb for use by child modules
  data->length = owner->data->baseToEndLength[data->limbNetworkIndex];
  // calculate whether limb is colliding
  bool isColliding = headLimbState.m_mainPartIsColliding;
  feedOut->setInContact(isColliding);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, headLimbState.m_dynamicState.getPosition(), SCALE_DIST(0.5f), NMP::Colour::YELLOW);
}

//----------------------------------------------------------------------------------------------------------------------
void Head::entry()
{
  data->limbNetworkIndex = MyNetworkData::firstHeadNetworkIndex + getChildIndex();
  data->limbRigIndex = owner->data->firstHeadRigIndex + getChildIndex();
  data->dimensionalScaling = owner->data->dimensionalScaling;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

