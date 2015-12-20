// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMathUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

// indirection alignment of formal parameter 1 (16) is
// greater than the actual argument alignment (8)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4328)
#endif

// A division by a value below this limit is deemed to have the possibility of giving us an inaccurate result.
#define ACCURATE_DIVISION_ERROR_LIMIT (0.000001f)

//----------------------------------------------------------------------------------------------------------------------
bool Vector3::compare(const NMP::Vector3& a, const NMP::Vector3& b, const float tolerance)
{
  if (NMP::nmfabs(a.x - b.x) > tolerance)
    return false;

  if (NMP::nmfabs(a.y - b.y) > tolerance)
    return false;

  if (NMP::nmfabs(a.z - b.z) > tolerance)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3::transform(const Matrix34& m)
{
  m.transformVector(*this);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3::rotate(const Matrix34& m)
{
  m.rotateVector(*this);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3::inverseTransform(const Matrix34& m)
{
  m.inverseTransformVector(*this);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3::inverseRotate(const Matrix34& m)
{
  m.inverseRotateVector(*this);
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 vTransform(const NMP::Vector3& v, const Matrix34& m)
{
  Vector3 result;
  m.transformVector(v, result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 vInverseTransform(const NMP::Vector3& v, const Matrix34& m)
{
  Vector3 result;
  m.inverseTransformVector(v, result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 vRotate(const NMP::Vector3& v, const Matrix34& m)
{
  Vector3 result;
  m.rotateVector(v, result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 vInverseRotate(const NMP::Vector3& v, const Matrix34& m)
{
  Vector3 result;
  m.inverseRotateVector(v, result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void vBounds(uint32_t n, const Vector3* v, Vector3& minV, Vector3& maxV)
{
  NMP_ASSERT(n > 0);
  NMP_ASSERT(v);
  minV.set(v[0].x, v[0].y, v[0].z);
  maxV.set(v[0].x, v[0].y, v[0].z);

  for (uint32_t i = 1; i < n; ++i)
  {
    // X component
    if (v[i].x < minV.x)
    {
      minV.x = v[i].x;
    }
    else if (v[i].x > maxV.x)
    {
      maxV.x = v[i].x;
    }

    // Y Component
    if (v[i].y < minV.y)
    {
      minV.y = v[i].y;
    }
    else if (v[i].y > maxV.y)
    {
      maxV.y = v[i].y;
    }

    // Z Component
    if (v[i].z < minV.z)
    {
      minV.z = v[i].z;
    }
    else if (v[i].z > maxV.z)
    {
      maxV.z = v[i].z;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Slerp interpolate between the 2 input vectors and produce a vector result.
// Linearly interpolates the angle between the inputs and the length of the inputs.
// NOTE: Result becomes inaccurate as either input vector length tends to zero or if vectors lie parallel and in
// diametrically opposite directions.
void Vector3::slerp(const NMP::Vector3& sourceVec0, const NMP::Vector3& sourceVec1, float alpha)
{
  NMP::Vector3  vec0;
  NMP::Vector3  vec1;
  NMP::Vector3  axisOfRotation;
  float         scaledRotationAngle;
  NMP::Quat     scaledRotationQuat;
  float         vec0Length;
  float         vec1Length;
  float         recipVec0Len;
  float         recipVec1Len;
  float         interpolatedLength;

  vec0Length = sourceVec0.magnitude();
  vec1Length = sourceVec1.magnitude();

  if (vec0Length < ACCURATE_DIVISION_ERROR_LIMIT || vec1Length < ACCURATE_DIVISION_ERROR_LIMIT)
  {
    // One or both vectors are too short to do an accurate slerp, so just approximate with a lerp.
    lerp(sourceVec0, sourceVec1, alpha);
  }
  else
  {
    // Find required length of result.
    interpolatedLength = vec0Length + ((vec1Length - vec0Length) * alpha);

    // Normalise source vectors
    recipVec0Len = 1.0f / vec0Length;
    vec0 = sourceVec0 * recipVec0Len;
    recipVec1Len = 1.0f / vec1Length;
    vec1 = sourceVec1 * recipVec1Len;

    // Find angle of rotation between source vectors
    float dotProd = vec0.dot(vec1);
    // We must normalise the dot product in case of rounding errors.
    if (dotProd > 1.0f)
      dotProd = 1.0f;
    if (dotProd < -1.0f)
      dotProd = -1.0f;

    if (NMP::nmfabs((NMP::nmfabs(dotProd) - 1.0f)) < ACCURATE_DIVISION_ERROR_LIMIT)
    {
      // Input vectors lie almost parallel (in same direction or diametrically opposite) so are too close together
      // to give an accurate slerp so just lerp them.
      lerp(sourceVec0, sourceVec1, alpha);
    }
    else
    {
      scaledRotationAngle = acosf(dotProd) * alpha;

      NMP_ASSERT(scaledRotationAngle == scaledRotationAngle);

      // Find axis of rotation between source vectors.
      axisOfRotation.cross(vec0, vec1);
      axisOfRotation.normalise();

      // Scale the rotation angle based on the weighting.
      scaledRotationQuat.fromAxisAngle(axisOfRotation, scaledRotationAngle);

      // Rotate vec0 to get the new direction vector.
      scaledRotationQuat.rotateVector(vec0, *this);

      // Scale the direction vector to the correct length.
      (*this) *= interpolatedLength;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NMP::Vector3::isValid() const
{
  return ((x * 0 == 0) && (y * 0 == 0) && (z * 0 == 0));
}

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------

