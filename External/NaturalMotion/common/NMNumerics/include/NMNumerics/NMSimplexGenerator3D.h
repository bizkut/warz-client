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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_SIMPLEX_GENERATOR_3D_H
#define NM_SIMPLEX_GENERATOR_3D_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::NMSimplexGenerator3D
/// \brief A class that generates tetrahedral simplexes from a point cloud.
/// \ingroup Maths
//----------------------------------------------------------------------------------------------------------------------
class NMSimplexGenerator3D
{
public:
  NMSimplexGenerator3D();
  ~NMSimplexGenerator3D();

  void release();

  bool compute(
    uint32_t numPoints,
    const float* X,
    const float* Y,
    const float* Z);

  NM_INLINE uint32_t getNumVertices() const;
  NM_INLINE const float* getX() const;
  NM_INLINE const float* getY() const;
  NM_INLINE const float* getZ() const;
  NM_INLINE float getX(uint32_t i) const;
  NM_INLINE float getY(uint32_t i) const;
  NM_INLINE float getZ(uint32_t i) const;

  NM_INLINE uint32_t getNumSimplexes() const;
  NM_INLINE const uint32_t* getSimplexIndices(uint32_t component) const;
  NM_INLINE uint32_t getSimplexIndex(uint32_t component, uint32_t element) const;

protected:
  // Vertices
  uint32_t    m_numVertices;
  float*      m_X;
  float*      m_Y;
  float*      m_Z;

  // Simplexes
  uint32_t    m_numSimplexes;
  uint32_t*   m_simplexIndices[4];
};


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NMSimplexGenerator3D::getNumVertices() const
{
  return m_numVertices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NMSimplexGenerator3D::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NMSimplexGenerator3D::getY() const
{
  return m_Y;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NMSimplexGenerator3D::getZ() const
{
  return m_Z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NMSimplexGenerator3D::getX(uint32_t i) const
{
  NMP_ASSERT(i < m_numVertices);
  return m_X[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NMSimplexGenerator3D::getY(uint32_t i) const
{
  NMP_ASSERT(i < m_numVertices);
  return m_Y[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NMSimplexGenerator3D::getZ(uint32_t i) const
{
  NMP_ASSERT(i < m_numVertices);
  return m_Z[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NMSimplexGenerator3D::getNumSimplexes() const
{
  return m_numSimplexes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NMSimplexGenerator3D::getSimplexIndices(uint32_t component) const
{
  NMP_ASSERT(component < 4);
  return m_simplexIndices[component];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NMSimplexGenerator3D::getSimplexIndex(uint32_t component, uint32_t element) const
{
  NMP_ASSERT(component < 4);
  NMP_ASSERT(element < m_numSimplexes);
  return m_simplexIndices[component][element];
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_SIMPLEX_GENERATOR_3D_H
//----------------------------------------------------------------------------------------------------------------------
