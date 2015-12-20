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

#ifndef NM_MDF_TYPE_ENVIRONMENT_STATE_H
#define NM_MDF_TYPE_ENVIRONMENT_STATE_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMGeomUtils/NMGeomUtils.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace MR { class InstanceDebugInterface; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct SphereTrajectory; 

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 128 Bytes
// Alignment: 16

// smaller class just for the bounds and state

struct State
{

  NMRU::GeomUtils::AABB aabb;

  NMP::Vector3 position;                     ///< (Position)

  NMP::Vector3 velocity;                     ///< (Velocity)

  NMP::Vector3 angularVelocity;              ///< (AngularVelocity)

  NMP::Vector3 acceleration;                 ///< (Acceleration)

  int64_t shapeID;  ///< Will be cast from a pointer to the PxShape

  float accSqr;

  float mass;  ///< this can be the effective mass of that point on the object, but also can be scaled by its dangerousness  (Mass)

  bool isStatic;


  // functions

  State();
  NMP::Vector3 getInstantVelocityAtPoint(const NMP::Vector3& point) const;
  NMP::Vector3 getVelocityAtPoint(const NMP::Vector3& point) const;
  NMP::Vector3 getAccelerationAtPoint(const NMP::Vector3& point) const;
  // bends a path to account for angular velocity of object
  void adjustPathForAngularVelocity(SphereTrajectory& path) const;
  // All these tests expand the box dimensions by the sphere radius, not inflate it
  // this means it is only an approximation of a sweeping sphere with a box at the corners
  bool getTrajectoryOverlapTimes(const SphereTrajectory& path, float& t1, float& t2);
  /// returns -1.f for no collision
  float sphereTrajectoryCollides(const SphereTrajectory& path, bool forwards);
  /// returns true if the box of the path and the object intersect
  bool boundingBoxCull(const SphereTrajectory& path) const;
  void debugDraw(const NMP::Vector3& startColour, const NMP::Vector3& endColour, MR::InstanceDebugInterface* pDebugDrawInst);
  NMP::Vector3 rotateNormal(const NMP::Vector3& normal, float time) const;
  NMP::Vector3 getAverageAngularVel() const;
  NMP::Vector3 getAveragedPointVel(const NMP::Vector3& point, float timePeriod) const;


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(position), "position");
    NM_VALIDATOR(Vector3Valid(velocity), "velocity");
    NM_VALIDATOR(Vector3Valid(angularVelocity), "angularVelocity");
    NM_VALIDATOR(Vector3Valid(acceleration), "acceleration");
    NM_VALIDATOR(FloatValid(accSqr), "accSqr");
    NM_VALIDATOR(FloatNonNegative(mass), "mass");
#endif // NM_CALL_VALIDATORS
  }

}; // struct State

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_STATE_H

