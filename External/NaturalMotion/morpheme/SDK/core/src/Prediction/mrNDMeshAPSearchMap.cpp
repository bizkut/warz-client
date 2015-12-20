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
#include "morpheme/Prediction/mrNDMeshAPSearchMap.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

#define NM_PROFILE_APSEARCHMAP_INTERNALx
#define NM_PROFILE_APSEARCHMAP_LEAFNODEx
#define NM_PROFILE_PROJECTION_SUBROUTINESx

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPSearchMap
//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPSearchMap::dislocate(NDMeshAPSearchMap* target)
{
  uint32_t numNodes = target->m_numBranchNodes + target->m_numLeafNodes;

  // Branch node children
  if (target->m_numBranchNodes > 0)
  {
    NMP::endianSwapArray(target->m_left, target->m_numBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_left, target);
    NMP::endianSwapArray(target->m_right, target->m_numBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_right, target);

    NMP::endianSwapArray(target->m_branchPartitionDims, target->m_numBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint8_t, target->m_branchPartitionDims, target);
  }

  // AABBs
  uint32_t numAABBElements = target->m_numAnalysedProperties * numNodes;
  NMP::endianSwapArray(target->m_aabbMaxVals, numAABBElements);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_aabbMaxVals, target);
  NMP::endianSwapArray(target->m_aabbMinVals, numAABBElements);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_aabbMinVals, target);

  // Header
  NMP::endianSwap(target->m_edgeLeafNodeOffset);
  NMP::endianSwap(target->m_edgeBranchNodeOffset);
  NMP::endianSwap(target->m_numLeafNodes);
  NMP::endianSwap(target->m_numBranchNodes);
  NMP::endianSwap(target->m_numComponentsPerSample);
  NMP::endianSwap(target->m_numAnalysedProperties);
  NMP::endianSwap(target->m_numDimensions);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPSearchMap::locate(NDMeshAPSearchMap* target)
{
  // Header
  NMP::endianSwap(target->m_numDimensions);
  NMP::endianSwap(target->m_numAnalysedProperties);
  NMP::endianSwap(target->m_numComponentsPerSample);
  NMP::endianSwap(target->m_numBranchNodes);
  NMP::endianSwap(target->m_numLeafNodes);
  NMP::endianSwap(target->m_edgeBranchNodeOffset);
  NMP::endianSwap(target->m_edgeLeafNodeOffset);
  uint32_t numNodes = target->m_numBranchNodes + target->m_numLeafNodes;

  // AABBs
  uint32_t numAABBElements = target->m_numAnalysedProperties * numNodes;
  REFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_aabbMinVals, target);
  NMP::endianSwapArray(target->m_aabbMinVals, numAABBElements);
  REFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_aabbMaxVals, target);
  NMP::endianSwapArray(target->m_aabbMaxVals, numAABBElements);

  // Branch node children
  if (target->m_numBranchNodes > 0)
  {
    REFIX_SWAP_PTR_RELATIVE(uint8_t, target->m_branchPartitionDims, target);
    NMP::endianSwapArray(target->m_branchPartitionDims, target->m_numBranchNodes);

    REFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_left, target);
    NMP::endianSwapArray(target->m_left, target->m_numBranchNodes);
    REFIX_SWAP_PTR_RELATIVE(uint16_t, target->m_right, target);
    NMP::endianSwapArray(target->m_right, target->m_numBranchNodes);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshAPSearchMap* NDMeshAPSearchMap::relocate(void*& ptr)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshAPSearchMap), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshAPSearchMap* result = (NDMeshAPSearchMap*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  uint32_t numNodes = result->m_numBranchNodes + result->m_numLeafNodes;

  // AABBs
  NMP::Memory::Format memReqsBB(sizeof(uint16_t) * result->m_numAnalysedProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsBB *= numNodes;
  result->m_aabbMinVals = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBB);
  result->m_aabbMaxVals = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBB);

  // Branch node children
  if (result->m_numBranchNodes > 0)
  {
    NMP::Memory::Format memReqsBPDims(sizeof(uint8_t) * result->m_numBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_branchPartitionDims = (uint8_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBPDims);

    NMP::Memory::Format memReqsChild(sizeof(uint16_t) * result->m_numBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_left = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsChild);
    result->m_right = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsChild);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
InterpolationReturnVal NDMeshAPSearchMap::mapFromAPs(
  const NDMesh*       nDMesh,
  float*              queryPoint,
  const uint32_t*     queryPointComponentIndices,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents,
  bool                enableProjection,
  const Projection*   projectionParams) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(ndMeshQueryResult);
  NMP_ASSERT(numOutputSampleComponents > 0);
  NMP_ASSERT(outputSampleComponentIndices);
  NMP_ASSERT(outputSampleComponents);

  PREDICTION_PROFILE_BEGIN_CONTEXT("mapFromAPs");

  InterpolationReturnVal result = INTERPOLATION_FAILED;  

  //------------------------------
  // Interpolation
  bool found = interpolate(
    nDMesh,                         // IN
    queryPoint,                     // IN: Query AP components
    queryPointComponentIndices,     // IN: Query point component indices
    ndMeshQueryResult,              // OUT: The NDCell interpolation information
    numOutputSampleComponents,      // IN
    outputSampleComponentIndices,   // IN
    outputSampleComponents);        // OUT: Complementary sample components

  if (found)
  {
    result = INTERPOLATION_SUCCESS;
  }
  else if (enableProjection)
  {
    //------------------------------
    // Default projection
    if (!projectionParams)
    {
      for (uint32_t i = 0; i < m_numDimensions; ++i)
      {
        found = projectAlongOneDimension(
          nDMesh,
          queryPoint,
          queryPointComponentIndices,
          i, // This an index into the queryPointComponentIndices
          ndMeshQueryResult,
          numOutputSampleComponents,
          outputSampleComponentIndices,
          outputSampleComponents);

        if (found)
        {
          result = INTERPOLATION_PROJECTED;
          break;
        }
      }
    }
    else if (projectionParams->getMethod() == Projection::ProjectAlongDimension)
    {
      // Find the projected point by allowing each dimension to be varied in user-specified order.
      for (uint32_t i = 0; i < projectionParams->getNumComponents(); ++i)
      {
        uint32_t whichVariableToCompromise = projectionParams->getVariableToSacrifice(i);
        found = projectAlongOneDimension(
          nDMesh,
          queryPoint,
          queryPointComponentIndices,
          whichVariableToCompromise, // This an index into the queryPointComponentIndices
          ndMeshQueryResult,
          numOutputSampleComponents,
          outputSampleComponentIndices,
          outputSampleComponents);

        if (found)
        {
          result = INTERPOLATION_PROJECTED;
          break;
        }
      }
    }
    else if (projectionParams->getMethod() == Projection::ProjectThroughPoint)
    {
      NMP_ASSERT(projectionParams->getNumComponents() == m_numDimensions);
      found = projectThroughPoint(
        nDMesh,
        queryPoint,
        queryPointComponentIndices,
        projectionParams->getProjectionCentre(),
        ndMeshQueryResult,
        numOutputSampleComponents,
        outputSampleComponentIndices,
        outputSampleComponents);

      if (found)
      {
        result = INTERPOLATION_PROJECTED;
      }
    }
    else
    {
      NMP_ASSERT(projectionParams->getMethod() == Projection::ProjectThroughSampleCentre);

      // Recover the sample centre
      float projectionCentre[SCATTERED_DATA_MAX_DIM];
      nDMesh->getSampleCentre(
        m_numDimensions,
        queryPointComponentIndices,
        projectionCentre);

      found = projectThroughPoint(
        nDMesh,
        queryPoint,
        queryPointComponentIndices,
        projectionCentre,
        ndMeshQueryResult,
        numOutputSampleComponents,
        outputSampleComponentIndices,
        outputSampleComponents);

      if (found)
      {
        result = INTERPOLATION_PROJECTED;
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::interpolate(
  const NDMesh*       nDMesh,
  const float*        queryPoint,
  const uint32_t*     queryPointComponentIndices,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(ndMeshQueryResult);

#ifdef NM_PROFILE_PROJECTION_SUBROUTINES
  PREDICTION_PROFILE_BEGIN_CONTEXT("interpolate");
#endif
  ndMeshQueryResult->init();

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    queryPoint,
    queryPointNorm);

  InterpolateResultQuery interpolateResult;
  interpolateResult.init(
    nDMesh,
    queryPointComponentIndices,
    queryPointNorm,
    ndMeshQueryResult);

  interpolateResult.initState();

  if (interpolate(0, &interpolateResult))
  {
    // Interpolate the specified output components
    if (outputSampleComponents)
    {
      NMP_ASSERT(numOutputSampleComponents > 0);

      // Retrieve the output NDCell vertex sample data
      nDMesh->getNDCellVertexComponents(
        ndMeshQueryResult->m_topLeftCoord,
        numOutputSampleComponents,
        outputSampleComponentIndices,
        interpolateResult.getVertexComponents());

      // Interpolate the output components
      const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(m_numDimensions);
      managerDataBlock.m_multilinearInterpFn(
        m_numDimensions,
        numOutputSampleComponents,
        outputSampleComponents,
        ndMeshQueryResult->m_interpolants,
        interpolateResult.getVertexComponents());
    }

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::projectAlongOneDimension(
  const NDMesh*       nDMesh,
  float*              queryPoint,
  const uint32_t*     queryPointComponentIndices,
  uint32_t            whichVariableToCompromise,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(whichVariableToCompromise < m_numDimensions);
  NMP_ASSERT(ndMeshQueryResult);

#ifdef NM_PROFILE_PROJECTION_SUBROUTINES
  PREDICTION_PROFILE_BEGIN_CONTEXT("projectAlongOneDimension");
#endif

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  if (m_numDimensions == 1)
  {
    nDMesh->projection1DClosest(
      queryPoint,
      queryPointComponentIndices,
      ndMeshQueryResult,
      numOutputSampleComponents,
      outputSampleComponentIndices,
      outputSampleComponents);

    return true;
  }

  // Initialisation
  ndMeshQueryResult->init();

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    queryPoint,
    queryPointNorm);

  ProjectionBasis projectionBasis;
  projectionBasis.initAlongDimension(
    m_numDimensions,
    queryPointNorm,
    whichVariableToCompromise);

  ProjectRayResultClosest projectionResult;
  projectionResult.initClosest(
    nDMesh,
    queryPointComponentIndices,
    ndMeshQueryResult);

  projectionResult.initState();

  // Project the query point onto the NDMesh
  if (projectRayAlongOneDimension(0, &projectionResult, &projectionBasis))
  {
    NMP_ASSERT(ndMeshQueryResult->m_interpolationType == NDMeshQueryResult::kNDCellFacet);

    // Overwrite the output query point with the projected result
    nDMesh->getNDCellFacetVertexComponents(
      ndMeshQueryResult->m_topLeftCoord,
      ndMeshQueryResult->m_whichDim,
      ndMeshQueryResult->m_whichBoundary,
      m_numDimensions,
      queryPointComponentIndices,
      projectionResult.getFacetVertexComponents());

    uint32_t M = m_numDimensions - 1;
    const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(M);
    managerDataBlock.m_multilinearInterpFn(
      M,
      m_numDimensions,
      queryPoint,
      ndMeshQueryResult->m_interpolants,
      projectionResult.getFacetVertexComponents());

    // Interpolate the specified output components
    if (outputSampleComponents)
    {
      NMP_ASSERT(numOutputSampleComponents > 0);

      // Get the ND cell vertex samples
      nDMesh->getNDCellFacetVertexComponents(
        ndMeshQueryResult->m_topLeftCoord,
        ndMeshQueryResult->m_whichDim,
        ndMeshQueryResult->m_whichBoundary,
        numOutputSampleComponents,
        outputSampleComponentIndices,
        projectionResult.getFacetVertexComponents());

      managerDataBlock.m_multilinearInterpFn(
        M,
        numOutputSampleComponents,
        outputSampleComponents,
        ndMeshQueryResult->m_interpolants,
        projectionResult.getFacetVertexComponents());
    }

    return true;
  }


  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::projectThroughPoint(
  const NDMesh*       nDMesh,
  float*              queryPoint,
  const uint32_t*     queryPointComponentIndices,
  const float*        projectionCentre,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(queryPointComponentIndices);
  NMP_ASSERT(projectionCentre);
  NMP_ASSERT(ndMeshQueryResult);

#ifdef NM_PROFILE_PROJECTION_SUBROUTINES
  PREDICTION_PROFILE_BEGIN_CONTEXT("projectThroughPoint");
#endif

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  if (m_numDimensions == 1)
  {
    nDMesh->projection1DClosest(
      queryPoint,
      queryPointComponentIndices,
      ndMeshQueryResult,
      numOutputSampleComponents,
      outputSampleComponentIndices,
      outputSampleComponents);

    return true;
  }

  // Initialisation
  ndMeshQueryResult->init();

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    queryPoint,
    queryPointNorm);

  // Apply the transform that maps the projection centre into the normalised space
  float projectionCentreNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    projectionCentre,
    projectionCentreNorm);

  ProjectionBasis projectionBasis;
  projectionBasis.initThroughPoint(
    m_numDimensions,
    queryPointNorm,
    projectionCentreNorm);

  ProjectRayResultClosest projectionResult;
  projectionResult.initClosest(
    nDMesh,
    queryPointComponentIndices,
    ndMeshQueryResult);

  projectionResult.initState();

  // Project the query point onto the NDMesh
  if (projectRayThroughPoint(0, &projectionResult, &projectionBasis))
  {
    NMP_ASSERT(ndMeshQueryResult->m_interpolationType == NDMeshQueryResult::kNDCellFacet);

    // Overwrite the output query point with the projected result
    nDMesh->getNDCellFacetVertexComponents(
      ndMeshQueryResult->m_topLeftCoord,
      ndMeshQueryResult->m_whichDim,
      ndMeshQueryResult->m_whichBoundary,
      m_numDimensions,
      queryPointComponentIndices,
      projectionResult.getFacetVertexComponents());

    uint32_t M = m_numDimensions - 1;
    const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(M);
    managerDataBlock.m_multilinearInterpFn(
      M,
      m_numDimensions,
      queryPoint,
      ndMeshQueryResult->m_interpolants,
      projectionResult.getFacetVertexComponents());

    // Interpolate the specified output components
    if (outputSampleComponents)
    {
      NMP_ASSERT(numOutputSampleComponents > 0);

      // Get the ND cell vertex samples
      nDMesh->getNDCellFacetVertexComponents(
        ndMeshQueryResult->m_topLeftCoord,
        ndMeshQueryResult->m_whichDim,
        ndMeshQueryResult->m_whichBoundary,
        numOutputSampleComponents,
        outputSampleComponentIndices,
        projectionResult.getFacetVertexComponents());

      managerDataBlock.m_multilinearInterpFn(
        M,
        numOutputSampleComponents,
        outputSampleComponents,
        ndMeshQueryResult->m_interpolants,
        projectionResult.getFacetVertexComponents());
    }

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::getRangeInDimension(
  const NDMesh*       nDMesh,
  const float*        queryPoint,
  const uint32_t*     queryPointComponentIndices,
  uint32_t            whichQueryVariable,
  float&              minimum,
  float&              maximum,
  NDMeshQueryResult*  ndMeshQueryResultRear,
  NDMeshQueryResult*  ndMeshQueryResultFore) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(whichQueryVariable < m_numDimensions);

  PREDICTION_PROFILE_BEGIN_CONTEXT("getRangeInDimension");

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  if (m_numDimensions == 1)
  {
    NMP_ASSERT(whichQueryVariable == 0);
    nDMesh->projection1DExtremes(
      queryPointComponentIndices,
      ndMeshQueryResultRear,
      ndMeshQueryResultFore,
      minimum,
      maximum);
    NMP_ASSERT(minimum <= maximum);

    return true;
  }

  // Initialisation
  ndMeshQueryResultRear->init();
  ndMeshQueryResultFore->init();

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    queryPointComponentIndices,
    queryPoint,
    queryPointNorm);

  // Initialisation
  ProjectionBasis projectionBasis;
  projectionBasis.initAlongDimension(
    m_numDimensions,
    queryPointNorm,
    whichQueryVariable);

  ProjectRayResultExtremes projectionResult;
  projectionResult.initExtremes(
    nDMesh,
    queryPointComponentIndices,
    ndMeshQueryResultRear,
    ndMeshQueryResultFore);

  projectionResult.initState();

  // Project the query point onto the NDMesh
  if (projectRayAlongOneDimension(0, &projectionResult, &projectionBasis))
  {
    uint32_t M = m_numDimensions - 1;
    const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(M);
    uint32_t index = queryPointComponentIndices[whichQueryVariable];

    // Rear point
    nDMesh->getNDCellFacetVertexComponents(
      ndMeshQueryResultRear->m_topLeftCoord,
      ndMeshQueryResultRear->m_whichDim,
      ndMeshQueryResultRear->m_whichBoundary,
      1,
      &index,
      projectionResult.getFacetVertexComponents());

    managerDataBlock.m_multilinearInterpFn(
      M,
      1,
      &minimum,
      ndMeshQueryResultRear->m_interpolants,
      projectionResult.getFacetVertexComponents());

    // Fore point
    nDMesh->getNDCellFacetVertexComponents(
      ndMeshQueryResultFore->m_topLeftCoord,
      ndMeshQueryResultFore->m_whichDim,
      ndMeshQueryResultFore->m_whichBoundary,
      1,
      &index,
      projectionResult.getFacetVertexComponents());

    managerDataBlock.m_multilinearInterpFn(
      M,
      1,
      &maximum,
      ndMeshQueryResultFore->m_interpolants,
      projectionResult.getFacetVertexComponents());

    NMP_ASSERT(minimum <= maximum);

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::projectRayThroughPoint(
  uint32_t nodeID,
  ProjectRayResultQuery* resultData,
  const ProjectionBasis* projectionBasis) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  NMP_ASSERT(resultData);
  NMP_ASSERT(projectionBasis);

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  // Projection against 1D Meshes is handled separately as a special case.
  NMP_ASSERT(m_numDimensions > 1);

  // Check if any of the AABB facets touch the external boundary
  if (!isEdgeElement(nodeID))
    return false;

  //------------------------------
  // NOTE: This is a cheep way of culling out entire branches of the APSearch tree.
  // If the ray does not intersect the branch node AABB then it will also not intersect
  // any of the sub-branch or leaf node AABBs.
  const uint32_t* queryPointComponentIndices = resultData->getQueryPointComponentIndices();
  uint32_t* coordinates = resultData->getCoordinates();
  uint32_t* cellSizes = resultData->getCellSizes();

  // Get the AABB information for the specified node
  const NDMesh* nDMesh = resultData->getNDMesh();
  NMP_ASSERT(nDMesh);
  const float* scalesRealToNorm = nDMesh->getScalesRealToNorm();
  const float* offsetsRealToNorm = nDMesh->getOffsetsRealToNorm();
  const float* scalesQuantisedToNorm = nDMesh->getScalesQuantisedToNorm();
  const float* offsetsQuantisedToNorm = nDMesh->getOffsetsQuantisedToNorm();
  uint32_t offset = nodeID * m_numAnalysedProperties;
  const uint16_t* aabbMinValsU = &m_aabbMinVals[offset];
  const uint16_t* aabbMaxValsU = &m_aabbMaxVals[offset];

  // Dequantise the AABB data
  float aabbMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbMaxVals[SCATTERED_DATA_MAX_DIM];
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    // Index of the component in the vertex sample
    uint32_t index = queryPointComponentIndices[dim];
    NMP_ASSERT(index < m_numComponentsPerSample);

    if (index < m_numDimensions)
    {
      nDMesh->getControlComponentAABB(
        index,
        coordinates[index],
        coordinates[index] + cellSizes[index],
        aabbMinVals[dim],
        aabbMaxVals[dim]);

      // Apply the normalisation transform
      aabbMinVals[dim] = scalesRealToNorm[index] * aabbMinVals[dim] + offsetsRealToNorm[index];
      aabbMaxVals[dim] = scalesRealToNorm[index] * aabbMaxVals[dim] + offsetsRealToNorm[index];
    }
    else
    {
      // The analysis property component index
      uint32_t apIndex = index - m_numDimensions;

      // Dequantise the AABB
      aabbMinVals[dim] = scalesQuantisedToNorm[index] * (float)aabbMinValsU[apIndex] + offsetsQuantisedToNorm[index];
      aabbMaxVals[dim] = scalesQuantisedToNorm[index] * (float)aabbMaxValsU[apIndex] + offsetsQuantisedToNorm[index];
    }
  }

  // Check if the projection ray intersects the node AABB
  if (!MR::ScatteredData::aabbIntersectsRay(
    m_numDimensions,
    aabbMinVals,
    aabbMaxVals,
    projectionBasis->getNumRayComponents(),
    projectionBasis->getRayComponentOrder(),
    projectionBasis->getProjectionOrigin(),
    projectionBasis->getProjectionDirection()))
  {
    return false;
  }

  //------------------------------
  // Project the ray through all child node bounding regions, if they have faces on the edge of the sample space.
  if (isBranchNode(nodeID))
  {
    // We need to project against both child branch nodes so that the result data
    // can be updated appropriately.
    uint32_t nextDimension = getBranchPartitionDim(nodeID);
    NMP_ASSERT(nextDimension < m_numDimensions);
    uint32_t coordForDim = coordinates[nextDimension];
    uint32_t cellSizeForDim = cellSizes[nextDimension];
    uint32_t nextCellSizeRight = cellSizeForDim >> 1;
    uint32_t nextCellSizeLeft = cellSizeForDim - nextCellSizeRight; // Remainder in here

    // Left branch
    cellSizes[nextDimension] = nextCellSizeLeft;
    bool found = projectRayThroughPoint(
      getBranchNodeLeft(nodeID),
      resultData,
      projectionBasis);

    // Right branch
    coordinates[nextDimension] += nextCellSizeLeft;
    cellSizes[nextDimension] = nextCellSizeRight;
    found |= projectRayThroughPoint(
      getBranchNodeRight(nodeID),
      resultData,
      projectionBasis);

    // Roll back the old coordinate and cell size information
    coordinates[nextDimension] = coordForDim;
    cellSizes[nextDimension] = cellSizeForDim;

    return found;
  }

  //------------------------------
  // This is a leaf node so do an actual cell facet intersect test.
  bool found = false;
  for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
  {
    // There are two boundary faces per dimension, so step through them.
    for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
    {
      if (nDMesh->isNDCellBoundaryFacet(resultData->getCoordinates(), whichDim, whichBoundary))
      {
        // Compute the projected point in the Nd space
        found |= nDMesh->projectRay(
          resultData->getCoordinates(),
          whichDim,
          whichBoundary,
          resultData,
          projectionBasis);

      }
    }
  }

  return found;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::projectRayAlongOneDimension(
  uint32_t nodeID,
  ProjectRayResultQuery* resultData,
  const ProjectionBasis* projectionBasis) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  NMP_ASSERT(resultData);
  NMP_ASSERT(projectionBasis);

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  // Projection against 1D Meshes is handled separately as a special case.
  NMP_ASSERT(m_numDimensions > 1);

  // Check if any of the AABB facets touch the external boundary
  if (!isEdgeElement(nodeID))
    return false;

  const uint32_t* queryPointComponentIndices = resultData->getQueryPointComponentIndices();
  const float* queryPoint = projectionBasis->getProjectionOrigin();
  uint32_t M = m_numDimensions - 1;
  NMP_ASSERT(projectionBasis->getNumRayComponents() == M); // ND - 1
  uint32_t* coordinates = resultData->getCoordinates();
  uint32_t* cellSizes = resultData->getCellSizes();

  // Get the AABB information for the specified node
  const NDMesh* nDMesh = resultData->getNDMesh();
  const float* scalesRealToNorm = nDMesh->getScalesRealToNorm();
  const float* offsetsRealToNorm = nDMesh->getOffsetsRealToNorm();
  const float* scalesQuantisedToNorm = nDMesh->getScalesQuantisedToNorm();
  const float* offsetsQuantisedToNorm = nDMesh->getOffsetsQuantisedToNorm();
  uint32_t offset = nodeID * m_numAnalysedProperties;
  const uint16_t* aabbMinValsU = &m_aabbMinVals[offset];
  const uint16_t* aabbMaxValsU = &m_aabbMaxVals[offset];

  //------------------------------
  // NOTE: This is a cheep way of culling out entire branches of the APSearch tree.
  // If the ray does not intersect the branch node AABB then it will also not intersect
  // any of the sub-branch or leaf node AABBs.
  const uint32_t* rayComponentOrder = projectionBasis->getRayComponentOrder();
  for (uint32_t basisIndex = 0; basisIndex < M; ++basisIndex)
  {
    uint32_t dim = rayComponentOrder[basisIndex];
    NMP_ASSERT(dim < m_numDimensions);
    NMP_ASSERT(dim != projectionBasis->getWhichVariableToCompromise());

    // Index of the component in the vertex sample
    uint32_t index = queryPointComponentIndices[dim];
    NMP_ASSERT(index < m_numComponentsPerSample);

    if (index < m_numDimensions)
    {
      // Remove the offset from the query point
      float x = queryPoint[dim] - offsetsRealToNorm[index];

      float aabbMinValue, aabbMaxValue;
      nDMesh->getControlComponentAABB(
        index,
        coordinates[index],
        coordinates[index] + cellSizes[index],
        aabbMinValue,
        aabbMaxValue);

      // Min value
      aabbMinValue *= scalesRealToNorm[index];
      if (x < aabbMinValue - gAABBTol)
      {
        return false;
      }

      // Max value
      aabbMaxValue *= scalesRealToNorm[index];
      if (x > aabbMaxValue + gAABBTol)
      {
        return false;
      }
    }
    else
    {
      // The analysis property component index
      uint32_t apIndex = index - m_numDimensions;

      // Remove the offset from the query point
      float x = queryPoint[dim] - offsetsQuantisedToNorm[index];

      // Min value
      float aabbMinValue = scalesQuantisedToNorm[index] * (float)aabbMinValsU[apIndex];
      if (x < aabbMinValue - gAABBTol)
      {
        return false;
      }

      // Max value
      float aabbMaxValue = scalesQuantisedToNorm[index] * (float)aabbMaxValsU[apIndex];
      if (x > aabbMaxValue + gAABBTol)
      {
        return false;
      }
    }
  }

  //------------------------------
  // Project the ray through all child node bounding regions, if they have faces on the edge of the sample space.
  if (isBranchNode(nodeID))
  {
    // We need to project against both child branch nodes so that the result data
    // can be updated appropriately.
    uint32_t nextDimension = getBranchPartitionDim(nodeID);
    NMP_ASSERT(nextDimension < m_numDimensions);
    uint32_t coordForDim = coordinates[nextDimension];
    uint32_t cellSizeForDim = cellSizes[nextDimension];
    uint32_t nextCellSizeRight = cellSizeForDim >> 1;
    uint32_t nextCellSizeLeft = cellSizeForDim - nextCellSizeRight; // Remainder in here

    // Left branch
    cellSizes[nextDimension] = nextCellSizeLeft;
    bool found = projectRayAlongOneDimension(
      getBranchNodeLeft(nodeID),
      resultData,
      projectionBasis);

    // Right branch
    coordinates[nextDimension] += nextCellSizeLeft;
    cellSizes[nextDimension] = nextCellSizeRight;
    found |= projectRayAlongOneDimension(
      getBranchNodeRight(nodeID),
      resultData,
      projectionBasis);

    // Roll back the old coordinate and cell size information
    coordinates[nextDimension] = coordForDim;
    cellSizes[nextDimension] = cellSizeForDim;

    return found;
  }

  //------------------------------
  // This is a leaf node so do an actual cell facet intersect test.
  bool found = false;
  for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
  {
    // There are two boundary faces per dimension, so step through them.
    for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
    {
      if (nDMesh->isNDCellBoundaryFacet(resultData->getCoordinates(), whichDim, whichBoundary))
      {
        // Compute the projected point in the Nd space
        found |= nDMesh->projectRay(
          resultData->getCoordinates(),
          whichDim,
          whichBoundary,
          resultData,
          projectionBasis);
      }
    }
  }

  return found;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPSearchMap::interpolate(
  uint32_t nodeID,
  InterpolateResultQuery* resultData) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  NMP_ASSERT(resultData);

  //------------------------------
  // Test whether the query point is inside the AABB.  If not, we can return empty-handed.
  const float* queryPoint = resultData->getQueryPoint();
  const uint32_t* queryPointComponentIndices = resultData->getQueryPointComponentIndices();
  uint32_t* coordinates = resultData->getCoordinates();
  uint32_t* cellSizes = resultData->getCellSizes();

  // Get the AABB information for the specified node
  const NDMesh* nDMesh = resultData->getNDMesh();
  const float* scalesRealToNorm = nDMesh->getScalesRealToNorm();
  const float* offsetsRealToNorm = nDMesh->getOffsetsRealToNorm();
  const float* scalesQuantisedToNorm = nDMesh->getScalesQuantisedToNorm();
  const float* offsetsQuantisedToNorm = nDMesh->getOffsetsQuantisedToNorm();
  uint32_t offset = nodeID * m_numAnalysedProperties;
  const uint16_t* aabbMinVals = &m_aabbMinVals[offset];
  const uint16_t* aabbMaxVals = &m_aabbMaxVals[offset];

  //------------------------------
  // NOTE: This is a cheep way of culling out entire branches of the APSearch tree.
  // If the query point does not lay inside the branch node AABB then it will also not
  // lay inside any of the sub-branch or leaf node AABBs.
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    // Index of the component in the vertex sample
    uint32_t index = queryPointComponentIndices[dim];
    NMP_ASSERT(index < m_numComponentsPerSample);

    if (index < m_numDimensions)
    {
      // Remove the offset from the query point
      float x = queryPoint[dim] - offsetsRealToNorm[index];

      float aabbMinValue, aabbMaxValue;
      nDMesh->getControlComponentAABB(
        index,
        coordinates[index],
        coordinates[index] + cellSizes[index],
        aabbMinValue,
        aabbMaxValue);

      // Min value
      aabbMinValue *= scalesRealToNorm[index];
      if (x < aabbMinValue - gAABBTol)
      {
        return false;
      }

      // Max value
      aabbMaxValue *= scalesRealToNorm[index];
      if (x > aabbMaxValue + gAABBTol)
      {
        return false;
      }
    }
    else
    {
      // The analysis property component index
      uint32_t apIndex = index - m_numDimensions;

      // Remove the offset from the query point
      float x = queryPoint[dim] - offsetsQuantisedToNorm[index];

      // Min value
      float aabbMinValue = scalesQuantisedToNorm[index] * (float)aabbMinVals[apIndex];
      if (x < aabbMinValue - gAABBTol)
      {
        return false;
      }

      // Max value
      float aabbMaxValue = scalesQuantisedToNorm[index] * (float)aabbMaxVals[apIndex];
      if (x > aabbMaxValue + gAABBTol)
      {
        return false;
      }
    }
  }

  //------------------------------
  // Recurse to all child node bounding regions
  if (isBranchNode(nodeID))
  {
    // Partition the cells in the specified dimension
    uint32_t nextDimension = getBranchPartitionDim(nodeID);
    NMP_ASSERT(nextDimension < m_numDimensions);
    uint32_t coordForDim = coordinates[nextDimension];
    uint32_t cellSizeForDim = cellSizes[nextDimension];
    uint32_t nextCellSizeRight = cellSizeForDim >> 1;
    uint32_t nextCellSizeLeft = cellSizeForDim - nextCellSizeRight; // Remainder in here

    // Left branch
    cellSizes[nextDimension] = nextCellSizeLeft;
    if (interpolate(
      getBranchNodeLeft(nodeID),
      resultData))
    {
      return true;
    }

    // Right branch
    coordinates[nextDimension] += nextCellSizeLeft;
    cellSizes[nextDimension] = nextCellSizeRight;
    if (interpolate(
      getBranchNodeRight(nodeID),
      resultData))
    {
      return true;
    }

    // Roll back the old coordinate and cell size information
    coordinates[nextDimension] = coordForDim;
    cellSizes[nextDimension] = cellSizeForDim;

    return false;
  }

  //------------------------------
  // This is a leaf node so do an actual blend coefficient calculation to determine containment.
  return nDMesh->interpolate(
    resultData->getCoordinates(),
    resultData);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPSearchMap::getNDCellIntersectingQueryAABBInterpolate(
  uint32_t nodeID,
  AABBIntersectionResult* resultData) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  NMP_ASSERT(resultData);

  // State data
  uint32_t* coordinates = resultData->getCoordinates();
  uint32_t* cellSizes = resultData->getCellSizes();

  const uint32_t* queryPointComponentIndices = resultData->getQueryPointComponentIndices();
  const float* queryAABBMinVals = resultData->getQueryAABBMinVals();
  const float* queryAABBMaxVals = resultData->getQueryAABBMaxVals();

  // Get the annotation AABB information for the specified node
  const NDMesh* nDMesh = resultData->getNDMesh();
  const float* scalesRealToNorm = nDMesh->getScalesRealToNorm();
  const float* offsetsRealToNorm = nDMesh->getOffsetsRealToNorm();
  const float* scalesQuantisedToNorm = nDMesh->getScalesQuantisedToNorm();
  const float* offsetsQuantisedToNorm = nDMesh->getOffsetsQuantisedToNorm();
  uint32_t offset = nodeID * m_numAnalysedProperties;
  const uint16_t* aabbMinVals = &m_aabbMinVals[offset];
  const uint16_t* aabbMaxVals = &m_aabbMaxVals[offset];

  //------------------------------
  // NOTE: This is a cheep way of culling out entire branches of the APSearch tree.
  // If the query AABB does not intersect the branch node AABB then it will also not
  // intersect any of the sub-branch or leaf node AABBs.
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    // Index of the component in the vertex sample
    uint32_t index = queryPointComponentIndices[dim];
    NMP_ASSERT(index < m_numComponentsPerSample);

    if (index < m_numDimensions)
    {
      float aabbMinValue, aabbMaxValue;
      nDMesh->getControlComponentAABB(
        index,
        coordinates[index],
        coordinates[index] + cellSizes[index],
        aabbMinValue,
        aabbMaxValue);

      // Min value
      aabbMinValue = scalesRealToNorm[index] * aabbMinValue + offsetsRealToNorm[index];
      if (queryAABBMaxVals[dim] < aabbMinValue - gAABBTol)
      {
        return;
      }

      // Max value
      aabbMaxValue = scalesRealToNorm[index] * aabbMaxValue + offsetsRealToNorm[index];
      if (queryAABBMinVals[dim] > aabbMaxValue + gAABBTol)
      {
        return;
      }
    }
    else
    {
      // The analysis property component index
      uint32_t apIndex = index - m_numDimensions;

      // Min value
      float aabbMinValue = scalesQuantisedToNorm[index] * (float)aabbMinVals[apIndex] + offsetsQuantisedToNorm[index];
      if (queryAABBMaxVals[dim] < aabbMinValue - gAABBTol)
      {
        return;
      }

      // Max value
      float aabbMaxValue = scalesQuantisedToNorm[index] * (float)aabbMaxVals[apIndex] + offsetsQuantisedToNorm[index];
      if (queryAABBMinVals[dim] > aabbMaxValue + gAABBTol)
      {
        return;
      }
    }
  }

  //------------------------------
  // Recurse to all child node bounding regions
  if (isBranchNode(nodeID))
  {
    // Partition the cells in the specified dimension
    uint32_t nextDimension = getBranchPartitionDim(nodeID);
    NMP_ASSERT(nextDimension < m_numDimensions);
    uint32_t coordForDim = coordinates[nextDimension];
    uint32_t cellSizeForDim = cellSizes[nextDimension];
    uint32_t nextCellSizeRight = cellSizeForDim >> 1;
    uint32_t nextCellSizeLeft = cellSizeForDim - nextCellSizeRight; // Remainder in here

    // Left branch
    cellSizes[nextDimension] = nextCellSizeLeft;
    getNDCellIntersectingQueryAABBInterpolate(
      getBranchNodeLeft(nodeID),
      resultData);

    // Right branch
    coordinates[nextDimension] += nextCellSizeLeft;
    cellSizes[nextDimension] = nextCellSizeRight;
    getNDCellIntersectingQueryAABBInterpolate(
      getBranchNodeRight(nodeID),
      resultData);

    // Roll back the old coordinate and cell size information
    coordinates[nextDimension] = coordForDim;
    cellSizes[nextDimension] = cellSizeForDim;
    return;
  }

  //------------------------------
  // This is a leaf node so we must now test if the annotation cell intersects with the query AABB
  if (resultData->queryAABBIntersectsNDCell(coordinates))
  {
    resultData->addEntry(nodeID, coordinates);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPSearchMap::getNDCellFacetIntersectingQueryAABBProjectAlongOneDimension(
  uint32_t nodeID,
  AABBProjectionResult* resultData) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  NMP_ASSERT(resultData);

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  // Projection against 1D Meshes is handled separately as a special case.
  NMP_ASSERT(m_numDimensions > 1);

  // Check if any of the AABB facets touch the external boundary
  if (!isEdgeElement(nodeID))
    return;

  uint32_t M = m_numDimensions - 1;
  uint32_t* coordinates = resultData->getCoordinates();
  uint32_t* cellSizes = resultData->getCellSizes();

  const uint32_t* queryPointComponentIndices = resultData->getQueryPointComponentIndices();
  const float* queryAABBMinVals = resultData->getQueryAABBMinVals(); // Nd (The compromised dim should be the same as the max vals)
  const float* queryAABBMaxVals = resultData->getQueryAABBMaxVals(); // Nd (The compromised dim should be the same as the min vals)

  // Get the AABB information for the specified node
  const NDMesh* nDMesh = resultData->getNDMesh();
  const float* scalesRealToNorm = nDMesh->getScalesRealToNorm();
  const float* offsetsRealToNorm = nDMesh->getOffsetsRealToNorm();
  const float* scalesQuantisedToNorm = nDMesh->getScalesQuantisedToNorm();
  const float* offsetsQuantisedToNorm = nDMesh->getOffsetsQuantisedToNorm();
  uint32_t offset = nodeID * m_numAnalysedProperties;
  const uint16_t* aabbMinVals = &m_aabbMinVals[offset];
  const uint16_t* aabbMaxVals = &m_aabbMaxVals[offset];

  //------------------------------
  // Check if the query AABB for the projection cell intersects the NDCell AABB.
  // NOTE: This is a cheep way of culling out entire branches of the APSearch tree
  // if the branch node AABB lies outside of the query AABB.
  const uint32_t* tmP = resultData->getPermutationOrder();
  NMP_ASSERT(tmP);

  for (uint32_t basisIndex = 0; basisIndex < M; ++basisIndex)
  {
    uint32_t dim = tmP[basisIndex];
    NMP_ASSERT(dim < m_numDimensions);

    // Index of the component in the vertex sample
    uint32_t index = queryPointComponentIndices[dim];
    NMP_ASSERT(index < m_numComponentsPerSample);

    if (index < m_numDimensions)
    {
      float aabbMinValue, aabbMaxValue;
      nDMesh->getControlComponentAABB(
        index,
        coordinates[index],
        coordinates[index] + cellSizes[index],
        aabbMinValue,
        aabbMaxValue);

      // Min value
      aabbMinValue = scalesRealToNorm[index] * aabbMinValue + offsetsRealToNorm[index];
      if (queryAABBMaxVals[dim] < aabbMinValue - gAABBTol)
      {
        return;
      }

      // Max value
      aabbMaxValue = scalesRealToNorm[index] * aabbMaxValue + offsetsRealToNorm[index];
      if (queryAABBMinVals[dim] > aabbMaxValue + gAABBTol)
      {
        return;
      }
    }
    else
    {
      // The analysis property component index
      uint32_t apIndex = index - m_numDimensions;

      // Min value
      float aabbMinValue = scalesQuantisedToNorm[index] * (float)aabbMinVals[apIndex] + offsetsQuantisedToNorm[index];
      if (queryAABBMaxVals[dim] < aabbMinValue - gAABBTol)
      {
        return;
      }

      // Max value
      float aabbMaxValue = scalesQuantisedToNorm[index] * (float)aabbMaxVals[apIndex] + offsetsQuantisedToNorm[index];
      if (queryAABBMinVals[dim] > aabbMaxValue + gAABBTol)
      {
        return;
      }
    }
  }

  //------------------------------
  // Project the ray through all child node bounding regions, if they have faces on the edge of the sample space.
  if (isBranchNode(nodeID))
  {
    // We need to project against both child branch nodes so that the result data
    // can be updated appropriately.
    uint32_t nextDimension = getBranchPartitionDim(nodeID);
    NMP_ASSERT(nextDimension < m_numDimensions);
    uint32_t coordForDim = coordinates[nextDimension];
    uint32_t cellSizeForDim = cellSizes[nextDimension];
    uint32_t nextCellSizeRight = cellSizeForDim >> 1;
    uint32_t nextCellSizeLeft = cellSizeForDim - nextCellSizeRight; // Remainder in here

    // Left branch
    cellSizes[nextDimension] = nextCellSizeLeft;
    getNDCellFacetIntersectingQueryAABBProjectAlongOneDimension(
      getBranchNodeLeft(nodeID),
      resultData);

    // Right branch
    coordinates[nextDimension] += nextCellSizeLeft;
    cellSizes[nextDimension] = nextCellSizeRight;
    getNDCellFacetIntersectingQueryAABBProjectAlongOneDimension(
      getBranchNodeRight(nodeID),
      resultData);

    // Roll back the old coordinate and cell size information
    coordinates[nextDimension] = coordForDim;
    cellSizes[nextDimension] = cellSizeForDim;

    return;
  }

  //------------------------------
  // This is a leaf node so do an actual cell facet intersect test.
  uint32_t whichBoundaryDimFlags[2] = {0, 0};
  for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
  {
    uint32_t dimFlag = 1 << whichDim;

    for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
    {
      if (nDMesh->isNDCellBoundaryFacet(coordinates, whichDim, whichBoundary))
      {
        if (resultData->queryAABBIntersectsNDCellFacet(
          coordinates,
          whichDim,
          whichBoundary))
        {
          whichBoundaryDimFlags[whichBoundary] |= dimFlag;
        }
      }
    }
  }

  // Add a new entry to the result data
  uint32_t whichDimFlags = whichBoundaryDimFlags[0] | whichBoundaryDimFlags[1];
  if (whichDimFlags)
  {
    // Check if both boundaries of the cell are set (this occurs if there is only a single cell
    // in one of the sample dimensions).
    if (whichBoundaryDimFlags[0] & whichBoundaryDimFlags[1])
    {
      // Add two entries for the cell (one for each boundary)
      resultData->addEntry(
        nodeID,
        coordinates,
        whichBoundaryDimFlags[0],
        0);
      resultData->addEntry(
        nodeID,
        coordinates,
        whichBoundaryDimFlags[1],
        whichBoundaryDimFlags[1]);
    }
    else
    {
      // The boundaries are mutually exclusive so combine into a single entry
      resultData->addEntry(
        nodeID,
        coordinates,
        whichDimFlags,
        whichBoundaryDimFlags[1]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPSearchMap::getNDCellFacetIntersectingQueryAABBProjectThroughPoint(
  uint32_t nodeID,
  AABBProjectionResult* resultData) const
{
  NMP_ASSERT(nodeID < getNumNodes());
  NMP_ASSERT(resultData);

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  // Projection against 1D Meshes is handled separately as a special case.
  NMP_ASSERT(m_numDimensions > 1);

  // Check if any of the AABB facets touch the external boundary
  if (!isEdgeElement(nodeID))
    return; 

  // NOTE: We can't perform a cheep rejection test because we are performing central projection

  //------------------------------
  // Project the ray through all child node bounding regions, if they have faces on the edge of the sample space.
  uint32_t* coordinates = resultData->getCoordinates();
  uint32_t* cellSizes = resultData->getCellSizes();

  if (isBranchNode(nodeID))
  {
    // We need to project against both child branch nodes so that the result data
    // can be updated appropriately.
    uint32_t nextDimension = getBranchPartitionDim(nodeID);
    NMP_ASSERT(nextDimension < m_numDimensions);
    uint32_t coordForDim = coordinates[nextDimension];
    uint32_t cellSizeForDim = cellSizes[nextDimension];
    uint32_t nextCellSizeRight = cellSizeForDim >> 1;
    uint32_t nextCellSizeLeft = cellSizeForDim - nextCellSizeRight; // Remainder in here

    // Left branch
    cellSizes[nextDimension] = nextCellSizeLeft;
    getNDCellFacetIntersectingQueryAABBProjectThroughPoint(
      getBranchNodeLeft(nodeID),
      resultData);

    // Right branch
    coordinates[nextDimension] += nextCellSizeLeft;
    cellSizes[nextDimension] = nextCellSizeRight;
    getNDCellFacetIntersectingQueryAABBProjectThroughPoint(
      getBranchNodeRight(nodeID),
      resultData);

    // Roll back the old coordinate and cell size information
    coordinates[nextDimension] = coordForDim;
    cellSizes[nextDimension] = cellSizeForDim;

    return;
  }

  //------------------------------
  // This is a leaf node so do an actual cell facet intersect test.
  const NDMesh* nDMesh = resultData->getNDMesh();

  uint32_t whichBoundaryDimFlags[2] = {0, 0};
  for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
  {
    uint32_t dimFlag = 1 << whichDim;

    for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
    {
      if (nDMesh->isNDCellBoundaryFacet(coordinates, whichDim, whichBoundary))
      {
        if (resultData->queryAABBIntersectsNDCellFacet(
          coordinates,
          whichDim,
          whichBoundary))
        {
          whichBoundaryDimFlags[whichBoundary] |= dimFlag;
        }
      }
    }
  }

  // Add a new entry to the result data
  uint32_t whichDimFlags = whichBoundaryDimFlags[0] | whichBoundaryDimFlags[1];
  if (whichDimFlags)
  {
    // Check if both boundaries of the cell are set (this occurs if there is only a single cell
    // in one of the sample dimensions).
    if (whichBoundaryDimFlags[0] & whichBoundaryDimFlags[1])
    {
      // Add two entries for the cell (one for each boundary)
      resultData->addEntry(
        nodeID,
        coordinates,
        whichBoundaryDimFlags[0],
        0);
      resultData->addEntry(
        nodeID,
        coordinates,
        whichBoundaryDimFlags[1],
        whichBoundaryDimFlags[1]);
    }
    else
    {
      // The boundaries are mutually exclusive so combine into a single entry
      resultData->addEntry(
        nodeID,
        coordinates,
        whichDimFlags,
        whichBoundaryDimFlags[1]);
    }
  }
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
