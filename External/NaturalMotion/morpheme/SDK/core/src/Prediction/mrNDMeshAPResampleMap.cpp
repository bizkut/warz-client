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
#include "morpheme/Prediction/mrScatteredDataUtils.h"
#include "morpheme/Prediction/mrNDMeshAPResampleMap.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace ScatteredData
{

//----------------------------------------------------------------------------------------------------------------------
static const uint32_t gMaxNumCellEntries = 128;

//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPResampleMapProjectionData
//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPResampleMapProjectionData::dislocate(NDMeshAPResampleMapProjectionData* target)
{
  NMP_ASSERT(target);
  PackedArrayUInt32::dislocate(target->m_cellEntries);
  UNFIX_SWAP_PTR_RELATIVE(PackedArrayUInt32, target->m_cellEntries, target);
  PackedArrayUInt32::dislocate(target->m_cellEntryOffsets);
  UNFIX_SWAP_PTR_RELATIVE(PackedArrayUInt32, target->m_cellEntryOffsets, target);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPResampleMapProjectionData::locate(NDMeshAPResampleMapProjectionData* target)
{
  NMP_ASSERT(target);
  REFIX_SWAP_PTR_RELATIVE(PackedArrayUInt32, target->m_cellEntryOffsets, target);
  PackedArrayUInt32::locate(target->m_cellEntryOffsets);
  REFIX_SWAP_PTR_RELATIVE(PackedArrayUInt32, target->m_cellEntries, target);
  PackedArrayUInt32::locate(target->m_cellEntries);
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshAPResampleMapProjectionData* NDMeshAPResampleMapProjectionData::relocate(void*& ptr)
{
  NMP_ASSERT(ptr);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshAPResampleMapProjectionData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshAPResampleMapProjectionData* result = (NDMeshAPResampleMapProjectionData*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Data
  result->m_cellEntryOffsets = PackedArrayUInt32::relocate(ptr);
  result->m_cellEntries = PackedArrayUInt32::relocate(ptr);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPResampleMap
//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPResampleMap::dislocate(NDMeshAPResampleMap* target)
{
  NMP_ASSERT(target);
  uint32_t numDimensions = target->m_numDimensions;
  NMP_ASSERT(numDimensions > 0);
  uint32_t M = numDimensions - 1;
  NMP_ASSERT(target->m_numAPQueryComplementComponentIndices > 0 && target->m_numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);

  //------------------------------
  // Projection along dimension query resample maps
  if (M > 0)
  {
    //------------------------------
    // Projection through point resample map data
    if (target->m_projectionThroughPointEnable)
    {
      for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
      {
        NDMeshAPResampleMapProjectionData::dislocate(target->m_projectionThroughPointData[1][whichDim]);
        UNFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData, target->m_projectionThroughPointData[1][whichDim], target);
        NDMeshAPResampleMapProjectionData::dislocate(target->m_projectionThroughPointData[0][whichDim]);
        UNFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData, target->m_projectionThroughPointData[0][whichDim], target);
      }

      UNFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData*, target->m_projectionThroughPointData[1], target);
      UNFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData*, target->m_projectionThroughPointData[0], target);
    }

    //------------------------------
    // Projection along dimension resample map data
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      NDMeshAPResampleMapProjectionData::dislocate(target->m_projectionAlongDimensionData[whichDim]);
      UNFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData, target->m_projectionAlongDimensionData[whichDim], target);
    }
    UNFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData*, target->m_projectionAlongDimensionData, target);

    //------------------------------
    // Projection grid data    
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      NMP::endianSwapArray(target->m_projectionWhichQueryComponentIndices[whichDim], M);
      UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_projectionWhichQueryComponentIndices[whichDim], target);

      NMP::endianSwapArray(target->m_projectionCellBlockSizes[whichDim], M + 1);
      UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_projectionCellBlockSizes[whichDim], target);
    }
    UNFIX_SWAP_PTR_RELATIVE(uint32_t*, target->m_projectionWhichQueryComponentIndices, target);
    UNFIX_SWAP_PTR_RELATIVE(uint32_t*, target->m_projectionCellBlockSizes, target);
  }
  NMP::endianSwap(target->m_projectionThroughPointEnable);

  //------------------------------
  // Interpolation resample map data 
  target->m_interpolationCellFlags->dislocate();
  UNFIX_SWAP_PTR_RELATIVE(NMP::BitArray, target->m_interpolationCellFlags, target);

  PackedArrayUInt32::dislocate(target->m_interpolationVertexCPSamples);
  UNFIX_SWAP_PTR_RELATIVE(PackedArrayUInt32, target->m_interpolationVertexCPSamples, target);

  NMP::endianSwapArray(target->m_interpolationVertexCPOffsets, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_interpolationVertexCPOffsets, target);

  NMP::endianSwapArray(target->m_interpolationVertexCPScales, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_interpolationVertexCPScales, target);

  NMP::endianSwapArray(target->m_interpolationCellBlockSizes, numDimensions + 1);
  UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_interpolationCellBlockSizes, target);

  NMP::endianSwapArray(target->m_interpolationVertexBlockSizes, numDimensions + 1);
  UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_interpolationVertexBlockSizes, target);

  //------------------------------
  // Header
  NMP::endianSwapArray(target->m_apQueryComponentIndices, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_apQueryComponentIndices, target);

  NMP::endianSwapArray(target->m_apQueryComplementComponentIndices, target->m_numAPQueryComplementComponentIndices);
  UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_apQueryComplementComponentIndices, target);

  NMP::endianSwapArray(target->m_sampleCountsPerDimension, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_sampleCountsPerDimension, target);

  NMP::endianSwapArray(target->m_apGridStartValues, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_apGridStartValues, target);

  NMP::endianSwapArray(target->m_apGridEndValues, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_apGridEndValues, target);

  NMP::endianSwapArray(target->m_apGridSampleScales, numDimensions);
  UNFIX_SWAP_PTR_RELATIVE(float, target->m_apGridSampleScales, target);

  NMP::endianSwap(target->m_numAPQueryComplementComponentIndices);
  NMP::endianSwap(target->m_numDimensions);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAPResampleMap::locate(NDMeshAPResampleMap* target)
{
  NMP_ASSERT(target);

  //------------------------------
  // Header
  NMP::endianSwap(target->m_numDimensions);
  uint32_t numDimensions = target->m_numDimensions;
  NMP_ASSERT(numDimensions > 0);
  uint32_t M = numDimensions - 1;
  NMP::endianSwap(target->m_numAPQueryComplementComponentIndices);
  NMP_ASSERT(target->m_numAPQueryComplementComponentIndices > 0 && target->m_numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);

  REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_apQueryComponentIndices, target);
  NMP::endianSwapArray(target->m_apQueryComponentIndices, numDimensions);

  REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_apQueryComplementComponentIndices, target);
  NMP::endianSwapArray(target->m_apQueryComplementComponentIndices, target->m_numAPQueryComplementComponentIndices);

  REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_sampleCountsPerDimension, target);
  NMP::endianSwapArray(target->m_sampleCountsPerDimension, numDimensions);

  REFIX_SWAP_PTR_RELATIVE(float, target->m_apGridStartValues, target);
  NMP::endianSwapArray(target->m_apGridStartValues, numDimensions);

  REFIX_SWAP_PTR_RELATIVE(float, target->m_apGridEndValues, target);
  NMP::endianSwapArray(target->m_apGridEndValues, numDimensions);

  REFIX_SWAP_PTR_RELATIVE(float, target->m_apGridSampleScales, target);
  NMP::endianSwapArray(target->m_apGridSampleScales, numDimensions);

  //------------------------------
  // Interpolation resample map data
  REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_interpolationVertexBlockSizes, target);
  NMP::endianSwapArray(target->m_interpolationVertexBlockSizes, numDimensions + 1);

  REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_interpolationCellBlockSizes, target);
  NMP::endianSwapArray(target->m_interpolationCellBlockSizes, numDimensions + 1);  

  REFIX_SWAP_PTR_RELATIVE(float, target->m_interpolationVertexCPScales, target);
  NMP::endianSwapArray(target->m_interpolationVertexCPScales, numDimensions);

  REFIX_SWAP_PTR_RELATIVE(float, target->m_interpolationVertexCPOffsets, target);
  NMP::endianSwapArray(target->m_interpolationVertexCPOffsets, numDimensions);

  REFIX_SWAP_PTR_RELATIVE(PackedArrayUInt32, target->m_interpolationVertexCPSamples, target);
  PackedArrayUInt32::locate(target->m_interpolationVertexCPSamples);

  REFIX_SWAP_PTR_RELATIVE(NMP::BitArray, target->m_interpolationCellFlags, target);
  target->m_interpolationCellFlags->locate();

  //------------------------------
  // Projection resample maps
  NMP::endianSwap(target->m_projectionThroughPointEnable);
  if (M > 0)
  {
    //------------------------------
    // Projection grid data
    REFIX_SWAP_PTR_RELATIVE(uint32_t*, target->m_projectionWhichQueryComponentIndices, target);
    REFIX_SWAP_PTR_RELATIVE(uint32_t*, target->m_projectionCellBlockSizes, target);
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_projectionWhichQueryComponentIndices[whichDim], target);
      NMP::endianSwapArray(target->m_projectionWhichQueryComponentIndices[whichDim], M);

      REFIX_SWAP_PTR_RELATIVE(uint32_t, target->m_projectionCellBlockSizes[whichDim], target);
      NMP::endianSwapArray(target->m_projectionCellBlockSizes[whichDim], M + 1);
    }

    //------------------------------
    // Projection along dimension resample map data
    REFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData*, target->m_projectionAlongDimensionData, target);
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      REFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData, target->m_projectionAlongDimensionData[whichDim], target);
      NDMeshAPResampleMapProjectionData::locate(target->m_projectionAlongDimensionData[whichDim]);
    }

    //------------------------------
    // Projection through point resample map data
    if (target->m_projectionThroughPointEnable)
    {
      REFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData*, target->m_projectionThroughPointData[0], target);
      REFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData*, target->m_projectionThroughPointData[1], target);

      for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
      {
        REFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData, target->m_projectionThroughPointData[0][whichDim], target);
        NDMeshAPResampleMapProjectionData::locate(target->m_projectionThroughPointData[0][whichDim]);
        REFIX_SWAP_PTR_RELATIVE(NDMeshAPResampleMapProjectionData, target->m_projectionThroughPointData[1][whichDim], target);
        NDMeshAPResampleMapProjectionData::locate(target->m_projectionThroughPointData[1][whichDim]);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshAPResampleMap* NDMeshAPResampleMap::relocate(void*& ptr)
{
  NMP_ASSERT(ptr);

  //------------------------------
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshAPResampleMap), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshAPResampleMap* result = (NDMeshAPResampleMap*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  uint32_t numDimensions = result->m_numDimensions;
  NMP_ASSERT(numDimensions > 0);
  uint32_t M = numDimensions - 1;
  NMP_ASSERT(result->m_numAPQueryComplementComponentIndices > 0 && result->m_numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);

  // AP Query components
  NMP::Memory::Format memReqsSampleCount(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apQueryComponentIndices = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSampleCount);
  NMP_ASSERT(result->m_numAPQueryComplementComponentIndices > 0);
  NMP::Memory::Format memReqsAPComplement(sizeof(uint32_t) * result->m_numAPQueryComplementComponentIndices, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apQueryComplementComponentIndices = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsAPComplement);

  // Sample counts
  result->m_sampleCountsPerDimension = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSampleCount);

  // Grid range information
  NMP::Memory::Format memReqsGridValues(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apGridStartValues = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsGridValues);
  result->m_apGridEndValues = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsGridValues);
  result->m_apGridSampleScales = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsGridValues);

  //------------------------------
  // Interpolation resample map data
  NMP::Memory::Format memReqsInterpBlockSizes(sizeof(uint32_t) * (numDimensions + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_interpolationVertexBlockSizes = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsInterpBlockSizes);
  result->m_interpolationCellBlockSizes = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsInterpBlockSizes);

  NMP::Memory::Format memReqsInterpQData(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_interpolationVertexCPScales = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsInterpQData);
  result->m_interpolationVertexCPOffsets = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsInterpQData);

  result->m_interpolationVertexCPSamples = PackedArrayUInt32::relocate(ptr);
  result->m_interpolationCellFlags = NMP::BitArray::relocate(ptr);

  //------------------------------
  // Projection resample maps
  if (M > 0)
  {
    //------------------------------
    // Projection grid data
    NMP::Memory::Format memReqsPrjTable(sizeof(void*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_projectionWhichQueryComponentIndices = (uint32_t**)NMP::Memory::alignAndIncrement(ptr, memReqsPrjTable);
    result->m_projectionCellBlockSizes = (uint32_t**)NMP::Memory::alignAndIncrement(ptr, memReqsPrjTable);

    NMP::Memory::Format memReqsPrjQueryIndices(sizeof(uint32_t) * M, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsPrjBlockSizes(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      result->m_projectionWhichQueryComponentIndices[whichDim] = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsPrjQueryIndices);
      result->m_projectionCellBlockSizes[whichDim] = (uint32_t*)NMP::Memory::alignAndIncrement(ptr, memReqsPrjBlockSizes);
    }

    //------------------------------
    // Projection along dimension resample map data
    result->m_projectionAlongDimensionData = (NDMeshAPResampleMapProjectionData**)NMP::Memory::alignAndIncrement(ptr, memReqsPrjTable);
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      result->m_projectionAlongDimensionData[whichDim] = NDMeshAPResampleMapProjectionData::relocate(ptr);
    }

    //------------------------------
    // Projection through point resample map data
    if (result->m_projectionThroughPointEnable)
    {
      // Pointers table
      result->m_projectionThroughPointData[0] = (NDMeshAPResampleMapProjectionData**)NMP::Memory::alignAndIncrement(ptr, memReqsPrjTable);
      result->m_projectionThroughPointData[1] = (NDMeshAPResampleMapProjectionData**)NMP::Memory::alignAndIncrement(ptr, memReqsPrjTable);

      for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
      {
        result->m_projectionThroughPointData[0][whichDim] = NDMeshAPResampleMapProjectionData::relocate(ptr);
        result->m_projectionThroughPointData[1][whichDim] = NDMeshAPResampleMapProjectionData::relocate(ptr);
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPResampleMap::isValidQuery(
  const uint32_t* apQueryComponentIndices,
  const ScatteredData::Projection* projectionParams) const
{
  // Check that the AP query components are the same
  NMP_ASSERT(apQueryComponentIndices);
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    if (apQueryComponentIndices[i] != m_apQueryComponentIndices[i])
    {
      return false;
    }
  }

  // Check if the projection options are supported
  if (projectionParams)
  {
    // Note: Projection along the principal directions is always supported
    Projection::Method method = projectionParams->getMethod();
    if (method == Projection::ProjectThroughSampleCentre)
    {
      return m_projectionThroughPointEnable;
    }
    else if (method == Projection::ProjectThroughPoint)
    {
      // Resample maps do not support projection queries through an arbitrary point
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
InterpolationReturnVal NDMeshAPResampleMap::mapFromAPs(
  const NDMesh*       nDMesh,
  float*              queryPoint,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents,
  bool                enableProjection,
  const Projection*   projectionParams) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
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
          nDMesh,                         // IN
          queryPoint,                     // IN: Query AP components
          i,                              // IN: This an index into the queryPointComponentIndices
          ndMeshQueryResult,              // OUT: The NDCell facet projection information
          numOutputSampleComponents,      // IN
          outputSampleComponentIndices,   // IN
          outputSampleComponents);        // OUT: Complementary sample components

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
          nDMesh,                         // IN
          queryPoint,                     // IN: Query AP components
          whichVariableToCompromise,      // IN: This an index into the queryPointComponentIndices
          ndMeshQueryResult,              // OUT: The NDCell facet projection information
          numOutputSampleComponents,      // IN
          outputSampleComponentIndices,   // IN
          outputSampleComponents);        // OUT: Complementary sample components

        if (found)
        {
          result = INTERPOLATION_PROJECTED;
          break;
        }
      }
    }
    else if (projectionParams->getMethod() == Projection::ProjectThroughSampleCentre)
    {
      NMP_ASSERT(projectionParams->getNumComponents() == m_numDimensions);     
      found = projectThroughPoint(
        nDMesh,                         // IN
        queryPoint,                     // IN: Query AP components
        ndMeshQueryResult,              // OUT: The NDCell facet projection information
        numOutputSampleComponents,      // IN
        outputSampleComponentIndices,   // IN
        outputSampleComponents);        // OUT: Complementary sample components

      if (found)
      {
        result = INTERPOLATION_PROJECTED;
      }
    }
    else
    {
      NMP_ASSERT_FAIL_MSG("Unsupported projection mode");
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPResampleMap::getRangeInDimension(
  const NDMesh*       nDMesh,
  const float*        queryPoint,
  uint32_t            whichQueryVariable,
  float&              minimum,
  float&              maximum,
  NDMeshQueryResult*  ndMeshQueryResultRear,
  NDMeshQueryResult*  ndMeshQueryResultFore) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(whichQueryVariable < m_numDimensions);

  PREDICTION_PROFILE_BEGIN_CONTEXT("getRangeInDimension");

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  if (m_numDimensions == 1)
  {
    NMP_ASSERT(whichQueryVariable == 0);
    nDMesh->projection1DExtremes(
      m_apQueryComponentIndices,
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

  //------------------------------
  // Find the projection cell coordinates within the projection map
  uint32_t apTopLeftCoord[SCATTERED_DATA_MAX_DIM];
  bool statusAABB = projectAlongDimCellCoordinates(
    queryPoint,           // IN (function permutes Nd -> N-1d)
    whichQueryVariable,   // IN: the dimension to compromise for projection (hyper plane selection)
    apTopLeftCoord);      // OUT: (N-1)d The coordinate within the resample grid hyper plane cell

  if (!statusAABB)
  {
    return false;
  }

  // Unpack the projection cell entries
  uint32_t cpCellAddresses[gMaxNumCellEntries];
  uint32_t whichDimFlags[gMaxNumCellEntries];
  uint32_t whichBoundaryFlags[gMaxNumCellEntries];
  uint32_t numEntries = getProjectionAlongDimensionCellEntries(
    whichQueryVariable,
    apTopLeftCoord,
    gMaxNumCellEntries,
    cpCellAddresses,
    whichDimFlags,
    whichBoundaryFlags);

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    m_apQueryComponentIndices,
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
    m_apQueryComponentIndices,
    ndMeshQueryResultRear,
    ndMeshQueryResultFore);

  for (uint32_t index = 0; index < numEntries; ++index)
  {
    // Convert the CP cell address to a coordinate
    uint32_t cpTopLeftCoord[SCATTERED_DATA_MAX_DIM];
    nDMesh->coordinateFromAddress(cpTopLeftCoord, cpCellAddresses[index]);

    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      if ((whichDimFlags[index] >> whichDim) & 0x01)
      {
        uint32_t whichBoundary = (whichBoundaryFlags[index] >> whichDim) & 0x01;
        NMP_ASSERT(nDMesh->isNDCellBoundaryFacet(cpTopLeftCoord, whichDim, whichBoundary));

        // Compute the projected point in the Nd space
        nDMesh->projectRay(
          cpTopLeftCoord,
          whichDim,
          whichBoundary,
          &projectionResult,
          &projectionBasis);
      }
    }
  }

  //------------------------------
  // Write the range query data
  if (projectionResult.getCount() > 0)
  {
    uint32_t M = m_numDimensions - 1;
    const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(M);
    uint32_t index = m_apQueryComponentIndices[whichQueryVariable];

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
void NDMeshAPResampleMap::getInterpolationCellVertexComponents(
  const uint32_t* topLeftCoord,
  float* const* vertexComponents) const
{
  NMP_ASSERT(topLeftCoord);
  NMP_ASSERT(vertexComponents);

  // Recover the vertex samples for the resample grid cell
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
      vertexSampleIndex += (topLeftCoord[dim] + offset) * m_interpolationVertexBlockSizes[dim];
    }

    // Get the vertex sample quantised data
    NMP_ASSERT(vertexSampleIndex < m_interpolationVertexBlockSizes[m_numDimensions]);    
    uint32_t offset = vertexSampleIndex * m_numDimensions;

    // Dequantise the vertex sample data
    float* vertexData = vertexComponents[vertex];
    NMP_ASSERT(vertexData);
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      uint32_t entry = m_interpolationVertexCPSamples->getEntry(offset + i);
      vertexData[i] = m_interpolationVertexCPScales[i] * (float)entry + m_interpolationVertexCPOffsets[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPResampleMap::interpolate(
  const NDMesh*       nDMesh,
  const float*        queryPoint,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(ndMeshQueryResult);
  NMP_ASSERT(numOutputSampleComponents > 0);
  NMP_ASSERT(outputSampleComponentIndices);
  NMP_ASSERT(outputSampleComponents);

  PREDICTION_PROFILE_BEGIN_CONTEXT("interpolate");
  ndMeshQueryResult->init();

  //------------------------------
  // Find the resample cell coordinates and multi-linear weights
  uint32_t apTopLeftCoord[SCATTERED_DATA_MAX_DIM];
  float apInterpolants[SCATTERED_DATA_MAX_DIM];

  if (!interpolateMultilinearWeights(
    queryPoint,       // IN
    apTopLeftCoord,   // OUT
    apInterpolants))  // OUT
  {
    return false;
  }

  //------------------------------
  // Convert the grid cell coordinate into an address
  uint32_t cellSampleIndex = 0;
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    NMP_ASSERT(apTopLeftCoord[dim] < m_sampleCountsPerDimension[dim] - 1);
    cellSampleIndex += apTopLeftCoord[dim] * m_interpolationCellBlockSizes[dim];
  }

  // Check if the interpolation grid cell has valid data
  if (!m_interpolationCellFlags->isBitSet(cellSampleIndex))
  {
    return false;
  }

  //------------------------------
  // Interpolate the interpolation cell CP samples based on the multi-linear weights
  const ScatteredDataManager::DataBlock& managerDataBlock = ScatteredDataManager::getDataBlock(m_numDimensions);

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    m_apQueryComponentIndices,
    queryPoint,
    queryPointNorm);

  InterpolateResultQuery interpolateResult;
  interpolateResult.init(
    nDMesh,
    m_apQueryComponentIndices,
    queryPointNorm,
    ndMeshQueryResult);

  // Initialise the local search state data
  float cpComponents[SCATTERED_DATA_MAX_DIM];
  uint32_t cpTopLeftCoord[SCATTERED_DATA_MAX_DIM];
  float cpInterpolants[SCATTERED_DATA_MAX_DIM];

  // Recover the CP vertex samples for the resample grid cell
  getInterpolationCellVertexComponents(
    apTopLeftCoord,
    interpolateResult.getVertexComponents());

  // Interpolate the CP vertex samples to determine a suitable initial estimate for the guided search
  managerDataBlock.m_multilinearInterpFn(
    m_numDimensions,  // IN
    m_numDimensions,  // IN: Num components per sample
    cpComponents,     // OUT
    apInterpolants,   // IN
    interpolateResult.getVertexComponents()); // IN

  //------------------------------
  // Find the NDMesh cell coordinates and multi-linear weights for the result CP value
  nDMesh->multilinearWeights(
    cpComponents,     // IN / OUT
    cpTopLeftCoord,   // OUT
    cpInterpolants);  // OUT

  if (nDMesh->interpolateUsingGuidedSearch(
    cpTopLeftCoord,   // IN
    cpInterpolants,   // IN
    &interpolateResult))  // IN / OUT (local search state data)
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
bool NDMeshAPResampleMap::projectAlongOneDimension(
  const NDMesh*       nDMesh,
  float*              queryPoint,
  uint32_t            whichVariableToCompromise,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(whichVariableToCompromise < m_numDimensions);
  NMP_ASSERT(ndMeshQueryResult);
  NMP_ASSERT(numOutputSampleComponents > 0);
  NMP_ASSERT(outputSampleComponentIndices);
  NMP_ASSERT(outputSampleComponents);

  PREDICTION_PROFILE_BEGIN_CONTEXT("projectAlongOneDimension");

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  if (m_numDimensions == 1)
  {
    NMP_ASSERT(whichVariableToCompromise == 0);
    nDMesh->projection1DClosest(
      queryPoint,
      m_apQueryComponentIndices,
      ndMeshQueryResult,
      numOutputSampleComponents,
      outputSampleComponentIndices,
      outputSampleComponents);

    return true;
  }

  // Initialisation
  ndMeshQueryResult->init();

  //------------------------------
  // Find the projection cell coordinates within the projection map
  uint32_t apTopLeftCoord[SCATTERED_DATA_MAX_DIM];
  if (!projectAlongDimCellCoordinates(
    queryPoint,                 // IN (function permutes Nd -> N-1d)
    whichVariableToCompromise,  // IN: the dimension to compromise for projection (hyper plane selection)
    apTopLeftCoord))            // OUT: (N-1)d The coordinate within the resample grid hyper plane cell
  {
    return false;
  }

  // Unpack the projection cell entries
  uint32_t cpCellAddresses[gMaxNumCellEntries];
  uint32_t whichDimFlags[gMaxNumCellEntries];
  uint32_t whichBoundaryFlags[gMaxNumCellEntries];
  uint32_t numEntries = getProjectionAlongDimensionCellEntries(
    whichVariableToCompromise,
    apTopLeftCoord,
    gMaxNumCellEntries,
    cpCellAddresses,
    whichDimFlags,
    whichBoundaryFlags);

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    m_apQueryComponentIndices,
    queryPoint,
    queryPointNorm);

  // Initialisation
  ProjectionBasis projectionBasis;
  projectionBasis.initAlongDimension(
    m_numDimensions,
    queryPointNorm,
    whichVariableToCompromise);

  ProjectRayResultClosest projectionResult;
  projectionResult.initClosest(
    nDMesh,
    m_apQueryComponentIndices,
    ndMeshQueryResult);

  for (uint32_t index = 0; index < numEntries; ++index)
  {
    // Convert the CP cell address to a coordinate
    uint32_t cpTopLeftCoord[SCATTERED_DATA_MAX_DIM];
    nDMesh->coordinateFromAddress(cpTopLeftCoord, cpCellAddresses[index]);

    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      if ((whichDimFlags[index] >> whichDim) & 0x01)
      {
        uint32_t whichBoundary = (whichBoundaryFlags[index] >> whichDim) & 0x01;
        NMP_ASSERT(nDMesh->isNDCellBoundaryFacet(cpTopLeftCoord, whichDim, whichBoundary));

        // Compute the projected point in the Nd space
        nDMesh->projectRay(
          cpTopLeftCoord,
          whichDim,
          whichBoundary,
          &projectionResult,
          &projectionBasis);
      }
    }
  }

  //------------------------------
  // Write the projected point data
  if (projectionResult.getCount() > 0)
  {
    NMP_ASSERT(ndMeshQueryResult->m_interpolationType == NDMeshQueryResult::kNDCellFacet);

    // Overwrite the output query point with the projected result
    nDMesh->getNDCellFacetVertexComponents(
      ndMeshQueryResult->m_topLeftCoord,
      ndMeshQueryResult->m_whichDim,
      ndMeshQueryResult->m_whichBoundary,
      m_numDimensions,
      m_apQueryComponentIndices,
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
bool NDMeshAPResampleMap::projectThroughPoint(
  const NDMesh*       nDMesh,
  float*              queryPoint,
  NDMeshQueryResult*  ndMeshQueryResult,
  uint32_t            numOutputSampleComponents,
  const uint32_t*     outputSampleComponentIndices,
  float*              outputSampleComponents) const
{
  NMP_ASSERT(nDMesh);
  NMP_ASSERT(queryPoint);
  NMP_ASSERT(ndMeshQueryResult);
  NMP_ASSERT(numOutputSampleComponents > 0);
  NMP_ASSERT(outputSampleComponentIndices);
  NMP_ASSERT(outputSampleComponents);

  PREDICTION_PROFILE_BEGIN_CONTEXT("projectThroughPoint");

  // In 1D you always project to 'all' 2 boundary points wherever you are.
  if (m_numDimensions == 1)
  {
    nDMesh->projection1DClosest(
      queryPoint,
      m_apQueryComponentIndices,
      ndMeshQueryResult,
      numOutputSampleComponents,
      outputSampleComponentIndices,
      outputSampleComponents);

    return true;
  }

  NMP_ASSERT_MSG(
    m_projectionThroughPointEnable,
    "A ProjectThroughPoint map has not been built for this resample map");

  // Initialisation
  ndMeshQueryResult->init();

  // Get the projection centre
  float projectionCentre[SCATTERED_DATA_MAX_DIM];
  nDMesh->getSampleCentre(
    m_numDimensions,
    m_apQueryComponentIndices,
    projectionCentre);

  //------------------------------
  // Find the projection cell coordinates within the projection map
  uint32_t whichAABBDim;
  uint32_t whichAABBBoundary;
  uint32_t apTopLeftCoord[SCATTERED_DATA_MAX_DIM];
  if (!projectThroughPointCellCoordinates(
    queryPoint,
    projectionCentre,
    whichAABBDim,
    whichAABBBoundary,
    apTopLeftCoord))
  {
    return false;
  }

  // Unpack the projection cell entries
  uint32_t cpCellAddresses[gMaxNumCellEntries];
  uint32_t whichDimFlags[gMaxNumCellEntries];
  uint32_t whichBoundaryFlags[gMaxNumCellEntries];
  uint32_t numEntries = getProjectionThroughPointCellEntries(
    whichAABBDim,
    whichAABBBoundary,
    apTopLeftCoord,
    gMaxNumCellEntries,
    cpCellAddresses,
    whichDimFlags,
    whichBoundaryFlags);

  // Apply the transform that maps the query point into the normalised space
  float queryPointNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    m_apQueryComponentIndices,
    queryPoint,
    queryPointNorm);

  // Apply the transform that maps the projection centre into the normalised space
  float projectionCentreNorm[SCATTERED_DATA_MAX_DIM];
  nDMesh->transformRealToNorm(
    m_numDimensions,
    m_apQueryComponentIndices,
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
    m_apQueryComponentIndices,
    ndMeshQueryResult);

  for (uint32_t index = 0; index < numEntries; ++index)
  {
    // Convert the CP cell address to a coordinate
    uint32_t cpTopLeftCoord[SCATTERED_DATA_MAX_DIM];
    nDMesh->coordinateFromAddress(cpTopLeftCoord, cpCellAddresses[index]);

    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      if ((whichDimFlags[index] >> whichDim) & 0x01)
      {
        uint32_t whichBoundary = (whichBoundaryFlags[index] >> whichDim) & 0x01;
        NMP_ASSERT(nDMesh->isNDCellBoundaryFacet(cpTopLeftCoord, whichDim, whichBoundary));

        // Compute the projected point in the Nd space
        nDMesh->projectRay(
          cpTopLeftCoord,
          whichDim,
          whichBoundary,
          &projectionResult,
          &projectionBasis);
      }
    }
  }

  //------------------------------
  // Write the projected point data
  if (projectionResult.getCount() > 0)
  {
    NMP_ASSERT(ndMeshQueryResult->m_interpolationType == NDMeshQueryResult::kNDCellFacet);

    // Overwrite the output query point with the projected result
    nDMesh->getNDCellFacetVertexComponents(
      ndMeshQueryResult->m_topLeftCoord,
      ndMeshQueryResult->m_whichDim,
      ndMeshQueryResult->m_whichBoundary,
      m_numDimensions,
      m_apQueryComponentIndices,
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
bool NDMeshAPResampleMap::interpolateMultilinearWeights(
  const float*      apQueryPoint,
  uint32_t*         apTopLeftCoord,
  float*            apInterpolants) const
{
  NMP_ASSERT(apQueryPoint);
  NMP_ASSERT(apTopLeftCoord);
  NMP_ASSERT(apInterpolants);

  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    // Cells must have at least two boundary samples. We don't allow single
    // samples in a dimension for a prediction model since it doesn't model
    // a controlling input.
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);

    // Compute the regular sample grid position 
    float u = (apQueryPoint[dim] - m_apGridStartValues[dim]) * m_apGridSampleScales[dim];
    if (u < -gAbsCellTol || u > (float)(m_sampleCountsPerDimension[dim] - 1) + gAbsCellTol)
    {
      return false;
    }

    // Compute the top left grid position and interpolant. Note that the position and interpolant
    // are adjusted for the last span if the CP parameter is at the end sample value
    apTopLeftCoord[dim] = NMP::minimum((uint32_t)u, m_sampleCountsPerDimension[dim] - 2);
    NMP_ASSERT(apTopLeftCoord[dim] < m_sampleCountsPerDimension[dim]);
    apInterpolants[dim] = u - (float)apTopLeftCoord[dim];
    NMP_ASSERT(apInterpolants[dim] >= gMinCellTol && apInterpolants[dim] <= gMaxCellTol);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPResampleMap::projectAlongDimCellCoordinates(
  const float*      apQueryPoint,
  uint32_t          whichVariableToCompromise,
  uint32_t*         apTopLeftCoord) const
{
  NMP_ASSERT(apQueryPoint);
  NMP_ASSERT(m_numDimensions > 1);
  NMP_ASSERT(whichVariableToCompromise < m_numDimensions);
  NMP_ASSERT(apTopLeftCoord);

  // Get the (N-1)d array of which query component indices correspond to the complementary
  // dimensions, i.e. the array of indices that does not contain whichVariableToCompromise.
  const uint32_t* projectionWhichQueryComponentIndices = m_projectionWhichQueryComponentIndices[whichVariableToCompromise];

  uint32_t M = m_numDimensions - 1;
  for (uint32_t i = 0; i < M; ++i)
  {
    // Get the dimension index for the query
    uint32_t dim = projectionWhichQueryComponentIndices[i];
    NMP_ASSERT(dim < m_numDimensions);

    // Cells must have at least two boundary samples. We don't allow single
    // samples in a dimension for a prediction model since it doesn't model
    // a controlling input.
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);

    // Compute the regular sample grid position 
    float u = (apQueryPoint[dim] - m_apGridStartValues[dim]) * m_apGridSampleScales[dim];
    if (u < -gAbsCellTol || u > (float)(m_sampleCountsPerDimension[dim] - 1) + gAbsCellTol)
    {
      return false;
    }

    // Compute the top left grid position and interpolant. Note that the position and interpolant
    // are adjusted for the last span if the CP parameter is at the end sample value
    apTopLeftCoord[i] = NMP::minimum((uint32_t)u, m_sampleCountsPerDimension[dim] - 2);
    NMP_ASSERT(apTopLeftCoord[i] < m_sampleCountsPerDimension[dim]);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NDMeshAPResampleMap::projectThroughPointCellCoordinates(
  const float*        apQueryPoint,
  const float*        projectionCentre,
  uint32_t&           whichAABBDim,
  uint32_t&           whichAABBBoundary,
  uint32_t*           apTopLeftCoord) const
{
  NMP_ASSERT(apQueryPoint);
  NMP_ASSERT(m_numDimensions > 1);
  NMP_ASSERT(projectionCentre);
  NMP_ASSERT(apTopLeftCoord);

  // Compute the closest point that projects through the query point onto the resample map AABB
  float projectedAABBPoint[SCATTERED_DATA_MAX_DIM];
  if (!ScatteredData::aabbIntersectsRayClosestPoint(
    m_numDimensions,
    m_apGridStartValues,
    m_apGridEndValues,
    projectionCentre,
    apQueryPoint,
    whichAABBDim,
    whichAABBBoundary,
    projectedAABBPoint))
  {
    return false;
  }

  //------------------------------
  // Find the projection cell coordinates within the projection map: (function permutes Nd -> N-1d)
  NMP_ASSERT(whichAABBDim < m_numDimensions);
  const uint32_t* projectionWhichQueryComponentIndices = m_projectionWhichQueryComponentIndices[whichAABBDim];
  uint32_t M = m_numDimensions - 1;
  for (uint32_t i = 0; i < M; ++i)
  {
    // Get the dimension index for the query
    uint32_t dim = projectionWhichQueryComponentIndices[i];
    NMP_ASSERT(dim < m_numDimensions);

    // Cells must have at least two boundary samples. We don't allow single
    // samples in a dimension for a prediction model since it doesn't model
    // a controlling input.
    NMP_ASSERT(m_sampleCountsPerDimension[dim] > 1);

    // Compute the regular sample grid position 
    float u = (projectedAABBPoint[dim] - m_apGridStartValues[dim]) * m_apGridSampleScales[dim];

    // Compute the top left grid position and interpolant. Note that the position and interpolant
    // are adjusted for the last span if the CP parameter is at the end sample value
    apTopLeftCoord[i] = NMP::minimum((uint32_t)u, m_sampleCountsPerDimension[dim] - 2);
    NMP_ASSERT(apTopLeftCoord[i] < m_sampleCountsPerDimension[dim]);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NDMeshAPResampleMap::getProjectionAlongDimensionCellEntries(
  uint32_t          whichVariableToCompromise,
  const uint32_t*   apTopLeftCoord,
  uint32_t          NMP_USED_FOR_ASSERTS(maxNumEntries),
  uint32_t*         cpCellAddresses,
  uint32_t*         whichDimFlags,
  uint32_t*         whichBoundaryFlags) const
{
  NMP_ASSERT(whichVariableToCompromise < m_numDimensions);
  NMP_ASSERT(apTopLeftCoord);
  NMP_ASSERT(maxNumEntries > 0);
  NMP_ASSERT(cpCellAddresses);
  NMP_ASSERT(whichDimFlags);
  NMP_ASSERT(whichBoundaryFlags);

  NMP_ASSERT(m_projectionCellBlockSizes);
  const uint32_t* projectionCellBlockSizes = m_projectionCellBlockSizes[whichVariableToCompromise];
  NMP_ASSERT(projectionCellBlockSizes);
  uint32_t M = m_numDimensions - 1;

  // Get the address of the projection cell
  uint32_t apCellAddress = 0;
  for (uint32_t i = 0; i < M; ++i)
  {
    apCellAddress += apTopLeftCoord[i] * projectionCellBlockSizes[i];
  }

  // Get the offset within the cell entry table corresponding to the cell address
  const NDMeshAPResampleMapProjectionData* projectionAlongDimensionData = m_projectionAlongDimensionData[whichVariableToCompromise];
  NMP_ASSERT(projectionAlongDimensionData);
  uint32_t offsetA = projectionAlongDimensionData->m_cellEntryOffsets->getEntry(apCellAddress);
  uint32_t offsetB = projectionAlongDimensionData->m_cellEntryOffsets->getEntry(apCellAddress + 1);

  // Bitmasks
  uint32_t dimsMask = ~(0xFFFFFFFF << m_numDimensions);
  uint32_t cellAddrShift = m_numDimensions << 1;

  // Unpack the entry data [cpCellAddress | whichDimFlags | whichBoundaryFlags]
  uint32_t numEntries = 0;
  for (uint32_t index = offsetA; index < offsetB; ++index)
  {
    NMP_ASSERT(numEntries < maxNumEntries);
    uint32_t entry = projectionAlongDimensionData->m_cellEntries->getEntry(index);
    whichBoundaryFlags[numEntries] = entry & dimsMask;
    whichDimFlags[numEntries] = (entry >> m_numDimensions) & dimsMask;
    cpCellAddresses[numEntries] = (entry >> cellAddrShift);
    numEntries++;
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NDMeshAPResampleMap::getProjectionThroughPointCellEntries(
  uint32_t          whichDim,
  uint32_t          whichBoundary,
  const uint32_t*   apTopLeftCoord,
  uint32_t          NMP_USED_FOR_ASSERTS(maxNumEntries),
  uint32_t*         cpCellAddresses,
  uint32_t*         whichDimFlags,
  uint32_t*         whichBoundaryFlags) const
{
  NMP_ASSERT(whichDim < m_numDimensions);
  NMP_ASSERT(whichBoundary == 0 || whichBoundary == 1);
  NMP_ASSERT(apTopLeftCoord);
  NMP_ASSERT(maxNumEntries > 0);
  NMP_ASSERT(cpCellAddresses);
  NMP_ASSERT(whichDimFlags);
  NMP_ASSERT(whichBoundaryFlags);

  const NDMeshAPResampleMapProjectionData* const* projectionThroughPointDataTbl = m_projectionThroughPointData[whichBoundary];
  NMP_ASSERT(projectionThroughPointDataTbl);
  const NDMeshAPResampleMapProjectionData* projectionThroughPointData = projectionThroughPointDataTbl[whichDim];
  NMP_ASSERT(projectionThroughPointData);

  NMP_ASSERT(m_projectionCellBlockSizes);
  const uint32_t* projectionCellBlockSizes = m_projectionCellBlockSizes[whichDim];
  NMP_ASSERT(projectionCellBlockSizes);
  uint32_t M = m_numDimensions - 1;

  // Get the address of the projection cell
  uint32_t apCellAddress = 0;
  for (uint32_t i = 0; i < M; ++i)
  {
    apCellAddress += apTopLeftCoord[i] * projectionCellBlockSizes[i];
  }

  uint32_t offsetA = projectionThroughPointData->m_cellEntryOffsets->getEntry(apCellAddress);
  uint32_t offsetB = projectionThroughPointData->m_cellEntryOffsets->getEntry(apCellAddress + 1);

  // Bitmasks
  uint32_t dimsMask = ~(0xFFFFFFFF << m_numDimensions);
  uint32_t cellAddrShift = m_numDimensions << 1;

  // Unpack the entry data [cpCellAddress | whichDimFlags | whichBoundaryFlags]
  uint32_t numEntries = 0;
  for (uint32_t index = offsetA; index < offsetB; ++index)
  {
    NMP_ASSERT(numEntries < maxNumEntries);
    uint32_t entry = projectionThroughPointData->m_cellEntries->getEntry(index);
    whichBoundaryFlags[numEntries] = entry & dimsMask;
    whichDimFlags[numEntries] = (entry >> m_numDimensions) & dimsMask;
    cpCellAddresses[numEntries] = (entry >> cellAddrShift);
    numEntries++;
  }

  return numEntries;
}

} // namespace ScatteredData

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
