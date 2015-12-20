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
#include "SpineWrithePackaging.h"
#include "SpineWrithe.h"
#include "Spine.h"
#include "ArmPackaging.h"
#include "MyNetwork.h"
#include "Helpers/Helpers.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// SpineWritheUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void SpineWritheUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (in->getWritheParamsImportance() == 0.0f)
  {
    return;
  }

  const WritheParameters& writheParams = in->getWritheParams();

  if (data->lastResetTimer > writheParams.resetTargetTP) // Need to set a new target
  {
    //Work out based TM in root's space.
    NMP::Matrix34 basedTM = writheParams.defaultPoseEndRelativeToRoot;
    if (!writheParams.basedOnDefaultPose)
    {
      NMP::Matrix34 rootTM = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM;
      NMP::Matrix34 endTM = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_endTM;
      rootTM.invertFast();
      basedTM.multiply(endTM, rootTM);
    }
    //Work out random rotation to add.
    float rTwist = data->rng.genFloat();
    float rx = data->rng.genFloat();
    float ry = data->rng.genFloat();
    float rz = data->rng.genFloat();
    rTwist = rTwist * 2.0f - 1.0f;
    rx = rx * 2.0f - 1.0f;
    ry = ry * 2.0f - 1.0f;
    rz = rz * 2.0f - 1.0f;
    // Twist clamped by amplitudeScale.
    NMP::Vector3 writheTwist =
      owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir()
        * 2.0f * NM_PI * writheParams.amplitudeScale * rTwist;
    // Add another random rotation without any twist.
    NMP::Vector3 writheRandom(rx, ry, rz);
    writheRandom -= writheRandom.dot(
      owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir())
      * owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir();
    writheRandom.getNormalised();
    // Ya and pitch clamped based on reachLimit.
    writheRandom = writheRandom * cosf(owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_reachLimit.m_angle) * writheParams.amplitudeScale;
    NMP::Matrix34 writheRotTM;
    writheRotTM.fromRotationVector(writheRandom + writheTwist);
    basedTM.multiply3x3(writheRotTM);

    //Work out random translation to add.
    rx = data->rng.genFloat();
    ry = data->rng.genFloat();
    rz = data->rng.genFloat();
    rx = rx * 2.0f - 1.0f;
    ry = ry * 2.0f - 1.0f;
    rz = rz * 2.0f - 1.0f;
    NMP::Vector3 writheTranslation(rx, ry, rz);
    writheTranslation = writheTranslation.getNormalised() * owner->data->spineLength * writheParams.amplitudeScale;
    data->targetTM.translation() = basedTM.translation() + writheTranslation;
    basedTM.translation() += writheTranslation;
    data->targetTM = basedTM;
    data->lastResetTimer = 0.0f; // Reset timer.
  }

  // Send control struct.
  LimbControl& control = out->startControlModification();
  control.reinit(
    owner->data->normalStiffness * writheParams.stiffnessScale,
    owner->data->normalDampingRatio,
    owner->data->normalDriveCompensation * writheParams.driveCompensationScale);
  control.setExpectedRootForTarget(
    owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM,
    1.0f,
    owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootData.velocity,
    owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity);
  NMP::Matrix34 currentTargetTM = data->targetTM;
  currentTargetTM.multiply(owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM);
  control.setTargetPos(currentTargetTM.translation(), 1.0f, 
    owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootData.velocity);
  control.setTargetOrientation(currentTargetTM, 1.0f,
    owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity);
  control.setControlAmount(ER::writheControl, 1.0f);
  control.setGravityCompensation(1.0f);
  out->stopControlModification(1.0f);

  // Store data.
  data->lastResetTimer += timeStep; // Update timer.

  // Debug
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, currentTargetTM, SCALE_DIST(1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe::entry()
{
  data->targetTM.identity();
  data->rng.setSeed(owner->owner->getAndBumpNetworkRandomSeed());
  data->lastResetTimer = 100000.0f;  // Set to a really long time to be sure it starts at the entry.
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
