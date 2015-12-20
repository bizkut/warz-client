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
#include "euphoria/erDimensionalScaling.h"
#include "Types/Environment_Patch.h"
#include "Types/Environment_LocalShape.h"
#include "Types/Environment_CollideResult.h"
#include "Types/SphereTrajectory.h"
#include "euphoria/erCollisionProbes.h"
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

extern int cNumberOfFacesOrCapsuleEdges[Environment::Patch::EO_Max];
int cNumberOfFaces[Environment::Patch::EO_Max] =  { -1, 3, 2, 1, 0, 0, 1 };
int cNumberOfEdges [Environment::Patch::EO_Max] = { -1, 3, 1, 0, 1, 0, 0 };

// Just guard against completely un-normalised values - don't require perfection
static const float NORMAL_ERROR_LIMIT = 0.01f;
// Epsilon avoids divide by zero.
static const float EPSILON = 1e-10f;

//----------------------------------------------------------------------------------------------------------------------
// returns the nearest point in the aabb represented by just the vector extents
NM_INLINE bool nearestPointOnBox(NMP::Vector3& vecLocal, const NMP::Vector3& halfExtents)
{
  NMP::Vector3 vec;
  vec.x = (vecLocal.x > halfExtents.x) ? halfExtents.x : ((vecLocal.x < -halfExtents.x) ? -halfExtents.x : vecLocal.x);
  vec.y = (vecLocal.y > halfExtents.y) ? halfExtents.y : ((vecLocal.y < -halfExtents.y) ? -halfExtents.y : vecLocal.y);
  vec.z = (vecLocal.z > halfExtents.z) ? halfExtents.z : ((vecLocal.z < -halfExtents.z) ? -halfExtents.z : vecLocal.z);
  vec.w = 0.0f;

  bool outside = (vecLocal.x != vec.x) || (vecLocal.y != vec.y) || (vecLocal.z != vec.z);
  vecLocal = vec;
  return outside;
}

//----------------------------------------------------------------------------------------------------------------------
Environment::Patch::Patch()
{
  corner.setToZero();

  for (size_t i = 0 ; i < sizeof(faceNormals) / sizeof(faceNormals[0]) ; i++)
  {
    faceNormals[i].setToZero();
  }

  knownContactPoint.setToZero();

  for (size_t i = 0 ; i < sizeof(edgeLengths) / sizeof(edgeLengths[0]) ; ++i)
  {
    edgeLengths[i] = 0.0f;
  }
  numKnownEdgeLengths = 0;

  radius = 0.0f;
  type = EO_NoContact;
  isPredicted = false;
}

//----------------------------------------------------------------------------------------------------------------------
// collides the path against the patch, the final bool allows the function to approximate the
// angular velocity of the patch by adjusting the path the adjustment is quite costly, but only done
// on rotating objects
bool Environment::Patch::sphereTrajectoryCollides(
  const SphereTrajectory& traj, 
  const NMP::Vector3& lastNormal, 
  CollideResult& result,
  const ER::DimensionalScaling& dimensionalScaling, 
  bool adjustForAngularVelocity) const
{
  const float radiusBailoutEpsilon = 0.01f; // 1 percent accuracy
  result.time = 0.0f;
  SphereTrajectory path = traj;
  if (adjustForAngularVelocity)
  {
    state.adjustPathForAngularVelocity(path);
  }

  // line below, perhaps do a state.sphereTrajectoryCollides instead,
  // it is slower but a tighter check and it should give a better initial guess for the iterative scheme that follows
  if (!state.boundingBoxCull(path)) // attempted speedup by early out. Note this may be mostly redundant if this collides function is only called on recently cull tested objects
  {
    return false; // not contacting
  }

  NMP::Vector3 edgeTangents[3];
  getEdgeTangents(edgeTangents);

  NMP::Vector3 userPos  = path.position;
  NMP::Vector3 localVel = path.velocity - state.velocity;
  NMP::Vector3 localAcc = path.acceleration - state.acceleration;

  float approachAcc = 0.0f;
  NMP::Vector3 planeIntersection = userPos + lastNormal * dimensionalScaling.scaleDist(1000.0f); // ie if lastNormal is zero, we start at the user pos
  NMP::Vector3 nearestPoint;

  // Here I use a simple iterative scheme
  // The scheme is as follows, starting with a guess at the intersection normal, we compute the intersection
  // with this object, which produces a closer approximation to the intersection normal
  float pathRadius = 0.0f;
  float lastT = 0.0f;
  float distSqr = 0.0f;
  const float squareTotalRadius = NMP::sqr(radius + path.radius) * (1.0f + radiusBailoutEpsilon);
  int i = 0;
  const int maxIterations = 4;
  for (; i < maxIterations; i++)
  {
    // find nearest point on box first
    // This epsilon addition basically allows 0 radius sweeps
    float epsilon = 1e-4f;
    pathRadius = path.radius + epsilon;
    lastT = result.time;
    // Find the nearest point on this patch to the plane intersection point.
    bool outside = getNearestPointInside(nearestPoint, planeIntersection, edgeTangents, false);

    // Find the nearest point on the state AABB to the nearest point on the patch found above.
    nearestPoint -= state.aabb.getCentre();
    result.clippedToBoundingBox = nearestPointOnBox(nearestPoint, state.aabb.getHalfExtents());
    nearestPoint += state.aabb.getCentre();

    // When inside shape already, nearestPoint is planeIntersection.
    // In order to calculate result normal, make nearestPoint to be patch position.
    if (!result.clippedToBoundingBox && !outside)
    {
      nearestPoint = state.position;
    }

    result.normal = NMP::vNormalise(planeIntersection - nearestPoint);
    result.approachSpeed = -localVel.dot(result.normal);
    result.distance = (userPos - nearestPoint).dot(result.normal);

    if (!result.clippedToBoundingBox && !outside)
    {
      // inside shape already
      result.impactVel      = localVel;
      result.impactPosition = nearestPoint;
      result.impactSpeed    = result.approachSpeed;
      result.time           = 0.0f;
      result.timeMetric     = 0.0f;
      result.distance       = 0.0f;
      result.initialOverlap = true;

      return true;
    }

    // next find the nearest point on the actual patch geometry
    approachAcc = -localAcc.dot(result.normal);
    result.time = SphereTrajectory::getCollidePlaneTime(result.distance, result.approachSpeed, approachAcc, pathRadius);
    if (result.time < 0.0f || result.time > path.maxTime)
    {
      return false; // no collision happens with this sphere
    }
    planeIntersection = userPos + localVel * result.time + 0.5f * localAcc * NMP::sqr(result.time);
    // if the current calculated time gets with radiusBailoutEpsilon+1 of the sphere radius then we are close enough so bail out
    distSqr = (planeIntersection - nearestPoint).magnitudeSquared();
    if (distSqr < squareTotalRadius)
      break;
  }
  if (i == maxIterations && fabsf(lastT - result.time) > 0.1f * path.maxTime)
  {
    return false; // ie it hasn't converged.. may want to increase number of iterations if this happens a lot
  }

  result.distance = NMP::maximum(result.distance - pathRadius, 0.0f); // gives 0 if intersecting at the start
  result.impactSpeed = result.approachSpeed + approachAcc * result.time;
  result.impactVel = localVel + localAcc * result.time;
  result.impactPosition = nearestPoint;

  // Override the normal for planes as we want to avoid strange normals for internal edges
  if (type == EO_Plane || type == EO_ContactPlane)
  {
    result.normal = faceNormals[0];
  }
  return true;
}
// returns a metric of how reliable a resulting position is
float Environment::Patch::getReliabilityOfPosition(const NMP::Vector3& position, const NMP::Vector3& pathPosition, bool clipped) const
{
  float errorScale = (clipped || type == EO_ContactPlane) ? 3.0f : 1.0f; // larger distance for less reliable situations (contact plane is low quality)

  // time metric is the time to impact but scaled so less confident impacts (far from the known contact point) are given a longer time
  // i.e. it is a common metric for us to utilise
  float toKnownContactSquared = (position - knownContactPoint).magnitudeSquared();
  float toStartSquared = (position - pathPosition).magnitudeSquared();
  return (1.0f + errorScale * (toKnownContactSquared / (toStartSquared + EPSILON))); // Epsilon avoids divide by zero.
}

//----------------------------------------------------------------------------------------------------------------------
int32_t Environment::Patch::getNormal1Index(int32_t edgeIndex) const
{
  if (cNumberOfFaces[type] > 0)
  {
    return edgeIndex;
  }
  return -1;
}
//----------------------------------------------------------------------------------------------------------------------
int32_t Environment::Patch::getNormal2Index(int32_t edgeIndex) const
{
  if (cNumberOfFaces[type] > 0)
  {
    // backwards wrap around
    edgeIndex -= 1;
    if (edgeIndex < 0)
    {
      return cNumberOfFaces[type] - 1;
    }
    else
    {
      return edgeIndex;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t Environment::Patch::getEdgeDirections(NMP::Vector3* edgeDirections) const
{
  getEdgeTangents(edgeDirections);
  for (int i = 0; i < cNumberOfEdges[type]; i++)
  {
    edgeDirections[i].normalise();
  }
  return cNumberOfEdges[type];
}

//----------------------------------------------------------------------------------------------------------------------
void Environment::Patch::getEdgeTangents(NMP::Vector3* edgeTangents) const
{
  // edge tangents initialised to zero as code below doesn't write to all three
  for (int i = 0; i < 3; i++)
  {
    edgeTangents[i].setToZero();
  }

  if (type == EO_Capsule)
  {
    // for compactness we store the capsule 'to end' vector in the faceNormal for this special case
    edgeTangents[0] = faceNormals[0];
  }
  else
  {
    // create the edge tangents from the face normals
    int numEdges = cNumberOfEdges[type];
    int j = cNumberOfFaces[type] - 1;

    for (int i = 0; i < numEdges; j = i, i++)
      edgeTangents[i] = NMP::vCross(faceNormals[i], faceNormals[j]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Function is a publicly exposed wrapper for the private function getNearestPointInside.
bool Environment::Patch::getNearestPointInside(const NMP::Vector3& pointWorld, NMP::Vector3& nearestPoint, bool getSurfacePoint) const
{
  NMP::Vector3 edgeTangents[3];
  getEdgeTangents(edgeTangents);
  return getNearestPointInside(nearestPoint, pointWorld, edgeTangents, getSurfacePoint);
}

//----------------------------------------------------------------------------------------------------------------------
// returns 0 if already inside the shape, -1 if clamps to box, 1 if usual
// if getSurfacePoint is true it returns the point on the surface, otherwise the nearest point inside the surface
int32_t Environment::Patch::nearestPoint(const NMP::Vector3& pointWorld, NMP::Vector3& nearestPoint, bool getSurfacePoint) const
{
  // first get the point on the patch geometry
  bool outsidePatch = getNearestPointInside(pointWorld, nearestPoint, getSurfacePoint);

  // now check whether it is outside the box, simultaneously get the nearest point on the box
  nearestPoint -= state.aabb.getCentre();
  bool outsideBox = nearestPointOnBox(nearestPoint, state.aabb.getHalfExtents());
  nearestPoint += state.aabb.getCentre();

  if (!outsidePatch && !outsideBox)
  {
    if (!getSurfacePoint)
    {
      nearestPoint = pointWorld;
    }
    return 0;
  }
  return outsideBox ? -1 : 1;
}

//----------------------------------------------------------------------------------------------------------------------
// Sets nearestPoint to either the nearest point in the shape volume or the nearest point on the
// surface depending on the getSurfacePoint bool. returns false if already inside the shape. 
bool Environment::Patch::getNearestPointInside(
  NMP::Vector3& nearestPoint, 
  const NMP::Vector3& planeIntersectionPoint, 
  const NMP::Vector3* edgeTangents, 
  bool getSurfacePoint) const
{
  const NMP::Vector3 pos = planeIntersectionPoint - corner; // do everything local to the shape

  // find nearest point on this shape made of several faces
  float dots[3] = { 0, 0, 0 };

  // 1. check if a face has the closest point
  int largestI = 0;
  const int numFaces = cNumberOfFaces[type];
  for (int i = 0; i < numFaces; i++)
  {
    NMP::Vector3 cross = NMP::vCross(pos, faceNormals[i]);
    dots[i] = pos.dot(faceNormals[i]);
    if (dots[i] >= 0.0f && cross.dot(edgeTangents[i]) >= 0.0f && cross.dot(edgeTangents[(i+1)%numFaces]) <= 0.0f)
    {
      nearestPoint = planeIntersectionPoint - faceNormals[i] * dots[i]; // we'd like this first early-out to be the most common case
      return true;
    }
    if (dots[i] > dots[largestI])
      largestI = i;
  }
  // and return false if inside these planes
  if (numFaces && dots[largestI] < 0.0f)
  {
    nearestPoint = planeIntersectionPoint;
    if (getSurfacePoint)
      nearestPoint -= faceNormals[largestI] * dots[largestI];
    return false; // we're inside the shape
  }

  // 2. check if an edge has the closest point
  const int numEdges = cNumberOfEdges[type];
  float biggestLength = 0.0f;
  int biggestI = 0;
  for (int i = 0; i < numEdges; i++)
  {
    float length = pos.dot(edgeTangents[i]);
    if (length > biggestLength)
    {
      biggestLength = length;
      biggestI = i;
    }
  }
  float edgeSqr = edgeTangents[biggestI].magnitudeSquared();
  if (type == EO_Capsule && biggestLength > edgeSqr) // this line clamps capsules to the far end length, relies on edgeTangents[0] being vector to far side
  {
    biggestLength = edgeSqr;
  }

  // Epsilon avoids divide by zero.
  nearestPoint = corner + biggestLength * (edgeTangents[biggestI] / (edgeSqr + EPSILON)); // note, the corner case happens naturally if no edge has a t>0

  NMP_ASSERT_MSG(((type == EO_Capsule || type == EO_Sphere) || !radius), "Only a sphere or a capsule patch type can have a nonzero radius.");

  // Inflate with radius if patch type is a sphere or a capsule.
  if (type == EO_Capsule || type == EO_Sphere)
  {
    const NMP::Vector3 nearestPointToPlaneIntersectionPoint = planeIntersectionPoint - nearestPoint;
    const float intersectDistanceFromCenter = nearestPointToPlaneIntersectionPoint.magnitude();
    const bool isOutsideShape = intersectDistanceFromCenter > radius;
    const float nearestPointDistanceFromCenter =
      (getSurfacePoint || isOutsideShape) ? radius : intersectDistanceFromCenter;
    // Epsilon avoids divide by zero.
    nearestPoint += nearestPointToPlaneIntersectionPoint * (nearestPointDistanceFromCenter / (intersectDistanceFromCenter + EPSILON));
    return isOutsideShape;
  }

#if NM_CALL_VALIDATORS
  NM_VALIDATOR(Vector3Valid(nearestPoint), "nearestPoint");
#endif
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// this updates the position, velocity and orientation of the patch based on its known angular vel and acceleration
void Environment::Patch::update(float timeStep)
{
  knownContactPoint -= state.position;
  corner -= state.position;
  state.velocity += state.acceleration * timeStep;
  state.position += state.velocity * timeStep;
  state.aabb.move(state.velocity * timeStep); // we don't know what will happen to the bounding box so we have to assume it'll move with the position

  // now update from the angular velocity
  NMP::Quat rotation(state.angularVelocity * timeStep, false);
  rotation.normalise();
  NMP::Matrix34 rot;
  rot.fromQuat(rotation);

  knownContactPoint.rotate(rot);
  corner.rotate(rot);
  knownContactPoint += state.position;
  corner += state.position;
  for (int i = 0; i < cNumberOfFacesOrCapsuleEdges[type]; i++)
  {
    faceNormals[i].rotate(rot);
    NMP_ASSERT(faceNormals[i].isNormal(NORMAL_ERROR_LIMIT));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This function converts the sweep result into a format that allows for fast collision testing
// Note that environment objects could be built in other ways, not just from sweep results
// For example, they could be generated by level markup, for poles or ledges etc.
void Environment::Patch::updateFromSweepResult(
  const ER::SweepResult& sweep, 
  const ER::DimensionalScaling& dimensionalScaling)
{
  NMP_ASSERT(sweep.getShapeID() == state.shapeID); // sweep result being applied to the wrong object
  numKnownEdgeLengths = 0;
  radius = 0.0f;
  switch (sweep.getType())
  {
  case ER::SweepResult::SR_NoContact:
  {
    type = EO_NoContact;
    break;
  }

  case ER::SweepResult::SR_Triangle:
  {
    NMP::Vector3 triangleEdges[3];
    faceNormals[0] = sweep.getTriangleNormal();
    NMP_ASSERT(faceNormals[0].isNormal(NORMAL_ERROR_LIMIT));
    int numFaces = 1;
    int closestEdgeIndex = 0;
    float closestEdgeDistanceSq = 1e10f;
    for (int i = 0; i < 3; i++)
    {
      triangleEdges[i] = NMP::vNormalise(sweep.getVertex((i + 1) % 3) - sweep.getVertex(i));
      NMP::Vector3 posOnEdge = sweep.getVertex(i) + 
        triangleEdges[i] * (sweep.getContactPoint() - sweep.getVertex(i)).dot(triangleEdges[i]);
      // This ensures we're starting with the closest edge (which speeds up collision detection later down the line)
      float distanceToEdgeSq = (sweep.getContactPoint() - posOnEdge).magnitudeSquared();
      if (distanceToEdgeSq < closestEdgeDistanceSq)
      {
        closestEdgeDistanceSq = distanceToEdgeSq;
        closestEdgeIndex = i;
      }
    }
    int edgeI = -1;
    int unusedEdge = closestEdgeIndex + 2;
    int unused = -1;
    int l = 0;
    for (int j = closestEdgeIndex; j < closestEdgeIndex + 3; j++)
    {
      int edgeIndex = j % 3;
      float bend = NMP::vCross(sweep.getTriangleNormal(), sweep.getNeighbourNormal(edgeIndex)).dot(triangleEdges[edgeIndex]);
      // Disallow concave or flat edges - concave results in bad corner patches being generated
      if (bend < 0.01f) 
      {
        unused = l;
        unusedEdge = edgeIndex;
        continue; // we cut out edges with little bend
      }
      l++;
      faceNormals[numFaces++] = NMP::vNormalise(sweep.getNeighbourNormal(edgeIndex));
      NMP_ASSERT(faceNormals[numFaces-1].isNormal(NORMAL_ERROR_LIMIT));
      edgeLengths[numKnownEdgeLengths++] = (sweep.getVertex((edgeIndex + 1) % 3) - sweep.getVertex(edgeIndex)).magnitude();
      edgeI = edgeIndex;
      if (numFaces == 3) // max allowed in our shape
        break;
    }
    // if we are deleting the middle triangle then that would make the handedness reversed so we
    // have to swap the ones either side around
    if (numFaces == 3 && unused == 1) 
    {
      NMP::nmSwap(faceNormals[1], faceNormals[2]);
      NMP_ASSERT(faceNormals[1].isNormal(NORMAL_ERROR_LIMIT));
      NMP_ASSERT(faceNormals[2].isNormal(NORMAL_ERROR_LIMIT));
      NMP::nmSwap(edgeLengths[0], edgeLengths[1]);
    }
    if (numKnownEdgeLengths == 0)
    {
      corner = sweep.getContactPoint();
    }
    else if (numKnownEdgeLengths == 1)
    {
      corner = sweep.getVertex(edgeI);
    }
    else // 2 edges so far, this means a 3rd edge needs adding
    {
      // Note that the edge lengths calculated above are the lengths of the edges of the triangle,
      // skipping the missing one. However, what needs to be stored are the edge lengths of the
      // boundaries between each of the three faces. We don't know one of these edge lengths, so we
      // make up a value that is large enough to use if necessary, but not so large that it will
      // generate artefacts (e.g. if used for grabbing).
      corner = sweep.getVertex((unusedEdge + 2) % 3);
      edgeLengths[2] = edgeLengths[1];
      edgeLengths[1] = dimensionalScaling.scaleDist(0.5f);

      float t = edgeLengths[0];
      edgeLengths[0] = edgeLengths[2];
      edgeLengths[2] = edgeLengths[1];
      edgeLengths[1] = t;
    }

    // Mesh must be static... but is likely very big. Recalculate the extents just around the
    // triangle of interest.
    state.aabb.setEmpty();
    state.aabb.addPoint(sweep.getVertex(0));
    state.aabb.addPoint(sweep.getVertex(1));
    state.aabb.addPoint(sweep.getVertex(2));

    // now work out the type of shape. Note numFaces cannot be 0 here
    if (numFaces == 1)
      type = EO_Plane;
    else if (numFaces == 2)
      type = EO_Edge;
    else if (numFaces == 3)
      type = EO_Corner;
    break;
  }
  case ER::SweepResult::SR_ConvexCorner: // just a corner and 3 normals
  {
    // this is quite easy
    corner = sweep.getCCCorner();
    for (int i = 0; i < 3; i++)
    {
      faceNormals[i] = sweep.getCCFaceNormal(i);
      NMP_ASSERT(faceNormals[i].isNormal(NORMAL_ERROR_LIMIT));
      edgeLengths[i] = sweep.getCCEdgeLength(i);
    }
    type = EO_Corner;
    break;
  }
  case ER::SweepResult::SR_Box:
  {
    // OK we find the closest corner and represent that in our shape
    type = EO_Corner;

    // 1. find the octant that the contact point is in, and find the closest face
    NMP::Vector3 hitPoint = sweep.getContactPoint() - sweep.getBoxMatrix().translation();
    float dirs[3];
    float dots[3];
    int biggestI = 0;
    float biggestDot = -1e10f;
    for (int i = 0; i < 3; i++)
    {
      dots[i] = hitPoint.dot(sweep.getBoxMatrix().r[i]);
      dirs[i] = dots[i] > 0 ? 1.0f : -1.0f;
      if (dots[i] - sweep.getBoxExtents()[i] > biggestDot) // This finds the closest face
      {
        biggestDot = dots[i] - sweep.getBoxExtents()[i];
        biggestI = i;
      }
    }

    // 2. store the 3 faces, the 3 edges and generate the corner vector
    corner = sweep.getBoxMatrix().translation();
    numKnownEdgeLengths = 3;
    for (int i = 0; i < 3; i++)
    {
      int j = (i + biggestI) % 3;
      corner += sweep.getBoxMatrix().r[j] * dirs[j] * sweep.getBoxExtents()[j];
      edgeLengths[i] = 2.0f * sweep.getBoxExtents()[(j+1)%3];
      faceNormals[i] = sweep.getBoxMatrix().r[j] * dirs[j];
      NMP_ASSERT(faceNormals[i].isNormal(NORMAL_ERROR_LIMIT));
    }

    if (dirs[0]*dirs[1]*dirs[2] < 0.0f) // we need to change the handedness (which is implied in the collision tests)
    {
      // If we swap the face normals around we must swap the edge lengths around equivalently
      // but the edges have a different indexing so we swap 0 and 1 around
      NMP::nmSwap(faceNormals[1], faceNormals[2]);
      NMP_ASSERT(faceNormals[1].isNormal(NORMAL_ERROR_LIMIT));
      NMP_ASSERT(faceNormals[2].isNormal(NORMAL_ERROR_LIMIT));
      NMP::nmSwap(edgeLengths[0], edgeLengths[1]);
    }
    break;
  }

  case ER::SweepResult::SR_Sphere:
  {
    type = EO_Sphere;
    corner = sweep.getSpherePosition();
    radius = sweep.getSphereRadius();
    break;
  }

  case ER::SweepResult::SR_Capsule:
  {
    // We can only represent the whole capsule either way around but for paranoia and legacy reasons we pick the closest point as the corner
    type = EO_Capsule;
    NMP::Vector3 capsuleCentre = (sweep.getCapsuleEnd() + sweep.getCapsuleStart()) * 0.5f;
    NMP::Vector3 toEnd = sweep.getCapsuleEnd() - sweep.getCapsuleStart();
    if ((sweep.getContactPoint() - capsuleCentre).dot(toEnd) > 0.0f)
    {
      corner = sweep.getCapsuleEnd();
      faceNormals[0] = -NMP::vNormalise(toEnd);
      NMP_ASSERT(faceNormals[0].isNormal(NORMAL_ERROR_LIMIT));
    }
    else
    {
      corner = sweep.getCapsuleStart();
      faceNormals[0] = NMP::vNormalise(toEnd);
      NMP_ASSERT(faceNormals[0].isNormal(NORMAL_ERROR_LIMIT));
    }
    radius = sweep.getCapsuleRadius();
    edgeLengths[0] = sweep.getCapsuleLength();
    break;
  }

  case ER::SweepResult::SR_Plane:
  {
    type = EO_Plane;
    corner = sweep.getPlanePosition();
    faceNormals[0] = sweep.getPlaneNormal();
    NMP_ASSERT(faceNormals[0].isNormal(NORMAL_ERROR_LIMIT));
    break;
  }

  case ER::SweepResult::SR_ContactPlane:
  {
    type = EO_ContactPlane;
    corner = sweep.getPlanePosition();
    faceNormals[0] = sweep.getPlaneNormal();
    NMP_ASSERT(faceNormals[0].isNormal(NORMAL_ERROR_LIMIT));
    break;
  }

  default:
  {
    NMP_ASSERT_FAIL(); // shouldn't get here
    return;
  }
  }

  NMP_ASSERT(type <= ER::SweepResult::SR_ContactPlane && type > ER::SweepResult::SR_NoContact);
  knownContactPoint = sweep.getContactPoint();
}

//----------------------------------------------------------------------------------------------------------------------
// returns true if this object is connected to the argument object
bool Environment::Patch::isConnectedTo(const Patch& object, const Environment::LocalShape& shape, const Environment::LocalShape& objectShape, float epsilon)
{
  if (state.shapeID != object.state.shapeID)
  {
    // they're not the same object at all
    return false;
  }
  // this is quite a coarse early-out, but it saves time. It says, if the object is dynamic (meaning it is a convex mesh)
  // then just assume it is connected to previous patches, i.e. just use the 1 most recent patch on this object as a best guess.
  if (!state.isStatic)
    return true;
  // if they are on the same object and the patch represents the whole object, then they must be connected
  if (type > 3 || object.type > 3)
    return true;

  return shape.isConnectedTo(objectShape, epsilon);
}

//----------------------------------------------------------------------------------------------------------------------
// returns true if this object is connected to the argument object, ie if they share part of their shape
bool Environment::LocalShape::isConnectedTo(const Environment::LocalShape& object, float epsilon) const
{
  // if corner points are close enough then shapes are connected
  float epsilonSqr = NMP::sqr(epsilon);
  float distSqrToCorner = (corner - object.corner).magnitudeSquared();
  if (distSqrToCorner < epsilonSqr)
  {
    return true; // they share a corner
  }

  // this compares the two shapes for similarity based on their face normals
  for (int i = 0; i < cNumberOfFaces[type]; i++)
  {
    for (int j = 0; j < cNumberOfFaces[object.type]; j++)
    {
      float distToCornerSqr = NMP::sqr(faceNormals[i].dot(corner) - object.faceNormals[j].dot(object.corner));
      if (distToCornerSqr < epsilonSqr)
      {
        float normalDifferenceSqr = (faceNormals[i] - object.faceNormals[j]).magnitudeSquared();
        if (normalDifferenceSqr < epsilonSqr)
        {
          return true; // they share a face, perhaps
        }
      }
    }
  }
  // special case for planes, we want to always update to the latest plane regardless of whether the planes are close... to prevent excess of contact planes really
  if (type == Patch::EO_ContactPlane)
  {
    if (object.type == Patch::EO_ContactPlane)
      return true;
    // this tells us whether the plane passes through the object patch
    float distToCornerSqr = NMP::sqr((corner - object.corner).dot(faceNormals[0]));
    if (distToCornerSqr < epsilonSqr)
    {
      return true;
    }
  }
  // special case comparison for capsule vs capsule, comparing end pos and direction vectors
  if (type == Patch::EO_Capsule && object.type == Patch::EO_Capsule)
  {
    if (NMP::vCross(faceNormals[0], object.faceNormals[0]).magnitudeSquared() < epsilonSqr)
    {
      NMP::Vector3 origin1 = corner - faceNormals[0] * faceNormals[0].dot(corner);
      NMP::Vector3 origin2 = object.corner - object.faceNormals[0] * object.faceNormals[0].dot(object.corner);
      if ((origin1 - origin2).magnitudeSquared() < epsilonSqr)
      {
        return true;
      }
    }
  }
  return false;
}
void Environment::Patch::createAsPlane(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float r,
  const NMP::Vector3& velocity,
  const NMP::Vector3& angularVelocity,
  float mass,
  int64_t shapeID)
{
  // note, state constructor covers acceleration and spinAverageTimeLength
  // initialise state
  state.shapeID = shapeID;
  state.position = position;
  state.aabb.setCentreAndHalfExtents(position, NMP::Vector3(r, r, r));
  state.velocity = velocity;
  state.angularVelocity = angularVelocity;
  state.mass = mass;
  state.accSqr = 0.0f;

  // initialise patch to a plane
  corner = position;
  type = EO_Plane;
  knownContactPoint = position;
  faceNormals[0] = normal;
  NMP_ASSERT(faceNormals[0].isNormal(NORMAL_ERROR_LIMIT));
  numKnownEdgeLengths = 0;
  radius = 0.0f; // this is an expansion radius so isn't needed for a plane
}

void Environment::Patch::createAsSphere(
  const NMP::Vector3& position,
  float r,
  const NMP::Vector3& velocity,
  float mass,
  int64_t shapeID)
{
  // note, state constructor covers acceleration and spinAverageTimeLength
  // initialise state
  state.shapeID = shapeID;
  state.position = position;
  state.aabb.setCentreAndHalfExtents(position, NMP::Vector3(r, r, r));
  state.velocity = velocity;
  state.mass = mass;
  state.accSqr = 0.0f;

  // initialise patch to a plane
  corner = position;
  type = EO_Sphere;
  knownContactPoint = position;
  numKnownEdgeLengths = 0;
  radius = r;
}
//----------------------------------------------------------------------------------------------------------------------
void Environment::Patch::debugDraw(
  float MR_OUTPUT_DEBUG_ARG(size),
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const ER::DimensionalScaling& MR_OUTPUT_DEBUG_ARG(dimensionalScaling)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, knownContactPoint, 0.04f * size, NMP::Colour::DARK_TURQUOISE);

  // Firstly draw the surrounding box
  {
    NMP::Vector3 pos = state.aabb.getCentre();
    NMP::Vector3 x(state.aabb.getHalfExtents().x, 0, 0);
    NMP::Vector3 y(0, state.aabb.getHalfExtents().y, 0);
    NMP::Vector3 z(0, 0, state.aabb.getHalfExtents().z);

    NMP::Colour colour = NMP::Colour::WHITE;
    if (type == EO_NoContact)
    {
      colour = NMP::Colour::RED;
    }

    MR_DEBUG_DRAW_BOX(pDebugDrawInst, pos, state.aabb.getHalfExtents(), colour);
  }

  NMP::Vector3 edgeTangents[3];
  getEdgeTangents(edgeTangents);
  float lengths[3];
  for (int i = 0; i < 3; i++)
  {
    edgeTangents[i].normalise();
    if (edgeLengths[i] == 0.0f) // sometimes edge lengths are 0 but we still want to draw the triangle here
      lengths[i] = size*0.05f;
    else
      lengths[i] = edgeLengths[i];
  }

  // Now draw the shape, this is tricky, not only are there several variations of shape but any
  // shape can be inflated by a radius (very hard to visualise without 3d curve rendering) anyway,
  // lets give it a go..
  if (type == EO_Corner) // draw ends to make parallelogram
  {
    for (int i = 0; i < 3; i++)
    {
      MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, 
        corner, 
        corner + edgeTangents[i]*lengths[i], 
        corner + edgeTangents[(i+1)%3]*lengths[(i+1)%3], 
        i == 0 ? NMP::Colour::DARK_ORANGE : NMP::Colour::LIGHT_ORANGE);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, corner, 
        faceNormals[i] * dimensionalScaling.scaleDist(1.0f), i == 0 ? NMP::Colour::DARK_YELLOW : NMP::Colour::YELLOW);
    }
  }
  else if (type == EO_Edge) // draw 2 squares
  {
    NMP::Vector3 side1 = NMP::vCross(faceNormals[0], edgeTangents[0]) * 0.5f * lengths[0];
    NMP::Vector3 side2 = -NMP::vCross(faceNormals[1], edgeTangents[0]) * 0.5f * lengths[0];
    // project knownContactPoint onto edge tangent:
    NMP::Vector3 midPos = corner + edgeTangents[0] * (knownContactPoint - corner).dot(edgeTangents[0]);
    NMP::Vector3 midPos2 = midPos + side1;
    NMP::Vector3 midPos3 = midPos + side2;
    MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, midPos - edgeTangents[0]*size, midPos2 - edgeTangents[0]*size,
      midPos2 + edgeTangents[0]*size, NMP::Colour::DARK_ORANGE);
    MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, midPos - edgeTangents[0]*size, midPos2 + edgeTangents[0]*size,
      midPos + edgeTangents[0]*size, NMP::Colour::DARK_ORANGE);
    MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, midPos + edgeTangents[0]*size, midPos3 + edgeTangents[0]*size,
      midPos3 - edgeTangents[0]*size, NMP::Colour::DARK_ORANGE);
    MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, midPos + edgeTangents[0]*size, midPos3 - edgeTangents[0]*size,
      midPos - edgeTangents[0]*size, NMP::Colour::DARK_ORANGE);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, midPos,
      faceNormals[0] * dimensionalScaling.scaleDist(1.0f), NMP::Colour::YELLOW);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, midPos,
      faceNormals[1] * dimensionalScaling.scaleDist(1.0f), NMP::Colour::YELLOW);
  }
  else if (type == EO_Plane) // draw 1 square
  {
    // Create two orthogonal axes to a normal using a random vector which is not axis aligned (to
    // prevent an easy singularity)
    NMP::Vector3 obscureVector(1, 2, 3);
    NMP::Vector3 side1(NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(faceNormals[0], obscureVector));
    NMP::Vector3 side2(-NMRU::GeomUtils::calculateOrthogonalPlanarVector(faceNormals[0], obscureVector));

    side1.normalise();
    side2.normalise();

    side1 *= size;
    side2 *= size;

    MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, corner - side1 - side2,
      corner - side1 + side2, corner + side1 + side2, NMP::Colour::ORANGE);
    MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst,
      corner + side1 + side2, corner + side1 - side2, corner - side1 - side2, NMP::Colour::ORANGE);
  }
  else if (type == EO_ContactPlane) // draw contact plane as a wireframe square
  {
    MR_DEBUG_DRAW_ENVIRONMENT_PATCH(pDebugDrawInst, corner, faceNormals[0], size, NMP::Colour::WHITE);
  }
  else if (type == EO_Capsule)
  {
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, corner,
      faceNormals[0] * dimensionalScaling.scaleDist(1.0f), NMP::Colour::ORANGE);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, corner, radius, NMP::Colour::DARK_YELLOW);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, corner + faceNormals[0], radius, NMP::Colour::DARK_YELLOW);
  }
  else if (type == EO_Sphere)
  {
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, corner, radius, NMP::Colour::DARK_YELLOW); // sphere just drawn as point here
  }
#endif // defined(MR_OUTPUT_DEBUGGING)
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

