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
#include "HoldActionBehaviourInterface.h"
#include "Behaviours/HoldActionBehaviour.h"
#include "euphoria/erCharacter.h"
#include "NetworkDescriptor.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
#endif

  const HoldActionBehaviourData& params =
    ((HoldActionBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_HoldAction)))->getParams();

  bool doHold = params.getDoHoldCP();
  Edge gameEdge;
  if (doHold)
  {
    // edge geometrical information. 
    gameEdge.uprightNormal = params.getEdgeNormalCP();
    gameEdge.uprightNormal.normaliseOrDef(owner->data->up);

    const NMP::Vector3& edgeStart = params.getEdgeStartCP();
    const NMP::Vector3& edgeEnd = params.getEdgeEndCP();
    gameEdge.corner = edgeStart;
    gameEdge.edge = edgeEnd - edgeStart;
    if (gameEdge.edge.magnitudeSquared() < NMP::sqr(SCALE_DIST(0.0001f)))
    {
      // Refuse to hold points
      doHold = false;
    }
    else
    {
      gameEdge.otherNormal = NMP::vCross(gameEdge.edge, gameEdge.uprightNormal);
      gameEdge.otherNormal.normalise();
      // Make the normal perpendicular to the edge
      gameEdge.uprightNormal = NMP::vCross(gameEdge.otherNormal, gameEdge.edge);
      gameEdge.uprightNormal.normalise();

      // -1 indicates a constraint with world rather than an actual dynamic or static object
      gameEdge.shapeID = -1; 
      // indicates that we want grabbability to be calculated by grab, if > 0 we don't
      gameEdge.quality = params.getEdgeImportanceCP();
      gameEdge.isWall = params.getIsWallCP();

      physx::PxRigidActor* physicsActor = (physx::PxRigidActor*) params.getPhysicsObjectIDCP();
      if (physicsActor)
      {
        // This edge is attached to a dynamic object. The code to handle grabbing dynamic objects
        // works with the shapeID, which is simply the shape pointer. It doesn't matter which shape
        // is actually chosen, so we'll choose the first. However, note that the edge is still
        // specified in world space, so we need to convert our input which is relative to the object.
        physx::PxShape* shape;
        physx::PxU32 numShapes = physicsActor->getShapes(&shape, 1);
        if (numShapes != 0)
        {
          gameEdge.shapeID = (int64_t)(size_t) shape;
          NMP::Matrix34 actorTM = MR::nmPxTransformToNmMatrix34(physicsActor->getGlobalPose());
          actorTM.transformVector(gameEdge.corner);
          actorTM.rotateVector(gameEdge.edge);
          actorTM.rotateVector(gameEdge.otherNormal);
          actorTM.rotateVector(gameEdge.uprightNormal);
        }
      }
    }
  }

  // If the edge was bad we might not hold
  if (doHold)
  {
    // per limb toggles
    for (uint32_t i = 0 ; i < owner->data->numArms; ++i)
    {
      out->setHoldPerArmImportanceAt(i, params.getHoldImportanceCP(i), 1.0f);
    }

    MR_DEBUG_DRAW_VECTOR(
      rootModule->getDebugInterface(), MR::VT_Delta, gameEdge.corner, gameEdge.edge, NMP::Colour::DARK_YELLOW);

    GrabControl grabControl;

    // constraint parameters
    grabControl.constraintParams.initialise(SCALING_SOURCE);
    grabControl.constraintParams.constrainOnContact = params.getHoldOnContact();
    grabControl.constraintParams.disableCollisions = params.getDisableCollisions();
    grabControl.constraintParams.lockedLinearDofs = (uint16_t) params.getLockedLinearDofs();
    grabControl.constraintParams.lockedAngularDofs = (uint16_t) params.getLockedAngularDofs();
    grabControl.constraintParams.createDistance = SCALE_DIST(params.getCreateAtDistance());
    grabControl.constraintParams.destroyDistance = SCALE_DIST(params.getDestroyAtDistance());
    grabControl.constraintParams.startOrientationAngle = NMP::degreesToRadians(params.getEnableOrientationAtAngle());
    grabControl.constraintParams.stopOrientationAngle = NMP::degreesToRadians(params.getDisableOrientationAtAngle());
    grabControl.constraintParams.disableAngularDofsUntilHanging = params.getDisableAngularDofsUntilHanging();

    // Hold timer
    grabControl.holdTimer.minHoldPeriod = SCALE_TIME(params.getMinHoldPeriod());
    grabControl.holdTimer.maxHoldPeriod = SCALE_TIME(params.getMaxHoldPeriod());
    if (grabControl.holdTimer.maxHoldPeriod <= 0.0f)
    {
      grabControl.holdTimer.maxHoldPeriod = FLT_MAX;
    }
    grabControl.holdTimer.noHoldPeriod =  SCALE_TIME(params.getNoHoldPeriod());
    NMP_ASSERT(grabControl.holdTimer.minHoldPeriod >= 0.0f);
    NMP_ASSERT(grabControl.holdTimer.maxHoldPeriod >= 0.0f);
    NMP_ASSERT(grabControl.holdTimer.noHoldPeriod >= 0.0f);

    // Chest control
    grabControl.grabChestParams.imminence = SCALE_IMMINENCE(params.getChestControlImminence());
    grabControl.grabChestParams.stiffnessScale = SCALE_IMMINENCE(params.getChestControlStiffnessScale());
    grabControl.grabChestParams.passOnAmount = SCALE_IMMINENCE(params.getChestControlPassOnAmount());

    // Detection parameters - MaxReachDistance is the only relevant parameter.
    GrabDetectionParams grabDetectionParams;
    grabDetectionParams.maxReachDistance = SCALE_DIST(params.getMaxReachDistance());
    grabDetectionParams.maxSlope = 0.0f;
    grabDetectionParams.minAngleBetweenNormals = 0.0f;
    grabDetectionParams.minEdgeLength = 0.0f;
    grabDetectionParams.minMass = 0.0f;
    grabDetectionParams.minVolume = 0.0f;
    grabDetectionParams.minEdgeQuality = 0.0f;

    // enable conditions
    GrabEnableConditions enableConditions;
    enableConditions.minSupportSlope = NMP::degreesToRadians(params.getMinSupportSlope());
    enableConditions.unbalancedAmount = params.getUnbalancedAmount();
    enableConditions.minUnbalancedPeriod = SCALE_TIME(params.getMinUnbalancedPeriod());
    enableConditions.verticalSpeedToStart = SCALE_VEL(params.getVerticalSpeedToStart());
    enableConditions.verticalSpeedToStop = SCALE_VEL(params.getVerticalSpeedToStop());

    // failure conditions
    GrabFailureConditions failureConditions;
    failureConditions.maxHandsBehindBackPeriod = SCALE_TIME(params.getMaxHandsBehindBackPeriod());
    failureConditions.maxReachAttemptPeriod = SCALE_TIME(params.getMaxReachAttemptPeriod());
    failureConditions.minReachRecoveryPeriod = SCALE_TIME(params.getMinReachRecoveryPeriod());

    GrabAliveParams grabAliveParams;
    grabAliveParams.pullUpStrengthScale = params.getPullUpStrengthScaleCP();
    grabAliveParams.pullUpAmount = params.getPullUpAmountCP();

    out->setGrabControl(grabControl, 1.0f);
    out->setGrabDetectionParams(grabDetectionParams, 1.0f);
    out->setGrabEnableConditions(enableConditions, 1.0f);
    out->setGrabFailureConditions(failureConditions, 1.0f);
    out->setGrabAliveParams(grabAliveParams);
    out->setTimeRatioBeforeLookingDown(params.getTimeBeforeLookingDown());

    // Append edge defined by input CPs to the end of the game edges array, overwriting the last game
    // edge if the array is at capacity.
    unsigned int numGameEdges = userIn->calculateNumGameEdges();

    if (gameEdge.quality != 0.0f)
    {
      const unsigned int maxNumGameEdges = userIn->getMaxGameEdges();
      const unsigned int index = NMP::minimum(numGameEdges, maxNumGameEdges - 1);
      userIn->setGameEdgesAt(index, gameEdge, 1.0f);

      // Increase the number of edges unless there was an override.
      if (index < maxNumGameEdges - 1)
      {
        ++numGameEdges;
      }
    }

    // If game has a forced edge (quality > 1.0f), then we know we don't have to bother with environment awareness,
    // and we pass this information on to the grab module (which controls use of EA)
    for (unsigned int i = 0; i < numGameEdges; i++)
    {
      if (userIn->getGameEdges(i).quality > 1.0f)
      {
        out->setHasForcedEdge(true, 1.0f);
        break;
      }
    }

  } // if doHold
}

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  HoldActionBehaviourData& params =
    ((HoldActionBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_HoldAction)))->getParams();

  uint16_t numConstrainedArms = feedIn->getNumConstrainedArms();
  feedOut->setNumConstrainedArms(numConstrainedArms);
  params.setNumConstrainedHandsOCP((float)numConstrainedArms);
  params.setHoldAttemptImportanceOCP(feedIn->getHoldAttemptImportance());
  params.setHoldDurationOCP(feedIn->getHoldDuration());

  for (unsigned int handIndex(0); handIndex < HoldActionBehaviourData::maxHandHoldingOCP; ++handIndex)
  {
    params.setHandHoldingOCP(handIndex, owner->data->armLimbSharedStates[handIndex].m_isConstrained);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HoldActionBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

