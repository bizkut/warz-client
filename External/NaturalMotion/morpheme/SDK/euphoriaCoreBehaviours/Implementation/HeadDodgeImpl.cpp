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
#include "HeadDodgePackaging.h"
#include "HeadDodge.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erDebugDraw.h"
#include "types/DodgeHazard.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
inline void rampDownDodgeable(float& dodgeable, float timeStep, float period)
{
  // when there is no hazard, it is better to ramp down overall importance
  dodgeable = NMP::clampValue(dodgeable - timeStep / period, 0.0f, 1.0f);
}
//----------------------------------------------------------------------------------------------------------------------
// HeadDodgeUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void HeadDodgeUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Note there are two levels of ramping going on:
  // - if a hazard disappears then its data->dodgeable will ramp down over rampDownPeriod
  // - if a hazard is still there but has become unable to be dodged then its data->dodgeable will ramp down over
  //   dodgeableRampPeriod
  // dodgeable is unbounded so if hazard has been dodgeable for x seconds it will still try and dodge it for the next x
  // seconds. Basically it will dodge for dodgeableRampPeriod + amount of time it has been dodgeable, or rampDownPeriod
  // if hazard has gone

  float rampPeriod = in->getDodgeHazard().rampDownDurationScale;
  // if there is no information on this hazard anymore (it has gone!), then ramp down the avoid based on last known data
  if (in->getDodgeHazardImportance() == 0.0f)
  {
    rampDownDodgeable(data->dodgeable, timeStep, rampPeriod);
    if (data->dodgeable == 0.0f) // if there has been no hazard for rampDownPeriod then do nothing
    {
      return;
    }
  }
  else // otherwise update the brace with the latest hazard data
  {
    const DodgeHazard& hazard = in->getDodgeHazard();
    // To prevent the dodge flipping on/off suddenly we ramp up and down the dodgeable value
    const float dodgeableRampPeriod = SCALE_TIME(0.5f);
    float oldDodgeable = data->dodgeable;
    if (in->getDodgeHazard().dodgeableAmount > 0.5f) // if it is dodgeable then react quickly
    {
      // this isn't clamped which means that the longer the hazard is dodgeable, the longer before he'll stop trying to
      // dodge when it is signalled as not dodgeable... a form of hysteresis.
      // if hazard is flagged as dodgeable then ramp up, but from this running start value, as you want to react quite
      // quickly
      const float startAmount = 1.0f;
      data->dodgeable = NMP::maximum(startAmount, data->dodgeable + timeStep / dodgeableRampPeriod);
    }
    else // otherwise reduce dodge
    {
      data->dodgeable = NMP::maximum(0.0f, data->dodgeable - timeStep / dodgeableRampPeriod);
    }
    if (!data->dodgeable)
    {
      return;
    }
    float safeRadius = in->getDodgeHazard().radius;

    // Calculate the speed towards the head for early out and use in next block
    const float epsilon = 1e-10f;
    NMP::Vector3 headToHazardDir = hazard.position
      - owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation();
    const float distanceToTarget = headToHazardDir.normaliseGetLength() + epsilon;
    // Convert the absolute velocity into a relative velocity using the velocity of the head root.
    const NMP::Vector3 hazardSpeedRel = hazard.velocity
      - owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootData.velocity;
    const float towardsHeadSpeed = -headToHazardDir.dot(hazardSpeedRel);

    // the hazard isn't actually heading towards the head
    if (towardsHeadSpeed < 0.0f)
    {
      data->dodgeable = oldDodgeable;
      rampDownDodgeable(data->dodgeable, timeStep, rampPeriod);
      return;
    }

    // A heuristic is used here to calculate the imminence to pass into importance calculation
    float pain = in->getDodgeHazardImportance() * towardsHeadSpeed / SCALE_VEL(2.0f);
    float imminence = pain * towardsHeadSpeed / distanceToTarget;

    // importance calculated based on imminence and stiffness of limb
    data->importance = data->process.processRequest(
      NMP::minimum(data->dodgeable, 1.0f),
      imminence,
      owner->data->normalStiffness,
      1.0f, // Pass on everything
      owner->owner->data->minStiffnessForActing,
      owner->owner->data->maxStiffness);
    if (data->importance == 0.0f)
    {
      data->dodgeable = 0.0f;
      return; // too weak a problem to bother acting
    }
    // The up direction of the chest
    NMP::Vector3 up = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection();
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation(), 
      SCALE_DIST(up), NMP::Colour::WHITE);

    // here we find the height on body that the closest point intersects
    NMP::Vector3 dir = NMP::vNormalise(hazardSpeedRel);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
      MR::VT_Delta, owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      SCALE_DIST(dir), NMP::Colour::DARK_TURQUOISE);

    NMP::Vector3 midPos = (owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      + owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation()) / 2.0f;
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, midPos, safeRadius, NMP::Colour::YELLOW);
    // OK trying a different method here. 1. find closest point on path to midPos:
    NMP::Vector3 closest = hazard.position + dir * (midPos - hazard.position).dot(dir);
    float distance = (owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      - closest).magnitude();
    // 2. if closest is below then remove component in up direction
    float dot = (closest - midPos).dot(up);
    if (dot < 0.0f)
    {
      closest -= dot * up;
    }

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, hazard.position, closest - hazard.position, NMP::Colour::LIGHT_GREEN);

    // make head always pull forwards a bit
    closest -= owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.frontDirection()
      * SCALE_DIST(0.05f);
    data->headShift = midPos - closest;

    // calculate amount to shift the head in order to dodge the hazard
    data->headShift.normalise();
    distance = safeRadius - distance;
    data->headShift *= distance;
    if (distance <= 0.0f)
    {
      data->dodgeable = oldDodgeable;
      rampDownDodgeable(data->dodgeable, timeStep, rampPeriod);
      return;
    }
  }
  float dodgeImportance = NMP::minimum(data->dodgeable, 1.0f);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
    owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
    data->headShift, NMP::Colour::RED);

  // now update the control for the limb
  LimbControl& control = out->startControlModification();
  control.reinit(
    owner->data->normalStiffness * data->process.stiffnessScale,
    owner->data->normalDampingRatio,
    owner->data->normalDriveCompensation);
  control.setTargetPos(owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation() 
    + data->headShift, 2);
  control.setGravityCompensation(1.0f);
  control.setControlAmount(ER::dodgeControl, 1.0f);
  out->stopControlModification(NMP::clampValue(data->importance * dodgeImportance, 0.0f, 1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
// HeadDodgeFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void HeadDodgeFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  float importance = data->process.importance * NMP::minimum(data->dodgeable, 1.0f);
  if (importance != 0.0f)
  {
    // pass up the head shift as a request to the upper body

    // probably more stable than the actual current direction
    NMP::Vector3 chestToHeadDir = 
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection();
    // double the vertical request length to compensated for the lower importance despite the head
    // not being able to achieve motion in this direction
    NMP::Vector3 spineShift = data->headShift + chestToHeadDir * data->headShift.dot(chestToHeadDir) * 0.5f;
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
      spineShift, NMP::Colour(ER::getDefaultColourForControl(ER::dodgeControl)));
    TranslationRequest request(spineShift, SCALING_SOURCE);
    request.imminence = data->process.imminence;
    request.stiffnessScale = 2.0f;
#if defined(MR_OUTPUT_DEBUGGING)
    request.debugControlAmounts.setControlAmount(ER::dodgeControl, 1.0f);
#endif
    feedOut->setRootTranslationRequest(request, importance);

    // offset a head above chest
    NMP::Vector3 upOffset = chestToHeadDir * SCALE_DIST(0.5f);

    // generate rotation request for the chest, this should be half the rotation needed from the shift value,
    // since some is coming from the translation already
    RotationRequest oRequest(NMP::vCross(upOffset, spineShift) / upOffset.dot(upOffset), SCALING_SOURCE);
    oRequest.imminence = data->process.imminence;
    oRequest.stiffnessScale = 2.0f;
#if defined(MR_OUTPUT_DEBUGGING)
    oRequest.debugControlAmounts.setControlAmount(ER::dodgeControl, 1.0f);
#endif
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
      oRequest.rotation, NMP::Colour(ER::getDefaultColourForControl(ER::dodgeControl)));
    feedOut->setRootRotationRequest(oRequest, importance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodge::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

