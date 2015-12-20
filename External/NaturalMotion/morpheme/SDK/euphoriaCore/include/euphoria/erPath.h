// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_PATH_H
#define NM_PATH_H

#include "euphoria/erDebugDraw.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// a very simple circular path tool
// describes a plane circle embedded in R^3
//
// setup inputs:
// takes the circle radius and a basis
// queries:
// returns points on the circle given
//  either a scalar "s" in [0, 1]
//  or a point "x" in R^3
//
// note1: s from 0 to 1 gives a sweep of radius * unit_z of the basis
// sense is the usual rh convention giving a counter clock rotation z -> x about y
// s may take any value in R but will just wrap p(0) = p(1) = radius * unit_z
// by use of sin's and cos's in the circle point calc
//
// note2: all R^3 coords are wrt whatever circle.basis is wrt
// if circle basis is wrt eg. the shoulder then inputs and results are also
//----------------------------------------------------------------------------------------------------------------------
class Path
{
public:
  // built a circle / ellipse.
  void setup(const NMP::Matrix34& basisFrameMatrix, const float radius);
  void setup(const NMP::Matrix34& basisFrameMatrix, const float radiusX, const float radiusZ);

  // Query point on the path with a specified angle (angle in radians)
  void point(const float angle, NMP::Vector3& pointOnPath) const;

  // Get the tangent of the point on path.
  void tangent(const NMP::Vector3& pointOnPath, NMP::Vector3& tangentOnPoint) const;

  // Compute the position and the tangent for a point specified by an angle on the circle built with CircleTM.
  static void calcCirclePoint(
    NMP::Vector3& pointOnPath,
    NMP::Vector3& tangentOnPoint,
    const NMP::Matrix34& circleTM,
    const float radius,
    const float angle,
    MR::InstanceDebugInterface* pDebugDrawInst = 0);

  // Compute the position and the tangent for a point specified by an angle on the ellipse built with EllipseTM.
  static void calcEllipsePoint(
    NMP::Vector3& pointOnPath,
    NMP::Vector3& tangentOnPoint,
    const NMP::Matrix34& ellipseTM,
    const float radiusX,
    const float radiusY,
    const float angle,
    MR::InstanceDebugInterface* pDebugDrawInst = 0);

  // Compute the position for a point specified by an angle on the ellipse built with EllipseTM.
  static void calcEllipsePoint(
    NMP::Vector3& pointOnPath,
    const NMP::Matrix34& ellipseTM,
    const float radiusX,
    const float radiusY,
    const float angle,
    MR::InstanceDebugInterface* pDebugDrawInst = 0);

  static float getCircumference(const float radiusX, const float radiusY);
  static float getRadiusOnPoint(const float radiusX, const float radiusY, const float angle);
  static float getAngle(const NMP::Matrix34& ellipseTM, const NMP::Vector3& currentPoint);

  // Clamp any radians angle between 0 and 2Pi.
  static float clampAngle(const float angle);

  // path basis frame (circle centre, and plane)
  NMP::Matrix34   m_basis;    // pos ori of the path, normal of the ellipse is Z axis

  // radius
  float           m_radiusX;
  float           m_radiusY;
};

} // namespace ER

#endif // NM_PATH_H

