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
#include "ArmSpinPackaging.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erPath.h"
#include "ArmSpin.h"

#include "Helpers/Spin.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// ArmSpinUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmSpinUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
#if defined(TEST_ARMSPIN)
  // Will spin to keep chest upright, at imminence 2, should have approximately full importance but no more 
  //than normalStiffness
  RotationRequest& req = (RotationRequest&)in->getRotationRequest();
  NMP::Vector3 chestUp = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection();
  NMP::Quat quat;
  quat.forRotation(chestUp, owner->owner->data->up);
  req.rotation = quat.toRotationVector();
  req.imminence = 0.5f;
  req.stiffnessScale = 2.0f;
  float& imp = (float&)in->getRotationRequestImportanceRef();
  imp = 1.0f;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta, 
    owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rooTM.translation(),
    req.rotation, NMP::Colour::ORANGE);
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
  if (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    shouldSpin = false;
  }
  // don't swing when in contact unless specifically required to
  if (!(in->getIgnoreContacts()) && owner->in->getIsBodyBarFeetContacting())
  {
    shouldSpin = false;
  }
  if (importance == 0.0f || owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    shouldSpin = false;
  }
  // don't spin if spin amount is zero.
  if (spinAmount == 0.0f)
  {
     shouldSpin = false;
  }

  // don't try and brace if hand is constrained
  if (shouldSpin == false)
  {
    // Reset state data
    data->targetDirection.setToZero();
    data->targetTorque.setToZero();
    return;
  }
  const ER::LimbTransform& rootTM = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM;
  NMP::Vector3 shoulderPos 
    = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation();
  bool useParams = in->getSpinParamsImportance() > 0.0f;
  bool swap = false;
  bool isRight = (shoulderPos - rootTM.translation()).dot(rootTM.rightDirection()) > 0.0f;
  // This coordinates the control between the limbs
  if (!useParams || in->getSpinParams().synchronised) // Default to synchronised
  {
    if (in->getCombinedTargetDirectionImportance())
    {
      data->targetDirection = in->getCombinedTargetDirection();
      bool spinInPhase = in->getArmsSpinInPhase();
      if (!spinInPhase && isRight)
      {
        swap = true;
        data->targetDirection = -data->targetDirection;
      }
    }
  }

  // These will be spin attributes:
  float lateralOffset = SCALE_DIST(0.3f);
  float upOffset = SCALE_DIST(0.0f);
  float forwardOffset = SCALE_DIST(0.2f);
  float radius = SCALE_DIST(0.6f);
  float angSpeed = SCALE_ANGVEL(14.0f);
  float spinOutwardsDistanceWhenBehind = SCALE_DIST(0.3f);
  float compensationScale = 1.0f;
  float strengthReductionTowardsEnd = 0.5f;

  if (useParams)
  {
    const SpinParameters& spinParams = in->getSpinParams();
    lateralOffset = spinParams.spinCentreLateral;
    upOffset      = spinParams.spinCentreUp;
    forwardOffset = spinParams.spinCentreForward;
    radius = spinParams.maxRadius;
    angSpeed = spinParams.maxAngSpeed; 
    spinOutwardsDistanceWhenBehind = spinParams.spinOutwardsDistanceWhenBehind;
    compensationScale = spinParams.spinArmControlCompensationScale;
    strengthReductionTowardsEnd = spinParams.strengthReductionTowardsEnd;
  }
  // This is then used to position the centre of the spin
  NMP::Vector3 spinCentre = rootTM.rightDirection() * lateralOffset * (isRight ? 1.0f : -1.0f);
  spinCentre += rootTM.upDirection() * upOffset + rootTM.frontDirection() * forwardOffset;

  // Need to initialise the state data. TODO: not sure if this is necessary, it might fall out more naturally
  if (data->targetDirection.magnitudeSquared() == 0.0f)
  {
    data->targetDirection =
      NMP::vNormalise( owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      - (shoulderPos + spinCentre));
  }

  float stiffness = owner->owner->data->maxStiffness * in->getRotationRequest().stiffnessScale * spinAmount;
  float maxNormalTorque = radius * radius * angSpeed * stiffness; // Notice the correct units here, torque/mass

  // 1. Calculate torque required to get to requested rotation within requested time
  NMP::Vector3 targetNormalisedTorque
    = getRequiredNormalisedTorque(data->targetTorque, owner, in,
      owner->owner->data->armLimbSharedStates[owner->data->childIndex], timeStep,
    maxNormalTorque, pDebugDrawInst);

  NMP::Vector3 torqueDir = targetNormalisedTorque;
  float normalisedTorqueMag = torqueDir.normaliseGetLength();

  // 2. Use totalTorqueMag to scale speed radius and stiffness down from their maximums
  scaleParametersProportionallyFromTorque(angSpeed, radius, stiffness, normalisedTorqueMag, owner);

  float radiusCutoff = SCALE_DIST(0.05f);
  if (radius < radiusCutoff)
    stiffness *= radius / radiusCutoff;

  // so set the target angular velocity
  NMP::Vector3 targetAngularVelocity = torqueDir * angSpeed;

  // 3. Now we have to move the internal target
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
    MR::VT_AngleDelta, shoulderPos, targetAngularVelocity * timeStep, NMP::Colour::PURPLE);
  NMP::Vector3 oldTarget = data->targetDirection * radius;

  data->targetDirection += NMP::vCross(targetAngularVelocity * timeStep, data->targetDirection);

  data->targetDirection -= torqueDir * torqueDir.dot(data->targetDirection); // flatten in direction of delta
  data->targetDirection.fastNormalise();

  NMP::Vector3 target = data->targetDirection * radius;
  // This is just a unitless constant which extends the arms outwards when doing large spins
  // it is about right, so doesn't really need to be adjusted
  const float outwardScaleWithRadius = 0.3f;
  NMP::Vector3 reachDir = NMP::vNormalise(spinCentre);
  NMP::Vector3 offset = torqueDir * torqueDir.dot(reachDir) * radius * outwardScaleWithRadius;

  // Push hands out when they go back
  float backward = 0.5f * (1.0f - rootTM.frontDirection().dot(data->targetDirection));
  target += rootTM.rightDirection() * (backward * spinOutwardsDistanceWhenBehind) * (isRight ? 1.0f : -1.0f);

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
    MR::VT_Normal, shoulderPos, spinCentre, NMP::Colour::PURPLE);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, 
    shoulderPos, SCALE_DIST(0.1f), NMP::Colour::RED);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
    MR::VT_Normal, shoulderPos + spinCentre, offset, NMP::Colour::BLUE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
    MR::VT_Normal, shoulderPos + spinCentre + offset, oldTarget, NMP::Colour::WHITE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
    MR::VT_Delta, shoulderPos + spinCentre + offset + oldTarget, target - oldTarget, NMP::Colour::YELLOW);

  NMP::Vector3 worldTarget = shoulderPos + spinCentre + offset + target;

  // 4. Work out control struct.
  LimbControl& control = out->startControlModification();
  control.reinit(
    stiffness, 
    0.5f * owner->data->normalDampingRatio, 
    owner->data->normalDriveCompensation * compensationScale);

  control.setExpectedRootForTarget(
    rootTM, 1,
    NMP::Vector3::InitZero,
    owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity);

  control.setTargetPos(worldTarget, 1.0f);
  control.setControlAmount(ER::spinControl, 1.0f);
  control.setStrengthReductionTowardsEnd(strengthReductionTowardsEnd);
  out->stopControlModification(importance); 

  if (swap)
  {
    data->targetDirection = -data->targetDirection;
  }
}
//----------------------------------------------------------------------------------------------------------------------
// ArmSpinFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void ArmSpinFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  // don't try and brace if hand is constrained
  if (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    return;
  }
  feedOut->setTargetDirection(data->targetDirection, in->getRotationRequestImportance());
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSpin::entry()
{
}
} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

