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
#ifndef NM_TRIANGULATION_2D_H
#define NM_TRIANGULATION_2D_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::NMTriangulation2D
/// \brief A class that triangulates a 2D point cloud into its constituent vertices and triangle facets.
/// \ingroup Maths
//----------------------------------------------------------------------------------------------------------------------
class NMTriangulation2D
{
public:
  NMTriangulation2D();
  ~NMTriangulation2D();
  
  void release();

  bool compute(
    uint32_t numPoints,
    const float* X,
    const float* Y);
    
  void computeTriangleIndicesCCW();

  NM_INLINE uint32_t getNumVertices() const;
  NM_INLINE const float* getX() const;
  NM_INLINE const float* getY() const;
  NM_INLINE float getX(uint32_t i) const;
  NM_INLINE float getY(uint32_t i) const;
  
  NM_INLINE uint32_t getNumTriangles() const;
  NM_INLINE const uint32_t* getTriangleIndices(uint32_t component) const;
  NM_INLINE uint32_t getTriangleIndex(uint32_t component, uint32_t element) const;

protected:
  // Vertices
  uint32_t    m_numVertices;
  float*      m_X;
  float*      m_Y;
  
  // Triangles
  uint32_t    m_numTriangles;
  uint32_t*   m_triangleIndices[3];
};


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NMTriangulation2D::getNumVertices() const
{
  return m_numVertices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NMTriangulation2D::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NMTriangulation2D::getY() const
{
  return m_Y;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NMTriangulation2D::getX(uint32_t i) const
{
  NMP_ASSERT(i < m_numVertices);
  return m_X[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NMTriangulation2D::getY(uint32_t i) const
{
  NMP_ASSERT(i < m_numVertices);
  return m_Y[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NMTriangulation2D::getNumTriangles() const
{
  return m_numTriangles;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NMTriangulation2D::getTriangleIndices(uint32_t component) const
{
  NMP_ASSERT(component < 3);
  return m_triangleIndices[component];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NMTriangulation2D::getTriangleIndex(uint32_t component, uint32_t element) const
{
  NMP_ASSERT(component < 3);
  NMP_ASSERT(element < m_numTriangles);
  return m_triangleIndices[component][element];
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_TRIANGULATION_2D_H
//----------------------------------------------------------------------------------------------------------------------
