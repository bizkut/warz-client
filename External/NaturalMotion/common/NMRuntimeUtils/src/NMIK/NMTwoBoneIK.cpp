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
#include "NMIK/NMTwoBoneIK.h"
//----------------------------------------------------------------------------------------------------------------------

#define NMRU_TWOBONEIK_DEBUGDRAW
#ifdef NMRU_TWOBONEIK_DEBUGDRAW
  #define TWOBONEIK_DEBUGDRAW_PARAM(x) x
#else
  #define TWOBONEIK_DEBUGDRAW_PARAM(x) {}
#endif

#ifdef NMRU_TWOBONEIK_DEBUGDRAW
  #define TARGET_CROSSHAIR_SIZE 0.04f
  #define DEBUG_DRAW(N) \
if (debugBuffer) \
{ \
  GeomUtils::PosQuat prevT = GeomUtils::identityPosQuat(); \
  for (uint32_t jdd = 0; jdd < N; ++jdd) \
  { \
    GeomUtils::PosQuat nextT = prevT; \
    GeomUtils::premultiplyPosQuat(joints[jdd], nextT); \
    debugBuffer->drawLine(prevT.t, nextT.t); \
    prevT = nextT; \
  } \
  debugBuffer->drawLine( \
    target.t-NMP::Vector3(TARGET_CROSSHAIR_SIZE, 0, 0), target.t+NMP::Vector3(TARGET_CROSSHAIR_SIZE, 0, 0)); \
  debugBuffer->drawLine( \
    target.t-NMP::Vector3(0, TARGET_CROSSHAIR_SIZE, 0), target.t+NMP::Vector3(0, TARGET_CROSSHAIR_SIZE, 0)); \
  debugBuffer->drawLine( \
    target.t-NMP::Vector3(0, 0, TARGET_CROSSHAIR_SIZE), target.t+NMP::Vector3(0, 0, TARGET_CROSSHAIR_SIZE)); \
}
#else
  #define DEBUG_DRAW(N) {}
#endif

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
void TwoBoneIK::solve(
  const TwoBoneIK::Params* params,
  GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  GeomUtils::DebugDraw* TWOBONEIK_DEBUGDRAW_PARAM(debugBuffer) /* = 0 */)
{
  // The vector in the root joint frame, from root to target
  NMP::Vector3 rtV = target.t - joints[0].t;
  float rtLength = rtV.magnitude();

  // The original orientations of the root and middle joints.
  const NMP::Quat& rootStart = joints[0].q;
  const NMP::Quat& midStart = joints[1].q;

  // The original orientation of the middle joint in the root frame.
  NMP::Quat midQuat = rootStart * midStart;

  // The translation of the middle joint from the root frame
  NMP::Vector3 mV;
  rootStart.rotateVector(joints[1].t, mV);

  // The rotation axis of the mid joint, transformed to root space.  Reverse if required.
  NMP::Vector3 axisInRootFrame;
  rootStart.rotateVector(params->hingeAxis, axisInRootFrame);

  // The vector going from middle to end joint when the system is in its start configuration,
  // transformed into root space.
  NMP::Vector3 eEffV;
  midQuat.rotateVector(joints[2].t, eEffV);

  // Get the two possible rotation angles for the mid joint
  float tanHalfAngle1, tanHalfAngle2;
  bool validHingeSolve = calculateHingeRotation(
    axisInRootFrame, eEffV, mV, rtLength, params->maximumReachFraction,
    tanHalfAngle1, tanHalfAngle2, params->correctHingeDrift ? &params->hingeLocusOffset : 0);

  // Choose the right angle and adjust for backwards-bending requirements and conditioning
  if (validHingeSolve)
  {

    // The solution is usually the one for the smallest rotation
    float test = NMP::nmfabs(tanHalfAngle1) - NMP::nmfabs(tanHalfAngle2);
    // This says that theta is the smaller angle
    float tanHalfTheta = NMP::floatSelect(test, tanHalfAngle2, tanHalfAngle1);
    float tanHalfPhi = NMP::floatSelect(test, tanHalfAngle1, tanHalfAngle2);

    // Find the quaternion that represents this rotation
    NMP::Quat midRootRotation(tanHalfTheta * axisInRootFrame);

    // Bending the leg backwards may be a smaller motion.  If we wish to prevent this
    // we must measure the angle between the bones and choose the other solution if
    // we are violating our backwards limit.
    if (params->preventBendingBackwards)
    {
      // If the zero angle is zero then all we need to do is test that the rotation of the
      // upper bone vector towards the lower is clockwise around the hinge axis - this will
      // put us in the correct semicircle of motion.
      if (params->zeroHingeTanHalfAngle == 0)
      {
        NMP::Vector3 newEV = midRootRotation.rotateVector(eEffV);
        NMP::Vector3 dirnV;
        dirnV.cross(mV, newEV);
        float dirn = axisInRootFrame.dot(dirnV);
        if (dirn < 0)
        {
          // Theta was bending us backwards, so choose phi instead
          midRootRotation.fromTanHalfAngleRotationVector(tanHalfPhi * axisInRootFrame);
        }
      }

      // If the zero angle is non-zero, we actually need to measure the angle of rotation
      else
      {
        NMP::Vector3 v1 = mV - axisInRootFrame * mV.dot(axisInRootFrame);
        NMP::Vector3 v2 = midRootRotation.rotateVector(eEffV);
        v2 -= axisInRootFrame * v2.dot(axisInRootFrame);
        // If either bone is too short, or too close to parallel to the hinge axis, we
        // cannot evaluate backward-bending, and so must not rotate the hinge joint just
        // in case.
        if (v1.magnitudeSquared() < GeomUtils::nTolSq || v2.magnitudeSquared() < GeomUtils::nTolSq)
        {
          midRootRotation.identity();
        }
        else
        {
          v1.fastNormalise();
          v2.fastNormalise();
          NMP::Vector3 dirnV;
          dirnV.cross(v1, v2);
          float dirn = axisInRootFrame.dot(dirnV);
          // By clamping to -0.9999f we avoid a divide-by-zero without a branch.  All this does
          // is mean we can't have a ridiculously high zero hinge angle, i.e. one which basically
          // forces the limb to be fully collapsed all the time.  That's okay.
          float cosHingeAngle = NMP::clampValue(v1.dot(v2), -0.9999f, 1.0f);
          float tanHalfHingeAngle = NMP::fastSqrt((1.0f - cosHingeAngle) / (1.0f + cosHingeAngle));
          tanHalfHingeAngle = NMP::floatSelect(dirn, tanHalfHingeAngle, -tanHalfHingeAngle);

          // If the zero angle is positive there are some angles which cannot be reached.  Detect
          // this and clamp to the furthest reach in this case.
          if (NMP::nmfabs(tanHalfHingeAngle) < params->zeroHingeTanHalfAngle)
          {
            // OPTIMISE  You could store the limit orientation up front.
            NMP::Quat rotateToStraight = NMRU::GeomUtils::forRotationAroundAxis(eEffV, mV, axisInRootFrame);
            NMP::Quat rotateToLimit(params->zeroHingeTanHalfAngle * axisInRootFrame);
            midRootRotation = rotateToLimit * rotateToStraight;
          }

          // Detect rotation beyond the backwards bending limit (zeroHingeAngle) and try the other
          // solution if need be.
          // To ensure consistency, we prevent switching between solutions if they are very
          // close - this avoids possible wobbling.
          else if (tanHalfHingeAngle < params->zeroHingeTanHalfAngle &&
                   NMP::nmfabs(tanHalfAngle1 - tanHalfAngle2) > 1e-4f)
          {
            midRootRotation.fromTanHalfAngleRotationVector(tanHalfPhi * axisInRootFrame);
          }
        }
      }
    }

    // If required, scale the rotation by the conditioning of the hinge axis
    if (params->hingeConditionLowerBound > GeomUtils::nTol)
    {
      NMP::Vector3 bone1Vec = mV;
      NMP::Vector3 bone2Vec = eEffV;
      bone1Vec.fastNormalise();
      bone2Vec.fastNormalise();
      float conditionNumber = NMP::nmfabs(1.0f - NMP::nmfabs(bone1Vec.dot(axisInRootFrame)));
      conditionNumber *= NMP::nmfabs(1.0f - NMP::nmfabs(bone2Vec.dot(axisInRootFrame)));
      conditionNumber = NMP::clampValue(conditionNumber / params->hingeConditionLowerBound, 0.0f, 1.0f);

      // Reduce rotation by slerping back to the identity
      NMP::Quat identityQuat(0, 0, 0, NMP::floatSelect(midRootRotation.w, 1.0f, -1.0f));
      midRootRotation.fastSlerp(identityQuat, 1.0f - conditionNumber, NMP::nmfabs(midRootRotation.w));
    }

    // *************************************************************************************

    // Get new vector to the end joint
    eEffV = midRootRotation.rotateVector(eEffV);

    // Shift this into the middle joint's frame and compound with the initial rotation of
    // the middle joint to give our resulting IKed quaternion.
    NMP::Quat midIKQuat = midStart * (~midQuat * midRootRotation * midQuat);
    midIKQuat.fastNormalise();

    // Modify the mid joint orientation in the input joint chain
    joints[1].q = midIKQuat;
  }

  // *************************************************************************************
  // Root joint orientation calculation
  // ----------------------------------

  // Get the quaternion representing the rotation at the root joint that will move the end
  // effector onto the target.
  NMP::Quat rootRotation;
  rootRotation.forRotation(mV + eEffV, rtV);

  // Get the resulting overall root rotation
  NMP::Quat rootIKQuat = rootRotation * rootStart;

  // Add to this a swivel around the axis that points towards the target.  For an arm this
  // is the 'elbow angle'.
  // Get the swivel rotation due to aligning of the control pole vector (if active)
  NMP::Vector3 zeroSwivelRotationVector(0, 0, 0);
  NMP::Vector3& swivelAxis = rtV;
  swivelAxis.fastNormalise();
  if (params->midJointRedundancyParams.active)
  {
    // The reference pole vector of the mid joint, which should be in the fixed root frame.
    const NMP::Vector3& poleReferenceVector = params->midJointRedundancyParams.referencePoleVector;
    // The control pole vector of the mid joint.  As input it is expressed in the fixed mid
    // joint frame (equivalently, the moving root joint frame), so is moved to the fixed
    // root frame.
    NMP::Vector3 poleControlVector = (rootRotation * rootStart).rotateVector(params->midJointRedundancyParams.controlPoleVector);
    // Get swivel rotation that brings this axis as close as possible to the reference axis
    NMP::Quat zeroSwivelQuat = GeomUtils::forRotationAroundAxis(poleControlVector, poleReferenceVector, swivelAxis);
    zeroSwivelQuat.multiply(rootIKQuat);
    // Add to root rotation and weight appropriately
    GeomUtils::shortestArcFastSlerp(rootIKQuat, zeroSwivelQuat, params->midJointRedundancyParams.weight);
  }
  // From here, apply the appropriate offset as specified by the swivel parameter
  rootIKQuat = NMP::Quat(swivelAxis, params->swivel) * rootIKQuat;
  // Get the swivel for adjusting the orientation of the end effector
  if (params->endEffectorPoleVectorParams.active || params->swivelOrientationWeight != 0)
  {
    NMP::Quat midAndEndRotation = joints[1].q * joints[2].q;
    // Get the swivel to adjust for end effector pole vector control
    if (params->endEffectorPoleVectorParams.active)
    {
      NMP::Quat endOrientation = rootIKQuat * midAndEndRotation;
      NMP::Vector3 poleControlVector = endOrientation.rotateVector(params->endEffectorPoleVectorParams.controlPoleVector);
      // For the end effector, the reference vector is in the frame of the target
      NMP::Vector3 poleReferenceVector = target.q.rotateVector(params->endEffectorPoleVectorParams.referencePoleVector);
      NMP::Quat endOrientate = GeomUtils::forRotationAroundAxis(poleControlVector, poleReferenceVector, swivelAxis);
      // Add to root rotation and weight appropriately as a motion from the previous position
      endOrientate.multiply(rootIKQuat);
      GeomUtils::shortestArcFastSlerp(rootIKQuat, endOrientate, params->endEffectorPoleVectorParams.weight);
    }

    // Get the swivel to approximate a desired orientation
    if (params->swivelOrientationWeight != 0)
    {
      NMP::Quat endOrientation = rootIKQuat * midAndEndRotation;
      // Rotation between the two in root space
      NMP::Quat endOrientate = GeomUtils::closestRotationAroundAxis(target.q * ~endOrientation, swivelAxis);
      // Add to root rotation and weight appropriately as a motion from the previous position
      endOrientate.multiply(rootIKQuat);
      GeomUtils::shortestArcFastSlerp(rootIKQuat, endOrientate, params->swivelOrientationWeight);
    }
  }
  rootIKQuat.fastNormalise();

  // Modify the root joint orientation in the input joint chain
  joints[0].q = rootIKQuat;

  //----------------------------------------------------------------------------------------------------------------------
  DEBUG_DRAW(3);

} // end of TwoBoneIK::solve()

//----------------------------------------------------------------------------------------------------------------------
void TwoBoneIK::solveWithEndJoint(
  const TwoBoneIK::Params* params,
  GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  GeomUtils::DebugDraw* TWOBONEIK_DEBUGDRAW_PARAM(debugBuffer) /* = 0 */)
{
  // Calculate the desired position of the end joint were the end effector in the target pose
  GeomUtils::PosQuat adjustedTarget = target;
  GeomUtils::premultiplyInversePosQuat(joints[3], adjustedTarget);

  // Solve for the end joint position
  TwoBoneIK::solve(params, joints, adjustedTarget, 0);

  //----------------------------------------------------------------------------------------------------------------------
  // Now rotate the end joint to complete the orientation solve
  NMP::Quat endJointFixedFrameQ = joints[0].q * joints[1].q;
  NMP::Quat targetOrientationQ = ~endJointFixedFrameQ * target.q;
  NMP::Quat eeQ = joints[2].q * joints[3].q;

  // End effector pole vector control
  if (params->endEffectorPoleVectorParams.active)
  {
    // Control and reference vectors are in the frames of the end effector and target, respectively.
    // Move them into the frame of the end joint's fixed frame.
    NMP::Vector3 controlVector = eeQ.rotateVector(params->endEffectorPoleVectorParams.controlPoleVector);
    NMP::Vector3 referenceVector = targetOrientationQ.rotateVector(params->endEffectorPoleVectorParams.referencePoleVector);
    NMP::Quat endOrientate;
    endOrientate.forRotation(controlVector, referenceVector);
    endOrientate.multiply(joints[2].q);
    GeomUtils::shortestArcFastSlerp(joints[2].q, endOrientate, params->endEffectorPoleVectorParams.weight);
  }

  // Full frame orientation control
  // If there is a translational offset between the end joint and end effector, then the position
  // goal will be compromised by having less than 100% orientation weight.
  if (params->endJointOrientationWeight != 0)
  {
    GeomUtils::shortestArcFastSlerp(joints[2].q, targetOrientationQ * ~joints[3].q, params->endJointOrientationWeight);
  }

  //----------------------------------------------------------------------------------------------------------------------
  DEBUG_DRAW(4);

} // end of TwoBoneIK::solveWithEndJoint()

//----------------------------------------------------------------------------------------------------------------------
void TwoBoneIK::solveWithFixedEndJoint(
  const TwoBoneIK::Params* params,
  GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  GeomUtils::DebugDraw* TWOBONEIK_DEBUGDRAW_PARAM(debugBuffer) /* = 0 */)
{
  // A copy of the parameters - we need to modify them
  TwoBoneIK::Params localParams = *params;

  // Conflate the end joint and end effector geometry to create a 'virtual' second bone from mid
  // joint to end effector, so we can target that.
  GeomUtils::PosQuat virtualBone = joints[2];
  GeomUtils::premultiplyPosQuat(joints[3], virtualBone);

  // If we are preventing backwards-bending, and our virtual bone is not in the same direction as
  // our real bone, then the backwards-bending limit, or zero-angle, will be different.  So
  // we need to calculate what it is.
  // Also sanity check that our bones are not zero-length, if so then 'bending backwards' is
  // meaningless.
  if (params->preventBendingBackwards && joints[3].t.magnitudeSquared() > GeomUtils::nTolSq &&
      joints[1].t.magnitudeSquared() > GeomUtils::nTolSq && joints[2].t.magnitudeSquared() > GeomUtils::nTolSq &&
      virtualBone.t.magnitudeSquared() > GeomUtils::nTolSq)
  {
    // Find the backwards hinge limit
    const NMP::Vector3& axis = params->hingeAxis;
    // What is the hinge rotation when the neighbouring true bones are straight?
    NMP::Vector3 v1 = joints[1].q.rotateVector(joints[2].t);
    NMP::Vector3 v2 = joints[1].t;
    v1.fastNormalise();
    v2.fastNormalise();
    NMP::Quat qh = GeomUtils::forRotationAroundAxis(v1, v2, axis);
    // What is the hinge rotation when the neighbouring true bones are at their backwards limit?
    NMP::Quat ql(params->zeroHingeTanHalfAngle * axis);
    ql = ql * qh;
    // What is the hinge angle of the virtual system at this orientation?
    v1 = (ql * joints[1].q).rotateVector(virtualBone.t);
    v1.fastNormalise();
    //  Clamping to -0.9999f avoids a divide-by-zero, and only prevents silly values of the zero angle.
    float cosHingeAngle = NMP::clampValue(v1.dot(v2), -0.9999f, 1.0f);
    // Convert to tan-half-angle and get the sign correct based on the axis direction.
    NMP::Vector3 dirnV;
    dirnV.cross(v2, v1);
    float dirn = dirnV.dot(axis);
    localParams.zeroHingeTanHalfAngle = NMP::fastSqrt((1.0f - cosHingeAngle) / (1.0f + cosHingeAngle));
    localParams.zeroHingeTanHalfAngle =
      NMP::floatSelect(dirn, localParams.zeroHingeTanHalfAngle, -localParams.zeroHingeTanHalfAngle);
  }
  else
  {
    localParams.preventBendingBackwards = false;
  }

  // Change the geometry
  joints[2] = virtualBone;

  // Solve for the 2-joint + end-effector system
  TwoBoneIK::solve(&localParams, joints, target, 0);

  // Recover the geometry
  NMRU::GeomUtils::premultiplyInversePosQuat(joints[3], joints[2]);

  //----------------------------------------------------------------------------------------------------------------------
  DEBUG_DRAW(4);

} // end of TwoBoneIK::solveWithEndJoint()

//----------------------------------------------------------------------------------------------------------------------
void TwoBoneIK::solveHinge(
  const NMP::Vector3& hingeAxis,
  const GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  float& angleOut1,
  float& angleOut2,
  float maximumReachFraction /* = xTwoBoneIKReachStabilityFactor */,
  GeomUtils::DebugDraw* TWOBONEIK_DEBUGDRAW_PARAM(debugBuffer) /* = 0 */)
{
  // The vector in the root joint frame moving frame, from root to target
  NMP::Vector3 rtV = joints[0].q.inverseRotateVector(target.t - joints[0].t);
  float rtLength = rtV.magnitude();

  // The translation of the middle joint from the root frame
  const NMP::Vector3& mV = joints[1].t;

  // The vector going from middle to end joint, transformed into root space.
  NMP::Vector3 eEffV;
  joints[1].q.rotateVector(joints[2].t, eEffV);

#ifdef NMRU_TWOBONEIK_DEBUGDRAW
  if (debugBuffer)
  {
    GeomUtils::PosQuat jointMovingFrameWorld;
    GeomUtils::multiplyPosQuat(jointMovingFrameWorld, joints[1], joints[0]);
    static const uint32_t N = 50;
    const float stepAngle = 2.0f * NM_PI / ((float)N);
    NMP::Vector3 endEffectorLocusPrev = jointMovingFrameWorld.t;
    NMP::Vector3 targetLocusPrev = joints[0].t;
    for (uint32_t i = 0; i <= N; ++i)
    {
      float angle = stepAngle * i;
      NMP::Quat q(hingeAxis, angle);

      NMP::Vector3 endEffectorLocusNext = jointMovingFrameWorld.q.rotateVector(q.rotateVector(joints[2].t)) + jointMovingFrameWorld.t;
      NMP::Vector3 targetLocusNext = jointMovingFrameWorld.q.rotateVector((q * ~joints[1].q).rotateVector(rtV)) + joints[0].t;
      if (i > 0)
      {
        debugBuffer->drawLine(endEffectorLocusPrev, endEffectorLocusNext);
        debugBuffer->drawLine(targetLocusPrev, targetLocusNext);
      }
      else
      {
        debugBuffer->drawDashedLine(endEffectorLocusPrev, endEffectorLocusNext, 0.01f);
        debugBuffer->drawDashedLine(targetLocusPrev, targetLocusNext, 0.01f);
      }
      endEffectorLocusPrev = endEffectorLocusNext;
      targetLocusPrev = targetLocusNext;
    }
  }
#endif

  // Get the two possible rotation angles for the mid joint
  float tanHalfAngle1, tanHalfAngle2;
  bool validHingeSolve = calculateHingeRotation(
    hingeAxis, eEffV, mV, rtLength, maximumReachFraction, tanHalfAngle1, tanHalfAngle2);

  // Convert to angle-space
  if (validHingeSolve)
  {
    angleOut1 = 2.0f * atanf(tanHalfAngle1);
    angleOut2 = 2.0f * atanf(tanHalfAngle2);
  }

} // end of TwoBoneIK::solveHinge()

//----------------------------------------------------------------------------------------------------------------------
bool TwoBoneIK::calculateHingeRotation(
  const NMP::Vector3& hingeAxis,
  const NMP::Vector3& lowerBoneVector,
  const NMP::Vector3& upperBoneVector,
  float targetDistance,
  float maximumReachFraction,
  float& tanHalfAngleOut1,
  float& tanHalfAngleOut2,
  const float* hingeLocusOffset /* = 0 */)
{
  float lowerBoneLength = lowerBoneVector.magnitude();

  // If the mid joint and end joint are coincident, there is no solution
  if (lowerBoneLength > 1e-4f)
  {
    float upperBoneLength = upperBoneVector.magnitude();

    // If the target is out of reach, make adjustments by shifting the target towards the
    // root along the same direction.
    float reach = upperBoneLength + lowerBoneLength;
    targetDistance = NMP::minimum(targetDistance, reach * maximumReachFraction);

    // PERFORM THE IK SOLVE

    // *************************************************************************************
    // Mid-joint orientation calculation
    // ---------------------------------
    //
    // Rather than use the cosine formula, we need to calculate the angle of rotation of
    // the middle joint which doesn't assume that the two bones lie in a plane perpendicular
    // to the hinge axis.
    //
    // Everything here is represented in the root frame.  Let * be the dot product and x the
    // cross product.  We have the vector to the middle joint, mV, and to the end joint, eEffV.
    // We also define the length of eEffV = e, and the normalised vector r = eEffV/e.  From
    // the current position, we rotate the end joint around the hinge axis a at the middle
    // joint, which means it moves in a circle, with the bone locus forming a cone shape
    // centered at the middle joint.
    //
    // The centre of this circle is offset from the middle joint along the rotation axis by a
    // vector cV = (a*eEffV)a.  Then we can represent the plane of this circle by two orthogonal
    // direction vectors, y = a x r, and z = y x a.  The radius of circle l = eEffV * z.
    //
    // The current position of the end joint is then eV = (mV + cV) + lz = CV + lz.  After a
    // rotation around the hinge axis of theta, eV = CV + lcos(theta)z + lsin(theta)y.
    // Knowing this, I can apply my requirement that the distance from root to the end joint
    // be the same as that from the root to my target T.  So, I can write (note T^2 = T.T):
    //
    //     ||CV + lcos(theta)z + lsin(theta)y|| = T
    // =>  ||CV||^2 + 2l(cos(theta)CV*z + sin(theta)CV*y) + l^2 = T^2
    // =>  cos(theta)mV*z + sin(theta)mV*y = (T^2 - ||CV||^2 - l^2) / 2l
    //
    // Let a = mV*z, b = mV*y, d = (T^2 - ||CV||^2 - l^2) / 2l, giving
    //     acos(theta) + bsin(theta) = d
    // Substitute t = tan(theta/2), so cos(theta) = (1-t^2)/(1+t^2), sin(theta) = 2t/(1+t^2), so
    //     a(1 - t^2) + 2bt = d(1 + t^2)
    // =>  (a + d)t^2 - 2bt + (d - a) = 0
    //
    // Hence we have a quadratic At^2 + Bt + C = 0, where A = (a + d), B = -2b, C = (d - a).

    NMP::Vector3 cV = hingeAxis * (hingeLocusOffset ? *hingeLocusOffset : hingeAxis.dot(lowerBoneVector));
    NMP::Vector3 r = lowerBoneVector;  r.fastNormalise();
    NMP::Vector3 y; y.cross(hingeAxis, r); y.fastNormalise();
    NMP::Vector3 z; z.cross(y, hingeAxis);
    float l = lowerBoneVector.dot(z);
    NMP::Vector3 CV = upperBoneVector + cV;
    float C2 = CV.magnitudeSquared();
    float a = upperBoneVector.dot(z);
    float b = upperBoneVector.dot(y);
    float d = (targetDistance * targetDistance - C2 - l * l) / (2 * l);
    // This is a periodic function, d is positive, we cannot get closer than d^2 = a^2 + b^2.
    // So clamp d to this value.  This is equivalent to saying get the distance between root
    // and end effector as close as possible to the desired value.
    float maxDSquared = a * a + b * b;
    // OPTIMISE Trade a conditional branch for a square root by removing this condition.
    if (d * d > maxDSquared)
    {
      float maxD = NMP::fastSqrt(maxDSquared);
      d = NMP::clampValue(d, -maxD, maxD);
    }
    // Set up and solve the quadratic equation
    float A = a + d;
    float B = -2 * b;
    float C = d - a;
    tanHalfAngleOut1 = 0;
    tanHalfAngleOut2 = 0;
    NMRU::GeomUtils::solveQuadratic(A, B, C, tanHalfAngleOut1, tanHalfAngleOut2);
  }
  else
  {
    return false;
  }

  return true;
} // end of TwoBoneIK::calculateHingeRotation()

} // end of namespace NMRU

//----------------------------------------------------------------------------------------------------------------------
