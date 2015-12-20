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
#include "IdleAwakeBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/IdleAwakeBehaviour.h"
#include "NetworkDescriptor.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface::update(float timeStep)
{
  const IdleAwakeBehaviourData& params =
    ((IdleAwakeBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_IdleAwake)))->getParams();
  IdleAwakePoseRequirements& poseRequirements = ((IdleAwakeBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_IdleAwake)))->getPoseRequirements();

  // We behave differently depending on whether we're standing/supported, or if we've fallen.
  bool isSupported = true;
  if (feedIn->getSupportAmount() == 0.0f && feedIn->getLegsInContact() && feedIn->getArmOrLegSupportAmount() == 0.0f &&
    (feedIn->getSpineInContact() || feedIn->getHeadsInContact() || feedIn->getArmsInContact()))
    isSupported = false;

  // Looking
  float lookAmount = 0.0f;
  if (isSupported)
    lookAmount = params.getLookStrengthWhenSupported();
  else
    lookAmount = params.getLookStrengthWhenUnsupported();

  if (lookAmount > 0.0f)
  {
    RandomLookParameters randomLookParameters;
    randomLookParameters.lookAmount = lookAmount;
    randomLookParameters.lookRangeSideways = NMP::degreesToRadians(params.getLookRangeSideways());
    randomLookParameters.lookRangeUpDown = NMP::degreesToRadians(params.getLookRangeUpDown());
    randomLookParameters.lookTimescale = params.getLookTimescale();
    randomLookParameters.lookWithWholeBody = params.getWholeBodyLookCP();
    randomLookParameters.lookTransitionTime = SCALE_TIME(params.getLookTransitionTime());
    randomLookParameters.lookFocusDistance = SCALE_DIST(params.getLookFocusDistance());
    randomLookParameters.lookVerticalOffset = SCALE_DIST(params.getLookVerticalOffset());
    out->setRandomLookParameters(randomLookParameters);
  }

  // Modify the standing stance over time
  if (params.getStanceChangeTime() > 0.0f)
  {
    if (feedIn->getIsStepping())
    {
      data->balanceOffsetFwd = 0.0f;
      data->balanceOffsetRight = 0.0f;
      data->balanceOffsetFwdRate = 0.0f;
      data->balanceOffsetRightRate = 0.0f;
      data->timeSinceLastOffsetTarget = 0.0f;
      data->balanceOffsetFwdTarget = 0.0f;
      data->balanceOffsetRightTarget = 0.0f;
    }
    else
    {
      float driftTime = SCALE_TIME(params.getStanceChangeTime());
      if (data->timeSinceLastOffsetTarget > driftTime)
      {
        float fwd = SCALE_DIST(params.getFwdRange());
        float back = SCALE_DIST(params.getBackRange());
        float left = SCALE_DIST(params.getLeftRange());
        float right = SCALE_DIST(params.getRightRange());

        data->balanceOffsetFwdTarget = data->rng.genFloat(-back, fwd);
        data->balanceOffsetRightTarget = data->rng.genFloat(-left, right);

        data->timeSinceLastOffsetTarget = 0.0f;
      }
      data->timeSinceLastOffsetTarget += timeStep;

      // Rather arbitrary scaling here, to make the change in pose happen faster than the change in pose target
      float smoothTime = SCALE_TIME(params.getPoseAdjustTime());
      NMP::SmoothCD(
        data->balanceOffsetFwd, data->balanceOffsetFwdRate, timeStep, data->balanceOffsetFwdTarget, smoothTime);
      NMP::SmoothCD(
        data->balanceOffsetRight, data->balanceOffsetRightRate, timeStep, data->balanceOffsetRightTarget, smoothTime);

      out->setBalanceOffsetFwd(data->balanceOffsetFwd);
      out->setBalanceOffsetRight(data->balanceOffsetRight);
    }
  }

  // Pose when fallen
  const BodyPoseData* bp = &params.getFallenPose();
  if (!isSupported && bp->m_poseWeight != 0.0f)
  {
    // basically lying down - get into a sensible pose
    // on back or front?
    if (!poseRequirements.enablePoseInput_FallenPose)
    {
      poseRequirements.enablePoseInput_FallenPose = true;
      // wait until the next update before using it
    }
    else
    {
      PoseData poseData;
      poseData.dampingRatioScale = 2.0f; // reduce floppiness

      // apply some pose control
      // Here we assume that the incoming data is in the order arms, heads, spine, legs
      // arms. Also... note that the poses and the weights use the network indexing.
      float importance = 1.0f;
      for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
      {
        uint32_t index = owner->data->firstArmNetworkIndex + i;
        if (bp->m_poseWeights[index] > 0.0f)
        {
          poseData.poseEndRelativeToRoot = bp->m_poseEndRelativeToRoots[index];
          poseData.stiffnessScale = bp->m_poseWeights[index];
          poseData.driveCompensationScale = bp->m_poseWeights[index]; // reduce any artefacts at low strength
          out->setArmPoseAt(i, poseData, importance);
        }
      }
      // heads
      for (uint32_t i = 0 ; i < owner->data->numHeads ; ++i)
      {
        uint32_t index = owner->data->firstHeadNetworkIndex + i;
        if (bp->m_poseWeights[index] > 0.0f)
        {
          poseData.poseEndRelativeToRoot = bp->m_poseEndRelativeToRoots[index];
          poseData.stiffnessScale = bp->m_poseWeights[index];
          poseData.driveCompensationScale = bp->m_poseWeights[index]; // reduce any artefacts at low strength
          out->setHeadPoseAt(i, poseData, importance);
        }
      }
      // legs
      for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
      {
        uint32_t index = owner->data->firstLegNetworkIndex + i;
        if (bp->m_poseWeights[index] > 0.0f)
        {
          poseData.poseEndRelativeToRoot = bp->m_poseEndRelativeToRoots[index];
          poseData.stiffnessScale = bp->m_poseWeights[index];
          poseData.driveCompensationScale = bp->m_poseWeights[index]; // reduce any artefacts at low strength
          out->setLegPoseAt(i, poseData, importance);
        }
      }
      // spine
      for (uint32_t i = 0 ; i < owner->data->numSpines ; ++i)
      {
        uint32_t index = owner->data->firstSpineNetworkIndex + i;
        if (bp->m_poseWeights[index] > 0.0f)
        {
          poseData.poseEndRelativeToRoot = bp->m_poseEndRelativeToRoots[index];
          poseData.stiffnessScale = bp->m_poseWeights[index];
          poseData.driveCompensationScale = bp->m_poseWeights[index]; // reduce any artefacts at low strength
          out->setSpinePoseAt(i, poseData, importance);
        }
      }
    }
  }
  else
  {
    poseRequirements.enablePoseInput_FallenPose = false;

    if (feedIn->getLegsInContact() ||  feedIn->getSpineInContact() ||
      feedIn->getArmsInContact() || feedIn->getHeadsInContact())
    {
      // If supported and in some kind of contact - don't go completely limp
      out->setUseSpineDefaultPose(1.0f);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
}

//----------------------------------------------------------------------------------------------------------------------
void IdleAwakeBehaviourInterface::entry()
{
  IdleAwakePoseRequirements& poseRequirements = ((IdleAwakeBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_IdleAwake)))->getPoseRequirements();
  poseRequirements.enablePoseInput_FallenPose = false;

  data->balanceOffsetFwd = 0.0f;
  data->balanceOffsetRight = 0.0f;
  data->balanceOffsetFwdRate = 0.0f;
  data->balanceOffsetRightRate = 0.0f;
  data->timeSinceLastOffsetTarget = 0.0f;
  data->balanceOffsetFwdTarget = 0.0f;
  data->balanceOffsetRightTarget = 0.0f;
  data->rng.setSeed(owner->getAndBumpNetworkRandomSeed());
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

