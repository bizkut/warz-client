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

#ifndef NM_MDF_TYPE_ENVIRONMENT_SUPPORTPOLY_H
#define NM_MDF_TYPE_ENVIRONMENT_SUPPORTPOLY_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// external types
#include "NMPlatform/NMVector3.h"

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

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 528 Bytes
// Alignment: 16

// This structure stores 3d points, to allow inside outside tests in various plane directions. For simplicity it
// requires all the points to be in clockwise order around the plane direction, something which isn't a concern for
// under 3 points.

struct SupportPoly
{

  NMP::Vector3 m_corners[32];                ///< (Position)

  uint32_t m_numCorners;

  bool m_fullySupported;  ///< if true the getDistanceToPoint will return 0


  // functions

  /// distance to the polygon in the plane defined by normal, -ve means penetration. 
  /// closestPointInPolygon also in plane defined by normal and position
  float getDistanceToPoint(const NMP::Vector3& position, const NMP::Vector3& normal, float radius, NMP::Vector3* closestPointInPolygon) const;
  void debugDraw(const NMP::Vector3& position, const NMP::Vector3& normal, float radius, const NMP::Vector3& closestPointInPolygon, const NMP::Vector3& colour, MR::InstanceDebugInterface* pDebugDrawInst) const;
  /// Returns the extent (width) in the direction (must be normalised). 
  /// May not return the minimum in the case of parallel sides
  float getExtentsInDirection(const NMP::Vector3& direction) const;
  /// Returns the convex hull of points, based on up vector which is perpendicular to the plane. The points are not
  /// projected onto the plane - i.e. the points out are a subset of the points in. Convex hull is clockwise wound
  /// when viewed along the up direction.
  void buildConvexHull(const NMP::Vector3* points, int32_t numPoints, const NMP::Vector3& up, bool fullySupported=0);
  NMP::Vector3 getCentre() const;


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(m_corners[0]), "m_corners[0]");
    NM_VALIDATOR(Vector3Valid(m_corners[1]), "m_corners[1]");
    NM_VALIDATOR(Vector3Valid(m_corners[2]), "m_corners[2]");
    NM_VALIDATOR(Vector3Valid(m_corners[3]), "m_corners[3]");
    NM_VALIDATOR(Vector3Valid(m_corners[4]), "m_corners[4]");
    NM_VALIDATOR(Vector3Valid(m_corners[5]), "m_corners[5]");
    NM_VALIDATOR(Vector3Valid(m_corners[6]), "m_corners[6]");
    NM_VALIDATOR(Vector3Valid(m_corners[7]), "m_corners[7]");
    NM_VALIDATOR(Vector3Valid(m_corners[8]), "m_corners[8]");
    NM_VALIDATOR(Vector3Valid(m_corners[9]), "m_corners[9]");
    NM_VALIDATOR(Vector3Valid(m_corners[10]), "m_corners[10]");
    NM_VALIDATOR(Vector3Valid(m_corners[11]), "m_corners[11]");
    NM_VALIDATOR(Vector3Valid(m_corners[12]), "m_corners[12]");
    NM_VALIDATOR(Vector3Valid(m_corners[13]), "m_corners[13]");
    NM_VALIDATOR(Vector3Valid(m_corners[14]), "m_corners[14]");
    NM_VALIDATOR(Vector3Valid(m_corners[15]), "m_corners[15]");
    NM_VALIDATOR(Vector3Valid(m_corners[16]), "m_corners[16]");
    NM_VALIDATOR(Vector3Valid(m_corners[17]), "m_corners[17]");
    NM_VALIDATOR(Vector3Valid(m_corners[18]), "m_corners[18]");
    NM_VALIDATOR(Vector3Valid(m_corners[19]), "m_corners[19]");
    NM_VALIDATOR(Vector3Valid(m_corners[20]), "m_corners[20]");
    NM_VALIDATOR(Vector3Valid(m_corners[21]), "m_corners[21]");
    NM_VALIDATOR(Vector3Valid(m_corners[22]), "m_corners[22]");
    NM_VALIDATOR(Vector3Valid(m_corners[23]), "m_corners[23]");
    NM_VALIDATOR(Vector3Valid(m_corners[24]), "m_corners[24]");
    NM_VALIDATOR(Vector3Valid(m_corners[25]), "m_corners[25]");
    NM_VALIDATOR(Vector3Valid(m_corners[26]), "m_corners[26]");
    NM_VALIDATOR(Vector3Valid(m_corners[27]), "m_corners[27]");
    NM_VALIDATOR(Vector3Valid(m_corners[28]), "m_corners[28]");
    NM_VALIDATOR(Vector3Valid(m_corners[29]), "m_corners[29]");
    NM_VALIDATOR(Vector3Valid(m_corners[30]), "m_corners[30]");
    NM_VALIDATOR(Vector3Valid(m_corners[31]), "m_corners[31]");
#endif // NM_CALL_VALIDATORS
  }

}; // struct SupportPoly

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_SUPPORTPOLY_H

