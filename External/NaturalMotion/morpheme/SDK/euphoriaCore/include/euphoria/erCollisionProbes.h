// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_COLLISIONPROBES_H
#define NM_COLLISIONPROBES_H

#include "erDebugDraw.h"
#include "Physics/PhysX3/mrPhysX3Includes.h"

namespace MR
{
  class PhysicsScenePhysX3;
  class PhysicsRigPhysX3Articulation;
}

namespace ER
{
  physx::PxShape* getPxShapeFromShapeID(int64_t shapeID);

/// The sphere sweep request structure. Usable inside behaviour modules and physics engine independent
struct SphereSweep
{
  NMP::Vector3 position;
  NMP::Vector3 motion;
  NMP::Vector3 motion2;
  float radius;
  int64_t targetShapeID;
  int probeID;

  void debugDraw(const NMP::Vector3& colour, MR::InstanceDebugInterface* pDebugDrawInst) const;
};

/// Physics engine independent results from a sweep test, including geometry at the hit point
/// Usable in behaviour modules
class SweepResult
{
public:
  enum Type
  {
    SR_NoContact, // the sweep didn't collide (type = 0)
    SR_Triangle,
    SR_Box,
    SR_Sphere,
    SR_Capsule,
    SR_Plane,
    SR_ConvexCorner,
    SR_ContactPlane,
    SR_Max
  };

  /// Performs the specified sweep (on the specified scene) and stores the results as member data
  bool applySweep(
    const SphereSweep& sweep,
    MR::PhysicsScenePhysX3* scene,
    physx::PxClientID clientID,
    uint32_t ignoreGroups,
    bool dynamic,
    float distScale,
    MR::InstanceDebugInterface* pDebugDrawInst);

#if defined(MR_OUTPUT_DEBUGGING)
  void debugDraw(MR::InstanceDebugInterface* pDebugDrawInst, float size = 0.5f);
#endif // defined(MR_OUTPUT_DEBUGGING)

  // accessors
  NMP::Vector3 getContactPoint() const;
  int32_t getType() const;
  int64_t getShapeID() const;
  int32_t getProbeID() const;
  NMP::Vector3 getTriangleNormal() const;
  NMP::Vector3 getVertex(uint32_t n) const;
  NMP::Vector3 getNeighbourNormal(uint32_t n) const;
  NMP::Matrix34 getBoxMatrix() const;
  NMP::Vector3 getBoxExtents() const;
  NMP::Vector3 getSpherePosition() const;
  float getSphereRadius() const;
  NMP::Vector3 getCapsuleStart() const;
  NMP::Vector3 getCapsuleEnd() const;
  float getCapsuleRadius() const;
  float getCapsuleLength() const;
  NMP::Vector3 getPlanePosition() const;
  NMP::Vector3 getPlaneNormal() const;
  NMP::Vector3 getCCCorner() const;
  NMP::Vector3 getCCFaceNormal(uint32_t i) const;
  float getCCEdgeLength(uint32_t i) const;

  // modifiers
  void setShapeID(int64_t shapeID);
  void setType(Type type) { m_type = type; }

  void setFromContact(
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    int64_t shapeID);

  void setFromPlane(
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    int64_t shapeID,
    int32_t probeID);

  void setFromCapsule(
    const NMP::Vector3& start,
    const NMP::Vector3& end,
    float radius,
    float halfHeight,
    int64_t shapeID,
    int32_t probeID);

private:

/*
  Results from different types of geometric object are stored in the m_vectorData array and m_floatData as follows:

    -- triangle with face normals for each edge too
      NMP::Vector3 triangleNormal;          0
      NMP::Vector3 vertex[3];               1,2,3
      NMP::Vector3 neighbourNormal[3];      4,5,6

    -- box
      NMP::Matrix34 boxMatrix;              0,1,2,3
      NMP::Vector3 boxExtents;              4

    -- sphere
      NMP::Vector3 spherePosition;          0
      float sphereRadius;                   m_floatData

    -- capsule
      NMP::Vector3 capsuleStart;            0
      NMP::Vector3 capsuleEnd;              1
      float capsuleLength                   2 (x)
      float capsuleRadius;                  m_floatData

    -- plane
      NMP::Vector3 planePosition;           0   
      NMP::Vector3 planeNormal;             1

    -- convex corner
      NMP::Vector3 ccCorner;                0
      NMP::Vector3 ccFaceNormal[3];         1,2,3
      float  ccEdgeLength[3];               4 (x,y,z)
*/
  NMP::Vector3 m_vectorData[7];
  float m_floatData;

  uint32_t pad[3]; // pad bytes

  NMP::Vector3 m_contactPoint;
  int64_t m_shapeID; // gets cast to/from pointer
  int32_t m_type;
  int32_t m_probeID;
};

} // namespace ER

#include "euphoria/erCollisionProbes.inl"

#endif // NM_COLLISIONPROBES_H
