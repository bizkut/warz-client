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
#include "NetworkDescriptor.h"
#include "ShieldBehaviourInterface.h"
#include "Behaviours/ShieldBehaviour.h"
#include "Helpers/Animate.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
static void outputLimbTargets(
  ShieldBehaviourInterface& bi,
  const BodyPoseData& bp,
  const float upperBodyImp,
  const float lowerBodyImp)
{
  PoseData poseData;
  poseData.stiffnessScale = 1.0f;
  poseData.driveCompensationScale = 1.0f;

  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < bi.owner->data->numArms ; ++i)
  {
    uint32_t index = bi.owner->data->firstArmNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    bi.out->setArmPoseAt(i, poseData, upperBodyImp * bp.m_poseWeights[index]);
  }
  // heads
  for (uint32_t i = 0 ; i < bi.owner->data->numHeads ; ++i)
  {
    uint32_t index = bi.owner->data->firstHeadNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    bi.out->setHeadPoseAt(i, poseData, upperBodyImp * bp.m_poseWeights[index]);
  }
  // legs
  for (uint32_t i = 0 ; i < bi.owner->data->numLegs ; ++i)
  {
    uint32_t index = bi.owner->data->firstLegNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    bi.out->setLegPoseAt(i, poseData, lowerBodyImp * bp.m_poseWeights[index]);
  }
  // spine
  for (uint32_t i = 0 ; i < bi.owner->data->numSpines ; ++i)
  {
    uint32_t index = bi.owner->data->firstSpineNetworkIndex + i;
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    bi.out->setSpinePoseAt(i, poseData, lowerBodyImp * bp.m_poseWeights[index]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void outputCachedLimbTargets(
  ShieldBehaviourInterface& bi,
  const PoseData* cachedPose,
  const float* cachedPoseImportance,
  const float upperBodyImp,
  const float lowerBodyImp)
{
  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < bi.owner->data->numArms ; ++i)
  {
    uint32_t index = bi.owner->data->firstArmNetworkIndex + i;
    bi.out->setArmPoseAt(i, cachedPose[index], upperBodyImp * cachedPoseImportance[index]);
  }
  // heads
  for (uint32_t i = 0 ; i < bi.owner->data->numHeads ; ++i)
  {
    uint32_t index = bi.owner->data->firstHeadNetworkIndex + i;
    bi.out->setHeadPoseAt(i, cachedPose[index], upperBodyImp * cachedPoseImportance[index]);
  }
  // legs
  for (uint32_t i = 0 ; i < bi.owner->data->numLegs ; ++i)
  {
    uint32_t index = bi.owner->data->firstLegNetworkIndex + i;
    bi.out->setLegPoseAt(i, cachedPose[index], lowerBodyImp * cachedPoseImportance[index]);
  }
  // spine
  for (uint32_t i = 0 ; i < bi.owner->data->numSpines ; ++i)
  {
    uint32_t index = bi.owner->data->firstSpineNetworkIndex + i;
    bi.out->setSpinePoseAt(i, cachedPose[index], lowerBodyImp * cachedPoseImportance[index]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  // Suppress shield, if feedIn->suppress is set or an arm is constrained
  if (feedIn->getSuppress() )
  {
    return;
  }
  for (uint32_t i = 0; i < owner->data->numArms; ++i)
  {
    if (owner->data->armLimbSharedStates[i].m_isConstrained)
    {
      return;
    }
  }
  

  const ShieldBehaviourData& params =
    ((ShieldBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Shield)))->getParams();

  const float revsToRadians = 2.0f * NM_PI;
  // Send parameters
  ShieldParameters shieldParameters;
  shieldParameters.impactResponseSpeedForShield = SCALE_VEL(params.getImpactResponseForShield());
  shieldParameters.angularSpeedForShield = SCALE_ANGVEL(params.getAngularSpeedForShield()*revsToRadians);
  shieldParameters.tangentialSpeedForShield = SCALE_VEL(params.getTangentialSpeedForShield());
  out->setShieldParameters(shieldParameters);

  out->setSmoothReturnTP(SCALE_TIME(params.getSmoothReturnTimePeriod()));

  // Depending of the presence of a hazard and of its orientation, we send transform matrices
  // from the different animation poses. Note that even if bp.m_poseWeight is 0, morpheme runtime
  // will pass us a default pose, so we guarantee to output a pose, though the importance may be 0,
  // even on the leg/spine poses.
  ShieldPoseRequirements& poseRequirements =
    ((ShieldBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Shield)))->getPoseRequirements();

  // If character is supported, the importance of the animation for the lower body will always be 0.
  float upperBodyImp = feedIn->getDoShieldWithUpperBody();
  float lowerBodyImp = feedIn->getDoShieldWithLowerBody();

  bool hazardous = feedIn->getYawAngleImportance() > 0.0f;

  PoseData poseData;
  poseData.stiffnessScale = 1.0f;
  poseData.driveCompensationScale = feedIn->getDriveCompensationScale();

  // Build struct parameters to do a default pitch. Divided between pelvis, spine and head.
  BalancePoseParameters poseParameters;
  poseParameters.pitch = NMP::degreesToRadians(params.getDefaultPitchAngle());
  poseParameters.pelvisMultiplier = 1.0f;
  poseParameters.spineMultiplier = 1.0f;
  poseParameters.headMultiplier = 1.0f;
  poseParameters.crouchAmount = params.getCrouchAmount();

  // On the first frame only cache the shield pose if so desired. The flags in poseRequirements tells
  // morpheme whether or not to process the incoming animations.
  if (params.getUseSingleFrameForShieldPoses())
  {
    if (poseRequirements.enablePoseInput_ShieldPose)
    {
      Animate::cacheLimbTargets(data->shieldPose, data->shieldPoseImportance,
        params.getShieldPose(), 1.0f, 1.0f, owner);

      poseRequirements.enablePoseInput_ShieldPose = false;
    }
  }

  if (hazardous)
  {
    if (params.getUseSingleFrameForShieldPoses())
    {
      outputCachedLimbTargets(*this, data->shieldPose, data->shieldPoseImportance,
        upperBodyImp, lowerBodyImp);
    }
    else
    {
      const BodyPoseData& dp = params.getShieldPose();
      outputLimbTargets(*this, dp, upperBodyImp, lowerBodyImp);
    }
    out->setTurnAwayScale(params.getTurnAwayScaleCP(), 1.0f);
    out->setPoseParameters(poseParameters, 1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  bool isShielding = feedIn->getYawAngleImportance() > 0.0f;
  feedOut->setIsShielding(isShielding);

  ShieldBehaviour* behaviour = ((ShieldBehaviour*)owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Shield));
  ShieldBehaviourData& params = behaviour->getParams();

  params.setIsShieldingOCP(isShielding ? 1.0f : 0.0f);
  params.setHazardAngleOCP(NMP::radiansToDegrees(feedIn->getHazardAngle()));
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourInterface::entry()
{
  // Reset this in case it was previously set to false (doesn't get cleared)
  ShieldPoseRequirements& poseRequirements = ((ShieldBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Shield)))->getPoseRequirements();

  poseRequirements.enablePoseInput_ShieldPose = true;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

