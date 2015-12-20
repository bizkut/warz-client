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
#include "myNetworkPackaging.h"
#include "ArmReachForWorldPackaging.h"
#include "ArmReachForWorld.h"
#include "ArmPackaging.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE pkg.owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "euphoria/erLimbTransforms.h"

#include "euphoria/erPath.h"
#include "Helpers/Helpers.h"
#include "helpers/Reach.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// Swivel heuristic feature
// aux for update: calculates swivel amount according to where the target is.
//----------------------------------------------------------------------------------------------------------------------
static float swivelHeuristicUpdateAux(
  const ArmReachForWorldUpdatePackage& pkg, const NMP::Vector3& ikTgt,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  NMP::Vector3 chestPos
    = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_reachLimit.m_transform.translation();
  NMP::Vector3 spineUpDir(chestPos - pkg.owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation());
  spineUpDir.fastNormalise();
  NMP::Vector3 shoulderPos
    = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_reachLimit.m_transform.translation();

  // shoulderToSpineLoca and direction and magnitude
  NMP::Vector3 shoulderToSpineLoca(chestPos - shoulderPos);
  shoulderToSpineLoca -= (spineUpDir.dot(shoulderToSpineLoca) * spineUpDir);
  float shoulderToSpineDistance = NMP::fastSqrt(shoulderToSpineLoca.magnitudeSquared());
  NMP::Vector3 shoulderToSpineLocaDir(shoulderToSpineLoca);
  shoulderToSpineLocaDir.fastNormalise();
  NMP::Vector3 shoulderPointOnSpine = shoulderPos + shoulderToSpineLoca;

  // spine to target projection (target is projected onto a cylinder with axis the spine up dir and radius the
  // shoulder to spine distance)
  NMP::Vector3 spineToTarget((ikTgt - chestPos).getComponentOrthogonalToDir(spineUpDir));
  spineToTarget.fastNormalise();
  spineToTarget *= shoulderToSpineDistance;
  NMP::Vector3 targetProjection = shoulderPointOnSpine + spineToTarget;

  // shoulderToTarget
  NMP::Vector3 shoulderToTargetProjection(targetProjection - shoulderPos);
  float shoulderToTargetProjectionDotShoulderToSpineLocaDir = shoulderToTargetProjection.dot(shoulderToSpineLocaDir);

#if 0
  NMP::Vector3 pointOnSpine = (ikTgt - spineToTargetLocal);
  NMP::Vector3 testDotPoint = shoulderPos + shoulderToSpineLocaDir * shoulderToTargetProjectionDotShoulderToSpineLocaDir;
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, testDotPoint, SCALE_DIST(0.03f), NMP::Colour::RED);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, shoulderPos, SCALE_DIST(0.03f), NMP::Colour::YELLOW);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, shoulderPointOnSpine, SCALE_DIST(0.03f), NMP::Colour::YELLOW);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, shoulderPos, testDotPoint - shoulderPos, NMP::Colour::YELLOW);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, pointOnSpine, SCALE_DIST(0.1f), NMP::Colour::WHITE);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, targetProjection, SCALE_DIST(0.1f), NMP::Colour::WHITE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
    shoulderPointOnSpine, targetProjection - shoulderPointOnSpine, NMP::Colour::WHITE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_Delta, chestPos, spineUpDir, NMP::Colour::WHITE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
    MR::VT_Delta, shoulderPos, shoulderPointOnSpine - shoulderPos, NMP::Colour::YELLOW);
#endif

  // Swivel amount heuristic. First by height of target. If height is above shoulder or below shoulder,
  // but "above elbow" denoted by half arm length) we base the heuristic on that height offset otherwise we
  // base the heuristic on the lateral offset
  const float halfArmLength = pkg.owner->data->length * 0.5f;
  float tgtToShoulder = (shoulderPos - ikTgt).dot(spineUpDir);
  float swivelAmount = 0.0f;
  // above shoulder
  if (tgtToShoulder < 0.0f)
  {
    swivelAmount = -0.5f;
  }
  // above half arm length
  else if (tgtToShoulder < halfArmLength)
  {
    swivelAmount = -0.5f * ((halfArmLength - tgtToShoulder) / halfArmLength);
  }
  // below half arm length
  else
  {
    swivelAmount =
      (shoulderToTargetProjectionDotShoulderToSpineLocaDir - shoulderToSpineDistance) / shoulderToSpineDistance;
    swivelAmount = NMP::clampValue(swivelAmount, -1.0f, 1.0f);
  }
  return swivelAmount;
}


// Calculates a "reach sphere", (approximation to the reach workspace) and if ikTgt is not within it then this is
// projected onto the surface. Returns true if ikTgt is modified, false otherwise.
//
// The reach sphere calculation accounts for whether the character is in support or not as follows.
// If the character is not supported (balancing) the sphere is centered at the current position of the arm reach limit 
// and has radius arm reach length (limb length scaled by maxReachScale); in support the sphere is centered on a point 
// at pelvis height vertically (direction opposing gravity) above the support with radius the sum of arm and spine
// lengths.
// 
static bool calculatePointProjectedIntoReachSphere(const ArmReachForWorldAPIBase& pkg, NMP::Vector3& ikTgt,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // We assume that the target is inside the reach sphere and there is no projection required, this is determined below
  // and the following flag set as required.
  //
  bool targetModified = false;

  // Determine reach workspace: a sphere with position and radius depending on whether character is in support or not.
  // - no support case: "reach radius" is arm reach length, position is arm reach limit position.
  // - in support case: "reach radius" is spine plus arm lengths, position is leg length above the support.
  //
  float reachRadius;
  NMP::Vector3 reachSpherePos;
  const float armLength = pkg.data->maxReach;

  // Calculate reach sphere for character:
  // - no support case
  if (pkg.in->getSupportTMImportance() == 0.0f)
  {
    reachRadius = armLength;
    reachSpherePos
      = pkg.owner->owner->data->armLimbSharedStates[pkg.owner->data->childIndex].m_reachLimit.m_transform.translation();
  }
  // - in support case
  else
  {
    const float legLength = pkg.owner->owner->data->defaultPoseLimbLengths[pkg.owner->owner->data->firstLegRigIndex];
    const float spineLength = pkg.owner->owner->data->defaultPoseLimbLengths[pkg.owner->owner->data->firstSpineRigIndex];

    reachRadius = spineLength + armLength;
    reachSpherePos = pkg.in->getSupportTM().translation() + pkg.owner->owner->data->up * legLength;
  }

  // Determine if the target is outside the reach sphere, if so project it onto the surface and set the return flag.
  //
  float targetDistanceFromReachCentre = NMP::fastSqrt((ikTgt - reachSpherePos).magnitudeSquared());
  if (targetDistanceFromReachCentre > reachRadius)
  {
    ikTgt = reachSpherePos + (ikTgt - reachSpherePos).getNormalised() * reachRadius;
    targetModified = true;
  }

#if defined(MR_OUTPUT_DEBUGGING)
  // Debug draw the resulting target and sphere.
  NMP::Matrix34 sphereTM;
  sphereTM.identity();
  sphereTM.setTranslation(reachSpherePos);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(0.1f), NMP::Colour::TURQUOISE);
  MR_DEBUG_DRAW_SPHERE(pDebugDrawInst, sphereTM, reachRadius, NMP::Colour::LIGHT_BLUE);
#endif

  return targetModified;
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->data->dimensionalScaling

//----------------------------------------------------------------------------------------------------------------------
// ArmReachForWorldUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorldUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* pDebugDrawInst)
{
  // No limb control means there's no reaching to be done so neither update() nor feedback() need run.
  // The importance is referenced here in update() and again in feedback() so it is cached here even if 0.
  data->reachImportance = in->getLimbControlImportance();
  if (data->reachImportance == 0.0f)
  {
    return;
  }
  
  // If we did not return above then we have limbcontrol input. This must be accompanied by some reach params if not 
  // we'd like to know (in debug).
  NMP_ASSERT(in->getReachParamsImportance() > 0.0f);
  // In release we just won't reach, but we zero the internal reachImportance so feedback() also returns.
  if (in->getReachParamsImportance() == 0.0f)
  {
    data->reachImportance = 0.0f;
    return;
  }

  const ReachWorldParameters& reachParameters = in->getReachParams();
  
  // Derive a maximum reach length as the anatomical reach length scaled by the user specified scale ( we require 
  // a lower bound on this greater than zero for numerical stability but it is worth noting that in
  // practice since maxReach represents the radius of a sphere around the shoulder which the final ik target will
  // be forced to lie inside, a truly sensible lower bound will be much larger than zero and depends on the rig.
  // No upper bound is enforced to allow client behaviours to effectively disable this feature.
  //  
  // Check lower bound to avoid division by zero.
  data->maxReach = owner->data->length * reachParameters.maxReachScale;
  NMP_ASSERT(data->maxReach > 0.001f);

  // Grab the current reach target input.
  //
  const LimbControl& limbControl = in->getLimbControl();
  NMP::Vector3 ikTgt = limbControl.getTargetPos();

  // Filter unreachable target: if necessary we project the limbcontrol onto a "reach sphere" (approximation of the 
  // arm or, in support, the full-body reach workspace).
  // If the target is modified (iff deemed out of reach) we adjust the reach importance by the user specified
  // multiplier.
  //
  if (calculatePointProjectedIntoReachSphere(*this, ikTgt, pDebugDrawInst))
  {
    data->reachImportance *= in->getReachParams().unreachableTargetImportanceScale;
  }

  // Grab the current effector position.
  const NMP::Vector3 effector = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(); // Effector position.
  // Modify/filter target for self-avoidance, if required.
  // Either way we store the angular error for reference in feedback().
  //
  data->effectorToTargetAngle = 0.0f;
  if (in->getReachParams().selfAvoidanceStrategy == ReachWorldParameters::automatic)
  {
    ikTgt = owner->avoidSpineUsingAngleCoefficient(
      ikTgt, owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation(), 
      in->getReachParams().selfAvoidanceSlackAngle, 
      pDebugDrawInst, &data->effectorToTargetAngle);
  }
  else
  {
    const NMP::Vector3 reachOrg 
      = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation();
    const NMP::Vector3 reachUp
      = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation() 
      - owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation(); // Up
    const NMP::Vector3 reachForward = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir();                       // Forward

    data->effectorToTargetAngle = 
      calcReachAngleToTarget(ikTgt, effector, NM_PI / 60.0f, reachOrg, reachUp, reachForward, NULL, NULL);
  }

  // Given effector, target and a maximum (effector) displacement returns a new target no more than displacement
  // away from the effector (aux for velocity limiting).

  // If this code is allowed to run after the above, then it is quite possible that it undoes
  // some of the good work by producing a target that eg. lies inside the leg. However,
  // we have yet to encounter significant use cases where the issue arises.

  // MORPH-14211: expose parameter to User via ArmReachForWorld behaviour interface.
  // This data is originally provided via ReachForBodyBehaviourInterface, look for reachActionParams.effectorSpeedLimit
  const float effectorSpeedLimit = SCALE_VEL(20.0f);
  float maxDisplacement = calcMaxDisplacement(
    effectorSpeedLimit, limbControl.getDampingRatio(), limbControl.getStiffness());
  ikTgt = calcDisplacementLimitedTarget(
    ikTgt,
    effector,
    maxDisplacement);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(0.05f), NMP::Colour::LIGHT_BLUE);

  // Bake result into a control structure.
  LimbControl& control = out->startControlModification();
  control = limbControl;

  // Final clamp to enforce max reach property applied if the ikTarget is further than maxReach from the shoulder
  //
  NMP::Vector3 pivot = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition;
  NMP::Vector3 shoulderToTarget = ikTgt - pivot;
  float shoulderToTargetDistance = shoulderToTarget.magnitude();
  if (shoulderToTargetDistance > data->maxReach)
  {
    shoulderToTarget *= (data->maxReach / shoulderToTargetDistance);
    ikTgt = pivot + shoulderToTarget;
  }

  // Debug draw final target.
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(0.5f), NMP::Colour::DARK_PURPLE);

  control.setTargetPos(ikTgt, limbControl.getPositionWeight());
  
  // IK substep size for advanced IK
  NMP_ASSERT( NMP::valueInRange(in->getReachParams().ikSubstepSize, 0.01f, 1.0f));
  control.setTargetDeltaStep(in->getReachParams().ikSubstepSize);

  // Swivel amount
  float swivelAmount = 0.0f; // default to no swivel case
  // TODO (MORPH-22196) this is similar to reach for body and should be shared code.
  int32_t swivelMode = in->getReachParams().swivelMode;
  switch (swivelMode)
  {
    case ER::REACH_SWIVEL_SPECIFIED:
      swivelAmount = in->getReachParams().swivelAmount;
      break;
    case ER::REACH_SWIVEL_AUTOMATIC:
      swivelAmount = swivelHeuristicUpdateAux(*this, ikTgt, pDebugDrawInst);
      break;
    case ER::REACH_SWIVEL_NONE:
      swivelAmount = 0.0f;
    break;
    default:
      NMP_ASSERT_FAIL_MSG("Swivel mode was not correctly initialised.")
    break;
  }
  control.setSwivelAmount(swivelAmount);

  // Target normal.
  NMP::Vector3 normal = limbControl.getTargetNormal();
  float normalWeight = limbControl.getNormalWeight();

  float endEffectorToTargetDistance = 
    NMP::fastSqrt((in->getLimbControl().getTargetPos() - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation()).magnitudeSquared());
  float targetRadius = SCALE_DIST(0.2f);
  float normalWeightMul = NMP::maximum( 1.0f - (endEffectorToTargetDistance / targetRadius), 0.0f);
  normalWeight *= normalWeightMul;

  control.setTargetNormal(normal, normalWeight);

  // When actively reaching increase the skin width to avoid penetration when we're reaching towards
  // a physical object.
  control.setSkinWidthIncrease(SCALE_DIST(0.1f));

  out->stopControlModification(in->getLimbControlImportance() * data->reachImportance);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorldFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // no action for targets of zero importance except
  if (in->getLimbControlImportance() == 0.0f || data->reachImportance == 0.0f)
  {
    return;
  }

  // Retrieve the target position and apply the same projection onto reach space filter as used in update.
  NMP::Vector3 targetPos(in->getLimbControl().getTargetPos());
  calculatePointProjectedIntoReachSphere(*this, targetPos, pDebugDrawInst);

  // In feedback, we set the rotation requests and the look parameters.
  

  // Make available the current error (end effector to target offset distance) to the rest of the network.
  //
  NMP::Vector3 endEffectorPos(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation());
  float endEffectorToTargetDistance = NMP::fastSqrt((targetPos - endEffectorPos).magnitudeSquared());
  feedOut->setDistanceToTarget(endEffectorToTargetDistance);

  // Set full body motion requests only if the body is "in support".
  //
  if (in->getSupportAmount() > 0.0f)
  {
    // Determine reach workspace (a sphere of radius "reach radius" hovering at pelvis height above the support point) 
    // 
    NMP::Vector3 worldUp(owner->owner->data->up);
    const float legLength = owner->owner->data->defaultPoseLimbLengths[owner->owner->data->firstLegRigIndex];
    const float armLength = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_distance;
    const float spineLength = owner->owner->data->defaultPoseLimbLengths[owner->owner->data->firstSpineRigIndex];
    float reachRadius = spineLength + armLength;
    NMP::Vector3 supportPoint = in->getSupportTM().translation();
    NMP::Vector3 reachSpherePos = supportPoint + worldUp * legLength;

    // Determine if the target is outside the reach sphere and if so project it onto the surface
    //
    float targetDistanceFromReachCentre = NMP::fastSqrt((targetPos - reachSpherePos).magnitudeSquared());
    bool outOfReach = reachRadius - targetDistanceFromReachCentre < 0.0f;
    if (outOfReach)
    {
      NMP::Vector3 unreachableTargetProjection = reachSpherePos + (targetPos - reachSpherePos).getNormalised() * reachRadius;
      targetPos = unreachableTargetProjection;
    }

    // Retrieve or calculate character state info required for request calcs below.
    //
    NMP::Vector3 pelvisPos(owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation());
    NMP::Vector3 chestPos(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation());
    NMP::Vector3 spineUp(chestPos - pelvisPos);
    NMP::Vector3 chestToTargetDirection = targetPos - chestPos;
    const float chestToTargetDistance = chestToTargetDirection.magnitude();
    chestToTargetDirection /= chestToTargetDistance;
    NMP::Vector3 armBaseToTarget = targetPos - 
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition;

    // Compute multipliers to reflect distribution of rotation between chest and pelvis as given by the reach params.
    //
    float sum = in->getReachParams().pelvisRotationScale + in->getReachParams().chestRotationScale;
    float factor = NMP::floatSelect(sum - 1.0f, 1.0f / sum, 1.0f);
    float pelvisRotationScale = in->getReachParams().pelvisRotationScale * factor;
    float chestRotationScale = in->getReachParams().chestRotationScale  * factor;

    // Pre-calculate effector to target angle quat, if chest and/or pelvis direction request is required.
    //
    NMP::Quat qEffToTarget;
    if(chestRotationScale + pelvisRotationScale > 0.0f)
    {
      qEffToTarget.fromAxisAngle(spineUp.getNormalised(), data->effectorToTargetAngle);
    }

    // Calculate required chest rotation and formulate a corresponding direction request.
    //
    if (chestRotationScale > 0.0f)
    {
      // Request a chest rotation that brings the shoulder closer to the target position if this is necessary to reach
      // the target. This may well involve making the chest forward direction perpendicular to the direction from chest 
      // to target so that the character's shoulder points toward the target. 
      
      // Try to rotate the chest so that a triangle is formed by the line from the chest to the target, the line from 
      // the shoulder to the target and the line from the chest to the shoulder. Find cosine of angle at the chest 
      // position in the triangle using the cosine rule and the lengths of the sides of the triangle. Use this to find
      // the angle between the current chest to target direction and the desired chest forward direction needed to form 
      // the triangle.
      //
      // note: -ve cosAngle indicates that a triangle cannot be formed because the target is out of reach (the sum of
      //       the arm length and the distance from chest to shoulder is less than the distance from chest to target).
      const float cosAngle = NMP::clampValue(
        (NMP::sqr(owner->owner->data->selfAvoidanceRadius) + NMP::sqr(chestToTargetDistance) - NMP::sqr(armLength)) /
        (2.0f * owner->owner->data->selfAvoidanceRadius * chestToTargetDistance), 0.0f, 1.0f); 
      const float targetDirToChestDirAngle = NM_PI_OVER_TWO - NMP::fastArccos(cosAngle);

      // Find axis about which chest should rotate. This will be parallel or anti-parallel with world up direction.
      NMP::Vector3 chestRotationAxis =
        NMP::vCross(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir(), 
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.frontDirection());
      chestRotationAxis = owner->owner->data->up * NMP::vDot(chestRotationAxis, owner->owner->data->up);
      chestRotationAxis.fastNormalise();

      // Find current chest forward direction projected into ground plane.
      NMP::Vector3 desiredChestDirection = chestToTargetDirection.getComponentOrthogonalToDir(owner->owner->data->up);
      desiredChestDirection.fastNormalise();

      // Rotate current chest direction to find desired chest direction.
      NMP::Quat targetDirToChestDir(chestRotationAxis, targetDirToChestDirAngle);
      desiredChestDirection = targetDirToChestDir.rotateVector(desiredChestDirection);

      // Generate chest rotation request.
      NMP::Vector3 chestFwdLocal(1, 0, 0);
      DirectionRequest chestDirRequest(chestFwdLocal, desiredChestDirection, SCALING_SOURCE);

      chestDirRequest.imminence = NMP::minimum(in->getReachParams().reachImminence, SCALE_IMMINENCE(2.0f));
#if defined(MR_OUTPUT_DEBUGGING)
      chestDirRequest.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif

      feedOut->setChestDirectionRequest(chestDirRequest, in->getLimbControlImportance() * data->reachImportance);

#if defined(MR_OUTPUT_DEBUGGING)
      float lengthScale = SCALE_DIST(1.0f);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, pelvisPos, 
        lengthScale * owner->owner->data->spineLimbSharedStates[0].m_rootTM.frontDirection(), NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, chestPos,
        lengthScale * owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.frontDirection(),
        NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, chestPos,
        lengthScale * desiredChestDirection, NMP::Colour::DARK_ORANGE);
#endif
    }

    // Calculate required pelvis rotation and formulate a corresponding direction request.
    //
    if (pelvisRotationScale > 0.0f)
    {
      NMP::Quat desiredOrientation = owner->owner->data->spineLimbSharedStates[0].m_rootTM.toQuat();
      NMP::Quat aligned;
      aligned.multiply(qEffToTarget, 
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.toQuat());

      NMP::Quat desiredPelvisPitch;
      desiredPelvisPitch.forRotation(owner->owner->data->spineLimbSharedStates[0].m_rootTM.frontDirection(),
        targetPos - pelvisPos);
      aligned.multiply(desiredPelvisPitch);

      desiredOrientation.slerp(aligned, pelvisRotationScale);

      // Pelvis direction request : align pelvis front towards the target.
      NMP::Vector3 pelvisFwdLocal(1, 0, 0);
      DirectionRequest pelvisDirRequest(pelvisFwdLocal, desiredOrientation.getXAxis(), SCALING_SOURCE);
      pelvisDirRequest.imminence = NMP::maximum(in->getReachParams().reachImminence, SCALE_IMMINENCE(2.0f));
#if defined(MR_OUTPUT_DEBUGGING)
      pelvisDirRequest.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
      feedOut->setPelvisDirectionRequest(pelvisDirRequest, in->getLimbControlImportance());

      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, in->getSupportTM().translation(), \
        in->getSupportTM().frontDirection(), NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, pelvisPos, \
        owner->owner->data->spineLimbSharedStates[0].m_rootTM.frontDirection(), \
        NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, pelvisPos, desiredOrientation.getXAxis(), NMP::Colour::RED);
    }

    // Compute chest and pelvis translation requests.

    // Chest translation.
    //
    if (in->getReachParams().maxChestTranslation > 0.0f)
    {
      // Determine whether the chest must move to enable this arm to reach the target.
      const float armBaseToTargetDistance = NMP::fastSqrt(armBaseToTarget.magnitudeSquared());
      const float handToTargetDistance    = NMP::maximum(armBaseToTargetDistance - armLength, 0.0f);
      const float translationImportance   = NMP::clampValue(handToTargetDistance / SCALE_DIST(0.1f), 0.0f, 1.0f);

      if (translationImportance > 0.0f)
      {
        // Find vertical component of required chest translation.
        const NMP::Vector3 handToTarget = armBaseToTarget * (handToTargetDistance / armBaseToTargetDistance);
        const NMP::Vector3 handToTargetUpComponent =
          handToTarget - handToTarget.getComponentOrthogonalToDir(owner->owner->data->up);

        // Calculate horizontal component of chest translation such that the spine would not be compressed if the 
        // translation occurred. This should cause the character to lean forward more to reach low targets.
        NMP::Vector3 directionInGroundPlane =
          (targetPos - in->getSupportTM().translation()).getComponentOrthogonalToDir(owner->owner->data->up);
        directionInGroundPlane.fastNormalise();
        const NMP::Vector3 handToTargetHorizontalComponent = directionInGroundPlane *
          NMP::fastSqrt(NMP::maximum(NMP::sqr(spineLength) - handToTargetUpComponent.magnitudeSquared(), 0.0f));

        TranslationRequest chestTranslationRequest(NMP::Vector3::InitZero, SCALING_SOURCE);
        chestTranslationRequest.translation = 
          (handToTargetUpComponent + handToTargetHorizontalComponent) * data->reachImportance;
        chestTranslationRequest.imminence = in->getReachParams().reachImminence;
        chestTranslationRequest.passOnAmount = 0.0f; // Zero as this module is setting targets for other limbs by hand.
#if defined(MR_OUTPUT_DEBUGGING)
        chestTranslationRequest.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
       feedOut->setChestTranslationRequest(
         chestTranslationRequest,
         in->getLimbControlImportance() * translationImportance);

       MR_DEBUG_DRAW_POINT(
         pDebugDrawInst,
         owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation()
         + chestTranslationRequest.translation,
         SCALE_DIST(0.4f),
         NMP::Colour::YELLOW)
      }
    }

    // Pelvis translation.
    //
    if (in->getReachParams().maxPelvisTranslation > 0.0f)
    {
      float targetHeight =
        NMP::maximum(NMP::vDot(targetPos - in->getSupportTM().translation(), owner->owner->data->up), 0.0f);
      targetHeight = legLength - ((legLength - targetHeight) * data->reachImportance);
      targetHeight = NMP::maximum(targetHeight, legLength * (1.0f - in->getReachParams().maxPelvisTranslation));
      const float currentHeight =
        NMP::maximum(NMP::vDot(owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation()
        - in->getSupportTM().translation(), owner->owner->data->up), 0.0f);
      const NMP::Vector3 translationRequired = 
        owner->owner->data->down * (currentHeight - targetHeight);

      TranslationRequest pelvisTranslation(translationRequired, SCALING_SOURCE);
      pelvisTranslation.imminence = in->getReachParams().reachImminence;
      pelvisTranslation.passOnAmount = 0.0f;

      MR_DEBUG_DRAW_POINT(pDebugDrawInst, 
        in->getSupportTM().translation() + (owner->owner->data->up * targetHeight),
        SCALE_DIST(0.4f),
        NMP::Colour::DARK_GREEN);

#if defined(MR_OUTPUT_DEBUGGING)
      pelvisTranslation.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
      feedOut->setPelvisTranslationRequest(pelvisTranslation, in->getLimbControlImportance());
#if 0
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, pelvisPos, pelvisTranslation.translation, NMP::Colour::WHITE);
#endif
    }
  }
  else if (in->getReachParams().pelvisRotationScale + in->getReachParams().chestRotationScale > 0.0f)
  {
    // When character is not supported, we only send a root rotation request.
    // Root rotation request : align chest front in direction to the target.
    NMP::Vector3 currentDir =
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.frontDirection();
    NMP::Vector3 targetDir = targetPos
      - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation();
    NMP::Quat toTarget;
    toTarget.forRotation(currentDir, targetDir);
    // To scale the rotation requests, we take the average of the rotationScales of the chest and the pelvis.
    float avg = in->getReachParams().pelvisRotationScale + in->getReachParams().chestRotationScale / 2.0f;
    RotationRequest rootRotRequest(toTarget.toRotationVector() * avg, SCALING_SOURCE);
    rootRotRequest.imminence = in->getReachParams().reachImminence;
    rootRotRequest.passOnAmount = 1.0f;
#if defined(MR_OUTPUT_DEBUGGING)
    rootRotRequest.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
    feedOut->setRootRotationRequest(rootRotRequest, in->getLimbControlImportance());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForWorld::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

