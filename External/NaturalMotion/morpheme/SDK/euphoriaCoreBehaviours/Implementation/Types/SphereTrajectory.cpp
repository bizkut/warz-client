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
#include "Types/SphereTrajectory.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

void SphereTrajectory::set(const NMP::Vector3& pos, const NMP::Vector3& vel, const NMP::Vector3& acc, float rad, float extraSearchRad, float maxT)
{
  position = pos;
  velocity = vel;
  acceleration = acc;
  radius = rad;
  maxTime = maxT;
  extraSearchRadius = extraSearchRad;
}

// +ve startPos is distance away
// +ve velocity is an approaching velocity
// +ve acceleration is accelerating towards the plane
float SphereTrajectory::getCollidePlaneTime(float startPos, float velocity, float acceleration, float& modifiedRadius)
{
  // This next block is a feature that allows contact within the sphere radius, and adjusts the radius appropriately
  // specifically this is to deal with initial intersections of the sphere trajectory. Dealing with this makes the system more robust
  float farthestAwayTime = 0.0f;
  if (acceleration > 0.01f)
  {
    farthestAwayTime = NMP::maximum(0.0f, -0.5f * velocity / acceleration);
    float pos = startPos - (velocity * farthestAwayTime + acceleration * 0.5f * farthestAwayTime * farthestAwayTime);
    if (pos < modifiedRadius)
    {
      if (pos < 0.0f)
        return -1.0f; // doesn't collide even with a radius of 0
      modifiedRadius = pos;
      if (farthestAwayTime == 0.0f)
        return 0.0f; // collides at the very start
    }
  }
  else if (startPos > 0.0f && startPos < modifiedRadius && velocity > 0.0f) // if heading towards the plane and touching it already
  {
    modifiedRadius = startPos;
    return 0.0f; // collides at the very start
  }
  startPos -= (modifiedRadius - 1e-5f);

  float speedSqr = NMP::sqr(velocity) + 2.0f * acceleration * startPos;
  if (speedSqr < 0.0f)
    return -1.0f; // no contact flag

  float impactSpeed = sqrtf(speedSqr);
  float impactTime;

  if (NMP::sqr(acceleration) > NMP::sqr(impactSpeed + velocity)) // note the denominators below are always > 0 as a result of this check
    impactTime = (impactSpeed - velocity) / acceleration;
  else if ((impactSpeed + velocity) != 0.0f)
    impactTime = 2.0f * startPos / (impactSpeed + velocity);
  else
    impactTime = 0.0f; // if no velocity or acceleration then return time of 0
  return impactTime;
}

// returns -1 if no contact, 0 if it starts in collision
float SphereTrajectory::getCollidePlaneTime(const NMP::Vector3& point, const NMP::Vector3& normal) // radius gets adjusted here
{
  float startPos = (point - position).dot(normal);
  float speed = -velocity.dot(normal);
  float acc = -acceleration.dot(normal);
  float newRadius = radius;
  return getCollidePlaneTime(startPos, speed, acc, newRadius);
}

NMP::Vector3 SphereTrajectory::getPoint(float t) const
{
  return position + velocity * t + acceleration * 0.5f * t * t;
}

NMP::Vector3 SphereTrajectory::getVelocity(float t) const
{
  return velocity + acceleration * t;
}

// returns -1 for no collision, 0 if it starts in collision
float SphereTrajectory::getCollidePointTime(const NMP::Vector3& point)
{
  NMP::Vector3 newPosition = position;
  float t = 0.0f;
  // we could use variable loop in order to reach a given accuracy
  // but this fixed loop is easier to work with, with with known cost, and no sudden change in result when
  // the number of iterations changes.
  for (int i = 0; i < 4; i++)
  {
    NMP::Vector3 normal = vNormalise(point - newPosition);
    t = getCollidePlaneTime(point, normal);
    if (t <= 0.0f) // inside at start or no collision
      return t;
    newPosition = position + velocity * t + acceleration * 0.5f * t * t;
  }
  return t;
}

void SphereTrajectory::debugDraw(
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(startColour),
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(endColour),
  float MR_OUTPUT_DEBUG_ARG(maximumTime),
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
#if defined(MR_OUTPUT_DEBUGGING)
  if (maximumTime == 0.0f)
    maximumTime = maxTime;
  int numSections = 8;
  MR_DEBUG_DRAW_SPHERE(pDebugDrawInst, NMP::Matrix34(NMP::Quat::kIdentity, position), radius, NMP::Colour::RED);
  for (int i = 0; i < numSections; i++)
  {
    float blend = (float)i / (float)(numSections);
    float t1 = maximumTime * blend;
    float t2 = t1 + maximumTime * 0.5f / (float)(numSections);
    NMP::Vector3 pos1 = position + velocity * t1 + acceleration * 0.5f * t1 * t1;
    NMP::Vector3 pos2 = position + velocity * t2 + acceleration * 0.5f * t2 * t2;
    NMP::Vector3 colour = startColour * (1.0f - blend) + endColour * blend;
    colour.w = t2 > maxTime ? 0.3f : 1.0f;
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, pos1, pos2, NMP::Colour(colour));
  }
#endif
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

