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
#include "NMPlatform/NMPlatform.h"

#ifdef NM_COMPILER_GHS
// This warning disable is here because the GHS compiler gets confused and emits a warning that some variables are
//  being used before they are set.
#pragma ghs nowarning 549
#endif // NM_COMPILER_GHS

#include "NMPlatform/NMMathUtils.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "morpheme/Prediction/mrScatteredDataUtils.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

#define NM_PROFILE_NDMESH_INTERPOLATIONx
#define NM_PROFILE_NDMESH_SOLVER_INTERNALx

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
// Axis-Aligned Bounding Box functions
//----------------------------------------------------------------------------------------------------------------------
void aabbFromVertices(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* const* vertices,   // IN: the set of working vertices to test
  float* aabbMinVals,             // OUT: the AABB min values
  float* aabbMaxVals)             // OUT: the AABB max values
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(vertices);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);

  // Initialisation
  const float* vertexPtr = vertices[0];
  NMP_ASSERT(vertexPtr);
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    aabbMinVals[dim] = aabbMaxVals[dim] = vertexPtr[dim];
  }

  // Update
  uint32_t numVertices = 1 << numDimensions;
  for (uint32_t vertex = 1; vertex < numVertices; ++vertex)
  {
    vertexPtr = vertices[vertex];
    NMP_ASSERT(vertexPtr);
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      aabbMinVals[dim] = NMP::minimum(vertexPtr[dim], aabbMinVals[dim]);
      aabbMaxVals[dim] = NMP::maximum(vertexPtr[dim], aabbMaxVals[dim]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbFromVertices(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* const* vertices,   // IN: the set of working vertices to test
  const bool* valid,              // IN: the set of working valid vertices
  float* aabbMinVals,             // OUT: the AABB min values
  float* aabbMaxVals)             // OUT: the AABB max values
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(vertices);
  NMP_ASSERT(valid);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);

  uint32_t numVertices = 1 << numDimensions;
  for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
  {
    if (valid[vertex])
    {
      // Initialisation
      const float* vertexPtr = vertices[vertex];
      NMP_ASSERT(vertexPtr);
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        aabbMinVals[dim] = aabbMaxVals[dim] = vertexPtr[dim];
      }

      // Update
      for (++vertex; vertex < numVertices; ++vertex)
      {        
        if (valid[vertex])
        {
          vertexPtr = vertices[vertex];
          NMP_ASSERT(vertexPtr);
          for (uint32_t dim = 0; dim < numDimensions; ++dim)
          {
            aabbMinVals[dim] = NMP::minimum(vertexPtr[dim], aabbMinVals[dim]);
            aabbMaxVals[dim] = NMP::maximum(vertexPtr[dim], aabbMaxVals[dim]);
          }
        }
      }

      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsAnotherAABB(
  uint32_t numDimensions, // IN: the dimensionality of the AABB
  float* aabbMinValsA,    // IN: the AABB min values for the first AABB
  float* aabbMaxValsA,    // IN: the AABB max values for the first AABB
  float* aabbMinValsB,    // IN: the AABB min values for the second AABB
  float* aabbMaxValsB)    // IN: the AABB max values for the second AABB
{
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    if (aabbMaxValsB[dim] < aabbMinValsA[dim] - gAABBTol ||
        aabbMinValsB[dim] > aabbMaxValsA[dim] + gAABBTol)
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsAABBFromVertices(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* aabbMinVals,       // IN: the AABB min values
  const float* aabbMaxVals,       // IN: the AABB max values
  const float* const* vertices)   // IN: the set of working vertices to test
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(vertices);

  //------------------------------
  // Initialisation
  float aabbMinValsAccum[SCATTERED_DATA_MAX_DIM];
  float aabbMaxValsAccum[SCATTERED_DATA_MAX_DIM];
  const float* vertexPtr = vertices[0];
  NMP_ASSERT(vertexPtr);  
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    aabbMinValsAccum[dim] = aabbMaxValsAccum[dim] = vertexPtr[dim];
  }

  //------------------------------
  // Update
  uint32_t numVertices = 1 << numDimensions;
  for (uint32_t vertex = 1; vertex < numVertices; ++vertex)
  {
    vertexPtr = vertices[vertex];
    NMP_ASSERT(vertexPtr);
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      aabbMinValsAccum[dim] = NMP::minimum(vertexPtr[dim], aabbMinValsAccum[dim]);
      aabbMaxValsAccum[dim] = NMP::maximum(vertexPtr[dim], aabbMaxValsAccum[dim]);
    }
  }

  //------------------------------
  // Check if the AABB from the vertices intersects the query AABB
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    if (aabbMaxValsAccum[dim] < aabbMinVals[dim] - gAABBTol ||
        aabbMinValsAccum[dim] > aabbMaxVals[dim] + gAABBTol)
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbContainsAnyVertex(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* aabbMinVals,       // IN: the AABB min values
  const float* aabbMaxVals,       // IN: the AABB max values
  const float* const* vertices)   // IN: the set of working vertices to test
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(vertices);

  //------------------------------
  // Compute the adjusted AABB for numerical tolerance
  float aabbMinValsAdj[SCATTERED_DATA_MAX_DIM];
  float aabbMaxValsAdj[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    aabbMinValsAdj[i] = aabbMinVals[i] - gAABBTol;
    aabbMaxValsAdj[i] = aabbMaxVals[i] + gAABBTol;
  }

  uint32_t numVertices = 1 << numDimensions;
  for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
  {
    const float* vertexPtr = vertices[vertex];
    NMP_ASSERT(vertexPtr);
    bool found = true;
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      if (vertexPtr[dim] < aabbMinValsAdj[dim] ||
          vertexPtr[dim] > aabbMaxValsAdj[dim])
      {
        found = false;
        break;
      }
    }
    if (found)
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbContainsAnyVertex(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* aabbMinVals,       // IN: the AABB min values
  const float* aabbMaxVals,       // IN: the AABB max values
  const float* const* vertices,   // IN: the set of working vertices to test
  const bool* valid)              // IN: the set of working valid vertices
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(vertices);
  NMP_ASSERT(valid);

  //------------------------------
  // Compute the adjusted AABB for numerical tolerance
  float aabbMinValsAdj[SCATTERED_DATA_MAX_DIM];
  float aabbMaxValsAdj[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    aabbMinValsAdj[i] = aabbMinVals[i] - gAABBTol;
    aabbMaxValsAdj[i] = aabbMaxVals[i] + gAABBTol;
  }

  uint32_t numVertices = 1 << numDimensions;
  for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
  {
    if (valid[vertex])
    {
      const float* vertexPtr = vertices[vertex];
      NMP_ASSERT(vertexPtr);
      bool found = true;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        if (vertexPtr[dim] < aabbMinValsAdj[dim] ||
            vertexPtr[dim] > aabbMaxValsAdj[dim])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsRay(
  uint32_t numDimensions,             // IN: the dimensionality of the AABB
  const float* aabbMinVals,           // IN: the AABB min values
  const float* aabbMaxVals,           // IN: the AABB max values
  uint32_t numRayComponents,          // IN: the number of non-zero ray components to test
  const uint32_t* rayComponentOrder,  // IN: vector of ray component indices to test
  const float* projectionOrigin,      // IN: the origin of the projection
  const float* projectionDirection)   // IN: the direction of the ray
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(numRayComponents > 0);
  NMP_ASSERT(rayComponentOrder);
  NMP_ASSERT(projectionOrigin);
  NMP_ASSERT(projectionDirection);

  //------------------------------
  // Compute the adjusted AABB for numerical tolerance
  float aabbMinValsAdj[SCATTERED_DATA_MAX_DIM];
  float aabbMaxValsAdj[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    aabbMinValsAdj[i] = aabbMinVals[i] - gAABBTol;
    aabbMaxValsAdj[i] = aabbMaxVals[i] + gAABBTol;
  }

  //------------------------------
  // Each dimension has two faces. So iterate through each dimension and each facet.
  // Note that we maintain the order in which to test the AABB facet intersection. This
  // allows us to avoid testing against facets that can't be reached due to zero components
  // in the ray direction.  
  bool queryPointInsideAABB = true;
  for (uint32_t index = 0; index < numRayComponents; ++index)
  {
    uint32_t whichDim = rayComponentOrder[index];
    NMP_ASSERT(whichDim < numDimensions);
    NMP_ASSERT_MSG(
      projectionDirection[whichDim] != 0.0f,
      "This function assumes that you've pre-determined the set of non-zero ray components");

    // Left boundary
    if (projectionOrigin[whichDim] < aabbMinVals[whichDim])
    {
      queryPointInsideAABB = false;

      // Project onto facet.
      float distanceFromFace = aabbMinVals[whichDim] - projectionOrigin[whichDim];
      float amountToMoveAlongRay = distanceFromFace / projectionDirection[whichDim];
      bool intersects = true;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        if (dim != whichDim)
        {
          float projectedValue = projectionOrigin[dim] + amountToMoveAlongRay * projectionDirection[dim];
          if (projectedValue < aabbMinValsAdj[dim] || projectedValue > aabbMaxValsAdj[dim])
          {
            intersects = false;
            break;
          }
        }
      }
      if (intersects)
      {
        return true;
      }
    }

    // Right boundary
    else if (projectionOrigin[whichDim] > aabbMaxVals[whichDim])
    {
      queryPointInsideAABB = false;

      // Project onto facet.
      float distanceFromFace = aabbMaxVals[whichDim] - projectionOrigin[whichDim];
      float amountToMoveAlongRay = distanceFromFace / projectionDirection[whichDim];
      bool intersects = true;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        if (dim != whichDim)
        {
          float projectedValue = projectionOrigin[dim] + amountToMoveAlongRay * projectionDirection[dim];
          if (projectedValue < aabbMinValsAdj[dim] || projectedValue > aabbMaxValsAdj[dim])
          {
            intersects = false;
            break;
          }
        }
      }
      if (intersects)
      {
        return true;
      }
    }
  }

  // A ray projected in any direction from inside the AABB will always intersect the AABB.
  // Note: Query points rarely lie within the AABB when performing projection, so this test
  // is performed last to improve performance.
  if (queryPointInsideAABB)
  {
    // We've already tested all the dimensions with non zero ray direction components
    // to see if they lay inside the AABB. We should test the remaining dimensions to
    // check that they are inside the AABB.
    for (uint32_t index = numRayComponents; index < numDimensions; ++index)
    {
      uint32_t whichDim = rayComponentOrder[index];
      NMP_ASSERT(whichDim < numDimensions);

      if (projectionOrigin[whichDim] < aabbMinValsAdj[whichDim] ||
          projectionOrigin[whichDim] > aabbMaxValsAdj[whichDim])
      {
        return false;
      }
    }
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsRayClosestPoint(
  uint32_t numDimensions,             // IN: the dimensionality of the AABB
  const float* aabbMinVals,           // IN: the AABB min values
  const float* aabbMaxVals,           // IN: the AABB max values
  const float* projectionOrigin,      // IN: the origin of the projection
  const float* queryPoint,            // IN: the query point through which to project the ray
  uint32_t& whichAABBDim,             // OUT: whichDim of the AABB identifies the intersecting facet
  uint32_t& whichAABBBoundary,        // OUT: whichBoundary of the AABB identifies the intersecting facet
  float* projectedAABBPoint)          // OUT: the projected point intersecting the AABB facet
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(projectionOrigin);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(projectedAABBPoint);

  bool intersectionResult = false;
  float projectedPoint[SCATTERED_DATA_MAX_DIM];
  float minCost = FLT_MAX;

  //------------------------------
  // Compute the adjusted AABB for numerical tolerance
  float aabbMinValsAdj[SCATTERED_DATA_MAX_DIM];
  float aabbMaxValsAdj[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    aabbMinValsAdj[i] = aabbMinVals[i] - gAABBTol;
    aabbMaxValsAdj[i] = aabbMaxVals[i] + gAABBTol;
  }

  // Compute the projection direction vector
  float projectionDirection[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    projectionDirection[dim] = queryPoint[dim] - projectionOrigin[dim];
  }

  //------------------------------
  // Each dimension has two faces. So iterate through each dimension and each facet
  for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
  {
    // Check if the component in the dimension to compromise can get us to the AABB plane
    if (NMP::nmfabs(projectionDirection[whichDim]) < gAABBTol)
    {
      continue;
    }

    float distanceFromFace, amountToMoveAlongRay;
    bool intersects;

    //------------------------------
    // Left facet
    distanceFromFace = aabbMinVals[whichDim] - projectionOrigin[whichDim];
    amountToMoveAlongRay = distanceFromFace / projectionDirection[whichDim];

    intersects = true;
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      projectedPoint[dim] = projectionOrigin[dim] + amountToMoveAlongRay * projectionDirection[dim];
      if (projectedPoint[dim] < aabbMinValsAdj[dim] || projectedPoint[dim] > aabbMaxValsAdj[dim])
      {
        intersects = false;
        break;
      }
    }
    if (intersects)
    {
      // Update the closest point
      float cost = 0.0f;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        cost += NMP::sqr(projectedPoint[dim] - queryPoint[dim]);
      }

      if (cost < minCost)
      {
        for (uint32_t dim = 0; dim < numDimensions; ++dim)
        {
          projectedAABBPoint[dim] = projectedPoint[dim];
        }
        whichAABBDim = whichDim;
        whichAABBBoundary = 0;
        minCost = cost;
        intersectionResult = true;
      }
    }

    //------------------------------
    // Right facet
    distanceFromFace = aabbMaxVals[whichDim] - projectionOrigin[whichDim];
    amountToMoveAlongRay = distanceFromFace / projectionDirection[whichDim];

    intersects = true;
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      projectedPoint[dim] = projectionOrigin[dim] + amountToMoveAlongRay * projectionDirection[dim];
      if (projectedPoint[dim] < aabbMinValsAdj[dim] || projectedPoint[dim] > aabbMaxValsAdj[dim])
      {
        intersects = false;
        break;
      }
    }
    if (intersects)
    {
      // Update the closest point
      float cost = 0.0f;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        cost += NMP::sqr(projectedPoint[dim] - queryPoint[dim]);
      }

      if (cost < minCost)
      {
        for (uint32_t dim = 0; dim < numDimensions; ++dim)
        {
          projectedAABBPoint[dim] = projectedPoint[dim];
        }
        whichAABBDim = whichDim;
        whichAABBBoundary = 1;
        minCost = cost;
        intersectionResult = true;
      }
    }
  }

  return intersectionResult;
}

//----------------------------------------------------------------------------------------------------------------------
static bool aabbIntersectsNDCellAux(
  uint32_t numDimensions,
  const float* aabbMinVals,
  const float* aabbMaxVals,
  const float* const* vertices,
  float* const* verticesA,
  uint32_t depth,
  const float* aabbFocalPoint,
  const float* weightsMinVals,
  const float* weightsMaxVals)
{
  const uint32_t maxIterations = 10;  // NOTE: This is enough iterations for the vertex interpolants to
                                      // have converged to a single value within 1e-3
  const float alphas[3] = {0.0f, 0.5f, 1.0f};
  const float oma[3] = {1.0f, 0.5f, 0.0f};

  // Check for termination on the maximum number of iterations
  if (depth == maxIterations)
  {
    return false;
  }

  uint32_t numVertices = 1 << numDimensions;
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(numDimensions);

  // State data
  float aabbVerticesMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbVerticesMaxVals[SCATTERED_DATA_MAX_DIM];
  float subWeightsMinVals[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_DIM];
  float subWeightsMaxVals[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_DIM];  
  float aabbIntersectFocalPoints[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_DIM];
  float subCellCosts[SCATTERED_DATA_MAX_CELL_VERTICES];
  bool subCellsValid[SCATTERED_DATA_MAX_CELL_VERTICES];
  uint32_t subCellRankOrder[SCATTERED_DATA_MAX_CELL_VERTICES];
  uint32_t numValidSubCells = 0;

  //------------------------------
  // Test all sub-cells for intersection with the AABB
  for (uint32_t subCellIndex = 0; subCellIndex < numVertices; ++subCellIndex)
  {
    // Compute the bisected set of vertex interpolants for the sub-cell      
    float* curSubWeightsMinVals = subWeightsMinVals[subCellIndex];
    float* curSubWeightsMaxVals = subWeightsMaxVals[subCellIndex];
    float* subWeights[2];
    subWeights[0] = curSubWeightsMinVals;
    subWeights[1] = curSubWeightsMaxVals;

    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      uint32_t index = (subCellIndex >> dim) & 1;
      curSubWeightsMinVals[dim] = oma[index] * weightsMinVals[dim] + alphas[index] * weightsMaxVals[dim];
      curSubWeightsMaxVals[dim] = oma[index + 1] * weightsMinVals[dim] + alphas[index + 1] * weightsMaxVals[dim];
    }

    uint32_t subCellCoordX = subCellIndex & 1;
    if (subCellCoordX > 0)
    {
      // Copy all X+1 vertex entries from the previous update into the X+0 entries.
      // This cuts the number of interpolations required to compute the sub-cell vertices by a quarter.
      // MORPH-21357: Compute all vertices in the sub-cell mesh in one go.
      for (uint32_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex += 2)
      {
        float* vSrc = verticesA[vertexIndex + 1];
        float* vDst = verticesA[vertexIndex];
        for (uint32_t dim = 0; dim < numDimensions; ++dim)
        {
          vDst[dim] = vSrc[dim];
        }
      }

      // Evaluate the X+1 vertex entries
      for (uint32_t vertexIndex = 1; vertexIndex < numVertices; vertexIndex += 2)
      {
        float ts[SCATTERED_DATA_MAX_DIM];
        for (uint32_t dim = 0; dim < numDimensions; ++dim)
        {
          uint32_t vertexCoord = (vertexIndex >> dim) & 1;
          ts[dim] = subWeights[vertexCoord][dim];
        }

        managerDataBlock.m_multilinearInterpFn(
          numDimensions,            // IN: number of dimensions
          numDimensions,            // IN: number of components in each vertex
          verticesA[vertexIndex],   // OUT: the interpolated vertex sample
          ts,                       // IN: the multi-linear weights
          vertices);                // IN: the data vertices to interpolate between
      }
    }
    else
    {
      // We must recompute the X+0 entry since it isn't already available in the X+1 entry
      // of last frame's vertices
      for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
      {
        float ts[SCATTERED_DATA_MAX_DIM];
        for (uint32_t dim = 0; dim < numDimensions; ++dim)
        {
          uint32_t vertexCoord = (vertexIndex >> dim) & 1;
          ts[dim] = subWeights[vertexCoord][dim];
        }

        managerDataBlock.m_multilinearInterpFn(
          numDimensions,            // IN: number of dimensions
          numDimensions,            // IN: number of components in each vertex
          verticesA[vertexIndex],   // OUT: the interpolated vertex sample
          ts,                       // IN: the multi-linear weights
          vertices);                // IN: the data vertices to interpolate between
      }
    }

    //------------------------------
    // Compute the AABB of the resampled vertices
    aabbFromVertices(
      numDimensions,
      verticesA,
      aabbVerticesMinVals,
      aabbVerticesMaxVals);

    // Check if the AABB of the resampled vertices intersects the query AABB
    subCellsValid[subCellIndex] = true;
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      if (aabbVerticesMaxVals[dim] < aabbMinVals[dim] - gAABBTol ||
          aabbVerticesMinVals[dim] > aabbMaxVals[dim] + gAABBTol)
      {
        subCellsValid[subCellIndex] = false;
        break;
      }
    }

    if (subCellsValid[subCellIndex])
    {
      // Check if any of the resampled vertices lies inside the query AABB
      bool vertFound = aabbContainsAnyVertex(
        numDimensions,
        aabbMinVals,
        aabbMaxVals,
        verticesA);

      if (vertFound)
      {
        return true;
      }

      // Compute the squared distance of the resampled vertex AABB mid-point to the
      // current AABB focal point. Also compute the intersection of the resampled vertex
      // AABB and the query AABB.
      float aabbIntersectMinVals[SCATTERED_DATA_MAX_DIM];
      float aabbIntersectMaxVals[SCATTERED_DATA_MAX_DIM];
      float distSq = 0.0f;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        // Cost
        float aabbVerticesMidPoint = 0.5f * (aabbVerticesMinVals[dim] + aabbVerticesMaxVals[dim]);
        distSq += NMP::sqr(aabbVerticesMidPoint - aabbFocalPoint[dim]);

        // AABB intersection
        aabbIntersectMinVals[dim] = NMP::maximum(aabbMinVals[dim], aabbVerticesMinVals[dim]);
        aabbIntersectMaxVals[dim] = NMP::minimum(aabbMaxVals[dim], aabbVerticesMaxVals[dim]);
      }
      subCellCosts[subCellIndex] = distSq;

      // Compute the mid-point of the intersection AABB
      float* aabbIntersectFocalPoint = aabbIntersectFocalPoints[subCellIndex];      
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        aabbIntersectFocalPoint[dim] = 0.5f * (aabbIntersectMinVals[dim] + aabbIntersectMaxVals[dim]);
      }

      // Add another valid sub-cell entry
      subCellRankOrder[numValidSubCells] = subCellIndex;
      numValidSubCells++;
    }
  }

  // Check if no sub-cell AABBs intersect the query AABB
  if (numValidSubCells == 0)
  {
    return false;
  }

  //------------------------------
  // Rank the valid sub-cells in distance order from the current AABB focal point
  for (uint32_t i = 0; i < numValidSubCells - 1; ++i)
  {
    for (uint32_t j = i + 1; j < numValidSubCells; ++j)
    {
      uint32_t indexA = subCellRankOrder[i];
      uint32_t indexB = subCellRankOrder[j];
      NMP_ASSERT(indexA != indexB);
      if (subCellCosts[indexB] < subCellCosts[indexA])
      {
        NMP::nmSwap(subCellRankOrder[i], subCellRankOrder[j]);
      }
    }
  }

  //------------------------------
  // Recurse to the valid child sub-cells
  uint32_t nextDepth = depth + 1;
  for (uint32_t i = 0; i < numValidSubCells; ++i)
  {
    uint32_t subCellIndex = subCellRankOrder[i];

    if (aabbIntersectsNDCellAux(  
      numDimensions,
      aabbMinVals,
      aabbMaxVals,
      vertices,
      verticesA,
      nextDepth,
      aabbIntersectFocalPoints[subCellIndex],
      subWeightsMinVals[subCellIndex],
      subWeightsMaxVals[subCellIndex]))
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsNDCell(
  uint32_t numDimensions,         // IN: the dimensionality of the AABB
  const float* aabbMinVals,       // IN: the AABB min values
  const float* aabbMaxVals,       // IN: the AABB max values
  const float* const* vertices,   // IN: the vertices of the NDCell
  float* const* verticesA)        // TEMP: working memory for the resampled NDCell vertices
{
  NMP_ASSERT(numDimensions > 0);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(vertices);
  NMP_ASSERT(verticesA);

  //------------------------------
  // Compute the AABB of the NDCell vertices
  float aabbVerticesMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbVerticesMaxVals[SCATTERED_DATA_MAX_DIM];
  aabbFromVertices(
    numDimensions,
    vertices,
    aabbVerticesMinVals,
    aabbVerticesMaxVals);

  // Compute the intersection of the vertices AABB and the query AABB
  float aabbIntersectMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbIntersectMaxVals[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    aabbIntersectMinVals[dim] = NMP::maximum(aabbMinVals[dim], aabbVerticesMinVals[dim]);
    aabbIntersectMaxVals[dim] = NMP::minimum(aabbMaxVals[dim], aabbVerticesMaxVals[dim]);
    if (aabbIntersectMinVals[dim] - aabbIntersectMaxVals[dim] > gAABBTol)
    {
      return false;
    }
  }

  //------------------------------
  // Initialise the vertex interpolants
  float weightsMinVals[SCATTERED_DATA_MAX_DIM];
  float weightsMaxVals[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    weightsMinVals[dim] = 0.0f;
    weightsMaxVals[dim] = 1.0f;
  }

  // Compute the mid-point of the intersection AABB
  float aabbIntersectMidPoint[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    aabbIntersectMidPoint[dim] = 0.5f * (aabbIntersectMinVals[dim] + aabbIntersectMaxVals[dim]);
  }

  //------------------------------
  // Iteratively bisect the set of vertex interpolants, recursing to any sub-cells whose
  // AABBs intersect the query AABB
  bool result = aabbIntersectsNDCellAux(
    numDimensions,
    aabbMinVals,
    aabbMaxVals,
    vertices,
    verticesA,
    1,
    aabbIntersectMidPoint,
    weightsMinVals,
    weightsMaxVals);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
static bool aabbIntersectsProjectedNDCellFacetAux(
  uint32_t numDimensions,
  const float* aabbMinVals,
  const float* aabbMaxVals,
  const float* const* vertices,
  float* const* verticesA,
  bool* validA,
  const float* tmK,
  uint32_t depth,
  const float* aabbFocalPoint,
  const float* weightsMinVals,
  const float* weightsMaxVals)
{
  const uint32_t maxIterations = 10;  // NOTE: This is enough iterations for the vertex interpolants to
                                      // have converged to a single value within 1e-3
  const float alphas[3] = {0.0f, 0.5f, 1.0f};
  const float oma[3] = {1.0f, 0.5f, 0.0f};

  // Check for termination on the maximum number of iterations
  if (depth == maxIterations)
  {
    return false;
  }

  uint32_t M = numDimensions - 1;
  uint32_t numCellFacetVertices = 1 << M;
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(M);

  // State data
  float aabbVerticesMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbVerticesMaxVals[SCATTERED_DATA_MAX_DIM];
  float subWeightsMinVals[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_DIM];
  float subWeightsMaxVals[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_DIM];  
  float aabbIntersectFocalPoints[SCATTERED_DATA_MAX_CELLFACET_VERTICES][SCATTERED_DATA_MAX_DIM];
  float subCellCosts[SCATTERED_DATA_MAX_CELLFACET_VERTICES];
  bool subCellsValid[SCATTERED_DATA_MAX_CELLFACET_VERTICES];
  uint32_t subCellRankOrder[SCATTERED_DATA_MAX_CELLFACET_VERTICES];
  uint32_t numValidSubCells = 0;

  //------------------------------
  // Test all sub-cells for intersection with the AABB
  for (uint32_t subCellIndex = 0; subCellIndex < numCellFacetVertices; ++subCellIndex)
  {
    // Compute the bisected set of vertex interpolants for the sub-cell      
    float* curSubWeightsMinVals = subWeightsMinVals[subCellIndex];
    float* curSubWeightsMaxVals = subWeightsMaxVals[subCellIndex];
    float* subWeights[2];
    subWeights[0] = curSubWeightsMinVals;
    subWeights[1] = curSubWeightsMaxVals;

    for (uint32_t dim = 0; dim < M; ++dim)
    {
      uint32_t index = (subCellIndex >> dim) & 1;
      curSubWeightsMinVals[dim] = oma[index] * weightsMinVals[dim] + alphas[index] * weightsMaxVals[dim];
      curSubWeightsMaxVals[dim] = oma[index + 1] * weightsMinVals[dim] + alphas[index + 1] * weightsMaxVals[dim];
    }

    // Iteration logic
    uint32_t startIndexA, startIndexB, indexInc;
    uint32_t subCellCoordX = subCellIndex & 1;
    if (subCellCoordX > 0)
    {
      startIndexA = 0;
      startIndexB = 1;
      indexInc = 2;
    }
    else
    {
      startIndexA = numCellFacetVertices;
      startIndexB = 0;
      indexInc = 1;
    }

    // Copy all X+1 vertex entries from the previous update into the X+0 entries.
    // This cuts the number of interpolations required to compute the sub-cell vertices by a quarter.
    // MORPH-21357: Compute all vertices in the sub-cell mesh in one go.
    for (uint32_t vertexIndex = startIndexA; vertexIndex < numCellFacetVertices; vertexIndex += indexInc)
    {
      float* vSrc = verticesA[vertexIndex + 1];
      float* vDst = verticesA[vertexIndex];
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        vDst[dim] = vSrc[dim];
      }
      validA[vertexIndex] = validA[vertexIndex + 1];
    }

    // Evaluate the remaining X+1 vertex entries
    for (uint32_t vertexIndex = startIndexB; vertexIndex < numCellFacetVertices; vertexIndex += indexInc)
    {
      float ts[SCATTERED_DATA_MAX_DIM];
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        uint32_t vertexCoord = (vertexIndex >> dim) & 1;
        ts[dim] = subWeights[vertexCoord][dim];
      }

      float* vDst = verticesA[vertexIndex];
      managerDataBlock.m_multilinearInterpFn(
        M,                        // IN: number of dimensions
        numDimensions,            // IN: number of components in each vertex
        vDst,                     // OUT: the interpolated vertex sample
        ts,                       // IN: the multi-linear weights
        vertices);                // IN: the data vertices to interpolate between

      //------------------------------
      // Project the resampled vertex onto the AABB hyper-plane
      float w = tmK[1] * vDst[M] + tmK[2];

      // Use w component to decide if the vertex lies in the positive side of the ray direction
      // that project towards the AABB hyper-plane
      if (w > 0.0f)
      {
        float kFac = tmK[0] / w;
        for (uint32_t dim = 0; dim < M; ++dim)
        {
          vDst[dim] = kFac * vDst[dim];
        }
        validA[vertexIndex] = true;
      }
      else
      {
        validA[vertexIndex] = false;
      }
    }

    //------------------------------
    // Compute the AABB of the resampled vertices
    subCellsValid[subCellIndex] = aabbFromVertices(
      M,
      verticesA,
      validA,
      aabbVerticesMinVals,
      aabbVerticesMaxVals);

    if (subCellsValid[subCellIndex])
    {
      // Check if the AABB of the resampled vertices intersects the query AABB
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        if (aabbVerticesMaxVals[dim] < aabbMinVals[dim] - gAABBTol ||
            aabbVerticesMinVals[dim] > aabbMaxVals[dim] + gAABBTol)
        {
          subCellsValid[subCellIndex] = false;
          break;
        }
      }
    }

    if (subCellsValid[subCellIndex])
    {
      // Check if any of the resampled vertices lies inside the query AABB
      bool vertFound = aabbContainsAnyVertex(
        M,
        aabbMinVals,
        aabbMaxVals,
        verticesA,
        validA);

      if (vertFound)
      {
        return true;
      }

      // Compute the squared distance of the resampled vertex AABB mid-point to the
      // current AABB focal point. Also compute the intersection of the resampled vertex
      // AABB and the query AABB.
      float aabbIntersectMinVals[SCATTERED_DATA_MAX_DIM];
      float aabbIntersectMaxVals[SCATTERED_DATA_MAX_DIM];
      float distSq = 0.0f;
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        // Cost
        float aabbVerticesMidPoint = 0.5f * (aabbVerticesMinVals[dim] + aabbVerticesMaxVals[dim]);
        distSq += NMP::sqr(aabbVerticesMidPoint - aabbFocalPoint[dim]);

        // AABB intersection
        aabbIntersectMinVals[dim] = NMP::maximum(aabbMinVals[dim], aabbVerticesMinVals[dim]);
        aabbIntersectMaxVals[dim] = NMP::minimum(aabbMaxVals[dim], aabbVerticesMaxVals[dim]);
      }
      subCellCosts[subCellIndex] = distSq;

      // Compute the mid-point of the intersection AABB
      float* aabbIntersectFocalPoint = aabbIntersectFocalPoints[subCellIndex];      
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        aabbIntersectFocalPoint[dim] = 0.5f * (aabbIntersectMinVals[dim] + aabbIntersectMaxVals[dim]);
      }

      // Add another valid sub-cell entry
      subCellRankOrder[numValidSubCells] = subCellIndex;
      numValidSubCells++;
    }
  }

  // Check if no sub-cell AABBs intersect the query AABB
  if (numValidSubCells == 0)
  {
    return false;
  }

  //------------------------------
  // Rank the valid sub-cells in distance order from the current AABB focal point
  for (uint32_t i = 0; i < numValidSubCells - 1; ++i)
  {
    for (uint32_t j = i + 1; j < numValidSubCells; ++j)
    {
      uint32_t indexA = subCellRankOrder[i];
      uint32_t indexB = subCellRankOrder[j];
      NMP_ASSERT(indexA != indexB);
      if (subCellCosts[indexB] < subCellCosts[indexA])
      {
        NMP::nmSwap(subCellRankOrder[i], subCellRankOrder[j]);
      }
    }
  }

  //------------------------------
  // Recurse to the valid child sub-cells
  uint32_t nextDepth = depth + 1;
  for (uint32_t i = 0; i < numValidSubCells; ++i)
  {
    uint32_t subCellIndex = subCellRankOrder[i];

    if (aabbIntersectsProjectedNDCellFacetAux(  
      numDimensions,
      aabbMinVals,
      aabbMaxVals,
      vertices,
      verticesA,
      validA,
      tmK,
      nextDepth,
      aabbIntersectFocalPoints[subCellIndex],
      subWeightsMinVals[subCellIndex],
      subWeightsMaxVals[subCellIndex]))
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool aabbIntersectsProjectedNDCellFacet(
  uint32_t numDimensions,
  const float* aabbMinVals,
  const float* aabbMaxVals,
  const float* const* vertices,
  float* const* verticesA,
  bool* validA,
  const float* tmK)
{
  NMP_ASSERT(numDimensions > 1);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);
  NMP_ASSERT(vertices);
  NMP_ASSERT(verticesA);
  NMP_ASSERT(validA);
  NMP_ASSERT(tmK);
  NMP_ASSERT(tmK[0] > 0.0f);

  uint32_t M = numDimensions - 1;
  uint32_t numCellFacetVertices = 1 << M;

  //------------------------------
  // Project the input NDCell facet vertices onto the AABB hyper-plane
  for (uint32_t vertexIndex = 0; vertexIndex < numCellFacetVertices; ++vertexIndex)
  {
    const float* vSrc = vertices[vertexIndex];
    float* vDst = verticesA[vertexIndex];
    float w = tmK[1] * vSrc[M] + tmK[2];

    // Use w component to decide if the vertex lies in the positive side of the ray direction
    // that project towards the AABB hyper-plane
    if (w > 0.0f)
    {
      float kFac = tmK[0] / w;
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        vDst[dim] = kFac * vSrc[dim];
      }
      validA[vertexIndex] = true;
    }
    else
    {
      validA[vertexIndex] = false;
    }
  }

  //------------------------------
  // Compute the AABB of the projected vertices
  float aabbVerticesMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbVerticesMaxVals[SCATTERED_DATA_MAX_DIM];
  bool aabbValid = aabbFromVertices(
    M,
    verticesA,
    validA,
    aabbVerticesMinVals,
    aabbVerticesMaxVals);

  if (!aabbValid)
  {
    return false;
  }

  // Compute the intersection of the projected vertices AABB and the query AABB
  float aabbIntersectMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbIntersectMaxVals[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < M; ++dim)
  {
    aabbIntersectMinVals[dim] = NMP::maximum(aabbMinVals[dim], aabbVerticesMinVals[dim]);
    aabbIntersectMaxVals[dim] = NMP::minimum(aabbMaxVals[dim], aabbVerticesMaxVals[dim]);
    if (aabbIntersectMinVals[dim] - aabbIntersectMaxVals[dim] > gAABBTol)
    {
      return false;
    }
  }

  //------------------------------
  // Initialise the vertex interpolants
  float weightsMinVals[SCATTERED_DATA_MAX_DIM];
  float weightsMaxVals[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < M; ++dim)
  {
    weightsMinVals[dim] = 0.0f;
    weightsMaxVals[dim] = 1.0f;
  }

  // Compute the mid-point of the intersection AABB
  float aabbIntersectMidPoint[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < M; ++dim)
  {
    aabbIntersectMidPoint[dim] = 0.5f * (aabbIntersectMinVals[dim] + aabbIntersectMaxVals[dim]);
  }

  //------------------------------
  // Iteratively bisect the set of vertex interpolants, recursing to any sub-cells whose
  // AABBs intersect the query AABB
  bool result = aabbIntersectsProjectedNDCellFacetAux(  
    numDimensions,
    aabbMinVals,
    aabbMaxVals,
    vertices,
    verticesA,
    validA,
    tmK,
    1,
    aabbIntersectMidPoint,
    weightsMinVals,
    weightsMaxVals);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool insideNDCell(
  uint32_t      numDimensions,
  const float*  ts)
{
  NMP_ASSERT(numDimensions > 0);
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    if (ts[dim] < gMinCellTol || ts[dim] > gMaxCellTol)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Multi-linear interpolation
//----------------------------------------------------------------------------------------------------------------------
void computeMultilinearInterp1D(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices)
{
  NMP_ASSERT(numDimensions == 1);
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("computeMultilinearInterp1D");
#endif

  // Barycentric Weights
  float weights[2] = {1.0f - ts[0], ts[0]};

  // Interpolation
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    x[i] = weights[0] * vertices[0][i] + weights[1] * vertices[1][i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeMultilinearInterp2D(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices)
{
  NMP_ASSERT(numDimensions == 2);
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("computeMultilinearInterp2D");
#endif

  // Barycentric Weights
  float tbar[2];
  tbar[0] = 1.0f - ts[0];
  tbar[1] = 1.0f - ts[1];
  float weights[4] =
  {
    tbar[0] * tbar[1],
    ts[0]   * tbar[1],
    tbar[0] * ts[1],
    ts[0]   * ts[1]
  };

  // Interpolation
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    x[i] = weights[0] * vertices[0][i] +
           weights[1] * vertices[1][i] +
           weights[2] * vertices[2][i] +
           weights[3] * vertices[3][i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeMultilinearInterp3D(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices)
{
  NMP_ASSERT(numDimensions == 3);
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("computeMultilinearInterp3D");
#endif

  // Barycentric Weights
  float tbar[3];
  tbar[0] = 1.0f - ts[0];
  tbar[1] = 1.0f - ts[1];
  tbar[2] = 1.0f - ts[2];

  float weights2[4];
  weights2[0] = tbar[0] * tbar[1];
  weights2[1] = ts[0]   * tbar[1];
  weights2[2] = tbar[0] * ts[1];
  weights2[3] = ts[0]   * ts[1];

  float weights3[8] =
  {
    weights2[0] * tbar[2],
    weights2[1] * tbar[2],
    weights2[2] * tbar[2],
    weights2[3] * tbar[2],
    weights2[0] * ts[2],
    weights2[1] * ts[2],
    weights2[2] * ts[2],
    weights2[3] * ts[2]
  };

  // Interpolation
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    float u = weights3[0] * vertices[0][i] +
              weights3[1] * vertices[1][i] +
              weights3[2] * vertices[2][i] +
              weights3[3] * vertices[3][i];
    float v = weights3[4] * vertices[4][i] +
              weights3[5] * vertices[5][i] +
              weights3[6] * vertices[6][i] +
              weights3[7] * vertices[7][i];
    x[i] = u + v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeMultilinearInterpND(
  uint32_t              numDimensions,
  uint32_t              numSampleComponents,
  float*                x,
  const float*          ts,
  const float* const*   vertices)
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("computeMultilinearInterpND");
#endif

  // Interpolants
  float omts[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    omts[dim] = 1.0f - ts[dim];
  }
  const float* interpolants[2] = {omts, ts};

  // Compute the Barycentric weights
  float weights[SCATTERED_DATA_MAX_CELL_VERTICES];
  uint32_t numVertices = 1 << numDimensions;

  for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
  {
    float w = 1.0f;
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      // Select the interpolant depending on the vertex coordinate
      // (vertexCoord == 0) ? 1 - t : t
      uint32_t vertexCoord = (vertexIndex >> dim) & 1; 
      w *= interpolants[vertexCoord][dim];
    }
    weights[vertexIndex] = w;
  }

  // Interpolation
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    x[i] = 0.0f;
  }
  for (uint32_t v = 0; v < numVertices; ++v)
  {
    for (uint32_t i = 0; i < numSampleComponents; ++i)
    {
      x[i] += weights[v] * vertices[v][i];
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
// Multilinear Cell Coeffients
//----------------------------------------------------------------------------------------------------------------------
// Specialisation of the 1D multilinear cell weight calculation
NDCellSolverReturnVal multilinearCellCoeffs1D(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  const float*          x,
  float*                ts,
  const float* const*   vertices,
  NDCellSolverData*     solverData)
{
  NMP_ASSERT(numDimensions == 1);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);
  NMP_ASSERT(solverData);

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs1D");
#endif

  solverData->setSolverStatus(kAnalyticSolution);

  // Solve: (X1 - X0) * t = (X - X0)
  ts[0] = 0.0f;
  float X0 = vertices[0][0];
  float X1 = vertices[1][0];
  float b = x[0] - X0;
  if (NMP::nmfabs(b) < gEps)
  {
    return kNDCellSolveInterpolated;
  }
  float A = X1 - X0;
  if (NMP::nmfabs(A) < gEps)
  {
    return kNDCellSolveFailed;
  }
  ts[0] = b / A;
  if (ts[0] >= gMinCellTol && ts[0] <= gMaxCellTol)
  {
    return kNDCellSolveInterpolated;
  }

  return kNDCellSolveExtrapolated;
}

//----------------------------------------------------------------------------------------------------------------------
// Specialisation of the 2D multilinear cell weight calculation
NDCellSolverReturnVal multilinearCellCoeffs2D(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  const float*          x,
  float*                ts,
  const float* const*   vertices,
  NDCellSolverData*     solverData)
{
  NMP_ASSERT(numDimensions == 2);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);
  NMP_ASSERT(solverData);

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs2D");
#endif

  solverData->setSolverStatus(kAnalyticSolution);

  // Apply the translation that moves the query point X back to the origin.
  static const float vAlphas[4] = {0.0f, 1.0f, 0.0f, 1.0f};
  static const float vBetas[4] = {0.0f, 0.0f, 1.0f, 1.0f};
  float Vx[4];
  float Vy[4];
  for (uint32_t i = 0; i < 4; ++i)
  {
    // Apply the translation to the 2D cell vertices
    Vx[i] = vertices[i][0] - x[0];
    Vy[i] = vertices[i][1] - x[1];

    // Check if the query point is coincident with any of the vertices. This prevents
    // problems later if one or more cell vertices are themselves coincident.
    float vDist = Vx[i] * Vx[i] + Vy[i] * Vy[i];
    if (vDist < gEps)
    {
      ts[0] = vAlphas[i];
      ts[1] = vBetas[i];
      return kNDCellSolveInterpolated;
    }
  }

  // ^ C------D
  // | |      |
  // | |   x  |
  // | A------B 
  // O---------->
  // We need to find the alpha weight that interpolates points E and F on edges
  // AB and CD respectively. The point X must also lay on the line L that
  // passes through E and F. Using homogeneous coordinates: X^T * L = 0,
  // where L = cross(E, F). Since we have mapped X back to the origin then
  // X = [0, 0, 1]^T and L = [u, v, w]^T. Solving L_w = 0 in terms of the
  // interpolant gives the required result.

  // Set up the quadratic polynomial coefficients. Note that if the sides
  // AB and CD are collinear then the quadratic is degenerate.
  float AxBx = Vx[1] - Vx[0];
  float AyBy = Vy[1] - Vy[0];
  float CxDx = Vx[3] - Vx[2];
  float CyDy = Vy[3] - Vy[2];
  float a = AxBx * CyDy - AyBy * CxDx;  // Note that this is the Z component of the cross product
                                        // between vectors AB and CD. This will be zero if the two
                                        // vectors are parallel.
  float b = AxBx * Vy[2] + CyDy * Vx[0] - AyBy * Vx[2] - CxDx * Vy[0];
  float c = Vx[0] * Vy[2] - Vy[0] * Vx[2];
  if (NMP::nmfabs(a) > gEps)
  {
    //------------------------------
    // STABLE QUADRATIC SOLVE FOR REAL ROOTS.
    float signb = NMP::floatSelect(b, 1.0f, -1.0f);
    float desc = b * b - 4 * a * c;

    // Avoid complex roots due to numerical error. Complex roots occur in practice if the query
    // point lies within the unreachable part of the space outside of the 2D cell.
    if (desc < -gEps)
    {
      return kNDCellSolveFailed;
    }
    desc = NMP::floatSelect(desc, desc, 0.0f);

    // Note it is possible to have both roots inside the interpolation region.
    // Typically this happens whenever two of the vertices are coincident.
    float q = -0.5f * (b + signb * sqrtf(desc));
    if (NMP::nmfabs(q) > gEps)
    {
      float alpha, oma;
      float alphas[2], betas[2];
      float Ex, Ey, Fx, Fy, Wx, Wy, M, r;
      uint32_t numEntries = 0;

      //-----------------------------
      // First root
      alpha = q / a;

      // The point X must lay on the line between points E and F, such that
      // (F - E) * beta = X - E. We must first compute points E and F.      
      oma = 1.0f - alpha;
      Ex = oma * Vx[0] + alpha * Vx[1];
      Ey = oma * Vy[0] + alpha * Vy[1];
      Fx = oma * Vx[2] + alpha * Vx[3];
      Fy = oma * Vy[2] + alpha * Vy[3];

      // The system of equations required to solve beta is over-determined. We
      // can solve beta by forming the normal equations: (A^T * A) * x = A^T * b
      Wx = Fx - Ex;
      Wy = Fy - Ey;
      M = Wx * Wx + Wy * Wy; // Zero if interpolated points E, F are coincident. Note: The query
                             // point has already been tested for coincidence with the vertices.
      if (M > gEps)
      {
        r = -Wx * Ex - Wy * Ey;
        alphas[numEntries] = alpha;
        betas[numEntries] = r / M;
        numEntries++;
      }

      //-----------------------------
      // Second root
      alpha = c / q;

      // The point X must lay on the line between points E and F, such that
      // (F - E) * beta = X - E. We must first compute points E and F.
      oma = 1.0f - alpha;
      Ex = oma * Vx[0] + alpha * Vx[1];
      Ey = oma * Vy[0] + alpha * Vy[1];
      Fx = oma * Vx[2] + alpha * Vx[3];
      Fy = oma * Vy[2] + alpha * Vy[3];

      // The system of equations required to solve beta is over-determined. We
      // can solve beta by forming the normal equations: (A^T * A) * x = A^T * b
      Wx = Fx - Ex;
      Wy = Fy - Ey;
      M = Wx * Wx + Wy * Wy; // Zero if interpolated points E, F are coincident. Note: The query
                             // point has already been tested for coincidence with the vertices.
      if (M > gEps)
      {
        r = -Wx * Ex - Wy * Ey;
        alphas[numEntries] = alpha;
        betas[numEntries] = r / M;
        numEntries++;
      }

      //-----------------------------
      // Determine the best solution (closest to the centre of the 2D cell)
      if (numEntries == 0)
      {
        return kNDCellSolveFailed;
      }

      // Note that the interpolated point should be the same as the input for
      // both roots, so the best root should be the one closest to the mid-point
      // of the cell. The exception to this is if the query point lies in the
      // unreachable space and the roots are complex. Ignoring the imaginary part
      // gives you a repeated root that projects onto the closest point on the 
      // boundary of the reachable space. We don't test if the solution was complex
      // and the interpolated point represents the query point.
      ts[0] = alphas[0];
      ts[1] = betas[0];
      if (numEntries > 1)
      {
        float dist1 = NMP::sqr(alphas[0] - 0.5f) + NMP::sqr(betas[0] - 0.5f);
        float dist2 = NMP::sqr(alphas[1] - 0.5f) + NMP::sqr(betas[1] - 0.5f);
        if (dist2 < dist1)
        {
          ts[0] = alphas[1];
          ts[1] = betas[1];
        }
      }

      // Determine the status of the interpolation query
      if (ts[0] >= gMinCellTol && ts[0] <= gMaxCellTol &&
          ts[1] >= gMinCellTol && ts[1] <= gMaxCellTol)
      {
        return kNDCellSolveInterpolated;
      }

      return kNDCellSolveExtrapolated;
    }
    else
    {
      //-----------------------------
      // ONE VALID ROOT
      ts[0] = 0.0f; // q / a

      // The system of equations required to solve beta is over-determined. We
      // can solve beta by forming the normal equations: (A^T * A) * x = A^T * b
      float Wx = Vx[2] - Vx[0];
      float Wy = Vy[2] - Vy[0];
      float M = Wx * Wx + Wy * Wy;
      float r = -Wx * Vx[0] - Wy * Vy[0];
      ts[1] = M < gEps ? 0.0f : r / M;

      // Determine the status of the interpolation query
      if (ts[1] >= gMinCellTol && ts[1] <= gMaxCellTol)
      {
        return kNDCellSolveInterpolated;
      }
      return kNDCellSolveExtrapolated;
    }
  }
  else
  {
    //------------------------------
    // SOLVE THE LINEAR POLYNOMIAL

    // Vectors AB and CD are parallel. Typically the 2D cell is rectangular.
    if (NMP::nmfabs(b) > gEps)
    {
      ts[0] = -c / b;

      // The point X must lay on the line between points E and F, such that
      // (F - E) * beta = X - E. We must first compute points E and F.
      float oma = 1.0f - ts[0];
      float Ex = oma * Vx[0] + ts[0] * Vx[1];
      float Ey = oma * Vy[0] + ts[0] * Vy[1];
      float Fx = oma * Vx[2] + ts[0] * Vx[3];
      float Fy = oma * Vy[2] + ts[0] * Vy[3];

      // The system of equations required to solve beta is over-determined. We
      // can solve beta by forming the normal equations: (A^T * A) * x = A^T * b
      float Wx = Fx - Ex;
      float Wy = Fy - Ey;
      float M = Wx * Wx + Wy * Wy;
      float r = -Wx * Ex - Wy * Ey;
      ts[1] = M < gEps ? 0.0f : r / M;

      // Determine the status of the interpolation query
      if (ts[0] >= gMinCellTol && ts[0] <= gMaxCellTol &&
          ts[1] >= gMinCellTol && ts[1] <= gMaxCellTol)
      {
        return kNDCellSolveInterpolated;
      }
      return kNDCellSolveExtrapolated;
    }
    else
    {
      //------------------------------
      // The 2D cell is actually squashed into a line. However, we don't know which vertices
      // are degenerate. We should cycle through the line segments AB, CD, AC, BD in turn
      // to determine if X lies on any of these segments.
      // Solve: X = alpha * U + beta * V, where U is the principal direction for the line side
      // and V is the orthogonal complement.
      // The query point may lay within the bounds of more than one line segment. The best
      // solution should be the one closest to the mid point of each line segment. It is then
      // easy to determine if the point was interpolated or extrapolated.
      float det, alpha, beta, err;
      float bb[2];
      float dist = FLT_MAX;
      bool status = false;

      // Side AB
      det = NMP::sqr(AxBx) + NMP::sqr(AyBy); // Zero if vertices A and B are coincident.
                                             // Note: The query point has already been tested for
                                             // coincidence with the vertices.
      if (det > gEps)
      {
        bb[0] = -Vx[0] / det;
        bb[1] = -Vy[0] / det;
        alpha = AxBx * bb[0] + AyBy * bb[1];
        beta = AxBx * bb[1] - AyBy * bb[0];

        // Check the perpendicular distance to check if the query point lies on the line
        if (NMP::nmfabs(beta) < gEps)
        {
          // Update the best solution closest to the line mid-point
          err = NMP::nmfabs(0.5f - alpha);
          if (err < dist)
          {
            ts[0] = alpha;
            ts[1] = 0.0f;
            dist = err;
            status = true;
          }
        }
      }

      // Side CD
      det = NMP::sqr(CxDx) + NMP::sqr(CyDy); // Zero if vertices C and D are coincident.
                                             // Note: The query point has already been tested for
                                             // coincidence with the vertices.
      if (det > gEps)
      {
        bb[0] = -Vx[2] / det;
        bb[1] = -Vy[2] / det;
        alpha = CxDx * bb[0] + CyDy * bb[1];
        beta = CxDx * bb[1] - CyDy * bb[0];

        // Check the perpendicular distance to check if the query point lies on the line
        if (NMP::nmfabs(beta) < gEps)
        {
          // Update the best solution closest to the line mid-point
          err = NMP::nmfabs(0.5f - alpha);
          if (err < dist)
          {
            ts[0] = alpha;
            ts[1] = 1.0f;
            dist = err;
            status = true;
          }
        }
      }

      // Side AC
      float AxCx = Vx[2] - Vx[0];
      float AyCy = Vy[2] - Vy[0];
      det = NMP::sqr(AxCx) + NMP::sqr(AyCy); // Zero if vertices A and C are coincident.
                                             // Note: The query point has already been tested for
                                             // coincidence with the vertices.
      if (det > gEps)
      {
        bb[0] = -Vx[0] / det;
        bb[1] = -Vy[0] / det;
        alpha = AxCx * bb[0] + AyCy * bb[1];
        beta = AxCx * bb[1] - AyCy * bb[0];

        // Check the perpendicular distance to check if the query point lies on the line
        if (NMP::nmfabs(beta) < gEps)
        {
          // Update the best solution closest to the line mid-point
          err = NMP::nmfabs(0.5f - alpha);
          if (err < dist)
          {
            ts[0] = 0.0f;
            ts[1] = alpha;
            dist = err;
            status = true;
          }
        }
      }

      // Side BD
      float BxDx = Vx[3] - Vx[1];
      float ByDy = Vy[3] - Vy[1];
      det = NMP::sqr(BxDx) + NMP::sqr(ByDy); // Zero if vertices B and D are coincident.
                                             // Note: The query point has already been tested for
                                             // coincidence with the vertices.
      if (det > gEps)
      {
        bb[0] = -Vx[1] / det;
        bb[1] = -Vy[1] / det;
        alpha = BxDx * bb[0] + ByDy * bb[1];
        beta = BxDx * bb[1] - ByDy * bb[0];

        // Check the perpendicular distance to check if the query point lies on the line
        if (NMP::nmfabs(beta) < gEps)
        {
          // Update the best solution closest to the line mid-point
          err = NMP::nmfabs(0.5f - alpha);
          if (err < dist)
          {
            ts[0] = 1.0f;
            ts[1] = alpha;
            dist = err;
            status = true;
          }
        }
      }

      // Determine the status of the interpolation query
      if (!status)
      {
        return kNDCellSolveFailed;
      }
      if (dist < gMidCellTol)
      {
        return kNDCellSolveInterpolated;
      }
      else
      {
        return kNDCellSolveExtrapolated;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function computes a new search direction by forming the Hessian and
/// solving the linear system (J^T * J) * dt = Jtr
//----------------------------------------------------------------------------------------------------------------------
bool multilinearCellCoeffs1DSearchDir(
  uint32_t NMP_USED_FOR_ASSERTS(numDimensions),
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions == 1);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs1DSearchDir");
#endif

  // Compute the upper triangle of the Hessian: H = J^T * J
  float A11 = NMP::sqr(J[0][0]);

  // Solve the linear system
  if (NMP::nmfabs(A11) < gEps)
  {
    return false;
  }

  dt[0] = Jtr[0] / A11;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function computes a new search direction by forming the Hessian and
/// solving the linear system (J^T * J) * dt = Jtr
//----------------------------------------------------------------------------------------------------------------------
bool multilinearCellCoeffs2DSearchDir(
  uint32_t NMP_USED_FOR_ASSERTS(numDimensions),
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions == 2);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs2DSearchDir");
#endif

  //------------------------------
  // Compute the upper triangle of the Hessian: H = J^T * J
  float A11 = NMP::sqr(J[0][0]) + NMP::sqr(J[1][0]);
  float A12 = J[0][0] * J[0][1] + J[1][0] * J[1][1];
  float A22 = NMP::sqr(J[0][1]) + NMP::sqr(J[1][1]);

  // Apply the smoothing factor to the diagonal entries of the Hessian. For
  // scales > 1 this biases the solution towards gradient descent. When the
  // parameters are far from the estimates then it is better to apply smoothing
  // to cut down on the update step size and move in the direction of the local
  // gradient rather than overshoot.
  A11 *= gLambda;
  A22 *= gLambda;

  //------------------------------
  // Solve the linear system via the standard 2x2 matrix inverse
  float det = A11 * A22 - A12 * A12;
  if (NMP::nmfabs(det) < gEps)
  {
    return false;
  }

  dt[0] = (A22 * Jtr[0] - A12 * Jtr[1]) / det;
  dt[1] = (A11 * Jtr[1] - A12 * Jtr[0]) / det;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function computes a new search direction by forming the Hessian and
/// solving the linear system (J^T * J) * dt = Jtr
//----------------------------------------------------------------------------------------------------------------------
bool multilinearCellCoeffs3DSearchDir(
  uint32_t NMP_USED_FOR_ASSERTS(numDimensions),
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions == 3);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs3DSearchDir");
#endif

  //------------------------------
  // Compute the upper triangle of the Hessian: H = J^T * J
  float A11 = NMP::sqr(J[0][0]) + NMP::sqr(J[1][0]) + NMP::sqr(J[2][0]);
  float A12 = J[0][0] * J[0][1] + J[1][0] * J[1][1] + J[2][0] * J[2][1];
  float A13 = J[0][0] * J[0][2] + J[1][0] * J[1][2] + J[2][0] * J[2][2];
  float A22 = NMP::sqr(J[0][1]) + NMP::sqr(J[1][1]) + NMP::sqr(J[2][1]);
  float A23 = J[0][1] * J[0][2] + J[1][1] * J[1][2] + J[2][1] * J[2][2];
  float A33 = NMP::sqr(J[0][2]) + NMP::sqr(J[1][2]) + NMP::sqr(J[2][2]);

  // Apply the smoothing factor to the diagonal entries of the Hessian. For
  // scales > 1 this biases the solution towards gradient descent. When the
  // parameters are far from the estimates then it is better to apply smoothing
  // to cut down on the update step size and move in the direction of the local
  // gradient rather than overshoot.
  A11 *= gLambda;
  A22 *= gLambda;
  A33 *= gLambda;

  //------------------------------
  // Gauss transform G1 to clear out the first column below the diagonal of A
  if (A11 < gEps) // Diagonal entries are initially positive
  {
    return false;
  }
  float iA11 = 1.0f / A11;

  // Apply G1 to A and b
  float b1 = Jtr[0];
  float tau1 = iA11 * A12; // A21 / A11
  A22 -= tau1 * A12;
  float tau2 = iA11 * A13; // A31 / A11
  float A32 = A23 - tau2 * A12; // Note: A32 = A23
  A23 -= tau1 * A13;
  float b2 = Jtr[1] - tau1 * b1;
  A33 -= tau2 * A13;
  float b3 = Jtr[2] - tau2 * b1;

  //------------------------------
  // Gauss transform G2 to clear out the second column below the diagonal of A
  if (NMP::nmfabs(A22) < gEps)
  {
    return false;
  }
  float iA22 = 1.0f / A22;

  // Apply G2 to A and b
  float tau3 = iA22 * A32;
  A33 -= tau3 * A23;
  b3 -= tau3 * b2;

  //------------------------------
  // Back substitution
  if (NMP::nmfabs(A33) < gEps)
  {
    return false;
  }
  dt[2] = b3 / A33;
  dt[1] = iA22 * (b2 - (A23 * dt[2]));
  dt[0] = iA11 * (b1 - (A12 * dt[1]) - (A13 * dt[2]));

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function computes a new search direction by forming the Hessian and
/// solving the linear system (J^T * J) * dt = Jtr, via Cholesky decomposition.
//----------------------------------------------------------------------------------------------------------------------
bool multilinearCellCoeffsNDSearchDir(
  uint32_t numDimensions,
  float dt[SCATTERED_DATA_VECTOR_SIZE],
  const float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  const float Jtr[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffsNDSearchDir");
#endif

  // Lower triangle of the decomposition and the reciprocal of the diagonals
  float H[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE];
  float iD[SCATTERED_DATA_VECTOR_SIZE];

  //------------------------------
  // Compute the upper triangle of the Hessian: H = J^T * J
  for (uint32_t row = 0; row < numDimensions; ++row)
  {
    for (uint32_t col = row; col < numDimensions; ++col)
    {
      // Update upper triangle of the Hessian
      float value = 0.0f;
      for (uint32_t i = 0; i < numDimensions; ++i)
      {
        value += J[i][row] * J[i][col];
      }
      H[row][col] = value;
    }
  }

  // Apply the smoothing factor to the diagonal entries of the Hessian. For
  // scales > 1 this biases the solution towards gradient descent. When the
  // parameters are far from the estimates then it is better to apply smoothing
  // to cut down on the update step size and move in the direction of the local
  // gradient rather than overshoot.
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    H[dim][dim] *= gLambda;
  }

  //------------------------------
  // Cholesky decomposition: H = L * L^T
  // The decomposition is performed inplace in the lower triangle of H.
  // The upper triangle containing the Hessian remains unchanged.
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    // Compute the diagonal element
    float dotProduct = 0.0f;
    for (uint32_t k = 0; k < i; ++k)
    {
      dotProduct += NMP::sqr(H[i][k]);
    }
    float Lsqr = H[i][i] - dotProduct;
    if (Lsqr < gEps)
    {
      return false;
    }
    iD[i] = 1.0f / sqrtf(Lsqr);

    // Compute the other values on this row/column.
    for (uint32_t j = (i + 1); j < numDimensions; ++j)
    {
      dotProduct = 0.0f;
      for (uint32_t k = 0; k < i; ++k)
      {
        dotProduct += H[i][k] * H[j][k];
      }
      H[j][i] = (H[i][j] - dotProduct) * iD[i];
    }
  }

  //------------------------------
  // Elimination 1: L * y = b
  float y[SCATTERED_DATA_VECTOR_SIZE];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    float dotProduct = 0.0f;
    for (uint32_t j = 0; j < i; ++j)
    {
      dotProduct += H[i][j] * y[j];
    }
    NMP_ASSERT(iD[i] > 0.0f);
    y[i] = (Jtr[i] - dotProduct) * iD[i];
  }

  // Elimination 2: L^T * x = y
  for (int32_t i = (int32_t)(numDimensions - 1); i >= 0; --i)
  {
    float dotProduct = 0.0f;
    for (uint32_t j = i + 1; j < numDimensions; ++j)
    {
      dotProduct += H[j][i] * dt[j];
    }
    dt[i] = (y[i] - dotProduct) * iD[i];
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
float multilinearCellCoeffs1DJac(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions == 1);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs1DJac");
#endif

  float omtn = 1.0f - tn[0];

  // Vertices
  float Xs[2];
  Xs[0] = vertices[0][0];
  Xs[1] = vertices[1][0];

  // Jacobian
  J[0][0] = Xs[1] - Xs[0];

  // Interpolate the point estimate xEst
  float xEst = omtn * Xs[0] + tn[0] * Xs[1];

  // Compute the residuals
  r[0] = x[0] - xEst;
  float cost = NMP::sqr(r[0]);

  return cost;
}

//----------------------------------------------------------------------------------------------------------------------
float multilinearCellCoeffs2DJac(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions == 2);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs2DJac");
#endif

  float omtn[2];
  omtn[0] = 1.0f - tn[0];
  omtn[1] = 1.0f - tn[1];

  // Vertices
  float Xs[4];
  Xs[0] = vertices[0][0];
  Xs[1] = vertices[1][0];
  Xs[2] = vertices[2][0];
  Xs[3] = vertices[3][0];
  float Ys[4];
  Ys[0] = vertices[0][1];
  Ys[1] = vertices[1][1];
  Ys[2] = vertices[2][1];
  Ys[3] = vertices[3][1];

  // Jacobian
  float v0[4];
  v0[0] = -omtn[1];
  v0[1] = omtn[1];
  v0[2] = -tn[1];
  v0[3] = tn[1];
  J[0][0] = v0[0] * Xs[0] + v0[1] * Xs[1] + v0[2] * Xs[2] + v0[3] * Xs[3];
  J[1][0] = v0[0] * Ys[0] + v0[1] * Ys[1] + v0[2] * Ys[2] + v0[3] * Ys[3];
  float v1[4];
  v1[0] = -omtn[0];
  v1[1] = -tn[0];
  v1[2] = omtn[0];
  v1[3] = tn[0];
  J[0][1] = v1[0] * Xs[0] + v1[1] * Xs[1] + v1[2] * Xs[2] + v1[3] * Xs[3];
  J[1][1] = v1[0] * Ys[0] + v1[1] * Ys[1] + v1[2] * Ys[2] + v1[3] * Ys[3];

  // Compute the Barycentric weights
  float weights[4];
  weights[0] = omtn[0] * omtn[1];
  weights[1] = tn[0] * omtn[1];
  weights[2] = omtn[0] * tn[1];
  weights[3] = tn[0] * tn[1];

  // Interpolate the point estimate xEst
  float xEst[2];
  xEst[0] = weights[0] * Xs[0] + weights[1] * Xs[1] + weights[2] * Xs[2] + weights[3] * Xs[3];
  xEst[1] = weights[0] * Ys[0] + weights[1] * Ys[1] + weights[2] * Ys[2] + weights[3] * Ys[3];

  // Compute the residuals
  r[0] = x[0] - xEst[0];
  r[1] = x[1] - xEst[1];
  float cost = NMP::sqr(r[0]) + NMP::sqr(r[1]);

  return cost;
}

//----------------------------------------------------------------------------------------------------------------------
float multilinearCellCoeffs3DJac(
  uint32_t              NMP_USED_FOR_ASSERTS(numDimensions),
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions == 3);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffs3DJac");
#endif

  float omtn[3];
  omtn[0] = 1.0f - tn[0];
  omtn[1] = 1.0f - tn[1];
  omtn[2] = 1.0f - tn[2];

  // Vertices
  float Xs[8];
  Xs[0] = vertices[0][0];
  Xs[1] = vertices[1][0];
  Xs[2] = vertices[2][0];
  Xs[3] = vertices[3][0];
  Xs[4] = vertices[4][0];
  Xs[5] = vertices[5][0];
  Xs[6] = vertices[6][0];
  Xs[7] = vertices[7][0];
  float Ys[8];
  Ys[0] = vertices[0][1];
  Ys[1] = vertices[1][1];
  Ys[2] = vertices[2][1];
  Ys[3] = vertices[3][1];
  Ys[4] = vertices[4][1];
  Ys[5] = vertices[5][1];
  Ys[6] = vertices[6][1];
  Ys[7] = vertices[7][1];
  float Zs[8];
  Zs[0] = vertices[0][2];
  Zs[1] = vertices[1][2];
  Zs[2] = vertices[2][2];
  Zs[3] = vertices[3][2];
  Zs[4] = vertices[4][2];
  Zs[5] = vertices[5][2];
  Zs[6] = vertices[6][2];
  Zs[7] = vertices[7][2];

  // Jacobian
  float v0[8];  
  v0[1] = omtn[1] * omtn[2];
  v0[0] = -v0[1];  
  v0[3] = tn[1] * omtn[2];
  v0[2] = -v0[3];  
  v0[5] = omtn[1] * tn[2];
  v0[4] = -v0[5];
  v0[7] = tn[1] * tn[2];
  v0[6] = -v0[7];
  J[0][0] = (v0[0] * Xs[0]) + (v0[1] * Xs[1]) + (v0[2] * Xs[2]) + (v0[3] * Xs[3]) +
            (v0[4] * Xs[4]) + (v0[5] * Xs[5]) + (v0[6] * Xs[6]) + (v0[7] * Xs[7]);
  J[1][0] = (v0[0] * Ys[0]) + (v0[1] * Ys[1]) + (v0[2] * Ys[2]) + (v0[3] * Ys[3]) +
            (v0[4] * Ys[4]) + (v0[5] * Ys[5]) + (v0[6] * Ys[6]) + (v0[7] * Ys[7]);
  J[2][0] = (v0[0] * Zs[0]) + (v0[1] * Zs[1]) + (v0[2] * Zs[2]) + (v0[3] * Zs[3]) +
            (v0[4] * Zs[4]) + (v0[5] * Zs[5]) + (v0[6] * Zs[6]) + (v0[7] * Zs[7]);

  float v1[8];
  v1[2] = omtn[0] * omtn[2];
  v1[0] = -v1[2];
  v1[3] = tn[0] * omtn[2];
  v1[1] = -v1[3];
  v1[6] = omtn[0] * tn[2];
  v1[4] = -v1[6];
  v1[7] = tn[0] * tn[2];
  v1[5] = -v1[7];
  J[0][1] = (v1[0] * Xs[0]) + (v1[1] * Xs[1]) + (v1[2] * Xs[2]) + (v1[3] * Xs[3]) +
            (v1[4] * Xs[4]) + (v1[5] * Xs[5]) + (v1[6] * Xs[6]) + (v1[7] * Xs[7]);
  J[1][1] = (v1[0] * Ys[0]) + (v1[1] * Ys[1]) + (v1[2] * Ys[2]) + (v1[3] * Ys[3]) +
            (v1[4] * Ys[4]) + (v1[5] * Ys[5]) + (v1[6] * Ys[6]) + (v1[7] * Ys[7]);
  J[2][1] = (v1[0] * Zs[0]) + (v1[1] * Zs[1]) + (v1[2] * Zs[2]) + (v1[3] * Zs[3]) +
            (v1[4] * Zs[4]) + (v1[5] * Zs[5]) + (v1[6] * Zs[6]) + (v1[7] * Zs[7]);

  float v2[8];
  v2[4] = omtn[0] * omtn[1];
  v2[0] = -v2[4];
  v2[5] = tn[0] * omtn[1];
  v2[1] = -v2[5];
  v2[6] = omtn[0] * tn[1];
  v2[2] = -v2[6];
  v2[7] = tn[0] * tn[1];
  v2[3] = -v2[7];
  J[0][2] = (v2[0] * Xs[0]) + (v2[1] * Xs[1]) + (v2[2] * Xs[2]) + (v2[3] * Xs[3]) +
            (v2[4] * Xs[4]) + (v2[5] * Xs[5]) + (v2[6] * Xs[6]) + (v2[7] * Xs[7]);
  J[1][2] = (v2[0] * Ys[0]) + (v2[1] * Ys[1]) + (v2[2] * Ys[2]) + (v2[3] * Ys[3]) +
            (v2[4] * Ys[4]) + (v2[5] * Ys[5]) + (v2[6] * Ys[6]) + (v2[7] * Ys[7]);
  J[2][2] = (v2[0] * Zs[0]) + (v2[1] * Zs[1]) + (v2[2] * Zs[2]) + (v2[3] * Zs[3]) +
            (v2[4] * Zs[4]) + (v2[5] * Zs[5]) + (v2[6] * Zs[6]) + (v2[7] * Zs[7]);
  
  // Compute the Barycentric weights
  float weights[8];
  weights[0] = v2[4] * omtn[2];
  weights[1] = v2[5] * omtn[2];
  weights[2] = v2[6] * omtn[2];
  weights[3] = v2[7] * omtn[2];
  weights[4] = v2[4] * tn[2];
  weights[5] = v2[5] * tn[2];
  weights[6] = v2[6] * tn[2];
  weights[7] = v2[7] * tn[2];

  // Interpolate the point estimate xEst
  float xEst[3];
  xEst[0] = (weights[0] * Xs[0]) + (weights[1] * Xs[1]) + (weights[2] * Xs[2]) + (weights[3] * Xs[3]) +
            (weights[4] * Xs[4]) + (weights[5] * Xs[5]) + (weights[6] * Xs[6]) + (weights[7] * Xs[7]);
  xEst[1] = (weights[0] * Ys[0]) + (weights[1] * Ys[1]) + (weights[2] * Ys[2]) + (weights[3] * Ys[3]) +
            (weights[4] * Ys[4]) + (weights[5] * Ys[5]) + (weights[6] * Ys[6]) + (weights[7] * Ys[7]);
  xEst[2] = (weights[0] * Zs[0]) + (weights[1] * Zs[1]) + (weights[2] * Zs[2]) + (weights[3] * Zs[3]) +
            (weights[4] * Zs[4]) + (weights[5] * Zs[5]) + (weights[6] * Zs[6]) + (weights[7] * Zs[7]);

  // Compute the residuals
  r[0] = x[0] - xEst[0];
  r[1] = x[1] - xEst[1];
  r[2] = x[2] - xEst[2];
  float cost = NMP::sqr(r[0]) + NMP::sqr(r[1]) + NMP::sqr(r[2]);

  return cost;
}

//----------------------------------------------------------------------------------------------------------------------
float multilinearCellCoeffsNDJac(
  uint32_t              numDimensions,
  const float*          x,
  const float*          tn,
  const float* const*   vertices,
  float                 J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE],
  float                 r[SCATTERED_DATA_VECTOR_SIZE])
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffsNDJac");
#endif

  static const float sn[2] = {-1.0f, 1.0f};

  float omtn[SCATTERED_DATA_VECTOR_SIZE];
  const float* interpolants[2] = {omtn, tn};

  // Derivatives of f(V : t_i) with respect to t_i. Used as workspace memory while calculating the Jacobian.
  float dfdt[SCATTERED_DATA_VECTOR_SIZE];

  // The estimate of the interpolated point using the current interpolation weights: xEst = f(V : t_i)
  float xEst[SCATTERED_DATA_VECTOR_SIZE];

  // Barycentric interpolation weights
  float weights[SCATTERED_DATA_MAX_CELL_VERTICES];

  const uint32_t numSampleComponents = numDimensions;
  const uint32_t numVertices = 1 << numDimensions;

  // Compute the weights
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    omtn[dim] = 1.0f - tn[dim];
  }

  // Initialise the Barycentric weights
  for (uint32_t v = 0; v < numVertices; ++v)
  {
    weights[v] = 1.0f;
  }

  // Jacobian
  for (uint32_t dim = 0; dim < numDimensions; ++dim) // Over the column vectors of the Jacobian
  {
    // Zero the column vector of partial derivatives for df / dt_i
    for (uint32_t i = 0; i < numSampleComponents; ++i)
    {
      dfdt[i] = 0.0f;
    }

    // Iterate through each vertex to accumulate the elements of dfdt.
    for (uint32_t v = 0; v < numVertices; ++v)
    {
      // The main dimension dim contributes -1 or 1 depending on whether its coordinate is 0 or 1.
      // The other dimensions contribute (1 - t_i) for coord 0, and t_i for coord 1.
      float val = 1.0f;
      for (uint32_t subdim = 0; subdim < dim; ++subdim)
      {
        uint32_t coord = (v >> subdim) & 1;
        val *= interpolants[coord][subdim];
      }

      uint32_t coordDim = (v >> dim) & 1;
      val *= sn[coordDim];

      // Update the Barycentric interpolation weights here for the main dim
      weights[v] *= interpolants[coordDim][dim];

      for (uint32_t subdim = dim + 1; subdim < numDimensions; ++subdim)
      {
        uint32_t coord = (v >> subdim) & 1;
        val *= interpolants[coord][subdim];
      }

      // This value is multiplied to each component of the corresponding vertex sample
      const float* vertex = vertices[v];
      for (uint32_t i = 0; i < numSampleComponents; ++i)
      {
        dfdt[i] += vertex[i] * val;
      }
    }

    // Copy the column vector of partial derivatives df / dt_i into the Jacobian.
    for (uint32_t row = 0; row < numSampleComponents; ++row)
    {
      J[row][dim] = dfdt[row];
    }
  }

  // Interpolate the point estimate xEst
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    xEst[i] = 0.0f;
  }
  for (uint32_t v = 0; v < numVertices; ++v)
  {
    for (uint32_t i = 0; i < numSampleComponents; ++i)
    {
      xEst[i] += weights[v] * vertices[v][i];
    }
  }

  // Compute the residuals
  float cost = 0.0f;
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    r[i] = x[i] - xEst[i];
    cost += NMP::sqr(r[i]);
  }

  return cost;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float cubicFuncMin(float f1, float f2, float c1, float c2, float dx)
{
  float xmin;

  // Find the coefficients of: f(x) = a*x^3 + b*x^2 + c*x + d
  // Trivially c = c1 and d = f1 at x=0. a and b are recovered by solving
  // the simultaneous equations at x = dx
  float dx2 = dx * dx;
  float dx3 = dx * dx2;
  float a = (c1 * dx + c2 * dx - 2.0f * f2 + 2.0f * f1) / dx3;
  float b = (3.0f * f2 - 3.0f * f1 - 2.0f * c1 * dx - c2 * dx) / dx2;

  // Find the root of df/dx = 3*a*x^2 + 2*b*x + c that is the *minimum*
  // of the cubic function, i.e. has positive d^2f/dx^2
  float a3 = 3.0f * a;
  float desc = b * b - a3 * c1;
  if (NMP::nmfabs(a) < gEps)
  {
    // df/dx is a linear function
    xmin = -c1 / (2.0f * b);
  }
  else if (desc <= 0.0f)
  {
    // df/dx has complex roots. Use the real component of the roots.
    xmin = -b / a3;
  }
  else
  {
    // Compute the two roots stabally
    float signb = NMP::floatSelect(b, 1.0f, -1.0f);
    float q = -(b + signb * sqrtf(desc));
    float x1 = q / a3;
    float x2 = c1 / q;

    // Determine the root corresponding to the minimum
    // xmin = a3 * x1 >= a3 * x2 ? x1 : x2
    xmin = NMP::floatSelect(a3 * x1 - a3 * x2, x1, x2);
  }

  return xmin;
}

//----------------------------------------------------------------------------------------------------------------------
// Newton iteration algorithm to calculate the set of multi-linear weights t_i that
// minimise the cost error function C(t) = \sum_i (x_i - f(V : t_i))^2
// where x_i are the components of the input point, V are the vertex samples of the
// ND cell and t_i are the components of the multi-linear weights.
//----------------------------------------------------------------------------------------------------------------------
NDCellSolverReturnVal multilinearCellCoeffsND(
  uint32_t              numDimensions,
  const float*          x,
  float*                ts,
  const float* const*   vertices,
  NDCellSolverData*     solverData)
{
  NMP_ASSERT(numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_ASSERT(x);
  NMP_ASSERT(ts);
  NMP_ASSERT(vertices);
  NMP_ASSERT(solverData);
  NMP_ASSERT(solverData->getMaxIterations() < 50); // Sensible limit for data validation

#ifdef NM_PROFILE_NDMESH_INTERPOLATION
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearCellCoeffsND");
#endif

  static const float xCostTol = 1e-8f; // Squared error convergence tolerance
  static const float xSDValAbsTol = 2e-4f;
  const uint32_t numSampleComponents = numDimensions;

  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(numDimensions);

  //-----------------------------
  // Coefficients for the next estimate
  float tn[SCATTERED_DATA_VECTOR_SIZE];

  // The parameter update vector tbar for the current step used for the search direction.
  float searchDir[SCATTERED_DATA_VECTOR_SIZE];

  // Vector of residual components between input point x and the estimate xEst: x_i - f(V : t_i)
  float r[SCATTERED_DATA_VECTOR_SIZE];

  // Jacobian: the complete matrix of partial derivatives df(V:t)/dt
  float J[SCATTERED_DATA_VECTOR_SIZE][SCATTERED_DATA_VECTOR_SIZE];

  // The right hand side vector for H * tbar = -dC(t_cur)/dt
  float Jtr[SCATTERED_DATA_VECTOR_SIZE];

  // The gradient of the cost function C(t) w.r.t the multi-linear weights t: dC(t)/dt
  float grad[SCATTERED_DATA_VECTOR_SIZE];

  //------------------------------
  // INITIALISE THE SEARCH DIRECTION
#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN("Init search direction");
#endif

  // Initialise the parameters to the centre of the ND cell
  solverData->setNumIterations(0);
  const float* initialInterpolants = solverData->getInitialInterpolants();
  if (initialInterpolants)
  {
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      ts[dim] = initialInterpolants[dim];
    }
  }
  else
  {
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      ts[dim] = 0.5f;
    }
  }

  // Compute the Jacobian and residuals at the current parameter estimate
  float costCur = managerDataBlock.m_multilinearCellCoeffsJacFn(numDimensions, x, ts, vertices, J, r);

  // Termination due to small cost error
  if (costCur < xCostTol)
  {
    solverData->setSolverStatus(kConvergedCostTol);
    return kNDCellSolveInterpolated;
  }

  // RHS vector for H * tbar = -dC(t_cur)/dt
  for (uint32_t row = 0; row < numDimensions; ++row)
  {
    Jtr[row] = 0.0f;
    for (uint32_t i = 0; i < numSampleComponents; ++i)
    {
      Jtr[row] += J[i][row] * r[i];
    }

    // Cost function gradient
    grad[row] = -2.0f * Jtr[row];
  }

  // Solve H * tbar = -dC(t_cur)/dt for the initial search direction
  bool detStatus = managerDataBlock.m_multilinearCellCoeffsSearchDirFn(numDimensions, searchDir, J, Jtr);
  if (!detStatus)
  {
#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
    PREDICTION_PROFILE_END();
#endif
    solverData->setSolverStatus(kHessianSingular);
    return kNDCellSolveFailed;
  }

  // Initialise the iteration state
  uint32_t numItsOutsideCell = 0;
  float curStepSize = 1.0f; // Reset the step length
  float gradSDCur = 0.0f;
  float maxSDValAbs = 0.0f;
  for (uint32_t dim = 0; dim < numDimensions; ++dim)
  {
    // Compute the gradient in the new search direction
    gradSDCur += grad[dim] * searchDir[dim];

    // Next parameter estimate
    tn[dim] = ts[dim] + searchDir[dim];

    // Termination tolerance
    float absVal = NMP::nmfabs(searchDir[dim]);
    maxSDValAbs = NMP::maximum(maxSDValAbs, absVal);
  }

  // Check if the maximum component of the search direction is sufficiently small
  if (maxSDValAbs < xSDValAbsTol)
  {
#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
    PREDICTION_PROFILE_END();
#endif
    solverData->setSolverStatus(kConvergedDeltaParamTol);
    return kNDCellSolveInterpolated;
  }

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_END(); // Init search direction
#endif

  //------------------------------
  // Parameter update iteration
  solverData->setSolverStatus(kTerminatedMaxIterations);
  NDCellSolverReturnVal resultStatus = kNDCellSolveFailed; // Flag for solver converged inside the ND Cell
  bool transitionClampEnable = true;

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_BEGIN("Iterations");
#endif
  while (solverData->getNumIterations() < solverData->getMaxIterations())
  {
    // Update the iteration count
    solverData->incrementNumIterations();

    // CALCULATION OF JACOBIAN AND RESIDUALS FOR THE NEW PARAMETER ESTIMATE
    float costNext = managerDataBlock.m_multilinearCellCoeffsJacFn(numDimensions, x, tn, vertices, J, r);

    //------------------------------
    // UPDATE PARAMETERS IF THE COST ERROR WAS REDUCED
    bool calcNewSearchDir = false;
    if (costNext < costCur)
    {
      calcNewSearchDir = true;

      // Update the current parameter state: tc, Cc
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        ts[dim] = tn[dim];
      }
      costCur = costNext;

      // Check if the parameters lay inside the ND cell
      uint32_t numItsOutsideCellOld = numItsOutsideCell;
      numItsOutsideCell = 0;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        if (ts[dim] < gMinCellTol || ts[dim] > gMaxCellTol)
        {
          numItsOutsideCell = numItsOutsideCellOld + 1;
          break;
        }
      }

      // Check if the parameters have just transitioned from inside the cell
      // to outside. If we've just made a large parameter update (i.e. the first update)
      // then it is quite possible that the parameters may have jumped sufficiently close
      // to the solution (which lies on the cell border) to terminate the solver iteration.
      // However, if the update placed the parameters slightly outside the cell then the solver
      // will erroneously return false. We should let the solver do one more iteration to allow
      // the next update to move the parameters back into the cell.
      if (numItsOutsideCell == 0)
      {
        // The solution is inside the cell: Check if the residual error is sufficiently small
        if (costNext < xCostTol)
        {
          solverData->setSolverStatus(kConvergedCostTol);
          resultStatus = kNDCellSolveInterpolated;
          break;
        }
      }
      else if (!solverData->getEnableExtrapolation())
      {
        // Interpolation only: The discontinuity between neighbouring NDCell boundaries
        // can cause significant problems with slow convergence and termination conditions.
        // In the worst case a query point that should lay on the border between adjacent
        // NDCells will iterate to a point just outside, and the solver will fail. The query
        // point will then be projected onto the nearest external facet boundary, even if
        // the point clearly lies inside the interior of the NDMesh. On the first transition
        // from inside to outside of the NDCell, the interpolants should be clamped so that
        // the point lies on the boundary. Further iterations will then move the point either
        // back inside or further outside the boundary.
        if (transitionClampEnable)
        {
          // Perform the clamp
          for (uint32_t dim = 0; dim < numDimensions; ++dim)
          {
            ts[dim] = NMP::clampValue(ts[dim], 0.0f, 1.0f);
          }
          transitionClampEnable = false;
          numItsOutsideCell = 0;

          // Recompute the Jacobian at the clamped parameter estimate. Note that this is likely to
          // cause the current cost error to increase. However, this discontinuity will only be
          // performed once but prevents convergence issues for query points near the NDCell borders.
          costCur = managerDataBlock.m_multilinearCellCoeffsJacFn(numDimensions, x, ts, vertices, J, r);
        }
        else if (numItsOutsideCell > 1)
        {
          // The parameters have been outside the cell now for two reductions of the cost function.
          // We assume that the parameters will hereafter continue to diverge further from the cell.
          // The result should contain the updated multi-linear weights for point outside the cell
          // at the current iteration, and a result code indicating divergence outside the cell.
          solverData->setSolverStatus(kDivergedOutsideNDCell);
          resultStatus = kNDCellSolveOutside;
          break;
        }
      }
    }

    ///--------------------------
    // RHS vector for H * tbar = -dC(t_cur)/dt
    for (uint32_t row = 0; row < numDimensions; ++row)
    {
      Jtr[row] = 0.0f;
      for (uint32_t i = 0; i < numSampleComponents; ++i)
      {
        Jtr[row] += J[i][row] * r[i];
      }

      // Cost function gradient
      grad[row] = -2.0f * Jtr[row];
    }

    //------------------------------
    // CALCULATE A NEW SEARCH DIRECTION
    if (calcNewSearchDir)
    {
      // Solve H * tbar = -dC(t_cur)/dt for the new search direction
      detStatus = managerDataBlock.m_multilinearCellCoeffsSearchDirFn(numDimensions, searchDir, J, Jtr);
      if (!detStatus)
      {
        // We were unable to compute the new search direction since the Hessian was singular.
        // While the interior of a well defined annotation cell can be interpolated, not all of the
        // exterior space can be reached by multi-linear interpolation (unless the sample vertices
        // form a rectangular structure, the locus of interpolation tends to butterfly about some
        // focal point). This may present a problem for extrapolation if the query point lies within
        // the unreachable space.
        solverData->setSolverStatus(kHessianSingular);
        resultStatus = kNDCellSolveFailed;
        break;
      }

      curStepSize = 1.0f; // Reset the step length
      gradSDCur = 0.0f;
      maxSDValAbs = 0.0f;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        // Compute the gradient in the new search direction
        gradSDCur += grad[dim] * searchDir[dim];

        // Next parameter estimate
        tn[dim] = ts[dim] + searchDir[dim];

        // Termination tolerance
        float absVal = NMP::nmfabs(searchDir[dim]);
        maxSDValAbs = NMP::maximum(maxSDValAbs, absVal);
      }

      // Check if the maximum component of the search direction is sufficiently small
      if (maxSDValAbs < xSDValAbsTol)
      {
        solverData->setSolverStatus(kConvergedDeltaParamTol);
        if (numItsOutsideCell == 0)
        {
          resultStatus = kNDCellSolveInterpolated;
        }
        else
        {
          // We converged on the appropriate solution that is outside the NDCell
          resultStatus = kNDCellSolveExtrapolated;
        }
        break;
      }
    }
    else
    {
      //------------------------------
      // STEP LENGTH REDUCTION ALONG THE CURRENT SEARCH DIRECTION

      // Compute the gradient in the search direction
      float gradSDNext = 0.0f;
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        gradSDNext += grad[dim] * searchDir[dim];
      }

      if (gradSDNext > 0.0f)
      {
        // The gradient at the next parameter position is uphill. Fit a cubic polynomial to
        // the current and next position and gradient information then find the step size
        // that takes us directly to the minimum along the search direction.
        curStepSize = cubicFuncMin(costCur, costNext, gradSDCur, gradSDNext, curStepSize);
      }
      else
      {
        // No reduction in the residual error so halve the step length to avoid bouncing
        // between the sides of the valley.
        curStepSize *= 0.5f;
      }

      // Compute the next parameter estimate
      for (uint32_t dim = 0; dim < numDimensions; ++dim)
      {
        tn[dim] = ts[dim] + curStepSize * searchDir[dim];
      }
    }
  }

#ifdef NM_PROFILE_NDMESH_SOLVER_INTERNAL
  PREDICTION_PROFILE_END(); // Iterations
#endif

  return resultStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/// MR::ScatteredDataManager
//----------------------------------------------------------------------------------------------------------------------
const ScatteredDataManager::DataBlock ScatteredDataManager::sm_dataBlocks[SCATTERED_DATA_MAX_DIM] =
{
  //------------------------
  // 1 Dimension
  {
    computeMultilinearInterp1D,
    multilinearCellCoeffs1D,
    multilinearCellCoeffs1DSearchDir,
    multilinearCellCoeffs1DJac
  },

  //------------------------
  // 2 Dimensions
  {
    computeMultilinearInterp2D,
    multilinearCellCoeffs2D,
    multilinearCellCoeffs2DSearchDir,
    multilinearCellCoeffs2DJac
  },

  //------------------------
  // 3 Dimensions
  {
    computeMultilinearInterp3D,
    multilinearCellCoeffsND,
    multilinearCellCoeffs3DSearchDir,
    multilinearCellCoeffs3DJac
  },

  //------------------------
  // 4 Dimensions
  {
    computeMultilinearInterpND,
    multilinearCellCoeffsND,
    multilinearCellCoeffsNDSearchDir,
    multilinearCellCoeffsNDJac
  },

  //------------------------
  // 5 Dimensions
  {
    computeMultilinearInterpND,
    multilinearCellCoeffsND,
    multilinearCellCoeffsNDSearchDir,
    multilinearCellCoeffsNDJac
  },
};

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
