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
#include "mrPhysicsScenePhysX3.h"
#include "Grab.h"
#include "MyNetwork.h"
#include "GrabPackaging.h"
#include "Helpers/Helpers.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erBody.h"
#include "HazardManagement.h"

#define SCALING_SOURCE pkg.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

// Convert edge from world to local space
//----------------------------------------------------------------------------------------------------------------------
static void convertEdgeToLocalSpace(Edge& grabbableEdge)
{
  // world to local space
  if (grabbableEdge.shapeID >= 0)
  {
    physx::PxShape* shape = NULL;
    shape = (physx::PxShape*) (size_t) grabbableEdge.shapeID;
    if (shape != NULL && MR::PhysXPerShapeData::getFromShape(shape))
    {
      physx::PxRigidActor* actor = &shape->getActor();
      if (actor && actor->isRigidDynamic())
      {
        NMP::Matrix34 actorTM = MR::nmPxTransformToNmMatrix34(actor->getGlobalPose());
        actorTM.inverseTransformVector(grabbableEdge.corner);
        actorTM.inverseTransformVector(grabbableEdge.point);
        actorTM.inverseRotateVector(grabbableEdge.edge);
        actorTM.inverseRotateVector(grabbableEdge.uprightNormal);
        actorTM.inverseRotateVector(grabbableEdge.otherNormal);
      }
    }
  }
}

// Convert edge from local to world space
//----------------------------------------------------------------------------------------------------------------------
static void convertEdgeToWorldSpace(Edge& grabbableEdge)
{
  if (grabbableEdge.shapeID >= 0)
  {
    physx::PxShape* shape = NULL;
    shape = (physx::PxShape*) (size_t) grabbableEdge.shapeID;
    if (shape != NULL && MR::PhysXPerShapeData::getFromShape(shape))
    {
      physx::PxRigidActor* actor = &shape->getActor();
      if (actor && actor->isRigidDynamic())
      {
        NMP::Matrix34 actorTM = MR::nmPxTransformToNmMatrix34(actor->getGlobalPose());
        actorTM.transformVector(grabbableEdge.corner);
        actorTM.transformVector(grabbableEdge.point);
        actorTM.rotateVector(grabbableEdge.edge);
        actorTM.rotateVector(grabbableEdge.uprightNormal);
        actorTM.rotateVector(grabbableEdge.otherNormal);
      }
    }
  }
}

// calculates the edge direction such that it points to the left, i.e. left hand grab points will
// be in a more positive edge direction than right hand targets
//----------------------------------------------------------------------------------------------------------------------
static void fixEdgeDirection(const Grab& pkg, Edge& grabbableEdge)
{
  NMP::Vector3 naturalEdgeDir = NMP::vCross(grabbableEdge.otherNormal, grabbableEdge.uprightNormal);
  if (grabbableEdge.edge.dot(naturalEdgeDir) < 0)
  {
    // flip the direction
    grabbableEdge.corner = grabbableEdge.corner + grabbableEdge.edge; // change corner over to other side
    grabbableEdge.edge *= -1;                         // flip edge direction
  }

  // Make non-wall edges align with the character
  if (!grabbableEdge.isWall)
  {
    const ER::LimbTransform& chest = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM;
    if (grabbableEdge.edge.dot(chest.rightDirection()) > 0.0f)
    {
      // flip the direction
      grabbableEdge.corner = grabbableEdge.corner + grabbableEdge.edge; // change corner over to other side
      grabbableEdge.edge *= -1;                         // flip edge direction
      grabbableEdge.otherNormal *= -1;
    }
  }
}

// Checks whether the character is facing away from the edge
//----------------------------------------------------------------------------------------------------------------------
static bool hasTwistedAround(const Grab& pkg, Edge& grabbableEdge, int numConstrainedArms, float timeStep,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Always grab in zero gravity
  if (pkg.owner->owner->data->gravityMagnitude == 0.0f)
    return false;

  // determine the outwards facing direction of the surface normal in the horizontal plane
  NMP::Vector3 desiredFwdFlat = NMP::vCross(grabbableEdge.edge, grabbableEdge.uprightNormal);
  desiredFwdFlat = desiredFwdFlat.getComponentOrthogonalToDir(pkg.owner->owner->data->up);
  desiredFwdFlat.fastNormalise();

  // determine current torso facing direction in horizontal plane
  const ER::LimbTransform& chest = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM;
  const ER::LimbTransform& pelvis = pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM;
  NMP::Vector3 torsoFwdFlat =
    0.5f * (chest.frontDirection() + pelvis.frontDirection());
  torsoFwdFlat = torsoFwdFlat.getComponentOrthogonalToDir(pkg.owner->owner->data->up);
  torsoFwdFlat.fastNormalise();

  bool isConstrained = numConstrainedArms > 0;

  // Only do this once character is hanging
  if (isConstrained)
  {
    // check whether character is upright
    const float uprightnessThreshold = 0.75f; // TODO: detail motives for this value
    NMP::Vector3 spineAxis = NMP::vNormalise(chest.translation() - pelvis.translation());
    float uprightness = spineAxis.dot(pkg.owner->owner->data->up);
    bool isUpright = uprightness > uprightnessThreshold;

    if (!pkg.data->isHanging)
    {
      // check whether chest is below edge
      NMP::Vector3 chestToEdge = grabbableEdge.point - chest.translation();
      float belowness = chestToEdge.dot(pkg.owner->owner->data->up);
      const float vertHangingDist = SCALE_DIST(0.3f); // TODO: detail motives for this value
      bool isBelowEdge = belowness > vertHangingDist;

      pkg.data->isHanging = isBelowEdge && isUpright;
      if (pkg.data->isHanging)
      {
        // If hanging from a wall we want to be facing it
        pkg.data->desiredTorsoFwd = desiredFwdFlat;
      }
    }
    else if (!isUpright && pkg.feedIn->getLegsInContact())
    {
      // if already hanging, check whether to terminate the hanging state
      pkg.data->isHanging = false;
      pkg.data->edgeBehindPeriod = 0.0f;
    }
  }
  else
  {
    pkg.data->isHanging = false;
    pkg.data->edgeBehindPeriod = 0.0f;
  }

  if (pkg.data->isHanging)
  {
    pkg.out->setIsHanging(true);

    // Check whether character has his back turned towards the edge (in which case he should be giving up sooner)
    const float behindThresh = -0.5f; // in "dot-product-space"
    float currentDotDesired = torsoFwdFlat.dot(pkg.data->desiredTorsoFwd);
    bool edgeIsBehind = currentDotDesired < behindThresh; // TODO: detail motives for this value
    if (edgeIsBehind && isConstrained)
    {
      pkg.data->edgeBehindPeriod += timeStep;
    }
    else
    {
      pkg.data->edgeBehindPeriod = 0.0f;
    }
    bool hasTwistedAround = pkg.data->edgeBehindPeriod > pkg.in->getMaxTwistBwdPeriod();

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, chest.translation(), pkg.data->desiredTorsoFwd, NMP::Colour::WHITE);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, chest.translation(), torsoFwdFlat, \
      NMP::Colour(getRainbowColourMapped(pkg.data->edgeBehindPeriod)));

    return hasTwistedAround;
  }
  return false;
}

// Checks whether to stop a grab, because the character is lying on the ground
//----------------------------------------------------------------------------------------------------------------------
static bool isOnGround(const Grab& pkg, bool isConstrained, float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Always grab in zero gravity
  if (pkg.owner->owner->data->gravityMagnitude == 0.0f)
    return false;

  // Flag onGround defined by being relatively horizontal, non-moving and in contact.
  // Extract estimated ground slope from current spine angle
  NMP::Vector3 spineAxis = NMP::vNormalise(pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation()
    - pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation());
  float spineDotUp = NMP::clampValue(spineAxis.dot(pkg.owner->owner->data->up), -1.0f, 1.0f);
  float upAngle = NMP::fastArccos(spineDotUp);
  // get the angle of character "normal" with the up direction
  upAngle = fabs(upAngle - NM_PI_OVER_TWO);
  bool spineIsHorizontal = upAngle < pkg.in->getEnableConditions().minSupportSlope;
  bool inContact = pkg.feedIn->getSpineInContact() || pkg.feedIn->getLegsInContact();

  // find most upright contact normal, to see whether character is supported by something more or less horizontal
  pkg.out->setUprightContactDir(pkg.owner->owner->data->up);
  float slope = 0.0f;
  bool slopeSupporting = false;
  if (pkg.feedIn->getMostUprightContactImportance() > 0.0f)
  {
    float normalDotUp = pkg.feedIn->getMostUprightContact().lastNormal.dot(pkg.owner->owner->data->up);
    slope = acosf(normalDotUp);
    if (slope < pkg.in->getEnableConditions().minSupportSlope)
    {
      slopeSupporting = true;
    }

    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Normal,
      pkg.feedIn->getMostUprightContact().lastPosition,
      pkg.feedIn->getMostUprightContact().lastNormal,
      (slopeSupporting ? NMP::Colour::GREEN : NMP::Colour::RED));
  }

  // Hysteresis to avoid flickering grab decisions: use different threshold for dynamically enabling and disabling
  // the grab attempt, based on whether it was allowed in the last frame
  bool isMoving;
  float comVertSpeed = pkg.owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity().dot(pkg.owner->owner->data->up);
  float bodyAngSpeed = pkg.owner->owner->data->bodyLimbSharedState.m_dynamicState.getAngularVelocity().magnitude();
  if (pkg.data->grabAllowed || isConstrained)
  {
    // Only DISABLE grab attempt when really motionless, i.e. small speed in BOTH linear AND angular terms
    const float lowerVertSpeedThresh = SCALE_VEL(pkg.in->getEnableConditions().verticalSpeedToStop);
    const float lowerAngSpeedThresh = SCALE_ANGVEL(1.0f);
    isMoving = (comVertSpeed < -lowerVertSpeedThresh) || (bodyAngSpeed > lowerAngSpeedThresh);
  }
  else
  {
    // Only ENABLE when moving relatively fast, in linear terms
    const float upperVertSpeedThresh = SCALE_VEL(pkg.in->getEnableConditions().verticalSpeedToStart);
    isMoving = (fabs(comVertSpeed) > upperVertSpeedThresh);
  }

  MR_DEBUG_DRAW_VECTOR(
    pDebugDrawInst,
    MR::VT_Delta,
    pkg.owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition(),
    comVertSpeed * pkg.owner->owner->data->up,
    (isMoving ? NMP::Colour::RED : NMP::Colour::GREEN));

  bool isHorizontallySupported = (spineIsHorizontal && (inContact || !isMoving)) || slopeSupporting;
  bool isLyingOnGroundNow = isHorizontallySupported && !isMoving;
  if (isLyingOnGroundNow)
  {
    pkg.data->onGroundPeriod += timeStep;
  }
  else
  {
    pkg.data->onGroundPeriod -= timeStep;
  }
  const float minOnGroundPeriod = SCALE_TIME(0.3f); // TODO: detail motives for this value
  pkg.data->onGroundPeriod = NMP::clampValue(pkg.data->onGroundPeriod, 0.0f, 2 * minOnGroundPeriod);
  bool onGround = pkg.data->onGroundPeriod > minOnGroundPeriod;

#ifdef USING_EUPHORIA_LOGGING
  if (pkg.data->grabAllowed && onGround)
  {
    EUPHORIA_LOG_MESSAGE("Grab now grounded: \n");
    EUPHORIA_LOG_MESSAGE("  spineHorizontal: %d (%2.2f of %2.2f deg)\n", spineIsHorizontal, upAngle,
      NMP::radiansToDegrees(pkg.in->getEnableConditions().minSupportSlope));
    EUPHORIA_LOG_MESSAGE("  isMoving: %d (COMvertSpeed: %2.2f)\n", isMoving, comVertSpeed);
    EUPHORIA_LOG_MESSAGE("  inContact (legs or spine): %d\n", inContact);
    EUPHORIA_LOG_MESSAGE("  contact slope supporting: %d (%2.2f of %2.2f deg)\n", slopeSupporting, slope,
      NMP::radiansToDegrees(pkg.in->getEnableConditions().minSupportSlope));
    EUPHORIA_LOG_MESSAGE("  onGroundDuration: %f\n", pkg.data->onGroundPeriod);
  }
#endif

  return onGround;
}

// Checks whether to start/stop a grab, based on whether the character is balanced
//----------------------------------------------------------------------------------------------------------------------
static bool isUnbalanced(const Grab& pkg, float timeStep)
{
  // Always keep grabbing in zero gravity
  if (pkg.owner->owner->data->gravityMagnitude == 0.0f)
    return true;

  // buffer the isBalanced decision, too erratic otherwise
  const float balanceAmount = pkg.owner->feedIn->getSupportAmount();
  const float balanceAmountThreshold = pkg.in->getEnableConditions().unbalancedAmount;
  const float balancePeriodThreshold = pkg.in->getEnableConditions().minUnbalancedPeriod;
  if (balanceAmount > balanceAmountThreshold)
  {
    pkg.data->balancedPeriod += timeStep;
  }
  else
  {
    pkg.data->balancedPeriod -= 2.0f * timeStep;
  }
  pkg.data->balancedPeriod = NMP::clampValue(pkg.data->balancedPeriod, -balancePeriodThreshold, balancePeriodThreshold);
  return pkg.data->balancedPeriod < 0.0f;
}

// Collect information about constrained arms (how many, and their average constraint position)
//----------------------------------------------------------------------------------------------------------------------
static bool getConstrainedArmsInfo(const Grab& pkg, uint16_t& numConstrainedArms, NMP::Vector3& avgConstraintPos)
{
  avgConstraintPos.set(0, 0, 0);
  numConstrainedArms = 0;
  for (uint16_t i = 0; i < pkg.owner->owner->data->numArms; i++)
  {
    if (pkg.owner->owner->data->armLimbSharedStates[i].m_isConstrained == true)
    {
      avgConstraintPos += pkg.owner->owner->data->armLimbSharedStates[i].m_endTM.translation();
      numConstrainedArms++;
    }
  }

  bool isConstrained = numConstrainedArms > 0;
  if (isConstrained)
  {
    avgConstraintPos /= (float) numConstrainedArms;
  }

  return isConstrained;
}

// Function causes a ray to be cast offset from the edge downwards. If that ray hits a suitably
// flat piece of environment within a short enough distance then the character won’t grab the edge.
//----------------------------------------------------------------------------------------------------------------------
static bool isEdgeObstructed(
  const Grab& pkg,
  const Edge& grabbableEdge,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Always grab in zero gravity
  if (pkg.owner->owner->data->gravityMagnitude == 0.0f)
    return false;

  // Early out if the length of the ray is 0.
  const float raycastLength = pkg.in->getGrabAbilityControl().edgeRaycastParams.raycastLength;
  if (raycastLength == 0.0f)
  {
    return false;
  }
  // Length of the ray needs to be larger than 0.
  NMP_ASSERT (raycastLength > 0.0f);

  // Early out when there is no gravity i.e. there is no way of telling how the character
  // is going to hang off the edge.
  if (pkg.owner->owner->data->gravityMagnitude == 0.0f)
  {
    return false;
  }

  // Use default PxFilterData here to bypass the internal filtering, and we pass the real flags into
  // the callback. Make ray cast hit static and dynamic objects.
  physx::PxSceneQueryFilterData filterData;

  filterData.flags = physx::PxSceneQueryFilterFlags(
    physx::PxSceneQueryFilterFlag::eSTATIC | 
    physx::PxSceneQueryFilterFlag::eDYNAMIC | 
    physx::PxSceneQueryFilterFlag::ePREFILTER);

  physx::PxSceneQueryFlags flags(
    physx::PxSceneQueryFlag::eIMPACT |
    physx::PxSceneQueryFlag::eNORMAL |
    physx::PxSceneQueryFlag::eDISTANCE);

  // Skip self collisions and all other characters - that means we won't detect another
  // ragdoll etc. as the geometry below.
  uint32_t collisionIgnoreMask =
    1 << MR::GROUP_CHARACTER_PART | 
    1 << MR::GROUP_CHARACTER_CONTROLLER | 
    1 << MR::GROUP_INTERACTION_PROXY | 
    1 << MR::GROUP_NON_COLLIDABLE;

  MR::MorphemePhysX3QueryFilterCallback morphemePhysX3QueryFilterCallback(physx::PxFilterData(
    0,
    collisionIgnoreMask, 
    0, 
    0));

  // Calculate raycast point using brace hazard predicted impact position to edge projection
  // or edge point.
  NMP::Vector3 raycastPoint;
  if (pkg.feedIn->getBraceHazardImportance() > 0.0f)
  {
    const NMP::Vector3 impactPos = pkg.feedIn->getBraceHazard().impactPosition;
    NMP::Vector3 edgeDir = grabbableEdge.edge;
    const float edgeLength = edgeDir.normaliseGetLength();
    raycastPoint = projectPointOntoLineSegment(grabbableEdge.corner, edgeDir, impactPos, 0.0f, edgeLength);
  }
  else
  {
    raycastPoint = grabbableEdge.point;
  }

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, raycastPoint, SCALE_DIST(grabbableEdge.uprightNormal), NMP::Colour::BLUE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, raycastPoint, SCALE_DIST(grabbableEdge.otherNormal), NMP::Colour::LIGHT_BLUE);

  const float awayAlongEdgeOtherNormal = pkg.in->getGrabAbilityControl().edgeRaycastParams.raycastEdgeOffset;
  raycastPoint += (grabbableEdge.otherNormal * awayAlongEdgeOtherNormal);
  const physx::PxVec3 pxRaycastPoint(raycastPoint.x, raycastPoint.y, raycastPoint.z);

  // Stores raycast single result.
  physx::PxRaycastHit raycastHit;

  // Perform raycast returning a single result.
  MR::PhysicsScene* scene = pkg.getRootModule()->getCharacter()->getBody().getPhysicsRig()->getPhysicsScene();
  physx::PxScene* physXScene = ((MR::PhysicsScenePhysX3*)scene)->getPhysXScene();

  bool rayCollide = physXScene->raycastSingle(
    pxRaycastPoint,
    MR::nmVector3ToPxVec3(pkg.owner->owner->data->down),
    raycastLength,
    flags,
    raycastHit,
    filterData,
    &morphemePhysX3QueryFilterCallback);

  // Ray cast debug draw.
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, raycastPoint, SCALE_DIST(0.1f), NMP::Colour::LIGHT_GREEN);
  MR_DEBUG_DRAW_VECTOR(
    pDebugDrawInst,
    MR::VT_Normal,
    raycastPoint,
    (pkg.owner->owner->data->down * raycastLength),
    NMP::Colour::LIGHT_ORANGE);

  // Measure slope.
  if (rayCollide)
  {
    const NMP::Vector3 rayHitNormal(raycastHit.normal.x ,raycastHit.normal.y ,raycastHit.normal.z);
    const float upDotHitNormal = NMP::clampValue(rayHitNormal.dot(pkg.owner->owner->data->up), -1.0f, 1.0f);
    const float minSupportSlope = pkg.in->getEnableConditions().minSupportSlope;
    if (upDotHitNormal > cosf(minSupportSlope)) // When true edge is obstructed by relatively flat geometry just below it.
    {
      // Ray cast results.
      MR_DEBUG_DRAW_POINT(
        pDebugDrawInst,
        NMP::Vector3(raycastHit.impact.x, raycastHit.impact.y, raycastHit.impact.z),
        SCALE_DIST(0.2f),
        NMP::Colour::RED);

      MR_DEBUG_DRAW_VECTOR(
        pDebugDrawInst,
        MR::VT_Normal,
        NMP::Vector3(raycastHit.impact.x, raycastHit.impact.y, raycastHit.impact.z),
        SCALE_DIST(NMP::Vector3(raycastHit.normal.x ,raycastHit.normal.y ,raycastHit.normal.z)),
        NMP::Colour::RED);

      return true;
    }
    else
    {
      // Ray cast results.
      MR_DEBUG_DRAW_POINT(
        pDebugDrawInst,
        NMP::Vector3(raycastHit.impact.x, raycastHit.impact.y, raycastHit.impact.z),
        SCALE_DIST(0.2f),
        NMP::Colour::GREEN);

      MR_DEBUG_DRAW_VECTOR(
        pDebugDrawInst,
        MR::VT_Normal,
        NMP::Vector3(raycastHit.impact.x, raycastHit.impact.y, raycastHit.impact.z),
        SCALE_DIST(NMP::Vector3(raycastHit.normal.x ,raycastHit.normal.y ,raycastHit.normal.z)),
        NMP::Colour::GREEN);

      return false;
    }
  } // No raycast collision detected.

  return false;
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling

//----------------------------------------------------------------------------------------------------------------------
void Grab::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());

  data->useEnvironmentAwareness = false;

  // check whether character is in a balanced state, only grab when not stably balanced
  bool unbalanced = isUnbalanced(*this, timeStep);

  // Check whether character is already grabbing and calculate average grab point
  // This can be used as a starting point for yet unconstrained arms to aim for (left or right of it, to be precise)
  uint16_t numConstrainedArms;
  NMP::Vector3 avgConstraintPos;
  bool isConstrained = getConstrainedArmsInfo(*this, numConstrainedArms, avgConstraintPos);
  out->setNumConstrainedArms(numConstrainedArms);
  data->numConstrainedArms = numConstrainedArms; // Store to also send it in feedback

  // Update timer of how long the character has been grabbing
  if (isConstrained)
  {
    data->holdDuration += timeStep;
  }
  else
  {
    data->holdDuration = 0.0f;
  }

  // Conditional grabbing: decision can be forced by the game (if passing in an edge with quality > 1.0f)
  bool allowGrabNow = in->getHasForcedEdge();

  // only perform the other checks if the grab isn't forced already by the game
  if (!allowGrabNow)
  {
    // TODO rename isOnGround because it looks like it's const, but it updates state
    bool lyingOnGround = isOnGround(*this, isConstrained, timeStep, rootModule->getDebugInterface());
    allowGrabNow = unbalanced && !lyingOnGround;
  }

#ifdef USING_EUPHORIA_LOGGING
  // Indicates that character has stopped trying to grab because he's come to rest
  if (data->grabAllowed && !allowGrabNow)
    EUPHORIA_LOG_MESSAGE("Grab: aborted. Character has come to rest. \n");
  if (!data->grabAllowed && allowGrabNow)
    EUPHORIA_LOG_MESSAGE("Grab: allowed now. Character is unsupported. \n");
#endif
  // store for use in hysteresis calculations
  data->grabAllowed = allowGrabNow;

  // early out: nothing to do here
  if (!data->grabAllowed)
  {
    return;
  }

  // Check whether character's chest is likely to overshoot the edge and prevent grab in that case.
  if (in->getIgnoreOvershotEdges() && (in->getGrabbableEdgeImportance() > 0.0f) && (feedIn->getBraceHazardImportance() > 0.0f))
  {
    const Edge& edge = in->getGrabbableEdge();
    NMP::Vector3 topFaceInwardsDir = -0.5f * (edge.uprightNormal + edge.otherNormal);
    topFaceInwardsDir = topFaceInwardsDir.getComponentOrthogonalToDir(edge.uprightNormal);
    topFaceInwardsDir.fastNormalise();
    const NMP::Vector3& impactPos = feedIn->getBraceHazard().impactPosition;
    NMP::Vector3 edgeToImpact =  impactPos - edge.point;
    float impactPosInwardDist = edgeToImpact.dot(topFaceInwardsDir);
    float impactPosPlaneDist = fabs(edgeToImpact.dot(edge.uprightNormal));
    bool protectInstead = (impactPosPlaneDist < SCALE_DIST(0.1f)) && (impactPosInwardDist > SCALE_DIST(0.4f));

    MR_DEBUG_DRAW_PLANE(
      rootModule->getDebugInterface(),
      impactPos,
      in->getGrabbableEdge().uprightNormal,
      SCALE_DIST(0.2f),
      (protectInstead) ? NMP::Colour::RED : NMP::Colour::GREEN,
      0.3f);
    MR_DEBUG_DRAW_VECTOR(
      rootModule->getDebugInterface(),
      MR::VT_Normal,
      edge.point,
      topFaceInwardsDir * impactPosInwardDist,
      (protectInstead) ? NMP::Colour::RED : NMP::Colour::GREEN);

    if (protectInstead)
    {
      out->setEdgeOvershot(true);
      return;
    }
  }

  // look for and analyse edges when not already grabbing

  // Forward grabbable geometry to arm if EA was allowed to look for one, or if the game has sent an edge itself
  const float grabEdgeImportance = in->getGrabbableEdgeImportance();

  Edge grabbableEdge = in->getGrabbableEdge();
  Edge grabbableEdgeLocal = in->getGrabbableEdge(); // grabbable edge in local space

  // If we're not passed an edge and we were constrained to a game edge, then check to see if it
  // needs to be updated
  if (
    (grabEdgeImportance == 0 || grabbableEdge.gameEdgeID == data->cachedGrabbableEdgeLocal.gameEdgeID) && 
    (isConstrained) && 
    (data->cachedGrabbableEdgeLocal.gameEdgeID >= 0)
    )
  {
    // Just re-use the cached edge as it may have moved, and may not be getting detected. Get the
    // original edge, so long as it's still being set.
    if (in->getGameEdgesImportance(data->cachedGrabbableEdgeLocal.gameEdgeID) == 0.0f)
    {
      // Don't carry on grabbing if the edge isn't being set
      return;
    }
    grabbableEdge = in->getGameEdges(data->cachedGrabbableEdgeLocal.gameEdgeID);
    grabbableEdgeLocal = grabbableEdge;
  }

  if (grabEdgeImportance > 0.0f)
  {
    fixEdgeDirection(*this, grabbableEdge);
    fixEdgeDirection(*this, grabbableEdgeLocal);
    // world to local space (only modifies edges attached to shapes)
    convertEdgeToLocalSpace(grabbableEdgeLocal);
  }

  if (!isConstrained)
  {
    // If grab allowed, tell GrabDetection that it can look for patches using the EA system.
    // If game has specified an edge with quality > 1.0, it overrides anything found by EA, so no point using it.
    data->useEnvironmentAwareness = in->getGrabAbilityControlImportance() && !in->getHasForcedEdge();

    if (grabEdgeImportance > 0.0f)
    {
      // The edge direction determines the relative desired position of the hands. This is decided here.
      out->setEdgeToGrab(grabbableEdge);
      MR_DEBUG_DRAW_POINT(rootModule->getDebugInterface(), grabbableEdge.point, SCALE_DIST(0.5f), NMP::Colour::WHITE);

      data->reEvaluateGrabPoint = true;
#if defined(MR_OUTPUT_DEBUGGING)
      grabbableEdge.debugDraw(rootModule->getDebugInterface());
#endif
      // Perform ray-cast to assess whether edge is obstructed by geometry out and below the edge.
      // Always perform ray cast on first entry, then only when edge has changed provided it's a non-action edge.
      // Edge corresponds to the action or the ability type?
      const bool isEdgeActionType = (grabbableEdge.gameEdgeID >= 0) ? true : false;
      if (!(isEdgeActionType) && (data->rayCastEdge || (!(grabbableEdgeLocal.equals(data->cachedGrabbableEdgeLocal)))))
      {
        data->edgeIsObstructed = isEdgeObstructed(*this, grabbableEdge, rootModule->getDebugInterface());
        data->rayCastEdge = false;
      }

      out->setEdgeIsObstructed(data->edgeIsObstructed);

      // cache the local space edge
      data->cachedGrabbableEdgeLocal = grabbableEdgeLocal;

      // forward only one set of parameters for the grab, depending on whether the edge corresponds to the action
      // or the ability type
      if (isEdgeActionType)
      {
        // action-type grab
        out->setEndConstraintOptions(in->getGrabActionControl().constraintParams, 1.0f);
        out->setHoldTimer(in->getGrabActionControl().holdTimer, 1.0f);
      }
      else
      {
        // ability-type grab
        out->setEndConstraintOptions(in->getGrabAbilityControl().constraintParams, 1.0f);
        out->setHoldTimer(in->getGrabAbilityControl().holdTimer, 1.0f);
      }
    }

    data->isHanging = false;
    data->edgeBehindPeriod = 0.0f;
    // This dampens the spine to compensate for the high stiffness arms motion used in grabbing, so
    // the chest doesn't rotate too quickly in response
    out->setSpineDampingIncrease(4.0f);
  }
  else
  {
    // If we're grabbing a physical shape (which will be in local space), then don't change the grab
    // point - i.e. use the cached edge

    // Let go if GrabDetection filtered this shape out or shape was deleted.
    if (in->getLetGoEdge())
    {
      return;
    }

    if (grabbableEdge.shapeID >= 0)
    {
      // Either this edge is attached to a dynamic object, or edgeImportance is zero. In either
      // case, we can carry on trying to grab the cached edge, if only because erEndConstraint
      // doesn't handle updating the grab position on constraints with dynamic objects.

      // If this is a game edge and the dynamic object has changed then we need to drop the old edge
      // and pick up the new one. The easiest way to do this is provide no output for one update,
      // which will result in the constraint being released, so next update we'll be unconstrained
      // and available to grab the new edge. Note that there may be a delay before reaching for the
      // new edge due to the "Sequencing Durations: No hold" attribute in HoldAction.
      if (
        grabEdgeImportance > 0.0f && 
        grabbableEdge.gameEdgeID >= 0 &&
        grabbableEdge.shapeID != data->cachedGrabbableEdgeLocal.shapeID
        )
      {
        return;
      }
      grabbableEdgeLocal = data->cachedGrabbableEdgeLocal;
    }
    else
    {
      // We have a game edge that's not attached to a dynamic object. However, it might be moving,
      // so we want to grab the same position on the moved edge as in the cached edge. 
      float distAlongCachedEdge = 
        (data->cachedGrabbableEdgeLocal.point - data->cachedGrabbableEdgeLocal.corner).dot(data->cachedGrabbableEdgeLocal.edge.getNormalised());
      if (data->cachedGrabbableEdgeLocal.edge.dot(grabbableEdgeLocal.edge) < 0.0f)
      {
        // Preserve the edge sense
        grabbableEdgeLocal.corner += grabbableEdgeLocal.edge;
        grabbableEdgeLocal.edge *= -1.0f;
      }
      grabbableEdgeLocal.point = grabbableEdgeLocal.corner + grabbableEdgeLocal.edge.getNormalised() * distAlongCachedEdge;
      if (grabEdgeImportance == 0.0f)
      {
        grabbableEdgeLocal.gameEdgeID = data->cachedGrabbableEdgeLocal.gameEdgeID;
      }

      grabbableEdgeLocal.gameEdgeID = data->cachedGrabbableEdgeLocal.gameEdgeID;
      data->cachedGrabbableEdgeLocal = grabbableEdgeLocal;
    }

    // Convert to world space
    grabbableEdge = data->cachedGrabbableEdgeLocal;
    convertEdgeToWorldSpace(grabbableEdge);

    // Forced edges (quality > 0.0f) can now be clamped to normal range
    grabbableEdge.quality = NMP::clampValue(grabbableEdge.quality, 0.0f, 1.0f);
    fixEdgeDirection(*this, grabbableEdge);

    // Backwards grab detection; if more than one arm is grabbing an edge behind the body, the arms
    // can respond by at least one of them releasing. Note that hasTwistedAround also sets isHanging
    // output (should be changed, as we're forced to call the function and then ignore the result).
    bool twistedAround = hasTwistedAround(
      *this, grabbableEdge, numConstrainedArms, timeStep, rootModule->getDebugInterface());
    if (!grabbableEdge.isWall)
    {
      // If it's not a wall then ignore the result as it doesn't matter if we flip over the top etc.
      twistedAround = false;
    }
    out->setHasTwistedAround(twistedAround);

    // update the approximate grab point once we've grabbed it properly
    if (data->reEvaluateGrabPoint && numConstrainedArms > 1)
    {
      NMP::Vector3 edgeDir = grabbableEdge.edge;
      float edgeLength = edgeDir.normaliseGetLength();
      grabbableEdge.point = projectPointOntoLineSegment(grabbableEdge.corner, edgeDir, avgConstraintPos, 0, edgeLength);

      data->cachedGrabbableEdgeLocal = grabbableEdge;
      convertEdgeToLocalSpace(data->cachedGrabbableEdgeLocal);

      data->reEvaluateGrabPoint = false;
    }
    out->setEdgeToGrab(grabbableEdge);
    MR_DEBUG_DRAW_POINT(rootModule->getDebugInterface(), grabbableEdge.point, SCALE_DIST(0.5f), NMP::Colour::LIGHT_GREY);

#if defined(MR_OUTPUT_DEBUGGING)
    grabbableEdge.debugDraw(rootModule->getDebugInterface());
#endif

    // Forward only one set of parameters for the grab, depending on whether the edge corresponds to
    // the action or the ability type
    const GrabControl& grabControl = 
      grabbableEdge.gameEdgeID >= 0 
      ? in->getGrabActionControl() 
      : in->getGrabAbilityControl();

    out->setEndConstraintOptions(grabControl.constraintParams, 1.0f);
    out->setHoldTimer(grabControl.holdTimer, 1.0f);

    // When grabbing (try to) disable arm swing since it is ineffective - spin looks much better
    out->setArmSwingStiffnessScale(0.0f, 1.0f);

    // When already grabbing, tell limb support modules how to orient the chest. The limb support
    // modules will autonomously detect when they're constrained, and will respond to the desired
    // chestTM when it's set here
    ER::LimbTransform desiredChestTM;
    // spine up (y) and out a bit
    desiredChestTM.upDirection() = (owner->owner->data->up + grabbableEdge.otherNormal * 0.5f).getNormalised();
    // spine right (z): edge horizontally aligned
    NMP::Vector3 edgeHor = -1 * NMP::vNormalise(grabbableEdge.edge);
    edgeHor = edgeHor.getComponentOrthogonalToDir(owner->owner->data->up);
    edgeHor.normalise();
    desiredChestTM.rightDirection() = edgeHor;
    // spine forward:
    desiredChestTM.frontDirection() = NMP::vCross(desiredChestTM.upDirection(), desiredChestTM.rightDirection());
    desiredChestTM.frontDirection().normalise();

    // TM pass down, such that the support request can compete with or override the existing
    // requests, such as from headAvoid. Can change the leg swing strength by adjusting the
    // maxStiffnessScale in the requests away from 1
    NMP::Quat rot = desiredChestTM.toQuat() * ~owner->owner->data->spineLimbSharedStates[0].m_endTM.toQuat();
    RotationRequest rotRequest(rot.toRotationVector(), SCALING_SOURCE);
#if defined(MR_OUTPUT_DEBUGGING)
    rotRequest.debugControlAmounts.setControlAmount(ER::grabControl, 1.0f);
#endif
    // A smaller value her results in arm spin, larger results in swing. Neither looks right really
    rotRequest.imminence = grabControl.grabChestParams.imminence;
    rotRequest.passOnAmount = grabControl.grabChestParams.passOnAmount;
    rotRequest.stiffnessScale = grabControl.grabChestParams.stiffnessScale;
    out->setRotationRequest(rotRequest, 1.0f);

    // Pull in the direction of the edge by a proportion of arm length
    const float desChestDist = data->maxArmLength * (1.0f - in->getGrabAliveParams().pullUpAmount);
    NMP::Vector3 liftDir = avgConstraintPos - feedIn->getCentreOfShoulders();
    const float currentChestDist = liftDir.normaliseGetLength();
    // Note that we allow the lift amount to be negative - i.e. allow "push down" as well as "pull
    // up". The reason is that otherwise we end up using full strength to maintain the current
    // position when pull up amount is zero, but pull up strength is positive. This results in us
    // drifting upwards (due to gravity compensation, IK errors, noise etc). An alternative solution
    // would be to decrease strength when we don't need to pull up, but that would mean we'd always
    // sag below where we're supposed to be. See MORPH-15092 (which occurred when liftAmount was
    // clamped to be > 0). However, if people really want loose arms when hanging, they can just set
    // the strength to zero.
    float liftAmount = currentChestDist - desChestDist;
    TranslationRequest transRequest(liftAmount * liftDir, SCALING_SOURCE);
    desiredChestTM.r[3] = owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() + transRequest.translation;
#if defined(MR_OUTPUT_DEBUGGING)
    transRequest.debugControlAmounts.setControlAmount(ER::grabControl, 1.0f);
#endif
    out->setTranslationRequest(transRequest, 1.0f);

    // Look at
    TargetRequest lookTarget;
    const NMP::Vector3 chestRightDir = owner->owner->data->spineLimbSharedStates[0].m_endTM.rightDirection();
    NMP::Quat q;
    NMP::Vector3 rotV;
    NMP::Vector3 lLookConeLimit; // Left look cone limit.
    NMP::Vector3 rLookConeLimit; // Right look cone limit.
    const HoldTimer& holdTimer = (grabbableEdge.shapeID == -1) ? (in->getGrabActionControl().holdTimer) : (in->getGrabAbilityControl().holdTimer);

    // Look downwards and to the sides along the plane spanned by chest-to-pelvis and character side directions
    // clamped to cone before letting go the edge.
    if (data->holdDuration > in->getTimeRatioBeforeLookingDown() * holdTimer.maxHoldPeriod)
    {
      const ER::LimbTransform& chest = owner->owner->data->spineLimbSharedStates[0].m_endTM;
      const ER::LimbTransform& pelvis = owner->owner->data->spineLimbSharedStates[0].m_rootTM;
      NMP::Vector3 chestPelvisDir = (pelvis.translation() - chest.translation());
      chestPelvisDir.normalise();
      const float rightDotDown = NMP::clampValue((chestRightDir.dot(-(owner->owner->data->up))), -0.5f, 0.5f);
      q.forRotation(chestPelvisDir, chestRightDir);
      rotV = q.toRotationVector();
      rotV.normalise();
      const float maxLookConeHalfSpanAngleRad = 1.13446f;  // 65 degrees.
      rotV *= maxLookConeHalfSpanAngleRad;
      q.fromRotationVector(rotV);
      q.normalise();
      // Set up look cone limits.
      q.rotateVector(chestPelvisDir, rLookConeLimit);
      q.conjugate();
      q.rotateVector(chestPelvisDir, lLookConeLimit);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(), SCALE_DIST(lLookConeLimit), NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(), SCALE_DIST(rLookConeLimit), NMP::Colour::GREEN);
      // Generate look at target.
      (lookTarget.target).lerp(lLookConeLimit, rLookConeLimit, (rightDotDown + 0.5f));
      (lookTarget.target).normalise();
      lookTarget.target = SCALE_DIST(lookTarget.target);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
        lookTarget.target, NMP::Colour::LIGHT_GREEN);
      lookTarget.target += 
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();
    }
    else // Look towards the edge and to the sides along the plane spanned by lift and character side directions
         // clamped to cone.
    {
      const float rightDotUp = NMP::clampValue((chestRightDir.dot(owner->owner->data->up)), -0.5f, 0.5f);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(),
        MR::VT_Normal, owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(), SCALE_DIST(liftDir),
        NMP::Colour::WHITE);
      q.forRotation(liftDir, chestRightDir);
      rotV = q.toRotationVector();
      // Safe vector normalisation in case liftDir and chestRightDir are parallel.
      rotV.normalise(NMP::Vector3::InitTypeOneX);
      const float maxLookConeHalfSpanAngleRad = 1.22173f;  // 70 degrees.
      rotV *= maxLookConeHalfSpanAngleRad;
      q.fromRotationVector(rotV);
      q.normalise();
      // Set up look cone limits.
      q.rotateVector(liftDir, rLookConeLimit);
      q.conjugate();
      q.rotateVector(liftDir, lLookConeLimit);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
        SCALE_DIST(lLookConeLimit), NMP::Colour::LIGHT_BLUE);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
        SCALE_DIST(rLookConeLimit), NMP::Colour::LIGHT_BLUE);
      // Generate look at target.
      (lookTarget.target).lerp(rLookConeLimit, lLookConeLimit, (rightDotUp + 0.5f));
      (lookTarget.target).normalise();
      lookTarget.target = SCALE_DIST(lookTarget.target);
      MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal,
        owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
        lookTarget.target, NMP::Colour::YELLOW);
      lookTarget.target += owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();
    }

    lookTarget.imminence = SCALE_FREQUENCY(8.0f);
#if defined(MR_OUTPUT_DEBUGGING)
    lookTarget.debugControlAmounts.setControlAmount(ER::grabControl, 1.0f);
#endif
    lookTarget.stiffnessScale = 1.5f;
    lookTarget.passOnAmount = 0.5f;
    out->setLookTarget(lookTarget, 1.0f);

    MR_DEBUG_DRAW_POINT(rootModule->getDebugInterface(), lookTarget.target, SCALE_DIST(0.1f), NMP::Colour::WHITE);
    MR_DEBUG_DRAW_MATRIX(rootModule->getDebugInterface(), desiredChestTM, SCALE_DIST(0.25f));
    MR_DEBUG_DRAW_MATRIX(rootModule->getDebugInterface(),
      owner->owner->data->spineLimbSharedStates[0].m_endTM, SCALE_DIST(0.25f));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Grab::feedback(float NMP_UNUSED(timeStep))
{
  // Information useful to game
  feedOut->setNumConstrainedArms(data->numConstrainedArms);

  // How hard Hold will be trying to reach and hold the edge
  if (data->numConstrainedArms == 0)
  {
    if (in->getGrabbableEdgeImportance() > 0.0f)
    {
      feedOut->setHoldAttemptImportance(in->getGrabbableEdge().quality);
    }
    else
    {
      feedOut->setHoldAttemptImportance(0.0f);
    }
  }
  else
  {
    feedOut->setHoldAttemptImportance(1.0f);
  }

  feedOut->setHoldDuration(data->holdDuration);

  Edge worldSpaceEdge = data->cachedGrabbableEdgeLocal;

  // transform back to world space
  convertEdgeToWorldSpace(worldSpaceEdge);

  feedOut->setEdgeForwardNormal(worldSpaceEdge.otherNormal);

  feedOut->setUseEnvironmentAwareness(data->useEnvironmentAwareness);
}

//----------------------------------------------------------------------------------------------------------------------
void Grab::entry()
{
  data->useEnvironmentAwareness = false;
  data->grabAllowed = false;

  // Start off thinking we're balanced. Otherwise character will immediately start looking around
  // for things to hold onto.
  data->balancedPeriod = 0.25f;
  data->desiredTorsoFwd = NMP::Vector3(1.0f, 0.0f, 0.0f);

  data->edgeBehindPeriod = 0.0f;
  data->onGroundPeriod = 0.0f;

  data->holdDuration = 0.0f;

  data->rayCastEdge = true;
  data->edgeIsObstructed = false;
  data->reEvaluateGrabPoint = true;

  // Determine largest arm length. Used in later calculations to decide how high to pull up.
  data->maxArmLength = 0.0f;
  for (uint32_t i = 0; i < owner->owner->data->numArms; i++)
  {
    float armLength = owner->owner->data->baseToEndLength[MyNetworkData::firstArmNetworkIndex + i];
    if (armLength > data->maxArmLength)
    {
      data->maxArmLength = armLength;
    }
  }
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

