// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erPath.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
void Path::setup(const NMP::Matrix34& basisFrameMatrix, const float radius)
{
  m_basis = basisFrameMatrix;
  m_radiusX = radius;
  m_radiusY = radius;
}

//----------------------------------------------------------------------------------------------------------------------
void Path::setup(const NMP::Matrix34& basisFrameMatrix, const float radiusX, const float radiusY)
{
  m_basis = basisFrameMatrix;
  m_radiusX = radiusX;
  m_radiusY = radiusY;
}

//----------------------------------------------------------------------------------------------------------------------
void Path::point(const float angle, NMP::Vector3& pointOnPath) const
{
  // calc coefficients for local x and z offsets given s
  float sin;
  float cos;
  NMP::fastSinCos(angle, sin, cos);
  float r_s_th = m_radiusX * sin;
  float r_c_th = m_radiusY * cos;

  // result of point calc back to user
  pointOnPath = m_basis.translation() + (r_s_th * m_basis.xAxis()) + (r_c_th * m_basis.yAxis());
}

//----------------------------------------------------------------------------------------------------------------------
void Path::tangent(const NMP::Vector3& pointOnPath, NMP::Vector3& tangentOnPoint) const
{
  // work out generator lines
  float c = NMP::fastSqrt(fabsf(NMP::sqr(m_radiusX) - NMP::sqr(m_radiusY)));

  bool longerAxisIsX = (m_radiusX >= m_radiusY ? true : false);
  NMP::Vector3 g1 = longerAxisIsX ? (m_basis.r[3] + c * m_basis.xAxis()) : (m_basis.r[3] + c * m_basis.yAxis());
  NMP::Vector3 g2 = longerAxisIsX ? (m_basis.r[3] - c * m_basis.xAxis()) : (m_basis.r[3] - c * m_basis.yAxis());

  NMP::Vector3 gl1 = g1 - pointOnPath;
  gl1.fastNormalise();
  NMP::Vector3 gl2 = g2 - pointOnPath;
  gl2.fastNormalise();

  // bissec of these generator lines
  NMP::Vector3 rotVec = vCross(gl1, gl2);
  rotVec /= 2.0f;
  NMP::Quat q;
  q.fromRotationVector(rotVec);
  NMP::Vector3 bissec = q.rotateVector(gl1);

  tangentOnPoint.cross(m_basis.zAxis(), bissec);
  tangentOnPoint.fastNormalise();
}

//----------------------------------------------------------------------------------------------------------------------
void Path::calcCirclePoint(
  NMP::Vector3& pointOnPath,
  NMP::Vector3& tangentOnPoint,
  const NMP::Matrix34& circleTM,
  const float radius,
  const float angle,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  Path circle;
  circle.setup(circleTM, radius);
  circle.tangent(pointOnPath, tangentOnPoint);

  // read off target point
  circle.point(angle, pointOnPath);
}

//----------------------------------------------------------------------------------------------------------------------
void Path::calcEllipsePoint(
  NMP::Vector3& pointOnPath,
  NMP::Vector3& tangentOnPoint,
  const NMP::Matrix34& ellipseTM,
  const float radiusX,
  const float radiusY,
  const float angle,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  Path ellipse;
  ellipse.setup(ellipseTM, radiusX, radiusY);

  // read off target point
  //
  ellipse.point(angle, pointOnPath);
  ellipse.tangent(pointOnPath, tangentOnPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void Path::calcEllipsePoint(
  NMP::Vector3& pointOnPath,
  const NMP::Matrix34& ellipseTM,
  const float radiusX,
  const float radiusY,
  const float angle,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  Path ellipse;
  ellipse.setup(ellipseTM, radiusX, radiusY);

  // read off target point
  //
  ellipse.point(angle, pointOnPath);
}

//----------------------------------------------------------------------------------------------------------------------
float Path::getRadiusOnPoint(const float radiusX, const float radiusY, const float angle)
{
  NMP::Matrix34 ellipseTM;
  ellipseTM.identity();
  Path ellipse;
  ellipse.setup(ellipseTM, radiusX, radiusY);

  NMP::Vector3 pointOnPath;
  ellipse.point(angle, pointOnPath);
  float radius = NMP::fastSqrt(pointOnPath.magnitudeSquared());
  return radius;
}

//----------------------------------------------------------------------------------------------------------------------
float Path::getCircumference(const float radiusX, const float radiusY)
{
  // Approximation of Ramanujan.
  float circumference = NM_PI * (3.0f * (radiusX + radiusY)
    - NMP::fastSqrt(10.0f * radiusX * radiusY + 3.0f * (NMP::sqr(radiusX) + NMP::sqr(radiusY))));
  return circumference;
}

//----------------------------------------------------------------------------------------------------------------------
float Path::getAngle(const NMP::Matrix34& ellipseTM, const NMP::Vector3& currentPoint)
{
  NMP::Vector3 toPoint = currentPoint - ellipseTM.translation();
  // Project toPoint on the plane of the ellipse.
  toPoint -= toPoint.dot(ellipseTM.zAxis()) * ellipseTM.zAxis();
  toPoint.fastNormalise();
  // Added epsilon here as fastArccos returns a small -ve value when dotWithY is 1.0
  float epsilon = 1e-8f;
  // Angle from Y axis to toPoint. Y axis is for the vertical axis for the circle/ellipse (angle 0).
  float dotWithY = NMP::clampValue(ellipseTM.yAxis().dot(toPoint), -1.0f + epsilon, 1.0f - epsilon);
  // Clamp result of fastArccos() to it's proper range ([0, pi])
  // fixes an assert where the angle returned by this function finds it's way into another fastTrigXXX()
  // which requires angle >= 0
  float result = NMP::clampValue(NMP::fastArccos(dotWithY), 0.0f, NM_PI) ;
  // Angle from X axis to toPoint. X axis is the horizontal axis for the circle/ellipse (angle Pi/2).
  float dotWithX = NMP::clampValue(ellipseTM.xAxis().dot(toPoint), -1.0f + epsilon, 1.0f - epsilon);
  float fromXToPointAngle = NMP::fastArccos(dotWithX);
  bool onSecondHalfOfcircle = fromXToPointAngle > NM_PI_OVER_TWO; // Means result between pi and 2Pi.
  if (onSecondHalfOfcircle)
  {
    result = 2.0f * NM_PI - result;
  }
  return NMP::clampValue(result, 0.0f, 2.0f * NM_PI);
}

//----------------------------------------------------------------------------------------------------------------------
float Path::clampAngle(const float angle)
{
  float result = angle;
  float maxAngle = 2.0f * NM_PI;

  if (result > maxAngle)
    result -= maxAngle;
  else if (result < 0.0f)
    result += maxAngle;

  return result;
}

} // namespace ER