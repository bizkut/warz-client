#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
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
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_TYPE_ENVIRONMENT_PATCH_H
#define NM_MDF_TYPE_ENVIRONMENT_PATCH_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Environment_State.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace ER { class DimensionalScaling; } 
namespace ER { class SweepResult; } 
namespace MR { class InstanceDebugInterface; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct SphereTrajectory; 
namespace Environment { struct CollideResult; } 
namespace Environment { struct Patch; } 
namespace Environment { struct LocalShape; } 

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 240 Bytes
// Alignment: 16

// one of these for each environment object the character will be aware of.
// I've kept the data to a minimum for the moment.
// Like GJK, we can store the last normal (and plane) used in a collision routine for warm starting.
// hopefully this can give fast collision results.
// Incidentally, Minkowski Portal Refinement might be a good way to extend the collision routines to a more general
// polyhedron shape.
//
// Note that the corner of the patch has a different meaning depending on the type of that patch:
// If patch type is EO_Corner this is the global position of the corner of known edges.
// If patch type is EO_Edge this is the start point of known edge that extends from length of last edgeLengths[0] in the direction given by the cross product of faceNormals[1] and faceNormals[2].
// If patch type is EO_Plane this is the sweep plane contact point global position.
// If patch type is EO_Capsule, this is the global position of either the capsule start point or capsule end point, depending on which point is closer to the sweep contact point.
// If patch type is EO_Sphere, this is the global position of its centre.
// If patch type is EO_ContactPlane this is the sweep contact plane global position.

struct Patch
{

  enum EOType
  {
    /*  0 */ EO_NoContact,  
    /*  1 */ EO_Corner,  
    /*  2 */ EO_Edge,  
    /*  3 */ EO_Plane,  
    /*  4 */ EO_Capsule,  
    /*  5 */ EO_Sphere,  
    /*  6 */ EO_ContactPlane,  
    /*  7 */ EO_Max,  
  };

  Environment::State state;
 // 128 Bytes 
  NMP::Vector3 corner;  //< Global position of the corner of the patch. Note that the corner has a different meaning depending on the type of the patch.  (Position)

  NMP::Vector3 faceNormals[3];               ///< (PositionDelta)

  NMP::Vector3 knownContactPoint;            ///< (Position)

  float edgeLengths[3];                      ///< (Length)

  int32_t numKnownEdgeLengths;

  float radius;  ///< Radius exclusively defined for EO_Sphere or EO_Capsule patch types. Note that all the other patch types should have their radius set to 0.  (Length)

  int32_t type;  ///< EOType

  bool isPredicted;


  // functions

  Patch();
  // sweeps a sphere along the trajectory and checks for first collision
  // can use this same routine for linear sphere sweeps
  // lastNormal can be used to warm start the iterations for quicker performance by passing in the 
  // last impactNormal (otherwise pass in zero vector)... though occasionally not using it seems necessary incase it occasionally misses
  bool sphereTrajectoryCollides(const SphereTrajectory& path, const NMP::Vector3& lastNormal, Environment::CollideResult& result, const ER::DimensionalScaling& dimensionalScaling, bool adjustForAngularVelocity=bool(true)) const;
  // finds the nearest point on the collision object
  int32_t nearestPoint(const NMP::Vector3& pointWorld, NMP::Vector3& nearestPoint, bool getSurfacePoint=0) const;
  // Function is a publicly exposed wrapper for the private function getNearestPointInside.
  bool getNearestPointInside(const NMP::Vector3& pointWorld, NMP::Vector3& nearestPoint, bool getSurfacePoint) const;
  // This function converts the sweep result into a format that allows for fast collision testing
  void updateFromSweepResult(const ER::SweepResult& sweep, const ER::DimensionalScaling& dimensionalScaling);
  /// epsilon in m for positions and radians for normals
  bool isConnectedTo(const Environment::Patch& object, const Environment::LocalShape& shape, const Environment::LocalShape& objectShape, float epsilon=0.08f);
  /// This updates the position, velocity and orientation of the patch based on its known angular vel and acceleration
  void update(float timeStep);
  void debugDraw(float size, MR::InstanceDebugInterface* pDebugDrawInst, const ER::DimensionalScaling& dimensionalScaling) const;
  /// Returns the number that have been filled in
  int32_t getEdgeDirections(NMP::Vector3* edgeDirections) const;
  /// Returns the first normal touching this edge index
  int32_t getNormal1Index(int32_t edgeIndex) const;
  /// Returns the second normal touching this edge index
  int32_t getNormal2Index(int32_t edgeIndex) const;
  /// Returns a metric of how reliable a resulting position is 
  float getReliabilityOfPosition(const NMP::Vector3& position, const NMP::Vector3& pathPosition, bool clipped) const;
  // Selection of creation functions for manually generated patch shapes
  void createAsPlane(const NMP::Vector3& position, const NMP::Vector3& normal, float radius, const NMP::Vector3& velocity, const NMP::Vector3& angularVelocity, float mass, int64_t shapeID);
  void createAsSphere(const NMP::Vector3& position, float radius, const NMP::Vector3& velocity, float mass, int64_t shapeID);

private:



  // functions

  bool getNearestPointInside(NMP::Vector3& nearestPoint, const NMP::Vector3& planeIntersectionPoint, const NMP::Vector3* edgeTangents, bool getSurfacePoint) const;
  /// Returns the number that have been filled in
  void getEdgeTangents(NMP::Vector3* edgeTangents) const;

public:


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    state.validate();
    NM_VALIDATOR(Vector3Valid(corner), "corner");
    NM_VALIDATOR(Vector3Valid(faceNormals[0]), "faceNormals[0]");
    NM_VALIDATOR(Vector3Valid(faceNormals[1]), "faceNormals[1]");
    NM_VALIDATOR(Vector3Valid(faceNormals[2]), "faceNormals[2]");
    NM_VALIDATOR(Vector3Valid(knownContactPoint), "knownContactPoint");
    NM_VALIDATOR(FloatNonNegative(edgeLengths[0]), "edgeLengths[0]");
    NM_VALIDATOR(FloatNonNegative(edgeLengths[1]), "edgeLengths[1]");
    NM_VALIDATOR(FloatNonNegative(edgeLengths[2]), "edgeLengths[2]");
    NM_VALIDATOR(FloatNonNegative(radius), "radius");
#endif // NM_CALL_VALIDATORS
  }

}; // struct Patch

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_PATCH_H

