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
#include "ArmPlacementPackaging.h"
#include "ArmPlacement.h"
#include "ArmBracePackaging.h"
#include "ArmPackaging.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// Calculates target offset along target normal to come at target from the normal direction so as not to 'skim' target.
// Normal offset is zero if the target is behind the character.
//----------------------------------------------------------------------------------------------------------------------
static NMP::Vector3 getTargetNormalOffset(ArmPlacementUpdatePackage& pkg,
  const NMP::Vector3& targetPosition,
  float behindness,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  NMP::Vector3 normalOffset(NMP::Vector3::InitTypeZero);

  // Normal offset is zero if the target is behind the character.
  if (behindness > 0.0f)
  {
    return normalOffset;
  }

  // Get position and velocity relative to the object.
  const ER::HandFootTransform& endTM = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endTM;
  const NMP::Vector3 handPosRelTarget = endTM.translation() - targetPosition;

  const LimbHandleData& endData = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endData;
  const NMP::Vector3 handVelRelTarget = endData.velocity - pkg.in->getTarget().velocity;

  // Get relative position and relative velocity components orthogonal to the target normal direction.
  NMP::Vector3 handPosRelTargetOrthoToTargetNormal
    = handPosRelTarget.getComponentOrthogonalToDir(pkg.in->getTarget().normal);

  NMP::Vector3 handVelRelTargetOrthoToTargetNormal =
    handVelRelTarget.getComponentOrthogonalToDir(pkg.in->getTarget().normal);

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_Delta, targetPosition, handPosRelTargetOrthoToTargetNormal, NMP::Colour::DARK_GREY);

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_Velocity,
    targetPosition,
    pkg.owner->data->dimensionalScaling.scaleDist(handVelRelTargetOrthoToTargetNormal), // Please note the implicit
    // multiplication of the velocity by the integration time of 1 second that justifies scaleDist usage.
    NMP::Colour::LIGHT_GREY);

  // Calculate the various local values to use below.
  const float lateralDistance = handPosRelTargetOrthoToTargetNormal.magnitude();

  // Smallish distance gradient as most happens with velocity gradient.
  const float distanceGradient = 0.75f;

  // This pulls up if you're going fast towards or away from the object, i.e. only settles when you're matching the
  // target's speed laterally.
  const float lateralSpeed = handVelRelTargetOrthoToTargetNormal.magnitude();

  // This constant was calculated from 1/normalStiffness.
  const float pushPerSpeed = pkg.owner->data->dimensionalScaling.scaleTime(0.0625f); // Unit is time: [m / (m/s) = s]

  const float pushDistance = pkg.owner->data->dimensionalScaling.scaleDist(0.03f);

  // Calculate push based on lateral distance and lateral speed.
  // The unit of push is in meters: [m = m * m/m + m/s * s - m]
  const float pushUnclamped = lateralDistance * distanceGradient + lateralSpeed * pushPerSpeed - pushDistance;

  // Clamp push so is in sensible range.
  const NMP::Vector3& basePosition
    = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_basePosition;

  const float distanceToTarget = 1e-10f + (basePosition - targetPosition).magnitude();

  float push = NMP::clampValue(pushUnclamped, 0.0f, distanceToTarget * 0.6f) - pushDistance;

  // If hands aren't close to correct angle then lift off further.
  const float palmDot
    = (pkg.in->getTarget().normal).dot(endTM.normalDirection());

  if (palmDot < 0.0f)
  {
    push += -palmDot * pkg.owner->data->dimensionalScaling.scaleDist(0.2f);
  }

  normalOffset = pkg.in->getTarget().normal * push;

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_Delta, targetPosition, normalOffset, NMP::Colour::DARK_PURPLE);

  return normalOffset;
}

//----------------------------------------------------------------------------------------------------------------------
// Get the target position relative to the centre of shoulders.
// Get the component of that relative position orthogonal to character right direction and return as direction.
//----------------------------------------------------------------------------------------------------------------------
static NMP::Vector3 getTargetRelChestInSagitalPlane(ArmPlacementUpdatePackage& pkg,
  const NMP::Vector3& targetPosition,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const ER::LimbTransform& rootTM = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_rootTM;

  NMP::Vector3 result =
    (targetPosition - pkg.owner->in->getCentreOfShoulders()).getComponentOrthogonalToDir(rootTM.rightDirection());

  result.fastNormalise();

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    pkg.owner->in->getCentreOfShoulders(),
    pkg.owner->data->dimensionalScaling.scaleDist(result), NMP::Colour::LIGHT_YELLOW);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate weight for the reaching target normal.
// 1. Get the desired position relative to the centre of shoulders.
// 2. Get the component of that relative position orthogonal to character right direction.
// 3. Use this component to measure how high or low reaching target is relative to the chest position,
// ramp down normal weight when reaching target is behind the character and approaching horizontal.
//----------------------------------------------------------------------------------------------------------------------
static float getNormalWeight(ArmPlacementUpdatePackage& pkg,
  const NMP::Vector3& targetPosition,
  float behindness,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // If the target is not behind the character use full normal weight.
  if (behindness < 0.0f)
  {
    return 1.0f;
  }

  const NMP::Vector3 targetDir =
    getTargetRelChestInSagitalPlane(pkg, targetPosition, pDebugDrawInst);

  const ER::LimbTransform& rootTM = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_rootTM;

  const float normalWeight = NMP::nmfabs(targetDir.dot(rootTM.upDirection()));

  return NMP::minimum(normalWeight, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
// Ramp down importance when reaching target is behind the chest.
// This is to prevent the character from reaching where it simply can't.
//----------------------------------------------------------------------------------------------------------------------
static float getPlacementImportance(ArmPlacementUpdatePackage& pkg,
  const NMP::Vector3& targetPosition,
  float behindness,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // If the target is not behind the character use target importance.
  if (behindness < 0.0f)
  {
    return pkg.in->getTargetImportance();
  }

  const NMP::Vector3 targetDir =
    getTargetRelChestInSagitalPlane(pkg, targetPosition, pDebugDrawInst);

  const ER::LimbTransform& rootTM = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_rootTM;

  const float importance =
    NMP::minimum(1.0f - NMP::maximum(behindness, 0.0f),
    NMP::nmfabs(targetDir.dot(rootTM.upDirection())));

  return pkg.in->getTargetImportance() * NMP::minimum(importance, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
// ArmPlacementUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmPlacementUpdatePackage::update(float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // don't try and brace if hand is constrained
  if (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    return;
  }
  if (in->getTargetImportance() == 0.0f)
  {
    return;
  }
  // this shoulder offset is a rig independent way to offset each hand so they don't aim for the same position
  NMP::Vector3 shoulderOffset = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition
    - owner->in->getCentreOfShoulders();
  // clamp the shoulder offset to the plane normal, this is quite well behaved on erratic normals
  shoulderOffset -= in->getTarget().normal * in->getTarget().normal.dot(shoulderOffset);
  NMP::Vector3 targetPos = in->getTarget().position + shoulderOffset;

  // collect some data into local variables
  const float armLengthMaxScale = in->getMaxArmExtensionScaleImportance() ? in->getMaxArmExtensionScale() : 1.0f;
  float reachLength = owner->data->length;
  float distanceToTargetSqr = 1e-10f + (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition
    - targetPos).magnitudeSquared();

  // hysteresis based early-out if the target is out of reach
  const float extraScale = 1.1f;
  if (data->isInRange)
  {
    if (distanceToTargetSqr > NMP::sqr(reachLength * extraScale))
      return;
  }
  else
  {
    if (distanceToTargetSqr > NMP::sqr(reachLength / extraScale))
      return;
  }
  data->isInRange = true;

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, targetPos, SCALE_DIST(0.1f) * in->getTargetImportance(), NMP::Colour::GREEN);

  // calculate the compensation which affects the stiffness of the arm
  float compensationScale = 0.5f;
  float compensationScaledByImportance =
    compensationScale * owner->data->normalDriveCompensation * in->getTargetImportance();

  // initialise the arm control
  LimbControl& control = out->startControlModification();
  control.reinit(
    owner->data->normalStiffness,
    owner->data->normalDampingRatio,
    compensationScaledByImportance);

  // want arms to go limp as he gets looser
  control.setGravityCompensation(in->getTargetImportance());

  // Find out how far behind the character the target is.
  const NMP::Vector3 targetToChestDir = NMP::vNormalise(owner->in->getCentreOfShoulders() - targetPos);
  const float behindness =
    targetToChestDir.dot(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.frontDirection());

  // Aim to reach to the target but coming at it from the normal direction so as not to 'skim' it.
  NMP::Vector3 desiredPos = targetPos + getTargetNormalOffset(*this, targetPos, behindness, pDebugDrawInst);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst,
    desiredPos, SCALE_DIST(0.05f) * in->getTargetImportance(), NMP::Colour::TURQUOISE);

  // This piece of code attempts to make arm reach work better for backwards reaching, by pulling the arm out in a cone
  NMP::Vector3 coneDir = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.frontDirection();
  NMP::Vector3 centre
    = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation();
  NMP::Vector3 toDesired = desiredPos - centre;
  float desiredDot = toDesired.dot(coneDir);
  const float coneForwardsProjection = owner->data->length * 0.3f;
  const float coneGradient = 1.0f;
  if (desiredDot < coneForwardsProjection)
  {
    NMP::Vector3 lateralDir = toDesired - coneDir * NMP::vDot(coneDir, toDesired);
    float lateralDist = lateralDir.normaliseGetLength();
    float limitDist = -(desiredDot - coneForwardsProjection) * coneGradient;
    if (lateralDist < limitDist) // push out in direction lateral to cone direction
    {
      toDesired += lateralDir * (limitDist - lateralDist);
    }
  }
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, desiredPos, centre + toDesired - desiredPos, NMP::Colour::LIGHT_BLUE);
  // now clamp the object to be not too far away
  toDesired.clampMagnitude(owner->data->length * armLengthMaxScale);
  desiredPos = centre + toDesired;
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, desiredPos, SCALE_DIST(0.02f), NMP::Colour::LIGHT_BLUE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, centre + coneDir * coneForwardsProjection, -coneDir, NMP::Colour::YELLOW);

  // update the limb control structure accordingly
  control.setTargetPos(desiredPos, 1.0f, in->getTarget().velocity);
  control.setTargetNormal(in->getTarget().normal, getNormalWeight(*this, desiredPos, behindness, pDebugDrawInst));
  out->setWristLimitReduction(0.99f * NMP::maximum(behindness, 0.0f)); // no rotation in wrist
  control.setEndSupportAmount(0.0f);
  control.setImplicitStiffness(1.0f); // with placement you know about the object I think, its a sort of support
  control.setControlAmount(ER::placementControl, 1.0f);

  if (in->getSwivelAmountImportance() > 0.0f)
  {
    control.setSwivelAmount(in->getSwivelAmount());
  }

  out->stopControlModification(getPlacementImportance(*this, desiredPos, behindness, pDebugDrawInst));
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPlacement::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
