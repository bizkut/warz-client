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
#include "AnimateBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/AnimateBehaviour.h"
#include "NetworkDescriptor.h"
#include "Helpers/Animate.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

static void outputLimbTargetsWithVelocity(AnimateBehaviourInterface& abi,
  NMP::Matrix34* previousPoseInputs,
  float* previousPoseImportances,
  const AnimateBehaviourData& params,
  const float timeStep,
  const MyNetwork* networkRoot,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const ER::DimensionalScaling&   MR_OUTPUT_DEBUG_ARG(dimensionalScaling))
{
  const BodyPoseData& bp = params.getBodyPose();

  PoseData poseData;
  poseData.driveCompensationScale = 1.0f;

  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < networkRoot->data->numArms ; ++i)
  {
    uint32_t index = networkRoot->data->firstArmNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getArmStrengthCP(i);
    poseData.gravityCompensation = params.getArmGravityCompensation(i);
    // Use the strength as a weight as it's quite intuitive for animate
    float imp = NMP::clampValue(poseData.stiffnessScale, 0.0f, 1.0f);
    if (imp > 0.0f && previousPoseImportances[index] > 0.0f)
    {
      poseData.poseEndRelativeToRootVelocity =
        (poseData.poseEndRelativeToRoot.translation() - previousPoseInputs[index].translation()) / timeStep;
    }
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, poseData.poseEndRelativeToRoot, dimensionalScaling.scaleDist(0.1f));
    previousPoseInputs[index] = poseData.poseEndRelativeToRoot;
    previousPoseImportances[index] = imp;
    if (params.getArmsPriority())
      abi.out->setArmPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setArmPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < networkRoot->data->numHeads ; ++i)
  {
    uint32_t index = networkRoot->data->firstHeadNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getHeadStrengthCP(i);
    poseData.gravityCompensation = params.getHeadGravityCompensation(i);
    // Use the strength as a weight as it's quite intuitive for animate
    float imp = NMP::clampValue(poseData.stiffnessScale, 0.0f, 1.0f);
    if (imp > 0.0f && previousPoseImportances[index] > 0.0f)
    {
      poseData.poseEndRelativeToRootVelocity =
        (poseData.poseEndRelativeToRoot.translation() - previousPoseInputs[index].translation()) / timeStep;
    }
    previousPoseInputs[index] = poseData.poseEndRelativeToRoot;
    previousPoseImportances[index] = imp;
    if (params.getHeadsPriority())
      abi.out->setHeadPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setHeadPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < networkRoot->data->numLegs ; ++i)
  {
    uint32_t index = networkRoot->data->firstLegNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getLegStrengthCP(i);
    poseData.gravityCompensation = params.getLegGravityCompensation(i);
    // Use the strength as a weight as it's quite intuitive for animate
    float imp = NMP::clampValue(poseData.stiffnessScale, 0.0f, 1.0f);
    if (imp > 0.0f && previousPoseImportances[index] > 0.0f)
    {
      poseData.poseEndRelativeToRootVelocity =
        (poseData.poseEndRelativeToRoot.translation() - previousPoseInputs[index].translation()) / timeStep;
    }
    previousPoseInputs[index] = poseData.poseEndRelativeToRoot;
    previousPoseImportances[index] = imp;
    if (params.getLegsPriority())
      abi.out->setLegPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setLegPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < networkRoot->data->numSpines ; ++i)
  {
    uint32_t index = networkRoot->data->firstSpineNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getSpineStrengthCP(i);
    poseData.gravityCompensation = params.getSpineGravityCompensation(i);
    // Use the strength as a weight as it's quite intuitive for animate
    float imp = NMP::clampValue(poseData.stiffnessScale, 0.0f, 1.0f);
    if (imp > 0.0f && previousPoseImportances[index] > 0.0f)
    {
      poseData.poseEndRelativeToRootVelocity =
        (poseData.poseEndRelativeToRoot.translation() - previousPoseInputs[index].translation()) / timeStep;
    }
    previousPoseInputs[index] = poseData.poseEndRelativeToRoot;
    previousPoseImportances[index] = imp;
    if (params.getSpinesPriority())
      abi.out->setSpinePoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setSpinePoseLowPriorityAt(i, poseData, imp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void outputLimbTargets(
  AnimateBehaviourInterface& abi,
  const AnimateBehaviourData& params,
  const MyNetwork* networkRoot)
{
  const BodyPoseData& bp = params.getBodyPose();

  PoseData poseData;
  poseData.driveCompensationScale = 1.0f;

  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < networkRoot->data->numArms ; ++i)
  {
    uint32_t index = networkRoot->data->firstArmNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getArmStrengthCP(i);
    poseData.gravityCompensation = params.getArmGravityCompensation(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getArmsPriority())
      abi.out->setArmPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setArmPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < networkRoot->data->numHeads ; ++i)
  {
    uint32_t index = networkRoot->data->firstHeadNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getHeadStrengthCP(i);
    poseData.gravityCompensation = params.getHeadGravityCompensation(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getHeadsPriority())
      abi.out->setHeadPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setHeadPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < networkRoot->data->numLegs ; ++i)
  {
    uint32_t index = networkRoot->data->firstLegNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getLegStrengthCP(i);
    poseData.gravityCompensation = params.getLegGravityCompensation(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getLegsPriority())
      abi.out->setLegPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setLegPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < networkRoot->data->numSpines ; ++i)
  {
    uint32_t index = networkRoot->data->firstSpineNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    poseData.stiffnessScale = params.getSpineStrengthCP(i);
    poseData.gravityCompensation = params.getSpineGravityCompensation(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getSpinesPriority())
      abi.out->setSpinePoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setSpinePoseLowPriorityAt(i, poseData, imp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void outputCachedLimbTargets(
  const AnimateBehaviourData& params,
  AnimateBehaviourInterface& abi,
  const MyNetwork* networkRoot)
{
  (void) networkRoot; // Without this compiler generates incorrect warning about networkRoot being unused
  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < abi.owner->data->numArms ; ++i)
  {
    uint32_t index = networkRoot->data->firstArmNetworkIndex + i;
    PoseData& poseData = abi.data->cachedPose[index];
    poseData.stiffnessScale = params.getArmStrengthCP(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getArmsPriority())
      abi.out->setArmPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setArmPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < abi.owner->data->numHeads ; ++i)
  {
    uint32_t index = networkRoot->data->firstHeadNetworkIndex + i;
    PoseData& poseData = abi.data->cachedPose[index];
    poseData.stiffnessScale = params.getHeadStrengthCP(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getHeadsPriority())
      abi.out->setHeadPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setHeadPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < abi.owner->data->numLegs ; ++i)
  {
    uint32_t index = networkRoot->data->firstLegNetworkIndex + i;
    PoseData& poseData = abi.data->cachedPose[index];
    poseData.stiffnessScale = params.getLegStrengthCP(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getLegsPriority())
      abi.out->setLegPoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setLegPoseLowPriorityAt(i, poseData, imp);
  }

  for (uint32_t i = 0 ; i < abi.owner->data->numSpines ; ++i)
  {
    uint32_t index = networkRoot->data->firstSpineNetworkIndex + i;
    PoseData& poseData = abi.data->cachedPose[index];
    poseData.stiffnessScale = params.getSpineStrengthCP(i);
    float imp = poseData.stiffnessScale; // Use the strength as a weight as it's quite intuitive for animate
    if (params.getSpinesPriority())
      abi.out->setSpinePoseHighPriorityAt(i, poseData, imp);
    else
      abi.out->setSpinePoseLowPriorityAt(i, poseData, imp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimateBehaviourInterface::update(float timeStep)
{
#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
  MR::InstanceDebugInterface* pDebugDrawInst = rootModule->getDebugInterface();
  MR_DEBUG_MODULE_ENTER(pDebugDrawInst, getClassName());
#else
  MR::InstanceDebugInterface* pDebugDrawInst = 0;
#endif

  const AnimateBehaviourData& params =
    ((AnimateBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Animate)))->getParams();

  AnimatePoseRequirements& poseRequirements =
    ((AnimateBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Animate)))->getPoseRequirements();

  float cachedPoseImportance[NetworkConstants::networkMaxNumLimbs]; // we don't actually use this, but need to pass it in
  out->setSleepingDisabled(params.getDisableSleeping());

  if (params.getUseSingleFrameForPose())
  {
    if (poseRequirements.enablePoseInput_BodyPose)
    {
      Animate::cacheLimbTargets(data->cachedPose, cachedPoseImportance,
        params.getBodyPose(), 1.0f, 1.0f, owner);
      poseRequirements.enablePoseInput_BodyPose = false;
    }
    outputCachedLimbTargets(params, *this, owner);
  }
  else
  {
    if (!poseRequirements.enablePoseInput_BodyPose)
    {
      poseRequirements.enablePoseInput_BodyPose = true;
      return;
    }
    outputLimbTargetsWithVelocity(
      *this, data->previousPoseInputs, data->previousPoseImportances,
      params, timeStep, owner, pDebugDrawInst, SCALING_SOURCE);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimateBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
}

//----------------------------------------------------------------------------------------------------------------------
void AnimateBehaviourInterface::entry()
{
  // Reset this in case it was previously set to false (doesn't get cleared)
  AnimatePoseRequirements& poseRequirements =
    ((AnimateBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Animate)))->getPoseRequirements();
  poseRequirements.enablePoseInput_BodyPose = true;

  for (uint32_t i = 0 ; i < NetworkConstants::networkMaxNumLimbs ; ++i)
  {
    data->previousPoseInputs[i].identity();
    data->previousPoseImportances[i] = 0.0f;
  }
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
