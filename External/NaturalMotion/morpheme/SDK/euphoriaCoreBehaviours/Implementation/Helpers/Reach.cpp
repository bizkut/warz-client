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
#include "Helpers/Reach.h"
#include "morpheme/mrDebugMacros.h"

//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// function to determine if a point in space lies within a prism with triangular base
// Note that a few potential degenerate cases are not handled in this function - one 
// assert and some additional notes are left below for future ref.
//
bool pointInTrianglePrism(
  const NMP::Vector3& x,
  const NMP::Vector3& v0,
  const NMP::Vector3& v1,
  const NMP::Vector3& v2,
  uint32_t& outCode)
{
  // this cross product of two edges determines the triangle normal and "front" of the tri
  // we'll use this to locate the testpoint and say which side of the tri it's on
  // (note: det too small <==> degenerate triangle)
  //
  NMP::Vector3 det = vCross(v1 - v0, v2 - v0);
//  NMP_ASSERT(det.magnitudeSquared() > 0.000001f);// possible degenerate triangle
  if (det.magnitudeSquared() < 0.000001f)
  {
    return false; // inconclusive result
  }
  NMP::Vector3 n = vNormalise(det);

  // projection of x onto the triangle plane
  NMP::Vector3 xInPlane = x - n * (x - v0).dot(n);

  // the s's are lines from xInPlane to each vertex
  NMP::Vector3 s0 = v0 - xInPlane;
  NMP::Vector3 s1 = v1 - xInPlane;
  NMP::Vector3 s2 = v2 - xInPlane;

  // e's are the sin of the angle between the s's
  // (note: check if any of the e's is too small <==> x lies on an edge)
  //
  float e0 = NMP::vCross(s0, s1).dot(n);
  float e1 = NMP::vCross(s1, s2).dot(n);
  float e2 = NMP::vCross(s2, s0).dot(n);

  // e > 0 <==> xInPlane is on the interior of the edge
  // store the results of the e > 0 test in retCode by setting or not the
  // corresponding bit for each edge
  //
  outCode = 0;
  outCode |= (e0 < 0) ? 1 : 0;
  outCode |= (e1 < 0) ? 2 : 0;
  outCode |= (e2 < 0) ? 4 : 0;

  // x cannot lie outside all three edges!
  NMP_ASSERT(outCode != 7);

  return (x - v0).dot(n) >= 0.0f;
}

float calcReachAngleToTarget(
  const NMP::Vector3& target,           // target position
  const NMP::Vector3& effector,         // effector position
  float slackAngle,                     // region in which the effector position determines the winding angle
  // "reach frame" parameters define the frame of rotation (origin, up and forward)
  const NMP::Vector3& reachOrigin,         // origin (e.g. shoulder or chest)
  const NMP::Vector3& reachUp,          // up direction (eg. spine or chest up)
  const NMP::Vector3& reachFwd,         // forward (eg. reach cone direction)
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const ER::DimensionalScaling*   MR_OUTPUT_DEBUG_ARG(dimensionalScaling)
  )
{
  // Make a frame of reference centered on the reach origin, oriented to match the reach up and reach forward inputs.
  //
  NMP_ASSERT(reachUp.magnitude() > 0.01f);
  NMP::Vector3 up = NMP::vNormalise(reachUp);
  NMP::Matrix34 reachFrame;
  reachFrame.yAxis() = up;
  reachFrame.zAxis() = reachFwd.getComponentOrthogonalToDir(up);
  reachFrame.xAxis() = NMP::vCross(reachFrame.yAxis(), reachFrame.zAxis());
  reachFrame.translation() = reachOrigin;
  NMP::Vector3 tgtLocal, effLocal;
  reachFrame.inverseTransformVector(target, tgtLocal);
  reachFrame.inverseTransformVector(effector, effLocal);

  // Compute the angle between the effector and target (projected onto the x,z plane of the reach frame).
  //
  float effAngle = atan2f(effLocal.x, effLocal.z);
  float tgtAngle = atan2f(tgtLocal.x, tgtLocal.z);
  float angleEffToTgt = tgtAngle - effAngle;

  // 1(a) Check if the target is within a dead-zone around pi and if so apply some slack.

  // "Slack" angle:
  // if the target is within a threshold angle of pi away from the reference fwd direction we let the current
  // position of the effector determine the winding sense by forcing it to take the shortest arc towards the target.
  // In other words we allow the reach to continue about the body the way it is already going.
  // This avoids jitter from changes of direction when the target angle wrt to fwd is close to pi.
  //

  // Target is within "dead-zone" (slack-angle is the half angle either side of pi).
  //
  if ((NM_PI - slackAngle) < NMP::nmfabs(tgtAngle))
  {
    // The override is only required if the angle error just computed is the longer arc.
    if (angleEffToTgt > NM_PI)
    {
      angleEffToTgt -= 2 * NM_PI;
    }
    else if (angleEffToTgt < -NM_PI)
    {
      angleEffToTgt += 2 * NM_PI;
    }
  }

#ifdef MR_OUTPUT_DEBUGGING
  if (pDebugDrawInst && dimensionalScaling)
  {
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, reachFrame, dimensionalScaling->scaleDist(0.25f));
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, 
      reachOrigin, effLocal.x * reachFrame.xAxis() + effLocal.z * reachFrame.zAxis(), NMP::Colour::LIGHT_PURPLE);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
      reachOrigin, tgtLocal.x * reachFrame.xAxis() + tgtLocal.z * reachFrame.zAxis(), NMP::Colour::LIGHT_PURPLE);
  }
#endif

  return angleEffToTgt;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 calcAngleLimitedTarget(
  const                   NMP::Vector3& target,
  const                   NMP::Vector3& effector,
  float                   avoidanceRadius,
  float                   slackAngle,
  // outputs
  NMP::Vector3&           adjustedTargetApproach,
  float&                  effectorToTargetAngle,
  // "reach frame" parameters define the frame of rotation (origin, up and forward)
  const NMP::Vector3&     reachOrigin,         // origin (e.g. shoulder or chest)
  const NMP::Vector3&     reachUp,          // up direction (eg. spine or chest up)
  const NMP::Vector3&     reachFwd,          // forward (eg. reach cone direction)
  MR::InstanceDebugInterface* pDebugDrawInst,
  const ER::DimensionalScaling&   dimensionalScaling)
{
  NMP_ASSERT(reachUp.magnitude() > 0.01f);
  NMP::Vector3 up = NMP::vNormalise(reachUp);

  // Computing an adjusted target:
  // 1 Calculate the angle between the target and effector
  // 2 Calculate target adjustment (rotation of target towards effector) given angleFraction
  // 3 Rotate input target towards effector

  // The avoidance will be done so that when the effector is close to the body, motion will be
  // largely tangential. When further away we can drive more directly to the target - if that brings
  // the effector close to the torso then avoidance will start to drive it tangentially.

  // Avoid divide by zeros
  avoidanceRadius = NMP::maximum(avoidanceRadius, dimensionalScaling.scaleDist(0.01f));

  const NMP::Vector3 torsoToEffectorDelta = (effector - reachOrigin).getComponentOrthogonalToDir(up);
  const float torsoToEffectorDistance = torsoToEffectorDelta.magnitude();

  // The angle fraction (i.e. fraction between the effector and the target) should be small when
  // close to the torso, but large when further out. Ramp down based on the avoidance radius, but
  // alternatively something like the reach limit could be used.
  float angleFraction = (torsoToEffectorDistance - avoidanceRadius) / (avoidanceRadius);
  angleFraction = NMP::clampValue(angleFraction, 0.25f, 1.0f);

  // 1 Calculate the angle between the target and effector
  float angleEffToTgt = calcReachAngleToTarget(target, effector, slackAngle, reachOrigin, reachUp, reachFwd, 
    pDebugDrawInst, &dimensionalScaling);

  // 2 Calculate target adjustment (rotation of target towards effector) given angleFraction.
  float desiredEffectorToTargetOffset = angleEffToTgt * angleFraction;

  // Clamp the angle to 180 degrees to prevent driving towards the wrong side of the body when going
  // right round.
  desiredEffectorToTargetOffset = NMP::clampValue(desiredEffectorToTargetOffset, -NM_PI, NM_PI);

  // 3 Rotate input target towards effector
  NMP::Quat q;
  q.fromAxisAngle(up, -angleEffToTgt + desiredEffectorToTargetOffset);
  NMP::Vector3 adjustedTarget = reachOrigin + q.rotateVector(target - reachOrigin);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, adjustedTarget, dimensionalScaling.scaleDist(0.1f), NMP::Colour::ORANGE);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, effector, dimensionalScaling.scaleDist(0.1f), NMP::Colour::WHITE);

  // Compensate for the change (normally reduction) in strength
  NMP::Vector3 deltaFromEffectorToAdjustedTarget = adjustedTarget - effector;
  float effToAdjustedTargetDist = deltaFromEffectorToAdjustedTarget.magnitude();
  if (effToAdjustedTargetDist > dimensionalScaling.scaleDist(0.01f))
  {
    float effToTargetDist = (target - effector).magnitude();
    deltaFromEffectorToAdjustedTarget *= effToTargetDist / effToAdjustedTargetDist;
    adjustedTarget = effector + deltaFromEffectorToAdjustedTarget;
  }
  // Project the target onto the height of the target to prevent overshoots. 
  adjustedTarget = calcPointOnPlane(adjustedTarget, up, target);

  // Set outputs for return to caller.
  // Project the target onto the height of the target
  NMP::Vector3 adjustedTargetOnOriginPlane = calcPointOnPlane(adjustedTarget, up, reachOrigin);
  adjustedTargetApproach = (adjustedTargetOnOriginPlane - reachOrigin).getNormalised();
  effectorToTargetAngle = angleEffToTgt;

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, adjustedTarget, dimensionalScaling.scaleDist(0.1f), NMP::Colour::YELLOW);

  return adjustedTarget;
}

}

