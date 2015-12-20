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

#include "euphoria/erDebugDraw.h"
#include "Types/Environment_State.h"
#include "Types/Environment_Patch.h"
#include "Types/Environment_LocalShape.h"
#include "Types/Environment_CollideResult.h"
#include "Types/SphereTrajectory.h"
#include "Types/ObjectMetric.h"

// This time period scales down angular velocities based on an averaging over time.
// It prevents quickly rotating objects from giving unhelpful point-in-time velocities
static float spinAverageTimeLengthSqr = 0.0f;

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
extern int cNumberOfFacesOrCapsuleEdges[Environment::Patch::EO_Max];

//----------------------------------------------------------------------------------------------------------------------
Environment::State::State()
{
  aabb.setEmpty();
  position.setToZero();
  velocity.setToZero();
  angularVelocity.setToZero();
  acceleration.setToZero();
  shapeID = -1;
  isStatic = false;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Environment::State::getInstantVelocityAtPoint(const NMP::Vector3& point) const
{
  return velocity + NMP::vCross(angularVelocity, point - position);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Environment::State::getVelocityAtPoint(const NMP::Vector3& point) const
{
  return velocity + NMP::vCross(getAverageAngularVel(), point - position);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Environment::State::getAccelerationAtPoint(const NMP::Vector3& point) const
{
  return acceleration + NMP::vCross(getAverageAngularVel(), getVelocityAtPoint(point));  // since no rotational velocity considered yet
}

//----------------------------------------------------------------------------------------------------------------------
// this is a simple heuristic way to get an average angular velocity from a fast spinning object
NMP::Vector3 Environment::State::getAverageAngularVel() const
{
  float angularEnergy = angularVelocity.magnitudeSquared();
  // If you are using the angular velocity to linearly predict the offset of an off centre point on a spinning object, then for low
  // ang vels you can use the linear prediction just fine, for larger ang vels the offset averages something closer and closer to 0 as the
  // start offset becomes less and less relevent.
  // the heuristic below reflects this
  float angVelHeuristic = NMP::maximum(0.0f, -1.0f + 16.0f / (8.0f + angularEnergy * spinAverageTimeLengthSqr));
  return angularVelocity * angVelHeuristic; // returns 0 for angular velocities > PI/timePeriod.
}

//----------------------------------------------------------------------------------------------------------------------
// average velocity of a point on the spinning object
NMP::Vector3 Environment::State::getAveragedPointVel(const NMP::Vector3& point, float timePeriod) const
{
  if (timePeriod <= 0.01f)
  {
    return getVelocityAtPoint(point);
  }

  // Gets the average angular vel over the time period, then rotates the point appropriately.
  NMP::Vector3 offset = point - position;
  NMP::Vector3 averageAngVel = getAverageAngularVel();
  NMP::Quat endRot(averageAngVel * timePeriod, false);
  NMP::Vector3 end = endRot.rotateVector(offset);

  // then add on the linear velocity and acceleration
  end += velocity * timePeriod + 0.5f * acceleration * NMP::sqr(timePeriod);

  return (end - offset) / timePeriod; // return the difference / time
}

//----------------------------------------------------------------------------------------------------------------------
// for objects rotating faster than this square angular vel, do a linear extrapolation of the path due to this rotation
static const float minAngVelSqr = NMP::sqr(0.2f);
// for objects rotating faster than this value, linear extrapolation is inaccurate so use and even slower, curved extrapolation
static const float linearAngVelSqr = NMP::sqr(1.0f);

//----------------------------------------------------------------------------------------------------------------------
// bends a path to account for angular velocity of object
// a quadratic path collision against an unspinning object can be extended to work on spinning objects by
// bending the quadratic path to approximate the curved spiral it would take relative to the object
void Environment::State::adjustPathForAngularVelocity(SphereTrajectory& path) const
{
  // the multiplier below is a hard coded improvement where we assume more accuracy is needed in the first half of the curve than the second
  // set to 1 to use the full curve for the adjustment
  NMP::Vector3 averageAngVel = getAverageAngularVel();
  float magSqr = averageAngVel.magnitudeSquared();
  if (magSqr > minAngVelSqr)
  {
    // first localise the path to the object linearly
    // this factor is for how much of the path you want to get an accurate motion for.
    // we could approximate a motion for the whole path (value = 1) but then it is less accurate for more imminent part of the path
    // since the more imminent part is more important we choose a value here less than 1
    const float reasonablePathAmount = 0.5f;
    float maxTime = reasonablePathAmount * path.maxTime;
    float timeSqr = NMP::sqr(maxTime);
    path.position -= position;
    path.velocity -= velocity;
    path.acceleration -= acceleration;

    // now get the mid and end points on this path
    NMP::Vector3 end = path.position + path.velocity * maxTime + path.acceleration * 0.5f * timeSqr;
    NMP::Vector3 mid = path.position + path.velocity * 0.5f * maxTime + path.acceleration * 0.5f * 0.25f * timeSqr;
    averageAngVel *= -maxTime * 0.5f;
    // rotate the mid point based on average angular velocity over half the time period
    if (magSqr < linearAngVelSqr) // for smallish velocities we can linearise and save time
    {
      NMP::Vector3 offset;
      offset.cross(averageAngVel, mid);
      mid += offset - path.position;
      offset.cross(averageAngVel * 2.0f, end);
      end += offset - path.position;
    }
    else
    {
      NMP::Quat rot(averageAngVel, false);
      mid = rot.rotateVector(mid) - path.position;
      // rotate the end point based on average angular velocity over the whole time period
      rot = rot * rot; // double the rotation
      end = rot.rotateVector(end) - path.position;
    }
    // convert end and mid back into vel and acc
    path.velocity = (4.0f * mid - end) / maxTime;
    path.acceleration = 4.0f * (end - 2.0f * mid) / timeSqr;

    // lastly globalise the path again
    path.position += position;
    path.velocity += velocity;
    path.acceleration += acceleration;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Environment::State::rotateNormal(const NMP::Vector3& normal, float time) const
{
  NMP::Vector3 averageAngVel = getAverageAngularVel();
  NMP::Quat endRot(averageAngVel * time, false);
  return endRot.rotateVector(normal);
}

//----------------------------------------------------------------------------------------------------------------------
bool Environment::State::getTrajectoryOverlapTimes(const SphereTrajectory& path, float& t1, float& t2)
{
  if ((t1 = sphereTrajectoryCollides(path, true)) == -1.0f)
  {
    return false;
  }

  // the backwards sphere trajectory is colliding the same path but in a backwards direction
  // This is used to get the exit time, the forwards trajectory above gets the entry time
  if ((t2 = sphereTrajectoryCollides(path, false)) == -1.0f)
  {
    return false;
  }
  t2 = path.maxTime - t2;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// returns the first time value when the sphere trajectory will collide with the state object (which is an aabb)
float Environment::State::sphereTrajectoryCollides(const SphereTrajectory& path, bool forwards)
{
  int i = 0;
  float t = 0.0f;
  // firstly localise the trajectory to the state object
  NMP::Vector3 startLocal = path.position - aabb.getCentre();
  NMP::Vector3 velLocal = path.velocity - velocity;
  NMP::Vector3 accLocal = path.acceleration - acceleration;
  NMP::Vector3 halfExtents = aabb.getHalfExtents();
  // option for iterating backwards, so move values to other end of time line
  if (!forwards)
  {
    startLocal += velLocal * path.maxTime + 0.5f * accLocal * NMP::sqr(path.maxTime);
    velLocal = -(velLocal + accLocal * path.maxTime);
  }
  float newRadius = path.radius;

  // then pick a preferred axis to start the sweep tests with
  if (startLocal.x > halfExtents.x + newRadius || startLocal.x < -halfExtents.x - newRadius)
  {
    i = 0;
  }
  else if (startLocal.y > halfExtents.y + newRadius || startLocal.y < -halfExtents.y - newRadius)
  {
    i = 1;
  }
  else if (startLocal.z > halfExtents.z + newRadius || startLocal.z < -halfExtents.z - newRadius)
  {
    i = 2;
  }
  else
  {
    // inside box at the very start
    return 0.0f;
  }
  float intersect = startLocal[i];

  // now iterate over the axes getting closer each time
  for (; ;)
  {
    float dir = intersect > 0.0f ? 1.0f : -1.0f;
    // sweep time for colliding with just this axis
    float newT = SphereTrajectory::getCollidePlaneTime(startLocal[i] * dir + -halfExtents[i], velLocal[i] * -dir, accLocal[i] * -dir, newRadius);
    if (newT <= t || newT > path.maxTime) // intersects too early or doesn't intersect at all
    {
      return -1.0f;
    }
    t = newT;
    // check for collision against next axis
    i = (i + 1) % 3;
    intersect = startLocal[i] + velLocal[i] * t + accLocal[i] * 0.5f * t * t;
    if (intersect > halfExtents[i] + newRadius || intersect < -halfExtents[i] - newRadius)
    {
      continue;
    }
    // check for collision against next axis
    i = (i + 1) % 3;
    intersect = startLocal[i] + velLocal[i] * t + accLocal[i] * 0.5f * t * t;
    if (intersect > halfExtents[i] + newRadius || intersect < -halfExtents[i] - newRadius)
    {
      continue;
    }
    return t;
  }

#ifdef _MSC_VER
  __assume(0);
#endif // _MSC_VER
}

//----------------------------------------------------------------------------------------------------------------------
// culls the object bound against the bound of the path
bool Environment::State::boundingBoxCull(const SphereTrajectory& path) const
{
  NMP::Vector3 startLocal = path.position - aabb.getCentre();

  // we use a trick here, mid is the middle handle of the trajectory if it was a quadratic bezier
  // that means the triangle start-mid-end completely encloses the trajectory, making culling easy
  NMP::Vector3 midLocal = startLocal + (path.velocity - velocity) * path.maxTime;
  NMP::Vector3 endLocal = midLocal   + 0.5f * (path.acceleration - acceleration) * path.maxTime;

  for (int i = 0; i < 3; i++)
  {
    float dim = aabb.getHalfExtents()[i] + path.radius;
    if (dim < startLocal[i] && dim < endLocal[i] && dim < midLocal[i])
    {
      return false;
    }

    dim = -dim;
    if (dim > startLocal[i] && dim > endLocal[i] && dim > midLocal[i])
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Environment::State::debugDraw(
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(startColour),
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(endColour),
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
#if defined(MR_OUTPUT_DEBUGGING)
  // Sequence of connected line segments (alpha blended linearly)
  int numSections = 8;
  float maximumTime = 1.0f;
  for (int i = 0; i < numSections; i++)
  {
    float blend = (float)i / (float)(numSections);
    float t1 = maximumTime * blend;
    float t2 = t1 + maximumTime / (float)(numSections);
    NMP::Vector3 pos1 = position + velocity * t1 + acceleration * 0.5f * t1 * t1;
    NMP::Vector3 pos2 = position + velocity * t2 + acceleration * 0.5f * t2 * t2;
    NMP::Vector3 colour = startColour * (1.0f - blend) + endColour * blend;
    colour.w = startColour.w * (1.0f - blend) + endColour.w * blend;
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, pos1, pos2, NMP::Colour(colour));
  }
#endif
}
// Returns the metric value for the passed in object state.
// 0 is an unimportance object, 1 is sufficiently important to act on, can be larger than 1
// The actual choice of what makes an object important is controlled by the metric's coefficients
float ObjectMetric::getMetric(const Environment::State& state, float timeStep, float& lastMetric) const
{
  // Must have greater than a minimum mass
  if (state.mass < minMass)
  {
    return 0.0f;
  }
  // Must be smaller than a maximum size, this prevents trying to look at large objects like the terrain
  if (state.aabb.getHalfExtents().x + state.aabb.getHalfExtents().y + state.aabb.getHalfExtents().z > 3.0f * maxAverageSize)
  {
    return 0.0f;
  }
  // Must be within a certain distance to be important

  // Importance given for speed anf acceleration of the object relative to the character
  float velocityMetric = (state.velocity - charactersVelocity).magnitudeSquared() * speedSquaredCoefficient;
  float accelerationMetric = state.acceleration.magnitudeSquared() * accelerationSquaredCoefficient;
  float distanceMetric = (state.position - charactersPosition).magnitudeSquared() * distanceSquaredCoefficient;
  float metric = velocityMetric + accelerationMetric - distanceMetric;

  // Hysteresis- currently most important object is given extra advantage, to prevent flipping
  if (state.shapeID == focusShapeID)
  {
    // Implicit exponential decay, linear decay is too susceptible to large value outliers
    float oldMetricReduced = lastMetric / (1.0f + interestReductionRate * timeStep);
    metric = NMP::maximum(metric + shapeAdvantage, oldMetricReduced);
    lastMetric = metric;
  }
  return metric;
}
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

