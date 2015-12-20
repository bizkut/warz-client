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
#include "BodyBalancePackaging.h"
#include "BodyBalance.h"
#include "BodyFramePackaging.h"
#include "BodyFrame.h"
#include "SupportPolygonData.h"
#define SCALING_SOURCE pkg.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaLogger.h"
#include "Types/Environment_SupportPoly.h"

//----------------------------------------------------------------------------------------------------------------------
float g_armsRotateScale = 10.0f;
float g_bodyBalanceSupportDirSmoothTime = 0.25f;
float g_bodyBalancePelvisStabilisationDuringStepping = 1.0f;

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
static NM_INLINE NMP::Vector3 rotateAroundDirection(const NMP::Vector3& v, const NMP::Vector3& dir, float angle)
{
  NMP_ASSERT(fabsf(dir.magnitudeSquared() - 1.0f) < 0.01f);
  NMP::Quat q(dir, angle);
  return q.rotateVector(v);
}

// These things can be calculated at the start of the update, and are needed only during the update
// (i.e. don't need to be stored for the next update, or for feedback).
struct BalanceUpdateData
{
  ER::LimbTransform pelvisMatrix;
  NMP::Vector3 balancePoint; ///< This is the point (in world space) to move over the support - normally CoM
  float supportForwardsDistance;
  float supportSidewaysDistance;
};

//----------------------------------------------------------------------------------------------------------------------
// This gets array of limb end contact info from my network's limb states ({legs}, {arms})
static void getLimbEndContacts(const BodyBalanceAPIBase& pkg, const ContactInfo** endContacts)
{
  const uint32_t maxLimbIndex = pkg.owner->owner->data->numLegs + pkg.owner->owner->data->numArms;

  // Build the array: ({legs}, {arms})
  uint32_t i = 0;
  for( ; i < pkg.owner->owner->data->numLegs; ++i)
  {
    endContacts[i] = &pkg.owner->owner->data->legLimbSharedStates[i].m_endContact;
  }
  for( ; i < maxLimbIndex; ++i)
  {
    endContacts[i] = &pkg.owner->owner->data->armLimbSharedStates[i].m_endContact;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This gets array of limb end transforms from my network's limb states ({legs}, {arms})
static void getLimbEndTMs(const BodyBalanceAPIBase& pkg, const ER::HandFootTransform** endTMs)
{
  const uint32_t maxLimbIndex = pkg.owner->owner->data->numLegs + pkg.owner->owner->data->numArms;

  // Build the array of: ({legs}, {arms})
  uint32_t i = 0;
  for( ; i < pkg.owner->owner->data->numLegs; ++i)
  {
    endTMs[i] = &pkg.owner->owner->data->legLimbSharedStates[i].m_endTM;
  }
  for( ; i < maxLimbIndex; ++i)
  {
    endTMs[i] = &pkg.owner->owner->data->armLimbSharedStates[i].m_endTM;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This calculates various pieces of data, most of which are used in multiple places in the
// subsequent balance code. This moves the complexity of analysing the current state into one place,
// to separate it from the complexity of deciding how to use that information. Note that some
// similar things are also calculated in feedback, and then the results stored in data.
static void calculateBalanceUpdateData(
  const BodyBalanceUpdatePackage& pkg,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const BalanceParameters& balanceParameters,
  BalanceUpdateData& balanceUpdateData)
{
  const BodyBalanceInputs* in = pkg.in;
  const BodyFrameAPIBase* owner = pkg.owner;
  BodyBalanceData* data = pkg.data;

  // Get the support extents.
  const Environment::SupportPoly& poly = in->getSupportPolygon();
  balanceUpdateData.supportForwardsDistance = poly.getExtentsInDirection(data->horizontalSupportTM.frontDirection());
  balanceUpdateData.supportSidewaysDistance = poly.getExtentsInDirection(data->horizontalSupportTM.rightDirection());

  balanceUpdateData.supportForwardsDistance += balanceParameters.footLength * 0.5f;
  balanceUpdateData.supportSidewaysDistance += balanceParameters.footLength * 0.25f; // Rather arbitrary here...

  balanceUpdateData.pelvisMatrix = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
  NMP_ASSERT(balanceUpdateData.pelvisMatrix.isValidTM(0.05f));

  // For the balance point (the balancer tries to place this over the CoS) use a weighted average of
  // the point above the CoS in the animation, and the CoM.
  float balanceUsingAnimationHorizontalPelvisOffsetAmount = in->getBalanceUsingAnimationHorizontalPelvisOffsetAmount();
  NMP::Vector3 pelvisOffsetInBalancePose(NMP::Vector3::InitZero);
  if (in->getPelvisRelSupportImportance() > 0.0f)
  {
    pelvisOffsetInBalancePose = data->horizontalSupportTM.getRotatedVector(in->getPelvisRelSupport().translation());
    pelvisOffsetInBalancePose = pelvisOffsetInBalancePose.getComponentOrthogonalToDir(owner->owner->data->up);
  }
  NMP::Vector3 animationBalancePoint = balanceUpdateData.pelvisMatrix.translation() - pelvisOffsetInBalancePose;
  balanceUpdateData.balancePoint = 
    animationBalancePoint * balanceUsingAnimationHorizontalPelvisOffsetAmount + 
    (1.0f - balanceUsingAnimationHorizontalPelvisOffsetAmount)
      * owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition();

  // Draw useful things here
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, balanceUpdateData.balancePoint, SCALE_DIST(.25f), NMP::Colour::YELLOW);
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, balanceUpdateData.pelvisMatrix, SCALE_DIST(0.5f));
}

//----------------------------------------------------------------------------------------------------------------------
// This calculates the world space pelvis transform required to balance and pose the character
static NMP::Matrix34 getBalanceDesiredPelvisTransform(
  const BodyBalanceUpdatePackage& pkg,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const BalanceParameters& balanceParameters,
  const BalanceUpdateData& balanceUpdateData)
{
  BodyBalanceData* data = pkg.data;
  const BodyBalanceInputs* in = pkg.in;
  const BodyBalanceFeedbackInputs* const feedIn = pkg.feedIn;
  const BodyFrameAPIBase* owner = pkg.owner;

  // make the pelvis so that it will place the COM over the COS
  ER::LimbTransform desiredPelvisTransform;

  NMP::Vector3 horCOMOffsetFromPelvis = (balanceUpdateData.balancePoint - balanceUpdateData.pelvisMatrix.translation()).
    getComponentOrthogonalToDir(data->effectiveGravityDir);
  NMP::Vector3 desiredPelvisPositionAtFeet = data->supportTM.translation() - horCOMOffsetFromPelvis;

  // only use the input if the importance is > 0, otherwise it will likely be zero!
  float verticalPelvisOffset =
    (in->getPelvisRelSupportImportance() > 0.0f) ?
      data->targetPelvisRelSupport.translation().y :
      -owner->owner->data->averageLegDefaultPoseEndRelativeToRoot.translation().y;

  desiredPelvisTransform.translation() = desiredPelvisPositionAtFeet - verticalPelvisOffset * data->effectiveGravityDir;

  // Lower the pelvis when the stepping (for example) asks for it
  desiredPelvisTransform.translation() -= owner->owner->data->up * feedIn->getLowerPelvisDistance();

  NMP_ASSERT(desiredPelvisTransform.isValidTM(0.05f));

  // Start by aiming for the pelvis to be upright - aligned up the effective gravity direction when
  // not stepping, or just vertical when stepping
  desiredPelvisTransform.set3x3(data->supportTM);

  // rotate the pelvis according to the input pose
  if (in->getPelvisRelSupportImportance() > 0.0f)
    desiredPelvisTransform.multiply3x3(data->targetPelvisRelSupport, desiredPelvisTransform);
  NMP_ASSERT(desiredPelvisTransform.isValidTM(0.05f));

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->supportTM.translation(), SCALE_DIST(0.2f), NMP::Colour::RED);
#if defined(MR_OUTPUT_DEBUGGING)
  {
    NMP::Vector3 COMFPos = data->supportTM.translation() +
      (balanceUpdateData.balancePoint - data->supportTM.translation()).getComponentOrthogonalToDir(data->effectiveGravityDir);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, COMFPos, SCALE_DIST(0.2f), NMP::Colour::YELLOW);
  }
#endif

  // Now do Proportional control (the D term comes from the effective gravity dir, and the I term
  // isn't needed so long as gravity compensation works properly) on desiredPelvisTransform by
  // rotating it around the COSPos, depending on the COM offset from the COS position
  NMP::Vector3 COMError = balanceUpdateData.balancePoint - data->supportTM.translation();

  float COMErrorSideways = NMP::vDot(COMError, data->supportTM.rightDirection());
  float COMErrorForwards = NMP::vDot(COMError, data->supportTM.frontDirection());

  //printf("COMErrorSideways = %5.3f COMErrorForwards = %5.3f\n", COMErrorSideways, COMErrorForwards);

  // use normalised position/velocity errors
  data->sidewaysPosErrorFrac = COMErrorSideways / balanceUpdateData.supportSidewaysDistance;
  data->forwardsPosErrorFrac = COMErrorForwards / balanceUpdateData.supportForwardsDistance;

  // square the error so that small deviations result in small corrections etc. Also scale it.
  data->sidewaysPosErrorFrac = data->sidewaysPosErrorFrac * NMP::nmfabs(data->sidewaysPosErrorFrac);
  data->forwardsPosErrorFrac = data->forwardsPosErrorFrac * NMP::nmfabs(data->forwardsPosErrorFrac);

  // convert horizontal distances into foot rotation angles (degrees)
  float pSideways = data->sidewaysPosErrorFrac * balanceParameters.footBalanceAmount;
  float pForwards = data->forwardsPosErrorFrac * balanceParameters.footBalanceAmount;

  // rotation vector - note that the sideways term rotates around the forwards direction
  NMP::Vector3 rot =
    -data->horizontalSupportTM.frontDirection() * pSideways + data->horizontalSupportTM.rightDirection() * pForwards;

  // Prevent large rotation targets as they can wrap around
  const float maxAngle = NMP::degreesToRadians(balanceParameters.maxAngle);
  rot.clampMagnitude(maxAngle);

  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredPelvisTransform, SCALE_DIST(1.0f));

  // want to rotate desiredPelvisTransform around the COS position - this equates to rotating the
  // foot support plane around where they are.
  NMP::Quat quat(rot, false);
  NMP::Matrix34 mat(quat);
  mat.translation().setToZero();
  // When stepping, it seems to help to rotate around the planted foot to avoid that leg bending -
  // especially on slopes.
  NMP::Vector3 rotationPoint = data->groundContactPoint;
  desiredPelvisTransform.translation() -= rotationPoint;
  // balancing with arms works better if the CoM is translated without rotation
  if (in->getEnableSupportOnArms())
  {
    mat.rotateVector(desiredPelvisTransform.translation());
  }
  else
  {
    desiredPelvisTransform.multiply(mat);
  }
  desiredPelvisTransform.translation() += rotationPoint;

  //MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredPelvisTransform, SCALE_DIST(0.1f));

  NMP_ASSERT(desiredPelvisTransform.isValidTM(0.05f));

  return desiredPelvisTransform;
}

//----------------------------------------------------------------------------------------------------------------------
// Use the arms to try to rotate the body
static void useArmsToBalance(
  const BodyBalanceUpdatePackage& pkg,
  float timeStep)
{
  BodyBalanceData* data = pkg.data;
  const BalanceParameters& balanceParameters = pkg.in->getBalanceParameters();

  const float continueArmsTime = SCALE_TIME(0.3f);

  NMP::Vector3 desiredPelvisRotationRelSupport(-data->sidewaysPosErrorFrac, 0.0f, data->forwardsPosErrorFrac);

  if (desiredPelvisRotationRelSupport.magnitude() > balanceParameters.spinThreshold)
  {
    data->continueArmsTimer = continueArmsTime;
  }
  else
  {
    data->continueArmsTimer -= timeStep;
  }

  if (data->continueArmsTimer > 0.0f)
  {
    NMP::Vector3 desiredPelvisRotation = 
      data->horizontalSupportTM.getRotatedVector(desiredPelvisRotationRelSupport);
    desiredPelvisRotation *= 
      balanceParameters.spinAmount * g_armsRotateScale * data->continueArmsTimer / continueArmsTime;
    static float imminence = SCALE_IMMINENCE(1.0f);
    static float stiffnessScale = 1.0f;
    // The request is for an rotation change in the pelvis, since the legs are supporting, this
    // will feed into swinging of the arms
    RotationRequest request(desiredPelvisRotation, SCALING_SOURCE);
    request.imminence = imminence;
    request.stiffnessScale = stiffnessScale;
#if defined(MR_OUTPUT_DEBUGGING)
    request.debugControlAmounts.setControlAmount(ER::supportControl, 1.0f);
#endif
    pkg.out->setPelvisRotationRequest(request, 1.0f);
    pkg.out->setArmsSwingOutwardsOnly(true);
    pkg.out->setArmsSpinInPhase(true);
  }
  else
  {
    data->continueArmsTimer = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// returns a value less than 1 when unstable (0 means we don't expect to be able to recover), and
// between 1 and 2 when stable.
static float getBalanceAmount(
  const BodyBalanceUpdatePackage& pkg,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const BalanceParameters& balanceParameters,
  const NMP::Vector3& COMPos,
  const NMP::Vector3& upDir)
{
  BodyBalanceData* data = pkg.data;
  // A support polygon object is used to decide when we're fallen (i.e. statically unstable).
  // Dynamic balance techniques (waving arms etc) can bring you back even when COM is outside this
  NMP::Vector3 tempPos;
  float radius = balanceParameters.footLength * 0.5f;
  NMP::Vector3 COMPosAtFeet = data->supportTM.translation() +
    (COMPos - data->supportTM.translation()).getComponentOrthogonalToDir(upDir);
  // This is +ve when outside the polygon, -ve inside
  float distanceToSupport = pkg.in->getSupportPolygon().getDistanceToPoint(
    COMPosAtFeet, upDir, radius, &tempPos);
#if defined(MR_OUTPUT_DEBUGGING)
  pkg.in->getSupportPolygon().debugDraw(
    COMPosAtFeet,
    upDir,
    radius,
    tempPos,
    NMP::Vector3(1, 0, 0, 1),
    pDebugDrawInst);
#endif

  if (distanceToSupport > 0.0f)
  {
    // The COM is outside the support polygon. Report a balance amount that decreases with distance,
    // and also according to the slope of the line joining the COM and the support poly. This is
    // used to detect when we're fallen over on the ground, and should prevent balance kicking in if
    // the feet get close to the COM (which can happen if the support polygon has quite a large
    // radius).
    float fadeDistance = balanceParameters.footLength;
    float balanceAmount = 1.0f - distanceToSupport / fadeDistance;

    // Height of the COM above the support poly
    float h = (COMPos - data->supportTM.translation()).dot(upDir);
    // The clamp just needs to avoid div by zero, and pull the result below zero.
    float invSlope = h > 0.0f ? distanceToSupport / h : 999.0f; 

    balanceAmount -= invSlope;
    return balanceAmount;
  }
  else
  {
    // COM is inside the support polygon
    return 1.0f - distanceToSupport / radius;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This decides if we need to step, or if we need force stopping the step
static void handleSteppingLogic(
  const BodyBalanceUpdatePackage& pkg,
  const uint32_t maxLimbIndex,
  BalanceParameters& balanceParameters,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const BodyBalanceFeedbackInputs* feedIn = pkg.feedIn;
  BodyBalanceOutputs* out = pkg.out;
  const BodyBalanceInputs* in = pkg.in;
  BodyBalanceData* data = pkg.data;
  const BodyFrameAPIBase* owner = pkg.owner;

  uint32_t numLimbs = owner->owner->data->numLegs + owner->owner->data->numArms;
  if (numLimbs <= 1)
    return;

  uint32_t iSteppingLimb = maxLimbIndex;
  for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
  {
    if (feedIn->getIsStepping(iLimb))
    {
      iSteppingLimb = iLimb;
      break;
    }
  }

  // See if we should prompt a step to recover the balance pose. BI ensures this is disabled if
  // stepping is disabled.
  if (
    in->getStepRecoveryParameters().stepToRecoverPose &&
    !data->needToStep &&
    iSteppingLimb == maxLimbIndex &&
    !feedIn->getIsSteppingBeingSuppressed() &&
    owner->owner->data->numLegs > 1)
  {
    bool allFeetOnGround = true;
    for (uint32_t iLeg = 0 ; iLeg < owner->owner->data->numLegs ; ++iLeg)
    {
      if (
        feedIn->getLimbEndOnGroundAmounts(iLeg) < 1.0f ||
        feedIn->getLimbSupportAmounts(iLeg) < 1.0f
        )
      {
        allFeetOnGround = false;
        break;
      }
    }
    if (allFeetOnGround)
    {
      const ER::HandFootTransform* endTMs[NetworkConstants::networkMaxNumLegs + NetworkConstants::networkMaxNumArms];
      getLimbEndTMs(pkg, &endTMs[0]);
      // We need a fixed reference point for deciding whether to make a recovery step. Choose the
      // foot that's furthest back, which will also be a foot that definitely doesn't step. 
      uint32_t iBackFootIndex = 0;
      float minForward = FLT_MAX;
      // Also calculate the minimum strength, as we want to avoid doing a recovery step with weak legs.
      float minLegStrength = FLT_MAX;
      for (uint32_t iLeg = 0 ; iLeg < owner->owner->data->numLegs ; ++iLeg)
      {
        if (data->smoothedLegStrength[iLeg] < minLegStrength)
        {
          minLegStrength = data->smoothedLegStrength[iLeg];
        }

        float forward = endTMs[iLeg]->translation().dot(data->horizontalSupportTM.frontDirection());
        if (forward < minForward)
        {
          iBackFootIndex = iLeg;
          minForward = forward;
        }
      }

      // We will calculate the desired foot positions, anchored to the back foot, and compare these
      // with the actual foot positions. The deviation of each foot will be measured, scaled
      // according to the allowed forward/side distances, and then if we need to step, we'll choose
      // the foot with the largest deviation.
      const NMP::Matrix34 backFootTM = *endTMs[iBackFootIndex];
      NMP::Matrix34 backFootRelRootInv = in->getLegEndRelativeToRoot(iBackFootIndex);
      backFootRelRootInv.invert();
      // This is the desired pelvisTM in world space, based on the the back foot being where it is.
      const NMP::Matrix34 desiredPelvisTM = backFootRelRootInv * backFootTM;
      uint32_t iLegToStep = 0;
      float maxNormalisedDistanceSq = -FLT_MAX;
      NMP::Matrix34 targetStepTM(NMP::Matrix34::kIdentity);

      for (uint32_t iLeg = 0 ; iLeg < owner->owner->data->numLegs ; ++iLeg)
      {
        if (iLeg == iBackFootIndex)
          continue;

        NMP::Matrix34 desiredFootTM = in->getLegEndRelativeToRoot(iLeg) * desiredPelvisTM;
        NMP::Matrix34 actualFootTM = *endTMs[iLeg];
        MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredFootTM, SCALE_DIST(1.0f));

        const NMP::Vector3 delta = actualFootTM.translation() - desiredFootTM.translation();
        float fwd = delta.dot(data->horizontalSupportTM.frontDirection());
        float side = delta.dot(data->horizontalSupportTM.rightDirection());
        fwd /= in->getStepRecoveryParameters().fwdDistanceToTriggerStep;
        side /= in->getStepRecoveryParameters().sidewaysDistanceToTriggerStep;
        float normalisedDistanceSq = NMP::sqr(fwd) + NMP::sqr(side);

        // avoid recovery steps when legs are being made weaker
        normalisedDistanceSq *= minLegStrength;
        if (normalisedDistanceSq > maxNormalisedDistanceSq)
        {
          maxNormalisedDistanceSq = normalisedDistanceSq;
          iLegToStep = iLeg;
          targetStepTM = desiredFootTM;
        }
      }

      if (maxNormalisedDistanceSq > 1.0f)
      {
        // Try to get the feet back to the balance.
        float timeBeforeShiftingWeight = in->getStepRecoveryParameters().timeBeforeShiftingWeight;
        float weightShiftingTime = in->getStepRecoveryParameters().weightShiftingTime;
        float standingness = NMP::clampValue((data->timeSinceLastStep - timeBeforeShiftingWeight) / weightShiftingTime, 0.0f, 1.0f);
        float supportWeighting = 1.0f - standingness;
        balanceParameters.supportPointWeightingPerLeg[iLegToStep] = NMP::clampValue(supportWeighting, 0.0f, 1.0f);
        if (standingness > 0.99f)
        {
          data->needToStep = false; // prevent a normal step
          out->setRecoveryLegStepTargetAt(iLegToStep, targetStepTM);
        }
      }
    }
  }

  // Default to not stepping, and evaluate only if we need to step
  bool allowLimbStep[BodyBalanceData::rigMaxNumSupports];
  for (uint32_t i = 0 ; i < maxLimbIndex ; ++i)
  {
    allowLimbStep[i] = false;
  }

  if (iSteppingLimb == maxLimbIndex)
  {
    // Decide which leg to step with, and set an output that allows stepping with that leg - the
    // connections will make it so the step overrides the balance. Each leg will be working out
    // where it would step to if it stepped, so we want to pick the most appropriate one.
    bool oneFootUp = false;
    for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
    {
      if (data->enableStepOnLimbs[iLimb] > 0.0f && feedIn->getLimbEndOnGroundAmounts(iLimb) < 0.5f)
      {
        oneFootUp = true;
        data->needToStep = true;
        break;
      }
    }

    // Not already stepping
    if (data->needToStep)
    {
      out->setEvaluateStep(true);
    }

    // At the moment, if a foot is in the air (but not stepping) we'll try to step, but then only
    // with a foot if all the other feet are on the ground. This might be a little restrictive when
    // there are more than two legs.
    if (data->needToStep || oneFootUp)
    {
      uint32_t iLimbToStep = maxLimbIndex;
      float longestStepLength = SCALE_DIST(0.1f);
      for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
      {
        if (!data->enableStepOnLimbs[iLimb])
          continue;
        float stepLength = feedIn->getStepLengths(iLimb);
        // Evaluate this leg for stepping by checking what all the others are doing
        for (uint32_t iLimbOther = 0 ; iLimbOther < maxLimbIndex ; ++iLimbOther)
        {
          if (!data->enableStepOnLimbs[iLimbOther])
            continue;
          if (iLimbOther == iLimb)
            continue;
          // Don't step with a foot if the other is in the air
          if (feedIn->getLimbEndOnGroundAmounts(iLimbOther) < 1.0f)
          {
            EUPHORIA_LOG_MESSAGE("Non-stepping leg %d is in the air\n", iLimbOther);
            stepLength = 0.0f;
            break;
          }
        }
        if (stepLength > longestStepLength)
        {
          longestStepLength = stepLength;
          iLimbToStep = iLimb;
        }
      }
      if (iLimbToStep != maxLimbIndex)
      {
        allowLimbStep[iLimbToStep] = true;
        EUPHORIA_LOG_MESSAGE("Stepping with limb %d\n", iLimbToStep);
        ++data->numStepsTaken;
      }
      else
      {
        // default of no step is set above
      }
    }
  }
  else
  {
    // Already stepping - note we don't need to set evaluateStep because stepping will continue once started
    data->timeSinceLastStep = 0.0f;
    // see if we need to abort a step due to none of the other legs being properly on the ground
    bool abortStep = true;
    for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
    {
      if (iLimb == iSteppingLimb)
        continue;
      if (!data->enableStepOnLimbs[iLimb])
        continue;
      // stop stepping if the supporting limb is significantly off the ground
      if (feedIn->getLimbEndOnGroundAmounts(iLimb) > 0.9f)
      {
        abortStep = false;
        break;
      }
    }
    if (!abortStep)
    {
      // continue step
      allowLimbStep[iSteppingLimb] = true;
    }
    else
    {
      EUPHORIA_LOG_MESSAGE("Aborting stepping as none of the non-stepping legs are firmly grounded\n");
      // stop all stepping - it may start again next update
    }
    if (!abortStep)
    {
      out->setStabilisePelvisAmount(g_bodyBalancePelvisStabilisationDuringStepping);
    }
  }

  // copy the limb output into leg/arm outputs
  for (uint32_t i = 0 ; i < pkg.owner->owner->data->numLegs ; ++i)
    out->setAllowLegStepAt(i, allowLimbStep[i], pkg.data->balanceAmount);
  for (uint32_t i = 0 ; i < pkg.owner->owner->data->numArms ; ++i)
    out->setAllowArmStepAt(i, allowLimbStep[i+NetworkConstants::networkMaxNumLegs]);
}

//----------------------------------------------------------------------------------------------------------------------
static void smoothLimitedAcceleration(NMP::Vector3& pos,
                               NMP::Vector3& vel,
                               float dt,
                               const NMP::Vector3& targetPos,
                               const NMP::Vector3& targetVel,
                               float maxAccel)
{

  NMP::Vector3 newVel = (targetPos - pos) / dt;

  // prevent overshoot by calculating the max speed we can have in the direction towards
  // the target given that we cannot decelerate faster than maxAccel
  NMP::Vector3 deltaVel = newVel - vel;

  NMP::Vector3 translationDir = targetPos - pos;
  float distToTarget = translationDir.normaliseGetLength();
  float curVelAlongTranslation = vel.dot(translationDir);
  float targetVelAlongTranslation = targetVel.dot(translationDir);

  if (curVelAlongTranslation > targetVelAlongTranslation)
  {
    float timeToCatchUp = distToTarget / (curVelAlongTranslation - targetVelAlongTranslation);
    float maxCurVelAlongTranslation = targetVelAlongTranslation + timeToCatchUp * maxAccel;
    if (curVelAlongTranslation > maxCurVelAlongTranslation)
    {
      // replace the old component along the translation with the new max value
      newVel += translationDir * (maxCurVelAlongTranslation - newVel.dot(translationDir));
      deltaVel = newVel - vel;
    }
  }

  // clamp the acceleration
  float deltaVelMag = deltaVel.magnitude();
  if (deltaVelMag > maxAccel * dt)
    deltaVel *= maxAccel * dt / deltaVelMag;

  vel += deltaVel;
  pos += vel * dt;
}

//----------------------------------------------------------------------------------------------------------------------
// Note that the support frame is defined as the same as the spine and head - i.e. x fwd, y up, z right.
static void clampPelvisRelSupport(
  const BodyBalanceUpdatePackage& pkg,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const float timeStep)
{
  BodyBalanceData* data = pkg.data;
  const BodyBalanceInputs* in = pkg.in;
  const BodyFrameAPIBase* owner = pkg.owner;

  // Smooth the inputs
  // Now have targetPelvisRelSupport. However, it may be rather "extreme", or have changed very
  // suddenly, so limit the changes.
  if (in->getPelvisRelSupportImportance() > 0.0f)
  {
    const BalancePoseClamping& balancePoseClamping = in->getBalancePoseClamping();

    if (!balancePoseClamping.enablePoseClamping)
    {
      // If there's no clamping we still need to make sure we'll be ready if clamping does get
      // enabled.
      data->targetPelvisRelSupport = in->getPelvisRelSupport();
      data->targetPelvisRelSupportPosition = data->targetPelvisRelSupport.translation();
      data->targetPelvisRelSupportPositionRate.setToZero();
      data->targetPelvisRelSupportOrientation = data->targetPelvisRelSupport.toRotationVector();
      data->targetPelvisRelSupportOrientationRate.setToZero();
      data->initialisedTargetPelvisRelSupport = true;
      return;
    }

    if (!data->initialisedTargetPelvisRelSupport)
    {
      // Initialise with the current pelvis relative to support, taken from physics
      data->initialisedTargetPelvisRelSupport = true;
      NMP::Matrix34 invSupportTM = data->supportTM;
      invSupportTM.invertFast();
      NMP::Matrix34 currentPelvisRelSupportTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM * invSupportTM;
      data->targetPelvisRelSupportPosition = currentPelvisRelSupportTM.translation();
      data->targetPelvisRelSupportOrientation = currentPelvisRelSupportTM.toRotationVector();
      data->targetPelvisRelSupportPositionRate.setToZero();
      data->targetPelvisRelSupportOrientationRate.setToZero();
      data->lastTargetPelvisRelSupportPosition = data->targetPelvisRelSupportPosition;
    }

    NMP_ASSERT(balancePoseClamping.pelvisPositionChangeTimescale >= 0.0f);
    NMP_ASSERT(balancePoseClamping.pelvisOrientationChangeTimescale >= 0.0f);

    // Smooth the position change by clamping the acceleration to a (rather arbitrary) fraction of
    // gravity
    smoothLimitedAcceleration(
      data->targetPelvisRelSupportPosition,
      data->targetPelvisRelSupportPositionRate,
      timeStep,
      in->getPelvisRelSupport().translation(),
      NMP::Vector3::InitZero,
      0.5f * owner->owner->data->gravityMagnitude);

    NMP::SmoothCD(
      data->targetPelvisRelSupportOrientation,
      data->targetPelvisRelSupportOrientationRate,
      timeStep,
      in->getPelvisRelSupport().toRotationVector(),
      balancePoseClamping.pelvisOrientationChangeTimescale);
    data->targetPelvisRelSupport.fromRotationVector(data->targetPelvisRelSupportOrientation);
    data->targetPelvisRelSupport.translation() = data->targetPelvisRelSupportPosition;

    // Clamp in roll/pitch/yaw space. Note that:
    // +ve pitch leans backwards
    // +ve roll leans to the right
    // +ve yaw turns to the left

    // Note that conversion from/to Euler is not terribly quick
    NMP::Vector3 rollYawPitch = data->targetPelvisRelSupport.toEulerXYZ();
    float height = data->targetPelvisRelSupport.translation().y;

    data->targetPelvisRelSupport.translation().y -= height;

    // Rather than just clamping to the "box" of allowed perturbations, which results in a lot of
    // destabilisation when more than one parameter is near the limits, clamp to a 4D sphere so that
    // the allowed pitch range reduces when crouching, etc.

    // Normalise the deviations relative to the relevant limit
    float rollAmount = rollYawPitch.x > 0.0f ? 
      rollYawPitch.x / balancePoseClamping.maxPelvisRoll : -rollYawPitch.x / balancePoseClamping.minPelvisRoll;
    float yawAmount  = rollYawPitch.y > 0.0f ? 
      rollYawPitch.y / balancePoseClamping.maxPelvisYaw : -rollYawPitch.y / balancePoseClamping.minPelvisYaw;
    float pitchAmount = rollYawPitch.z > 0.0f ? 
      rollYawPitch.z / balancePoseClamping.maxPelvisPitch : -rollYawPitch.z / balancePoseClamping.minPelvisPitch;
    float heightAmount = (balancePoseClamping.maxPelvisHeight - height) / 
      (balancePoseClamping.maxPelvisHeight - balancePoseClamping.minPelvisHeight);

    float deviation = NMP::fastSqrt(
      NMP::sqr(rollAmount) + NMP::sqr(yawAmount) + NMP::sqr(pitchAmount) + NMP::sqr(heightAmount));

    if (balancePoseClamping.dynamicClamping)
    {
      // Also reduce the allowed range according to how stable we are - this helps when leaning back
      // whilst crouching, for example.
      float legIKOrientationError = owner->owner->data->bodyLimbSharedState.m_averageOfLegsIKOrientationError
          / NMP::degreesToRadians(10.0f);
      NMP::SmoothCD(
        data->smoothedLegIKOrientationError,
        data->smoothedLegIKOrientationErrorRate,
        timeStep,
        legIKOrientationError,
        SCALE_TIME(2.0f));
      deviation += data->smoothedLegIKOrientationError;
    }

    if (deviation > 1.0f)
    {
      rollAmount /= deviation;
      yawAmount /= deviation;
      pitchAmount /= deviation;
      heightAmount /= deviation;
    }

    rollYawPitch.x = rollAmount > 0.0f ? 
      rollAmount * balancePoseClamping.maxPelvisRoll : -rollAmount * balancePoseClamping.minPelvisRoll;
    rollYawPitch.y = yawAmount > 0.0f ? 
      yawAmount * balancePoseClamping.maxPelvisYaw : -yawAmount * balancePoseClamping.minPelvisYaw;
    rollYawPitch.z = pitchAmount > 0.0f ? 
      pitchAmount * balancePoseClamping.maxPelvisPitch : -pitchAmount * balancePoseClamping.minPelvisPitch;
    height = balancePoseClamping.maxPelvisHeight - 
      heightAmount * (balancePoseClamping.maxPelvisHeight - balancePoseClamping.minPelvisHeight);

    data->targetPelvisRelSupport.fromEulerXYZ(rollYawPitch);
    data->targetPelvisRelSupport.translation().y += height;

    NMP::Matrix34 desiredPelvisTM = data->targetPelvisRelSupport * data->supportTM;
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredPelvisTM, SCALE_DIST(2.0f));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This also returns the maximum limb index that is used for support/stepping - this will normally
// be much smaller than the max number of limbs.
template<typename T>
uint32_t setSupportAndStepEnabledOnLimbs(T& pkg)
{
  float enableSupportOnLegs = pkg.in->getEnableSupportOnLegs();
  float enableStepOnLegs = pkg.in->getEnableStepOnLegs();
  float enableSupportOnArms = pkg.in->getEnableSupportOnArms();
  float enableStepOnArms = pkg.in->getEnableStepOnArms();
  for (size_t iLeg = 0 ; iLeg < pkg.owner->owner->data->numLegs ; ++iLeg)
  {
    pkg.data->enableSupportOnLimbs[iLeg] = enableSupportOnLegs;
    pkg.data->enableStepOnLimbs[iLeg] = enableStepOnLegs;
  }
  for (size_t iArm = 0 ; iArm < pkg.owner->owner->data->numArms ; ++iArm)
  {
    pkg.data->enableSupportOnLimbs[iArm + NetworkConstants::networkMaxNumLegs] = enableSupportOnArms;
    pkg.data->enableStepOnLimbs[iArm + NetworkConstants::networkMaxNumLegs] = enableStepOnArms;
  }
  uint32_t maxLimbIndex =
    (enableSupportOnArms || enableStepOnArms) ?
      NetworkConstants::networkMaxNumLegs + pkg.owner->owner->data->numArms :
      pkg.owner->owner->data->numLegs;
  return maxLimbIndex;
}

//----------------------------------------------------------------------------------------------------------------------
// This should be called if we give up balancing and want to clear our internal balance state just
// before an early out
static void clearBalanceData(const BodyBalanceUpdatePackage& pkg)
{
  for (uint32_t iLimb = 0 ; iLimb < BodyBalanceData::rigMaxNumSupports ; ++iLimb)
  {
    pkg.data->limbWeights[iLimb] = 0.0f;
    pkg.data->supportPointWeightingPerLimb[iLimb] = 0.0f;
  }
  pkg.data->balanceAmount = 0.0f;
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
void BodyBalanceUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  if (in->getBalanceParametersImportance() == 0.0f) // can happen if the BI is disabled
    return;
  // This is non-const because it (the support weightings) gets modified
  BalanceParameters balanceParameters = in->getBalanceParameters();

  // Need to call this at the start of update and feedback because limb support/step may get
  // enabled/disabled
  const uint32_t maxLimbIndex = setSupportAndStepEnabledOnLimbs(*this);

  // Pose clamping
  clampPelvisRelSupport(*this, pDebugDrawInst, timeStep);

  // Jump start some smoothed values
  if (data->isFirstUpdate)
  {
    data->isFirstUpdate = false;
    for (uint32_t i = 0 ; i < owner->owner->data->numLegs ; ++i)
    {
      float supportStrength = 1.0f - balanceParameters.balanceWeaknessPerLeg[i];
      data->smoothedLegStrength[i] = NMP::clampValue(supportStrength, 0.0f, 1.0f);
    }
    for (uint32_t i = 0 ; i < owner->owner->data->numArms ; ++i)
    {
      float supportStrength = 1.0f - balanceParameters.balanceWeaknessPerArm[i];
      data->smoothedArmStrength[i] = NMP::clampValue(supportStrength, 0.0f, 1.0f);
    }

  }

  // Update a few things that get used here
  const NMP::Vector3 COMPos = owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition();
  float staticBalanceAmount = NMP::clampValue(
    getBalanceAmount(*this, pDebugDrawInst, balanceParameters, COMPos, -data->effectiveGravityDir), 0.0f, 2.0f);
  data->needToStep = staticBalanceAmount < 1.0f;

  // Keep track of when we last stepped. This will be zeroed in the stepping logic code (but we
  // increment it here since that logic is not called if we've fallen over, and we can't step when
  // fallen).
  data->timeSinceLastStep += timeStep;

  // First check to see if we consider ourselves balanced at all - we could be statically unbalanced
  // but still OK, because we're stepping.
  float oldBalanceAmount = data->balanceAmount;
  data->balanceAmount = NMP::clampValue(
    getBalanceAmount(*this, pDebugDrawInst, balanceParameters, COMPos, owner->owner->data->up), 0.0f, 1.0f);
  // The limbs might be externally supported, in which case we treat things as if we're being helped
  // to balance.
  if (data->balanceAmount == 0.0f && !feedIn->getIsSupportedByConstraint())
  {
    if (oldBalanceAmount > 0.0f)
      EUPHORIA_LOG_MESSAGE("Fallen over\n");
    clearBalanceData(*this);
    return;
  }

  // up in the air?
  if (!feedIn->getIsSupportedByConstraint())
  {
    float onGroundAmount = 0.0f;
    for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
      onGroundAmount += feedIn->getLimbEndOnGroundAmounts(iLimb);
    if (onGroundAmount == 0.0f)
    {
      if (oldBalanceAmount > 0.0f)
        EUPHORIA_LOG_MESSAGE("In the air - give up balancing\n");
      clearBalanceData(*this);
      return;
    }
  }

  // Now, we're going to try to balance
  if (staticBalanceAmount > 0.0f || data->needToStep)
  {
    NMP::Matrix34 desiredPelvisTransform;

    // decide whether to request a start/stop of stepping
    handleSteppingLogic(*this, maxLimbIndex, balanceParameters, pDebugDrawInst);

    // Calculate the main variables used in analysing the current balance situation
    BalanceUpdateData balanceUpdateData;
    calculateBalanceUpdateData(*this, pDebugDrawInst, balanceParameters, balanceUpdateData);

    desiredPelvisTransform = getBalanceDesiredPelvisTransform(
      *this,
      pDebugDrawInst,
      balanceParameters,
      balanceUpdateData);

    // Arms. Waving arms whilst stepping tends to destabilise... but it looks good!
    if (balanceParameters.spinAmount > 0.0f && data->balanceAmount > 0.0f && !feedIn->getIsDoingRecoveryStep())
    {
      useArmsToBalance(*this, timeStep);
    }

    out->setDesiredPelvisTM(desiredPelvisTransform, 1.0f);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredPelvisTransform, SCALE_DIST(0.5f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, in->getPelvisRelSupport() * data->supportTM, SCALE_DIST(0.2f));

    // We only need to set the desired spine end (chest) TM if we're supporting with the arms.
    if (in->getEnableSupportOnArms())
    {
      const NMP::Matrix34& spinePoseEndRelativeToRoot =
        (in->getSpinePoseEndRelativeToRootImportance(0) > 0.0f) ?
          in->getSpinePoseEndRelativeToRoot(0) :
          owner->owner->data->defaultPoseEndsRelativeToRoot[MyNetworkData::firstSpineNetworkIndex];
      NMP::Matrix34 desiredChestTransform = spinePoseEndRelativeToRoot * desiredPelvisTransform;
      out->setDesiredChestTM(desiredChestTransform, 1.0f);
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredChestTransform, SCALE_DIST(10.0f));
    }

    out->setAverageSupportingLimbContactNormal(data->averageSupportingLimbContactNormal);

    // Set the target pelvis TM - what we want to actually achieve (not necessarily what we're
    // driving towards).
    NMP::Matrix34 targetPelvisTM = data->targetPelvisRelSupport * data->horizontalSupportTM;
    NMP::Vector3 horCOMOffsetFromPelvis = (balanceUpdateData.balancePoint - balanceUpdateData.pelvisMatrix.translation()).
      getComponentOrthogonalToDir(data->effectiveGravityDir);
    targetPelvisTM.translation() -= horCOMOffsetFromPelvis;

    out->setTargetPelvisTM(targetPelvisTM);

  } // staticBalanceAmount > 0 || data->needToStep
  else
  {
    // Might get here if we can't step (i.e. not enabled) and very unbalanced, in which case we give
    // up.
    clearBalanceData(*this);
    return;
  }

  // At this point we should have set a balance target, but we also want to output a strength.

  // disable balance when the last limb has left the ground completely
  float maxLimbOnGroundAmount = 0.0f;
  for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
  {
    float limbOnGroundAmount = feedIn->getLimbEndOnGroundAmounts(iLimb) * data->enableSupportOnLimbs[iLimb];
    if (limbOnGroundAmount > maxLimbOnGroundAmount)
    {
      maxLimbOnGroundAmount = limbOnGroundAmount;
    }
  }

  // Smooth the strength changes
  float strengthSmoothTime = SCALE_TIME(1.0f);
  for (uint32_t i = 0 ; i < owner->owner->data->numLegs ; ++i)
  {
    const uint32_t iLimb = i;

    float supportStrength = 1.0f - balanceParameters.balanceWeaknessPerLeg[i];
    NMP::SmoothCD(data->smoothedLegStrength[i], data->smoothedLegStrengthRate[i], timeStep, supportStrength, strengthSmoothTime);
    data->smoothedLegStrength[i] = NMP::clampValue(data->smoothedLegStrength[i], 0.0f, 2.0f);
    supportStrength = data->smoothedLegStrength[i] * maxLimbOnGroundAmount;

    out->setLegSupportStrengthScalesAt(i, supportStrength);
    // avoid putting the CoM over a weak leg
    data->limbWeights[iLimb] = supportStrength * supportStrength * data->enableSupportOnLimbs[iLimb];
    data->supportPointWeightingPerLimb[iLimb] = balanceParameters.supportPointWeightingPerLeg[i];
  }
  for (uint32_t i = 0 ; i < owner->owner->data->numArms ; ++i)
  {
    const uint32_t iLimb = NetworkConstants::networkMaxNumLegs + i;

    float supportStrength = 1.0f - balanceParameters.balanceWeaknessPerArm[i];
    NMP::SmoothCD(data->smoothedArmStrength[i], data->smoothedArmStrengthRate[i], timeStep, supportStrength, strengthSmoothTime);
    data->smoothedArmStrength[i] = NMP::clampValue(data->smoothedArmStrength[i], 0.0f, 2.0f);
    supportStrength = data->smoothedArmStrength[i] * maxLimbOnGroundAmount;

    if (data->enableSupportOnLimbs[iLimb] > 0.0f)
    {
      // Only output a support strength if support on arms is enabled
      out->setArmSupportStrengthScalesAt(i, supportStrength);
    }
    // avoid putting the CoM over a weak arm
    data->limbWeights[iLimb] = supportStrength * supportStrength * data->enableSupportOnLimbs[iLimb];
    data->supportPointWeightingPerLimb[iLimb] = balanceParameters.supportPointWeightingPerArm[i];
  }

  // Set some additional outputs used when we're balancing
  out->setGroundVelocity(data->groundVel);
  out->setSupportTM(data->horizontalSupportTM);

  out->setSpineDampingIncrease(1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
// BodyBalanceFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void BodyBalanceFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  // Need to call this at the start of update and feedback because limb support/step may get
  // enabled/disabled
  const uint32_t maxLimbIndex = setSupportAndStepEnabledOnLimbs(*this);
  const ER::HandFootTransform* endTMs[NetworkConstants::networkMaxNumLegs + NetworkConstants::networkMaxNumArms];
  getLimbEndTMs(*this, &endTMs[0]);

  data->isStepping = false;
  for (uint32_t iLimb = 0; iLimb < maxLimbIndex; ++iLimb)
  {
    if (feedIn->getIsStepping(iLimb))
    {
      data->isStepping = true;
      break;
    }
  }

  const BalanceParameters& balanceParameters = in->getBalanceParameters();

  NMP::Vector3 lastSupportPos = data->supportTM.translation();
  if (data->needToInitialiseSupportTM)
  {
    // On the first feedback update the behaviour interface won't have updated, so instead get the
    // info from the balanceBehaviourFeedback module. Don't do that normally because on normal
    // updates it can be a frame out of date.
    if (feedIn->getEnableSupportOnLegs() && feedIn->getEnableSupportOnArms())
    {
      if (owner->feedIn->getAverageOfSupportingHandsAndFeetImportance() > 0.0f)
      {
        data->supportTM.translation() = owner->feedIn->getAverageOfSupportingHandsAndFeet();
      }
      else
      {
        data->supportTM.translation() = owner->owner->data->bodyLimbSharedState.m_averageOfHandsAndFeetTM.translation();
      }
    }
    else if (feedIn->getEnableSupportOnArms())
    {
      if (owner->feedIn->getAverageOfSupportingHandsImportance() > 0.0f)
      {
        data->supportTM.translation() = owner->feedIn->getAverageOfSupportingHands();
      }
      else
      {
        data->supportTM.translation() = owner->owner->data->bodyLimbSharedState.m_averageOfHandsTM.translation();
      }
    }
    else if (owner->feedIn->getAverageOfSupportingFeetImportance() > 0.0f)
    {
      data->supportTM.translation() = owner->feedIn->getAverageOfSupportingFeet();
    }
    else
    {
      data->supportTM.translation() = owner->owner->data->bodyLimbSharedState.m_averageOfFeetTM.translation();
    }
    
    lastSupportPos = data->supportTM.translation();
  }
  else
  {
    float totalWeight = 0.0f;
    for (size_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
    {
      totalWeight += data->limbWeights[iLimb] * data->supportPointWeightingPerLimb[iLimb];
    }
    if (totalWeight > ER::g_minImportanceForJunction)
    {
      data->supportTM.translation().setToZero();
      float minY = SCALE_DIST(9999999.0f);
      for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
      {
        if (data->enableSupportOnLimbs[iLimb] > 0.0f)
        {
          if (endTMs[iLimb]->translation().dot(owner->owner->data->up) < minY)
          {
            minY = endTMs[iLimb]->translation().dot(owner->owner->data->up);
          }
          data->supportTM.translation() += endTMs[iLimb]->translation() *
            data->limbWeights[iLimb] * data->supportPointWeightingPerLimb[iLimb];
        }
      }
      data->supportTM.translation() /= totalWeight;
      data->supportTM.translation() += owner->owner->data->up *
        (minY - data->supportTM.translation().dot(owner->owner->data->up));
    }
    else
    {
      if (in->getEnableSupportOnLegs() && in->getEnableSupportOnArms())
      {
        if (owner->feedIn->getAverageOfSupportingHandsAndFeetImportance() > 0.0f)
        {
          data->supportTM.translation() = owner->feedIn->getAverageOfSupportingHandsAndFeet();
        }
        else
        {
          data->supportTM.translation() = owner->owner->data->bodyLimbSharedState.m_averageOfHandsAndFeetTM.translation();
        }
      }
      else if (in->getEnableSupportOnArms())
      {
        if (owner->feedIn->getAverageOfSupportingHandsImportance() > 0.0f)
        {
          data->supportTM.translation() = owner->feedIn->getAverageOfSupportingHands();
        }
        else
        {
          data->supportTM.translation() = owner->owner->data->bodyLimbSharedState.m_averageOfHandsTM.translation();
        }
      }
      else if (owner->feedIn->getAverageOfSupportingFeetImportance() > 0.0f)
      {
        data->supportTM.translation() = owner->feedIn->getAverageOfSupportingFeet();
      }
      else
      {
        data->supportTM.translation() = owner->owner->data->bodyLimbSharedState.m_averageOfFeetTM.translation();
      }
    }
  }
  NM_VALIDATOR(Matrix34Valid(data->supportTM), "supportTM");

  // Calculate supportTM orientation based on the foot directions and COM motion, with smoothing

  // Calculate the motion of the ground under the feet/hands, and the floor normal
  const ContactInfo* limbEndContacts[NetworkConstants::networkMaxNumLimbs];
  getLimbEndContacts(*this, limbEndContacts);
  NMP::Vector3 groundVel = NMP::Vector3::InitZero;
  float totalWeight = 0.0f;
  data->averageSupportingLimbContactNormal.setToZero();
  data->groundContactPoint.setToZero();
  for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
  {
    if (data->enableSupportOnLimbs[iLimb] != 0.0f)
    {
      if (limbEndContacts[iLimb]->inContact)
      {
        float weight = data->limbWeights[iLimb];
        if (weight > 0.0f)
        {
          weight += 1e-8f; // Tiny weights do not normalise properly!
        }
        totalWeight += weight;
        groundVel += limbEndContacts[iLimb]->lastVelocity * weight;
        data->averageSupportingLimbContactNormal += limbEndContacts[iLimb]->lastNormal * weight;
        data->groundContactPoint += endTMs[iLimb]->translation() * weight;
      }
    }
  }
  if (totalWeight > 0.0f)
  {
    groundVel /= totalWeight;
    data->averageSupportingLimbContactNormal.normalise();
    data->groundContactPoint /= totalWeight;
  }
  else
  {
    data->averageSupportingLimbContactNormal = owner->owner->data->up;
    data->groundContactPoint = data->supportTM.translation();
  }

  NMP::Vector3 groundAccel = (groundVel - data->lastGroundVel) / timeStep;
  data->lastGroundVel = groundVel;

  /// COMVel isn't very good on transitioning into balance from SK, so effectively disable its
  /// effects.
  if (data->needToInitialiseSupportTM)
  {
    data->COMVel = groundVel;
  }
  else
  {
    data->COMVel = owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();
  }

  // A small amount of smoothing is needed because otherwise the jitter from standing on smallish
  // objects messes up the balance horribly.
  float groundSmoothTime = SCALE_TIME(0.05f);
  SmoothCD(data->groundVel, data->groundVelRate, timeStep, groundVel, groundSmoothTime);
  SmoothCD(data->groundAccel, data->groundAccelRate, timeStep, groundAccel, groundSmoothTime);

  // Want to balance against an effective gravity direction that includes the tendency to decelerate
  // towards the floor motion
  data->effectiveGravityDir = owner->owner->data->gravity;

  // Lean into motion caused by accelerating floors. Remove the component along the acceleration
  // direction since it doesn't affect us when not leaning - i.e. if we're on a platform that's
  // accelerating upwards, we don't need to crouch/straighten.
  NMP::Vector3 horGroundAccel = data->groundAccel.getComponentOrthogonalToDir(owner->owner->data->down);
  data->effectiveGravityDir -= horGroundAccel;

  // When moving relative to the floor (mainly after stepping), we want to decelerate, so lean away
  // from the ground-relative motion. Note that it's tempting to calculate this exactly, assuming an
  // inverted pendulum, but that results in fairly big target angles at low velocity, triggering the
  // stepping etc. Also add in the balance velocity error as a target. Don't lean to decelerate when
  // stepping as that results in very lumpy stepping.
  float decelerateFrac = (data->isStepping) ? 0.0f : 1.0f;
  data->effectiveGravityDir += ((data->COMVel - data->groundVel) * decelerateFrac) *
    owner->owner->data->gravityMagnitude * balanceParameters.decelerationAmount;

  // Normalise the effective gravity direction and debug draw it
  data->effectiveGravityDir.normalise();

  // Calculate velocity of the CoM relative to the feet
  NMP::Vector3 COMVelRelGround = data->COMVel - data->groundVel;
  data->horCOMVelRelGround = COMVelRelGround.getComponentOrthogonalToDir(data->effectiveGravityDir);

  // Draw some useful things
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
    owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition(),
    owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity(), NMP::Colour::GREEN); // Green = CoM vel
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
    owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition(),
    data->horCOMVelRelGround, NMP::Colour::BLUE); // Blue = CoM vel relative to feet

  // Calculate the support direction. Note that unless supporting on hands and feet, for now we just
  // use the foot/hand directions even though they might not even be limited, so won't always give us a
  // sensible orientation. Really should use the relative positions too - perhaps comparing to the
  // balance pose? MORPH-11182
  NMP::Vector3 supportForward(NMP::Vector3::InitZero);
  if (data->needToInitialiseSupportTM)
  {
    for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
    {
      supportForward += endTMs[iLimb]->pointDirection();
    }
  }
  else if (
    in->getEnableSupportOnLegs() &&
    in->getEnableSupportOnArms() &&
    owner->feedIn->getAverageOfSupportingHandsImportance() > 0.0f &&
    owner->feedIn->getAverageOfSupportingFeetImportance() > 0.0f)
  {
    // This assumes left/right symmetry, which will often be the case. However, perhaps we should
    // also use the supportPointWeightingPerLimb values. See MORPH-11183
    supportForward = owner->feedIn->getAverageOfSupportingHands() - owner->feedIn->getAverageOfSupportingFeet();
  }
  else
  {
    for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
    {
      // Allow small minimum weighting so we get a direction even when all feet are up
      supportForward += endTMs[iLimb]->pointDirection() *
        NMP::maximum(feedIn->getLimbEndOnGroundAmounts(iLimb), 0.1f) * data->enableSupportOnLimbs[iLimb];
    }
  }

  // Bias the forwards direction to follow the step direction - the amount is not hugely important.
  if (data->isStepping)
  {
    supportForward += 2.0f * feedIn->getStepDir();
  }

  if (supportForward.magnitudeSquared() > 0.0f && data->needToInitialiseSupportTM)
  {
    data->needToInitialiseSupportTM = false;
    data->supportTM.frontDirection() = supportForward;
  }
  else
  {
    // Smooth the support direction
    NMP::SmoothCD(data->supportTM.frontDirection(), data->supportForwardRate, timeStep,
      supportForward, SCALE_TIME(g_bodyBalanceSupportDirSmoothTime));
  }

  NMP::Vector3 targetSupportForward = data->supportTM.frontDirection();
  data->supportTM.createFromUpAlignedByForward(
    -data->effectiveGravityDir, targetSupportForward, data->supportTM.translation());

  // Finally offset the supportTM according to the behaviour offsets
  data->supportTM.translation() += data->supportTM.frontDirection() * balanceParameters.supportFwdOffset;
  data->supportTM.translation() += data->supportTM.rightDirection() * balanceParameters.supportRightOffset;

  // also apply the requested horizontal offset
  float balanceUsingAnimationHorizontalPelvisOffsetAmount = in->getBalanceUsingAnimationHorizontalPelvisOffsetAmount();
  if (in->getPelvisRelSupportImportance() > 0.0f && data->initialisedTargetPelvisRelSupport)
  {
    const NMP::Vector3& pelvisRelSupport = data->targetPelvisRelSupport.translation();
    data->supportTM.translation() += data->supportTM.frontDirection() * 
      (pelvisRelSupport.x * (1.0f - balanceUsingAnimationHorizontalPelvisOffsetAmount));
    data->supportTM.translation() += data->supportTM.rightDirection() * 
      (pelvisRelSupport.z * (1.0f - balanceUsingAnimationHorizontalPelvisOffsetAmount));
  }

  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, data->supportTM, SCALE_DIST(2.0f));

  // Also calculate a horizontal version of the support TM.
  data->horizontalSupportTM.createFromUpAlignedByForward(
    owner->owner->data->up, data->supportTM.frontDirection(), data->supportTM.translation());

  // output support position and movement - but only if balancing to some degree. However, we
  // calculate it always for internal use (seems rather expensive).
  if (data->balanceAmount > 0.0f || feedIn->getIsSupportedByConstraint())
  {
    // Remove the tilting component from the supportTM, since that's only used internally.
    feedOut->setSupportTM(data->horizontalSupportTM);

    // Calculate/output the velocity of the supporting parts (this isn't meant to
    // be the velocity of the supporting surface!).
    NMP::Vector3 supportVelocity = (data->supportTM.translation() - lastSupportPos) / timeStep;
    feedOut->setSupportVelocity(supportVelocity);
  }

  float maxLimbOnGroundAmount = 0.0f;
  if (!feedIn->getIsSupportedByConstraint()) // limbs may also be externally supported
  {
    if (data->balanceAmount > 0.0f)
    {
      for (uint32_t iLimb = 0 ; iLimb < maxLimbIndex ; ++iLimb)
      {
        if (data->enableSupportOnLimbs[iLimb] > 0.0f)
        {
          // The limb end on ground amount will gradually ramp down if the foot lifts off the floor.
          // This can be good - it stops balance giving up completely just because he lifts off the
          // floor a bit. However, if he loses the floor entirely then he should claim to be not
          // balanced as quick as possible. This is done in the Leg code.
          maxLimbOnGroundAmount = NMP::maximum(feedIn->getLimbEndOnGroundAmounts(iLimb), maxLimbOnGroundAmount);
        }
      }
    }
  }
  else
  {
    maxLimbOnGroundAmount = 1.0f;
  }
  feedOut->setBalanceAmount(data->balanceAmount * maxLimbOnGroundAmount);

  if (pDebugDrawInst)
  {
    // debug draw the support amount
    float balanceAmount = feedOut->getBalanceAmount();
    NMP::Vector3 a = owner->owner->data->spineLimbSharedStates[0].m_endTM.translation()
      + SCALE_DIST(owner->owner->data->up);
    NMP::Vector3 b = a + SCALE_DIST(owner->owner->data->up) * balanceAmount;
    NMP::Vector3 c = a + SCALE_DIST(owner->owner->data->up);
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, a, b, NMP::Colour::GREEN); // green for good
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, b, c, NMP::Colour::RED); // red for bad
    char txt[32];
    NMP_SPRINTF(txt, 32, "%5.2f", balanceAmount);
    MR_DEBUG_DRAW_TEXT(pDebugDrawInst, c, txt, NMP::Colour::GREEN);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void BodyBalance::entry()
{
  data->isFirstUpdate = true;
  data->continueArmsTimer = 0.0f;
  data->supportTM.identity();
  data->horizontalSupportTM.identity();
  data->groundContactPoint.setToZero();
  data->supportForwardRate.setToZero();
  data->groundVel.setToZero();
  data->groundVelRate.setToZero();
  data->lastGroundVel.setToZero();
  data->groundAccel.setToZero();
  data->groundAccelRate.setToZero();
  data->COMVel.setToZero();
  data->horCOMVelRelGround.setToZero();
  data->effectiveGravityDir = owner->owner->data->down;
  data->averageSupportingLimbContactNormal = owner->owner->data->up;
  data->balanceAmount = 1.0f;
  data->numStepsTaken = 0;
  data->timeSinceLastStep = 0.0f; // A bit arbitrary
  data->needToInitialiseSupportTM = true;

  data->sidewaysPosErrorFrac = data->forwardsPosErrorFrac = 0.0f;

  int iLimb = 0;
  for (size_t iLeg = 0 ; iLeg < NetworkConstants::networkMaxNumLegs ; ++iLeg, ++iLimb)
  {
    data->enableSupportOnLimbs[iLimb] = 0.0f;
    data->enableStepOnLimbs[iLimb] = 0.0f;
    data->smoothedLegStrength[iLeg] = 1.0f;
    data->smoothedLegStrengthRate[iLeg] = 0.0f;
  }
  for (size_t iArm = 0 ; iArm < NetworkConstants::networkMaxNumArms ; ++iArm, ++iLimb)
  {
    data->enableSupportOnLimbs[iLimb] = 0.0f;
    data->enableStepOnLimbs[iLimb] = 0.0f;
    data->smoothedArmStrength[iArm] = 1.0f;
    data->smoothedArmStrengthRate[iArm] = 0.0f;
  }

  data->initialisedTargetPelvisRelSupport = false;
  data->targetPelvisRelSupportPosition.setToZero();
  data->targetPelvisRelSupportOrientation.setToZero();
  data->targetPelvisRelSupportPositionRate.setToZero();
  data->targetPelvisRelSupportOrientationRate.setToZero();
  data->targetPelvisRelSupport.identity();

  data->smoothedLegIKOrientationError = 0.0f;
  data->smoothedLegIKOrientationErrorRate = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

