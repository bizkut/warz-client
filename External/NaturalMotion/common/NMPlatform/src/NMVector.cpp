// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// Currently, no Vector methods are inlined, so Vector.inl is included here and
// not from Vector.h

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMFile.h"

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

#define ScalarType float
#define FloatSpecific
#include "NMVector_imp.cpp"
#undef FloatSpecific
#undef ScalarType

#define ScalarType double
#define FloatSpecific
#include "NMVector_imp.cpp"
#undef FloatSpecific
#undef ScalarType

#define ScalarType int32_t
#define IntegerSpecific
#include "NMVector_imp.cpp"
#undef IntegerSpecific
#undef ScalarType
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Type-specific functions
template <>
Vector<float>::Vector(const Vector3& v) :
  m_numElements(3)
{
  m_elements = new float [m_numElements];
  set(&v.f[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector3 Vector<float>::getVector3() const
{
  NMP_ASSERT(m_numElements == 3);
  Vector3 result;
  result.set(m_elements[0], m_elements[1], m_elements[2]);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Vector<int32_t>::isZero(int32_t threshold) const
{
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    if ((int32_t)abs((float)m_elements[i]) > threshold) return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Vector<int32_t>::isEqual(const Vector& v, int32_t threshold) const
{
  NMP_ASSERT(v.m_numElements == m_numElements);
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    if ((int32_t)abs((float)(m_elements[i] - v.m_elements[i])) > threshold) return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<int32_t>::squareroot()
{
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] = (int32_t)sqrt((float)m_elements[i]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t Vector<int32_t>::sd() const
{
  return (int32_t)sqrt((float)var());
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<float>::fwritef(const char* filename) const
{
  NMFile file;
  file.create(filename);

  char buffer[32];

  for (uint32_t i = 0; i < numElements(); i++)
  {
    NMP_SPRINTF(buffer, 32, "%.6f\n", element(i));
    file.write(buffer, strlen(buffer));
  }

  strcpy(buffer, "\n");
  file.write(buffer, strlen(buffer));

  file.close();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<double>::fwritef(const char* filename) const
{
  NMFile file;
  file.create(filename);

  char buffer[32];

  for (uint32_t i = 0; i < numElements(); i++)
  {
    NMP_SPRINTF(buffer, 32, "%.6f\n", element(i));
    file.write(buffer, strlen(buffer));
  }

  strcpy(buffer, "\n");
  file.write(buffer, strlen(buffer));

  file.close();
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
