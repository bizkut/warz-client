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
#include "morpheme/mrJointControlUtilities.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void jointPosVelControlInAxis(
  float               deltaTime,
  bool                jointPosVelInAxisEnable,
  float               jointPosVelMaxInAxis,
  bool                jointPosAccelInAxisEnable,
  float               jointPosAccelMaxInAxis,
  const NMP::Vector3& axisV,
  const NMP::Vector3& prevTargetJointPos,
  const NMP::Vector3& prevTargetJointPosVel,
  const NMP::Vector3& curTargetJointPos,
  NMP::Vector3&       curTargetJointPosVel)
{
  // Delta time factors
  NMP_ASSERT(deltaTime > 0.0f);
  float recipDeltaTime = 1.0f / deltaTime;

  // Assume that the target moves with constant velocity between the previous and current target positions.
  NMP::Vector3 curTargetJointPosVelInit = (curTargetJointPos - prevTargetJointPos) * recipDeltaTime;

  // Velocity component in the axis vector  
  float curTargetJointPosVelInAxisInit = axisV.dot(curTargetJointPosVelInit);
  float curTargetJointPosVelInAxis = curTargetJointPosVelInAxisInit;

  // Target velocity clamp
  if (jointPosVelInAxisEnable)
  {
    float signJointPosVelInAxis = NMP::floatSelect(curTargetJointPosVelInAxis, 1.0f, -1.0f);
    if (signJointPosVelInAxis * curTargetJointPosVelInAxis > jointPosVelMaxInAxis)
    {
      curTargetJointPosVelInAxis = signJointPosVelInAxis * jointPosVelMaxInAxis;
    }
  }

  // Target acceleration clamp
  if (jointPosAccelInAxisEnable)
  {
    float prevTargetJointPosVelInAxis = axisV.dot(prevTargetJointPosVel);

    // Discrete approximation for the target acceleration
    float curTargetJointPosAccelInAxis = (curTargetJointPosVelInAxis - prevTargetJointPosVelInAxis) * recipDeltaTime;
    float signJointPosAccelInAxis = NMP::floatSelect(curTargetJointPosAccelInAxis, 1.0f, -1.0f);
    if (signJointPosAccelInAxis * curTargetJointPosAccelInAxis > jointPosAccelMaxInAxis)
    {
      // Recompute the target velocity
      curTargetJointPosAccelInAxis = signJointPosAccelInAxis * jointPosAccelMaxInAxis;
      curTargetJointPosVelInAxis = curTargetJointPosAccelInAxis * deltaTime + prevTargetJointPosVelInAxis;
    }
  }

  // Set the output target velocity.
  curTargetJointPosVel = curTargetJointPosVelInit + axisV * (curTargetJointPosVelInAxis - curTargetJointPosVelInAxisInit);
}

//----------------------------------------------------------------------------------------------------------------------
void jointPosControlInAxis(
  float               deltaTime,
  bool                jointPosVelInAxisEnable,
  float               jointPosVelMaxInAxis,
  bool                jointPosAccelInAxisEnable,
  float               jointPosAccelMaxInAxis,
  float               weightBetweenPosAndVel,
  const NMP::Vector3& axisV,
  const NMP::Vector3& prevTargetJointPos,
  const NMP::Vector3& prevDrivenJointPos,
  const NMP::Vector3& prevDrivenJointPosVel,
  const NMP::Vector3& curTargetJointPos,
  const NMP::Vector3& curTargetJointPosVel,
  NMP::Vector3&       curDrivenJointPos,      // OUT
  NMP::Vector3&       curDrivenJointPosVel)   // OUT
{
  //---------------------------
  // TARGET JOINT MOTION

  NMP_ASSERT(deltaTime > 0.0f);
  float recipDeltaTime = 1.0f / deltaTime;
  float deltaTimeFac = 2.0f * deltaTime;
  float recipDeltaTimeFac = 0.5f * recipDeltaTime;

  // Compute an adjusted target joint position based on the weighting of the target joint
  // position and velocity. Since we model the driven joint motion using a quadratic curve
  // y(t) = a*t^2 + b*t + c, the resulting motion computed from position only fitting often
  // interpolates the target positions but can oscillate between time samples due to a lack of
  // velocity matching constraints.
  //
  // The target velocities can vary considerably for similar displacement step responses
  // with different delta time updates. Using the derivative y'(t) = 2*a*t + b to enforce
  // velocity constraints can lead to an unstable system with varying delta time updates.
  // The best strategy is to perform a position based solve based on an adjusted target
  // position. This adjusted target position is calculated by weighting between the desired
  // target position and an estimate of the driven joint position based on the clamped and
  // smoothed target velocity. This system is stable and tends to damp the driven joint motion
  // towards the target velocity.
  NMP::Vector3 curDrivenJointPosEstimate = prevDrivenJointPos + curTargetJointPosVel * deltaTime;

  // Compute the adjusted target position by weighting between the desired target position
  // and an estimate of the driven joint position. Note that we use the input target position
  // since the driven joint motion will have velocity and acceleration clamping applied to it.
  NMP::Vector3 curTargetJointPosAdj = curTargetJointPos * (1.0f - weightBetweenPosAndVel) + curDrivenJointPosEstimate * weightBetweenPosAndVel;

  //---------------------------
  // DRIVEN JOINT MOTION

  // Model the vertical component of the driven joint motion using a quadratic equation: y(t) = a*t^2 + b*t + c
  // where the acceleration is assumed to be constant between the previous and current driven positions.
  // Since y(t) = a*t^2 + b*t + c then y(0) = Dp (previous driven position), i.e. c = Dp
  // Since y'(t) = 2*a*t + b then y'(0) = Vp (previous driven velocity), i.e. b = Vp
  // In terms of y(t) we can recover the second order coefficient: a = (y(dt) - c - b*dt) / dt^2
  // i.e. a = (Vc - Vp) / dt, where Vc = (Tc - Dp) / dt (current discrete driven joint velocity)
  NMP::Vector3 curDrivenJointPosVelDiscrete = (curTargetJointPosAdj - prevDrivenJointPos) * recipDeltaTime;
  NMP::Vector3 jointPosCoefsA = (curDrivenJointPosVelDiscrete - prevDrivenJointPosVel) * recipDeltaTime;

  // Driven joint coefficients in the axis direction: y(t) = a*t^2 + b*t + c
  float jointPosCoefsInAxis[3];
  jointPosCoefsInAxis[0] = axisV.dot(jointPosCoefsA);         // a = (Vc - Vp) / dt, where Vc = (Tc - Dp) / dt
  jointPosCoefsInAxis[1] = axisV.dot(prevDrivenJointPosVel);  // b = Vp (previous driven velocity)
  jointPosCoefsInAxis[2] = axisV.dot(prevDrivenJointPos);     // c = Dp (previous driven position)

  // Initialise the driven joint output position
  curDrivenJointPos = curTargetJointPosAdj;
  float curTargetJointPosAdjInAxis = axisV.dot(curTargetJointPosAdj);

  // Initialise the driven joint output velocity: y'(t) = 2*a*t + b
  curDrivenJointPosVel = jointPosCoefsA * deltaTimeFac + prevDrivenJointPosVel;
  float curDrivenJointPosVelInAxis = axisV.dot(curDrivenJointPosVel);

  //---------------------------
  // Velocity clamp
  if (jointPosVelInAxisEnable)
  {
    float signJointPosVelInAxis = NMP::floatSelect(curDrivenJointPosVelInAxis, 1.0f, -1.0f);
    if (signJointPosVelInAxis * curDrivenJointPosVelInAxis > jointPosVelMaxInAxis)
    {
      // Since y'(t) = 2*a*t + b, and y'(dt) is clamped to the maximum velocity vmax then
      // vmax = 2*a*dt + b, thus a = (vmax - b) / (2*dt)
      jointPosCoefsInAxis[0] = (signJointPosVelInAxis * jointPosVelMaxInAxis - jointPosCoefsInAxis[1]) * recipDeltaTimeFac;
    }
  }

  //---------------------------
  // Acceleration clamping
  if (jointPosAccelInAxisEnable)
  {
    // Overshoot prediction
    bool status = true;
    if (jointPosAccelMaxInAxis > 1e-5f)
    {
      float deltaInAxis = curTargetJointPosAdjInAxis - jointPosCoefsInAxis[2]; // Tc - Dp
      float signDeltaInAxis = NMP::floatSelect(deltaInAxis, 1.0f, -1.0f);

      // Compute the overshoot curve coefficients, such that the quadratic motion equation
      // y(t) = a*t^2 + b*t + c has maximum deceleration.
      float overshootJointPosCoefsInAxis = -0.5f * jointPosAccelMaxInAxis * signDeltaInAxis; // Max deceleration

      // Target motion coefficients in axis direction: y(t) = b*t + c
      float targetJointPosCoefsInAxis[2];
      targetJointPosCoefsInAxis[0] = axisV.dot(curTargetJointPosVel);
      targetJointPosCoefsInAxis[1] = axisV.dot(prevTargetJointPos);

      // Compute the time at which the velocities of the driven (max deceleration) and target
      // joints will be the same. This assumes that the target joint continues with its linear
      // motion model y(t) = b*t + c, and the driven joint continues with its quadratic motion
      // model y(t) = a*t^2 + b*t + c. Equating the derivatives of the two and solving for t.
      float t = (targetJointPosCoefsInAxis[0] - jointPosCoefsInAxis[1]) / (2.0f * overshootJointPosCoefsInAxis);
      if (t > deltaTime)
      {
        // If we apply maximum deceleration towards the target and we calculate that it will take
        // longer than this frames time update step for the velocities to match then we may need
        // to apply the overshoot deceleration. Note that if the velocities match within the
        // time update step then the driven and target joint motion will be synchronised.

        // Compute the target joint position at that time
        float targetPos = targetJointPosCoefsInAxis[0] * t + targetJointPosCoefsInAxis[1];

        // Compute the driven joint position at that time
        float drivenPos = (overshootJointPosCoefsInAxis * t + jointPosCoefsInAxis[1]) * t + jointPosCoefsInAxis[2];

        // Check if the the driven joint overshoots the target joint by the time both have
        // the same velocity.
        if (signDeltaInAxis * (drivenPos - targetPos) > 0)
        {
          // There will be overshoot so apply maximum deceleration towards the target
          jointPosCoefsInAxis[0] = overshootJointPosCoefsInAxis;
          status = false;
        }
      }
    }

    // Acceleration clamping
    if (status)
    {
      // No overshoot. Just clamp the acceleration of the driven joint motion
      float jointPosAccelInAxis = 2.0f * jointPosCoefsInAxis[0];
      float jointPosAccelMagInAxis = NMP::nmfabs(jointPosAccelInAxis);
      if (jointPosAccelMagInAxis > jointPosAccelMaxInAxis)
      {
        float signPosAccelInAxis = jointPosAccelInAxis / jointPosAccelMagInAxis;
        jointPosAccelInAxis = signPosAccelInAxis * jointPosAccelMaxInAxis;
        jointPosCoefsInAxis[0] = 0.5f * jointPosAccelInAxis;
      }
    }
  }

  //---------------------------
  // Update the driven joint position
  float curDrivenJointPosInAxisNew = (jointPosCoefsInAxis[0] * deltaTime + jointPosCoefsInAxis[1]) * deltaTime + jointPosCoefsInAxis[2];
  curDrivenJointPos += axisV * (curDrivenJointPosInAxisNew - curTargetJointPosAdjInAxis);

  // Update the driven joint velocity
  float curDrivenJointPosVelInAxisNew = jointPosCoefsInAxis[0] * deltaTimeFac + jointPosCoefsInAxis[1];
  curDrivenJointPosVel += axisV * (curDrivenJointPosVelInAxisNew - curDrivenJointPosVelInAxis);
}

//----------------------------------------------------------------------------------------------------------------------
void jointAngVelControl(
  float               deltaTime,
  bool                jointAngVelEnable,
  float               jointAngVelMax,
  bool                jointAngAccelEnable,
  float               jointAngAccelMax,
  const NMP::Quat&    prevTargetJointAng,
  const NMP::Vector3& prevTargetJointAngVel,
  const NMP::Quat&    curTargetJointAng,
  NMP::Vector3&       curTargetJointAngVel)
{
  NMP_ASSERT(NMP::nmfabs(1.0f - prevTargetJointAng.magnitude()) < 1.5e-3f);
  NMP_ASSERT(NMP::nmfabs(1.0f - curTargetJointAng.magnitude()) < 1.5e-3f);

  // Delta time factors  
  NMP_ASSERT(deltaTime > 0.0f);
  float recipDeltaTime = 1.0f / deltaTime;

  // Assume that the target moves with constant velocity between the previous and current target orientations.
  // Compute the angular velocity for the closest path between the previous and current target orientations.
  NMP::Quat q0t = prevTargetJointAng;
  float signVal = NMP::floatSelect(q0t.dot(curTargetJointAng), 1.0f, -1.0f);
  q0t *= signVal;
  NMP::Quat targetDelta = ~q0t * curTargetJointAng;
  NMP::Vector3 targetJointAngVel = targetDelta.fastLog() * (2.0f * recipDeltaTime);

  // Target velocity clamp
  if (jointAngVelEnable)
  {
    float targetJointAngVelMag2 = targetJointAngVel.magnitudeSquared();
    if (targetJointAngVelMag2 > jointAngVelMax * jointAngVelMax)
    {
      float targetJointAngVelMag = sqrtf(targetJointAngVelMag2);
      targetJointAngVel *= (jointAngVelMax / targetJointAngVelMag);
    }
  }

  // Acceleration clamping
  if (jointAngAccelEnable)
  {
    NMP::Vector3 targetJointAngAccel = (targetJointAngVel - prevTargetJointAngVel) * recipDeltaTime;
    float targetJointAngAccelMag2 = targetJointAngAccel.magnitudeSquared();
    if (targetJointAngAccelMag2 > jointAngAccelMax * jointAngAccelMax)
    {
      float targetJointAngAccelMag = sqrtf(targetJointAngAccelMag2);
      targetJointAngAccel *= (jointAngAccelMax / targetJointAngAccelMag);
      targetJointAngVel = prevTargetJointAngVel + targetJointAngAccel * deltaTime;
    }
  }

  // Set the output target angular velocity.
  curTargetJointAngVel = targetJointAngVel;
}

//----------------------------------------------------------------------------------------------------------------------
void jointAngControl(
  float               deltaTime,
  bool                jointAngVelEnable,
  float               jointAngVelMax,
  bool                jointAngAccelEnable,
  float               jointAngAccelMax,
  float               weightBetweenAngAndVel,
  const NMP::Quat&    prevTargetJointAng,
  const NMP::Quat&    prevDrivenJointAng,
  const NMP::Vector3& prevDrivenJointAngVel,
  const NMP::Quat&    curTargetJointAng,
  const NMP::Vector3& curTargetJointAngVel,
  NMP::Quat&          curDrivenJointAng,
  NMP::Vector3&       curDrivenJointAngVel)
{
  NMP_ASSERT(NMP::nmfabs(1.0f - prevTargetJointAng.magnitude()) < 1.5e-3f);
  NMP_ASSERT(NMP::nmfabs(1.0f - prevDrivenJointAng.magnitude()) < 1.5e-3f);
  NMP_ASSERT(NMP::nmfabs(1.0f - curTargetJointAng.magnitude()) < 1.5e-3f); 

  //---------------------------
  // TARGET JOINT MOTION

  NMP_ASSERT(deltaTime > 0.0f);
  float recipDeltaTime = 1.0f / deltaTime;
  float deltaTimeFac = 0.5f * deltaTime;
  float recipDeltaTimeFac = 2.0f * recipDeltaTime;
  float timeDelta2Fac = deltaTimeFac * deltaTime;
  float recipDeltaTime2Fac = recipDeltaTimeFac * recipDeltaTime;

  // Compute an adjusted target joint orientation based on the weighting of the target joint
  // orientation and velocity. Since we model the driven joint motion using a quadratic
  // Bezier quaternion curve (in the cumulative form), the resulting motion computed from
  // orientation only fitting often interpolates the target orientations but can oscillate
  // between time samples due to a lack of angular velocity matching constraints.
  //
  // The target velocities can vary considerably for similar displacement step responses
  // with different delta time updates. Using the curve derivative to enforce velocity
  // constraints can lead to an unstable system with varying delta time updates.
  // The best strategy is to perform an orientation based solve based on an adjusted target
  // orientation. This adjusted target orientation is calculated by weighting between the desired
  // target orientation and an estimate of the driven joint orientation based on the clamped and
  // smoothed target velocity. This system is stable and tends to damp the driven joint motion
  // towards the target velocity.
  NMP::Quat curTargetJointAngVelDelta;
  curTargetJointAngVelDelta.fastExp(curTargetJointAngVel * deltaTimeFac);
  NMP::Quat curDrivenJointAngEstimate = prevDrivenJointAng * curTargetJointAngVelDelta;

  // Condition the estimate to be in the same semi-arc as the target orientation
  float fromDotTo = curTargetJointAng.dot(curDrivenJointAngEstimate);
  float signVal = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  curDrivenJointAngEstimate *= signVal;
  fromDotTo *= signVal;

  // Compute the adjusted target orientation by weighting between the desired target orientation
  // and an estimate of the driven joint orientation. Note that we use the input target orientation
  // since the driven joint motion will have velocity and acceleration clamping applied to it.
  NMP::Quat curTargetJointAngAdj;
  curTargetJointAngAdj.fastSlerp(curTargetJointAng, curDrivenJointAngEstimate, weightBetweenAngAndVel, fromDotTo);

  //---------------------------
  // DRIVEN JOINT MOTION

  // Get the end quat in the closest semi-arc to the current orientation
  NMP::Quat q2 = curTargetJointAngAdj;
  signVal = NMP::floatSelect(q2.dot(prevDrivenJointAng), 1.0f, -1.0f);
  q2 *= signVal;

  //---------------------------
  // Fit a quadratic curve (cumulative Bezier form) to the driven joint motion:
  // A quaternion Bezier curve can be represented by the control points {q0, q1, q2}.
  // However, it is better to represent this curve in the cumulative form (Hermite),
  // where interpolation is achieved from the deltas between control points. These deltas
  // are also closely linked to the angular velocities at the curve end points.
  //
  // q(u) = q0 * qexp(w1 * C1(u)) * qexp(w2 * C2(u)),
  // Where Ci(u) = \sum_{j=i} Bj(u) are the cumulative basis functions. Bj are the Bezier basis functions.
  // w1 = qlog(q0^-1 * q1), the delta between q0 and q1.
  // w2 = qlog(q1^-1 * q2), the delta between q1 and q2.
  //
  // Note: If u = t / dt is the change of interpolant for some t and delta time update dt.
  // w1 = wa * (dt / 2) where wa is the angular velocity at the curve endpoint q0.
  // w2 = wb * (dt / 2) where wb is the angular velocity at the curve endpoint q2.
  NMP::Quat jointAngQ0 = prevDrivenJointAng;
  NMP::Vector3 jointAngW1 = prevDrivenJointAngVel * deltaTimeFac;

  // Fit end w2 delta based on the orientation of the target
  NMP::Quat dq01;
  dq01.fastExp(jointAngW1);
  NMP::Quat q1 = jointAngQ0 * dq01;
  NMP::Quat dq12 = ~q1 * q2;
  NMP::Vector3 jointAngW2 = dq12.fastLog();

  // Fit end w2 delta based on the velocity of the target
  NMP::Quat q0t = prevTargetJointAng;
  signVal = NMP::floatSelect(q0t.dot(q2), 1.0f, -1.0f);
  q0t *= signVal;
  NMP::Quat targetDeltaQ02 = ~q0t * q2;
  NMP::Vector3 jointAngW2FromVel = targetDeltaQ02.fastLog() * 0.5f;

  //---------------------------
  // Velocity clamping
  if (jointAngVelEnable)
  {
    curDrivenJointAngVel = jointAngW2 * recipDeltaTimeFac;
    float curDrivenJointAngVelMag2 = curDrivenJointAngVel.magnitudeSquared();
    if (curDrivenJointAngVelMag2 > jointAngVelMax * jointAngVelMax)
    {
      float curDrivenJointAngVelMag = sqrtf(curDrivenJointAngVelMag2);
      curDrivenJointAngVel *= (jointAngVelMax / curDrivenJointAngVelMag);
      jointAngW2 = curDrivenJointAngVel * deltaTimeFac;
    }
  }

  //---------------------------
  // Acceleration clamping
  if (jointAngAccelEnable)
  {
    // Overshoot prediction
    bool status = true;

    NMP::Quat drivenDeltaQ02 = ~jointAngQ0 * q2;
    NMP::Vector3 jointDir = drivenDeltaQ02.fastLog();
    float mag2 = jointDir.magnitudeSquared();
    if (mag2 > 1e-5f)
    {
      // Direction vector to (de)accelerate in
      float fac = 1 / sqrtf(mag2);
      jointDir *= fac;

      // Compute the overshoot curve coefficients
      NMP::Vector3 overshootJointAngW2 = jointAngW1 + jointDir * (-jointAngAccelMax * timeDelta2Fac);

      // Find the time position where the velocities in the acceleration direction are the same
      float D = jointDir.dot(overshootJointAngW2 - jointAngW1);
      if (NMP::nmfabs(D) > 1e-5f)
      {
        float N = jointDir.dot(jointAngW2FromVel - jointAngW1);
        float u = N / D;
        if (u > 1)
        {
          // Compute the orientation at that time for the predicted target joint
          NMP::Quat dq, dq1, dq2;
          dq.fastExp(jointAngW2FromVel * u);
          NMP::Quat targetAng = q0t * dq;

          // Compute the orientation at that time for the driven joint
          float B1 = u * (2 - u);
          float B2 = u * u;
          dq1.fastExp(jointAngW1 * B1);
          dq2.fastExp(overshootJointAngW2 * B2);
          NMP::Quat drivenAng = jointAngQ0 * dq1 * dq2;

          // Check if the driven joint overshoots the target joint
          dq = ~targetAng * drivenAng;
          NMP::Vector3 w = dq.fastLog();
          if (jointDir.dot(w) > 0)
          {
            // Overshoot
            jointAngW2 = overshootJointAngW2;
            status = false;
          }
        }
      }
    }

    // Clamp the acceleration
    if (status)
    {
      // No overshoot. Just clamp the acceleration of the driven joint motion
      NMP::Vector3 jointAngAccel = (jointAngW2 - jointAngW1) * recipDeltaTime2Fac;
      float jointAngAccelMag2 = jointAngAccel.magnitudeSquared();
      if (jointAngAccelMag2 > jointAngAccelMax * jointAngAccelMax)
      {
        float jointAngAccelMag = sqrtf(jointAngAccelMag2);
        jointAngAccel *= (jointAngAccelMax / jointAngAccelMag);
        jointAngW2 = jointAngW1 + jointAngAccel * timeDelta2Fac;
      }
    }
  }

  //---------------------------
  // Update the driven joint orientation
  dq12.fastExp(jointAngW2);
  curDrivenJointAng = q1 * dq12;

  // Since the driven joint quat is a product of the previous quat and this frames update, there
  // is a real danger of numerical error accumulating over time. Normalise to prevent this issue.
  curDrivenJointAng.fastNormalise();

  // Update the driven joint velocity
  curDrivenJointAngVel = jointAngW2 * recipDeltaTimeFac;
}


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
