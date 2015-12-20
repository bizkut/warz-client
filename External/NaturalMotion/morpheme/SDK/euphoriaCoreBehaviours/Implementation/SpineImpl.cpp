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
#include "MyNetwork.h"
#include "SpinePackaging.h"
#include "Spine.h"
#include "euphoria/erDebugDraw.h"
#include "Helpers/LimbContact.h"

#define SCALING_SOURCE data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// SpineUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void SpineUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const SpineLimbSharedState& spineLimbState = owner->data->spineLimbSharedStates[data->childIndex];

  // hierarchically define stiffness parameters based on local data and on total stiffness of the
  // larger entity (the network)
  const float stiffnessMult = 1.0f - in->getLimbStiffnessReduction();
  const float dampingRatioMult = 1.0f - in->getLimbDampingRatioReduction();
  const float driveCompensationMult = 1.0f - in->getLimbControlCompensationReduction();
  data->normalStiffness = stiffnessMult * owner->data->normalStiffness;
  data->normalDampingRatio = dampingRatioMult * owner->data->normalDampingRatio;
  data->normalDriveCompensation = driveCompensationMult * owner->data->normalDriveCompensation;
  // While stepping loosen the spine to absorb wobble because the pelvis is not staying as stable
  // as it should. Gradually ramp the compensation up after stepping.
  if (in->getIsStepping())
  {
    data->timeSinceStep = 0.0f;
  }
  else
  {
    data->timeSinceStep += timeStep;
  }

  // This code ramps up the drive compensation after stepping stops
  float stepLoosenessTime = SCALE_TIME(1.0f);
  float frac = NMP::clampValue(data->timeSinceStep / stepLoosenessTime, 0.0f, 1.0f);
  data->normalDriveCompensation *= frac;

  // Stiffness actually slower/weaker then normalStiffnes because feet are on
  // the ground... he's not floating in space
  float stiffness = data->normalStiffness / 1.5f;
  MR_DEBUG_DRAW_VECTOR(
    pDebugDrawInst, MR::VT_AngleDelta,
    spineLimbState.m_endTM.translation(),
    in->getEndRotationRequest().rotation,
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getEndRotationRequest().debugControlAmounts)));
  MR_DEBUG_DRAW_VECTOR(
    pDebugDrawInst, MR::VT_Delta,
    spineLimbState.m_endTM.translation(),
    in->getEndTranslationRequest().translation,
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getEndTranslationRequest().debugControlAmounts)));

  // here we convert these requests into a post-processed format, passed through spine pose onto spine control.
  data->upperRotationRequest = in->getEndRotationRequest();
  data->upperRotationRequestImp = data->processEndRotation.processRequest(
    in->getEndRotationRequestImportance(),
    in->getEndRotationRequest().imminence,
    stiffness,
    in->getEndRotationRequest().passOnAmount,
    0, stiffness * in->getEndRotationRequest().stiffnessScale);

  data->lowerRotationRequest = in->getRootRotationRequest();
  data->lowerRotationRequestImp = data->processRootRotation.processRequest(
    in->getRootRotationRequestImportance(),
    in->getRootRotationRequest().imminence,
    stiffness,
    in->getRootRotationRequest().passOnAmount,
    0, stiffness * in->getRootRotationRequest().stiffnessScale);

  // this code reduces estimated flexibility if translation is in-line with
  // spine length.. since you can't really compress or stretch spine
  NMP::Vector3 spineDir = NMP::vNormalise(spineLimbState.m_endTM.translation() - spineLimbState.m_rootTM.translation());
  NMP::Vector3 translationDir = NMP::vNormalise(
    in->getEndTranslationRequest().translation - in->getRootTranslationRequest().translation);

  float translationInSpineDir = NMP::vDot(translationDir, spineDir);
  const float epsilon = 1e-5f;
  float spineTranslateWeight = 1.0f + epsilon - fabsf(translationInSpineDir); // 0 to 1 range
  // based on spine translation weight, calculate importance of each request
  // based on its imminence and stiffness of spine
  data->upperTranslationRequest = in->getEndTranslationRequest();
  data->upperTranslationRequestImp = data->processEndTranslation.processRequest(
    in->getEndTranslationRequestImportance(),
    in->getEndTranslationRequest().imminence,
    stiffness,
    in->getEndTranslationRequest().passOnAmount / spineTranslateWeight,
    0, stiffness * in->getEndTranslationRequest().stiffnessScale);
  data->lowerTranslationRequest = in->getRootTranslationRequest();
  data->lowerTranslationRequestImp = data->processRootTranslation.processRequest(
    in->getRootTranslationRequestImportance(),
    in->getRootTranslationRequest().imminence,
    stiffness,
    in->getRootTranslationRequest().passOnAmount / spineTranslateWeight,
    0, stiffness * in->getRootTranslationRequest().stiffnessScale);

  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, spineLimbState.m_endTM, SCALE_DIST(0.2f));
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, spineLimbState.m_rootTM, SCALE_DIST(0.2f));
  MR_DEBUG_DRAW_CONE_AND_DIAL(
    pDebugDrawInst,
    spineLimbState.m_reachLimit.m_transform.translation(),
    spineLimbState.m_reachLimit.getReachConeDir(),
    spineLimbState.m_reachLimit.m_angle,
    spineLimbState.m_reachLimit.m_distance,
    (spineLimbState.m_endTM.translation() - spineLimbState.m_basePosition).getNormalised(),
    NMP::Colour::RED);

}

//----------------------------------------------------------------------------------------------------------------------
// SpineFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void SpineFeedbackPackage::feedback(
  float timeStep,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const SpineLimbSharedState& spineLimbState = owner->data->spineLimbSharedStates[data->childIndex];

  // pass up the spine's default pose for use by the spine support in blending with static balance and shield desired
  // poses
  feedOut->setDesiredEndRotationRelRoot(
    owner->data->defaultPoseEndsRelativeToRoot[data->limbNetworkIndex].toQuat(), in->getDefaultPoseWeight());
  feedOut->setDesiredEndTranslationRelRoot(
    owner->data->defaultPoseEndsRelativeToRoot[data->limbNetworkIndex].translation(), in->getDefaultPoseWeight());
  // The spine adjusts whether the lower and upper are seen as supporting by whether the spine is constrained
  feedOut->setPelvisControlledAmount(
    NMP::minimum(in->getPelvisControlledAmount() + spineLimbState.m_externalSupportAmount, 1.0f));
  feedOut->setChestControlledAmount(
    NMP::minimum(in->getChestControlledAmount() + spineLimbState.m_externalSupportAmount, 1.0f));
  feedOut->setExternalSupportAmount(spineLimbState.m_externalSupportAmount, 1.0f);

  // Set state of the chest for use in higher level modules.
  {
    State& chest = feedOut->startChestStateModification();
    chest.matrix = spineLimbState.m_endTM;
    chest.velocity = spineLimbState.m_endData.velocity;
    chest.angularVelocity = spineLimbState.m_endData.angularVelocity;
    feedOut->stopChestStateModification();
  }

  // calculate length of spine
  data->spineLength = owner->data->defaultPoseLimbLengths[data->limbNetworkIndex];
  NMP::Vector3 pelvisToChestDir
    = NMP::vNormalise(spineLimbState.m_endTM.translation() - spineLimbState.m_rootTM.translation());

  // update spine contact feedback.
  bool isColliding = spineLimbState.m_mainPartIsColliding;

  updateLimbCollidingTime(isColliding, timeStep, data->collidingTime);
  updateLimbEndContactFeedback(*this, false, owner->data->spineLimbSharedStates[data->childIndex], timeStep, owner->data->collidingSupportTime, pDebugDrawInst);
  updateLimbRootContactFeedback(*this, false, owner->data->spineLimbSharedStates[data->childIndex], timeStep, owner->data->collidingSupportTime, pDebugDrawInst);

  feedOut->setInContact(isColliding);
  feedOut->setEndOnGroundAmount(data->endOnGroundAmount);
  feedOut->setRootOnGroundAmount(data->rootOnGroundAmount);

  // we are intentionally swapping around here
  NMP::Vector3 rootTargetRotation = in->getEndRotationRequest().rotation;
  NMP::Vector3 rootTargetTranslation = in->getEndTranslationRequest().translation;
  NMP::Vector3 endTargetRotation = in->getRootRotationRequest().rotation;
  NMP::Vector3 endTargetTranslation = in->getRootTranslationRequest().translation;

  // here we convert rotations on one end to requests at the other end of the
  // spine, so forwarding these requests on
  {
    // rotation for upper just matches that for lower...
    RotationRequest urRequest(endTargetRotation, SCALING_SOURCE);
    urRequest.stiffnessScale = in->getRootRotationRequest().stiffnessScale;
    urRequest.imminence = data->processRootRotation.imminence;
#if defined(MR_OUTPUT_DEBUGGING)
    urRequest.debugControlAmounts = in->getRootRotationRequest().debugControlAmounts;
#endif
    urRequest.passOnAmount = in->getRootRotationRequest().passOnAmount;
    feedOut->setUpstreamOnlyRotationRequest(urRequest, data->processRootRotation.importance);

    // rotation for lower just matches that for upper...
    RotationRequest drRequest(rootTargetRotation, SCALING_SOURCE);
    drRequest.stiffnessScale = in->getEndRotationRequest().stiffnessScale;
    drRequest.imminence = data->processEndRotation.imminence;
#if defined(MR_OUTPUT_DEBUGGING)
    drRequest.debugControlAmounts = in->getEndRotationRequest().debugControlAmounts;
#endif
    drRequest.passOnAmount = in->getEndRotationRequest().passOnAmount;
    feedOut->setDownstreamOnlyRotationRequest(drRequest, data->processEndRotation.importance);
  }

  // Here we convert position on one end to requests at the other end of the
  // spine. Currently just applies the necessary shifts, but possibly there is
  // some combination of pos and rotation to give better results
  {
    // position shift for upper is converted across to a shift on the upper
    TranslationRequest upRequest(endTargetTranslation, SCALING_SOURCE);
    upRequest.imminence = data->processRootTranslation.imminence;
    upRequest.stiffnessScale = in->getRootTranslationRequest().stiffnessScale;
#if defined(MR_OUTPUT_DEBUGGING)
    upRequest.debugControlAmounts = in->getRootTranslationRequest().debugControlAmounts;
#endif
    upRequest.passOnAmount = in->getRootTranslationRequest().passOnAmount;
    feedOut->setUpstreamOnlyTranslationRequest(upRequest, data->processRootTranslation.importance);

    // Here I double the translation request that is in the spine direction. This is because the spine cannot
    // move in this direction so more work needs to be done by the legs (in crouching basically)
    // I can't do this by increasing the importance for the root request, because it may also include lateral motion
    if (data->processEndTranslation.importance > 0.0f)
    {
      float alongSpine = rootTargetTranslation.dot(pelvisToChestDir);
      rootTargetTranslation += pelvisToChestDir * alongSpine;
    }
    // rotation for lower just matches that for upper...
    TranslationRequest dpRequest(rootTargetTranslation, SCALING_SOURCE);
#if defined(MR_OUTPUT_DEBUGGING)
    dpRequest.debugControlAmounts = in->getEndTranslationRequest().debugControlAmounts;
#endif
    dpRequest.stiffnessScale = in->getEndTranslationRequest().stiffnessScale;
    dpRequest.imminence = data->processEndTranslation.imminence;
    dpRequest.passOnAmount = in->getEndTranslationRequest().passOnAmount;
    feedOut->setDownstreamOnlyTranslationRequest(dpRequest, data->processEndTranslation.importance);
  }

  MR_DEBUG_DRAW_POINT(
    pDebugDrawInst, spineLimbState.m_dynamicState.getPosition(), SCALE_DIST(0.5f), NMP::Colour::YELLOW);
}

//----------------------------------------------------------------------------------------------------------------------
void Spine::entry()
{
  data->limbNetworkIndex = MyNetworkData::firstSpineNetworkIndex + getChildIndex();
  data->limbRigIndex = owner->data->firstSpineRigIndex + getChildIndex();
  data->childIndex = getChildIndex();
  data->dimensionalScaling = owner->data->dimensionalScaling;
  data->timeSinceStep = SCALE_TIME(100.0f);
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

