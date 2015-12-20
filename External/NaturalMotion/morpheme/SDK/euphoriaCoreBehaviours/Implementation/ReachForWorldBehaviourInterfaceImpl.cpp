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
#include "MyNetwork.h"
#include "ReachForWorldBehaviourInterface.h"
#include "Behaviours/ReachForWorldBehaviour.h"
#include "NetworkDescriptor.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const ReachForWorldBehaviourData& params =
    ((ReachForWorldBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ReachForWorld)))->getParams();

  float greatestReachImportance = 0.0f;
  // Build and send reach params.
  for (uint32_t i = 0; i < owner->data->numArms ; i++)
  {
    float reachImportance = params.getReachImportanceForArmCP(i);
    if (reachImportance > 0.0f)
    {
      LimbControl& control = out->startLimbControlsModificationAt(i);

      control.reinit(
        owner->data->normalStiffness * params.getStiffnessScaleForArmCP(i),
        owner->data->normalDampingRatio * params.getDampingScaleForArmCP(i),
        owner->data->normalDriveCompensation * params.getDriveCompensationScaleForArmCP(i));
      control.setTargetPos(params.getPositionForArmCP(i), params.getPositionWeightForArm(i));
      NMP::Vector3 normal = params.getNormalForArmCP(i);
      normal.fastNormalise();
      control.setTargetNormal(normal, params.getNormalWeightForArm(i));
      control.setGravityCompensation(1.0f);
      control.setControlAmount(ER::reachControl, 1.0f);
      out->stopLimbControlsModificationAt(i, reachImportance);

      ReachWorldParameters& reachParams = out->startReachParamsModificationAt(i);
      reachParams.selfAvoidanceStrategy 
        = params.getSelfAvoidanceEnable(i) ?
        ReachWorldParameters::automatic : ReachWorldParameters::disable;
      reachParams.unreachableTargetImportanceScale = params.getUnreachableTargetImportanceScaleCP();
      reachParams.torsoRadiusMultiplier = params.getTorsoAvoidanceRadiusMultiplier(i);
      reachParams.reachImminence = SCALE_IMMINENCE(params.getReachImminenceForArmCP(i));
      reachParams.slideAlongNormal = params.getSlideAlongNormalForArm(i);
      reachParams.swivelMode = params.getSwivelMode(i);
      reachParams.swivelAmount = params.getSwivelAmountForArm(i);
      reachParams.maxReachScale =  params.getMaxReachScale(i);
      reachParams.ikSubstepSize = params.getIKSubstepSize();
      reachParams.chestRotationScale = params.getChestRotationScaleForArm(i);
      reachParams.pelvisRotationScale = params.getPelvisRotationScaleForArm(i);
      reachParams.maxChestTranslation = params.getMaxChestTranslationForArm(i);
      reachParams.maxPelvisTranslation = params.getMaxPelvisTranslationForArm(i);
      out->stopReachParamsModificationAt(i, reachImportance);
      greatestReachImportance = NMP::floatSelect(reachImportance - greatestReachImportance,
        reachImportance, greatestReachImportance);
    }
  }
  // MORPH-12479. Expose this parameter if not good enough for reducing oscillations on all rigs.
  out->setSpineDampingIncrease(1.5f, greatestReachImportance);

  // Calculate average look target from the reach targets for each arm.
  TargetRequest lookTargetSum;
  float         lookWeightSum(0.0f);
  bool          lookInitialised(false);

  for (uint32_t i = 0; i < owner->data->numArms ; i++)
  {
    // Set a look target to the current target to reach.
    const float lookWeight(params.getLookWeightToPositionForArm(i));

    if (lookWeight)
    {
      TargetRequest lookTarget;
#if defined(MR_OUTPUT_DEBUGGING)
      lookTarget.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
      // Imminence for the look is the same than the chest's imminence.
      lookTarget.imminence = SCALE_IMMINENCE(params.getLookImminenceForArmCP(i));
      lookTarget.passOnAmount = 0.0f;
      lookTarget.lookOutOfRangeWeight = 0.7f; // Not 1 to avoid head jitters when target totally behind.
      lookTarget.target = params.getPositionForArmCP(i);

      lookTarget *= lookWeight;

      if (lookInitialised)
      {
        // Add subsequent look targets to sum
        lookTargetSum += lookTarget;
      }
      else
      {
        // This is the first look target we've encountered so initialise sum
        lookTargetSum = lookTarget;
      }

      lookInitialised = true;

      lookWeightSum += lookWeight;
    }
  }

  if (lookWeightSum > 0.0f)
  {
    lookTargetSum *= 1.0f / lookWeightSum;

    out->setLookTarget(lookTargetSum, 1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  for (uint32_t i = 0; i < owner->data->numArms ; i++)
  {
    feedOut->setDistanceToTargetAt(i, feedIn->getDistanceToTarget(i), feedIn->getDistanceToTargetImportance(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForWorldBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

