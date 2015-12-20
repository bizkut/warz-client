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
#include "ArmGrabPackaging.h"
#include "ArmPackaging.h"
#include "ArmGrab.h"
#include "Helpers/Helpers.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaLogger.h"
#include "NMGeomUtils/NMGeomUtils.h"

#define SCALING_SOURCE pkg.owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

// For releasing when hand is constrained behind the back (as measured in chest space)
//----------------------------------------------------------------------------------------------------------------------
static bool handIsBehindBody(ArmGrabUpdatePackage& pkg, float timeStep,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  // only let go if more than one hand is currently grabbing, i.e. a single hand grabbing behind is fine
  // (he will let go anyway if he has twisted around by more than 180 degrees)
  if (pkg.in->getNumConstrainedArms() > 1)
  {
    const NMP::Matrix34& chestTM = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_rootTM;
    NMP::Vector3 handPosChestSpace 
      = NMP::vInverseTransform(
      pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endTM.translation(), chestTM);
    bool handBehindNow = handPosChestSpace.x < SCALE_DIST(-0.1f);
    if (pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_isConstrained)
    {
      if (handBehindNow)
      {
        pkg.data->handBehindBodyPeriod += timeStep;
      }
      else
      {
        pkg.data->handBehindBodyPeriod -= timeStep;
      }
    }
    else
    {
      pkg.data->handBehindBodyPeriod = 0.0f;
    }

    const float maxBehindBackPeriod = pkg.in->getFailureConditions().maxHandsBehindBackPeriod;
    pkg.data->handBehindBodyPeriod = NMP::clampValue(pkg.data->handBehindBodyPeriod, 0.0f, 1.5f * maxBehindBackPeriod);
    return pkg.data->handBehindBodyPeriod > maxBehindBackPeriod;
  }

  return false;
}

// calculate how far to the left of the chest the clavicle (i.e. limb root) is
//----------------------------------------------------------------------------------------------------------------------
static float getRootLeftness(ArmGrabUpdatePackage& pkg)
{
  NMP::Vector3 chestToClavicle = 
    pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_basePosition
    - pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_rootTM.translation();
  return -chestToClavicle.dot(pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_rootTM.rightDirection());
}

// Check whether hand is constrained on the wrong side
//----------------------------------------------------------------------------------------------------------------------
static bool constrainedOnWrongSide(ArmGrabUpdatePackage& pkg)
{
  // We only need to check if more than one arm is constrained, and one of them is the current one.
  if (pkg.in->getNumConstrainedArms() > 1 
    && pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_isConstrained)
  {
    const NMP::Vector3& handPos = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endTM.translation();
    const NMP::Vector3& avgGrabPos = pkg.in->getEdgeToGrab().point;
    float rootLeftness = getRootLeftness(pkg);
    float onEdgeLeftness = (handPos - avgGrabPos).dot(pkg.in->getEdgeToGrab().edge);
    bool differentSigns = (rootLeftness * onEdgeLeftness) < 0.0f;
    // Only fail if on wrong side by more than this distance
    const float maxOffset = SCALE_DIST(0.1f);
    if (differentSigns && fabs(onEdgeLeftness) > maxOffset)
    {
      return differentSigns;
    }
  }
  return false;
}

// Check whether we've tried grabbing for too long. Pauses attempt for a certain period before trying again.
// "Attempt" is defined as already grabbing with another arm, but not yet with this one.
//----------------------------------------------------------------------------------------------------------------------
static bool abortAttempt(ArmGrabUpdatePackage& pkg, float timeStep)
{
  const float maxAttemptPeriod = pkg.in->getFailureConditions().maxReachAttemptPeriod;
  const float pausePeriod = pkg.in->getFailureConditions().minReachRecoveryPeriod;

  const int anyArmConstrained = pkg.in->getNumConstrainedArms() > 0;
  const bool isConstrained = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_isConstrained;

  // Update attempt period.
  if (anyArmConstrained)
  {
    pkg.data->grabAttemptPeriod += timeStep;
  }

  // Reset timer when pausePeriod has elapsed or we are not attempting.
  if (pkg.data->grabAttemptPeriod > (maxAttemptPeriod + pausePeriod) || !anyArmConstrained)
  {
    pkg.data->grabAttemptPeriod = 0.0f;
  }

  // Too long without success?
  if ((pkg.data->grabAttemptPeriod > maxAttemptPeriod) && !isConstrained)
  {
    return true;
  }

  return false;
}

// Calculated a desired hand orientation from edge information. The direction of the edge has already been adapted
// to the current scenario (wall, pole, etc) by the grab module to ensure coherence between different arms..
//----------------------------------------------------------------------------------------------------------------------
static ER::HandFootTransform getDesiredHandOrientation(const Edge& gEdge)
{
  ER::HandFootTransform desiredGrabTM;
  // hand up: average surface normal
  desiredGrabTM.normalDirection() = (gEdge.uprightNormal + gEdge.otherNormal);
  desiredGrabTM.normalDirection().normalise();
  // hand side: parallel to edge
  desiredGrabTM.leftDirection() = gEdge.edge.getNormalised();
  // hand forward (pointing):
  desiredGrabTM.pointDirection() = NMP::vCross(desiredGrabTM.leftDirection(), desiredGrabTM.normalDirection());
  desiredGrabTM.pointDirection().normalise();

  // The edge direction and normals should be OK
  NMP_ASSERT(desiredGrabTM.isValidTM3x3(0.01f))

  return desiredGrabTM;
}

// Calculated a desired hand orientation from edge information. The direction of the edge has already been adapted
// to the current scenario (wall, pole, etc) by the grab module to ensure coherence between different arms.
//----------------------------------------------------------------------------------------------------------------------
static void getDesiredHandOrientationWhileHanging(const Edge& gEdge, ER::HandFootTransform* desiredTM)
{
  NMP_ASSERT(desiredTM);
  desiredTM->normalDirection() = gEdge.otherNormal;
  desiredTM->normalDirection().fastNormalise();
  desiredTM->leftDirection() = gEdge.edge.getNormalised();
  desiredTM->pointDirection() = NMP::vCross(desiredTM->leftDirection(), desiredTM->normalDirection());
  desiredTM->pointDirection().fastNormalise();

  // Found that in some cases the assumption that uprightNormal and otherNormal are orthogonal is wrong!
  if (!desiredTM->isValidTM3x3(0.01f))
  {
    desiredTM->normalDirection() =
      NMRU::GeomUtils::calculateOrthogonalPlanarVector(desiredTM->leftDirection(), desiredTM->normalDirection());
    desiredTM->normalDirection().fastNormalise();

    NMP_ASSERT(desiredTM->isValidTM3x3(0.01f));
  }
}

// Calculate a desired hand position: if the hand is already very close to edge, go for it directly,
// unless another hand is already grabbing
//----------------------------------------------------------------------------------------------------------------------
static NMP::Vector3 getDesiredHandPosition(ArmGrabUpdatePackage& pkg, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  NMP::Vector3 grabPoint;
  const Edge& gEdge = pkg.in->getEdgeToGrab();
  NMP::Vector3 edgeDir = gEdge.edge;
  float edgeLength = edgeDir.normaliseGetLength();
  float rootLeftness = getRootLeftness(pkg);

  // calculate hand to edge distance
  const NMP::Vector3& handPos = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_endTM.translation();
  NMP::Vector3 handPosEdgeProj;
  if (pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_isConstrained)
  {
    handPosEdgeProj = gEdge.corner + gEdge.edge * pkg.data->cachedEdgeT;
  }
  else
  {
    handPosEdgeProj = projectPointOntoLineSegment(gEdge.corner, edgeDir, handPos, 0, edgeLength);
    pkg.data->cachedEdgeT = (handPosEdgeProj - gEdge.corner).dot(gEdge.edge) / (1e-10f + gEdge.edge.magnitudeSquared());
  }
  float handEdgeDistSq = (handPos - handPosEdgeProj).magnitudeSquared();
  static const float handEdgeDistToGrabThresholdSq = NMP::sqr(SCALE_DIST(0.3f)); // MORPH-22541 ArmGrab is using constant definition of handEdgeDistToGrabThresholdSq value.
  bool edgeWithinReach = handEdgeDistSq < handEdgeDistToGrabThresholdSq; 
  // When grabbing (but not constrained) aim for positions just either side of the edge point, based
  // on the clavicle (arm base) offset. However, when actually holding, or very close to holding,
  // then project the current position onto the edge. 
  if (edgeWithinReach || pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_isConstrained)
  {
    // if no hand is constrained yet, grab immediately (closest point)
    if (pkg.in->getNumConstrainedArms() < 2)
    {
      grabPoint = handPosEdgeProj;
    }
    // otherwise, grab immediately only if hands won't be crossed
    else
    {
      // get lateral offset vector from average grab point
      NMP::Vector3 projOffset = handPosEdgeProj - gEdge.point;
      bool projOffsetIsLeft = projOffset.dot(gEdge.edge) > 0;
      bool rootIsLeft = rootLeftness > 0;
      // is this a left or right arm? and how "left" is it exactly?
      bool handIsOnCorrectSide = projOffsetIsLeft == rootIsLeft;
      const float minimumHandSeparation = SCALE_DIST(0.2f); // TODO: make this value tunable
      if (handIsOnCorrectSide && (projOffset.magnitude() > minimumHandSeparation))
      {
        grabPoint = handPosEdgeProj;
      }
      else
      {
        // move it to reasonable distance away from average grab point and limit to extents of edge
        grabPoint = gEdge.point + edgeDir * rootLeftness;
        grabPoint = projectPointOntoLineSegment(gEdge.corner, edgeDir, grabPoint, 0, edgeLength);
      }
    }
  }
  // if edge is not within reach
  else
  {
    // Reach for points left/right of average grab point.
    grabPoint = gEdge.point + edgeDir * rootLeftness;
    grabPoint = projectPointOntoLineSegment(gEdge.corner, edgeDir, grabPoint, 0, edgeLength);
  }

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, grabPoint, SCALE_DIST(0.05f),
    (rootLeftness > 0 ? NMP::Colour::YELLOW : NMP::Colour::BLUE));

  return grabPoint;
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->data->dimensionalScaling
//----------------------------------------------------------------------------------------------------------------------
// ArmGrabUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmGrabUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  // early out if no grabbable edge was sent
  if (in->getEdgeToGrabImportance() == 0.0f || in->getImportance() <= 0.0f)
  {
    data->havePreviousGrabPoint = false;
    return;
  }

  const bool isConstrained = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained;
  bool inNoHoldPeriod = !isConstrained && (feedIn->getTimeHeldProportionImportance() > 0) &&
                        (feedIn->getTimeHeldProportion() > 0);

  // give up reaching for grab point after a while (allows e.g. armSwing to kick in)
#ifdef USING_EUPHORIA_LOGGING
  const float maxReachPeriod = in->getFailureConditions().maxReachAttemptPeriod;
  if ((data->grabAttemptPeriod < maxReachPeriod) &&
      (data->grabAttemptPeriod + timeStep > maxReachPeriod))
    EUPHORIA_LOG_MESSAGE("Grab: reach aborted. Time for attempt is up (%2.2f sec).\n", data->grabAttemptPeriod);
#endif

  bool doAbortAttempt = abortAttempt(*this, timeStep);

  if (doAbortAttempt || inNoHoldPeriod || in->getEdgeIsObstructed())
  {
    data->havePreviousGrabPoint = false;
    return;
  }

  // From here on we know we want to reach and grab the edge
  const bool anyArmConstrained = in->getNumConstrainedArms() > 0;
  bool doConstrain = true;
  const Edge& gEdge = in->getEdgeToGrab();

#define PREVENT_RE_GRABBING 0
#if PREVENT_RE_GRABBING
  // don't grab same edge twice
  if (!isConstrained)
  {
    if (gEdge.equals(data->lastGrabbedEdge))
    {
      doConstrain = false;
    }
  }
  else
  {
    data->lastGrabbedEdge = gEdge;
  }
#endif

  // use passed in parameters as default for the constraint if possible, allows for game control over details of grab
  EndConstraintControl endConstraint;
  endConstraint.initialise(SCALING_SOURCE);
  if (in->getEndConstraintOptionsImportance() > 0.0f)
  {
    endConstraint = in->getEndConstraintOptions();
  }
  endConstraint.setTargetShapeID(gEdge.shapeID);

  // Don't allow constraining orientation until hanging. So overwrite the default lockedAngularDofs.
  if (!in->getIsHanging())
  {
    if (endConstraint.disableAngularDofsUntilHanging)
    {
      endConstraint.lockedAngularDofs = 0;
    }
    endConstraint.useCheatForces = false;
    endConstraint.disableCollisions = false;
  }
  else
  {
    endConstraint.useCheatForces = true;
  }

  // Check whether to release, depending on input from above that indicates whether character is grabbing an edge
  // behind him with more than one hand. Can't early out here though. We still want to update the desired hand
  // orientation calculated next for reaching towards the edge, even if not constraining.
  bool hasTwistedBwd = in->getHasTwistedAround();
  if (isConstrained)
  {
    if (hasTwistedBwd && in->getNumConstrainedArms() == 1)
    {
      doConstrain = false;
    }
  }

  // Desired hand position and orientation on the edge.

  // Blend from hand current to desired reaching orientation
  // only when reaching target is further than twice the maximum arm reaching distance from the shoulder.
  const NMP::Vector3 shoulderPosition = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition;
  const float maxReachDistanceDoubled = 2.0f * owner->data->length;
  const float shoulderToTargetDistance =
    NMP::fastSqrt((getDesiredHandPosition(*this, pDebugDrawInst) - shoulderPosition).magnitudeSquared());
  const float handOrientWeight =
    NMP::minimum(maxReachDistanceDoubled / (shoulderToTargetDistance + SCALE_DIST(0.001f)), 1.0f);
  ER::HandFootTransform desiredGrabTM;
  // Interpolate orientation.
  const ER::HandFootTransform endTM = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM;
  desiredGrabTM.interpolate(endTM, getDesiredHandOrientation(gEdge), handOrientWeight);
  desiredGrabTM.translation() = getDesiredHandPosition(*this, pDebugDrawInst);

  NMP::Vector3 grabPosVel(NMP::Vector3::InitZero);
  if (data->havePreviousGrabPoint)
  {
    // Velocity of the grab target, this is needed in the grab control
    grabPosVel = (desiredGrabTM.translation() - data->previousGrabPoint) / timeStep;
    grabPosVel = grabPosVel.getComponentOrthogonalToDir(gEdge.edge);
  }
  data->previousGrabPoint = desiredGrabTM.translation();
  data->havePreviousGrabPoint = true;

  // Note that desiredGrabTM.translation() can be modified before it's actually used - that's why we cache the value here so it's possible to evaluate the target grab point velocity.
  endConstraint.setDesiredOrientation(desiredGrabTM);

  // Allow one(!) arm to release when hands are crossed. Each arms checks whether it's crossed with respect to
  // average constraint position. If it is, it provides a measure of its current reach error to MyNetwork,
  // which decides which hand constraint to break based on the worst reaching hand. The exact meaning of the measure
  // isn't really that important as long as it's guaranteed to be different between hands.
  const bool isConstrainedOnWrongSide = constrainedOnWrongSide(*this);
  const bool isHandBehindBody = handIsBehindBody(*this, timeStep, pDebugDrawInst);
  bool breakageOpportunity =
    isConstrainedOnWrongSide || isHandBehindBody || (hasTwistedBwd && in->getNumConstrainedArms() > 1);
  if (breakageOpportunity)
  {
    float separation = (desiredGrabTM.translation() - endTM.translation()).magnitudeSquared();
    out->setBreakageOpportunity(separation);
    EUPHORIA_LOG_MESSAGE("Grab: give up opportunity:\n");
    EUPHORIA_LOG_MESSAGE("  HandBehindBody: %d.\n", isHandBehindBody);
    EUPHORIA_LOG_MESSAGE("  ConstrainedOnWrongSide: %d.\n", isConstrainedOnWrongSide);
    EUPHORIA_LOG_MESSAGE("  hasTwistedBackwards: %d.\n", hasTwistedBwd);
  }

#define DISABLE_WHEN_OUTOFREACHx
#ifdef DISABLE_WHEN_OUTOFREACH
  bool canReach = true;
  canReach = owner->canReachPoint(desiredGrabTM.translation(), canReach, 0.0f, pDebugDrawInst);
  if (!canReach)
  {
    doConstrain = false;
  }
#endif

  if (doConstrain)
  {
    out->setEndConstraint(endConstraint, 1.0f);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endConstraint.desiredTM, SCALE_DIST(0.2f));

    // Send out strength information to support based on time that character has been grabbing.
    if (isConstrained)
    {
      float strengthScale;
      // Game can tune how strong to pull up when hanging under gravity. But when not hanging yet, stiff arms look
      // unnatural. Only when working against gravity do we want the pulling to have its effect, so keep the arms loose.
      if (!in->getIsHanging())
      {
        strengthScale = 0.25f;
      }
      else
      {
        strengthScale = in->getAliveParams().pullUpStrengthScale;

        // The fewer arms are holding the faster they fatigue
        float enduranceScale = (float)in->getNumConstrainedArms() / (float)owner->owner->data->numArms;
        // A character holding with all his arms will start fatiguing after half the maxHoldDuration, from which point
        // on strength will ramp down until it reaches the minimum at maxHoldDuration. The fewer arms are holding, the
        // earlier fatigue will set in and the quicker strength will ramp down.
        float rampDownStart = enduranceScale * 0.5f;
        float rampDownEnd = rampDownStart + enduranceScale * 0.5f;
        float minimumStrength = 0.1f;
        strengthScale *= linearRampBlend(feedIn->getTimeHeldProportion(), rampDownStart, rampDownEnd, 1.0f, minimumStrength);

        // This is counter-intuitive, but if the character is holding with one arm, he needs enough strength to pull
        // up, so the other hand can grab. Once grabbing with both hands however, too much strength will make him
        // look snappy and rigid. Hence the character is made to have more strength with one arm than with two.
        const float multiArmStrengthScaler = 1.0f;
        if (in->getNumConstrainedArms() > 1)
        {
          strengthScale *= multiArmStrengthScaler;
        }
      }
      out->setHoldStrengthScale(strengthScale, 1.0f);
    }

  }

  MR_DEBUG_DRAW_POINT(
    pDebugDrawInst, 
    desiredGrabTM.translation(), 
    SCALE_DIST(0.05f), 
    (doConstrain ? NMP::Colour::GREEN : NMP::Colour::RED));

  // Arm reaches for target when not yet constrained. Calculate reach importance from grabbability.
  float reachImportance = (doConstrain && !isConstrained) ? gEdge.quality : 0.0f;
  const float minThreshold = 0.1f;   // grabbability below this will lead to 0 importance
  const float maxThreshold = 0.2f;   // grabbability beyond this will lead to 1 importance
  reachImportance = linearRampBlend(reachImportance, minThreshold, maxThreshold, 0.0f, 1.0f);

  if (reachImportance > 0.0f)
  {
    // Use reachForWorld for the reaching, rather than limbControl directly. Will get whole body motion for free,
    // as well as self-avoidance. I.e. the following limb control feeds into the reachForWorld module, instead of
    // the limb itself.
    float stiffnessScale = anyArmConstrained ? 1.5f : 2.0f; // initially only goal is grabbing on
    LimbControl& control = out->startLimbControlModification();
    control.reinit(
      owner->data->normalStiffness * stiffnessScale,
      owner->data->normalDampingRatio,
      owner->data->normalDriveCompensation);

    NMP::Vector3 armRootToGrabPos = desiredGrabTM.translation() 
      - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation();
    // Remove the incoming component of each velocity, since it is too chaotic to try and compensate
    // in this direction, it changes too quickly when approaching an edge
    NMP::Vector3 rootVelOrthogonalToReachDir = 
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootData.velocity.getComponentOrthogonalToDir(armRootToGrabPos);
    NMP::Vector3 grabPosVelOrthogonalToReachDir = 
      grabPosVel.getComponentOrthogonalToDir(armRootToGrabPos);

    // Generate reaching targets for free arm when character hangs from the ledge
    // so that it is possible to reach and grab successfully.
    if (in->getIsHanging())
    {
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endTM, SCALE_DIST(0.1f));

      bool endInContact = owner->data->collidingTime > SCALE_TIME(0.3f);
      if (endInContact)
      {
        ER::LimbTransform chestTM = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM;
        MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, chestTM, SCALE_DIST(1.0f));

        // Find base position in chest frame and project it into rotation plane.
        NMP::Vector3 basePosWd = this->owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition;
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, basePosWd, SCALE_DIST(0.25f), NMP::Colour::LIGHT_PURPLE);

        NMP::Vector3 endDirLocal;  // Holds limb end direction projected to YZ plane relative to chest frame.
        NMP::Vector3 trgtDirLocal; // Holds edge upright normal direction projected to YZ plane relative to chest frame.
        NMP::Vector3 basePosLocal; // Holds limb base position projected to YZ plane relative to chest frame.

        // From World to chest frame transformation.
        chestTM.inverseTransformVector(endTM.translation(), endDirLocal);
        chestTM.inverseRotateVector(gEdge.uprightNormal, trgtDirLocal);
        chestTM.inverseTransformVector(basePosWd, basePosLocal);

        // Remove chest forward component to simplify working space into 2D.
        endDirLocal.x = 0.0f;
        trgtDirLocal.x = 0.0f;
        basePosLocal.x = 0.0f;

        // Use default value when vector has no direction after explicit component removal.
        endDirLocal.normalise(NMP::Vector3::InitTypeOneX);
        trgtDirLocal.normalise(NMP::Vector3::InitTypeOneY);

        // Arm length is used to clamp distance to final reaching target.
        const float armLength = this->owner->data->length;

        NMP::Quat rollRot;
        rollRot.forRotation(endDirLocal, trgtDirLocal);
        NMP::Vector3 rollRotVec = rollRot.toRotationVector();

        // Create new reaching target when valid rotation was obtained.
        if (rollRotVec.magnitudeSquared() > 0.0f)
        {
          const float minRotAngleRad = 0.01745f;
          const float maxDesiredOffsetAngleRad = 0.785f; // Maximum desired rotation offset - tunable parameter.
          const float endDir2TrgtDirAngleRadClamped = NMP::clampValue(rollRotVec.magnitude(), minRotAngleRad, maxDesiredOffsetAngleRad);

          // Start extending arm when angle to desired direction decreases.
          float elbowBendAmount = 1.01254f - 0.716612f * endDir2TrgtDirAngleRadClamped;
          elbowBendAmount = NMP::clampValue(elbowBendAmount, 0.45f, 1.0f);

          // Calculate new reaching target direction.
          rollRotVec.normalise();
          rollRotVec *= endDir2TrgtDirAngleRadClamped;
          rollRot.fromRotationVector(rollRotVec);
          rollRot.normalise();
          NMP::Vector3 newGrabPoint = rollRot.rotateVector(endDirLocal);

          // Calculate final arm extension taking to account minimum distance for constraint creation threshold.
          newGrabPoint.normalise ();
          // TODO: const value for 0.3 indicating intention
          newGrabPoint *= ((armLength + SCALE_DIST(0.3f)) * elbowBendAmount); 
          newGrabPoint += basePosLocal;

          // Transform new target to World space.
          chestTM.transformVector(newGrabPoint, desiredGrabTM.translation());
          MR_DEBUG_DRAW_POINT(pDebugDrawInst, desiredGrabTM.translation(), SCALE_DIST(0.2f), NMP::Colour::LIGHT_TURQUOISE);
        }
      }

      // Set reaching target World position, take to account edge velocity.
      control.setTargetPos(desiredGrabTM.translation(), 0.7f, grabPosVelOrthogonalToReachDir);

      // Get hand desired reaching orientation from edge normal.
      ER::HandFootTransform desiredReachTM;
      getDesiredHandOrientationWhileHanging(gEdge, &desiredReachTM);
      desiredReachTM.r[3] = desiredGrabTM.translation();

      // Blend from hand current to desired reaching orientation.
      const float blendWeight = 0.5f;
      ER::HandFootTransform blendDesiredTM;
      blendDesiredTM.interpolate(endTM, desiredReachTM, blendWeight);
      blendDesiredTM.r[3] = desiredGrabTM.translation();

      // Set hand desired reaching orientation.
      control.setTargetNormal(blendDesiredTM.normalDirection(), 1.0f);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Impulse, blendDesiredTM.translation(), blendDesiredTM.normalDirection(), NMP::Colour::WHITE);
    }
    else // In this case character is not hanging.
    {
      control.setTargetPos(desiredGrabTM.translation(), 1.0f, grabPosVelOrthogonalToReachDir);
      control.setTargetNormal(desiredGrabTM.normalDirection(), 0.25f);
    }

    MR_DEBUG_DRAW_POINT(pDebugDrawInst, control.getTargetPos(), SCALE_DIST(0.5f), NMP::Colour::YELLOW);

    // Compensate for own motion in reaching for target
    const float angVelPredictability = 0.25f; // 1 if angular velocity only changes slowly. 0 if ang vel is too fast changing to be able to compensate
    control.setExpectedRootForTarget(
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM,
      1,
      rootVelOrthogonalToReachDir,
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity * angVelPredictability);
    control.setGravityCompensation(1.0f);
    control.setControlAmount(ER::grabControl, 1.0f);
    out->stopLimbControlModification(reachImportance);

    // Configure how we want to reach. This also feeds into the reachForWorld module.
    ReachWorldParameters reachParams;
    reachParams.setToDefaults(SCALING_SOURCE);
    // Disable reach limiting, by setting the maxReach to 5 times the limbLength as grab performs slightly better.
    reachParams.maxReachScale = 5.0f;
    reachParams.reachImminence = SCALE_IMMINENCE(6.0f);
    reachParams.selfAvoidanceStrategy = ReachWorldParameters::automatic;
    reachParams.avoidLegsArea = true;
    reachParams.slideAlongNormal = false;
    out->setReachParams(reachParams, reachImportance);
  }

  // cache the reach importance, used in feedback
  data->reachImportance = reachImportance;
}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrabFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  // this suppresses the shield module
  const float highPriorityThreshold = 0.35f;
  if (data->reachImportance > highPriorityThreshold)
  {
    feedOut->setIsHighPriority(true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmGrab::entry()
{
  data->handBehindBodyPeriod = 0.0f;
  data->previousGrabPoint.setToZero();
  data->grabAttemptPeriod = 0.0f;
  data->handBehindBodyPeriod = 0.0f;
  data->reachImportance = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

