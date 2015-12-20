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
#include "Helpers/Helpers.h"
#include "morpheme/mrInstanceDebugInterface.h"
#include "euphoria/erDebugDraw.h"

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// Converts the problem to one where object A is stationary and at the origin - then the time is
// when B is closest to the origin.
ClosestPointOfApproachData calculateClosestPointOfApproach(
  const NMP::Vector3& positionA, const NMP::Vector3& positionB, 
  const NMP::Vector3& velocityA, const NMP::Vector3& velocityB)
{
  ClosestPointOfApproachData cpa;

  // initial difference vector between the two tracks
  NMP::Vector3 deltaPosition = positionA - positionB;
  // relative velocity
  NMP::Vector3 deltaVelocity = velocityA - velocityB;
  // magnitude of relative velocity in direction of PQ
  float proj = - deltaPosition.dot(deltaVelocity);
  // divide by square of relative velocity to get time to cpa
  float deltaVelocityMagSq = deltaVelocity.magnitudeSquared();
  if (deltaVelocityMagSq > 0.0f)
  {
    cpa.time = NMP::maximum(proj / deltaVelocityMagSq, 0.0f);

    // predicted positions
    cpa.positionA = positionA + cpa.time * velocityA;
    cpa.positionB = positionB + cpa.time * velocityB;
  }
  else
  {
    cpa.positionA = positionA;
    cpa.positionB = positionB;
    cpa.time = 0.0f;
  }
  // distance at time to cpa
  cpa.dist = NMP::vDistanceBetween(cpa.positionA, cpa.positionB);

  return cpa;
}

//----------------------------------------------------------------------------------------------------------------------
// ComplexNumber
//
// Very simple representation of a complex number used in the following solveCubicPolynomial function.
// 
struct ComplexNumber
{
  double r;
  double i;
};

//----------------------------------------------------------------------------------------------------------------------
// solveCubicPolynomial
// 
// Takes the parameters of a cubic polynomial in the form ax^3 + bx^2 + cx + d, computes the three roots of that
// polynomial and writes them to the outputRoot arguments.
// 
static void solveCubicPolynomial(
  const float a,
  const float b,
  const float c,
  const float d,
  ComplexNumber& outputRoot0,
  ComplexNumber& outputRoot1,
  ComplexNumber& outputRoot2)
{
  NMP_ASSERT(a > 0.0f); // ensure that the argument is actually a cubic polynomial

  if (a > 0.0f)
  {
    const double delta = 
      (18.0f * a * b * c * d) -
      (4.0f * NMP::cube(b) * d) + 
      (NMP::sqr(b) * NMP::sqr(c)) -
      (4.0f * a * NMP::cube(c)) -
      (27.0f * NMP::sqr(a) * NMP::sqr(d));

    const double delta0 = NMP::sqr(b) - (3.0 * a * c);
    const double delta1 = (2.0 * NMP::cube(b)) - (9.0 * a * b * c) + (27.0 * NMP::sqr(a) * d);

    // Find fixed term (complex number that is the same for each root)
    //
    // fixedTerm = ( delta1 + (-27 * a * a * delta)^1/2 )^1/3

    static const double oneThird  = 1.0 / 3.0;
    ComplexNumber fixedTerm;
    const double fixedTermIntermediate = -27.0 * a * a * delta; // this is the -27 * a * a * delta term from fixedTerm

    if (fixedTermIntermediate > 0.0)
    {
      fixedTerm.r = 0.5 * (delta1 + sqrt(fixedTermIntermediate));
      fixedTerm.i = 0.0;
    }
    else
    {
      fixedTerm.r = 0.5 * delta1;
      fixedTerm.i = 0.5 * sqrt(-fixedTermIntermediate);
    }

    // Find cube root
    // note: Pow fn result is undefined for negative value and non integer power

    const double fixedTermR = sqrt(NMP::sqr(fixedTerm.r) + NMP::sqr(fixedTerm.i));
    const double fixedTermTheta = atan2(fixedTerm.i, fixedTerm.r);

    const double cubeRootOfR = pow(fixedTermR, oneThird);
    const double fixedTermThetaOverThree = fixedTermTheta / 3.0;
    fixedTerm.r = cubeRootOfR * cos(fixedTermThetaOverThree);
    fixedTerm.i = cubeRootOfR * sin(fixedTermThetaOverThree);

    // Find varying terms (complex number that is different for each root)
    ComplexNumber varyingTerm[3];

    static const double sqrtOf3 = sqrt(3.0);

    varyingTerm[0].r = 1.0;
    varyingTerm[0].i = 0.0;

    varyingTerm[1].r = -0.5;
    varyingTerm[1].i = 0.5 * sqrtOf3;

    varyingTerm[2].r = -0.5;
    varyingTerm[2].i = -0.5 * sqrtOf3;

    // For each root calculate:
    //
    // root[i] = (-1/(3a))(b + (varyingTerm[i] * fixedTerm) + (delta0 / (varyingTerm[i] * fixedTerm)))
    // 
    // Separate 3rd term into real and complex parts by multiplying top and bottom by complex conjugate of
    // (varyingTerm[i] * fixedTerm).

    const double multiplier = (-1.0 / (3.0 * a)); 

    ComplexNumber outputRoot[3];

    for (uint32_t i=0; i<3; ++i)
    {
      outputRoot[i].r = 0.0f;
      outputRoot[i].i = 0.0f;

      ComplexNumber perRootTerm; // FixedTerm * VaryingTerm[i]
      perRootTerm.r = (varyingTerm[i].r * fixedTerm.r) - (varyingTerm[i].i * fixedTerm.i);
      perRootTerm.i = (varyingTerm[i].r * fixedTerm.i) + (varyingTerm[i].i * fixedTerm.r);

      // Invert per root term 
      // + multiply top and bottom of inverse by per root term's complex conjugate to move all imaginary parts to the
      //   numerator.
      const double perRootTermMagSq = (NMP::sqr(perRootTerm.r) + NMP::sqr(perRootTerm.i));
      if(perRootTermMagSq > 0.0f)
      {
        ComplexNumber invPerRootTerm;
        invPerRootTerm.r = perRootTerm.r / perRootTermMagSq;
        invPerRootTerm.i = -perRootTerm.i / perRootTermMagSq;

        // Evaluate real and imaginary parts of the current root.
        outputRoot[i].r = multiplier * (b + perRootTerm.r + (delta0 * invPerRootTerm.r));
        outputRoot[i].i = multiplier * (perRootTerm.i + (delta0 * invPerRootTerm.i));
      }
    }

    outputRoot0 = outputRoot[0];
    outputRoot1 = outputRoot[1];
    outputRoot2 = outputRoot[2];
  }
}

//----------------------------------------------------------------------------------------------------------------------
ClosestPointOfApproachData calculateClosestPointOfApproach(
  const NMP::Vector3& positionA,
  const NMP::Vector3& positionB,
  const NMP::Vector3& velocityA,
  const NMP::Vector3& velocityB,
  const NMP::Vector3& accelerationA,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Reframe everything so that A is effectively stationary at the origin.
  const NMP::Vector3 position     = positionB - positionA; // Initial position of track B relative to A
  const NMP::Vector3 velocity     = velocityB - velocityA; // Initial velocity of track B relative to A
  const NMP::Vector3 acceleration = -accelerationA;        // Initial acceleration of track B relative to A

  // Solve cubic polynomial to find the time of closest approach
  const float a = acceleration.magnitudeSquared();                                   // t^3 term
  const float b = 3.0f * velocity.dot(acceleration);                                 // t^2 term
  const float c = 2.0f * (position.dot(acceleration) + velocity.magnitudeSquared()); // t term
  const float d = 2.0f * position.dot(velocity);                                     // constant term

  // Select the root that describes the closest point of approach.
  ComplexNumber roots[3];
  solveCubicPolynomial(a, b, c, d, roots[0], roots[1], roots[2]);
  const float imaginaryComponentEpsilon = 0.01f; // Arbitrary max imaginary part that can be ignored.
  float minDistance = FLT_MAX;
  float t = 0.0f;

  for (uint32_t i=0; i<3; ++i)
  {
    // ignore complex roots
    if(fabs(static_cast<float>(roots[i].i)) < imaginaryComponentEpsilon)
    {
      const float rootTime = NMP::maximum(static_cast<float>(roots[i].r), 0.0f);
      float distance = (position + (velocity * rootTime) + (acceleration * (0.5f * NMP::sqr(rootTime)))).magnitude();

      if (distance < minDistance)
      {
        minDistance = distance;
        t = rootTime;
      }
    }
  }

  ClosestPointOfApproachData cpa;
  cpa.positionA = positionA + velocityA * t + accelerationA * (0.5f * t * t);
  cpa.positionB = positionB + velocityB * t;
  cpa.time = t;
  cpa.dist = minDistance;

#if defined(MR_OUTPUT_DEBUGGING)
  // Draw the trajectory of A.
  const float    timeDelta = 0.1f;
  const uint32_t stepCount = 32;

  for (uint32_t i = 0; i < (stepCount - 1); ++i)
  {
    const float time0 = i * timeDelta;
    const float time1 = time0 + timeDelta;

    const NMP::Vector3 position0 = positionA + (velocityA * time0) + (accelerationA * (time0 * time0 * 0.5f));
    const NMP::Vector3 position1 = positionA + (velocityA * time1) + (accelerationA * (time1 * time1 * 0.5f));

    MR_DEBUG_DRAW_LINE(pDebugDrawInst, position0, position1, NMP::Colour::ORANGE);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, position0, 0.05f, NMP::Colour::ORANGE);
  }

  // Draw the point of closest approach on the trajectory and the position of B at the time of closest approach.
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, cpa.positionA, 0.15f, NMP::Colour::DARK_RED);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, cpa.positionB, 0.15f, NMP::Colour::LIGHT_RED);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, cpa.positionA, cpa.positionB, NMP::Colour::LIGHT_RED);
#endif

  return cpa;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 projectPointOntoLineSegment(const NMP::Vector3& origin, const NMP::Vector3& lineDir, const NMP::Vector3& point, float minDist, float maxDist)
{
  NMP::Vector3 originToPoint = point - origin;
  // length of projection of point direction onto edge:
  float lineProj = originToPoint.dot(lineDir);
  // clamp distance from line origin:
  if (minDist < maxDist)
  {
    lineProj = NMP::clampValue(lineProj, minDist, maxDist);
  }
  return origin + lineProj * lineDir;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 getRainbowColourMapped(float f)
{
  const float dx = 0.8f;
  f = NMP::clampValue(f, 0.0f, 1.0f);
  float k = (6.0f - 2.0f * dx) * f + dx;         //scale f to [dx, 6-dx]
  float r = NMP::maximum(0.0f, (float)(3 - fabs(k - 4) - fabs(k - 5)) / 2);
  float g = NMP::maximum(0.0f, (float)(4 - fabs(k - 2) - fabs(k - 4)) / 2);
  float b = NMP::maximum(0.0f, (float)(3 - fabs(k - 1) - fabs(k - 2)) / 2);
  return NMP::Vector3(r, g, b, 1);
}

//----------------------------------------------------------------------------------------------------------------------
float sinusoidalUnitBlend(float val, float min, float max)
{
  if (val < min)
  {
    return 0.0f;
  }
  else if (val > max)
  {
    return 1.0f;
  }
  else
  {
    float valInUnitInterval = (val - min) / (max - min);  // map to [0, 1]
    float valInCosRange = valInUnitInterval * NM_PI - NM_PI;  // map to [-pi, 0], where cos exhibits increasing sigmoid
    float cosVal = NMP::fastSin(valInCosRange + NM_PI_OVER_TWO);
    return 0.5f + (cosVal / 2.0f);  // scale output to [0, 1]
  }
}

//----------------------------------------------------------------------------------------------------------------------
float linearRampBlend(float val, float start, float end, float startAmpl, float endAmpl)
{
  if (val < start)
  {
    return startAmpl;
  }
  else if (val > end)
  {
    return endAmpl;
  }
  else
  {
    float range = end - start;
    float proportion = (val - start) / range; // position in interval to [0, 1]
    return startAmpl + proportion * (endAmpl - startAmpl);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 getSupportTransformRelRoot(const NMP::Matrix34* legEndRelRoot, const uint32_t numLegs)
{
  NMP_ASSERT(numLegs > 0);
  NMP::Matrix34 supportMatrix;
  supportMatrix.xAxis() = legEndRelRoot[0].yAxis();
  supportMatrix.translation() = legEndRelRoot[0].translation();

  NMP::Vector3 guideVector = legEndRelRoot[0].xAxis();

  for (uint32_t i = 1 ; i < numLegs ; ++i) // Note we've already done the first leg
  {
    supportMatrix.translation() += legEndRelRoot[i].translation();
    supportMatrix.xAxis()       += legEndRelRoot[i].yAxis();
    guideVector                 += legEndRelRoot[i].xAxis();
  }
  supportMatrix.translation() *= 1.0f / numLegs;
  // Note that we don't make it horizontal, since we want everything relative to the root (and the
  // pelvis locater may not be aligned with the feet)
  supportMatrix.xAxis().normalise();
  guideVector.fastNormalise();

  supportMatrix.zAxis() = NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(supportMatrix.xAxis(), guideVector);
  supportMatrix.zAxis().normalise();
  supportMatrix.yAxis() = NMRU::GeomUtils::calculateOrthogonalPlanarVector(supportMatrix.xAxis(), guideVector);
  supportMatrix.yAxis().normalise();

  NMP_ASSERT(supportMatrix.isValidTM(0.05f));
  return supportMatrix;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 getSupportTransformRelRoot(
  const NMP::Matrix34* legEndRelRoot,
  const uint32_t numLegs,
  const NMP::Vector3& upDir)
{
  NMP_ASSERT(numLegs > 0);
  NMP::Matrix34 supportMatrix;
  supportMatrix.translation() = legEndRelRoot[0].translation();
  supportMatrix.xAxis() = legEndRelRoot[0].yAxis();

  for (uint32_t i = 1 ; i < numLegs ; ++i) // Note we've already done the first leg
  {
    supportMatrix.translation() += legEndRelRoot[i].translation();
    supportMatrix.xAxis()       += legEndRelRoot[i].yAxis();
  }

  supportMatrix.translation() *= 1.0f / numLegs;
  // Note that we don't make it horizontal, since we want everything relative to the root (and the
  // pelvis locater may not be aligned with the feet)
  supportMatrix.xAxis().normalise();

  supportMatrix.zAxis() =
    NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(supportMatrix.xAxis(), upDir);
  supportMatrix.zAxis().normalise();
  supportMatrix.yAxis() =
    NMRU::GeomUtils::calculateOrthogonalPlanarVector(supportMatrix.xAxis(), upDir);
  supportMatrix.yAxis().normalise();

  NMP_ASSERT(supportMatrix.isValidTM(0.05f));
  return supportMatrix;
}


}

