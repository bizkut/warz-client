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
#include "morpheme/Prediction/mrNDMesh.h"
#include "morpheme/Prediction/mrNDMeshQueryData.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

#define NM_PROFILE_NDMESH_INTERNALx

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
// MR::ScatteredData::NDMesh
//----------------------------------------------------------------------------------------------------------------------
void NDMesh::dislocate(NDMesh* target)
{
  // Sample centre
  NMP::endianSwapArray(
    target->m_sampleCentre,
    target->m_numComponentsPerSample);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_sampleCentre, target);

  // Definition data for convenience  
  NMP::endianSwapArray(
    target->m_analysisPropertyComponentIndices,
    target->m_numAnalysisProperties);
  UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_analysisPropertyComponentIndices, target);

  // Vertex samples
  NMP::endianSwapArray(
    target->m_vertexSamples,
    target->m_numSamples * target->m_numComponentsPerSample);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_vertexSamples, target);

  // Quantisation information
  NMP::endianSwapArray(
    target->m_scalesQuantisedToReal,
    target->m_numComponentsPerSample);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_scalesQuantisedToReal, target);
  NMP::endianSwapArray(
    target->m_offsetsQuantisedToReal,
    target->m_numComponentsPerSample);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_offsetsQuantisedToReal, target);

  if (target->m_numDimensions > 0)
  {
    // Query normalisation
    NMP::endianSwapArray(
      target->m_scalesQuantisedToNorm,
      target->m_numComponentsPerSample);
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_scalesQuantisedToNorm, target);
    NMP::endianSwapArray(
      target->m_offsetsQuantisedToNorm,
      target->m_numComponentsPerSample);
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_offsetsQuantisedToNorm, target);

    NMP::endianSwapArray(
      target->m_scalesRealToNorm,
      target->m_numComponentsPerSample);
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_scalesRealToNorm, target);
    NMP::endianSwapArray(
      target->m_offsetsRealToNorm,
      target->m_numComponentsPerSample);
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_offsetsRealToNorm, target);

    // Control component start and end values
    for (uint32_t i = 0; i < target->m_numDimensions; ++i)
    {
      NMP::endianSwap(target->m_controlComponentSampleScales[i]);
      NMP::endianSwap(target->m_controlComponentEndValues[i]);
      NMP::endianSwap(target->m_controlComponentStartValues[i]);
      NMP::endianSwap(target->m_nDBlockSizes[i]);
      NMP::endianSwap(target->m_sampleCountsPerDimension[i]);
    }
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_controlComponentSampleScales, target);
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_controlComponentEndValues, target);
    UNFIX_SWAP_PTR_RELATIVE(float, target->m_controlComponentStartValues, target);
    UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_nDBlockSizes, target);
    UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_sampleCountsPerDimension, target);
  }

  // Header
  NMP::endianSwap(target->m_numSamples);
  NMP::endianSwap(target->m_numComponentsPerSample);
  NMP::endianSwap(target->m_numAnalysisProperties);
  NMP::endianSwap(target->m_numDimensions);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::locate(NDMesh* target)
{
  // Header
  NMP::endianSwap(target->m_numDimensions);
  NMP::endianSwap(target->m_numAnalysisProperties);
  NMP::endianSwap(target->m_numComponentsPerSample);
  NMP::endianSwap(target->m_numSamples);

  if (target->m_numDimensions > 0)
  {
    // Control component start and end values
    REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_sampleCountsPerDimension, target);
    REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_nDBlockSizes, target);
    REFIX_SWAP_PTR_RELATIVE(float, target->m_controlComponentStartValues, target);
    REFIX_SWAP_PTR_RELATIVE(float, target->m_controlComponentEndValues, target);
    REFIX_SWAP_PTR_RELATIVE(float, target->m_controlComponentSampleScales, target);
    for (uint32_t i = 0; i < target->m_numDimensions; ++i)
    {
      NMP::endianSwap(target->m_sampleCountsPerDimension[i]);
      NMP::endianSwap(target->m_nDBlockSizes[i]);
      NMP::endianSwap(target->m_controlComponentStartValues[i]);
      NMP::endianSwap(target->m_controlComponentEndValues[i]);
      NMP::endianSwap(target->m_controlComponentSampleScales[i]);
    }

    // Query normalisation
    REFIX_SWAP_PTR_RELATIVE(float, target->m_scalesRealToNorm, target);
    NMP::endianSwapArray(
      target->m_scalesRealToNorm,
      target->m_numComponentsPerSample);
    REFIX_SWAP_PTR_RELATIVE(float, target->m_offsetsRealToNorm, target);
    NMP::endianSwapArray(
      target->m_offsetsRealToNorm,
      target->m_numComponentsPerSample);

    REFIX_SWAP_PTR_RELATIVE(float, target->m_scalesQuantisedToNorm, target);
    NMP::endianSwapArray(
      target->m_scalesQuantisedToNorm,
      target->m_numComponentsPerSample);
    REFIX_SWAP_PTR_RELATIVE(float, target->m_offsetsQuantisedToNorm, target);
    NMP::endianSwapArray(
      target->m_offsetsQuantisedToNorm,
      target->m_numComponentsPerSample);
  }

  // Quantisation information
  REFIX_SWAP_PTR_RELATIVE(float, target->m_scalesQuantisedToReal, target);
  NMP::endianSwapArray(
    target->m_scalesQuantisedToReal,
    target->m_numComponentsPerSample);
  REFIX_SWAP_PTR_RELATIVE(float, target->m_offsetsQuantisedToReal, target);
  NMP::endianSwapArray(
    target->m_offsetsQuantisedToReal,
    target->m_numComponentsPerSample);

  // Vertex samples
  REFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_vertexSamples, target);
  NMP::endianSwapArray(
    target->m_vertexSamples,
    target->m_numSamples * target->m_numComponentsPerSample);

  // Definition data for convenience
  REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_analysisPropertyComponentIndices, target);
  NMP::endianSwapArray(
    target->m_analysisPropertyComponentIndices,
    target->m_numAnalysisProperties);

  // Sample centre
  REFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_sampleCentre, target);
  NMP::endianSwapArray(
    target->m_sampleCentre,
    target->m_numComponentsPerSample);
}

//----------------------------------------------------------------------------------------------------------------------
NDMesh* NDMesh::relocate(void*& ptr)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMesh), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMesh* result = (NDMesh*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Quantisation information
  NMP::Memory::Format memReqsQInfo(sizeof(float) * result->m_numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);

  if (result->m_numDimensions > 0)
  {
    // Sample counts in each dimension
    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * result->m_numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_sampleCountsPerDimension = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSampleCounts);

    // ND Block sizes
    result->m_nDBlockSizes = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSampleCounts);

    // Control component start and end values
    NMP::Memory::Format memReqsCPRanges(sizeof(float) * result->m_numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_controlComponentStartValues = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsCPRanges);
    result->m_controlComponentEndValues = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsCPRanges);
    result->m_controlComponentSampleScales = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsCPRanges);

    // Query normalisation
    result->m_scalesRealToNorm = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsQInfo);
    result->m_offsetsRealToNorm = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsQInfo);

    result->m_scalesQuantisedToNorm = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsQInfo);
    result->m_offsetsQuantisedToNorm = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsQInfo);
  }

  // Quantisation information
  result->m_scalesQuantisedToReal = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsQInfo);
  result->m_offsetsQuantisedToReal = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsQInfo); 

  // Vertex samples
  NMP::Memory::Format memReqsSamples(
    sizeof(uint16_t) * result->m_numComponentsPerSample * result->m_numSamples,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_vertexSamples = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSamples);

  // Definition data for convenience
  NMP::Memory::Format memReqsAPIndices(sizeof(uint32_t) * result->m_numAnalysisProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_analysisPropertyComponentIndices = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsAPIndices);

  // Sample centre
  NMP::Memory::Format memReqsCentre(sizeof(uint16_t) * result->m_numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sampleCentre = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsCentre);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
InterpolationReturnVal NDMesh::mapFromCPs(
  float*          queryPoint,
  uint32_t        numOutputComponents,
  const uint32_t* outputComponentIndices,
  float*          outputComponentValues) const
{
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(numOutputComponents > 0);
  NMP_ASSERT(outputComponentIndices);
  NMP_ASSERT(outputComponentValues);

#ifdef NM_DEBUG
  for (uint32_t i = 0; i < numOutputComponents; ++i)
  {
    NMP_ASSERT(outputComponentIndices[i] < m_numComponentsPerSample);
  }
#endif

  PREDICTION_PROFILE_BEGIN("mapFromCPs");

  //------------------------------
  // Special case for 0 dimension models
  if (m_numDimensions == 0)
  {
    const uint32_t vertexIndex = 0;
    getVertexComponents(
      vertexIndex,
      numOutputComponents,
      outputComponentIndices,
      outputComponentValues);

    return INTERPOLATION_SUCCESS;
  }

  //------------------------------
  // Calculate the multi-linear weights
  uint32_t topLeftCoord[SCATTERED_DATA_MAX_DIM];
  float ts[SCATTERED_DATA_MAX_DIM];
  InterpolationReturnVal result = multilinearWeights(queryPoint, topLeftCoord, ts);

  //------------------------------
  // Compute the multi-linear interpolation
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(m_numDimensions);

  // Initialise some variable-length working arrays
  float vertexData[SCATTERED_DATA_MAX_CELL_VERTICES][SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  float* vertexComponents[SCATTERED_DATA_MAX_CELL_VERTICES];

  uint32_t numVertices = getNumVerticesPerNDCell();
  for (uint32_t i = 0; i < numVertices; ++i)
  {
    float* v = vertexData[i];
    vertexComponents[i] = v;
  }

  // Retrieve the NDCell vertex sample data
  getNDCellVertexComponents(
    topLeftCoord,           // IN
    numOutputComponents,    // IN
    outputComponentIndices, // IN
    vertexComponents);      // OUT

  // Interpolate the sample components.
  managerDataBlock.m_multilinearInterpFn(
    m_numDimensions,        // IN
    numOutputComponents,    // IN
    outputComponentValues,  // OUT
    ts,                     // IN
    vertexComponents);      // IN

  PREDICTION_PROFILE_END();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
InterpolationReturnVal NDMesh::multilinearWeights(
  float*      queryPoint, // IN/OUT
  uint32_t*   topLeftCoord,
  float*      ts) const
{
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(ts);

#ifdef NM_PROFILE_NDMESH_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("multilinearWeights");
#endif

  InterpolationReturnVal result = INTERPOLATION_SUCCESS;

  // Calculate multi-linear weights with projection if outside annotation
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    // Cells must have at least two boundary samples. We don't allow single
    // samples in a dimension for a prediction model since it doesn't model
    // a controlling input.
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);

    // Compute the regular sample grid position 
    float u = (queryPoint[dim] - m_controlComponentStartValues[dim]) * m_controlComponentSampleScales[dim];

    // Project the query point onto the sample grid if necessary
    float fLastIndex = (float)(m_sampleCountsPerDimension[dim] - 1);
    if (u < 0.0f)
    {
      queryPoint[dim] = m_controlComponentStartValues[dim];
      u = 0.0f;
      result = INTERPOLATION_PROJECTED;
    }
    else if (u > fLastIndex)
    {
      queryPoint[dim] = m_controlComponentEndValues[dim];
      u = fLastIndex;
      result = INTERPOLATION_PROJECTED;
    }

    // Compute the top left grid position and interpolant. Note that the position and interpolant
    // are adjusted for the last span if the CP parameter is at the end sample value
    topLeftCoord[dim] = NMP::minimum((uint32_t)u, m_sampleCountsPerDimension[dim] - 2);
    NMP_ASSERT(topLeftCoord[dim] < m_sampleCountsPerDimension[dim] - 1);
    ts[dim] = u - (float)topLeftCoord[dim];
    NMP_ASSERT(ts[dim] >= 0.0f && ts[dim] <= 1.0f);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMesh::interpolate(
  const uint32_t* topLeftCoord,
  InterpolateResultQuery* resultData) const
{
  NMP_ASSERT(resultData);
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(m_numDimensions);

  // Retrieve the input NDCell vertex sample data
  getNDCellVertexComponents(
    topLeftCoord,
    m_numDimensions,
    resultData->getQueryPointComponentIndices(),
    m_scalesQuantisedToNorm,
    m_offsetsQuantisedToNorm,
    resultData->getVertexComponents());

  // Retrieve the coefficients and whether this point is inside the cell.
  NMP_ASSERT(!(resultData->getSolverData()->getEnableExtrapolation()));
  float interpolants[SCATTERED_DATA_MAX_DIM];

  NDCellSolverReturnVal ndCellSolverReturnValue = managerDataBlock.m_multilinearCellCoeffsFn(
    m_numDimensions,
    resultData->getQueryPoint(),
    interpolants,
    resultData->getVertexComponents(),
    resultData->getSolverData());

  if (ndCellSolverReturnValue == kNDCellSolveInterpolated)
  {
    NDMeshQueryResult* ndMeshQueryResult = resultData->getNDMeshQueryResult();
    ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCell;

    // Clamp the interpolants to the cell bounds to remove the numerical error
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      ndMeshQueryResult->m_topLeftCoord[dim] = topLeftCoord[dim];
      ndMeshQueryResult->m_interpolants[dim] = NMP::clampValue(interpolants[dim], 0.0f, 1.0f);
    }

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMesh::interpolateUsingGuidedSearch(
  const uint32_t* initTopLeftCoord,
  const float* initInterpolants,
  InterpolateResultQuery* resultData) const
{
  NMP_ASSERT(initTopLeftCoord);
  NMP_ASSERT(initInterpolants);
  NMP_ASSERT(resultData);

  // Use the resample map interpolants for the initial solution (iterative solver)
  NMP_ASSERT(!(resultData->getSolverData()->getEnableExtrapolation()));
  NDMeshQueryResult* ndMeshQueryResult = resultData->getNDMeshQueryResult();
  NMP_ASSERT(ndMeshQueryResult);
  ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCellInvalid;
  resultData->getSolverData()->setInitialInterpolants(ndMeshQueryResult->m_interpolants);
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    ndMeshQueryResult->m_topLeftCoord[dim] = initTopLeftCoord[dim];
    ndMeshQueryResult->m_interpolants[dim] = initInterpolants[dim];
  }

  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(m_numDimensions);

  //------------------------------
  // UNROLL THE FIRST LOOP: in general the initial coordinate and interpolant will be
  // accurate and the solution will be found on the first iteration.
  getNDCellVertexComponents(
    ndMeshQueryResult->m_topLeftCoord,
    m_numDimensions,
    resultData->getQueryPointComponentIndices(),
    m_scalesQuantisedToNorm,
    m_offsetsQuantisedToNorm,
    resultData->getVertexComponents());

  // Retrieve the coefficients and whether this point is inside the cell.
  NDCellSolverReturnVal ndCellSolverReturnValue = managerDataBlock.m_multilinearCellCoeffsFn(
    m_numDimensions,
    resultData->getQueryPoint(),
    ndMeshQueryResult->m_interpolants, // OUT
    resultData->getVertexComponents(),
    resultData->getSolverData());

  if (ndCellSolverReturnValue == kNDCellSolveInterpolated)
  {
    ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCell;

    // Clamp the interpolants to the cell bounds to remove the numerical error
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      ndMeshQueryResult->m_interpolants[dim] = NMP::clampValue(ndMeshQueryResult->m_interpolants[dim], 0.0f, 1.0f);
    }

    return true;
  }
  else if (ndCellSolverReturnValue == kNDCellSolveFailed)
  {
    return false;
  }

  //------------------------------
  // Initialisation for the local cell search
  float fLastIndexBuffer[SCATTERED_DATA_MAX_DIM];
  uint32_t uLastCellBuffer[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    // Cells must have at least two boundary samples. We don't allow single
    // samples in a dimension for a prediction model since it doesn't model
    // a controlling input.
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);

    fLastIndexBuffer[dim] = (float)(m_sampleCountsPerDimension[dim] - 1);
    uLastCellBuffer[dim] = m_sampleCountsPerDimension[dim] - 2;
  }

  const uint32_t maxNumCellEntries = 5;
  uint32_t cellEntries[maxNumCellEntries];
  uint32_t numCellEntries = 0;
  uint32_t cellAddress = coordinateToAddress(ndMeshQueryResult->m_topLeftCoord);

  //------------------------------
  // LOCAL CELL SEARCH
  for (uint32_t cellSearchIndex = 0; cellSearchIndex < maxNumCellEntries; ++cellSearchIndex)
  {
    // Append the cell coordinate to the searched list
    NMP_ASSERT(numCellEntries < maxNumCellEntries);
    cellEntries[numCellEntries] = cellAddress;
    numCellEntries++;

    // Update the cell search coordinate and interpolant
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      float fValue = (float)ndMeshQueryResult->m_topLeftCoord[dim] + ndMeshQueryResult->m_interpolants[dim];
      fValue = NMP::clampValue(fValue, 0.0f, fLastIndexBuffer[dim]);

      // Compute the top left grid position and interpolant. Note that the position and interpolant
      // are adjusted for the last span if the CP parameter is at the end sample value
      ndMeshQueryResult->m_topLeftCoord[dim] = NMP::minimum((uint32_t)fValue, uLastCellBuffer[dim]);
      NMP_ASSERT(ndMeshQueryResult->m_topLeftCoord[dim] < m_sampleCountsPerDimension[dim] - 1);
      ndMeshQueryResult->m_interpolants[dim] = fValue - (float)ndMeshQueryResult->m_topLeftCoord[dim];
      NMP_ASSERT(ndMeshQueryResult->m_interpolants[dim] >= 0.0f && ndMeshQueryResult->m_interpolants[dim] <= 1.0f);
    }

    // Check if the cell has already been searched
    cellAddress = coordinateToAddress(ndMeshQueryResult->m_topLeftCoord);
    for (uint32_t i = 0; i < numCellEntries; ++i)
    {
      if (cellAddress == cellEntries[i])
      {
        return false;
      }
    }

    //------------------------------
    // Retrieve the input NDCell vertex sample data
    getNDCellVertexComponents(
      ndMeshQueryResult->m_topLeftCoord,
      m_numDimensions,
      resultData->getQueryPointComponentIndices(),
      m_scalesQuantisedToNorm,
      m_offsetsQuantisedToNorm,
      resultData->getVertexComponents());

    // Retrieve the coefficients and whether this point is inside the cell.
    ndCellSolverReturnValue = managerDataBlock.m_multilinearCellCoeffsFn(
      m_numDimensions,
      resultData->getQueryPoint(),
      ndMeshQueryResult->m_interpolants, // OUT
      resultData->getVertexComponents(),
      resultData->getSolverData());

    if (ndCellSolverReturnValue == kNDCellSolveInterpolated)
    {
      ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCell;

      // Clamp the interpolants to the cell bounds to remove the numerical error
      for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
      {
        ndMeshQueryResult->m_interpolants[dim] = NMP::clampValue(ndMeshQueryResult->m_interpolants[dim], 0.0f, 1.0f);
      }

      return true;
    }
    else if (ndCellSolverReturnValue == kNDCellSolveFailed)
    {
      return false;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMesh::projectRay(
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary,
  ProjectRayResultQuery* resultData,
  const ProjectionBasis* projectionBasis) const
{
  NMP_ASSERT(m_numDimensions > 1);
  NMP_ASSERT(resultData);
  NMP_ASSERT(projectionBasis);

  uint32_t M = m_numDimensions - 1;
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(M);

  // Get the cell facet vertex samples
  getNDCellFacetVertexComponents(
    topLeftCoord,
    whichDim,
    whichBoundary,
    m_numDimensions,
    resultData->getQueryPointComponentIndices(),
    m_scalesQuantisedToNorm,
    m_offsetsQuantisedToNorm,
    resultData->getFacetVertexComponents()); // Each facet vertex sample is Nd

  // Transform the cell facet vertices into the projection basis frame. The first (N-1)d components
  // of each transformed vertex encodes the projection of the point onto the hyperplane lying at the
  // query point, with plane normal collinear with the projection direction. The last component
  // encodes the distance of the vertex from the query point in the projection direction.
  projectionBasis->transform(
    resultData->getNumFacetVertices(),
    resultData->getFacetVertexComponents(),
    resultData->getTransformedFacetVertexComponents());

  // Test against the axis-aligned bounding box for a cheap rejection of this facet.
  bool insideAABB = resultData->preUpdate();

  // Compute the multi-linear cell interpolants for the query point in the projected hyper-plane. The
  // function return whether the query point lies inside the cell
  if (insideAABB)
  {
    float interpolants[SCATTERED_DATA_MAX_DIM];
    float projectedPoint[SCATTERED_DATA_MAX_DIM];

    NMP_ASSERT(!(resultData->getSolverData()->getEnableExtrapolation()));
    if (managerDataBlock.m_multilinearCellCoeffsFn(
      M,
      resultData->getZeroVector(),
      interpolants,
      resultData->getTransformedFacetVertexComponents(),
      resultData->getSolverData()) == kNDCellSolveInterpolated)
    {
      // Clamp the interpolants to the cell bounds to remove the numerical error
      for (uint32_t dim = 0; dim < M; ++dim)
      {
        interpolants[dim] = NMP::clampValue(interpolants[dim], 0.0f, 1.0f);
      }

      // Compute the projected point in the ND space
      managerDataBlock.m_multilinearInterpFn(
        M,                // IN: (N-1)d size of the interpolants
        m_numDimensions,  // IN: Nd size of each vertex within the cell facet
        projectedPoint,   // OUT: Nd projected point (via interpolation of the facet vertices)
        interpolants,     // IN: (N-1)d multi-linear interpolants
        resultData->getFacetVertexComponents()); // IN: cell facet vertex components (Nd)

      // Compute the distance of the projected point from the query point
      float dist = projectionBasis->distanceAlongProjectionDirection(projectedPoint);

      // Update the result
      resultData->postUpdate(
        dist,
        topLeftCoord,
        whichDim,
        whichBoundary,
        interpolants,
        projectedPoint);

      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::projection1DClosest(
  float*          queryPoint,
  const uint32_t* queryPointComponentIndices,
  NDMeshQueryResult* ndMeshQueryResult,
  uint32_t        numOutputSampleComponents,
  const uint32_t* outputSampleComponentIndices,
  float*          outputSampleComponents) const
{
  NMP_ASSERT(m_numDimensions == 1);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(m_sampleCountsPerDimension);
  NMP_ASSERT(m_sampleCountsPerDimension[0] >= 2);
  NMP_ASSERT(ndMeshQueryResult);

  // Project against the left boundary cell
  float value1;
  uint32_t startSampleIndex = 0;
  getVertexComponents(startSampleIndex, 1, queryPointComponentIndices, &value1);
  float distSqr1 = NMP::sqr(value1 - queryPoint[0]);

  // Project against the right boundary cell
  float value2;
  uint32_t endSampleIndex = m_sampleCountsPerDimension[0] - 1;
  getVertexComponents(endSampleIndex, 1, queryPointComponentIndices, &value2);
  float distSqr2 = NMP::sqr(value2 - queryPoint[0]);

  // Set the return status
  ndMeshQueryResult->m_interpolationType = NDMeshQueryResult::kNDCellFacet;
  ndMeshQueryResult->m_whichDim = 0;

  // Overwrite the query point with the boundary point
  if (distSqr1 <= distSqr2)
  {
    // NDCell facet result
    ndMeshQueryResult->m_topLeftCoord[0] = 0;
    ndMeshQueryResult->m_whichBoundary = 0;

    // Project the query point
    queryPoint[0] = value1;
  }
  else
  {
    // NDCell facet result
    ndMeshQueryResult->m_topLeftCoord[0] = m_sampleCountsPerDimension[0] - 2;
    ndMeshQueryResult->m_whichBoundary = 1;

    // Project the query point
    queryPoint[0] = value2;
  }

  // Interpolate the specified output components
  if (outputSampleComponents)
  {
    uint32_t sampleIndex = ndMeshQueryResult->m_topLeftCoord[0] + ndMeshQueryResult->m_whichBoundary;

    getVertexComponents(
      sampleIndex,
      numOutputSampleComponents,
      outputSampleComponentIndices,
      outputSampleComponents);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::projection1DExtremes(
  const uint32_t*     queryPointComponentIndices,
  NDMeshQueryResult*  ndMeshQueryResultRear,
  NDMeshQueryResult*  ndMeshQueryResultFore,
  float&              valueRear,
  float&              valueFore) const
{
  NMP_ASSERT(m_numDimensions == 1);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(m_sampleCountsPerDimension);
  NMP_ASSERT(m_sampleCountsPerDimension[0] >= 2);
  NMP_ASSERT(ndMeshQueryResultRear);
  NMP_ASSERT(ndMeshQueryResultFore);

  // Project against the left boundary cell
  float value1;
  uint32_t startSampleIndex = 0;
  getVertexComponents(startSampleIndex, 1, queryPointComponentIndices, &value1);

  // Project against the right boundary cell
  float value2;
  uint32_t endSampleIndex = m_sampleCountsPerDimension[0] - 1;
  getVertexComponents(endSampleIndex, 1, queryPointComponentIndices, &value2);
 
  // Set the return status
  ndMeshQueryResultRear->m_interpolationType = NDMeshQueryResult::kNDCellFacet;
  ndMeshQueryResultRear->m_whichDim = 0;
  ndMeshQueryResultFore->m_interpolationType = NDMeshQueryResult::kNDCellFacet;
  ndMeshQueryResultFore->m_whichDim = 0;

  if (value1 < value2)
  {
    ndMeshQueryResultRear->m_topLeftCoord[0] = 0;
    ndMeshQueryResultRear->m_whichBoundary = 0;
    valueRear = value1;

    ndMeshQueryResultFore->m_topLeftCoord[0] = m_sampleCountsPerDimension[0] - 2;
    ndMeshQueryResultFore->m_whichBoundary = 1;
    valueFore = value2;
  }
  else
  {
    ndMeshQueryResultRear->m_topLeftCoord[0] = m_sampleCountsPerDimension[0] - 2;
    ndMeshQueryResultRear->m_whichBoundary = 1;
    valueRear = value2;

    ndMeshQueryResultFore->m_topLeftCoord[0] = 0;
    ndMeshQueryResultFore->m_whichBoundary = 0;
    valueFore = value1;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMesh::extrapolateCPs(
  const uint32_t*   topLeftCoord,
  const float*      queryPoint,
  const uint32_t*   queryPointComponentIndices,
  float*            interpolants,
  bool              useInitialInterpolants,
  uint32_t          numOutputComponents,
  const uint32_t*   outputComponentIndices,
  float*            outputComponentValues) const
{
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(interpolants);
  NMP_ASSERT(numOutputComponents > 0);
  NMP_ASSERT(outputComponentIndices);
  NMP_ASSERT(outputComponentValues);

  PREDICTION_PROFILE_BEGIN_CONTEXT("extrapolateCPs");

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    queryPoint,
    queryPointNorm);

  // Initialisation
  InterpolateResult resultData;
  resultData.init(
    m_numDimensions,
    queryPointComponentIndices,
    queryPointNorm,
    true); // Extrapolate

  float* const* vertexComponents = resultData.getVertexComponents();
  NDCellSolverData* solverData = resultData.getSolverData();
  if (useInitialInterpolants)
  {
    solverData->setInitialInterpolants(interpolants);
  }

  // Retrieve the NDCell query AP vertex samples
  getNDCellVertexComponents(
    topLeftCoord,
    m_numDimensions,
    queryPointComponentIndices,
    m_scalesQuantisedToNorm,
    m_offsetsQuantisedToNorm,
    vertexComponents);

  // Extrapolate the multi-linear coefficients
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(m_numDimensions);

  NDCellSolverReturnVal solverResult;
  solverResult = managerDataBlock.m_multilinearCellCoeffsFn(
    m_numDimensions,
    queryPoint,
    interpolants,
    vertexComponents,
    solverData);

  // Interpolate the output components based on the multi-linear weights. Note the extrapolated result
  // can often take a long time to converge. In these situations we are likely to be quite close to the
  // appropriate result. Seeing as we are not too bothered about the accuracy of the extrapolated point
  // we should consider this a successful solution.
  if (solverResult != kNDCellSolveFailed || solverData->getSolverStatus() == kTerminatedMaxIterations)
  {
    // Retrieve the NDCell output CP vertex samples
    getNDCellVertexComponents(
      topLeftCoord,
      m_numDimensions,
      outputComponentIndices,
      vertexComponents);

    // Interpolate based on the multi-linear weights
    managerDataBlock.m_multilinearInterpFn(
      m_numDimensions,
      numOutputComponents,
      outputComponentValues,
      interpolants,
      vertexComponents);

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getVertex(
  uint32_t vertexSampleIndex,
  float* vertex) const
{
  NMP_ASSERT(vertexSampleIndex < m_numSamples);
  NMP_ASSERT(vertex);

  // Get the vertex sample quantised data
  uint32_t offset = vertexSampleIndex * m_numComponentsPerSample;
  const uint16_t* vertexSrc = &m_vertexSamples[offset];

  // Dequantise the data
  for (uint32_t i = 0; i < m_numComponentsPerSample; ++i)
  {
    vertex[i] = m_scalesQuantisedToReal[i] * (float)vertexSrc[i] + m_offsetsQuantisedToReal[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getVertexComponents(
  uint32_t vertexSampleIndex,
  uint32_t numSampleComponents,
  const uint32_t* whichSampleComponents,
  float* vertexComponents) const
{
  NMP_ASSERT(vertexSampleIndex < m_numSamples);
  NMP_ASSERT(numSampleComponents > 0 && numSampleComponents <= m_numComponentsPerSample);
  NMP_ASSERT(whichSampleComponents);
  NMP_ASSERT(vertexComponents);

  // Get the vertex sample quantised data
  uint32_t offset = vertexSampleIndex * m_numComponentsPerSample;
  const uint16_t* vertexSrc = &m_vertexSamples[offset];

  // Dequantise the data
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    uint32_t index = whichSampleComponents[i];
    NMP_ASSERT(index < m_numComponentsPerSample);
    vertexComponents[i] = m_scalesQuantisedToReal[index] * (float)vertexSrc[index] + m_offsetsQuantisedToReal[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getNDCellVertexComponents(
  const uint32_t* topLeftCoord,
  uint32_t numSampleComponents,
  const uint32_t* whichSampleComponents,
  const float* qScales,
  const float* qOffsets,
  float* const* vertexComponents) const
{
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(whichSampleComponents);
  NMP_ASSERT(qScales);
  NMP_ASSERT(qOffsets);
  NMP_ASSERT(vertexComponents);

#ifdef NM_PROFILE_NDMESH_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("getNDCellVertexComponents");
#endif

  // Recover the vertex samples for the NDCell
  uint32_t numVertices = 1 << m_numDimensions;
  for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
  {
    // Convert vertex index into a coordinate offset from the 'top left' of the cell for each dimension.
    uint32_t vertexSampleIndex = 0;
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      // Calculate the coordinate offset for the component of the vertex. This is an offset
      // depending on whether the bit for this element is 1 or 0.
      uint32_t offset = (vertex >> dim) & 0x01;
      NMP_ASSERT(topLeftCoord[dim] + offset < m_sampleCountsPerDimension[dim]);

      // Update the vertex sample index within the stored flat sample data
      vertexSampleIndex += (topLeftCoord[dim] + offset) * m_nDBlockSizes[dim];
    }

    // Get the vertex sample quantised data
    NMP_ASSERT(vertexSampleIndex < m_numSamples);
    uint32_t offset = vertexSampleIndex * m_numComponentsPerSample;
    const uint16_t* vertexSrc = &m_vertexSamples[offset];

    // Dequantise the vertex sample data
    float* vertexData = vertexComponents[vertex];
    NMP_ASSERT(vertexData);
    for (uint32_t i = 0; i < numSampleComponents; ++i)
    {
      uint32_t index = whichSampleComponents[i];
      NMP_ASSERT(index < m_numComponentsPerSample);
      vertexData[i] = qScales[index] * (float)vertexSrc[index] + qOffsets[index];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getNDCellFacetVertexComponents(
  const uint32_t* topLeftCoord,
  uint32_t whichDim,
  uint32_t whichBoundary,
  uint32_t numSampleComponents,
  const uint32_t* whichSampleComponents,
  const float* qScales,
  const float* qOffsets,
  float* const* vertexComponents) const
{
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(whichDim < m_numDimensions);
  NMP_ASSERT(whichBoundary <= 1); // 0 or 1
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(whichSampleComponents);
  NMP_ASSERT(qScales);
  NMP_ASSERT(qOffsets);
  NMP_ASSERT(vertexComponents);

#ifdef NM_PROFILE_NDMESH_INTERNAL
  PREDICTION_PROFILE_BEGIN_CONTEXT("getNDCellFacetVertexComponents");
#endif

  // Iterate over vertices in the ND cell facet
  uint32_t M = m_numDimensions - 1;
  uint32_t numFacetVertices = 1 << M;
  for (uint32_t vertex = 0; vertex < numFacetVertices; ++vertex)
  {
    // Pre
    uint32_t vertexSampleIndex = 0;
    for (uint32_t dim = 0; dim < whichDim; ++dim)
    {
      // Offset coordinate by 1 or 0, depending on whether the bit for this element is 1 or 0.
      uint32_t offset = (vertex >> dim) & 0x01;
      NMP_ASSERT(topLeftCoord[dim] + offset < m_sampleCountsPerDimension[dim]);
      vertexSampleIndex += (topLeftCoord[dim] + offset) * m_nDBlockSizes[dim];
    }

    // Skipped dim
    NMP_ASSERT(topLeftCoord[whichDim] + whichBoundary < m_sampleCountsPerDimension[whichDim]);
    vertexSampleIndex += (topLeftCoord[whichDim] + whichBoundary) * m_nDBlockSizes[whichDim];

    // Post
    for (uint32_t dim = whichDim + 1; dim < m_numDimensions; ++dim)
    {
      // Offset coordinate by 1 or 0, depending on whether the bit for this element is 1 or 0.
      uint32_t el = dim - 1;
      uint32_t offset = (vertex >> el) & 0x01;
      NMP_ASSERT(topLeftCoord[dim] + offset < m_sampleCountsPerDimension[dim]);
      vertexSampleIndex += (topLeftCoord[dim] + offset) * m_nDBlockSizes[dim];
    }

    // Get the vertex sample quantised data
    NMP_ASSERT(vertexSampleIndex < m_numSamples);
    uint32_t offset = vertexSampleIndex * m_numComponentsPerSample;
    const uint16_t* vertexSrc = &m_vertexSamples[offset];

    // Dequantise the vertex sample data
    float* vertexData = vertexComponents[vertex];
    NMP_ASSERT(vertexData);
    for (uint32_t i = 0; i < numSampleComponents; ++i)
    {
      uint32_t index = whichSampleComponents[i];
      NMP_ASSERT(index < m_numComponentsPerSample);
      vertexData[i] = qScales[index] * (float)vertexSrc[index] + qOffsets[index];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getSampleCentre(
  uint32_t componentsSize,
  float* components) const
{
  NMP_ASSERT(componentsSize == m_numComponentsPerSample);
  NMP_ASSERT(components);

  // Dequantise the sample centre
  for (uint32_t i = 0; i < componentsSize; ++i)
  {
    components[i] = m_scalesQuantisedToReal[i] * (float)m_sampleCentre[i] + m_offsetsQuantisedToReal[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getSampleCentre(
  uint32_t          outputComponentsSize,
  const uint32_t*   outputComponentIndices,
  float*            outputComponentValues) const
{
  NMP_ASSERT(outputComponentsSize > 0);
  NMP_ASSERT(outputComponentIndices);
  NMP_ASSERT(outputComponentValues);

  // Dequantise the sample centre
  for (uint32_t i = 0; i < outputComponentsSize; ++i)
  {
    uint32_t index = outputComponentIndices[i];
    NMP_ASSERT(index < m_numComponentsPerSample);
    outputComponentValues[i] = m_scalesQuantisedToReal[index] * (float)m_sampleCentre[index] + m_offsetsQuantisedToReal[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getSampleAABB(
  uint32_t NMP_USED_FOR_ASSERTS(componentsSize),
  float* aabbMinVals,
  float* aabbMaxVals) const
{
  NMP_ASSERT(componentsSize == m_numComponentsPerSample);
  NMP_ASSERT(aabbMinVals);
  NMP_ASSERT(aabbMaxVals);

  // Dequantise the AABB information. Note: we do not store the quantisation
  // max values as a separate float array since the vertex data is quantised
  // to 16-bits precision. We should return the precise max values representable
  // by the bit range to avoid issues with numerical error.
  for (uint32_t i = 0; i < m_numComponentsPerSample; ++i)
  {
    aabbMinVals[i] = m_offsetsQuantisedToReal[i];
    aabbMaxVals[i] = m_scalesQuantisedToReal[i] * 65535.0f + m_offsetsQuantisedToReal[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMesh::getSampleAABB(
  uint32_t        numSampleComponents,
  const uint32_t* queryPointComponentIndices,
  float*          aabbComponentMinVals,
  float*          aabbComponentMaxVals) const
{
  NMP_ASSERT(numSampleComponents > 0);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(aabbComponentMinVals);
  NMP_ASSERT(aabbComponentMaxVals);

  // Dequantise the AABB information. Note: we do not store the quantisation
  // max values as a separate float array since the vertex data is quantised
  // to 16-bits precision. We should return the precise max values representable
  // by the bit range to avoid issues with numerical error.
  for (uint32_t i = 0; i < numSampleComponents; ++i)
  {
    uint32_t index = queryPointComponentIndices[i];
    NMP_ASSERT(index < m_numComponentsPerSample);

    aabbComponentMinVals[i] = m_offsetsQuantisedToReal[index];
    aabbComponentMaxVals[i] = m_scalesQuantisedToReal[index] * 65535.0f + m_offsetsQuantisedToReal[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMesh::ndCellInterpolants(
  const NDMeshQueryResult* ndMeshQueryResult,
  float* interpolants) const
{
  NMP_ASSERT(ndMeshQueryResult);
  NMP_ASSERT(interpolants);
  NMP_ASSERT(ndMeshQueryResult->m_interpolationType != NDMeshQueryResult::kNDCellInvalid);

  const float alphas[2] = {0.0f, 1.0f};

  if (ndMeshQueryResult->m_interpolationType == NDMeshQueryResult::kNDCell)
  {
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      interpolants[dim] = ndMeshQueryResult->m_interpolants[dim];
    }
    return true;
  }
  else
  {
    NMP_ASSERT(ndMeshQueryResult->m_interpolationType == NDMeshQueryResult::kNDCellFacet);
    uint32_t whichDim = ndMeshQueryResult->m_whichDim;
    NMP_ASSERT(whichDim < m_numDimensions);
    uint32_t whichBoundary = ndMeshQueryResult->m_whichBoundary;
    NMP_ASSERT(whichBoundary < 2);
    
    // Pre    
    for (uint32_t dim = 0; dim < whichDim; ++dim)
    {
      interpolants[dim] = ndMeshQueryResult->m_interpolants[dim];
    }

    // Skipped dim
    interpolants[whichDim] = alphas[whichBoundary];

    // Post
    for (uint32_t dim = whichDim + 1; dim < m_numDimensions; ++dim)
    {
      interpolants[dim] = ndMeshQueryResult->m_interpolants[dim - 1];
    }

    return false;
  }
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
