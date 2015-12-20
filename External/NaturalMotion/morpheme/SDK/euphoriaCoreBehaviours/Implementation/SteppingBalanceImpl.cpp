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
#include "SteppingBalancePackaging.h"
#include "SteppingBalance.h"
#include "euphoria/erDebugDraw.h"

//----------------------------------------------------------------------------------------------------------------------


#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// SteppingBalanceUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void SteppingBalanceUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  SteppingBalanceParameters sbp = in->getSteppingBalanceParameters();
  if (in->getSteppingBalanceParametersImportance() == 0.0f)
    new (&sbp) SteppingBalanceParameters();

  NMP::Vector3 stepDir(NMP::Vector3::InitZero);

  // stepping or not?
  if (!owner->feedIn->getIsStepping() || feedIn->getIsDoingRecoveryStep())
  {
    data->wasStepping = false;
    data->timeSinceLastStep += timeStep;
    if (data->timeSinceLastStep > sbp.stepCountResetTime)
    {
      data->steppingTime = 0.0f;
      data->stepCount = 0;
    }
    data->armPoseAmount -= timeStep / SCALE_TIME(1.0f);
    if (data->armPoseAmount < 0.0f)
      data->armPoseAmount = 0.0f;
    data->stepLegIndex = -1;

    if (owner->feedIn->getSupportAmount() == 0.0f)
      return;
  }
  else
  {
    data->timeSinceLastStep = 0.0f;
    data->steppingTime += timeStep;
    NMP::Vector3 CoMVel = feedIn->getCentreOfMassBodyState().velocity;
    if (CoMVel.getComponentOrthogonalToDir(owner->owner->data->up).magnitudeSquared() > NMP::sqr(sbp.minSpeedForLooking))
    {
      // Only update the stepping look direction if there's significant movement
      data->horLookDir = CoMVel.getComponentOrthogonalToDir(owner->owner->data->up);
      data->horLookDir.fastNormalise();
    }
    data->armPoseAmount = 1.0f;

    if (!data->wasStepping)
    {
      data->wasStepping = true;
      ++data->stepCount;
    }

    for (uint32_t i = 0 ; i < owner->owner->data->numLegs ; ++i)
    {
      if (feedIn->getIsLegStepping(i))
      {
        stepDir = feedIn->getStepDir(i);
        if (i != (uint32_t) data->stepLegIndex)
        {
          data->stepLegIndex = i;
        }
      }
    }
  }

  // Pose with arms when stepping
  {
    float armReadyPoseStrength = data->armPoseAmount;
    if (armReadyPoseStrength > 0.0f)
    {
      // put the arms into a "ready" pose
      PoseData poseData; // Need to send these type of data for defaultPose
      poseData.stiffnessScale = armReadyPoseStrength;
      poseData.driveCompensationScale = armReadyPoseStrength;
      poseData.gravityCompensation = 1.0f; // hold the arms loosely
      // Default to graduating the control along the arm to reduce the stiffness. The effect isn't
      // really pronounced enough to warrant exposing it at the moment.
      poseData.strengthReductionTowardsEnd = 0.7f;

      for (uint32_t armIndex = 0 ; armIndex < owner->owner->data->numArms ; ++armIndex)
      {
        float armImportance = data->armPoseAmount;

        float armReadyWeight = in->getArmReadyPoseEndRelativeToRootImportance(armIndex);
        if (armReadyWeight > 0.0f)
        {
          poseData.poseEndRelativeToRoot = in->getArmReadyPoseEndRelativeToRoot(armIndex);
          armImportance *= in->getArmReadyPoseEndRelativeToRootImportance(armIndex);
        }
        else
        {
          poseData.poseEndRelativeToRoot = in->getArmBalancePoseEndRelativeToRoot(armIndex);
          armImportance *= in->getArmBalancePoseEndRelativeToRootImportance(armIndex);
        }
        if (armImportance < 0.001f)
          continue;

        // Add additional swing of the arms to coordinate with the feet
        if (sbp.armSwingStrengthScaleWhenStepping > 0.0f && owner->feedIn->getIsStepping() &&
          (owner->owner->data->numLegs == owner->owner->data->numArms))
        {
          NMP::Vector3 handOffset = stepDir * SCALE_DIST(0.4f);
          handOffset += owner->owner->data->up * SCALE_DIST(0.2f);

          bool isThisArmRight = owner->owner->data->
            defaultPoseEndsRelativeToRoot[owner->owner->data->firstArmRigIndex + armIndex].translation().z > 0.0f;
          bool isSteppingLegRight = owner->owner->data->
            defaultPoseEndsRelativeToRoot[owner->owner->data->firstLegRigIndex + data->stepLegIndex].translation().z > 0.0f;

          if (isThisArmRight == isSteppingLegRight)
            handOffset *= -1.0f;

          // Get the hand offset into the space of the arm root (all arm roots are just the spine
          // end) - after this we don't need to worry about the world up direction.
          NMP::Matrix34 rootTMInv = owner->owner->data->spineLimbSharedStates[0].m_endTM;
          rootTMInv.invertFast();
          rootTMInv.rotateVector(handOffset);

          poseData.poseEndRelativeToRoot.translation() += handOffset * sbp.armSwingStrengthScaleWhenStepping;
        }

        // Offset according to our movement
        const NMP::Vector3& CoMVel = feedIn->getCentreOfMassBodyState().velocity;
        const ER::LimbTransform& spineRootTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
        const NMP::Vector3& groundVel = feedIn->getGroundVel();

        const NMP::Vector3 velRelSupport = CoMVel - groundVel;

        float speedRight = spineRootTM.rightDirection().dot(velRelSupport);
        float speedForward = spineRootTM.frontDirection().dot(velRelSupport);

        const float deltaTime = sbp.stepPoseArmDisplacementTime;
        if (speedForward < 0.0f)
          poseData.poseEndRelativeToRoot.translation().x += speedForward * deltaTime;
        if (speedRight * poseData.poseEndRelativeToRoot.translation().z > 0.0f)
          poseData.poseEndRelativeToRoot.translation().z += speedRight * deltaTime;

        out->setArmPoseAt(armIndex, poseData, armImportance);
      }
    }
  }

  // Looking in the step direction
  if (sbp.lookInStepDirection && data->stepCount >= sbp.stepsBeforeLooking)
  {
    // Don't look directly backwards as it generally results in indecisiveness about which way to
    // twist. Instead choose a direction to turn and stick to it until out of the backwards zone.
    const float dotForLook = -0.5f;
    float horLookDirDotFwd = data->horLookDir.dot(owner->owner->data->spineLimbSharedStates[0].m_endTM.frontDirection());
    if (horLookDirDotFwd < dotForLook)
    {
      // Just look directly back based on the chest - this will be stable enough to prevent
      // oscillations.
      data->horLookDir = -owner->owner->data->spineLimbSharedStates[0].m_endTM.frontDirection();
      data->horLookDir = data->horLookDir.getComponentOrthogonalToDir(owner->owner->data->up);
      data->horLookDir.fastNormalise();
    }

    NMP::Vector3 CoMPos = feedIn->getCentreOfMassBodyState().position;

    // Make a sensible look position
    float lookAheadDist = SCALE_DIST(3.0f);
    float decayTime = sbp.stopLookingTime;

    NMP::Vector3 lookLeft = NMP::vCross(owner->owner->data->up, data->horLookDir);
    NMP::Quat q(lookLeft, NMP::degreesToRadians(sbp.lookDownAngle));

    NMP::Vector3 lookPos = CoMPos + q.rotateVector(data->horLookDir * lookAheadDist);

    TargetRequest lookRequest(lookPos, SCALING_SOURCE);

    float imminenceWhenStepping = lookRequest.imminence;
    float imminence = (1.0f - (data->timeSinceLastStep / decayTime)) * imminenceWhenStepping;
    if (imminence > 0.0f)
    {
      lookRequest.imminence = imminence;
      // if this is too large then it destabilises the step, as the spine twists (etc)
      lookRequest.passOnAmount = sbp.wholeBodyLook;

      out->setLookTarget(lookRequest, sbp.lookWeight);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, lookPos, SCALE_DIST(0.5f), NMP::Colour::RED);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SteppingBalanceFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void SteppingBalanceFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setTimeSinceLastStep(data->timeSinceLastStep);
  feedOut->setSteppingTime(data->steppingTime);
  feedOut->setStepCount(data->stepCount);
}

//----------------------------------------------------------------------------------------------------------------------
void SteppingBalance::entry()
{
  data->timeSinceLastStep = 1.0e6f;
  data->steppingTime = 0.0f;
  data->armPoseAmount = 0.0f;
  data->horLookDir.setToZero();
  data->stepLegIndex = -1;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

