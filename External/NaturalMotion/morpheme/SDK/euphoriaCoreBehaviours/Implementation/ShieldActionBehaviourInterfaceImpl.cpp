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
#include "MyNetwork.h"
#include "ShieldActionBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/ShieldActionBehaviour.h"
#include "euphoria/erDebugDraw.h"
#include "Helpers/Animate.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
static void outputLimbTargets(
  ShieldActionBehaviourInterface& bi,
  const ShieldActionBehaviourData& params,
  const float upperBodyImp,
  const float lowerBodyImp)
{
  const BodyPoseData& fp = params.getShieldPose();

  PoseData poseData;
  poseData.stiffnessScale = 1.0f;
  poseData.driveCompensationScale = 1.0f;

  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < bi.owner->data->numArms ; ++i)
  {
    uint32_t index = bi.owner->data->firstArmNetworkIndex + i;
    poseData.poseEndRelativeToRoot = fp.m_poseEndRelativeToRoots[index];
    bi.out->setArmPoseAt(i, poseData, upperBodyImp * fp.m_poseWeights[index]);
  }
  // heads
  for (uint32_t i = 0 ; i < bi.owner->data->numHeads ; ++i)
  {
    uint32_t index = bi.owner->data->firstHeadNetworkIndex + i;
    poseData.poseEndRelativeToRoot = fp.m_poseEndRelativeToRoots[index];
    bi.out->setHeadPoseAt(i, poseData, upperBodyImp * fp.m_poseWeights[index]);
  }
  // legs
  for (uint32_t i = 0 ; i < bi.owner->data->numLegs ; ++i)
  {
    uint32_t index = bi.owner->data->firstLegNetworkIndex + i;
    poseData.poseEndRelativeToRoot = fp.m_poseEndRelativeToRoots[index];
    bi.out->setLegPoseAt(i, poseData, lowerBodyImp * fp.m_poseWeights[index]);
  }
  // spine
  for (uint32_t i = 0 ; i < bi.owner->data->numSpines ; ++i)
  {
    uint32_t index = bi.owner->data->firstSpineNetworkIndex + i;
    poseData.poseEndRelativeToRoot = fp.m_poseEndRelativeToRoots[index];
    bi.out->setSpinePoseAt(i, poseData, lowerBodyImp * fp.m_poseWeights[index]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void outputCachedLimbTargets(
  ShieldActionBehaviourInterface& bi,
  const float upperBodyImp,
  const float lowerBodyImp)
{
  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < bi.owner->data->numArms ; ++i)
  {
    uint32_t index = bi.owner->data->firstArmNetworkIndex + i;
    bi.out->setArmPoseAt(i, bi.data->cachedPose[index], upperBodyImp * bi.data->cachedPoseImportance[index]);
  }
  // heads
  for (uint32_t i = 0 ; i < bi.owner->data->numHeads ; ++i)
  {
    uint32_t index = bi.owner->data->firstHeadNetworkIndex + i;
    bi.out->setHeadPoseAt(i, bi.data->cachedPose[index], upperBodyImp * bi.data->cachedPoseImportance[index]);
  }
  // legs
  for (uint32_t i = 0 ; i < bi.owner->data->numLegs ; ++i)
  {
    uint32_t index = bi.owner->data->firstLegNetworkIndex + i;
    bi.out->setLegPoseAt(i, bi.data->cachedPose[index], lowerBodyImp * bi.data->cachedPoseImportance[index]);
  }
  // spine
  for (uint32_t i = 0 ; i < bi.owner->data->numSpines ; ++i)
  {
    uint32_t index = bi.owner->data->firstSpineNetworkIndex + i;
    bi.out->setSpinePoseAt(i, bi.data->cachedPose[index], lowerBodyImp * bi.data->cachedPoseImportance[index]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
#endif
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());

  const ShieldActionBehaviourData& params =
    ((ShieldActionBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ShieldAction)))->getParams();

  ShieldActionPoseRequirements& poseRequirements =
    ((ShieldActionBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ShieldAction)))->getPoseRequirements();

  if (params.getUseSingleFrameForShieldPose())
  {
    Animate::cacheLimbTargets(data->cachedPose, data->cachedPoseImportance,
      params.getShieldPose(), 1.0f, 1.0f, owner);
    poseRequirements.enablePoseInput_ShieldPose = false;
  }

  float importance(NMP::clampValue(params.getImportanceCP(), 0.0f, 1.0f));

  if (importance == 0.0f)
  {
    return;
  }

  out->setSmoothReturnTP(SCALE_TIME(params.getSmoothReturnTimePeriod()), importance);
  out->setTurnAwayScale(params.getTurnAwayScaleCP(), importance);
  out->setIsRunning(true);

  float upperBodyImp = feedIn->getDoShieldWithUpperBody() * importance;
  float lowerBodyImp = feedIn->getDoShieldWithLowerBody() * importance;

  // Work out direction of the hazard from the position.
  const NMP::Vector3 hazardPosWS = params.getHazardPosCP();
  NMP::Vector3 shieldActionDirectionToHazardWS = hazardPosWS - feedIn->getCurrPelvisPartTM().translation();
  shieldActionDirectionToHazardWS.fastNormalise();
  out->setShieldActionDirectionToHazard(shieldActionDirectionToHazardWS, importance * 0.999f);

  MR_DEBUG_DRAW_POINT(rootModule->getDebugInterface(),
    hazardPosWS, SCALE_DIST(0.5f) * importance, NMP::Colour::BLACK);

  if (params.getUseSingleFrameForShieldPose())
  {
    if (poseRequirements.enablePoseInput_ShieldPose)
    {
      Animate::cacheLimbTargets(data->cachedPose, data->cachedPoseImportance,
        params.getShieldPose(), 1.0f, 1.0f, owner);
      poseRequirements.enablePoseInput_ShieldPose = false;
    }
    outputCachedLimbTargets(*this, upperBodyImp, lowerBodyImp);
  }
  else
  {
    if (!poseRequirements.enablePoseInput_ShieldPose)
    {
      poseRequirements.enablePoseInput_ShieldPose = true;
      return;
    }
    outputLimbTargets(*this, params, upperBodyImp, lowerBodyImp);
  }

  // Send parameters to do a default pitch. 
  BalancePoseParameters poseParameters;
  poseParameters.pitch = NMP::degreesToRadians(params.getDefaultPitchAngle());
  poseParameters.pelvisMultiplier = 0.3f; // Use a bit pelvis.
  poseParameters.spineMultiplier = 1.0f; // Most of the pitch from the spine
  poseParameters.headMultiplier = 0.0f; // Head is animated.
  poseParameters.crouchAmount = params.getCrouchAmount();

  importance *= feedIn->getHazardAngleImportance();
  out->setPoseParameters(poseParameters, importance);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  ShieldActionBehaviour* behaviour = ((ShieldActionBehaviour*)owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ShieldAction));
  ShieldActionBehaviourData& params = behaviour->getParams();

  params.setHazardAngleOCP(NMP::radiansToDegrees(feedIn->getHazardAngle()));
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldActionBehaviourInterface::entry()
{
  // Reset this in case it was previously set to false (doesn't get cleared)
  ShieldActionPoseRequirements& poseRequirements = ((ShieldActionBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ShieldAction)))->getPoseRequirements();

  poseRequirements.enablePoseInput_ShieldPose = true;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

