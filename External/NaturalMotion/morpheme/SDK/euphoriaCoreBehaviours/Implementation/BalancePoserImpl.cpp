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
#include "BalancePoserPackaging.h"
#include "BalancePoser.h"

#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

static void weightedYawPitchRoll(NMP::Quat& q, float yaw, float pitch, float roll, float weight)
{
  NMP::Quat offsetQ;

  if (yaw != 0.0f)
  {
    offsetQ.fromAxisAngle(NMP::Vector3YAxis(), yaw * weight);
    q.multiply(offsetQ);
  }
  if (pitch != 0.0f)
  {
    offsetQ.fromAxisAngle(NMP::Vector3ZAxis(), pitch * weight);
    q.multiply(offsetQ);
  }
  if (roll != 0.0f)
  {
    offsetQ.fromAxisAngle(NMP::Vector3XAxis(), roll * weight);
    q.multiply(offsetQ);
  }

  q.fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (in->getPelvisRelSupportImportance() == 0.0f)
    return;

  // Pelvis control: rotate input TM by specified values around yaw, pitch, roll axes
  //
  NMP::Matrix34 pelvisRelSupport = in->getPelvisRelSupport();
  NMP::Matrix34 offsetTM;

  const BalancePoseParameters& poseParams = in->getPoseParameters();

  if (poseParams.pelvisMultiplier != 0.0f)
  {
    if (poseParams.yaw != 0.0f)
    {
      offsetTM.set3x3ToYRotation(poseParams.yaw * poseParams.pelvisMultiplier);
      pelvisRelSupport.multiply3x3(offsetTM);
    }
    if (poseParams.pitch != 0.0f)
    {
      offsetTM.set3x3ToZRotation(poseParams.pitch * poseParams.pelvisMultiplier);
      pelvisRelSupport.multiply3x3(offsetTM);
    }
    if (poseParams.roll != 0.0f)
    {
      offsetTM.set3x3ToXRotation(poseParams.roll * poseParams.pelvisMultiplier);
      pelvisRelSupport.multiply3x3(offsetTM);
    }
  }

  // crouch
  pelvisRelSupport.translation().y *= 1.0f - poseParams.crouchAmount;

  out->setPelvisRelSupport(pelvisRelSupport, 1.0f);

  // Chest orientation control (only first is set, as we support only one spine)
  //

  // If the chest is rotated, we will also move it
  NMP::Vector3 chestRelPelvisTranslation = in->getChestRelPelvisTranslation(0);

  if (poseParams.spineMultiplier != 0.0f)
  {
    NMP::Quat chestRelPelvisOrientation = in->getChestRelPelvisOrientation(0);
    weightedYawPitchRoll(chestRelPelvisOrientation,
      poseParams.yaw, -poseParams.pitch, -poseParams.roll, poseParams.spineMultiplier);
    out->setChestRelPelvisOrientationAt(0, chestRelPelvisOrientation, 1.0f);

    // also move the chest to follow the rotation
    NMP::Quat q(NMP::Quat::kIdentity);

    // The angle is distributed along the spine, so the position change needs to be reduced,
    // otherwise the spine gets an "S" shape. angleFrac is therefore as big as possible without
    // getting this S shape.
    float angleFrac = 0.75f;
    weightedYawPitchRoll(q, poseParams.yaw, -poseParams.pitch, -poseParams.roll, angleFrac * poseParams.spineMultiplier);

    chestRelPelvisTranslation = q.rotateVector(chestRelPelvisTranslation);

    // The spine up is always along the pelvis y direction. Shorten the spine such that if the total
    // angle was 180deg then the chest would bend back to the pelvis (a rather extreme case!)
    float totalAngle = poseParams.spineMultiplier *
      NMP::fastSqrt(NMP::sqr(poseParams.yaw) + NMP::sqr(poseParams.pitch) + NMP::sqr(poseParams.roll));
    chestRelPelvisTranslation.y *= 1.0f - 0.2f * totalAngle / NM_PI;
  }
  // Chest translation control.
  //
  // Convert translation request in pelvis frame.
  if (poseParams.chestTranslation.magnitudeSquared() != 0.0f || poseParams.spineMultiplier > 0.0f)
  {
    NMP::Vector3 poseChestRelPelvisTranslation = poseParams.chestTranslation;
    NMP::Matrix34 currPelvisTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
    currPelvisTM.inverseRotateVector(poseChestRelPelvisTranslation);

    chestRelPelvisTranslation += poseChestRelPelvisTranslation;
    out->setChestRelPelvisTranslationAt(0, chestRelPelvisTranslation, 1.0f); // Euphoria only supports 1 spine
  }

  // Head control.
  if (poseParams.headMultiplier != 0.0f)
  {
    for (uint32_t iHead = 0; iHead < owner->owner->data->numHeads; ++iHead)
    {
      NMP::Quat headRelChestOrientation = in->getHeadRelChestOrientation(iHead);
      weightedYawPitchRoll(headRelChestOrientation,
        poseParams.yaw, -poseParams.pitch, -poseParams.roll, poseParams.headMultiplier);
      out->setHeadRelChestOrientationAt(iHead, headRelChestOrientation, 1.0f);
    }
  }

#if defined(MR_OUTPUT_DEBUGGING)
  {
    NMP::Matrix34 chestRelPelvis(out->getChestRelPelvisOrientation(0), out->getChestRelPelvisTranslation(0));
    NMP::Matrix34 desiredChest = chestRelPelvis * owner->owner->data->spineLimbSharedStates[0].m_rootTM;
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredChest, SCALE_DIST(2.0f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, owner->owner->data->spineLimbSharedStates[0].m_rootTM, SCALE_DIST(0.1f));
  }
#endif

}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoser::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

