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
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
template <>
void convertToLeftTransform(const Matrix34& H, Matrix<ScalarType>& R, Vector<ScalarType>& t)
{
  NMP_ASSERT(R.isSize(3, 3));
  NMP_ASSERT(t.isSize(3));

  ScalarType v[3];
  for (uint32_t i = 0; i < 3; ++i)
  {
    const Vector3& p = H.r[i];
    for (uint32_t k = 0; k < 3; ++k) v[k] = (ScalarType)p[k];
    R.setColumnVector(i, v);
  }

  const Vector3& p = H.translation();
  for (uint32_t k = 0; k < 3; ++k) t[k] = (ScalarType)p[k];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void convertFromLeftTransform(Matrix34& H, const Matrix<ScalarType>& R, const Vector<ScalarType>& t)
{
  NMP_ASSERT(R.isSize(3, 3));
  NMP_ASSERT(t.isSize(3));

  ScalarType v[3];
  for (uint32_t i = 0; i < 3; ++i)
  {
    R.getColumnVector(i, v);
    Vector3& p = H.r[i];
    for (uint32_t k = 0; k < 3; ++k) p[k] = (float)v[k];
  }

  Vector3& p = H.translation();
  for (uint32_t k = 0; k < 3; ++k) p[k] = (float)t[k];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t cycleAngle(ScalarType& theta)
{
  const ScalarType pi = static_cast<ScalarType>(3.1415926535897932384626433832795);
  const ScalarType pi2 = 2 * pi;

  int32_t seg = (int32_t)floor((theta + pi) / pi2);
  theta -= (seg * pi2);
  return seg;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t closestAngle(const ScalarType baseAng, ScalarType& ang)
{
  const ScalarType pi = static_cast<ScalarType>(3.1415926535897932384626433832795);
  const ScalarType pi2 = 2 * pi;

  // Make angle lie in the same cycle as the base angle
  int32_t seg = (int32_t)floor((baseAng + pi) / pi2);
  int32_t seg2 = (int32_t)floor((ang + pi) / pi2);
  ang += pi2 * (seg - seg2);

  // Although the angle now resides in the same cycle as
  // the base angle the two angles could differ by more than pi
  ScalarType d = ang - baseAng;
  if (fabs(d) > pi)
  {
    if (d > 0)
    {
      ang -= pi2;
      seg--;
    }
    else
    {
      ang += pi2;
      seg++;
    }
  }

  return seg;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
