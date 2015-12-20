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
#include "SupportPolygonPackaging.h"
#include "SupportPolygon.h"

#include "Types/Environment_SupportPoly.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
static NM_INLINE void sumSupportPoints(
  const SupportPolygonUpdatePackage& pkg,
  const uint32_t start,
  const uint32_t size,
  NMP::Vector3& summedPosition,
  float& summedWeight)
{
    for (uint32_t index = start, end = start + size; index < end; ++index)
  {
    summedPosition += pkg.feedIn->getSupportPoints(index) * pkg.feedIn->getSupportPointsImportance(index);
    summedWeight += pkg.feedIn->getSupportPointsImportance(index);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static NM_INLINE void calculateWeightedSupportPoints(
  const SupportPolygonUpdatePackage& pkg,
  const uint32_t start,
  const uint32_t size,
  const NMP::Vector3& meanSupportPoint,
  NMP::Vector3* supportPointArray,
  uint32_t& supportPointSize)
{
  for (uint32_t index = start, end = start + size; index < end; ++index)
  {
    float w = pkg.feedIn->getSupportPointsImportance(index);
    if (w > 0.0f)
    {
      supportPointArray[supportPointSize++] = pkg.feedIn->getSupportPoints(index) * w + meanSupportPoint * (1.0f - w);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygonUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Some of these points may have low importance - so perhaps first find the weighted
  // mean. Then for each point interpolate between that mean point and the point depending on its
  // importance.
  float totalWeight = 0.0f;
  NMP::Vector3 avPt(NMP::Vector3::InitZero);

  const bool includeArms = in->getIncludeArmSupportPoints();
  const bool includeLegs = in->getIncludeLegSupportPoints();
  const bool includeSpines = in->getIncludeSpineSupportPoints();

  const uint32_t firstLegIndex = 0;
  const uint32_t firstArmIndex = NetworkConstants::networkMaxNumLegs;
  const uint32_t firstSpineIndex = firstArmIndex + NetworkConstants::networkMaxNumArms;

  if (includeLegs)
  {
    sumSupportPoints(*this, firstLegIndex, owner->owner->data->numLegs, avPt, totalWeight);
  }

  if (includeArms)
  {
    sumSupportPoints(*this, firstArmIndex, owner->owner->data->numArms, avPt, totalWeight);
  }

  if (includeSpines)
  {
    sumSupportPoints(*this, firstSpineIndex, owner->owner->data->numSpines, avPt, totalWeight);
  }

  if (feedIn->getFullySupported() == 1.0f)
  {
    totalWeight = NMP::maximum(totalWeight, 1.0f);
  }

  if (totalWeight == 0.0f)
  {
    return; // no points passed up
  }

  avPt /= totalWeight;

  const int32_t maxNumPoints = sizeof(data->polygon.m_corners) / sizeof(data->polygon.m_corners[0]);
  NMP::Vector3 pts[maxNumPoints];
  uint32_t numPoints = 0;

  if (includeLegs)
  {
    calculateWeightedSupportPoints(*this, firstLegIndex, owner->owner->data->numLegs, avPt, pts, numPoints);
  }

  if (includeArms)
  {
    calculateWeightedSupportPoints(*this, firstArmIndex, owner->owner->data->numArms, avPt, pts, numPoints);
  }

  if (includeSpines)
  {
    calculateWeightedSupportPoints(*this, firstSpineIndex, owner->owner->data->numSpines, avPt, pts, numPoints);
  }

  data->polygon.buildConvexHull(&pts[0], numPoints, owner->owner->data->up, feedIn->getFullySupported() == 1.0f);
  out->setPolygon(data->polygon);

#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Colour colour;
  for (uint32_t i = 0 ; i < numPoints ; ++i)
  {
    float c = ((float) i) / numPoints;
    colour.setf(c, c, c);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, pts[i], SCALE_DIST(0.1f), colour);
  }

  for (uint32_t i = 0 ; i < data->polygon.m_numCorners; ++i)
  {
    uint32_t j = (i + 1) % data->polygon.m_numCorners;
    float c = ((float) i) / numPoints;
    colour.setf(c, c, c);
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, data->polygon.m_corners[i], data->polygon.m_corners[j], colour);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)
  NMP::Vector3 closestPoint;
  NMP::Vector3 pos = owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition();
  data->polygon.getDistanceToPoint(pos, owner->owner->data->up, 0.0f, &closestPoint);
  NMP::Vector3 closestPointAtPolygonLevel =
    closestPoint + owner->owner->data->up * (avPt - closestPoint).dot(owner->owner->data->up);
  out->setOffsetFromPolygon(pos - closestPointAtPolygonLevel);

}

//----------------------------------------------------------------------------------------------------------------------
void SupportPolygon::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

