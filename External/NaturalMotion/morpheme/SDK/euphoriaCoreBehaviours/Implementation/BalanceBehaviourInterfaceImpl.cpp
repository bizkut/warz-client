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
#include "Behaviours/BalanceBehaviour.h"
#include "BalanceBehaviourInterface.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "Behaviours/BalanceBehaviour.h"
#include "Helpers/Helpers.h"
#include "NetworkDescriptor.h"
#include "Types/BalanceAssistanceParameters.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "Implementation/Helpers/Step.h"

// Don't even try to balance if the (dimensionally scaled) gravity is less than this.
static const float gravityForBalance = 0.1f;

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

static void outputLimbTargets(const BalanceBehaviourData& params, BalanceBehaviourInterface& sbi)
{
  const BodyPoseData& bp = params.getBalancePose();
  // Note that even if bp.bp.m_poseWeight is 0, morpheme runtime will pass us a
  // default pose, so we guarantee to output a pose.

  // Here we assume that the incoming data is in the order arms, heads, spine,
  // legs arms. Also... note that the poses and the weights use the network indexing.
  PoseData poseData; // Need to send these type of data for defaultPose
  poseData.driveCompensationScale = 1.0f;
  poseData.stiffnessScale = 1.0f;

  // Only output the pose with the importance of the balance amount, since we
  // shouldn't do the pose when not balancing. The posing used to actually
  // balance goes via different outputs.
  float balanceAmount = sbi.feedIn->getBalanceAmount();

  const float minImportanceForBalancePose = 0.00001f;

  for (uint32_t i = 0 ; i < sbi.owner->data->numArms ; ++i)
  {
    uint32_t index = sbi.owner->data->firstArmNetworkIndex + i;
    float w = bp.m_poseWeights[index];
    poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
    // If the full weight is requested, apply it to the whole arm. Otherwise reduce the weight
    // towards the hand, as that makes it look looser.
    poseData.strengthReductionTowardsEnd = 1.0f - w;
    poseData.stiffnessScale = w;
    poseData.dampingRatioScale = params.getNonSupportingDampingScale();
    poseData.gravityCompensation = params.getNonSupportingGravityCompensationScale(); // not *w as w is in the importance, I think this is right
    poseData.driveCompensationScale = params.getNonSupportingDriveCompensationScale();
    sbi.out->setArmBalancePoseEndRelativeToRootAt(i, bp.m_poseEndRelativeToRoots[index], NMP::maximum(w, minImportanceForBalancePose));
    sbi.out->setArmBalancePoseAt(i, poseData, w * balanceAmount);
  }
  // heads
  for (uint32_t i = 0 ; i < sbi.owner->data->numHeads ; ++i)
  {
    uint32_t index = sbi.owner->data->firstHeadNetworkIndex + i;
    float w = bp.m_poseWeights[index];
    sbi.out->setHeadBalancePoseEndRelativeToRootAt(i, bp.m_poseEndRelativeToRoots[index], NMP::maximum(w, minImportanceForBalancePose));
  }
  // legs
  for (uint32_t i = 0 ; i < sbi.owner->data->numLegs ; ++i)
  {
    uint32_t index = sbi.owner->data->firstLegNetworkIndex + i;
    float w = bp.m_poseWeights[index];
    sbi.out->setLegBalancePoseEndRelativeToRootAt(i, bp.m_poseEndRelativeToRoots[index], NMP::maximum(w, minImportanceForBalancePose));
  }
  // spine
  for (uint32_t i = 0 ; i < sbi.owner->data->numSpines ; ++i)
  {
    uint32_t index = sbi.owner->data->firstSpineNetworkIndex + i;
    float w = bp.m_poseWeights[index];
    sbi.out->setSpineBalancePoseEndRelativeToRootAt(
      i, bp.m_poseEndRelativeToRoots[index], NMP::maximum(w, minImportanceForBalancePose));
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void cachePose(
  PoseData& poseData,
  float weight,
  float dampingRatioScale,
  float gravityCompensationScale,
  float driveCompensation,
  const BodyPoseData& bp,
  uint32_t index)
{
  new (&poseData) PoseData;
  poseData.poseEndRelativeToRoot = bp.m_poseEndRelativeToRoots[index];
  poseData.stiffnessScale = weight;
  poseData.dampingRatioScale = dampingRatioScale;
  poseData.gravityCompensation = gravityCompensationScale;
  poseData.driveCompensationScale = driveCompensation;
}

//----------------------------------------------------------------------------------------------------------------------
static void cacheLimbTargets(const BalanceBehaviourData& params, BalanceBehaviourInterface& sbi)
{
  const BodyPoseData& bp = params.getBalancePose();
  // Here we assume that the incoming data is in the order arms, heads, spine,
  // legs arms. Also... note that the poses and the weights use the network indexing.

  // arms
  for (uint32_t i = 0 ; i < sbi.owner->data->numArms ; ++i)
  {
    uint32_t index = sbi.owner->data->firstArmNetworkIndex + i;
    cachePose(
      sbi.data->cachedBalancePose[index],
      bp.m_poseWeights[index],
      params.getNonSupportingDampingScale(),
      params.getNonSupportingGravityCompensationScale(),
      params.getNonSupportingDriveCompensationScale(),
      bp,
      index);
  }
  // heads
  for (uint32_t i = 0 ; i < sbi.owner->data->numHeads ; ++i)
  {
    uint32_t index = sbi.owner->data->firstHeadNetworkIndex + i;
    cachePose(sbi.data->cachedBalancePose[index],
      bp.m_poseWeights[index],
      1.0f,
      1.0f,
      1.0f,
      bp,
      index);
  }
  // legs
  for (uint32_t i = 0 ; i < sbi.owner->data->numLegs ; ++i)
  {
    uint32_t index = sbi.owner->data->firstLegNetworkIndex + i;
    cachePose(sbi.data->cachedBalancePose[index],
      bp.m_poseWeights[index],
      1.0f,
      1.0f,
      0.0f,
      bp,
      index);
  }
  // spine
  for (uint32_t i = 0 ; i < sbi.owner->data->numSpines ; ++i)
  {
    uint32_t index = sbi.owner->data->firstSpineNetworkIndex + i;
    cachePose(sbi.data->cachedBalancePose[index],
      bp.m_poseWeights[index],
      1.0f,
      1.0f,
      1.0f,
      bp,
      index);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void outputCachedLimbTargets(BalanceBehaviourInterface& sbi)
{
  // Note that even if bp.bp.m_poseWeight is 0, morpheme runtime will pass us a
  // default pose, so we guarantee to output a pose with non-zero importance

  // Only output the pose with the importance of the balance amount, since we
  // shouldn't do the pose when not balancing. The posing used to actually
  // balance goes via different outputs.
  float balanceAmount = sbi.feedIn->getBalanceAmount();

  const float minImportanceForBalancePose = 0.00001f;

  for (uint32_t i = 0 ; i < sbi.owner->data->numArms ; ++i)
  {
    uint32_t index = sbi.owner->data->firstArmNetworkIndex + i;
    const PoseData& poseData = sbi.data->cachedBalancePose[index];
    sbi.out->setArmBalancePoseEndRelativeToRootAt(i, poseData.poseEndRelativeToRoot,
      NMP::maximum(poseData.stiffnessScale, minImportanceForBalancePose));
    float imp = balanceAmount;
    sbi.out->setArmBalancePoseAt(i, poseData, imp);
  }
  // heads
  for (uint32_t i = 0 ; i < sbi.owner->data->numHeads ; ++i)
  {
    uint32_t index = sbi.owner->data->firstHeadNetworkIndex + i;
    const PoseData& poseData = sbi.data->cachedBalancePose[index];
    sbi.out->setHeadBalancePoseEndRelativeToRootAt(i, poseData.poseEndRelativeToRoot,
      NMP::maximum(poseData.stiffnessScale, minImportanceForBalancePose));
  }
  // legs
  for (uint32_t i = 0 ; i < sbi.owner->data->numLegs ; ++i)
  {
    uint32_t index = sbi.owner->data->firstLegNetworkIndex + i;
    const PoseData& poseData = sbi.data->cachedBalancePose[index];
    sbi.out->setLegBalancePoseEndRelativeToRootAt(i, poseData.poseEndRelativeToRoot,
      NMP::maximum(poseData.stiffnessScale, minImportanceForBalancePose));
  }
  // spine
  for (uint32_t i = 0 ; i < sbi.owner->data->numSpines ; ++i)
  {
    uint32_t index = sbi.owner->data->firstSpineNetworkIndex + i;
    const PoseData& poseData = sbi.data->cachedBalancePose[index];
    sbi.out->setSpineBalancePoseEndRelativeToRootAt(i, poseData.poseEndRelativeToRoot,
      NMP::maximum(poseData.stiffnessScale, minImportanceForBalancePose));
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void updateTargetPelvisDirection(const BalanceBehaviourData& params, BalanceBehaviourInterface& module)
{
  // Output the target pelvis direction
  NMP::Vector3 targetPelvisDirection = params.getTargetPelvisDirectionCP();
  float targetPelvisDirectionLength = targetPelvisDirection.normaliseGetLength();
  if (targetPelvisDirectionLength > 0.0f)
  {
    bool shouldOutputTargetPelvisDirection = true;

    if(params.getIgnoreDirectionWhenOutOfRange())
    {
      // Calculate pitch and yaw from pelvis position and front direction to look target
      NMP::Vector3 targetPelvisDirectionInCharacterSpace = targetPelvisDirection;
      module.feedIn->getSupportTM().inverseRotateVector(targetPelvisDirectionInCharacterSpace);
      NMP::Quat rotationFromForwardToTarget;
      rotationFromForwardToTarget.forRotation(NMP::Vector3XAxis(), targetPelvisDirectionInCharacterSpace);
      NMP::Vector3 eulerXYZ = rotationFromForwardToTarget.toEulerXYZ();

      const float targetPitch = eulerXYZ.z;
      const float targetYaw   = eulerXYZ.y;

      const float yawMin   = NMP::degreesToRadians(params.getTargetYawRight());
      const float yawMax   = NMP::degreesToRadians(params.getTargetYawLeft());
      const float pitchMin = NMP::degreesToRadians(params.getTargetPitchDown());
      const float pitchMax = NMP::degreesToRadians(params.getTargetPitchUp());

      shouldOutputTargetPelvisDirection &= NMP::valueInRange(targetYaw, yawMin, yawMax);
      shouldOutputTargetPelvisDirection &= NMP::valueInRange(targetPitch, pitchMin, pitchMax);
    }

    if (shouldOutputTargetPelvisDirection)
    {
        targetPelvisDirectionLength = NMP::clampValue(targetPelvisDirectionLength, 0.0f, 1.0f);
        module.out->setTargetPelvisDirection(targetPelvisDirection, targetPelvisDirectionLength);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface::update(float timeStep)
{
#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
#endif
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());

  // Early out if there's no gravity - there's nothing to balance against! This should prevent all
  // associated modules doing any significant work. However, remember that the tolerance is itself
  // scaled by gravity, so this will only disable balance is the character is massively overpowered
  // to balance in this gravity (typically this will happen if gravity is zero, or the game manually
  // sets the dimensional scaling in low gravity, or if gravity is varying over time.
  if (owner->data->gravityMagnitude < SCALE_ACCEL(gravityForBalance))
  {
    return;
  }

  const BalanceBehaviourData& params =
    ((BalanceBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Balance)))->getParams();
  BalancePoseRequirements& poseRequirements =
    ((BalanceBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Balance)))->getPoseRequirements();

  //----------------------------------------------------------------------------------------------------------------------
  // Stand
  //----------------------------------------------------------------------------------------------------------------------

  if (params.getUseSingleFrameForBalancePose())
  {
    if (poseRequirements.enablePoseInput_BalancePose)
    {
      cacheLimbTargets(params, *this);
      poseRequirements.enablePoseInput_BalancePose = false;
    }
    outputCachedLimbTargets(*this);
  }
  else
  {
    poseRequirements.enablePoseInput_BalancePose = true;
    outputLimbTargets(params, *this);
  }

  BalanceParameters& bpOut = out->startBalanceParametersModification();

  const float degreesToRadians = NM_PI / 180.0f;
  bpOut.footBalanceAmount          = SCALE_ANG(params.getFootBalanceAmount() * degreesToRadians);
  bpOut.decelerationAmount         = params.getDecelerationAmount() / SCALE_VEL(1.0f);
  bpOut.maxAngle                   = SCALE_ANG(NMP::maximum(params.getMaxAngle(), 0.001f));
  bpOut.footLength                 = SCALE_DIST(NMP::maximum(params.getFootLength(), 0.01f));
  bpOut.spinAmount                 = params.getAllowArmSpin() ? params.getSpinAmount() : 0.0f;
  bpOut.spinThreshold              = params.getSpinThreshold();
  bpOut.lowerPelvisDistanceWhenFootLifts = SCALE_DIST(params.getLowerPelvisDistanceWhenFootLifts());
  bpOut.steppingDirectionThreshold  = params.getSteppingDirectionThreshold();

  updateTargetPelvisDirection(params, *this);

  for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
  {
    float standStrength = params.getLegStandStrengthCP(i);
    float strengthReduction = in->getLegStrengthReduction(i);
    bpOut.balanceWeaknessPerLeg[i] = NMP::minimum(1.0f, 1.0f - standStrength * (1.0f - strengthReduction));
    bpOut.supportPointWeightingPerLeg[i] = 1.0f;
  }
  for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
  {
    float standStrength = params.getArmStandStrengthCP(i);
    float strengthReduction = in->getArmStrengthReduction(i);
    bpOut.balanceWeaknessPerArm[i] = NMP::minimum(1.0f, 1.0f - standStrength * (1.0f - strengthReduction));
    bpOut.supportPointWeightingPerArm[i] = 1.0f;
  }
  bpOut.supportFwdOffset = SCALE_DIST(params.getBalanceFwdOffsetCP());
  bpOut.supportRightOffset = SCALE_DIST(params.getBalanceRightOffsetCP());
  out->stopBalanceParametersModification();

  BalancePoseClamping bpc;
  if (params.getEnablePoseClamping())
  {
    float tinyValue = 0.00001f; // just to avoid divide by zero etc later
    bpc.enablePoseClamping = true;
    bpc.dynamicClamping = params.getDynamicClamping();
    bpc.minPelvisPitch = SCALE_ANG(NMP::degreesToRadians(NMP::minimum(params.getMinPelvisPitch(), -tinyValue)));
    bpc.maxPelvisPitch = SCALE_ANG(NMP::degreesToRadians(NMP::maximum(params.getMaxPelvisPitch(), tinyValue)));
    bpc.minPelvisYaw = SCALE_ANG(NMP::degreesToRadians(NMP::minimum(params.getMinPelvisYaw(), -tinyValue)));
    bpc.maxPelvisYaw = SCALE_ANG(NMP::degreesToRadians(NMP::maximum(params.getMaxPelvisYaw(), tinyValue)));
    bpc.minPelvisRoll = SCALE_ANG(NMP::degreesToRadians(NMP::minimum(params.getMinPelvisRoll(), -tinyValue)));
    bpc.maxPelvisRoll = SCALE_ANG(NMP::degreesToRadians(NMP::maximum(params.getMaxPelvisRoll(), tinyValue)));
    bpc.minPelvisHeight = SCALE_DIST(NMP::minimum(params.getMinPelvisHeight(), -tinyValue));
    bpc.maxPelvisHeight = SCALE_DIST(NMP::maximum(params.getMaxPelvisHeight(), tinyValue));
    bpc.pelvisPositionChangeTimescale = NMP::maximum(SCALE_TIME(params.getPelvisPositionChangeTimescale()), 0.001f);
    bpc.pelvisOrientationChangeTimescale = NMP::maximum(SCALE_TIME(params.getPelvisOrientationChangeTimescale()), 0.001f);
    out->setBalancePoseClamping(bpc);
  }

  out->setSupportWithLegs(params.getEnableStand());
  out->setSupportWithArms(params.getSupportWithArms() && params.getEnableStand());

  out->setBalanceUsingAnimationHorizontalPelvisOffsetAmount(
    NMP::clampValue(params.getBalanceWithAnimationPoseCP(), 0.0f, 1.0f));

  //----------------------------------------------------------------------------------------------------------------------
  // Stagger
  //----------------------------------------------------------------------------------------------------------------------
  StepParameters sp;
  sp.collisionGroupIndex         =  -1;
  
  if(params.getEnableCollisionGroup())
  {
    sp.collisionGroupIndex = params.getCollisionGroupIndex();
  }

  sp.stepPredictionTimeForward   = SCALE_TIME(params.getStepPredictionTimeForward());
  sp.stepPredictionTimeBackward  = SCALE_TIME(params.getStepPredictionTimeBackward());
  sp.stepUpDistance              = SCALE_DIST(params.getStepUpDistance());
  sp.stepDownDistance            = SCALE_DIST(params.getStepDownDistance());
  sp.maxStepLength               = NMP::maximum(SCALE_DIST(params.getMaxStepLength()), 0.001f);
  sp.stepDownSpeed               = SCALE_VEL(params.getStepDownSpeed());
  sp.footSeparationFraction      = params.getFootSeparationFraction();
  sp.alignFootToFloorWeight      = params.getAlignFootToFloorWeight();
  sp.orientationWeight           = params.getSteppingOrientationWeight();
  sp.lowerPelvisAmount           = params.getLowerPelvisAmount();
  sp.IKSubstep                   = NMP::clampValue(params.getIKSubStep(), 0.0f, 1.0f);
  sp.gravityCompensation         = params.getSteppingGravityCompensation();
  sp.limbLengthToAbort           = SCALE_DIST(params.getSteppingLimbLengthToAbort());
  sp.rootDownSpeedToAbort        = SCALE_VEL(params.getSteppingRootDownSpeedToAbort());
  sp.steppingImplicitStiffness   = params.getSteppingImplicitStiffness();
  sp.steppingDirectionThreshold  = params.getSteppingDirectionThreshold();

  sp.strengthScale               = params.getStrengthScale();
  sp.dampingRatioScale           = params.getDampingRatioScale();
  for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
  {
    sp.stepWeaknessPerArm[i] = 1.0f - params.getArmStepStrengthCP(i);
  }
  for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
  {
    sp.stepWeaknessPerLeg[i] = 1.0f - params.getLegStepStrengthCP(i);
  }
  sp.exclusionPlane = params.getExclusionZoneDirectionCP();
  sp.exclusionPlane.normalise(NMP::Vector3::InitTypeZero);
  sp.exclusionPlane.w = -params.getExclusionZonePointCP().dot(sp.exclusionPlane);
  sp.suppressFootCrossing = params.getSuppressFootCrossing();
  sp.footCrossingOffset = SCALE_DIST(params.getFootCrossingOffset());
  out->setStepParameters(sp);

  if (params.getStepWithArms() || params.getStepWithLegs())
  {
    StepRecoveryParameters srp;
    srp.stepToRecoverPose = params.getStepToRecoverPose();
    srp.fwdDistanceToTriggerStep = SCALE_DIST(NMP::maximum(params.getFwdDistanceToTriggerStep(), 0.001f));
    srp.sidewaysDistanceToTriggerStep = SCALE_DIST(NMP::maximum(params.getSidewaysDistanceToTriggerStep(), 0.001f));
    srp.timeBeforeShiftingWeight = SCALE_TIME(params.getTimeBeforeShiftingWeight());
    srp.weightShiftingTime = SCALE_TIME(NMP::maximum(params.getWeightShiftingTime(), 0.001f));
    out->setStepRecoveryParameters(srp);
  }

  bool enableOnLegs = params.getStepWithLegs();
  bool enableOnArms = params.getStepWithArms();
  if (data->runningTime < SCALE_TIME(params.getSuppressSteppingTime()))
    enableOnLegs = enableOnArms = false;

  out->setStepWithLegs(enableOnLegs);
  out->setStepWithArms(enableOnArms);

  SteppingBalanceParameters sbp;
  sbp.lookInStepDirection   = params.getLookInStepDirectionCP() && params.getEnableLook();
  sbp.wholeBodyLook         = params.getWholeBodyLook();
  sbp.lookDownAngle         = SCALE_ANG(params.getLookDownAngle());
  sbp.stopLookingTime       = SCALE_TIME(params.getStopLookingTime());
  sbp.lookWeight            = params.getLookWeight();
  sbp.stepsBeforeLooking    = params.getStepsBeforeLooking();
  sbp.minSpeedForLooking    = SCALE_VEL(params.getMinSpeedForLooking());
  sbp.stepCountResetTime    = SCALE_TIME(params.getStepCountResetTime());
  sbp.armSwingStrengthScaleWhenStepping = params.getArmSwingStrengthScaleWhenStepping();
  sbp.stepPoseArmDisplacementTime = SCALE_TIME(params.getArmDisplacementTime());
  out->setSteppingBalanceParameters(sbp);

  if (params.getReadyPose().m_poseWeight != 0)
  {
    if (params.getUseSingleFrameForReadyPose())
    {
      if (poseRequirements.enablePoseInput_ReadyPose)
      {
        for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
        {
          float w = params.getReadyPose().m_poseWeights[i];
          data->cachedReadyPoseArmEndRelativeToRootWeights[i] = w;
          data->cachedReadyPoseArmEndRelativeToRoot[i] = params.getReadyPose().m_poseEndRelativeToRoots[i];
        }

        poseRequirements.enablePoseInput_ReadyPose = false;
      }
      for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
      {
        float w = data->cachedReadyPoseArmEndRelativeToRootWeights[i];
        out->setArmReadyPoseEndRelativeToRootAt(i, params.getReadyPose().m_poseEndRelativeToRoots[i], w);
      }
    }
    else
    {
      for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
      {
        float w = params.getReadyPose().m_poseWeights[i];
        out->setArmReadyPoseEndRelativeToRootAt(i, params.getReadyPose().m_poseEndRelativeToRoots[i], w);
      }
    }
  }
  else
  {
    poseRequirements.enablePoseInput_ReadyPose = false;
  }

  // Assistance
  BalanceAssistanceParameters assistance;
  assistance.reduceAssistanceWhenFalling = params.getReduceAssistanceWhenFalling();
  assistance.orientationAssistanceAmount = params.getOrientationAssistanceAmountCP();
  assistance.positionAssistanceAmount = params.getPositionAssistanceAmountCP();
  assistance.velocityAssistanceAmount = params.getVelocityAssistanceAmountCP();
  assistance.maxAngularAccelerationAssistance = params.getMaxAngularAccelerationAssistance();
  assistance.maxLinearAccelerationAssistance = params.getMaxLinearAccelerationAssistance();
  assistance.useCounterForceOnFeet = params.getUseCounterForceOnFeetCP();
  assistance.targetVelocity = params.getTargetVelocityCP();
  assistance.chestToPelvisRatio = params.getAssistanceChestToPelvisRatio();
  bool targetVelocityInCharacterSpace = params.getTargetVelocityInCharacterSpaceCP();
  if (targetVelocityInCharacterSpace)
  {
    if (feedIn->getSupportTMImportance() > 0.0f)
    {
      ER::LimbTransform supportTM = feedIn->getSupportTM();
      NMP::Vector3 localVel = params.getTargetVelocityCP();
      assistance.targetVelocity = supportTM.getRotatedVector(localVel);
    }
    else
    {
      assistance.targetVelocity.setToZero();
    }
  }
  else
  {
    assistance.targetVelocity = params.getTargetVelocityCP();
  }


  const float assistanceImportance = (params.getEnableAssistance())? 1.0f : 0.0f;

  out->setBalanceAssistanceParameters(assistance, assistanceImportance);

  data->runningTime += timeStep;
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface::feedback(float timeStep)
{
  feedOut->setBalanceAmount(feedIn->getBalanceAmount());
  feedOut->setSteppingTime(feedIn->getSteppingTime());
  feedOut->setStepCount(feedIn->getStepCount());
  feedOut->setCoMBodyState(feedIn->getCoMBodyState());

  BalanceBehaviour* behaviour = ((BalanceBehaviour*)owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Balance));
  BalanceBehaviourData& params = behaviour->getParams();

  //----------------------------------------------------------------------------------------------------------------------
  // Stand
  //----------------------------------------------------------------------------------------------------------------------

  float prevFallenTime = data->fallenTime;

  if (feedIn->getBalanceAmount() == 0.0f)
  {
    data->fallenTime += timeStep;
  }
  else
  {
    data->fallenTime = 0.0f;
  }
  feedOut->setFallenTime(data->fallenTime);

  if (feedIn->getMaxLegCollidingTime() > 0.0f && 
      owner->data->gravityMagnitude >= SCALE_ACCEL(gravityForBalance))
  {
    data->onGroundTime += timeStep;
  }
  else
  {
    data->onGroundTime = 0.0f;
  }
  feedOut->setOnGroundTime(data->onGroundTime);

  // standing still time - only increment if not fallen, on the ground and not stepping
  if (data->fallenTime > 0.0f ||
      data->onGroundTime == 0.0f ||
      (feedIn->getTimeSinceLastStepImportance() > 0.0f && feedIn->getTimeSinceLastStep() == 0.0f))
  {
    data->standingStillTime = 0.0f;
  }
  else
  {
    data->standingStillTime += timeStep;
  }
  feedOut->setStandingStillTime(data->standingStillTime);

  // Note that we can't reed feedOut values here because the connections haven't been evaluated yet
  params.setStandingStillTimeOCP(data->standingStillTime);
  params.setFallenTimeOCP(data->fallenTime);
  params.setOnGroundTimeOCP(data->onGroundTime);
  params.setBalanceAmountOCP(feedIn->getBalanceAmount());
  params.setSupportVelocityOCP(feedIn->getSupportVelocity());
  params.setForwardsDirectionOCP(feedIn->getSupportTM().frontDirection());
  params.setRightDirectionOCP(feedIn->getSupportTM().rightDirection());

  float fallenTimeToEmitRequests = SCALE_TIME(params.getFallenTimeToEmitRequests());
  if (data->fallenTime > fallenTimeToEmitRequests)
  {
    params.setMessage((uint32_t)behaviour->HasFallen);
  }
  else if (prevFallenTime > fallenTimeToEmitRequests && data->fallenTime == 0.0f)
  {
    params.setMessage((uint32_t)behaviour->HasRecoveredBalance);
  }

  //----------------------------------------------------------------------------------------------------------------------
  // Stagger
  //----------------------------------------------------------------------------------------------------------------------
  // Note that we can't reed feedOut values here because the connections haven't been evaluated yet
  params.setSteppingTimeOCP(feedIn->getSteppingTime());
  params.setStepCountOCP(feedIn->getStepCount());
  for (uint32_t i = 0; i < owner->data->numLegs; i++)
  {
    float legStepFraction = feedIn->getLegStepFractions(i);
    params.setLegStepFractionsOCP(i, legStepFraction);
  }

  //----------------------------------------------------------------------------------------------------------------------
  // Assist
  //----------------------------------------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourInterface::entry()
{
  // Reset this in case it was previously set to false (doesn't get cleared)
  BalancePoseRequirements& poseRequirements =
    ((BalanceBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Balance)))->getPoseRequirements();
  poseRequirements.enablePoseInput_BalancePose = true;
  poseRequirements.enablePoseInput_ReadyPose = true;

  data->standingStillTime = 0.0f;
  data->fallenTime = 0.0f;
  data->runningTime = 0.0f;
  data->onGroundTime = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

