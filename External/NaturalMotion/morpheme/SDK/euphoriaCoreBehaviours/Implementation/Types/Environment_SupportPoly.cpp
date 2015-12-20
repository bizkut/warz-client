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
#include "euphoria/erDebugDraw.h"
#include "Types/Environment_SupportPoly.h"
#include <cstdlib>
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
float Environment::SupportPoly::getDistanceToPoint(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  const float radius,
  NMP::Vector3* closestPointInPolygon) const
{
  if (m_fullySupported) // always inside polygon if fully supported
  {
    if (closestPointInPolygon)
      *closestPointInPolygon = position;
    return 0.0f;
  }
  // We assume points are in a clockwise order viewed along normal
  NMP::Vector3 centre(0, 0, 0);
  for (uint32_t i = 0; i < m_numCorners; i++)
    centre += m_corners[i] / (float)m_numCorners;

  *closestPointInPolygon = position; // default for if it is inside the polygon
  float maxDistance = 0.0f;
  NMP::Vector3 toClosest(0, 0, 0);
  if (m_numCorners > 1)
  {
    for (uint32_t i = 0; i < m_numCorners; i++)
    {
      int j = (i + 1) % m_numCorners;
      NMP::Vector3 outwards = NMP::vNormalise(NMP::vCross(m_corners[j] - m_corners[i], normal));
      float distance = (position - m_corners[i]).dot(outwards);
      if (distance > 0.0f) // is potentially outside the polygon
      {
        NMP::Vector3 start = m_corners[i] + normal * (position - m_corners[i]).dot(normal);
        NMP::Vector3 end = m_corners[j] + normal * (position - m_corners[j]).dot(normal);
        NMP::Vector3 toEnd = end - start;
        float t = NMP::clampValue((position - start).dot(toEnd) / (toEnd.magnitudeSquared() + 1e-10f), 0.0f, 1.0f);
        toClosest = start + toEnd * t - position;
        distance = toClosest.normaliseGetLength();
      }
      distance -= radius;
      if (distance > maxDistance || i == 0)
      {
        maxDistance = distance;
        if (distance > 0)
          *closestPointInPolygon = position + toClosest * distance;
      }
    }
  }
  else if (m_numCorners == 1)
  {
    toClosest = m_corners[0] + normal * (position - m_corners[0]).dot(normal) - position;
    maxDistance = toClosest.normaliseGetLength() - radius;
    if (maxDistance > 0)
      *closestPointInPolygon = position + toClosest * maxDistance;
  }
  else
  {
    maxDistance = 999999.0f;
  }
  return maxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Environment::SupportPoly::getCentre() const
{
  if (m_numCorners == 0)
    return NMP::Vector3(0, 0, 0);
  NMP::Vector3 centre(0, 0, 0);
  for (uint32_t i = 0; i < m_numCorners; i++)
    centre += m_corners[i];
  return centre / (float)m_numCorners;
}

//----------------------------------------------------------------------------------------------------------------------
void Environment::SupportPoly::debugDraw(
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(position),
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(normal),
  const float MR_OUTPUT_DEBUG_ARG(radius),
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(closestPointInPolygon),
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(colour),
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Colour lineColour;
  // Draw n-sided Polygon
  for (uint32_t i = 0; i < m_numCorners; i++)
  {
    int j = (i + 1) % m_numCorners;
    float c = ((float) i) / m_numCorners;
    lineColour.setf(c, c, c);
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, m_corners[i], m_corners[j], lineColour);
  }

  NMP::Vector3 centre(0, 0, 0);
  for (uint32_t i = 0; i < m_numCorners; i++)
    centre += m_corners[i] / (float)m_numCorners;

  NMP::Vector3 oldCorner = m_corners[0] + normal * (centre - m_corners[0]).dot(normal);
  for (uint32_t i = 0; i < m_numCorners; i++)
  {
    int j = (i + 1) % m_numCorners;
    NMP::Vector3 newCorner = m_corners[j] + normal * (centre - m_corners[j]).dot(normal);
    NMP::Vector3 offset = NMP::vNormalise(NMP::vCross(newCorner - oldCorner, normal)) ;
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, oldCorner + offset * radius, newCorner + offset * radius, NMP::Colour(colour, 255));
    int k = (i + 2) % m_numCorners;
    NMP::Vector3 dir = NMP::vNormalise(newCorner - oldCorner);
    NMP::Vector3 newNewCorner = m_corners[k] + normal * (centre - m_corners[k]).dot(normal);
    NMP::Vector3 newLine = newNewCorner - newCorner;
    float cornerAngle = atan2f(-newLine.dot(offset), newLine.dot(dir));
    if (cornerAngle < 0.0f)
      cornerAngle += 2.0f * NM_PI;
    int numCurveSegments = 8;
    NMP::Vector3 oldPos = newCorner + offset * radius;
    for (int l = 0; l < numCurveSegments; l++)
    {
      float angle = cornerAngle * (float)(l + 1) / (float)numCurveSegments;
      NMP::Vector3 newPos = newCorner + dir * sinf(angle) * radius + offset * cosf(angle) * radius;
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, oldPos, newPos, NMP::Colour(colour, 255));
      oldPos = newPos;
    }
    oldCorner = newCorner;
  }

  NMP::Vector3 testPos = position + normal * (centre - position).dot(normal);
  NMP::Vector3 closestPos = closestPointInPolygon + normal * (centre - closestPointInPolygon).dot(normal);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, testPos, closestPos, NMP::Colour(colour, 255));
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, testPos, radius, NMP::Colour(colour, 255));
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
struct GrahamSortVertex
{
  NMP::Vector3 v;
  float        a;
};

//----------------------------------------------------------------------------------------------------------------------
static int zoGrahamSortAnglesFn(const void* a, const void* b)
{
  const GrahamSortVertex* aVert = (GrahamSortVertex*)a;
  const GrahamSortVertex* bVert = (GrahamSortVertex*)b;

  // first criteria is sorting by polar angle
  if (aVert->a < bVert->a)
  {
    return -1;
  }
  if (aVert->a > bVert->a)
  {
    return 1;
  }

  // second is distance from the lowest point (shifted to 0)
  // only happens when points have equal angles, possible when
  // building with grid-snapping
  float distSqA = aVert->v.magnitudeSquared();
  float distSqB = bVert->v.magnitudeSquared();

  if (distSqA < distSqB)
  {
    return -1;
  }
  if (distSqA > distSqB)
  {
    return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// returns:
// >0 if p2 is left of the line through p0 and p1 (ignoring y)
//  0 if p2 is on the line
// >0 if p2 is to the right
static float isPointLeftOfLine(const NMP::Vector3& p0, const NMP::Vector3& p1, const NMP::Vector3& p2, const NMP::Vector3& normal)
{
  float res = NMP::vDot(NMP::vCross(normal, p2 - p1), p0 - p1);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
static float polarAngle(const NMP::Vector3& v, const NMP::Vector3& up, const NMP::Vector3& side)
{
  if (v.magnitudeSquared() == 0.0f)
  {
    return 0.0f;
  }

  NMP::Vector3 fwd = NMP::vCross(side, up); // assume they are orthonormal
  float res = atan2f(NMP::vDot(v, side), NMP::vDot(v, fwd));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
void Environment::SupportPoly::buildConvexHull(const NMP::Vector3* points, int32_t nPoints, const NMP::Vector3& up, bool fullySupported)
{
  m_numCorners = 0;
  m_fullySupported = fullySupported;
  if (nPoints <= 2)
  {
    m_numCorners = nPoints;
    for (int32_t i = 0 ; i < nPoints ; ++i)
      m_corners[i] = points[i];
    return;
  }

  const int maxNumCorners = sizeof(m_corners) / sizeof(m_corners[0]);
  NMP_ASSERT(nPoints <= maxNumCorners);
  GrahamSortVertex vSort[maxNumCorners];

  NMP::Vector3 side(1, 0, 0); // don't worry this is direction independent
  if (up.x > 0.5f)
  {
    side.set(0, 0, 1);
  }
  {
    side = NMP::vNormalise(NMRU::GeomUtils::calculateOrthogonalPlanarVector(up, side));
  }

  // go find a point most to one side - i.e. definitely on the hull
  int32_t sideIndex = 0;
  float lowDist = vDot(points[sideIndex], side);
  for (int32_t i = 1; i < nPoints; i++)
  {
    float dist = vDot(points[i], side);
    if (dist < lowDist)
    {
      sideIndex = i;
      lowDist = vDot(points[sideIndex], side);
    }
  }

  // create an array to sort; copy across the input vertices, calculate their polar angles against the side point while
  // we do so
  vSort[0].v = points[sideIndex];
  for (int32_t i = 0, j = 1; i < nPoints; ++i)
  {
    if (sideIndex != i)
    {
      vSort[j].v = points[i] - vSort[0].v;
      vSort[j].a = polarAngle(vSort[j].v, up, side);
      ++j;
    }
  }

  // sort the vertices based on the angles; we then have an ordered fan of vertices, the base of the fan being the point
  // furthest to the side found
  qsort(&vSort[1], nPoints - 1, sizeof(GrahamSortVertex), zoGrahamSortAnglesFn);

  for (int32_t i = 1; i < nPoints; i++)
  {
    vSort[i].v += vSort[0].v;
  }

  m_corners[0] = vSort[0].v;
  m_corners[1] = vSort[1].v;

  // iterate around the fan of vertices, only accepting the next iteration if it is a "left turn" from one vertex to
  // another.
  int32_t vOutIdx = 2;
  int32_t i = 2;
  while (i < nPoints)
  {
    if (vOutIdx > 1)
    {
      if (isPointLeftOfLine(m_corners[vOutIdx - 2], m_corners[vOutIdx - 1], vSort[i].v, up) > 0.0f)
      {
        m_corners[vOutIdx] = vSort[i++].v;
        ++vOutIdx;
      }
      else
      {
        --vOutIdx;
      }
    }
    else
    {
      m_corners[vOutIdx].set(vSort[i++].v);
      ++vOutIdx;
    }
  }

  m_numCorners = vOutIdx;
}

//----------------------------------------------------------------------------------------------------------------------
float Environment::SupportPoly::getExtentsInDirection(const NMP::Vector3& direction) const
{
  if (m_numCorners <= 1)
  {
    return 0.0f;
  }

  float minD = m_corners[0].dot(direction);
  float maxD = minD;
  for (uint32_t iCorner = 1 ; iCorner < m_numCorners ; ++iCorner)
  {
    float d = m_corners[iCorner].dot(direction);
    if (d < minD)
    {
      minD = d;
    }
    if (d > maxD)
    {
      maxD = d;
    }
  }

  return (maxD - minD);
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
