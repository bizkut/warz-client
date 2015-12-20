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
#include "HeadPackaging.h"
#include "HeadAvoidPackaging.h"
#include "HeadAvoid.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erDebugDraw.h"
#include "types/SpatialTarget.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
inline void rampDownImportance(float& importance, float timeStep, float period)
{
  importance = NMP::maximum(0.0f, importance - timeStep / period);
}
//----------------------------------------------------------------------------------------------------------------------
// HeadAvoidUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void HeadAvoidUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // this module is very simple, it just tries to keep the head away from the target.

  // The importance is a smoothed acceleration, but we want a little more smoothing on top
  // but only in ramping down, we want to be responsive to upward ramping
  float oldImportance = data->importance;
  data->importance = in->getAvoidTargetImportance();
  // smoothing down
  if (data->importance <= oldImportance)
  {
    float rampTime = SCALE_TIME(1.0f); // quite slow
    NMP::smoothExponential(data->importance, timeStep, oldImportance, rampTime);
    if (data->importance < 0.05f)
      return;
  }
  const SpatialTarget& hazard = in->getAvoidTarget();
  float defaultAvoidRadius = SCALE_DIST(0.5f);
  data->safeRadius = in->getAvoidRadiusImportance() ? in->getAvoidRadius() : defaultAvoidRadius;

  // calculate distance to hazard
  float hazardHeadSeparation
    = (owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      - hazard.position).dot(hazard.normal);

  // reduce importance with distance to edge of radius
  float distanceToEdge = data->safeRadius - hazardHeadSeparation;

  // calculate importance of avoiding based on hazardousness and the distance to the hazard
  float importanceUnclamped = 4.0f * distanceToEdge * in->getAvoidTargetImportance() / data->safeRadius;
  data->importance = NMP::clampValue(importanceUnclamped, 0.0f, 1.0f);
  data->headShift = hazard.normal * distanceToEdge;

  MR_DEBUG_DRAW_POINT(pDebugDrawInst,
    owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
    in->getAvoidTargetImportance()*SCALE_DIST(0.1f), NMP::Colour::GREEN);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst,
    owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
    data->importance * SCALE_DIST(0.1f), NMP::Colour::RED);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_Delta, owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
    data->headShift, NMP::Colour::GREEN);

  // update the limb control based on the calculated data
  LimbControl& control = out->startControlModification();
  control.reinit(owner->data->normalStiffness, owner->data->normalDampingRatio, owner->data->normalDriveCompensation);
  control.setTargetPos(owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation() + data->headShift, 1);
  control.setControlAmount(ER::avoidControl, 1.0f);
  control.setGravityCompensation(1.0f);
  if (owner->in->getUpperBodyInContact())
    control.setRootSupportedAmount(1.0f);
  out->stopControlModification(data->importance);
}

//----------------------------------------------------------------------------------------------------------------------
// HeadAvoidFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void HeadAvoidFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep), 
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (data->importance == 0.0f)
  {
    return;
  }
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM,
    SCALE_DIST(1.0f));

  NMP::Vector3 headShift = data->headShift;
  if (owner->in->getChestControlledAmount() > 0.0f)
  {
    // This manual damping seems crucial to avoid oscillations when standing due to higher latency
    // in motions going down the whole body particularly necessary for larger creatures for some
    // reason. This is internal damping, so not related to the velocity of the hazard actually If
    // balanced then we assume floor is fixed here, and use an absolute velocity would need extra
    // code to make headAvoid work on a reasonably fast moving floor
    NMP::Vector3 relVel = -owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootData.velocity;
    relVel.clampMagnitude(SCALE_VEL(4.0f)); // don't want to take this damping too far
    // generate translation request for the chest
    const float dampingAmount = SCALE_FREQUENCY(0.2f); // larger is more damping but will become unstable if too high
    headShift += relVel * dampingAmount;
  }

  TranslationRequest request(headShift, SCALING_SOURCE);
#if defined(MR_OUTPUT_DEBUGGING)
  request.debugControlAmounts.setControlAmount(ER::avoidControl, 1.0f);
#endif
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
    owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation(), request.translation, 
    NMP::Colour(ER::getDefaultColourForControl(ER::avoidControl)));
  feedOut->setRootTranslationRequest(request, data->importance * 0.5f);

  // offset a head above chest
  NMP::Vector3 upOffset = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection() * SCALE_DIST(0.5f);

  // generate rotation request for the chest
  RotationRequest oRequest(NMP::vCross(upOffset, headShift) / upOffset.dot(upOffset), SCALING_SOURCE);
#if defined(MR_OUTPUT_DEBUGGING)
  oRequest.debugControlAmounts.setControlAmount(ER::avoidControl, 1.0f);
#endif
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
    owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation(), oRequest.rotation
    , NMP::Colour(ER::getDefaultColourForControl(ER::avoidControl)));
  feedOut->setRootRotationRequest(oRequest, data->importance * 0.5f);

}

//----------------------------------------------------------------------------------------------------------------------
void HeadAvoid::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

