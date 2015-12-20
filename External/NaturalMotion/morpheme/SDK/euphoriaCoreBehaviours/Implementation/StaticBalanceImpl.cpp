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
#include "StaticBalancePackaging.h"
#include "StaticBalance.h"
#include "BalanceManagementPackaging.h"
#include "MyNetworkPackaging.h"
#include "Helpers/Helpers.h"
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------


#define SCALING_SOURCE dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
static void applyEndDirectionRequest(
  const NMP::Matrix34& rootTM,
  const NMP::Matrix34& endRelRootOrientation,
  const DirectionRequest& endDirectionRequest,
  const float requestWeight,
  NMP::Matrix34& resultEndRelRootRotation,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const ER::DimensionalScaling& MR_OUTPUT_DEBUG_ARG(dimensionalScaling))
{
  NMP::Matrix34 desiredEndTM = endRelRootOrientation * rootTM;

  // get the difference between the control dir ("desired") and reference dir in terms of pitch and
  // yaw (we are trying to preserve roll)
  //
  // control direction wrt world (on the desired end tm)
  NMP::Vector3 controlW;
  desiredEndTM.rotateVector(endDirectionRequest.controlDirLocal, controlW);
  // reference direction
  NMP::Vector3 referenceW = endDirectionRequest.referenceDirWorld;

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    desiredEndTM.translation(), controlW * SCALE_DIST(10.0f), NMP::Colour::WHITE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    desiredEndTM.translation(), referenceW * SCALE_DIST(10.0f), NMP::Colour::RED);

  // reference direction with up component removed
  NMP::Vector3 endTMUp = desiredEndTM.yAxis();
  NMP::Vector3 referenceInPlane = referenceW - endTMUp * endTMUp.dot(referenceW);

  // yaw (difference between reference in plane and controlW)
  NMP::Quat q;
  q.forRotation(controlW, referenceInPlane);

  // apply the difference to the desired tm and recompute controlW
  NMP::Quat desiredEndQuat = desiredEndTM.toQuat();
  NMP::Quat targetQ = desiredEndQuat;
  targetQ.multiply(q, targetQ);
  targetQ.rotateVector(endDirectionRequest.controlDirLocal, controlW);

  // pitch (difference between the yaw adjusted control and the reference)
  q.forRotation(controlW, referenceW);

  targetQ.multiply(q, targetQ);

  // slerp towards the new target
  NMP::Quat desiredQ = desiredEndQuat;
  desiredQ.slerp(targetQ, requestWeight);
  desiredEndTM.fromQuat(desiredQ);

  NMP::Matrix34 rootTMInv(rootTM);
  rootTMInv.invert();
  resultEndRelRootRotation = desiredEndTM * rootTMInv;

  // adding this after a number of asserts fired (on the headrelchest case) that proved to be seeing
  // a result just outside the ortho test threshold
  resultEndRelRootRotation.orthonormalise();
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling

//----------------------------------------------------------------------------------------------------------------------
void StaticBalanceUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* pDebugDrawInst)
{
  if (owner->owner->data->numLegs == 0)
    return;

  if (in->getLegPoseEndRelativeToRootImportance(0) == 0.0f) // can happen if the BI is disabled
    return;

  NMP::Matrix34 targetPelvisRelSupport =
    getSupportTransformRelRoot(
    &in->getLegPoseEndRelativeToRoot(0),
    owner->owner->data->numLegs,
    NMP::Vector3YAxis());
  targetPelvisRelSupport.invertFast();

  NMP::Matrix34 targetChestRelPelvis = in->getSpinePoseEndRelativeToRoot(0);

  float crouchDistance = 0.0f;

  // Only do the following modifications and calculations for the spine etc if
  // there's actually some kind of balance going on.
  if (feedIn->getSupportTMImportance() > 0.0f)
  {
    const NMP::Matrix34& supportTM = feedIn->getSupportTM();

    // Crouch a bit when something would make us unstable
    if (owner->feedIn->getImpendingInstabilityImportance())
    {
      // The impendingInstability value is a strategy for dealing with an approaching impact or
      // knock. The strategy is to crouch the character down as the instability approaches, then
      // lessen the crouch in the last fraction of a second. The point of this lessening is twofold,
      // firstly it prevents the character trying to crouch suddenly if it suddenly receives an
      // instability due in a fraction of a second; secondly the lessening acts to push the feet
      // against the ground and so increase the ground friction as the impact hits.
      // * The instability value is a measure of how unstable this instability will be... currently
      //  in momentum or impulse units but take your pick
      // * the instability importance is 1/(1+ time until instability occurs)
      // Need to expose these params - see MORPH-11239
      const float maxInstability = 5.0f;
      // Calculate a sensible maximum crouch distance
      float length = targetPelvisRelSupport.translation().y;
      float crouchAmount = owner->feedIn->getImpendingInstabilityImportance();
      // Make the crouch amount 0 when in threat is far away, and 0 when it comes very close
      crouchAmount *= 4.0f * (1.0f - crouchAmount);
      crouchAmount *= NMP::minimum(owner->feedIn->getImpendingInstability(), maxInstability) / maxInstability;
      crouchDistance = crouchAmount * length * in->getStabilisingCrouchDownAmount();

      MR_DEBUG_DRAW_VECTOR(
        pDebugDrawInst, MR::VT_Delta,
        supportTM.translation() + owner->owner->data->up * SCALE_DIST(3.0f),
        owner->owner->data->up * crouchDistance, NMP::Colour::WHITE);

      // When you crouch downward you should also bend forwards a little
      NMP::Matrix34 crouchForwards;
      crouchForwards.set3x3ToZRotation(crouchAmount * in->getStabilisingCrouchPitchAmount());
      targetPelvisRelSupport.multiply3x3(crouchForwards);
    }

    // Need to expose these params - MORPH-11240

    // should be legs support stiffness / 2.25... we'll need to pass this up. The division is
    // because feet are touching ground, so isn't as strong as it thinks
    float stiffness = SCALE_STIFFNESS(16.0f / 2.25f);
    float hipsBendWeight = 0.5f; // need this to be data driven somehow

    ProcessRequest processRotation;
    float rotationWeight = processRotation.processRequest(
      owner->feedIn->getPelvisRotationRequestImportance(),
      owner->feedIn->getPelvisRotationRequest().imminence,
      stiffness,
      hipsBendWeight);

    ProcessRequest processDirection;
    float directionWeight = processDirection.processRequest(
      owner->feedIn->getPelvisDirectionRequestImportance(),
      owner->feedIn->getPelvisDirectionRequest().imminence,
      stiffness,
      hipsBendWeight);

    // Can't shift weight much, need this to be data driven - MORPH-11240
    float hipMovementWeight = 0.5f;
    ProcessRequest processTranslation;
    float translationWeight = processTranslation.processRequest(
      owner->feedIn->getPelvisTranslationRequestImportance(),
      owner->feedIn->getPelvisTranslationRequest().imminence, 
      stiffness,
      hipMovementWeight);

    translationWeight = NMP::clampValue(translationWeight, 0.0f, 1.0f);
    rotationWeight = NMP::clampValue(rotationWeight, 0.0f, 1.0f);

    ER::LimbTransform desiredPelvisTM = targetPelvisRelSupport * supportTM ;
    ER::LimbTransform targetPelvisTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
    if (translationWeight > 0.0f)
    {
      targetPelvisTM.translation() += owner->feedIn->getPelvisTranslationRequest().translation;
      desiredPelvisTM.translation() = targetPelvisTM.translation() * translationWeight +
        desiredPelvisTM.translation() * (1.0f - translationWeight);
    }

    // Apply the pelvis rotation request
    if (rotationWeight > 0.0f)
    {
      NMP::Quat targetQ = targetPelvisTM.toQuat();
      NMP::Quat q; q.fromRotationVector(owner->feedIn->getPelvisRotationRequest().rotation);
      targetQ.multiply(q, targetQ);
      NMP::Quat desiredQ = desiredPelvisTM.toQuat();
      desiredQ.slerp(targetQ, rotationWeight);
      desiredPelvisTM.fromQuat(desiredQ);
    }

    // Slerp from the regular orientation to the orientation requested
    float targetPelvisOrientationWeight = in->getTargetPelvisOrientationImportance();
    if (targetPelvisOrientationWeight > 0.0f)
    {
      NMP::Quat result;
      result.slerp(desiredPelvisTM.toQuat(), in->getTargetPelvisOrientation(), targetPelvisOrientationWeight);
      desiredPelvisTM.fromQuat(result);
    }

    // Slerp from the regular orientation to the orientation requested
    float targetPelvisDirectionWeight = in->getTargetPelvisDirectionImportance();
    if (targetPelvisDirectionWeight > 0.0f)
    {
      ER::LimbTransform targetedPelvisTM = desiredPelvisTM;
      targetedPelvisTM.alignFrontDirectionWith(in->getTargetPelvisDirection());

      NMP::Quat aRot = desiredPelvisTM.toQuat();
      NMP::Quat bRot = targetedPelvisTM.toQuat();
      NMP::Quat result;
      result.slerp(aRot, bRot, targetPelvisDirectionWeight);
      desiredPelvisTM.fromQuat(result);
    }

    NMP::Matrix34 supportTMInv(supportTM);
    supportTMInv.invert();
    targetPelvisRelSupport = desiredPelvisTM * supportTMInv;

    // Apply the crouch
    targetPelvisRelSupport.translation().y -= crouchDistance;

    if (directionWeight > 0.0f)
    {
      applyEndDirectionRequest(
        supportTM, targetPelvisRelSupport,
        feedIn->getPelvisDirectionRequest(), directionWeight, targetPelvisRelSupport, pDebugDrawInst, SCALING_SOURCE);
    }

    // Chest relative to pelvis
    // Also apply lean etc rotations to the spine to make it bend naturally
    NMP::Matrix34 desiredChestRotation = in->getSpinePoseEndRelativeToRoot(0);

    // Apply any chest requests
    // Need to expose these params - MORPH-11240
    float chestStiffness = SCALE_STIFFNESS(16.0f / 2.25f);
    float chestFlexibility = 0.5f;
    ProcessRequest processChestDirection;
    float chestDirectionWeight = processChestDirection.processRequest(
      owner->feedIn->getChestDirectionRequestImportance(),
      owner->feedIn->getChestDirectionRequest().imminence,
      chestStiffness,
      chestFlexibility);
    if (chestDirectionWeight > 0.0f)
    {
      NMP::Matrix34 basicDesiredPelvisTM = targetPelvisRelSupport * supportTM;
      applyEndDirectionRequest(
        basicDesiredPelvisTM,
        desiredChestRotation,
        owner->feedIn->getChestDirectionRequest(),
        chestDirectionWeight,
        desiredChestRotation,
        pDebugDrawInst,
        SCALING_SOURCE);
    }

    // Calculate and output the spine etc transforms
    NMP::Vector3 desiredChestTranslation = in->getSpinePoseEndRelativeToRoot(0).translation();
    NMP::Quat desiredChestQ = desiredChestRotation.toQuat();

    // Note that there is only ever one spine
    out->setChestRelPelvisOrientationAt(0, desiredChestQ,
      owner->feedIn->getSupportAmount() * in->getSpinePoseEndRelativeToRootImportance(0));
    out->setChestRelPelvisTranslationAt(0, desiredChestTranslation,
      owner->feedIn->getSupportAmount() * in->getSpinePoseEndRelativeToRootImportance(0));

    // heads relative to spine
    NMP::Quat desiredHeadEndQ;
    for (uint32_t iHead = 0; iHead < owner->owner->data->numHeads; ++iHead)
    {
      desiredHeadEndQ = in->getHeadPoseEndRelativeToRoot(iHead).toQuat();
      out->setHeadRelChestOrientationAt(iHead, desiredHeadEndQ,
        owner->feedIn->getSupportAmount() * in->getHeadPoseEndRelativeToRootImportance(iHead));
    }

    // When balancing use the CoM (rather than the balance pose), remove horizontal components. from
    // targetPelvisRelSupport. targetPelvisRelSupport up is defined as along y, and the support
    // transform is defined so that y up, x fwd, z right
    float balanceUsingAnimationHorizontalPelvisOffsetAmount = in->getBalanceUsingAnimationHorizontalPelvisOffsetAmount();
    targetPelvisRelSupport.translation().x *= balanceUsingAnimationHorizontalPelvisOffsetAmount;
    targetPelvisRelSupport.translation().z *= balanceUsingAnimationHorizontalPelvisOffsetAmount;
    // And add the balance offset
    targetPelvisRelSupport.translation().x += in->getBalanceOffsetFwd();
    targetPelvisRelSupport.translation().z += in->getBalanceOffsetRight();

    targetChestRelPelvis = desiredChestRotation;
    targetChestRelPelvis.translation() = desiredChestTranslation;

#if defined(MR_OUTPUT_DEBUGGING)
    {
      NMP::Matrix34 desiredPelvis = targetPelvisRelSupport * supportTM ;
      NMP::Matrix34 desiredChest =
        NMP::Matrix34(desiredChestRotation.toQuat(), desiredChestTranslation) * desiredPelvis;
      NMP::Matrix34 desiredHead = NMP::Matrix34(desiredHeadEndQ, NMP::Vector3(0, 0, 0)) * desiredChest;
      desiredHead.translation() = feedIn->getCurrHeadEndPositions(0);

      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, supportTM,     SCALE_DIST(1.0f));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredPelvis, SCALE_DIST(2.0f));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, owner->owner->data->spineLimbSharedStates[0].m_rootTM, SCALE_DIST(0.5f));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredChest,  SCALE_DIST(1.0f));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredHead,   SCALE_DIST(1.0f));
    }
#endif
  }

  // Always output the desired pelvis transform, since even when we're not balancing that needs to
  // be there ready for when we start
  out->setPelvisRelSupport(targetPelvisRelSupport);

  /// Output the supporting hands and feet positions relative to the support. They're needed for
  /// stepping, which can be running even when not balanced, so always need to be output.
  NMP::Matrix34 poseEndRelSupport;
  for (uint32_t i = 0 ; i < owner->owner->data->numArms ; ++i)
  {
    float imp = in->getArmPoseEndRelativeToRootImportance(i);
    poseEndRelSupport = in->getArmPoseEndRelativeToRoot(i) * targetChestRelPelvis * targetPelvisRelSupport;
    out->setArmPoseEndRelSupportAt(i, poseEndRelSupport, imp);
  }
  for (uint32_t i = 0 ; i < owner->owner->data->numLegs ; ++i)
  {
    float imp = in->getLegPoseEndRelativeToRootImportance(i);
    poseEndRelSupport = in->getLegPoseEndRelativeToRoot(i) * targetPelvisRelSupport;
    out->setLegPoseEndRelSupportAt(i, poseEndRelSupport, imp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void StaticBalance::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

