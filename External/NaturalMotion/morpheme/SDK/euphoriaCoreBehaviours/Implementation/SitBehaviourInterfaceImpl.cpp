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
#include "MyNetwork.h"
#include "Behaviours/SitBehaviour.h"
#include "SitBehaviourInterface.h"
#include "NetworkDescriptor.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
static void outputLimbTargets(
  const SitBehaviourData& params,
  const NMP::Matrix34* const poseEndRelativeToRoots,
  const float* const poseWeights,
  SitBehaviourInterface& sbi)
{
  // Here we assume that the incoming data is in the order arms, heads, spine,
  // legs arms. Also... note that the poses and the weights use the network indexing.
  const float sitAmount = sbi.feedIn->getSitAmount();
  const float minSitAmount = params.getMinSitAmount();

  if (sitAmount > minSitAmount)
  {
    const float minPoseImportance = 0.00001f;

    // arms
    for (uint32_t i = 0; i < sbi.owner->data->numArms; ++i)
    {
      const uint32_t index = sbi.owner->data->firstArmNetworkIndex + i;
      const float poseWeight = poseWeights[index];
      const float importance = NMP::maximum(poseWeight, minPoseImportance);
      sbi.out->setArmPoseEndRelativeToRootAt(i, poseEndRelativeToRoots[index], importance);
    }
    // legs
    for (uint32_t i = 0; i < sbi.owner->data->numLegs; ++i)
    {
      const uint32_t index = sbi.owner->data->firstLegNetworkIndex + i;
      const float importance = NMP::maximum(poseWeights[index], minPoseImportance);
      sbi.out->setLegPoseEndRelativeToRootAt(i, poseEndRelativeToRoots[index], importance);
    }
    // spine
    for (uint32_t i = 0; i < sbi.owner->data->numSpines; ++i)
    {
      const uint32_t index = sbi.owner->data->firstSpineNetworkIndex + i;
      const float importance = NMP::maximum(poseWeights[index], minPoseImportance);
      sbi.out->setSpinePoseEndRelativeToRootAt(i, poseEndRelativeToRoots[index], importance);
    }
    // head
    for (uint32_t i = 0; i < sbi.owner->data->numHeads; ++i)
    {
      const uint32_t index = sbi.owner->data->firstHeadNetworkIndex + i;
      const float importance = NMP::maximum(poseWeights[index], minPoseImportance);
      sbi.out->setHeadPoseEndRelativeToRootAt(i, poseEndRelativeToRoots[index], importance);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void cacheLimbTargets(
  NMP::Matrix34* const poseEndRelativeToRoots,
  float* const poseWeights,
  const BodyPoseData& bodyPose)
{
  for (uint32_t i = 0 ; i < NetworkMetrics::numLimbs; ++i)
  {
    poseEndRelativeToRoots[i] = bodyPose.m_poseEndRelativeToRoots[i];
    poseWeights[i] = bodyPose.m_poseWeights[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  SitBehaviour* const sitBehaviour =
    static_cast<SitBehaviour*>(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Sit));
  NMP_ASSERT(sitBehaviour);

  const SitBehaviourData& params = sitBehaviour->getParams();

  if (params.getUseSingleFrameForPose())
  {
    SitPoseRequirements& poseRequirements = sitBehaviour->getPoseRequirements();

    // On the first frame only cache the pose if desired. The flags in poseRequirements tells
    // morpheme whether or not to process the incoming animations.
    if (poseRequirements.enablePoseInput_Pose)
    {
      cacheLimbTargets(data->cachedSitPoseEndRelativeToRoots, data->cachedSitPoseWeights, params.getPose());

      poseRequirements.enablePoseInput_Pose = false;
    }

    // Output cached pose.
    outputLimbTargets(params, data->cachedSitPoseEndRelativeToRoots, data->cachedSitPoseWeights, *this);
  }
  else
  {
    outputLimbTargets(params, params.getPose().m_poseEndRelativeToRoots, params.getPose().m_poseWeights, *this);
  }

  out->setSupportWithArms(params.getSupportWithArms());
  out->setSupportWithLegs(true);
  out->setSupportWithSpines(true);
  out->setLegsSwingWeight(0.0f, feedIn->getSitAmount());

  SitParameters sitParameters;
  sitParameters.armStepHeight = SCALE_DIST(params.getArmStepHeight());
  sitParameters.armStepTargetExtrapolationTime = SCALE_TIME(params.getArmStepTargetExtrapolationTime());
  sitParameters.armStepTargetSeparationFromBody = SCALE_DIST(params.getArmStepTargetSeparationFromBody());
  sitParameters.minSitAmount = params.getMinSitAmount();
  sitParameters.minStandingBalanceAmount = params.getMinStandingBalanceAmount();
  out->setSitParameters(sitParameters);
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourInterface::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
