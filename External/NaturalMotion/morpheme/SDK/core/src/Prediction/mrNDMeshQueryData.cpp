// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#include "NMGeomUtils/NMGeomUtils.h"
#include "morpheme/Prediction/mrScatteredDataUtils.h"
#include "morpheme/Prediction/mrNDMeshQueryData.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
// Projection
//----------------------------------------------------------------------------------------------------------------------
Projection::Projection(
  uint32_t        numVariablesSacrifice,
  const uint32_t* orderOfVariablesSacrifice)
{
  init(numVariablesSacrifice, orderOfVariablesSacrifice);
}

//----------------------------------------------------------------------------------------------------------------------
Projection::Projection(
  uint32_t     numDimensions,
  const float* projectionCentre)
{
  init(numDimensions, projectionCentre);
}

//----------------------------------------------------------------------------------------------------------------------
Projection::Projection(uint32_t numDimensions)
{
  init(numDimensions);
}

//----------------------------------------------------------------------------------------------------------------------
// InterpolateResult
//----------------------------------------------------------------------------------------------------------------------
void InterpolateResult::init(
  uint32_t        numDimensions,
  const uint32_t* queryPointComponentIndices,
  const float*    queryPoint,
  bool            enableExtrapolation)
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(queryPoint);

  // Input data
  m_numDimensions = numDimensions;
  m_numCellVertices = 1 << numDimensions;
  m_queryPointComponentIndices = queryPointComponentIndices;
  m_queryPoint = queryPoint;

  // NDCell vertex sample data
  for (uint32_t i = 0; i < m_numCellVertices; ++i)
  {
    float* v = m_vertexData[i];
    m_vertexComponents[i] = v;
  }

  // Solver data
  m_solverData.init(10, enableExtrapolation, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
// ProjectionBasis
//----------------------------------------------------------------------------------------------------------------------
void ProjectionBasis::initAlongDimension(
  uint32_t      numDimensions,
  uint32_t      whichVariableToCompromise)
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(whichVariableToCompromise < numDimensions);

  // Input data
  m_projectionBasisType = kProjectAlongOneDimension;
  m_numDimensions = numDimensions;

  // AABB ray-cast component test order (Note these are the component indices of
  // the orthogonal complement basis directions).
  m_numRayComponents = 0;
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    m_projectionOrigin[i] = 0.0f;
    if (i != whichVariableToCompromise)
    {
      m_rayComponentOrder[m_numRayComponents] = i;
      m_numRayComponents++;
    }
  }
  m_rayComponentOrder[m_numRayComponents] = whichVariableToCompromise;
}

//----------------------------------------------------------------------------------------------------------------------
void ProjectionBasis::initAlongDimension(
  uint32_t      numDimensions,
  const float*  queryPoint,
  uint32_t      whichVariableToCompromise)
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(whichVariableToCompromise < numDimensions);

  // Input data
  m_projectionBasisType = kProjectAlongOneDimension;
  m_numDimensions = numDimensions;

  // AABB ray-cast component test order (Note these are the component indices of
  // the orthogonal complement basis directions).
  m_numRayComponents = 0;
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    m_projectionOrigin[i] = queryPoint[i];
    if (i != whichVariableToCompromise)
    {
      m_rayComponentOrder[m_numRayComponents] = i;
      m_numRayComponents++;
    }
  }
  m_rayComponentOrder[m_numRayComponents] = whichVariableToCompromise;
}

//----------------------------------------------------------------------------------------------------------------------
void ProjectionBasis::initThroughPointProjectionDirectionOnly(
  uint32_t        numDimensions,
  const float*    queryPoint,
  const float*    projectionCentre)
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(projectionCentre);

  // Input data
  m_projectionBasisType = kProjectInDirection;
  m_numDimensions = numDimensions;
  uint32_t M = numDimensions - 1;

  // Initialise the projection basis
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    m_projectionOrigin[dim] = queryPoint[dim];
    float* v = m_projectionBasisData[dim];
    m_projectionBasis[dim] = v;
  }

  //------------------------------
  // Compute the projection direction
  float* ray = m_projectionBasis[M];
  float sqrMag = 0.0f;
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    ray[dim] = projectionCentre[dim] - queryPoint[dim];
    sqrMag += NMP::sqr(ray[dim]);
  }
  m_projectionDist = sqrtf(sqrMag);

  // Normalise.
  if (m_projectionDist < 1e-7f)
  {
    // For the degenerate case choose any direction, it doesn't matter.
    ray[0] = 1.0f;
    for (uint32_t dim = 1; dim < m_numDimensions; ++dim)
    {
      ray[dim] = 0.0f;
    }
  }
  else
  {
    float fac = 1.0f / m_projectionDist;
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      ray[dim] *= fac;
    }
  }

  //------------------------------
  // Order the components of the projection ray by magnitude  
  float mags[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    mags[dim] = NMP::sqr(ray[dim]);
    m_rayComponentOrder[dim] = dim;
  }

  // Partial sort
  for (m_numRayComponents = 0; m_numRayComponents < m_numDimensions; ++m_numRayComponents)
  {
    for (uint32_t j = m_numRayComponents + 1; j < m_numDimensions; ++j)
    {
      if (mags[j] > mags[m_numRayComponents])
      {
        NMP::nmSwap(mags[m_numRayComponents], mags[j]);
        NMP::nmSwap(m_rayComponentOrder[m_numRayComponents], m_rayComponentOrder[j]);
      }
    }

    if (mags[m_numRayComponents] < 1e-7f)
    {
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ProjectionBasis::initThroughPoint(
  uint32_t      numDimensions,
  const float*  queryPoint,
  const float*  projectionCentre)
{
  // Initialise the principal ray direction
  initThroughPointProjectionDirectionOnly(
    numDimensions,
    queryPoint,
    projectionCentre);

  //------------------------------
  // Compute the orthogonal complement to the projection direction. We do this by computing
  // the householder matrix H that maps the projection direction x back onto the e1 axis.
  // The householder matrix is symmetric and orthonormal so the 2:N rows or columns contain
  // the orthogonal complement directions.
  // H = I - (2 / v^T * v) * (v * v^T)
  // where the Householder vector v = x +/- ||x|| . e1
  // and e1 is the first principal axis direction: e1 = I(:,1)

  // We should choose the sign of v = x +/- ||x|| . e1 appropriately to avoid
  // the Householder vector v becoming the zero vector.
  const float* ray = getProjectionDirection();
  float v1 = ray[0] + NMP::floatSelect(ray[0], 1.0f, -1.0f);

  // v^T * v
  float vtv = v1 * v1;
  for (uint32_t dim = 1; dim < m_numDimensions; ++dim)
  {
    vtv += NMP::sqr(ray[dim]);
  }

  // Scale factor
  float s = -2.0f / vtv;

  // v * v^T: Remaining column vectors
  for (uint32_t dim = 1; dim < m_numDimensions; ++dim)
  {
    // NOTE: we permute the order of the orthonormal basis vectors so that the
    // principal ray direction appears in the last column vector of H. The first
    // (N-1) vectors then encode the orthogonal complement.
    float* u = m_projectionBasis[dim - 1];
    u[0] = s * v1 * ray[dim];
    for (uint32_t i = 1; i < m_numDimensions; ++i)
    {
      u[i] = s * ray[i] * ray[dim];
    }

    // Add identity matrix component
    u[dim] += 1.0f;
  }

  // Check for an orthonormal basis
#ifdef NM_DEBUG
  for (uint32_t dim = 0; dim < m_numDimensions - 1; ++dim)
  {
    float dot = 0.0f;
    const float* u = m_projectionBasis[dim];
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      dot += ray[i] * u[i];
    }
    NMP_ASSERT(NMP::nmfabs(dot) < 1e-5f);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void ProjectionBasis::transform(
  uint32_t numVertices,
  const float* const* inputVertexComponents,
  float* const* outputVertexComponents) const
{
  NMP_ASSERT(m_numDimensions > 0 && m_numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(inputVertexComponents);
  NMP_ASSERT(outputVertexComponents);

  if (m_projectionBasisType == kProjectAlongOneDimension)
  {
    for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
    {
      const float* vertexSrc = inputVertexComponents[vertex];
      float* vertexDst = outputVertexComponents[vertex];

      for (uint32_t basisIndex = 0; basisIndex < m_numDimensions; ++basisIndex)
      {
        uint32_t dim = m_rayComponentOrder[basisIndex];
        NMP_ASSERT(dim < m_numDimensions);
        vertexDst[basisIndex] = vertexSrc[dim] - m_projectionOrigin[dim];
      }
    }
  }
  else
  {
    NMP_ASSERT(m_projectionBasisType == kProjectInDirection);
    float vertexTemp[SCATTERED_DATA_MAX_DIM];
    for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
    {
      const float* vertexSrc = inputVertexComponents[vertex];
      float* vertexDst = outputVertexComponents[vertex];

      // Make relative to the projection origin
      for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
      {
        vertexTemp[dim] = vertexSrc[dim] - m_projectionOrigin[dim];
      }

      // Components in each of the projection basis directions
      for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
      {
        const float* basisDir = m_projectionBasis[dim];
        float dot = 0.0f;
        for (uint32_t i = 0; i < m_numDimensions; ++i)
        {
          dot += vertexTemp[i] * basisDir[i];
        }
        vertexDst[dim] = dot;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
float ProjectionBasis::distanceAlongProjectionDirection(
  const float* vertex) const
{
  NMP_ASSERT(vertex);
  if (m_projectionBasisType == kProjectAlongOneDimension)
  {
    uint32_t whichDim = getWhichVariableToCompromise();
    NMP_ASSERT(whichDim < m_numDimensions);
    return vertex[whichDim] - m_projectionOrigin[whichDim];
  }
  else
  {
    NMP_ASSERT(m_projectionBasisType == kProjectInDirection);
    float x[SCATTERED_DATA_MAX_DIM];
    const float* projectionDir = getProjectionDirection();

    // Make relative to the projection origin
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      x[dim] = vertex[dim] - m_projectionOrigin[dim];
    }

    // Compute the component in the projection direction
    float dot = 0.0f;
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      dot += (x[i] * projectionDir[i]);
    }

    return dot;
  }
}


//----------------------------------------------------------------------------------------------------------------------
// ProjectRayResult
//----------------------------------------------------------------------------------------------------------------------
void ProjectRayResult::init(
  uint32_t        numDimensions,
  const uint32_t* queryPointComponentIndices,
  bool            enableExtrapolation)
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(queryPointComponentIndices);

  // Input data
  m_numDimensions = numDimensions;
  m_numFacetVertices = 1 << (numDimensions - 1);
  m_queryPointComponentIndices = queryPointComponentIndices;

  // Zero vector
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    m_zeroVector[dim] = 0.0f;
  }

  // Facet vertex samples (N-d samples)
  for (uint32_t i = 0; i < m_numFacetVertices; ++i)
  {
    float* v = m_facetVertexData[i];
    m_facetVertexComponents[i] = v;
  }

  // Projected facet vertices (N-1 samples)
  for (uint32_t i = 0; i < m_numFacetVertices; ++i)
  {
    float* v = m_transformedFacetVertexData[i];
    m_transformedFacetVertexComponents[i] = v;
  }

  // Solver data
  m_solverData.init(10, enableExtrapolation, NULL);
}


//----------------------------------------------------------------------------------------------------------------------
// ProjectRayResultClosest
//----------------------------------------------------------------------------------------------------------------------
bool ProjectRayResultClosest::preUpdateClosest(
  ProjectRayResultQuery* target)
{
  NMP_ASSERT(target);
  ProjectRayResultClosest* result = (ProjectRayResultClosest*)target;

  // Check if the AABB of the cell facet vertices in the first (N-1) components
  // contains the zero point.
  if (result->aabbFromTransformedVerticesContainsZeroPoint())
  {
    uint32_t M = result->m_numDimensions - 1;
    uint32_t numFacetVertices = result->m_numFacetVertices;

    // Compute the vertex AABB in the projection direction
    float aabbMinVal, aabbMaxVal;
    aabbMinVal = aabbMaxVal = result->m_transformedFacetVertexComponents[0][M];
    for (uint32_t vertex = 1; vertex < numFacetVertices; ++vertex)
    {
      float x = result->m_transformedFacetVertexComponents[vertex][M];
      aabbMinVal = NMP::minimum(x, aabbMinVal);
      aabbMaxVal = NMP::maximum(x, aabbMaxVal);
    }

    // Detect if the AABB crosses the origin
    float absDist;
    if (aabbMinVal < 0.0f && aabbMaxVal >= 0.0f)
    {
      absDist = 0.0f;
    }
    else
    {
      float absMinVal = NMP::nmfabs(aabbMinVal);
      float absMaxVal = NMP::nmfabs(aabbMaxVal);
      absDist = NMP::minimum(absMinVal, absMaxVal);
    }

    // Compare with the current best distance
    if (absDist < result->m_bestDistance + gAbsCellTol)
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void ProjectRayResultClosest::postUpdateClosest(
  ProjectRayResultQuery* target,
  float             dist,
  const uint32_t*   topLeftCoord,
  uint32_t          whichDim,
  uint32_t          whichBoundary,
  const float*      interpolants,
  const float*      projectedPoint)
{
  NMP_ASSERT(target);
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(whichDim < target->getNumDimensions());
  NMP_ASSERT(whichBoundary <= 1);
  NMP_ASSERT(interpolants);
  NMP_ASSERT(projectedPoint);
  ProjectRayResultClosest* result = (ProjectRayResultClosest*)target;

  float absDist = NMP::nmfabs(dist);
  if (absDist < result->m_bestDistance)
  {
    result->m_bestDistance = absDist;

    NDMeshQueryResult* ndMeshQueryResult = result->m_ndMeshQueryResult;
    NMP_ASSERT(ndMeshQueryResult);

    ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCellFacet;
    ndMeshQueryResult->m_whichDim = whichDim;
    ndMeshQueryResult->m_whichBoundary = whichBoundary;

    for (uint32_t i = 0; i < result->m_numDimensions; ++i)
    {
      ndMeshQueryResult->m_topLeftCoord[i] = topLeftCoord[i];
      result->m_projectedPoint[i] = projectedPoint[i];
    }
    for (uint32_t i = 0; i < result->m_numDimensions - 1; ++i)
    {
      ndMeshQueryResult->m_interpolants[i] = interpolants[i];
    }
  }

  result->m_count++;
}

//----------------------------------------------------------------------------------------------------------------------
// ProjectRayResultExtremes
//----------------------------------------------------------------------------------------------------------------------
bool ProjectRayResultExtremes::preUpdateExtremes(
  ProjectRayResultQuery* target)
{
  NMP_ASSERT(target);
  ProjectRayResultExtremes* result = (ProjectRayResultExtremes*)target;

  // Check if the AABB of the cell facet vertices in the first (N-1) components
  // contains the zero point.
  if (result->aabbFromTransformedVerticesContainsZeroPoint())
  {
    uint32_t M = result->m_numDimensions - 1;
    uint32_t numFacetVertices = result->m_numFacetVertices;
    float bestDistanceRear = result->m_bestDistanceRear + gAbsCellTol;
    float bestDistanceFore = result->m_bestDistanceFore - gAbsCellTol;

    // Check if any of the cell facet vertices is closer than the current rear value
    // or further than the current fore value
    for (uint32_t vertex = 0; vertex < numFacetVertices; ++vertex)
    {
      float dist = result->m_transformedFacetVertexComponents[vertex][M];
      if (dist < bestDistanceRear || dist > bestDistanceFore)
      {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void ProjectRayResultExtremes::postUpdateExtremes(
  ProjectRayResultQuery* target,
  float             dist,
  const uint32_t*   topLeftCoord,
  uint32_t          whichDim,
  uint32_t          whichBoundary,
  const float*      interpolants,
  const float*      projectedPoint)
{
  NMP_ASSERT(target);
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(whichDim < target->getNumDimensions());
  NMP_ASSERT(whichBoundary <= 1);
  NMP_ASSERT(interpolants);
  NMP_ASSERT(projectedPoint);
  ProjectRayResultExtremes* result = (ProjectRayResultExtremes*)target;

  //------------------------------
  // Update the extreme point in the rear direction
  if (dist < result->m_bestDistanceRear)
  {
    result->m_bestDistanceRear = dist;

    NDMeshQueryResult* ndMeshQueryResult = result->m_ndMeshQueryResultRear;
    NMP_ASSERT(ndMeshQueryResult);

    ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCellFacet;
    ndMeshQueryResult->m_whichDim = whichDim;
    ndMeshQueryResult->m_whichBoundary = whichBoundary;

    for (uint32_t i = 0; i < result->m_numDimensions; ++i)
    {
      ndMeshQueryResult->m_topLeftCoord[i] = topLeftCoord[i];
      result->m_projectedPointRear[i] = projectedPoint[i];
    }
    for (uint32_t i = 0; i < result->m_numDimensions - 1; ++i)
    {
      ndMeshQueryResult->m_interpolants[i] = interpolants[i];
    }
  }

  //------------------------------
  // Update the extreme point in the fore direction
  if (dist > result->m_bestDistanceFore)
  {
    result->m_bestDistanceFore = dist;

    NDMeshQueryResult* ndMeshQueryResult = result->m_ndMeshQueryResultFore;
    NMP_ASSERT(ndMeshQueryResult);

    ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCellFacet;
    ndMeshQueryResult->m_whichDim = whichDim;
    ndMeshQueryResult->m_whichBoundary = whichBoundary;

    for (uint32_t i = 0; i < result->m_numDimensions; ++i)
    {
      ndMeshQueryResult->m_topLeftCoord[i] = topLeftCoord[i];
      result->m_projectedPointFore[i] = projectedPoint[i];
    }
    for (uint32_t i = 0; i < result->m_numDimensions - 1; ++i)
    {
      ndMeshQueryResult->m_interpolants[i] = interpolants[i];
    }
  }

  result->m_count++;
}


//----------------------------------------------------------------------------------------------------------------------
// AABBIntersectionResult
//----------------------------------------------------------------------------------------------------------------------
void AABBIntersectionResult::init(
  const NDMesh*     nDMesh,
  const uint32_t*   queryPointComponentIndices,
  const float*      aabbComponentMinVals,
  const float*      aabbComponentMaxVals,
  uint32_t          maxNumNodeEntries,
  NDCellEntry*      nodeEntries)
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(aabbComponentMinVals);
  NMP_ASSERT(aabbComponentMaxVals);
  NMP_ASSERT(maxNumNodeEntries > 0);
  NMP_ASSERT(nodeEntries);

  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t numCellVertices = nDMesh->getNumVerticesPerNDCell();

  // Query data
  m_nDMesh = nDMesh;
  m_queryPointComponentIndices = queryPointComponentIndices;
  m_queryAABBMinVals = aabbComponentMinVals;
  m_queryAABBMaxVals = aabbComponentMaxVals;

  // Result data
  m_maxNumNodeEntries = maxNumNodeEntries;
  m_nodeEntries = nodeEntries;
  m_nodeEntryCount = 0;

  // NDCell coordinates and sizes  
  const uint32_t* sampleCountsPerDimension = nDMesh->getSampleCountsPerDimension();
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    m_coordinates[dim] = 0;
    m_cellSizes[dim] = sampleCountsPerDimension[dim] - 1;
  }

  // Dequantised data
  for (uint32_t i = 0; i < numCellVertices; ++i)
  {
    float* v = m_vertexData[i];
    m_vertexComponents[i] = v;
  }

  // Temporary data
  for (uint32_t i = 0; i < numCellVertices; ++i)
  {
    float* v = m_vertexDataTemp[i];
    m_vertexComponentsTemp[i] = v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AABBProjectionResult
//----------------------------------------------------------------------------------------------------------------------
void AABBProjectionResult::initAlongDimension(
  const NDMesh*     nDMesh,
  const uint32_t*   queryPointComponentIndices,
  uint32_t          whichAABBDim,
  const float*      aabbComponentMinVals,
  const float*      aabbComponentMaxVals,
  uint32_t          maxNumNodeEntries,
  NDCellFacetEntry* nodeEntries)
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(whichAABBDim < nDMesh->getNumDimensions());
  NMP_ASSERT(aabbComponentMinVals);
  NMP_ASSERT(aabbComponentMaxVals);
  NMP_ASSERT(maxNumNodeEntries > 0);
  NMP_ASSERT(nodeEntries);
  NMP_ASSERT(aabbComponentMinVals[whichAABBDim] == aabbComponentMaxVals[whichAABBDim]);

  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t numCellFacetVertices = nDMesh->getNumVerticesPerNDCellFacet();

  // Query data
  m_nDMesh = nDMesh;
  m_queryPointComponentIndices = queryPointComponentIndices;
  m_queryAABBMinVals = aabbComponentMinVals;
  m_queryAABBMaxVals = aabbComponentMaxVals;
  m_whichAABBDim = whichAABBDim;

  // Result data
  m_maxNumNodeEntries = maxNumNodeEntries;
  m_nodeEntryCount = 0;
  m_nodeEntries = nodeEntries;

  // Intermediate data
  uint32_t index = 0;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    m_tmT[i] = 0.0f;

    if (i != whichAABBDim)
    {
      m_tmP[index] = i;
      m_queryAABBMinValsAdj[index] = aabbComponentMinVals[i];
      m_queryAABBMaxValsAdj[index] = aabbComponentMaxVals[i];
      index++;
    }
  }
  m_tmP[index] = whichAABBDim;
  m_queryAABBMinValsAdj[index] = aabbComponentMinVals[whichAABBDim];
  m_queryAABBMaxValsAdj[index] = aabbComponentMaxVals[whichAABBDim];

  // Parallel projection
  m_tmK[0] = 1.0f;
  m_tmK[1] = 0.0f;
  m_tmK[2] = 1.0f;

  for (uint32_t i = 0; i < numDimensions - 1; ++i)
  {
    m_midPointInterpolants[i] = 0.5f;
  }

  // NDCell coordinates and sizes  
  const uint32_t* sampleCountsPerDimension = nDMesh->getSampleCountsPerDimension();
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    m_coordinates[dim] = 0;
    m_cellSizes[dim] = sampleCountsPerDimension[dim] - 1;
  }

  // The NDCell facet vertex sample data
  for (uint32_t i = 0; i < numCellFacetVertices; ++i)
  {
    float* v = m_facetVertexData[i];
    m_facetVertexComponents[i] = v;

    v = m_facetVertexDataAdj[i];
    m_facetVertexComponentsAdj[i] = v;

    v = m_facetVertexDataTemp[i];
    m_facetVertexComponentsTemp[i] = v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AABBProjectionResult::initThroughPoint(
  const NDMesh*     nDMesh,
  const float*      projectionCentre,
  const uint32_t*   queryPointComponentIndices,
  uint32_t          whichAABBDim,
  const float*      aabbComponentMinVals,
  const float*      aabbComponentMaxVals,
  uint32_t          maxNumNodeEntries,
  NDCellFacetEntry* nodeEntries)
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(projectionCentre);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(whichAABBDim < nDMesh->getNumDimensions());
  NMP_ASSERT(aabbComponentMinVals);
  NMP_ASSERT(aabbComponentMaxVals);
  NMP_ASSERT(maxNumNodeEntries > 0);
  NMP_ASSERT(nodeEntries);
  NMP_ASSERT(aabbComponentMinVals[whichAABBDim] == aabbComponentMaxVals[whichAABBDim]);

  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t numCellFacetVertices = nDMesh->getNumVerticesPerNDCellFacet();

  // Query data
  m_nDMesh = nDMesh;
  m_queryPointComponentIndices = queryPointComponentIndices;
  m_queryAABBMinVals = aabbComponentMinVals;
  m_queryAABBMaxVals = aabbComponentMaxVals;
  m_whichAABBDim = whichAABBDim;

  // Result data
  m_maxNumNodeEntries = maxNumNodeEntries;
  m_nodeEntryCount = 0;
  m_nodeEntries = nodeEntries;

  // Intermediate data
  uint32_t index = 0;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    m_tmT[i] = projectionCentre[i];

    if (i != whichAABBDim)
    {
      m_tmP[index] = i;
      m_queryAABBMinValsAdj[index] = aabbComponentMinVals[i] - projectionCentre[i];
      m_queryAABBMaxValsAdj[index] = aabbComponentMaxVals[i] - projectionCentre[i];
      index++;
    }
  }
  m_tmP[index] = whichAABBDim;
  float f = aabbComponentMinVals[whichAABBDim] - projectionCentre[whichAABBDim];
  m_queryAABBMinValsAdj[index] = m_queryAABBMaxValsAdj[index] = f;

  // Central projection  
  m_tmK[2] = 0.0f;
  m_tmK[1] = NMP::floatSelect(f, 1.0f, -1.0f);
  m_tmK[0] = m_tmK[1] * f;

  for (uint32_t i = 0; i < numDimensions - 1; ++i)
  {
    m_midPointInterpolants[i] = 0.5f;
  }

  // NDCell coordinates and sizes  
  const uint32_t* sampleCountsPerDimension = nDMesh->getSampleCountsPerDimension();
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    m_coordinates[dim] = 0;
    m_cellSizes[dim] = sampleCountsPerDimension[dim] - 1;
  }

  // The NDCell facet vertex sample data
  for (uint32_t i = 0; i < numCellFacetVertices; ++i)
  {
    float* v = m_facetVertexData[i];
    m_facetVertexComponents[i] = v;

    v = m_facetVertexDataAdj[i];
    m_facetVertexComponentsAdj[i] = v;

    v = m_facetVertexDataTemp[i];
    m_facetVertexComponentsTemp[i] = v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AABBProjectionResult::queryAABBIntersectsNDCellFacet(
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary)
{
  uint32_t numDimensions = m_nDMesh->getNumDimensions();
  uint32_t numCellFacetVertices = m_nDMesh->getNumVerticesPerNDCellFacet();

  // Get the cell facet vertex samples - Nd
  m_nDMesh->getNDCellFacetVertexComponents(
    topLeftCoord,
    whichDim,
    whichBoundary,
    numDimensions,
    m_queryPointComponentIndices,
    m_nDMesh->getScalesQuantisedToNorm(),
    m_nDMesh->getOffsetsQuantisedToNorm(),
    m_facetVertexComponents); // Each facet vertex sample is Nd

  // Apply the permutation transform
  for (uint32_t vertex = 0; vertex < numCellFacetVertices; ++vertex)
  {
    const float* vSrc = m_facetVertexComponents[vertex];
    NMP_ASSERT(vSrc);
    float* vDst = m_facetVertexComponentsAdj[vertex];
    NMP_ASSERT(vDst);

    for (uint32_t index = 0; index < numDimensions; ++index)
    {
      uint32_t dim = m_tmP[index];
      vDst[index] = vSrc[dim] - m_tmT[dim];
    }
  }

  // Check if the projected NDCell facet intersects the query AABB
  return ScatteredData::aabbIntersectsProjectedNDCellFacet(
    numDimensions,
    m_queryAABBMinValsAdj,
    m_queryAABBMaxValsAdj,
    m_facetVertexComponentsAdj,
    m_facetVertexComponentsTemp,
    m_facetVertexValidTemp,
    m_tmK);
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
