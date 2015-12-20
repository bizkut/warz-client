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
#include "ShieldManagementPackaging.h"
#include "ShieldManagement.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagementUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const float smoothReturnTP = in->getSmoothReturnTP();

  // Not against 1.0f as HazardImportance is clamped to 0.999f if coming from shieldAction.
  bool dangerIsMax = in->getDirectionToHazardImportance() > 0.99f;
  // Here we work out if character has to continue shield even if there is no more hazard,
  // as you do a shield for at least minPeriod sec.
  bool wasDoingShield =  data->shieldTimer != 0.0f;
  bool continueShield = wasDoingShield && data->shieldTimer < smoothReturnTP;
  if (dangerIsMax || continueShield)
  {
    // Max angle to consider a hazard comes from mostly vertical so character won't yaw to turn away from the hazard.
    const float angleVerticalHazard = SCALE_ANG(NM_PI / 9.0f);

    const NMP::Vector3 directionToHazardWS = in->getDirectionToHazard();
    const ER::LimbTransform& rootSpineOri = owner->owner->data->spineLimbSharedStates[0].m_rootTM; // Current orientation
    bool isSupported = owner->feedIn->getSupportAmount() != 0.0f;
    const ER::LimbTransform& referentOri = isSupported && feedIn->getSupportTMImportance() > 0.0f ?
      feedIn->getSupportTM() : owner->owner->data->spineLimbSharedStates[0].m_endTM; // Current referent orientation

    if (dangerIsMax)
    {
      // we make sure from now, shield will be done for at least minPeriod secs.
      data->shieldTimer = 0.0f;
    }

    data->shieldTimer += timeStep;

    float lastYawAngle = data->yawAngle;
    data->yawAngle = 0.0f;
    if (dangerIsMax)
    {
      // We work out the best orientation to consider in order to define how to turn away from hazard.
      ER::LimbTransform rootSpineAdvOri = rootSpineOri; // by default the orientation is based on end of spine.
      // Align up with the referent orientation so if he is supported it will be with supportTM.upDirection.
      rootSpineAdvOri.alignUpDirectionWith(referentOri.upDirection());

      NMP::Vector3 directionToHazardLocal;
      referentOri.inverseRotateVector(directionToHazardWS, directionToHazardLocal);
      ER::LimbTransform targetOrientation = rootSpineAdvOri;

      float hazardUpDir = -directionToHazardLocal.y;
      float verticalityHazard = NMP::clampValue(fabsf(hazardUpDir), -1.0f, 1.0f); ;
      bool mostlyVertical = acosf(verticalityHazard) < angleVerticalHazard;
      //if hazard is mostly vertical, we don't yaw to turn away from the hazard
      if (!mostlyVertical)
      {
        // Level targetDir because we only want to compute the yaw angle.
        NMP::Vector3 targetDir = -directionToHazardWS.getComponentOrthogonalToDir(referentOri.upDirection());
        targetDir.fastNormalise();

        // Work out yaw rotation for the spine to turn away from the hazard.
        NMP::Vector3 currentDir = referentOri.frontDirection();
        NMP::Quat yaw;
        yaw.forRotation(currentDir, targetDir);

        // Add some hystersis to avoid switching between side he chooses to yaw.
        NMP::Vector3 yawRotationVector = yaw.toRotationVector();
        data->yawAngle = yawRotationVector.dot(targetOrientation.upDirection());
        if ((lastYawAngle > 0.0f) != (data->yawAngle > 0.0f))
        {
          const float hysteresisAngle = NMP::degreesToRadians(10.0f);
          if (fabsf(data->yawAngle) > (NM_PI - hysteresisAngle))
          {
            // Invert side choosen to keep the side of the precedent frames.
            data->yawAngle = -data->yawAngle;
            yawRotationVector = -yawRotationVector;
          }
        }

        // Clamp rotation: Need to "drive" IK otherwise could pitch backward.
        yawRotationVector.clampMagnitude(NM_PI_OVER_TWO);
        yaw.fromRotationVector(yawRotationVector, false);
        yaw.rotateVector(currentDir, targetDir);
        targetOrientation.alignFrontDirectionWith(targetDir);
      }

      // Set values which will be used to turn away from the hazard. By default time to impact set to
      // 1 sec as we can't always trust imminence especially if shield is forced (i.e shieldAction).
      float imminence = 1.0f / SCALE_TIME(1.0f);
      float turnAwayScale = in->getTurnAwayScale();
      NMP::Quat toTarget = targetOrientation.toQuat() * ~rootSpineOri.toQuat();
      if (in->getDirectionToHazardImportance() == 1.0f && isSupported)
      {
        // We increase imminence to allow a fast rotation when he is supported.
        // Time to impact set to 0.1 sec, maximal value to have the fastest rotation.
        imminence = SCALE_IMMINENCE(10.0f) * turnAwayScale;
      }
      NMP::Vector3 turnAwayRotation = toTarget.toRotationVector(false);
      // Remove the pitch as it's now configure directly through balancer.
      float pitch = turnAwayRotation.dot(referentOri.rightDirection());
      turnAwayRotation -= pitch * rootSpineOri.upDirection();
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation(), turnAwayRotation, NMP::Colour::BLUE);
      RotationRequest turnAwayRotationRequest (turnAwayRotation, SCALING_SOURCE);
      turnAwayRotationRequest.imminence = imminence;
      turnAwayRotationRequest.stiffnessScale = 2.0f;
      turnAwayRotationRequest.passOnAmount = turnAwayScale; // By default, use half of the lower body
      out->setSpineRotationRequestAt(0, turnAwayRotationRequest, 1.0f);
    }
    else
    {
      // If character is still doing a shield but there is no more hazard, smooth
      // return to the default pose for the spine, we assume character has only one real spine.
      NMP::Matrix34 targetOrientation =
        owner->owner->data->defaultPoseEndsRelativeToRoot[MyNetworkData::firstSpineNetworkIndex] *
        owner->owner->data->spineLimbSharedStates[0].m_rootTM;
      NMP::Quat toDefault = targetOrientation.toQuat() * ~rootSpineOri.toQuat();
      float imminence = 1.0f / smoothReturnTP;
      RotationRequest returnToDefault (toDefault.toRotationVector(), SCALING_SOURCE);
      returnToDefault.imminence = imminence;
      returnToDefault.passOnAmount = 0.0f; // Do not use his lower body at all.
      float imp = NMP::clampValue(1.0f - data->shieldTimer / smoothReturnTP, 0.0f, 1.0f);
      out->setSpineRotationRequestAt(0, returnToDefault, imp);
    }
  }
  else
  {
    // character doesn't do a shield, so we reset timer.
    data->shieldTimer = 0.0f;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ShieldManagementFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  //if timer is greater than 0, means character has to do a shield
  if (data->shieldTimer > 0.0f)
  {
    const float smoothReturnTP = in->getSmoothReturnTP();
    bool dangerIsMax = in->getDirectionToHazardImportance() >= 0.99f;
    // Ramp down importance proportionally to time after impact increase.
    float imp = dangerIsMax ? 1.0f : NMP::clampValue(1.0f - data->shieldTimer / smoothReturnTP, 0.0f, 1.0f);
    feedOut->setDoShieldWithUpperBody(imp);

    // Default value.
    float normalDriveCompensation = 1.0f;

    if (owner->feedIn->getSupportAmount() == 0.0f) // if not supported, will do a shield with lower body.
    {
      feedOut->setDoShieldWithLowerBody(imp);
      // Set drive compensation to 0 to make the character looser to impacts.
      normalDriveCompensation = 0.0f;
    }

    feedOut->setDriveCompensationScale(normalDriveCompensation, 1.0f);

    if (in->getDirectionToHazardImportance() > 0.0f)
    {
      // Use the support TM if standing, or pelvis otherwise
      ER::LimbTransform supportTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
      if (feedIn->getSupportTMImportance() > 0.0f)
      {
        supportTM = feedIn->getSupportTM();
      }
      const NMP::Vector3& directionToHazard = in->getDirectionToHazard();
      float fwd = supportTM.frontDirection().dot(directionToHazard);
      float right = supportTM.rightDirection().dot(directionToHazard);
      float angle = atan2f(right, fwd);
      feedOut->setHazardAngle(angle);
    }

    feedOut->setShieldSpineYawAngle(data->yawAngle);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldManagement::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

