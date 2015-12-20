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
#include "euphoria/erLimbTransforms.h"
#include "MyNetworkData.h"
#include "Types/BraceDynamic.h"
#include "Types/SphereTrajectory.h"
#include "Types/SpatialTarget.h"
#include "Types/LimbControl.h"
#include "Types/processRequest.h"

#define SCALING_SOURCE dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
float BraceDynamic::distanceScale()
{
  return mb / (m + mb);
}

//----------------------------------------------------------------------------------------------------------------------
void BraceDynamic::init(float baseMass, float braceObjectMass, float hazardMass)
{
  m = braceObjectMass;
  mb = baseMass;
  mh = hazardMass;
  maxReachDistance = 10.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void BraceDynamic::setPositions(const NMP::Vector3& basePosition, const NMP::Vector3& braceObjectPosition, const NMP::Vector3& hazardPosition)
{
  centreOfMass = (basePosition * mb + braceObjectPosition * m) / (m + mb);
  P0 = braceObjectPosition - centreOfMass;
  Ph = hazardPosition - centreOfMass;
}

//----------------------------------------------------------------------------------------------------------------------
void BraceDynamic::setVelocities(const NMP::Vector3& baseVelocity, const NMP::Vector3& braceObjectVelocity, const NMP::Vector3& hazardVelocity)
{
  centreOfMassVelocity = (baseVelocity * mb + braceObjectVelocity * m) / (m + mb);
  V0 = braceObjectVelocity - centreOfMassVelocity;
  Vh = hazardVelocity - centreOfMassVelocity;
}

//----------------------------------------------------------------------------------------------------------------------
float BraceDynamic::getBufferStiffnessScale() const
{
  // This scales the stiffness to compensate for the fact that the body is pushing against a bigger mass than just the hand
  float strengthScale = 1.0f + mb * mh / (m * (mh + mb + m));
  return sqrtf(strengthScale);
}

//----------------------------------------------------------------------------------------------------------------------
float BraceDynamic::getControlMass() const
{
  return mb * m / (mb + m);
}

//----------------------------------------------------------------------------------------------------------------------
float BraceDynamic::getImpactMass() const
{
  return mb * mh / (mb + mh);
}

//----------------------------------------------------------------------------------------------------------------------
float BraceDynamic::findInterceptTime(const NMP::Vector3& braceDirection, const NMP::Vector3& interceptVel)
{
  // The names below refer to mathematical tokens used in the brace documentation.
  // see Docs/BraceDynmamics.docx
  NMP::Vector3 Va = interceptVel;
  float mc =  ((m + mh) * mb / (m + mh + mb)); // combined 'effective' mass
  // convert problem into just the component in the brace direction
  float ph = Ph.dot(braceDirection);
  float vh = Vh.dot(braceDirection);
  float va = Va.dot(braceDirection);
  // temporary velocity-squared value
  float Vsqr = distanceScale() * NMP::sqr((m * va + mh * vh) / (m + mh) + m * va / mb);

  // if we're pre interception then we need to find the best intercept point and velocity
  float t = findBestInterceptTime(braceDirection, mc, Vsqr, 1.0f, Va);
  if (t <= 0.0f || t > -ph / vh)
  {
    t = findBestInterceptTime(braceDirection, mc, Vsqr, -1.0f, Va);
  }
  if (t <= 0.0f || t > -ph / vh)
  {
    t = 0.0f;
  }
  float p1 = ph + vh * t;
  // if out of range, then adjust time
  if (p1 > maxReachDistance * distanceScale() && vh < 0.0f)
    return (maxReachDistance * distanceScale() - ph) / vh;
  // calculate max time
  float maxTime = (minReachDistance + P0.dot(braceDirection) * (distanceScale() - 1.0f) - ph) / vh;
  // clamp t to be larger than the max time
  if (t > maxTime)
    t = NMP::maximum(maxTime, 0.0f);
  return t;
}

//----------------------------------------------------------------------------------------------------------------------
// braceDir- brace direction
// mc- combined effective mass
// VSqr- square of impact velocity
// dir- -1 or 1 for choosing direction to test from
// Va- intercept velocity
float BraceDynamic::findBestInterceptTime(
  const NMP::Vector3& braceDir,
  float mc,
  float Vsqr,
  float dir,
  const NMP::Vector3& Va)
{
  // see braceDynamic.docx
  // get position and velocity of the hazard in the brace direction
  float ph = Ph.dot(braceDir);
  float vh = Vh.dot(braceDir);

  // we basically are formulating a quadratic curve here, in order to find the intersection point
  NMP::Vector3 aV = 2.0f * Va + V0 - 3.0f * Vh;
  NMP::Vector3 bV = -3.0f * (Ph - P0);
  float grad = aV.magnitude();
  float min = aV.dot(bV) / aV.magnitudeSquared();
  float a = dir * grad;
  float height = -bV.magnitude() + grad * min;
  float b = height - dir * grad * min;

  // convert above into standard Ax^2 + Bx + C = 0 quadratic equation where x is the intercept time
  float A = mc * Vsqr / 2.0f  + 2.0f * m * a * vh;
  float B = 2.0f * m * (a * ph + b * vh);
  float C = 2.0f * m * b * ph;
  if (B * B - 4.0f * A * C < 0.0f)
  {
    return 0.0f;
  }

  // below is an accuracy friendly version of quadratic solution formula
  if (B < 0.0f)
  {
    return (-B + sqrtf(B * B - 4.0f * A * C)) / (2.0f * A);
  }
  else
  {
    return 2.0f * C / (-B - sqrtf(B * B - 4.0f * A * C));
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 BraceDynamic::getWorldVel(const NMP::Vector3& localVel) const
{
  return localVel + centreOfMassVelocity;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 BraceDynamic::getLocalVel(const NMP::Vector3& worldVel) const
{
  return worldVel - centreOfMassVelocity;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

