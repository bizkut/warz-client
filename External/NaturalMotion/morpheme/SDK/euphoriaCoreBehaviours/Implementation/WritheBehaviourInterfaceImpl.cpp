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
#include "WritheBehaviourInterface.h"
#include "euphoria/erCharacter.h"
#include "Behaviours/WritheBehaviour.h"
#include "NetworkDescriptor.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const WritheBehaviourData &params =
    ((WritheBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Writhe)))->getParams();
  const BodyPoseData& bp = params.getDefaultPose();

  WritheParameters writheParams;
  writheParams.defaultPoseEndRelativeToRoot.identity();
  // Here we assume that the incoming data is in the order arms, heads, spine, legs
  // arms. Also... note that the poses and the weights use the network indexing.
  for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
  {
    uint32_t index = owner->data->firstArmNetworkIndex + i;
    if (bp.m_poseWeights[index] > 0.0f)
    {
      writheParams.basedOnDefaultPose = params.getArmsBasedOnDefaultPose(i);
      if (writheParams.basedOnDefaultPose)
      {
        writheParams.defaultPoseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
      }
      writheParams.amplitudeScale = params.getArmsAmplitudeScale(i);
      writheParams.stiffnessScale = params.getImportanceArmCP(i);
      writheParams.driveCompensationScale = params.getArmsDriveCompensationScale(i);
      NMP_ASSERT(params.getArmsFrequency(i) > 0.0f);
      writheParams.resetTargetTP = SCALE_TIME(1.0f / params.getArmsFrequency(i));
      float importance = NMP::clampValue(params.getImportanceArmCP(i), 0.0f, 1.0f);
      out->setArmsParamsAt(i, writheParams, importance);
    }
  }
  for (uint32_t i = 0 ; i < owner->data->numHeads ; ++i)
  {
    uint32_t index = owner->data->firstHeadNetworkIndex + i;
    if (bp.m_poseWeights[index] > 0.0f)
    {
      RandomLookParameters randomLookParameters;
      PoseData headPose;
      headPose.driveCompensationScale = params.getHeadsDriveCompensationScale(i);
      headPose.stiffnessScale = params.getImportanceHeadCP(i);
      if (params.getHeadsBasedOnDefaultPose(i))
      {
        headPose.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
        out->setHeadPoseAt(i, headPose, bp.m_poseWeights[index]);
      }
      // RandomLook currently only supports one head so we can only feed it one input. When it
      // supports multiple heads we need to send multiple outputs. See MORPH-14122
      if (i == 0)
      {
        float importance = NMP::clampValue(params.getImportanceHeadCP(i), 0.0f, 1.0f);
        randomLookParameters.lookRangeSideways = importance * params.getHeadsAmplitudeScale(i) * NM_PI;
        randomLookParameters.lookRangeUpDown = params.getHeadsAmplitudeScale(i) * params.getHeadsAmplitudeScale(i) * NM_PI;
        randomLookParameters.lookAmount = bp.m_poseWeights[index];
        randomLookParameters.lookWithWholeBody = 0.5f;
        // Not exposing these as we are using the look in order to writhe, so clients shouldn't need to 
        // control the head using 'look' based parameters such as the focus distance.  
        randomLookParameters.lookTransitionTime = SCALE_TIME(0.5f);
        randomLookParameters.lookVerticalOffset = SCALE_DIST(-0.8f);
        randomLookParameters.lookFocusDistance = SCALE_DIST(5.0f);
        NMP_ASSERT(params.getHeadsFrequency(i) > 0.0f);
        randomLookParameters.lookTimescale = SCALE_TIME(1.0f / params.getHeadsFrequency(i));
        out->setRandomLookParameters(randomLookParameters, importance);
      }
    }
  }
  for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
  {
    uint32_t index = owner->data->firstLegNetworkIndex + i;
    if (bp.m_poseWeights[index] > 0.0f)
    {
      writheParams.basedOnDefaultPose = params.getLegsBasedOnDefaultPose(i);
      if (writheParams.basedOnDefaultPose)
      {
        writheParams.defaultPoseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
      }
      writheParams.amplitudeScale = params.getLegsAmplitudeScale(i);
      writheParams.stiffnessScale = params.getImportanceLegCP(i);
      writheParams.driveCompensationScale = params.getLegsDriveCompensationScale(i);
      NMP_ASSERT(params.getLegsFrequency(i) > 0.0f);
      writheParams.resetTargetTP = SCALE_TIME(1.0f / params.getLegsFrequency(i));
      float importance = NMP::clampValue(params.getImportanceLegCP(i), 0.0f, 1.0f);
      out->setLegsParamsAt(i, writheParams, importance);
    }
  }
  for (uint32_t i = 0 ; i < owner->data->numSpines ; ++i)
  {
    uint32_t index = owner->data->firstSpineNetworkIndex + i;
    if (bp.m_poseWeights[index] > 0.0f)
    {
      writheParams.basedOnDefaultPose = params.getSpinesBasedOnDefaultPose(i);
      if (writheParams.basedOnDefaultPose)
      {
        writheParams.defaultPoseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
      }
      writheParams.amplitudeScale = params.getSpinesAmplitudeScale(i);
      writheParams.stiffnessScale = params.getImportanceSpineCP(i);
      writheParams.driveCompensationScale = params.getSpinesDriveCompensationScale(i);
      NMP_ASSERT(params.getSpinesFrequency(i) > 0.0f);
      writheParams.resetTargetTP = SCALE_TIME(1.0f / params.getSpinesFrequency(i));
      float importance = NMP::clampValue(params.getImportanceSpineCP(i), 0.0f, 1.0f);
      out->setSpinesParamsAt(i, writheParams, importance);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviourInterface::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

