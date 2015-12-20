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

#ifndef NM_STANDING_SUPPORT_INL
#define NM_STANDING_SUPPORT_INL

#include "euphoria/erDebugDraw.h"

// These are defined in LegSupportImpl.cpp so can be shared between leg and arm support
extern float g_supportForceMultipler;
extern float g_supportImplicitStiffness;

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
#error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// This takes the foot targetTM and modifies it so that if the foot endTM is already tilting more
// than allowedTiltMin in a certain direction, the returned TM will have a much restricted tilt in
// that direction (whilst still allowing tilt in other directions). This is to stop tilting the foot
// forwards, for example, if the foot is already tilting forwards, since it's not going to help
// supporting that character. The effect kicks in gradually between allowedTiltMin and
// allowedTiltMax (in radians).
//----------------------------------------------------------------------------------------------------------------------
static NMP::Matrix34 getPlantedTargetTM(
  NMP::Matrix34& targetTM,
  const NMP::Matrix34& endTM,
  const NMP::Matrix34& floorTM,
  float allowedTiltMin, float allowedTiltMax)
{
  NMP_ASSERT(targetTM.isValidTM(0.05f));
  NMP_ASSERT(endTM.isValidTM(0.05f));
  NMP_ASSERT(floorTM.isValidTM(0.05f));

  // work in the floor TM reference frame
  NMP::Matrix34 floorTMInverse = floorTM;
  floorTMInverse.invertFast();
  // end transform in the frame of the floor (rotation only)
  NMP::Matrix34 endTMLocal = endTM * floorTMInverse;
  NMP::Vector3 endTMLocalRot = endTMLocal.toRotationVector();
  // desired end transform in the frame of the floor
  NMP::Matrix34 targetTMLocal = targetTM * floorTMInverse;
  NMP::Vector3 targetTMLocalRot = targetTMLocal.toRotationVector();

  if (endTMLocalRot.magnitude() > 0.0f)
  {
    NMP::Vector3 endTMLocalRotDir = endTMLocalRot;
    float endAngle = endTMLocalRotDir.normaliseGetLength();
    if (endAngle > allowedTiltMin)
    {
      float d = targetTMLocalRot.dot(endTMLocalRotDir);
      // fraction that the current tilt is between min and max
      float frac = NMP::clampValue((endAngle - allowedTiltMin) / (allowedTiltMax - allowedTiltMin), 0.0f, 1.0f);
      float targetTilt = frac * allowedTiltMax + (1.0f - frac) * d;
      float extra = d - targetTilt;
      if (extra > 0.0f)
        targetTMLocalRot -= extra * endTMLocalRotDir;
    }
  }
  targetTMLocal.fromRotationVector(targetTMLocalRot);
  NMP::Matrix34 plantedTM = targetTMLocal * floorTM;
  return plantedTM;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename OwnerType, typename InType, typename OutType, typename DataType>
inline void updateStandingSupport(
  OwnerType& owner,
  InType& in, OutType& out, DataType& data, const ArmAndLegLimbSharedState& limbState,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (!limbState.m_isConstrained && in->getEnableExternalSupportImportance() > 0.0f &&
    !in->getEnableExternalSupport())
  {
    return;
  }

  // Always apply at least just a tiny support amount (which may be overridden) so that legs don't
  // go completely floppy just because they've come off the ground.
  float endOnGroundAmount = NMP::clampValue(owner->data->endOnGroundAmount, 0.05f, 1.0f);

  if (owner->data->endGroundCollidingTime < -SCALE_TIME(1.0f))
  {
    data->lostContactWithGround = true;
  }

  if (in->getRootDesiredTMImportance() > 0.0f)
  {
    NMP::Matrix34 rootDesiredTM = in->getRootDesiredTM();
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, rootDesiredTM, SCALE_DIST(1.0f));

    const ER::LimbTransform& rootTM = limbState.m_rootTM;
    const ER::HandFootTransform& endTM = limbState.m_endTM;

    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Normal,
      limbState.m_endContact.lastPosition,
      limbState.m_endContact.lastNormal,
      NMP::Colour::LIGHT_PURPLE);

    // Rather than use the actual foot transform, work out where it would be if it were really
    // planted on the floor. This defines a coordinate frame on (perhaps floating above) the floor,
    // aligned along the length of the foot.
    NMP::Vector3 floorNormal;
    if (limbState.m_endContact.inContact)
    {
      floorNormal = limbState.m_endContact.lastNormal;
      // Don't use the actual floor position because this tends to be below the feet when they tilt,
      // and that just makes the tilting problem worse!
    }
    else
    {
      floorNormal = owner->owner->data->up;
    }
    ER::HandFootTransform floorTM;
    floorTM.createFromNormalAlignedByPoint(floorNormal, endTM.pointDirection(), endTM.translation());

    // Really just want to do "root" control, keeping the foot where it is. However, not convinced that works
    // too well - so convert to doing "end" control. Another reason for doing this is to avoid tilting our foot
    // further than necessary - i.e. if it's already tilted in a direction off the floor, don't tilt
    // any more in that direction.

    NMP::Matrix34 endDesiredTM;
    float strengthScale = 1.0f;
    float importance = 1.0f;
    float supportForceMultiplier = g_supportForceMultipler;
    float implicitStiffness = g_supportImplicitStiffness;

    // We know that our root is the same as the spine root
    NMP::Matrix34 rootDesiredTMInverse(rootDesiredTM);
    rootDesiredTMInverse.invert();
    endDesiredTM = floorTM * rootDesiredTMInverse * rootTM;

    if (in->getSupportStrengthScaleImportance() > 0.0f)
    {
      strengthScale *= in->getSupportStrengthScale();
    }

    // Prevent the target pushing a tilted foot even further. Switching this feature/ability off is
    // useful when debugging.
#if 1
    // When strengthScale is greater allow bigger foot deviations - seems to be needed for one-foot
    // balancing.
    endDesiredTM = getPlantedTargetTM(
      endDesiredTM, endTM, floorTM,
      SCALE_ANG(NMP::degreesToRadians(2.0f) * strengthScale),
      SCALE_ANG(NMP::degreesToRadians(5.0f) * strengthScale));
#else
    (void) &getPlantedTargetTM;
#endif
    // We might also want to make it so that rather than pushing down with the toes, we pull up with
    // the heels (for example). However, initial experiments indicate this doesn't really work, so
    // don't do it. Leaving code in disabled for now in case we want to try again (and as a reminded
    // that in its basic form it doesn't work!).

    // Note that we only want to support - we don't want to press down so hard that we leap up. So,
    // limit the downwards target position based on the root motion.
#if 0
    float pelvisUpSpeed = limbState.m_root.velocity.dot(owner->owner->data->up);
    float maxUpSpeed = SCALE_VEL(2.0f);
    float limitFrac = NMP::clampValue(pelvisUpSpeed / maxUpSpeed, 0.0f, 1.0f);
    if (endDesiredTM.translation().dot(owner->owner->data->up) < endTM.translation().dot(owner->owner->data->up))
    {
      endDesiredTM.translation() +=
        owner->owner->data->up * ((endTM.translation().dot(owner->owner->data->up) -
        endDesiredTM.translation().dot(owner->owner->data->up)) * limitFrac);
    }
#endif

    // If the feet lift off the ground then just move them back to the balance pose ready to support again
    if (endOnGroundAmount < 1.0f)
    {
      if (in->getBalancePoseEndRelativeToRootImportance() > 0.0f)
      {
        importance = endOnGroundAmount;
        endDesiredTM = in->getBalancePoseEndRelativeToRoot() * rootTM;
      }
      else
      {
        importance = 0.0f;
      }
    }

    // Reduce limb strength if end has lost contact with ground.
    if (data->lostContactWithGround)
    {
      implicitStiffness = owner->data->endGroundCollidingTime / SCALE_TIME(1.0f);
      if (implicitStiffness < 0.0f)
      {
        implicitStiffness = 0.0f;
      }
      else if (implicitStiffness > 1.0f)
      {
        implicitStiffness = 1.0f;
        data->lostContactWithGround = false;
      }

      implicitStiffness *= g_supportImplicitStiffness;
      // 1 is the default SFM anyway - it kicks in when the limb claims to be supporting
      supportForceMultiplier = g_supportForceMultipler * implicitStiffness;
    }

    // Could multiply this by strengthScale, but that multiplies up the strengthScale effect too
    // much. GC is already limited by the stiffness. Multiplying by strengthScale results in an odd
    // condition where as soon as the balanceWeakness > 0.5 he loses almost all strength. However,
    // we need to multiply since if we don't we end up in the situation where we output zero
    // strength with non-zero importance!
    float weight = NMP::clampValue(in->getRootDesiredTMImportance() * strengthScale, 0.0f, 1.0f);
    if (weight == 0.0f)
      return;

    LimbControl& control = out->startControlModification();

    float stiffness = NMP::clampValue(endOnGroundAmount * owner->data->normalStiffness * strengthScale,
      0.0f, owner->owner->data->maxStiffness);

    // Note that the drive compensation is set to 0 because otherwise the "floaty" bug occurs when
    // we run with small timesteps, leading to the stepping leg getting pushed out as he steps, and
    // balance becomes pretty bad.
    control.reinit(
      stiffness,
      owner->data->normalDampingRatio,
      owner->data->normalDriveCompensation * 0.0f);
    control.setTargetPos(endDesiredTM.translation(), importance);
    control.setTargetOrientation(endDesiredTM, importance);
    // used to be proportional to weight, but GC was mainly boolean anyway, so this is v close to the same
    control.setGravityCompensation(1.0f);
    control.setEndSupportAmount(endOnGroundAmount);

    control.setSupportForceMultiplier(supportForceMultiplier);
    control.setImplicitStiffness(implicitStiffness);

    control.setControlAmount(ER::standingSupportControl, 1.0f);

    if (in->getStabiliseRootAmount() > 0.0f)
    {
      NMP::Quat pelvisOrientation = rootTM.toQuat();
      NMP::Vector3 pelvisAngularVelocity = limbState.m_rootData.angularVelocity;
      NMP::Vector3 targetPelvisAngularVelocity = -pelvisAngularVelocity * in->getStabiliseRootAmount();
      control.setExpectedRootOrientationForTarget(pelvisOrientation, 1.0f, targetPelvisAngularVelocity);
    }

    out->stopControlModification(weight);
    out->setUseFullEndJointRange(true, weight);

    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endTM, SCALE_DIST(0.5f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endDesiredTM, SCALE_DIST(0.25f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, floorTM, SCALE_DIST(0.1f));
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, endTM.translation(), endDesiredTM.translation(), NMP::Colour::TURQUOISE);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<typename OwnerType, typename InType, typename DataType, typename FeedOutType>
inline void feedbackStandingSupport(
  OwnerType& owner,
  InType& in,
  DataType& data,
  const ArmAndLegLimbSharedState& limbState,
  FeedOutType& feedOut,
  float &controlAmount)
{
  if (!limbState.m_isConstrained &&
    in->getEnableExternalSupportImportance() > 0.0f && !in->getEnableExternalSupport())
  {
    return;
  }

  float constrainedAmount = (float) limbState.m_isConstrained;
  float endFixedAmount = NMP::maximum(owner->data->endOnGroundAmount, constrainedAmount);
  endFixedAmount = NMP::maximum(endFixedAmount, limbState.m_externalSupportAmount);
  float supportingAmount = endFixedAmount;

  if (limbState.m_externalSupportAmount == 0.0f && !limbState.m_isConstrained)
  {
    supportingAmount *= in->getRootDesiredTMImportance();
    if (in->getSupportStrengthScaleImportance() > 0.0f)
      supportingAmount *= in->getSupportStrengthScale();
    supportingAmount = NMP::clampValue(supportingAmount, 0.0f, 1.0f);

    if (!limbState.m_endContact.inContact)
    {
      float lowerRootDistance = in->getBalanceParameters().lowerPelvisDistanceWhenFootLifts *
        (1.0f - owner->data->endOnGroundAmount) *
        in->getRootDesiredTMImportance();
      feedOut->setLowerRootDistance(lowerRootDistance);
    }
  }

  // On the first feedback pretend that we were asked to support! This should stop other modules
  // kicking in like freefall, grab etc. Also, if we were previously supported externally (e.g.
  // using partial/full body HK) then we probably weren't receiving any collision info. That would
  // mean on the first update that follows we wouldn't know about the ground, and would do a
  // one-frame swing, which looks terrible. It's better to assume that we are on the ground, which
  // at its worst will enable the balancer and prevent immediate swing etc.
  if (!data->hasFeedbackRun)
  {
    data->hasFeedbackRun = true;
    supportingAmount = 1.0f;
  }
  if (limbState.m_externalSupportAmount > 0.0f)
    data->hasFeedbackRun = false;
  supportingAmount = NMP::clampValue(supportingAmount, 0.0f, 1.0f);

  controlAmount = supportingAmount; // default, e.g. for standing there is usually full control of pelvis
  if (limbState.m_isConstrained) // for hanging
  {
    // since control amounts are purposely summed in the body section this means that more limbs
    // controlling will provide a larger control amount currently the 0.25 roughly represents the
    // arm's stiffness but I should make it scale with the chosen arm stiffness.
    controlAmount *= 0.25f;
  }
  feedOut->setSupportAmount(supportingAmount);

  if (in->getEnableExternalSupport() &&
    (limbState.m_externalSupportAmount > 0.0f || limbState.m_isConstrained))
  {
    feedOut->setSupportedByConstraint(true);
  }

  // The foot support point can be used by things like the support polygon
  float imp = NMP::maximum(supportingAmount, endFixedAmount);
  feedOut->setEndSupportPoint(limbState.m_endTM.translation(), imp);
}

}

#endif
