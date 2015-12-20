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
#include "HoldBehaviourInterface.h"
#include "Behaviours/HoldBehaviour.h"
#include "euphoria/erCharacter.h"
#include "NetworkDescriptor.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const HoldBehaviourData& params =
    ((HoldBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Hold)))->getParams();

  bool doHold = params.getDoHoldCP();
  if (doHold)
  {
    // per limb toggles
    for (uint32_t i = 0 ; i < owner->data->numArms; ++i)
    {
      out->setHoldPerArmImportanceAt(i, params.getHoldImportanceCP(i), 1.0f);
    }

    GrabControl grabControl;

    // Constraint params
    grabControl.constraintParams.initialise(SCALING_SOURCE);
    grabControl.constraintParams.constrainOnContact = params.getHoldOnContact();
    grabControl.constraintParams.disableCollisions = params.getDisableCollisions();
    grabControl.constraintParams.lockedLinearDofs = (uint16_t) params.getLockedLinearDofs();
    grabControl.constraintParams.lockedAngularDofs = (uint16_t) params.getLockedAngularDofs();
    grabControl.constraintParams.createDistance = SCALE_DIST(params.getCreateAtDistance());
    grabControl.constraintParams.destroyDistance = SCALE_DIST(params.getDestroyAtDistance());
    grabControl.constraintParams.startOrientationAngle = NMP::degreesToRadians(params.getEnableOrientationAtAngle());
    grabControl.constraintParams.stopOrientationAngle = NMP::degreesToRadians(params.getDisableOrientationAtAngle());
    NMP_ASSERT(grabControl.constraintParams.startOrientationAngle >= 0.0f);
    NMP_ASSERT(grabControl.constraintParams.stopOrientationAngle >= 0.0f);
    grabControl.constraintParams.disableAngularDofsUntilHanging = params.getDisableAngularDofsUntilHanging();

    // Hold timer
    grabControl.holdTimer.minHoldPeriod = SCALE_TIME(params.getMinHoldPeriod());
    grabControl.holdTimer.maxHoldPeriod = SCALE_TIME(params.getMaxHoldPeriod());
    if (grabControl.holdTimer.maxHoldPeriod <= 0.0f)
    {
      grabControl.holdTimer.maxHoldPeriod = FLT_MAX;
    }
    grabControl.holdTimer.noHoldPeriod = SCALE_TIME(params.getNoHoldPeriod());
    NMP_ASSERT(grabControl.holdTimer.minHoldPeriod >= 0.0f);
    NMP_ASSERT(grabControl.holdTimer.maxHoldPeriod >= 0.0f);
    NMP_ASSERT(grabControl.holdTimer.noHoldPeriod >= 0.0f);

    // Chest control
    grabControl.grabChestParams.imminence = SCALE_IMMINENCE(params.getChestControlImminence());
    grabControl.grabChestParams.stiffnessScale = SCALE_IMMINENCE(params.getChestControlStiffnessScale());
    grabControl.grabChestParams.passOnAmount = SCALE_IMMINENCE(params.getChestControlPassOnAmount());

    // Raycast
    if (!params.getEnableRaycast())
    {
      grabControl.edgeRaycastParams.raycastLength = 0.0f;
    }
    else
    {
      grabControl.edgeRaycastParams.raycastLength = SCALE_DIST(params.getRaycastLength());
    }
    grabControl.edgeRaycastParams.raycastEdgeOffset = SCALE_DIST(params.getRaycastEdgeOffset());

    
    if (userIn->getUseFiltering())
    {
      ObjectFiltering grabFiltering;
      grabFiltering.word0 = userIn->getFilterBitmaskWord(0);
      grabFiltering.word1 = userIn->getFilterBitmaskWord(1);
      grabFiltering.word2 = userIn->getFilterBitmaskWord(2);
      grabFiltering.word3 = userIn->getFilterBitmaskWord(3);
      out->setGrabFiltering(grabFiltering);
    }

    GrabDetectionParams grabDetectionParams;
    grabDetectionParams.maxReachDistance = SCALE_DIST(params.getMaxReachDistance());
    grabDetectionParams.maxSlope = NMP::degreesToRadians(params.getMaxSlope());
    grabDetectionParams.minAngleBetweenNormals = NMP::degreesToRadians(params.getMinAngleBetweenNormals());
    grabDetectionParams.minEdgeLength = SCALE_DIST(params.getMinEdgeLength());
    grabDetectionParams.minMass = SCALE_MASS(params.getMinMass());
    grabDetectionParams.minVolume = SCALE_VOLUME(params.getMinVolume());
    grabDetectionParams.minEdgeQuality = params.getMinEdgeQuality();

    GrabEnableConditions enableConditions;
    enableConditions.minSupportSlope = NMP::degreesToRadians(params.getMinSupportSlope());
    enableConditions.unbalancedAmount = params.getUnbalancedAmount();
    enableConditions.minUnbalancedPeriod = SCALE_TIME(params.getMinUnbalancedPeriod());
    enableConditions.verticalSpeedToStart = SCALE_VEL(params.getVerticalSpeedToStart());
    enableConditions.verticalSpeedToStop = SCALE_VEL(params.getVerticalSpeedToStop());

    GrabFailureConditions failureConditions;
    failureConditions.maxHandsBehindBackPeriod = SCALE_TIME(params.getMaxHandsBehindBackPeriod());
    failureConditions.maxReachAttemptPeriod = SCALE_TIME(params.getMaxReachAttemptPeriod());
    failureConditions.minReachRecoveryPeriod = SCALE_TIME(params.getMinReachRecoveryPeriod());

    GrabAliveParams grabAliveParams;
    grabAliveParams.pullUpStrengthScale = params.getPullUpStrengthScaleCP();
    grabAliveParams.pullUpAmount = params.getPullUpAmountCP();

    out->setGrabControl(grabControl);
    out->setGrabDetectionParams(grabDetectionParams);
    out->setGrabEnableConditions(enableConditions);
    out->setGrabFailureConditions(failureConditions);
    out->setGrabAliveParams(grabAliveParams);
    out->setTimeRatioBeforeLookingDown(params.getTimeBeforeLookingDown());
    out->setIgnoreOvershotEdges(params.getIgnoreOvershotEdgesCP());
  }

}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  HoldBehaviourData& params =
    ((HoldBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Hold)))->getParams();

  uint16_t numConstrainedArms = feedIn->getNumConstrainedArms();
  feedOut->setNumConstrainedArms(numConstrainedArms);
  params.setNumConstrainedHandsOCP((float)numConstrainedArms);
  params.setHoldAttemptImportanceOCP(feedIn->getHoldAttemptImportance());
  params.setHoldDurationOCP(feedIn->getHoldDuration());

  for (uint32_t i = 0 ; i < owner->data->numArms; ++i)
  {
    params.setHandHoldingOCP(i, owner->data->armLimbSharedStates[i].m_isConstrained);
  }

  NMP::Vector3 edgeForwardNormal = feedIn->getEdgeForwardNormal();

  if (params.getProjectEdgeNormalOntoGroundPlane() && edgeForwardNormal.magnitudeSquared() > FLT_MIN)
  {
    edgeForwardNormal = edgeForwardNormal.getComponentOrthogonalToDir(owner->data->up);
    edgeForwardNormal.fastNormalise();
  }

  params.setEdgeForwardNormalOCP(edgeForwardNormal);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

