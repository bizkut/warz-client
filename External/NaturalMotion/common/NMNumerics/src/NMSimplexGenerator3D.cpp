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
#include "NMNumerics/NMSimplexGenerator3D.h"

extern "C" {
#include "libqhull/qhull_a.h"
}

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NMSimplexGenerator3D::NMSimplexGenerator3D() :
  m_numVertices(0), m_numSimplexes(0),
  m_X(0), m_Y(0), m_Z(0)
{
  m_simplexIndices[0] = NULL;
  m_simplexIndices[1] = NULL;
  m_simplexIndices[2] = NULL;
  m_simplexIndices[3] = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NMSimplexGenerator3D::~NMSimplexGenerator3D()
{
  release();
}

//----------------------------------------------------------------------------------------------------------------------
void NMSimplexGenerator3D::release()
{
  m_numVertices = 0;
  m_numSimplexes = 0;

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
  
  if (m_Z)
  {
    delete[] m_Z;
    m_Z = NULL;
  }

  if (m_simplexIndices[0])
  {
    delete[] m_simplexIndices[0];
    m_simplexIndices[0] = NULL;
  }
  
  if (m_simplexIndices[1])
  {
    delete[] m_simplexIndices[1];
    m_simplexIndices[1] = NULL;
  }
  
  if (m_simplexIndices[2])
  {
    delete[] m_simplexIndices[2];
    m_simplexIndices[2] = NULL;
  }
  
  if (m_simplexIndices[3])
  {
    delete[] m_simplexIndices[3];
    m_simplexIndices[3] = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NMSimplexGenerator3D::compute(
  uint32_t numPoints,
  const float* X,
  const float* Y,
  const float* Z)
{
  NMP_ASSERT(X);
  NMP_ASSERT(Y);
  NMP_ASSERT(Z);

  // Tidy up the old data
  release();

  // Validate the new data
  if (numPoints < 4)
    return false;

  // Check for a single simplex. QHull will need a minimum of 4-points
  // to form a single simplex
  if (numPoints == 4)
  {
    // Set the vertices
    m_numVertices = numPoints;
    m_X = new float[numPoints];
    m_Y = new float[numPoints];
    m_Z = new float[numPoints];
    for (uint32_t i = 0; i < numPoints; ++i)
    {
      m_X[i] = X[i];
      m_Y[i] = Y[i];
      m_Z[i] = Z[i];
    }

    // Set the triangle facet indices
    m_numSimplexes = 1;
    m_simplexIndices[0] = new uint32_t[m_numSimplexes];
    m_simplexIndices[1] = new uint32_t[m_numSimplexes];
    m_simplexIndices[2] = new uint32_t[m_numSimplexes];
    m_simplexIndices[3] = new uint32_t[m_numSimplexes];
    m_simplexIndices[0][0] = 0;
    m_simplexIndices[1][0] = 1;
    m_simplexIndices[2][0] = 2;
    m_simplexIndices[3][0] = 3;

    return true;
  }

  //----------------------------------
  // Convert the input samples into the form required by Qhull
  coordT* points = new coordT[3*numPoints];
  for (uint32_t i = 0; i < numPoints; ++i)
  {
    coordT* pt = &points[3*i];
    pt[0] = (coordT)X[i];
    pt[1] = (coordT)Y[i];
    pt[2] = (coordT)Z[i];
  }

  //----------------------------------
  // Qhull processing
  char flags[] = "qhull d Qt Qbb Qc Qz"; // Delaunay triangulation
  int exitcode = qh_new_qhull (3, numPoints, points, False, flags, NULL, stderr);
  
  bool result = false;
  if (!exitcode)
  {
    qh_prepare_output();

    // Count the number of unique vertices
    uint32_t numVertices = 0;
    vertexT *vertex, **vertexp;
    FORALLvertices
    {
      // Don't count the extra point (at infinity) added for the Qz option
      uint32_t id = qh_pointid(vertex->point);
      if (id < numPoints)
      {
        numVertices++;
      }
    }

    // Check if some points were removed because they were coincident
    if (numVertices == numPoints)
    {
      // Compute the number of simplexes
      facetT *facet;
      uint32_t numSimplexes = 0;
      FORALLfacets
      {      
        if (!qh_skipfacet(facet))
        {
          numSimplexes++;
        }
      }
      NMP_ASSERT(numSimplexes > 0);

      // Set the vertices
      m_numVertices = numPoints;
      m_X = new float[numPoints];
      m_Y = new float[numPoints];
      m_Z = new float[numPoints];
      for (uint32_t i = 0; i < numPoints; ++i)
      {
        m_X[i] = X[i];
        m_Y[i] = Y[i];
        m_Z[i] = Z[i];
      }

      // Set the simplex indices
      m_numSimplexes = numSimplexes;
      m_simplexIndices[0] = new uint32_t[m_numSimplexes];
      m_simplexIndices[1] = new uint32_t[m_numSimplexes];
      m_simplexIndices[2] = new uint32_t[m_numSimplexes];
      m_simplexIndices[3] = new uint32_t[m_numSimplexes];

      uint32_t element = 0;
      FORALLfacets
      {
        if (!qh_skipfacet(facet))
        {
          uint32_t component = 0;
          FOREACHvertex_(facet->vertices)
          {
            NMP_ASSERT(component < 4);
            uint32_t id = qh_pointid(vertex->point);
            NMP_VERIFY(id < numPoints); // Shouldn't be the added Qz point (at infinity)
            m_simplexIndices[component][element] = id;

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

} // namespace NMP
