// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMIK/NMPointIK.h"
//----------------------------------------------------------------------------------------------------------------------
// Tuning parameters

// Tweak this to improve results with limits (at the cost of CPU performance)
#define MAX_ITERATIONS 1

#define TESTING_ONLY_DO_OUTWARD_LOOPx
#define TESTING_RESTRICT_TO_JOINT -1
#define TESTING_IGNORE_OTHER_JOINTS

//----------------------------------------------------------------------------------------------------------------------
// Macros

#define INWARD_LOOP true
#define OUTWARD_LOOP false

// Very approximate arccos function, for determining the amount of rotation the algorithm applies.
#define VERY_APPROX_ACOSF(x) NMP::fastSqrt(2.0f * NMP::maximum((1.0f - (x)), 0.0f))

// For debugging
#define ISFINITE(x) ((x) * 0 == 0)
#define ISFINITE_VEC(q) (ISFINITE((q).x) && ISFINITE((q).y) && ISFINITE((q).z) && ISFINITE((q).w))

//----------------------------------------------------------------------------------------------------------------------
namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
void PointIK::solve(
  const PointIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t numJoints,
  const GeomUtils::PosQuat& target,
  const GeomUtils::PosQuat* globalTransform /* = 0 */,
  GeomUtils::DebugDraw* debugBuffer /* = 0 */)
{
  NMP_ASSERT(params);
  NMP_ASSERT(joints);
  NMP_ASSERT(jointIndices);

  // We need to keep track of the end effector in the current joint frame at each stage.  We need to
  // FK out to the end effector to find it.
  GeomUtils::PosQuat localEndEffector;
  GeomUtils::identityPosQuat(localEndEffector);
  // Start at 1 because the current joint is the root, so we don't include its transform.
  for (uint32_t j = 1; j < numJoints; ++j)
  {
    const uint32_t& ji = jointIndices[j];

    GeomUtils::premultiplyPosQuat(joints[ji], localEndEffector);
  }

  // Calculate our tilt reference vector in the frame of the end effector.  This can then adapt
  // to the input animation.
  NMP::Quat worldEndEffectorQ = joints[jointIndices[0]].q * localEndEffector.q;
  if (globalTransform)
  {
    worldEndEffectorQ = globalTransform->q * worldEndEffectorQ;
  }
  NMP::Vector3 worldUpAtEndEffector = worldEndEffectorQ.inverseRotateVector(params->worldUpAxis);
  NMP::Vector3 tiltReferenceVector;
  tiltReferenceVector.cross(worldUpAtEndEffector, params->pointingVector);

  //----------------------------------------------------------------------------------------------------------------------
  // ITERATION LOOP
  // Inside this loop we go from root to end effector, and back down once.
  float weightRemaining = 1.0f;
  float additionalWeight = 0.0f;
  for (uint32_t i = 0; i < MAX_ITERATIONS; ++i)
  {

    // We need to keep track of the 'globalJointTM', the fixed frame at each joint relative to the root.
    // OPTIMISE  If we are doing more than one iteration there is no need to recalculate this.
    GeomUtils::PosQuat globalJointTM;
    if (globalTransform)
    {
      globalJointTM = *globalTransform;
      globalJointTM.t += globalJointTM.q.rotateVector(joints[jointIndices[0]].t);
    }
    else
    {
      globalJointTM.q.identity();
      globalJointTM.t = joints[jointIndices[0]].t;
    }

    //----------------------------------------------------------------------------------------------------------------------
    // Outward loop.  Traverse from root to end effector.
    bool solved = false;
    for (uint32_t j = 0; j < numJoints; ++j)
    {
      solved = solveJoint(
        params,
        joints,
        jointIndices,
        j,
        OUTWARD_LOOP,
        weightRemaining,
        additionalWeight,
        numJoints,
        localEndEffector,
        target,
        tiltReferenceVector,
        globalJointTM,
        debugBuffer);

      // Normally the solve will complete after one traverse because the solve is analytical, but
      // joint limits can prevent that.
      if (solved)
      {
        return;
      }

    } // end of outward loop

#ifndef TESTING_ONLY_DO_OUTWARD_LOOP

    // The local end effector TM should now be the identity, so make sure of this.
    GeomUtils::identityPosQuat(localEndEffector);

    //----------------------------------------------------------------------------------------------------------------------
    // Inward loop.  Traverse from end effector to root.
    for (int32_t j = numJoints - 1; j >= 0; --j)
    {
      solved = solveJoint(
        params,
        joints,
        jointIndices,
        j,
        INWARD_LOOP,
        weightRemaining,
        additionalWeight,
        numJoints,
        localEndEffector,
        target,
        tiltReferenceVector,
        globalJointTM,
        debugBuffer);

      // Exit early on the return loop
      if (solved)
      {
        return;
      }

    } // end of inward loop

#endif // TESTING_ONLY_DO_OUTWARD_LOOP

  } // end of iteration loop

} // end of PointIK::solve()

//----------------------------------------------------------------------------------------------------------------------
bool PointIK::solveJoint(
  const PointIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t jointPosition,
  bool isInwardLoop,
  float& weightRemaining,
  float& additionalWeight,
  uint32_t numJoints,
  GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& target,
  const NMP::Vector3& tiltReferenceVector,
  GeomUtils::PosQuat& globalJointTM,
  GeomUtils::DebugDraw* debugBuffer /* = 0 */)
{
  // Some useful data
  uint32_t& j = jointPosition;
  const uint32_t& ji = jointIndices[j];
  GeomUtils::PosQuat& joint = joints[ji];
  const IKJointParams& jointParams = params->perJointParams[ji];

  // Determine the bias weight for this joint
  float jointWeight = jointParams.weight + additionalWeight;
  float jointBiasWeight = NMP::minimum(jointWeight / weightRemaining, 1.0f);
  float weightUsed = 0.0f;
#if TESTING_RESTRICT_TO_JOINT >= 0
  if (jointPosition == TESTING_RESTRICT_TO_JOINT)
  {
#ifndef TESTING_IGNORE_OTHER_JOINTS
    jointBiasWeight = 1.0f;
#endif
  }
  else
  {
    jointBiasWeight = 0.0f;
  }
#endif

  //----------------------------------------------------------------------------------------------------------------------
  // MAIN SOLVE CLAUSE
  if (jointParams.enabled)
  {

    //----------------------------------------------------------------------------------------------------------------------
    // SOLVE

    // Obtain the target in the fixed frame
    GeomUtils::PosQuat targetFF = target;
    GeomUtils::multiplyInversePosQuat(targetFF, globalJointTM);

    // Package arguments into a more concise structure.
    IKSolverData solverData = {
      localEndEffector,
      targetFF,
      globalJointTM,
      joint.q,
      0, // No tertiary constraints
      jointPosition,
      jointIndices,
      numJoints,
      debugBuffer
    };

    weightUsed = solveAimJoint(
      params,
      joints,
      solverData,
      jointBiasWeight,
      &tiltReferenceVector);
    weightUsed *= weightRemaining;

  }

  // Update the additional weight to be passed on to other joints, that has been lost
  // to joint limit clamping (or disabled joints).
  float moreAdditionalWeight = NMP::clampValue(jointWeight - weightUsed, 0.0f, 1.0f);
  // Divide amongst the other joints (all joints minus this one, which is apparently stuck)
  moreAdditionalWeight = NMRU::GeomUtils::safelyDivide(moreAdditionalWeight, (float)(numJoints - 1), 0.0f);
  // Add the the existing additional weight
  additionalWeight += moreAdditionalWeight;

  // Update the amount of overall weight left
  weightRemaining -= weightUsed;
  weightRemaining = NMP::maximum(weightRemaining, 0.0f);

  // If we've used up all the weight, we can exit now
  if (weightRemaining < xAccuracy)
  {
    return true;
  }

  //----------------------------------------------------------------------------------------------------------------------
  // SHIFT

  // Shift frame up to parent joint or down to child joint, depending whether this is inward
  // or outward loop.
  if (isInwardLoop)
  {
    // Shift frame up to parent joint
    if (j > 0)
    {
      const uint32_t& jiNext = jointIndices[j-1];
      GeomUtils::multiplyPosQuat(localEndEffector, joint);
      globalJointTM.t -= globalJointTM.q.rotateVector(joint.t);
      globalJointTM.q *= ~joints[jiNext].q;
    }
  }
  else
  {
    // Shift frame down to child joint
    if (j < numJoints - 1)
    {
      const uint32_t& jiNext = jointIndices[j+1];
      GeomUtils::multiplyInversePosQuat(localEndEffector, joints[jiNext]);
      globalJointTM.q *= joint.q;
      globalJointTM.t += globalJointTM.q.rotateVector(joints[jiNext].t);
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
float PointIK::solveAimJoint(
  const PointIK::Params* params,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  float biasWeight,
  const NMP::Vector3* tiltReferenceVector /* = 0 */)
{
  // Useful data
  uint32_t j = solverData.jointPosition;
  uint32_t ji = solverData.jointIndices[j];
  const IKJointParams& jointParams = params->perJointParams[ji];
  GeomUtils::PosQuat& joint = joints[ji];
  NMP::Quat oldQ = joint.q;
  const GeomUtils::PosQuat& localEndEffector = solverData.localEndEffector;
  const GeomUtils::PosQuat& localTarget = solverData.localTarget;

  // Exit if this joint has zero weight
  if (biasWeight == 0 || jointParams.weight == 0 || !jointParams.enabled)
  {
    return 0;
  }

  // Initialise the weight we ended up actually using after applying joint limits
  float effectiveWeight = biasWeight;

  // If the target and end effector are coincident, we cannot move any more joints
  // because there is no correct solution
  if ((joint.q.rotateVector(localEndEffector.t) - localTarget.t).magnitudeSquared() > GeomUtils::nTolSq)
  {
    // Distance and unit vector to target
    NMP::Vector3 tV = joint.q.inverseRotateVector(localTarget.t);
    float t2 = tV.magnitudeSquared();

    // Pointing vector in frame of current joint
    NMP::Vector3 pV;
    localEndEffector.q.rotateVector(params->pointingVector, pV);

    // The end effector vector
    const NMP::Vector3& eV = localEndEffector.t;
    float e2 = eV.magnitudeSquared();

    //-----------------------------------
    // SWING and TWIST

    // What is the minimum value of t that I can actually point to?  It's when pV is
    // perpendicular to tV, unless the angle of eV to pV is acute, in which case it's e.
    float eVdotpV = eV.dot(pV);
    float subtractor = NMP::floatSelect(eVdotpV, 0.0f, eVdotpV * eVdotpV);
    float minT2 = e2 - subtractor;
    float clampedT2 = NMP::maximum(t2, minT2);

    // How far, lambda, do I need to move along pointing vector pV 'til I'm distance t from the joint?
    float A = 1.0f;
    float B = 2.0f * eVdotpV;
    float C = e2 - clampedT2;
    float lambda1, lambda2;
    GeomUtils::solveQuadratic(A, B, C, lambda1, lambda2);

    // Reject a solution if lambda is negative, which is invalid (we must point towards the target,
    // not away from it)
    lambda1 = NMP::floatSelect(lambda1, lambda1, lambda2);
    lambda2 = NMP::floatSelect(lambda2, lambda2, lambda1);

    // Get two possible solutions for the swing reference tprimeV
    NMP::Vector3 tprimeV1 = eV + lambda1 * pV;
    NMP::Vector3 tprimeV2 = eV + lambda2 * pV;

    // Choose the swing reference closest to the current target direction
    float amountOfMotion1 = tV.dot(tprimeV1);
    float amountOfMotion2 = tV.dot(tprimeV2);
    float selector = amountOfMotion1 - amountOfMotion2;
    NMP::Vector3 tprimeV;
    tprimeV.x = NMP::floatSelect(selector, tprimeV1.x, tprimeV2.x);
    tprimeV.y = NMP::floatSelect(selector, tprimeV1.y, tprimeV2.y);
    tprimeV.z = NMP::floatSelect(selector, tprimeV1.z, tprimeV2.z);

    // Get the swing-twist rotation.  forRotation() correctly handles a zero-length vector to target.
    NMP::Quat resultQ;
    resultQ.forRotation(tprimeV, tV);

    //----------------------------------------------------------------------------------------------------------------------
    // BIAS and WEIGHT
    float weightBackToZero = 1.0f - effectiveWeight;
    GeomUtils::shortestArcFastSlerp(resultQ, NMP::QuatIdentity(), weightBackToZero);

    //-----------------------------------
    // KEEP UPRIGHT and MINIMISE TRANSLATION

    // These special formulations require a non-zero vector to the target
    if (t2 > GeomUtils::nTolSq)
    {
      // Recalculate the joint rotation enforcing yaw and tilt in order to eliminate roll
      // around the aiming direction.
      if (params->keepUpright && tiltReferenceVector)
      {
        // Up axis reference in original joint frame
        NMP::Vector3 localWorldUp =
          (solverData.globalJointTM.q * oldQ).inverseRotateVector(params->worldUpAxis);

        // Tilt vector in frame of current joint
        NMP::Vector3 localTiltReference;
        localEndEffector.q.rotateVector(*tiltReferenceVector, localTiltReference);

        // Where do I want my tilt vector to point now?  Perpendicular to the world up.
        // Get this by removing the component in the up direction.
        NMP::Vector3 newTiltReference = localTiltReference;
        newTiltReference -= localWorldUp * (newTiltReference.dot(localWorldUp));

        // Correct aberration from valid lean angle by rolling upright.  This may be caused by joint
        // limits or other constraints.
        // This correctly handles poor conditioning in which the newTiltReference is of zero length.
        NMP::Quat rollQ;
        rollQ.forRotation(localTiltReference, newTiltReference);

        // Correct direction to target for roll
        tV = rollQ.inverseRotateVector(tV);

        // Yaw around up direction to obtain target
        float condition;
        //    Normalise the vectors otherwise the condition value is incorrect
        float t = NMP::fastSqrt(t2);
        tprimeV /= t;
        tV /= t;
        NMP::Quat yawQ = NMRU::GeomUtils::forRotationAroundAxis(tprimeV, tV, localWorldUp, &condition);

        // Tilt onto target
        NMP::Quat tiltQ;
        tiltQ.forRotation(tprimeV, yawQ.inverseRotateVector(tV));

        // Conflate roll and yaw - they can be weighted together because roll should be minimal
        NMP::Quat rollYawQ = rollQ * yawQ;

        // Apply the bias and weight, which must be done independently for the two components to
        // retain the keep upright constraint.
        GeomUtils::shortestArcFastSlerp(rollYawQ, NMP::QuatIdentity(), weightBackToZero);
        GeomUtils::shortestArcFastSlerp(tiltQ, NMP::QuatIdentity(), weightBackToZero);

        // Calculate a weight between the original solve and the keep upright solve which will
        // smoothly switch off the keep upright version near singularities (aiming directly up or down).
        float rescaledCondition = (condition - xKeepUprightConditionRangeMin) *
          (1.0f / (xKeepUprightConditionRangeMax - xKeepUprightConditionRangeMin));
        float blendWeight = NMP::clampValue(rescaledCondition, 0.0f, 1.0f);

        // Apply this weight
        NMRU::GeomUtils::shortestArcFastSlerp(resultQ, rollYawQ * tiltQ, blendWeight);
      }

      // Use redundant motion around the target direction to minimise the overall translation
      // of the end effector.
      else if (!params->minimiseRotation)
      {
        NMP::Quat qSwivel = NMRU::GeomUtils::forRotationAroundAxis(resultQ.rotateVector(eV), eV, tV);

        // Still have to weight the swivel component correctly.
        GeomUtils::shortestArcFastSlerp(resultQ, qSwivel * resultQ, effectiveWeight);
      }
    }

    // Copy back result
    joint.q = oldQ * resultQ;
    joint.q.fastNormalise();

#ifndef NMIK_NO_TERTIARIES
    //----------------------------------------------------------------------------------------------------------------------
    // TERTIARY CONSTRAINTS
    float unusedWeightOut = 0;
    solveJointTertiaries(unusedWeightOut, &jointParams, joints, solverData);
#endif

    //----------------------------------------------------------------------------------------------------------------------
    // CLAMP
    NMP::Quat desiredQ = joint.q;

    // Clamp this motion to be around the hinge axis, if this is a hinge
    if (jointParams.isHinge)
    {
      NMP::Quat qOffset = ~oldQ * joint.q;  // Effective motion of moving frame
      NMP::Quat qRot = GeomUtils::closestRotationAroundAxisSoft(qOffset, jointParams.hingeAxis, xHingeSoftness);
      joint.q = oldQ * qRot;
    }

    if (isViolated(jointParams, joint.q))
    {
      JointLimits::Params tightenedLimits(jointParams.limits, xClampTighten);
  #ifdef DIRECT_CLAMP
    #ifdef SMOOTH_LIMITS
      JointLimits::clamp(tightenedLimits, joint.q, xLimitSmoothness);
    #else
      JointLimits::clamp(tightenedLimits, joint.q);
    #endif
  #else
    #ifdef SMOOTH_LIMITS
      JointLimits::clampBetween(tightenedLimits, oldQ, joint.q, xLimitSmoothness);
    #else
      JointLimits::clampBetween(tightenedLimits, oldQ, joint.q, xLimitSmoothness);
    #endif
  #endif
    }

    // Get effective amount of rotation compared to desired, after clamping
    float remainingHalfAngle = VERY_APPROX_ACOSF(NMP::nmfabs(desiredQ.dot(joint.q)));
    float desiredHalfAngle = VERY_APPROX_ACOSF(NMP::nmfabs(oldQ.dot(desiredQ)));
    effectiveWeight *= NMP::maximum((1.0f - GeomUtils::safelyDivide(remainingHalfAngle, desiredHalfAngle, 0.0f)), 0.0f);

  } // end of coincidence condition

  //----------------------------------------------------------------------------------------------------------------------
  // DEBUG DRAW
  if (solverData.debugBuffer)
  {
    GeomUtils::PosQuat eeTM = localEndEffector;
    GeomUtils::rotatePosQuat(joint.q, eeTM);
    GeomUtils::multiplyPosQuat(eeTM, solverData.globalJointTM);
    GeomUtils::PosQuat targetTM = localTarget;
    GeomUtils::multiplyPosQuat(targetTM, solverData.globalJointTM);
    NMP::Vector3 pointer = eeTM.q.rotateVector(params->pointingVector);
    float distanceToTarget = eeTM.t.distanceTo(targetTM.t);
    solverData.debugBuffer->drawDashedLine(eeTM.t, eeTM.t + (pointer * distanceToTarget), 0.025f);
    solverData.debugBuffer->drawLocator(targetTM.t, 0.05f);
  }

  return effectiveWeight;

} // end of PointIK::solveAimJoint()

} // end of namespace NMRU

//----------------------------------------------------------------------------------------------------------------------
