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
#include "LegPackaging.h"
#include "LegSpinPackaging.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erPath.h"
#include "LegSpin.h"

#include "Helpers/Spin.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// LegSpinUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void LegSpinUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
#if defined(TEST_LEG_SPIN)
  // Will spin to keep pelvis upright, at imminence 2, should have approximately full importance but no more than normalStiffness
  RotationRequest& req = (RotationRequest&)in->getRotationRequest();
  NMP::Vector3 chestUp = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection();
  NMP::Quat quat;
  quat.forRotation(chestUp, owner->owner->data->up);
  req.rotation = quat.toRotationVector();
  req.imminence = 2.0f;
  req.stiffnessScale = 2.0f;
  float& imp = (float&)in->getRotationRequestImportanceRef();
  imp = 1.0f;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_AngleDelta, owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.translation(), req.rotation, NMP::Colour::ORANGE);
#endif

  float importance = in->getRotationRequestImportance();
  // scale down to make sure swing wins when using the same rotation request
  float spinAmount = 1.0f;
  if (in->getSpinAmountImportance() > 0.0f)
  {
    spinAmount = in->getSpinAmount();
  }
  importance *= NMP::clampValue(spinAmount, 0.0f, 0.99f); 

  bool shouldSpin = true;
  if (owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    shouldSpin = false;
  }
  // don't swing when in contact unless specifically required to
  if (!(in->getIgnoreContacts()) && owner->in->getIsBodyBarFeetContacting())
  {
    shouldSpin = false;
  }
  if (importance == 0.0f || owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    shouldSpin = false;
  }
  // Don't spin if spin amount is zero.
  if (spinAmount ==0.0f)
  {
    shouldSpin = false;
  }

  if (shouldSpin == false) // use this as key input
  {
    // Reset state data
    data->targetDirection.setToZero();
    data->targetTorque.setToZero();
    return;
  }
  const ER::LimbTransform& rootTM = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM;
  bool useParams = in->getSpinParamsImportance() > 0.0f;
  bool swap = false;
  NMP::Vector3 toHip = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition - rootTM.translation();
  bool isRight = toHip.dot(rootTM.rightDirection()) > 0.0f;
  // This coordinates the control between the limbs
  if (!useParams || in->getSpinParams().synchronised) // Default to synchronised
  {
    if (in->getCombinedTargetDirectionImportance())
      data->targetDirection = in->getCombinedTargetDirection();
    bool antiphase = true;
    if (antiphase && isRight)
    {
      swap = true;
      data->targetDirection = -data->targetDirection;
    }
  }

  // Get max parameters that limb is capable of achieving
  float radius = SCALE_DIST(0.35f);
  if (useParams)
  {
    radius = in->getSpinParams().maxRadius;
  }
  float angSpeed = SCALE_ANGVEL(9.0f);
  if (useParams)
  {
    angSpeed = in->getSpinParams().maxAngSpeed;
  }
  float stiffness = owner->owner->data->maxStiffness * in->getRotationRequest().stiffnessScale * spinAmount;
  float maxNormalTorque = radius * radius * angSpeed * stiffness; // Notice the correct units here, torque/mass

  // 1. Calculate torque required to get to requested rotation within requested time
  NMP::Vector3 targetNormalisedTorque = getRequiredNormalisedTorque(
    data->targetTorque, owner, in, owner->owner->data->legLimbSharedStates[owner->data->childIndex], timeStep,
    maxNormalTorque, pDebugDrawInst);

  NMP::Vector3 torqueDir = targetNormalisedTorque;
  float normalisedTorqueMag = torqueDir.normaliseGetLength();

  // Get blended radius
  float sidewaysTorque = fabsf(torqueDir.dot(rootTM.rightDirection()));
  // Radius is a parameter, but the drop in radius is hard coded here
  const float minorRadiusScale = 0.57f;
  radius *= (sidewaysTorque + minorRadiusScale * (1.0f - sidewaysTorque)); // lesser radius when pedalling in non-sideways axis

  // 2. Use totalTorqueMag to scale speed radius and stiffness down from their maximums
  scaleParametersProportionallyFromTorque(angSpeed, radius, stiffness, normalisedTorqueMag, owner);

  float radiusCutoff = SCALE_DIST(0.05f);
  if (radius < radiusCutoff)
    stiffness *= radius / radiusCutoff;

  // so set the target angular velocity
  NMP::Vector3 targetAngularVelocity = torqueDir * angSpeed;

  // 3. Now we have to move the internal target
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition, targetAngularVelocity * timeStep,
    NMP::Colour(NMP::Vector3(1, 0, 1, 1)));
  NMP::Vector3 oldTarget = data->targetDirection * radius;

  data->targetDirection += NMP::vCross(targetAngularVelocity * timeStep, data->targetDirection);
  data->targetDirection -= torqueDir * torqueDir.dot(data->targetDirection); // flatten in direction of delta
  data->targetDirection.fastNormalise();

  NMP::Vector3 target = data->targetDirection * radius;

  // And get blended spin centre
  float fwd = NMP::vCross(torqueDir, rootTM.rightDirection()).magnitude();
  float up = NMP::vCross(torqueDir, rootTM.upDirection()).magnitude();
  NMP::Vector3 centreLocal = NMP::Vector3(0.0f, up * radius - owner->data->length, fwd * (isRight ? radius : -radius));
  NMP::Vector3 spinCentre;
  // Convert it to a world space offset (from basePosition)
  rootTM.rotateVector(centreLocal, spinCentre);

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition, spinCentre,
    NMP::Colour(NMP::Vector3(1, 0, 1, 1)));
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition,
    SCALE_DIST(0.1f), NMP::Colour(1, 0, 0, 1));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition + spinCentre, oldTarget,
    NMP::Colour(NMP::Vector3(1, 1, 1, 1)));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition + spinCentre + oldTarget, target - oldTarget,
    NMP::Colour(NMP::Vector3(1, 1, 0, 1)));

  NMP::Vector3 worldTarget = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition
    + spinCentre + target;

  // 4. Work out control struct.
  LimbControl& control = out->startControlModification();
  control.reinit(stiffness, 0.5f * owner->data->normalDampingRatio, owner->data->normalDriveCompensation * 2.0f);
  control.setTargetPos(worldTarget, 1.0f);
  control.setControlAmount(ER::spinControl, 1.0f);
  out->stopControlModification(importance);

  if (swap)
  {
    data->targetDirection = -data->targetDirection;
  }
}
//----------------------------------------------------------------------------------------------------------------------
// LegSpinFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void LegSpinFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setTargetDirection(data->targetDirection, in->getRotationRequestImportance());
}

//----------------------------------------------------------------------------------------------------------------------
void LegSpin::entry()
{
}
}
