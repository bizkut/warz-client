/*
 * Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
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
#include "MyNetwork.h"
#include "SittingBodyBalance.h"
#include "SittingBodyBalancePackaging.h"
#include "BodyFramePackaging.h"
#include "BodyFrame.h"
#include "SupportPolygonData.h"

#define SCALING_SOURCE pkg.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
//
// How much importance to place on maintaining a sitting position.
//
static NM_INLINE float calculateImportance(const SittingBodyBalanceUpdatePackage& pkg, const float sitAmount)
{
  // Importance is proportional to the sq of balance amount / min balance amount, clamped to 1.

  const float minSitAmount = pkg.in->getSitParameters().minSitAmount;
  float importance = NMP::sqr(sitAmount / (minSitAmount + 0.001f));
  importance = NMP::minimum(importance, 1.0f);

  importance = (importance > 0.01f) ? importance : 0.0f; // Set small importance to zero.

  return importance;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Determine to what degree the character is considered to be sitting.
//
static float calculateSittingAmount(const SittingBodyBalanceUpdatePackage& pkg)
{
  float amount = 0.0f;

  const float pelvisOnGroundAmount     = pkg.feedIn->getPelvisOnGroundAmount();
  const float chestOnGroundAmount      = pkg.feedIn->getChestOnGroundAmount();
  const float standingBalanceAmount    = pkg.feedIn->getStandingBalanceAmount();
  const float minStandingBalanceAmount = pkg.in->getSitParameters().minStandingBalanceAmount;

  // Sitting amount is zero if pelvis has left the ground or chest is in contact with ground or character is standing.
  if ((pelvisOnGroundAmount > 0.0f) &&
      (chestOnGroundAmount < 1.0f) &&
      (standingBalanceAmount < minStandingBalanceAmount))
  {
    const NMP::Vector3 pelvisPosition = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();

    NMP::Vector3 chestNormal = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() - pelvisPosition;
    chestNormal.fastNormalise();

    NMP::Vector3 feetNormal = pkg.owner->owner->data->bodyLimbSharedState.m_averageOfFeetTM.translation() - pelvisPosition;
    feetNormal.fastNormalise();

    // determine how well spine is aligned with up direction * how orthogonal spine is to mean leg direction.
    amount = NMP::maximum(chestNormal.dot(pkg.owner->owner->data->up), 0.0f);
    amount *= (1.0f - NMP::nmfabs(feetNormal.dot(pkg.owner->owner->data->up)));
    amount *= (1.0f - NMP::nmfabs(feetNormal.dot(chestNormal)));
    amount = NMP::clampValue(amount, 0.0f, 1.0f);
    amount = NMP::fastSqrt(amount);

    // scale amount by how much the pelvis is on the ground and how much the chest isn't on the ground.
    amount *= NMP::clampValue(pelvisOnGroundAmount, 0.0f, 1.0f);
    amount *= NMP::clampValue(1.0f - chestOnGroundAmount, 0.0f, 1.0f);
  }

  return amount;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Update pakage data regarding the ground.
//
static void updateSupportingSurfaceProperties(
  const SittingBodyBalanceUpdatePackage& pkg,
  const float timeStep,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Calculate the motion of the ground under the feet/hands, and the floor normal by finding the weighted mean of the 
  // velocities of the limbs ends/roots in contact.
  NMP::Vector3 groundVel = NMP::Vector3::InitZero;
  uint32_t contactCount = 0;

  const uint32_t maxLimbIndex = pkg.owner->owner->data->numArms + pkg.owner->owner->data->numLegs + 1;

  // Build the array of the contacts: (spine root, {leg ends}, {arm ends})
  const ContactInfo* supportContacts[NetworkConstants::networkMaxNumLimbs];
  supportContacts[0] = &pkg.owner->owner->data->spineLimbSharedStates[0].m_rootContact;
  uint32_t i = 1;
  for( ; i < pkg.owner->owner->data->numLegs + 1; ++i)
  {
    supportContacts[i] = &pkg.owner->owner->data->legLimbSharedStates[i].m_endContact;
  }
  for( ; i < maxLimbIndex; ++i)
  {
    supportContacts[i] = &pkg.owner->owner->data->armLimbSharedStates[i].m_endContact;
  }

  
  for (uint32_t iLimb = 0; iLimb < maxLimbIndex ; ++iLimb)
  {
    if (supportContacts[iLimb]->inContact)
    {
      groundVel += supportContacts[iLimb]->lastVelocity;
      ++contactCount;
    }
  }

  if (contactCount > 0)
  {
    groundVel /= static_cast<float>(contactCount);
  }

  NMP::Vector3 groundAccel = (groundVel - pkg.data->lastGroundVel) / timeStep;
  pkg.data->lastGroundVel = groundVel;

  /// COMVel isn't very good on transitioning into balance from SK, so effectively disable its
  /// effects.
  if (pkg.data->needToInitialiseSupportingSurface)
  {
    pkg.data->COMVel = groundVel;
  }
  else
  {
    pkg.data->COMVel = pkg.owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();
  }

  // A small amount of smoothing is needed because otherwise the jitter from standing on smallish
  // objects messes up the balance horribly.
  float groundSmoothTime = SCALE_TIME(0.05f);
  SmoothCD(pkg.data->groundVel, pkg.data->groundVelRate, timeStep, groundVel, groundSmoothTime);
  SmoothCD(pkg.data->groundAccel, pkg.data->groundAccelRate, timeStep, groundAccel, groundSmoothTime);

  // Want to balance against an effective gravity direction that includes the tendency to decelerate
  // towards the floor motion
  pkg.data->effectiveGravityDir = pkg.owner->owner->data->gravity;

  // Lean into motion caused by accelerating floors. Remove the component along the acceleration
  // direction since it doesn't affect us when not leaning - i.e. if we're on a platform that's
  // accelerating upwards, we don't need to crouch/straighten.
  NMP::Vector3 horGroundAccel = pkg.data->groundAccel.getComponentOrthogonalToDir(pkg.owner->owner->data->down);
  pkg.data->effectiveGravityDir -= horGroundAccel;
  pkg.data->effectiveGravityDir.normalise();

  pkg.data->needToInitialiseSupportingSurface = false;

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration, 
    pkg.in->getSupportPolygon().getCentre(), SCALE_DIST(-pkg.data->effectiveGravityDir), NMP::Colour::DARK_GREEN);
}

//----------------------------------------------------------------------------------------------------------------------
static void updateSpineSupport(
  const SittingBodyBalanceUpdatePackage& pkg,
  const float importance)
{
  if (pkg.in->getSpinePoseEndRelativeToRootImportance(0) > 0.0f)
  {
    // Output desired chest translation and rotation for spine support module.
    NMP::Matrix34 desiredChestRotation = pkg.in->getSpinePoseEndRelativeToRoot(0);
    NMP::Vector3 desiredChestTranslation = desiredChestRotation.translation();

    NMP::Quat desiredChestQ = desiredChestRotation.toQuat();

    // Note that there is only ever one spine
    pkg.out->setChestRelPelvisOrientationAt(0, desiredChestQ, importance);
    pkg.out->setChestRelPelvisTranslationAt(0, desiredChestTranslation, importance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void updateHeadSupport(
  const SittingBodyBalanceUpdatePackage& pkg,
  const float NMP_UNUSED(importance))
{
  // heads relative to spine
  for (uint32_t iHead = 0; iHead < pkg.owner->owner->data->numHeads; ++iHead)
  {
    if (pkg.in->getHeadPoseEndRelativeToRootImportance(iHead) > 0.0f)
    {
      pkg.out->setHeadRelChestOrientationAt(iHead, pkg.in->getHeadPoseEndRelativeToRoot(iHead).toQuat(),
        pkg.in->getHeadPoseEndRelativeToRootImportance(iHead) * pkg.data->sitAmount);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void updateArmSupport(
  const SittingBodyBalanceUpdatePackage& pkg,
  const NMP::Matrix34& desiredPelvisTM,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  float supportImportance = 0.0f;

  if (pkg.in->getSupportWithArms() && (pkg.in->getSpinePoseEndRelativeToRootImportance(0) > 0.0f))
  {
    const ER::LimbTransform& pelvisTM = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM;
    const ER::LimbTransform& chestTM = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM;
    const NMP::Vector3& effectiveUpDir = -pkg.data->effectiveGravityDir;
    const NMP::Vector3& pelvisPosition = pelvisTM.translation();
    const float spineLength =
      pkg.owner->owner->data->defaultPoseLimbLengths[pkg.owner->owner->data->firstSpineRigIndex];

    // Calculate desired chest transform.
    NMP::Matrix34 desiredChestTM = pkg.in->getSpinePoseEndRelativeToRoot(0);
    desiredChestTM.multiply(desiredPelvisTM);

    // Calculate support importance - how vital the arms are in keeping the spine upright.
    NMP::Vector3 unused;
    const float separation =
      pkg.in->getSupportPolygon().getDistanceToPoint(chestTM.translation(), effectiveUpDir, 0.0f, &unused);
    supportImportance = NMP::clampValue(2.0f * separation / spineLength, 0.01f, 1.0f);

    // Iterate over arms.
    const uint32_t numArms = pkg.owner->owner->data->numArms;

    for (uint32_t iArm = 0; iArm < numArms; ++iArm)
    {
      // Calculate arm support strength scale.
      NMP::Vector3 relativeChestPosition = (chestTM.translation() - pelvisPosition);
      const float chestSeparationInUpDir = spineLength - relativeChestPosition.dot(effectiveUpDir);
      const BodyState& chestState = pkg.owner->data->chestState;
      float supportStrengthScale = chestSeparationInUpDir /
        (NMP::maximum(chestState.velocity.dot(effectiveUpDir), SCALE_VEL(0.1f)) * SCALE_TIME(1.0f));
      supportStrengthScale = NMP::clampValue(supportStrengthScale, 0.0f, 1.0f);
      supportStrengthScale *= pkg.feedIn->getPelvisOnGroundAmount();
      pkg.out->setArmSupportStrengthScalesAt(iArm, supportStrengthScale);
    }

    pkg.out->setDesiredChestTM(desiredChestTM, supportImportance);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredChestTM, SCALE_DIST(0.5f));
  }

  pkg.out->setArmSupportImportance(supportImportance);
}

//----------------------------------------------------------------------------------------------------------------------
static NMP::Matrix34 calculateDesiredPelvisTM(
  const SittingBodyBalanceUpdatePackage& pkg,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const ER::LimbTransform& pelvisTM = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM;
  const NMP::Vector3 effectiveUpDir = -pkg.data->effectiveGravityDir;

  NMP::Vector3 directionToFeet = pkg.owner->owner->data->bodyLimbSharedState.m_averageOfFeetTM.translation()
    - pelvisTM.translation();
  directionToFeet.fastNormalise();

  // Calculate desired pelvis transform.
  NMP::Vector3 worldRightDir = 
    NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(directionToFeet, effectiveUpDir);
  worldRightDir.fastNormalise();

  NMP::Vector3 worldForwardDir =
    NMRU::GeomUtils::calculateOrthogonalPlanarVector(effectiveUpDir, directionToFeet);
  worldForwardDir.fastNormalise();

  // Initialise TM from world space axis calculated above.
  NMP::Matrix34 desiredPelvisTM;
  desiredPelvisTM.setXAxis(worldForwardDir);
  desiredPelvisTM.setYAxis(effectiveUpDir);
  desiredPelvisTM.setZAxis(worldRightDir);
  desiredPelvisTM.setTranslation(pelvisTM.translation());

  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredPelvisTM, SCALE_DIST(0.4f));

  return desiredPelvisTM;
}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalanceUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  data->sitAmount = calculateSittingAmount(*this);
  const float importance = calculateImportance(*this, data->sitAmount);

  if (importance > 0.0f)
  {
    updateSupportingSurfaceProperties(*this, timeStep, pDebugDrawInst);

    const NMP::Matrix34 desiredPelvisTM = calculateDesiredPelvisTM(*this, pDebugDrawInst);

    updateSpineSupport(*this, importance);
    updateHeadSupport(*this, importance);
    updateArmSupport(*this, desiredPelvisTM, pDebugDrawInst);

    out->setDesiredPelvisTM(desiredPelvisTM, importance);
    out->setSupportWithArms(in->getSupportWithArms(), importance);
    out->setSupportWithLegs(in->getSupportWithLegs(), importance);
    out->setSupportWithSpine(true, importance);
    out->setAllowLegStep(false, importance);
  }
}


//----------------------------------------------------------------------------------------------------------------------
// SittingBodyBalanceFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalanceFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setSitAmount(data->sitAmount);

  NMP::Vector3 leanVector = owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() 
    - owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation();
  leanVector.fastNormalise();
  feedOut->setLeanVector(leanVector);
}

//----------------------------------------------------------------------------------------------------------------------
void SittingBodyBalance::entry()
{
  data->sitAmount = 0.0f;

  data->effectiveGravityDir.setToZero();
  data->groundVel.setToZero();
  data->groundVelRate.setToZero();
  data->lastGroundVel.setToZero();
  data->groundAccel.setToZero();
  data->groundAccelRate.setToZero();
  data->COMVel.setToZero();
  data->needToInitialiseSupportingSurface = true;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

