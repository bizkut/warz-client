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
#include "NMPlatform/NMMathUtils.h"
#include "NMNumerics/NMTriangulation2D.h"

extern "C" {
#include "libqhull/qhull_a.h"
}

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NMTriangulation2D::NMTriangulation2D() :
  m_numVertices(0), m_numTriangles(0),
  m_X(0), m_Y(0)
{
  m_triangleIndices[0] = NULL;
  m_triangleIndices[1] = NULL;
  m_triangleIndices[2] = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NMTriangulation2D::~NMTriangulation2D()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void NMTriangulation2D::release()
{
  m_numVertices = 0;
  m_numTriangles = 0;

  if (m_X)
  {
    delete[] m_X;
    m_X = NULL;
  }
  
  if (m_Y)
  {
    delete[] m_Y;
    m_Y = NULL;
  }
  
  if (m_triangleIndices[0])
  {
    delete[] m_triangleIndices[0];
    m_triangleIndices[0] = NULL;
  }
  
  if (m_triangleIndices[1])
  {
    delete[] m_triangleIndices[1];
    m_triangleIndices[1] = NULL;
  }
  
  if (m_triangleIndices[2])
  {
    delete[] m_triangleIndices[2];
    m_triangleIndices[2] = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NMTriangulation2D::compute(
  uint32_t numPoints,
  const float* X,
  const float* Y)
{
  NMP_ASSERT(X);
  NMP_ASSERT(Y);

  // Tidy up the old data
  release();

  // Validate the new data
  if (numPoints < 3)
    return false;

  // Check for a single triangle. QHull will need a minimum of 3-points
  // to form a single simplex
  if (numPoints == 3)
  {
    // Set the vertices
    m_numVertices = numPoints;
    m_X = new float[numPoints];
    m_Y = new float[numPoints];
    for (uint32_t i = 0; i < numPoints; ++i)
    {
      m_X[i] = X[i];
      m_Y[i] = Y[i];
    }

    // Set the triangle facet indices
    m_numTriangles = 1;
    m_triangleIndices[0] = new uint32_t[m_numTriangles];
    m_triangleIndices[1] = new uint32_t[m_numTriangles];
    m_triangleIndices[2] = new uint32_t[m_numTriangles];
    m_triangleIndices[0][0] = 0;
    m_triangleIndices[1][0] = 1;
    m_triangleIndices[2][0] = 2;

    return true;
  }

  //----------------------------------
  // Convert the input samples into the form required by Qhull
  coordT* points = new coordT[2*numPoints];
  for (uint32_t i = 0; i < numPoints; ++i)
  {
    coordT* pt = &points[2*i];
    pt[0] = (coordT)X[i];
    pt[1] = (coordT)Y[i];
  }

  //----------------------------------
  // Qhull processing
  char flags[] = "qhull d Qbb Qt"; // Delaunay triangulation
  int exitcode = qh_new_qhull (2, numPoints, points, False, flags, NULL, stderr);
  
  bool result = false;
  if (!exitcode)
  {
    qh_prepare_output();
    
    // Count the number of unique vertices
    uint32_t numVertices = 0;
    vertexT *vertex, **vertexp;
    FORALLvertices
    {
      numVertices++;
    }
    
    // Check if some points were removed because they were coincident
    if (numVertices == numPoints)
    {
      // Compute the number of triangle facets
      facetT *facet;
      uint32_t numTriangles = 0;
      FORALLfacets
      {
        if (!qh_skipfacet(facet))
        {
          numTriangles++;
        }
      }
      NMP_ASSERT(numTriangles > 0);
    
      // Set the vertices
      m_numVertices = numPoints;
      m_X = new float[numPoints];
      m_Y = new float[numPoints];
      for (uint32_t i = 0; i < numPoints; ++i)
      {
        m_X[i] = X[i];
        m_Y[i] = Y[i];
      }
    
      // Set the triangle facet indices
      m_numTriangles = numTriangles;
      m_triangleIndices[0] = new uint32_t[m_numTriangles];
      m_triangleIndices[1] = new uint32_t[m_numTriangles];
      m_triangleIndices[2] = new uint32_t[m_numTriangles];

      uint32_t element = 0;
      FORALLfacets
      {
        if (!qh_skipfacet(facet))
        {
          uint32_t component = 0;
          FOREACHvertex_(facet->vertices)
          {
            NMP_ASSERT(component < 3);
            uint32_t id = qh_pointid(vertex->point);
            m_triangleIndices[component][element] = id;
            component++;            
          }
          element++;
        }
      }

      result = true;
    }
  }

  //----------------------------------
  // Tidy up
  qh_freeqhull(!qh_ALL);                  // free long memory
  int curlong, totlong;                   // memory remaining after qh_memfreeshort
  qh_memfreeshort (&curlong, &totlong);   // free short memory and memory allocator

  delete[] points;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NMTriangulation2D::computeTriangleIndicesCCW()
{
  for (uint32_t i = 0; i < m_numTriangles; ++i)
  {
    uint32_t& indexA = m_triangleIndices[0][i];
    uint32_t& indexB = m_triangleIndices[1][i];
    uint32_t& indexC = m_triangleIndices[2][i];

    float dxAB = m_X[indexB] - m_X[indexA];
    float dyAB = m_Y[indexB] - m_Y[indexA];
    float dxAC = m_X[indexC] - m_X[indexA];
    float dyAC = m_Y[indexC] - m_Y[indexA];

    // Directed orientation between the two vectors
    float crossZ = dxAB * dyAC - dxAC * dyAB;
    if (crossZ < 0)
    {
      NMP::nmSwap(indexB, indexC);
    }
  }
}

} // namespace NMP
