// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erCollisionProbes.h"

#include "mrPhysicsScenePhysX3.h"
#include "mrPhysX3.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaUserData.h"
#include "mrPhysX3Includes.h"
#include "NMGeomUtils/NMGeomUtils.h"

#include "euphoria/erValueValidators.h"

#define BACKUP_RAYCAST // If sweep sphere initially overlaps then perform a raycast to try and find the intersection info

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
static bool debug_drawSweepResult = true;
#endif

#define PROBE_GROUP 31 // 31 is our group reserved for our individual shape probes

namespace ER
{
physx::PxShape* getPxShapeFromShapeID(int64_t shapeID)
{
  physx::PxShape* shape = (physx::PxShape*) (size_t) shapeID;
  const bool shapeExists = (shapeID != -1 && shapeID != 0 && MR::PhysXPerShapeData::getFromShape(shape));
  if (!shapeExists)
    return 0;
  // Also check that the shape/actor is part of the scene - if not then we should always ignore it
  if (!shape->getActor().getScene())
    return 0;
  return shape;
}

void SphereSweep::debugDraw(
  const NMP::Vector3& MR_OUTPUT_DEBUG_ARG(colour),
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR_DEBUG_DRAW_SPHERESWEEP(pDebugDrawInst, position, motion, radius, NMP::Colour(colour));
  MR_DEBUG_DRAW_SPHERESWEEP(pDebugDrawInst, position + motion, motion2, radius, NMP::Colour(colour));
#endif
}

//----------------------------------------------------------------------------------------------------------------------
class EuphoriaPhysX3QueryFilterCallback : public MR::MorphemePhysX3QueryFilterCallback
{
public:
  EuphoriaPhysX3QueryFilterCallback(const physx::PxFilterData& queryFilterData, int ignoreID=-1) : 
      MorphemePhysX3QueryFilterCallback(queryFilterData, ignoreID) {}
private:
  physx::PxSceneQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, physx::PxShape* shape, physx::PxSceneQueryFilterFlags& filterFlags) NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
physx::PxSceneQueryHitType::Enum EuphoriaPhysX3QueryFilterCallback::preFilter(const physx::PxFilterData& filterData, physx::PxShape* shape, physx::PxSceneQueryFilterFlags& filterFlags)
{
  if (!MR::PhysXPerShapeData::getFromShape(shape))
    return physx::PxSceneQueryHitType::eNONE;
  return MorphemePhysX3QueryFilterCallback::preFilter(filterData, shape, filterFlags);
}

//----------------------------------------------------------------------------------------------------------------------
/// Performs the specified sweep (on the specified scene) and stores the results as member data
//----------------------------------------------------------------------------------------------------------------------
bool SweepResult::applySweep(
  const SphereSweep& sweep,
  MR::PhysicsScenePhysX3* scene,
  physx::PxClientID clientID,
  uint32_t ignoreGroups,
  bool dynamic,
  float distScale,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  m_probeID = sweep.probeID;
  // First, do the sweep test. We choose not to do a sphere check here first because it they don't
  // help much in terms of giving us info and it is a high penalty to just avoid an assert
  physx::PxVec3 motionDir[2] = { MR::nmVector3ToPxVec3(sweep.motion), MR::nmVector3ToPxVec3(sweep.motion2) };
  physx::PxVec3 pos[2];
  pos[0] = MR::nmVector3ToPxVec3(sweep.position);
  pos[1] = pos[0] + motionDir[0];
  physx::PxReal distance[2] = { motionDir[0].normalize(), motionDir[1].normalize() };
  uint32_t contacted = false;
  physx::PxSceneQueryImpactHit* probeHit = NULL;
  physx::PxSweepHit sweepHit;
  physx::PxRaycastHit rayHit;
  // defines what data is filled in from a successful probe
  physx::PxSceneQueryFlags hintFlags =
    physx::PxSceneQueryFlag::eDISTANCE |
    physx::PxSceneQueryFlag::eIMPACT |
    physx::PxSceneQueryFlag::eNORMAL |
    physx::PxSceneQueryFlag::eINITIAL_OVERLAP;
  physx::PxSceneQueryFlags queryFlags = physx::PxSceneQueryFlags(hintFlags);
  int i = 0;
  if (dynamic)
  {
    // Do the dynamic test
    NMP_ASSERT(sweep.targetShapeID);

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, sweep.position, sweep.motion, NMP::Colour::LIGHT_RED);

    physx::PxSphereGeometry sphere;
    sphere.radius = sweep.radius;

    physx::PxShape *targetShape = (physx::PxShape*) (size_t) sweep.targetShapeID;
    if (!MR::PhysXPerShapeData::getFromShape(targetShape))
    {
      m_type = SR_NoContact;
      return false; // No hit
    }

    physx::PxGeometryType::Enum type = targetShape->getGeometryType();
    physx::PxTransform shapeTM = MR::getShapeGlobalPose(*targetShape);
    contacted = false;

    // MORPH-21378: There's no sweep against shape, so the permutations are done manually here.
    physx::PxGeometry* geometry = NULL;
    physx::PxBoxGeometry boxGeom;
    physx::PxSphereGeometry sphereGeom;
    physx::PxCapsuleGeometry capsuleGeom;
    physx::PxConvexMeshGeometry convexMeshGeom;
    physx::PxTriangleMeshGeometry triangleMeshGeom;

    if (type == physx::PxGeometryType::eBOX)
    {
      targetShape->getBoxGeometry(boxGeom);
      geometry = &boxGeom;
    }
    else if (type == physx::PxGeometryType::eSPHERE)
    {
      targetShape->getSphereGeometry(sphereGeom);
      geometry = &sphereGeom;
    }
    else if (type == physx::PxGeometryType::eCAPSULE)
    {
      targetShape->getCapsuleGeometry(capsuleGeom);
      geometry = &capsuleGeom;
    }
    else if (type == physx::PxGeometryType::eCONVEXMESH)
    {
      targetShape->getConvexMeshGeometry(convexMeshGeom);
      geometry = &convexMeshGeom;
    }
    else if (type == physx::PxGeometryType::eTRIANGLEMESH)
    {
      targetShape->getTriangleMeshGeometry(triangleMeshGeom);
      geometry = &triangleMeshGeom;
    }
    else
    {
      NMP_ASSERT_FAIL_MSG("Unsupported collision type %d for dynamic objects", type); // Unsupported type
    }

    for (; i < 2; i++)
    {
      if (distance[i] == 0.0f)
      {
        // PhysX checks for initial overlap, but can't cope with zero distance. Ideally we'd do an
        // overlap test here and get a position/normal based on the minimum separation, but there's
        // no way of getting that info (yet) from physx. Also we don't want to use a tiny distance
        // with arbitrary direction, since the sweep call returns the negative sweep direction in
        // the case of initial overlap, which would be incorrect. Therefore we just skip this.
        continue;
      }
      physx::PxTransform t(pos[i], physx::PxQuat(0, 0, 0, 1));
      if (physx::PxGeometryQuery::sweep(motionDir[i], distance[i], sphere, t, *geometry, shapeTM,
        sweepHit, hintFlags))
      {
#if defined(BACKUP_RAYCAST)
        // physX bug that needs fixing
        if (sweepHit.distance == 0.0f) // sweepHit.flags & PxSceneQueryFlag::eINITIAL_OVERLAP)
        {
          // Initially overlapped, so do a ray cast instead
          if (physx::PxGeometryQuery::raycast(pos[i], motionDir[i], *geometry, shapeTM,
              distance[i] + sphere.radius, hintFlags, 1, &rayHit))
          {
            probeHit = &rayHit;
            contacted = true;
            break;
          }
        }
        else
#endif
        {
          probeHit = &sweepHit;
          contacted = true;
          break;
        }
      }
    }

    if (contacted)
    {
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, MR::nmPxVec3ToVector3(pos[i] + motionDir[i]*distance[i]), MR::nmPxVec3ToVector3(shapeTM.p), NMP::Colour::WHITE);
      probeHit->shape = targetShape;
    }
    else
    {
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, MR::nmPxVec3ToVector3(pos[i] + motionDir[i]*distance[i]), MR::nmPxVec3ToVector3(shapeTM.p), NMP::Colour::RED);
    }
  }
  else
  {
    // Do the static test
    physx::PxSphereGeometry sphere;
    sphere.radius = sweep.radius;

    // Use default PxFilterData here to bypass the internal filtering, and we pass the real flags
    // into the callback.
    physx::PxSceneQueryFilterData filterData(
      physx::PxSceneQueryFilterFlag::eSTATIC | physx::PxSceneQueryFilterFlag::ePREFILTER);
    EuphoriaPhysX3QueryFilterCallback euphoriaPhysX3QueryFilterCallback(physx::PxFilterData(
      0, 
      ignoreGroups, 
      0, 
      0));
    NMP_ASSERT(distance[0] > distScale * 0.00001f);
    for (; i < 2; i++)
    {
      if (distance[i] == 0.0f)
      {
        // See comment above for distance = 0 in the dynamic case
        continue;
      }

      if (scene->getPhysXScene()->sweepSingle(
        sphere,
        physx::PxTransform(pos[i], physx::PxQuat(0, 0, 0, 1)),
        motionDir[i],
        distance[i],
        queryFlags,
        sweepHit,
        filterData, &euphoriaPhysX3QueryFilterCallback, NULL, clientID))
      {
#if defined(BACKUP_RAYCAST)
        // Initial overlap so do a ray cast instead
        if (sweepHit.flags & physx::PxSceneQueryFlag::eINITIAL_OVERLAP)
        {
          if (scene->getPhysXScene()->raycastSingle(
            pos[i],
            motionDir[i],
            distance[i] + sphere.radius,
            queryFlags,
            rayHit,
            filterData,
            &euphoriaPhysX3QueryFilterCallback,
            NULL,
            clientID))
          {
            probeHit = &rayHit;
            contacted = true;
            break;
          }
        }
        else
#endif
        {
          probeHit = &sweepHit;
          contacted = true;
          break;
        }
      }
    }
  }
#if defined(MR_OUTPUT_DEBUGGING)
  // Draw original sweep
  sweep.debugDraw(NMP::Vector3(0.3f, 0, 0), pDebugDrawInst);
#endif

  if (!contacted || probeHit->normal.magnitudeSquared() < 0.01f)
  {
    // If no normal then we can't very well calculate correct sweep result
#if defined(MR_OUTPUT_DEBUGGING)
    sweep.debugDraw(NMP::Vector3(0.3f, 0, 0), pDebugDrawInst);
#endif
    m_type = SR_NoContact;
    return false; // No hit
  }
#if defined(MR_OUTPUT_DEBUGGING)
  SphereSweep tempSweep = sweep;
  if (i == 0)
  {
    tempSweep.motion2.setToZero();
    float time = probeHit->distance / (sweep.motion.magnitude() + 1e-10f);
    tempSweep.motion *= time;
  }
  else
  {
    float time = probeHit->distance / (sweep.motion2.magnitude() + 1e-10f);
    tempSweep.motion2 *= time;
  }
  tempSweep.debugDraw(NMP::Vector3(0, 1, 0), pDebugDrawInst);
#endif
  // Store common data straight away
  m_contactPoint = MR::nmPxVec3ToVector3(probeHit->impact);
  NMP::Vector3 contactNormal = MR::nmPxVec3ToVector3(probeHit->normal);
  NMP_ASSERT(Validators::Vector3Valid(m_contactPoint));
  NMP_ASSERT(Validators::Vector3Normalised(contactNormal));
  m_shapeID = (int64_t)(size_t)probeHit->shape;

  // Now store results based on what shape was hit...
  if (probeHit->shape->getGeometryType() == physx::PxGeometryType::eCONVEXMESH)
  {
    // MORPH-18325 Note that whilst this works fine for corners that have just got 3 planes, it
    // doesn't work/make sense for general case of a convex hull since there will be more than three
    // triangles surrounding the corner vertex. Environment_Patch.cpp calculates tangents using the
    // cross products between the face normals, but that results in one "triangle" being essentially
    // bogus. In practice it works OK (though looks a bit wrong) - however it might be better to
    // bypass SR_ConvexCorner and instead just calculate SR_Triangle instead.

    // Basically, the idea with this mesh is we take the contact triangle, and the 3 vertex normals
    // and (together with the face normal) we can calculate the 3 neighbouring face normals
    m_type = SR_ConvexCorner;
    physx::PxConvexMeshGeometry meshGeom;
    probeHit->shape->getConvexMeshGeometry(meshGeom);
    physx::PxConvexMesh* mesh = meshGeom.convexMesh;
    NMP::Matrix34 meshMat = MR::nmPxTransformToNmMatrix34(MR::getShapeGlobalPose(*probeHit->shape));

    // Next we have to adjust the meshMat to account for the mesh scale.
    // Convert internal mesh scaling to NMP matrix 
    physx::PxMat33 meshScale = meshGeom.scale.toMat33(); 

    // Copy transposed - PxMat33 is column-major 
    NMP::Matrix34 nmpMeshScale; 
    nmpMeshScale.r[0].set(meshScale.column0.x, meshScale.column0.y, meshScale.column0.z); 
    nmpMeshScale.r[1].set(meshScale.column1.x, meshScale.column1.y, meshScale.column1.z); 
    nmpMeshScale.r[2].set(meshScale.column2.x, meshScale.column2.y, meshScale.column2.z); 
    nmpMeshScale.r[3].setToZero(); 

    // Combine with actor transform, then inverse transform the contact point/normal to avoid having 
    // to transform each individual vertex 
    meshMat = nmpMeshScale * meshMat; 

    NMP::Matrix34 meshMatInv(meshMat);
    meshMatInv.invert();

    NMP::Vector3 contactPointLocal, contactNormalEndLocal;
    meshMatInv.transformVector(m_contactPoint, contactPointLocal); 

    // We need to transform the world space normal by the transpose of the inverse of the matrix
    // used to transform points (meshMatInv). However, since that was just inverted from meshMat,
    // all we need to do is transpose meshMat.
    NMP::Vector3 contactNormalLocal;
    NMP::Matrix34 meshMatTranspose(meshMat);
    meshMatTranspose.transpose3x3();
    meshMatTranspose.rotateVector(contactNormal, contactNormalLocal);
    contactNormalLocal.normalise();

    int numPolygons = mesh->getNbPolygons();
    const physx::PxVec3* vertices = (const physx::PxVec3*)mesh->getVertices();
    const uint8_t* indices = (const uint8_t*)mesh->getIndexBuffer();

    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Normal,
      MR::nmPxVec3ToVector3(probeHit->impact),
      MR::nmPxVec3ToVector3(probeHit->normal),
      NMP::Colour::DARK_YELLOW);
    MR_DEBUG_DRAW_BOX(
      pDebugDrawInst,
      MR::nmPxVec3ToVector3(probeHit->impact),
      NMP::Vector3(0,.01f, 0.01f, 0.01f),
      NMP::Colour::DARK_YELLOW);

    physx::PxU32 hitFaceIndex = probeHit->faceIndex;
    physx::PxHullPolygon hitPoly;
    bool success = mesh->getPolygonData(hitFaceIndex, hitPoly);
    (void) success; // For non NMP_ENABLE_ASSERTS builds
    NMP_ASSERT(success);

    // First, find the closest vertex in the hit face to the contact point.
    float minDistSq = FLT_MAX;
    int closestVertexIndex = -1;
    NMP::Vector3 corner(0, 0, 0);
    for (int j = 0; j != hitPoly.mNbVerts; j++)
    {
      int index = indices[hitPoly.mIndexBase + j];
      NMP::Vector3 vertex = MR::nmPxVec3ToVector3(vertices[index]);
      float squareDist = (contactPointLocal - vertex).magnitudeSquared();
      if (squareDist < minDistSq)
      {
        minDistSq = squareDist;
        closestVertexIndex = index;
        corner = vertex;
      }
    }

    NMP_ASSERT(closestVertexIndex != -1); // Failed to find nearest vertex.
    if (closestVertexIndex == -1)
    {
      m_type = SR_NoContact;
      return false; // No hit
    }

    // Now we have the closest vertex, we loop through each polygon and find its normal and whether
    // the vertex is in it. Note that this could be simplified if PhysX provided a way to get
    // adjacent faces for convex meshes.
    NMP::Vector3 normals[3];
    int numNormals = 0;
    for (int k = 0; k < numPolygons; k++)
    {
      physx::PxHullPolygon polygon;
      bool polySuccess = mesh->getPolygonData(k, polygon);
      (void) polySuccess; // For non NMP_ENABLE_ASSERTS builds
      NMP_ASSERT(polySuccess);
      for (int j = 0; j < polygon.mNbVerts; j++)
      {
        int index = indices[polygon.mIndexBase + j];
        if (index == closestVertexIndex)
        {
          int index2 = indices[polygon.mIndexBase + (j+1)%polygon.mNbVerts];
          NMP::Vector3 vertex1 = MR::nmPxVec3ToVector3(vertices[index]);
          NMP::Vector3 vertex2 = MR::nmPxVec3ToVector3(vertices[index2]);
          NMP::Vector3 faceNormal(polygon.mPlane[0], polygon.mPlane[1], polygon.mPlane[2]);
          // We expect the polygons to be CCW wound (this code would have to change if a different
          // winding was specified when the mesh is cooked). We could check this by comparing it to
          // a point definitely inside the convex hull - but the origin isn't guaranteed to be inside.
          if (numNormals==3)
          {
            float dot = (vertex1 - contactPointLocal).dot(contactNormalLocal);
            if (dot > -distScale*0.001f) // if we find the actual face that touches the hit point, then that takes priority
            {
              normals[0] = faceNormal;
              meshMat.rotateVector(vertex1); // necessary so that magnitude is correct with scaling
              meshMat.rotateVector(vertex2);
              m_vectorData[4][0] = (vertex2-vertex1).magnitude();
            }
          }
          else
          {
            meshMat.rotateVector(vertex1); // necessary so that magnitude is correct with scaling
            meshMat.rotateVector(vertex2);
            m_vectorData[4][numNormals] = (vertex2-vertex1).magnitude(); // edge length

            normals[numNormals++] = faceNormal;
          }
          break;
        }
      }
    }
    NMP_ASSERT(numNormals == 3); // Otherwise we haven't found enough faces that touch the closest corner

    // Swap normal order around so it is still considered convex
    if (NMP::vCross(normals[0], normals[1]).dot(normals[2]) < 0.0f)
    {
      NMP::nmSwap(normals[1], normals[2]);
      NMP::nmSwap(m_vectorData[4][1], m_vectorData[4][2]);
    }
    m_vectorData[0] = meshMat.getTransformedVector(corner);

    // To transform the normals into world space use the transopose of the inverse
    NMP::Matrix34 meshMatInvTranspose(meshMatInv);
    meshMatInvTranspose.transpose3x3();
    meshMatInvTranspose.rotateVector(contactNormal, contactNormalLocal);

    for (int j = 0; j < 3; j++)
    {
      m_vectorData[j+1] = NMP::vNormalise(meshMatInvTranspose.getRotatedVector(normals[j]));
      NMP_ASSERT(m_vectorData[j+1].isNormal(0.1f));
    }
  }
  else if (probeHit->shape->getGeometryType() == physx::PxGeometryType::eBOX)
  {
    m_type = SR_Box;
    // Retrieve the box object and convert parameters to NMPlatform structures
    physx::PxTransform shapeTransform = MR::getShapeGlobalPose(*probeHit->shape);
    physx::PxBoxGeometry boxGeom;
    probeHit->shape->getBoxGeometry(boxGeom);
    m_vectorData[0] = MR::nmPxVec3ToVector3(shapeTransform.q.rotate(physx::PxVec3(1, 0, 0)));
    m_vectorData[1] = MR::nmPxVec3ToVector3(shapeTransform.q.rotate(physx::PxVec3(0, 1, 0)));
    m_vectorData[2] = MR::nmPxVec3ToVector3(shapeTransform.q.rotate(physx::PxVec3(0, 0, 1)));
    m_vectorData[3] = MR::nmPxVec3ToVector3(shapeTransform.p);
    m_vectorData[4] = MR::nmPxVec3ToVector3(boxGeom.halfExtents);
  }
  else if (probeHit->shape->getGeometryType() == physx::PxGeometryType::eCAPSULE)
  {
    m_type = SR_Capsule;
    // Retrieve the box object and convert parameters to NMPlatform structures
    physx::PxTransform shapeTransform = MR::getShapeGlobalPose(*probeHit->shape);
    physx::PxVec3 up = shapeTransform.rotate(physx::PxVec3(1, 0, 0));
    physx::PxCapsuleGeometry caps;
    probeHit->shape->getCapsuleGeometry(caps);
    physx::PxVec3 p0 = shapeTransform.p - up * caps.halfHeight;
    physx::PxVec3 p1 = shapeTransform.p + up * caps.halfHeight;
    m_vectorData[0] = MR::nmPxVec3ToVector3(p0);
    m_vectorData[1] = MR::nmPxVec3ToVector3(p1);
    m_vectorData[2].x = 2.0f * caps.halfHeight; // Store capsule length
    m_floatData = caps.radius;
  }
  else if (probeHit->shape->getGeometryType() == physx::PxGeometryType::eTRIANGLEMESH)
  {
    m_type = SR_Triangle;
    physx::PxTriangleMeshGeometry trimeshGeom;
    probeHit->shape->getTriangleMeshGeometry(trimeshGeom);
    physx::PxTriangle triangle;
    physx::PxU32 vertexIndices[3];
    physx::PxTransform pose = MR::getShapeGlobalPose(*probeHit->shape);
    physx::PxU32 adjacencyIndices[3];
    physx::PxMeshQuery::getTriangle(trimeshGeom, pose, probeHit->faceIndex, triangle, vertexIndices, adjacencyIndices);
    physx::PxVec3 triangleNormal;
    triangle.normal(triangleNormal);
    m_vectorData[0] = MR::nmPxVec3ToVector3(triangleNormal);

#if defined(MR_OUTPUT_DEBUGGING)
    {
      NMP::Vector3 points[] = { 
        MR::nmPxVec3ToVector3(triangle.verts[0]), 
        MR::nmPxVec3ToVector3(triangle.verts[1]), 
        MR::nmPxVec3ToVector3(triangle.verts[2]), 
        MR::nmPxVec3ToVector3(triangle.verts[0])
      };
      MR_DEBUG_DRAW_POLYLINE(pDebugDrawInst, 4, points, NMP::Colour::ORANGE);
    }
#endif

    for (int j = 0 ; j != 3 ; ++j)
    {
      m_vectorData[j+1] = MR::nmPxVec3ToVector3(triangle.verts[j]);
      physx::PxU32 adjacencyIndex = adjacencyIndices[(j+0)%3];
      if (adjacencyIndex != 0xFFFFFFFF)
      {
        physx::PxTriangle neighbourTriangle;
        physx::PxMeshQuery::getTriangle(
          trimeshGeom, pose, adjacencyIndex, neighbourTriangle);

        physx::PxVec3 neighbourNormal;
        neighbourTriangle.normal(neighbourNormal);
        m_vectorData[j+4] = MR::nmPxVec3ToVector3(neighbourNormal);
      }
      else
      {
        // If no neighbour then just treat it like a 90 degree edge
        m_vectorData[j+4] = NMP::vNormalise(
          MR::nmPxVec3ToVector3((triangle.verts[(j+1)%3] - triangle.verts[j]).cross(triangleNormal)));
      }
    }
  }
  else if (probeHit->shape->getGeometryType() == physx::PxGeometryType::eHEIGHTFIELD)
  {
    // This is a repeat of the eTRIANGLEMESH except the physx calls use the different type
    m_type = SR_Triangle;
    physx::PxHeightFieldGeometry heightfieldGeom;
    probeHit->shape->getHeightFieldGeometry(heightfieldGeom);
    physx::PxTriangle triangle;
    physx::PxU32 vertexIndices[3];
    physx::PxTransform pose = MR::getShapeGlobalPose(*probeHit->shape);
    physx::PxU32 adjacencyIndices[3];
    physx::PxMeshQuery::getTriangle(heightfieldGeom, pose, probeHit->faceIndex, triangle, vertexIndices, adjacencyIndices);
    physx::PxVec3 triangleNormal;
    triangle.normal(triangleNormal);
    m_vectorData[0] = MR::nmPxVec3ToVector3(triangleNormal);

#if defined(MR_OUTPUT_DEBUGGING)
    {
      NMP::Vector3 points[] = { 
        MR::nmPxVec3ToVector3(triangle.verts[0]), 
        MR::nmPxVec3ToVector3(triangle.verts[1]), 
        MR::nmPxVec3ToVector3(triangle.verts[2]), 
        MR::nmPxVec3ToVector3(triangle.verts[0])
      };
      MR_DEBUG_DRAW_POLYLINE(pDebugDrawInst, 4, points, NMP::Colour::ORANGE);
    }
#endif

    for (int j = 0 ; j != 3 ; ++j)
    {
      m_vectorData[j+1] = MR::nmPxVec3ToVector3(triangle.verts[j]);
      physx::PxU32 adjacencyIndex = adjacencyIndices[(j+0)%3];
      if (adjacencyIndex != 0xFFFFFFFF)
      {
        physx::PxTriangle neighbourTriangle;
        physx::PxMeshQuery::getTriangle(
          heightfieldGeom, pose, adjacencyIndex, neighbourTriangle);

        physx::PxVec3 neighbourNormal;
        neighbourTriangle.normal(neighbourNormal);
        m_vectorData[j+4] = MR::nmPxVec3ToVector3(neighbourNormal);
      }
      else
      {
        // If no neighbour then just treat it like a 90 degree edge
        m_vectorData[j+4] = NMP::vNormalise(
          MR::nmPxVec3ToVector3((triangle.verts[(j+1)%3] - triangle.verts[j]).cross(triangleNormal)));
      }
    }
  }
  else if (probeHit->shape->getGeometryType() == physx::PxGeometryType::ePLANE)
  {
    m_type = SR_Plane;
    m_vectorData[0] = MR::nmPxVec3ToVector3(probeHit->impact);
    m_vectorData[1] = MR::nmPxVec3ToVector3(MR::getShapeGlobalPose(*probeHit->shape).rotate(physx::PxVec3(1, 0, 0)));
  }
  else if (probeHit->shape->getGeometryType() == physx::PxGeometryType::eSPHERE)
  {
    m_type = SR_Sphere;
    physx::PxTransform shapeTransform = MR::getShapeGlobalPose(*probeHit->shape);
    physx::PxSphereGeometry sphere;
    probeHit->shape->getSphereGeometry(sphere);
    m_floatData = sphere.radius;
    m_vectorData[0] = MR::nmPxVec3ToVector3(shapeTransform.p);
  }
  else
  {
    NMP_ASSERT_FAIL();
  }
#if defined(MR_OUTPUT_DEBUGGING)
  debugDraw(pDebugDrawInst); // May later have this called separately
#endif // defined(MR_OUTPUT_DEBUGGING)
  return true;
}

void SweepResult::setFromCapsule(
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  float radius,
  float halfHeight,
  int64_t shapeID,
  int32_t probeID)
{
  m_vectorData[0] = start;
  m_vectorData[1] = end;
  m_vectorData[2].x = 2.0f * halfHeight;
  m_floatData = radius;

  m_contactPoint = 0.5f * (start + end);
  m_shapeID = shapeID;
  m_probeID = probeID;
  m_type = SR_Capsule;
}

//----------------------------------------------------------------------------------------------------------------------
void SweepResult::setFromPlane(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  int64_t shapeID,
  int32_t probeID)
{
  m_vectorData[0] = position;
  m_vectorData[1] = normal;
  // This could be far from the sweep, but I suppose that is the correct thing to do since anything else is inaccurate,
  // you could say that the probe _is_ the contact
  m_contactPoint = position;
  m_shapeID = shapeID;
  m_probeID = probeID;
  m_type = SR_Plane;
}

//----------------------------------------------------------------------------------------------------------------------
void SweepResult::setFromContact(const NMP::Vector3& position, const NMP::Vector3& normal, int64_t shapeID)
{
  m_vectorData[0] = position;
  m_vectorData[1] = normal;
  // This could be far from the sweep, but I suppose that is the correct thing to do since anything else is inaccurate,
  // you could say that the probe _is_ the contact
  m_contactPoint = position;
  m_shapeID = shapeID;
  m_probeID = -1;
  m_type = SR_ContactPlane;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
void SweepResult::debugDraw(MR::InstanceDebugInterface* pDebugDrawInst, float size)
{
  if (!pDebugDrawInst)
    return;
  if (debug_drawSweepResult)
  {
    switch (m_type)
    {
    case SR_Triangle:
    {
      // Simple (unfilled) triangle
      NMP::Vector3 points[] = { m_vectorData[1], m_vectorData[2], m_vectorData[3], m_vectorData[1] };
      MR_DEBUG_DRAW_POLYLINE(pDebugDrawInst, 4, points, NMP::Colour::WHITE);

      // Normal to tri plane drawn from triangle centre
      // Normal to tri plane
      MR_DEBUG_DRAW_VECTOR(
        pDebugDrawInst,
        MR::VT_Normal,
        (m_vectorData[1] + m_vectorData[2] + m_vectorData[3]) / 3.0f,
        m_vectorData[0] * size,
        NMP::Colour::LIGHT_YELLOW);
      // Neighbouring triangle normals
      for (int i = 0; i < 3; i++)
      {
        NMP::Vector3 midPos = (m_vectorData[((i + 1) % 3) + 1] + m_vectorData[i + 1]) * 0.5f;
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, midPos, m_vectorData[i+4] * size, NMP::Colour::LIGHT_YELLOW);
      }
      return;
    }
    case SR_Capsule:
    {
      // Need better debug draw here
      // Capsule beginning to end, line with points at each end
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, m_vectorData[0], m_vectorData[1], NMP::Colour::WHITE);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, m_vectorData[0], m_floatData, NMP::Colour::WHITE);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, m_vectorData[1], m_floatData, NMP::Colour::WHITE);
      return;
    }
    case SR_Plane:
    case SR_ContactPlane:
    {
      // Plane
      MR_DEBUG_DRAW_PLANE(pDebugDrawInst, m_vectorData[0], m_vectorData[1] - m_vectorData[0], size, NMP::Colour::WHITE, size);

      // Plane normal
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, m_vectorData[0], m_vectorData[1] * size, NMP::Colour::LIGHT_YELLOW);
      return;
    }
    case SR_Sphere:
    {
      // Need better debug draw here
      // Point of size sphere radius, at centre of sphere
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, m_vectorData[0], m_floatData, NMP::Colour::WHITE);
      return;
    }
    case SR_Box:
    {
      // Draw TM-alligned Bounding box.
      NMP::Matrix34 tm(m_vectorData[0], m_vectorData[1], m_vectorData[2], m_vectorData[3]);
      NMP::Vector3 extents(m_vectorData[4][0], m_vectorData[4][1], m_vectorData[4][2]);
      MR_DEBUG_DRAW_BBOX(pDebugDrawInst, tm, extents, NMP::Colour::WHITE);
      return;
    }
    default:
      return;
    }
  }
}
#endif // Defined(MR_OUTPUT_DEBUGGING)

} // namespace ER
