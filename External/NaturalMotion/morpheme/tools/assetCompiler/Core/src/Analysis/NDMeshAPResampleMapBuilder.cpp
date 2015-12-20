// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/NDMeshAPResampleMapBuilder.h"
#include "Analysis/NDMeshResampleMapExportData.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "NMPlatform/NMSystem.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPResampleMapProjectionDataBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshAPResampleMapProjectionDataBuilder::getMemoryRequirements(
  uint32_t numDimensions,
  const NDMeshResampleMapProjectionExportData* projectionData)
{
  NMP_VERIFY(projectionData);
  uint32_t numCellSamples = projectionData->getNumCellSamples();
  uint32_t numEntries = projectionData->getNumEntries();

  // Get the maximum entry value
  uint32_t entryCAMax = 0;
  const uint32_t* entriesCA = projectionData->getEntriesCA();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    entryCAMax = NMP::maximum(entriesCA[i], entryCAMax);
  }

  // Compute the number of bits per entry
  uint32_t numBitsPerOffset = MR::PackedArrayUInt32::calculateNumBitsForValue(numEntries);
  uint32_t numBitsPerFlagEntry = numDimensions << 1;
  uint32_t numBitsPerEntry = MR::PackedArrayUInt32::calculateNumBitsForValue(entryCAMax) + numBitsPerFlagEntry;
  NMP_VERIFY(numBitsPerEntry <= 32);

  // Header
  NMP::Memory::Format result(sizeof(MR::ScatteredData::NDMeshAPResampleMapProjectionData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Data
  NMP::Memory::Format memReqsOffsets = MR::PackedArrayUInt32::getMemoryRequirements(numCellSamples + 1, numBitsPerOffset);
  result += memReqsOffsets;
  NMP::Memory::Format memReqsEntries = MR::PackedArrayUInt32::getMemoryRequirements(numEntries, numBitsPerEntry);
  result += memReqsEntries;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::ScatteredData::NDMeshAPResampleMapProjectionData*
NDMeshAPResampleMapProjectionDataBuilder::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  const NDMeshResampleMapProjectionExportData* projectionData)
{
  NMP_VERIFY(projectionData);
  uint32_t numCellSamples = projectionData->getNumCellSamples();
  uint32_t numEntries = projectionData->getNumEntries();

  // Projection data
  const uint32_t* entryCounts = projectionData->getCellEntryCounts();
  const uint32_t* entriesCA = projectionData->getEntriesCA();
  const uint32_t* entriesWD = projectionData->getEntriesWD();
  const uint32_t* entriesWB = projectionData->getEntriesWB();

  // Get the maximum entry value
  uint32_t entryCAMax = 0;
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    entryCAMax = NMP::maximum(entriesCA[i], entryCAMax);
  }

  // Compute the number of bits per entry
  uint32_t numBitsPerOffset = MR::PackedArrayUInt32::calculateNumBitsForValue(numEntries);
  uint32_t numBitsPerFlagEntry = numDimensions << 1;
  uint32_t numBitsPerEntry = MR::PackedArrayUInt32::calculateNumBitsForValue(entryCAMax) + numBitsPerFlagEntry;
  NMP_VERIFY(numBitsPerEntry <= 32);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::ScatteredData::NDMeshAPResampleMapProjectionData), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::ScatteredData::NDMeshAPResampleMapProjectionData* result =
    (MR::ScatteredData::NDMeshAPResampleMapProjectionData*)resource.alignAndIncrement(memReqsHdr);

  result->m_cellEntryOffsets = MR::PackedArrayUInt32::init(resource, numCellSamples + 1, numBitsPerOffset);
  result->m_cellEntries = MR::PackedArrayUInt32::init(resource, numEntries, numBitsPerEntry);

  // Initialise the data
  uint32_t dimsMask = ~(0xFFFFFFFF << numDimensions);
  uint32_t count = 0;
  for (uint32_t cellIndex = 0; cellIndex < numCellSamples; ++cellIndex)
  {
    // Offset entry
    uint32_t entryCount = entryCounts[cellIndex];
    result->m_cellEntryOffsets->setEntry(cellIndex, count);

    // Cell address entries
    for (uint32_t i = 0; i < entryCount; ++i, ++count)
    {
      NMP_VERIFY(entriesWD[count] <= dimsMask);
      NMP_VERIFY(entriesWB[count] <= dimsMask);

      // Pack the entry data [cpCellAddress | whichDimFlags | whichBoundaryFlags]
      uint32_t entry = (entriesCA[count] << numBitsPerFlagEntry) |
                       (entriesWD[count] << numDimensions) |
                       (entriesWB[count]);

      result->m_cellEntries->setEntry(count, entry);
    }
  }
  NMP_VERIFY(count == numEntries);
  result->m_cellEntryOffsets->setEntry(numCellSamples, count);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// NDMeshAPResampleMapBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshAPResampleMapBuilder::getMemoryRequirements(
  const NDMeshResampleMapExportData* nDMeshResampleGrid)
{
  NMP_VERIFY(nDMeshResampleGrid);

  uint32_t numDimensions = nDMeshResampleGrid->getNumDimensions();
  NMP_ASSERT(numDimensions > 0);
  uint32_t M = numDimensions - 1;
  uint32_t numAPQueryComplementComponentIndices = nDMeshResampleGrid->getNumAPQueryComplementComponentIndices();
  NMP_ASSERT(numAPQueryComplementComponentIndices > 0 && numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);
  const NDMeshResampleMapCPSampleQuantisationData* interpolationQuantisationData = nDMeshResampleGrid->getInterpolationQuantisationData();

  // Header
  NMP::Memory::Format result(sizeof(MR::ScatteredData::NDMeshAPResampleMap), NMP_NATURAL_TYPE_ALIGNMENT);

  // AP Query components
  NMP::Memory::Format memReqsSampleCount(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsSampleCount;
  NMP::Memory::Format memReqsAPComplement(sizeof(uint32_t) * numAPQueryComplementComponentIndices, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsAPComplement;

  // Sample counts
  result += memReqsSampleCount;

  // Grid range information
  NMP::Memory::Format memReqsGridValues(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsGridValues * 3);

  //------------------------------
  // Interpolation grid data
  const NDMeshResampleMapGridExportData* interpolationGrid = nDMeshResampleGrid->getInterpolationGrid();
  uint32_t numCellSamples = interpolationGrid->getNumCellSamples();

  NMP::Memory::Format memReqsInterpBlockSizes(sizeof(uint32_t) * (numDimensions + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsInterpBlockSizes * 2);

  NMP::Memory::Format memReqsInterpQData(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsInterpQData * 2);

  NMP::Memory::Format memReqsInterpSamples = MR::PackedArrayUInt32::getMemoryRequirements(
    interpolationQuantisationData->getVertexCPSamples());
  result += memReqsInterpSamples;

  NMP::Memory::Format memReqsInterpFlags = NMP::BitArray::getMemoryRequirements(numCellSamples);
  result += memReqsInterpFlags;

  //------------------------------
  // Projection resample maps
  if (M > 0)
  {
    //------------------------------
    // Projection grid data
    NMP::Memory::Format memReqsPrjTable(sizeof(void*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsPrjTable * 2);

    NMP::Memory::Format memReqsPrjQueryIndices(sizeof(uint32_t) * M, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsPrjBlockSizes(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      result += memReqsPrjQueryIndices;
      result += memReqsPrjBlockSizes;
    }

    //------------------------------
    // Projection along dimension resample map data
    result += memReqsPrjTable;
    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      const NDMeshResampleMapProjectionExportData* projectionData = nDMeshResampleGrid->getProjectAlongDimensionData(whichDim);
      NMP_VERIFY(projectionData);
      NMP::Memory::Format memReqsPrjData = NDMeshAPResampleMapProjectionDataBuilder::getMemoryRequirements(
        numDimensions,
        projectionData);
      result += memReqsPrjData;
    }

    //------------------------------
    // Projection through point resample map data
    bool projectionThroughPointEnable = nDMeshResampleGrid->getBuildProjectThroughSampleCentreMap();
    if (projectionThroughPointEnable)
    {
      // Pointers table
      result += (memReqsPrjTable * 2);

      for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
      {
        const NDMeshResampleMapProjectionExportData* projectionDataA = nDMeshResampleGrid->getProjectThroughPointData(whichDim, 0);
        NMP_VERIFY(projectionDataA);
        NMP::Memory::Format memReqsPrjDataA = NDMeshAPResampleMapProjectionDataBuilder::getMemoryRequirements(
          numDimensions,
          projectionDataA);
        result += memReqsPrjDataA;

        const NDMeshResampleMapProjectionExportData* projectionDataB = nDMeshResampleGrid->getProjectThroughPointData(whichDim, 1);
        NMP_VERIFY(projectionDataB);
        NMP::Memory::Format memReqsPrjDataB = NDMeshAPResampleMapProjectionDataBuilder::getMemoryRequirements(
          numDimensions,
          projectionDataB);
        result += memReqsPrjDataB;
      }
    }
  }

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::ScatteredData::NDMeshAPResampleMap* NDMeshAPResampleMapBuilder::init(
  NMP::Memory::Resource& resource,
  const NDMeshResampleMapExportData* nDMeshResampleGrid)
{
  NMP_VERIFY(nDMeshResampleGrid);

  uint32_t numDimensions = nDMeshResampleGrid->getNumDimensions();
  NMP_ASSERT(numDimensions > 0);
  uint32_t M = numDimensions - 1;
  uint32_t numAPQueryComplementComponentIndices = nDMeshResampleGrid->getNumAPQueryComplementComponentIndices();
  NMP_ASSERT(numAPQueryComplementComponentIndices > 0 && numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);

  const NDMeshResampleMapCPSampleExportData* interpolationData = nDMeshResampleGrid->getInterpolationData();
  const NDMeshResampleMapGridExportData* interpolationGrid = nDMeshResampleGrid->getInterpolationGrid();
  const NDMeshResampleMapCPSampleQuantisationData* interpolationQuantisationData = nDMeshResampleGrid->getInterpolationQuantisationData();
  const uint32_t* sampleCountsPerDimension = interpolationGrid->getSampleCountsPerDimension();
  const float* apGridStartValues = interpolationGrid->getStartRanges();
  const float* apGridEndValues = interpolationGrid->getEndRanges();
  const uint32_t* vertexBlockSizes = interpolationGrid->getVertexBlockSizes();
  const uint32_t* cellBlockSizes = interpolationGrid->getCellBlockSizes();
  uint32_t numVertexSamples = interpolationGrid->getNumVertexSamples();
  uint32_t numCellSamples = interpolationGrid->getNumCellSamples();

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::ScatteredData::NDMeshAPResampleMap), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshAPResampleMapBuilder* result = (NDMeshAPResampleMapBuilder*)resource.alignAndIncrement(memReqsHdr);
  result->m_numDimensions = numDimensions;
  result->m_numAPQueryComplementComponentIndices = numAPQueryComplementComponentIndices;

  // AP Query components
  NMP::Memory::Format memReqsSampleCount(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apQueryComponentIndices = (uint32_t*)resource.alignAndIncrement(memReqsSampleCount);
  const uint32_t* apQueryComponentIndices = nDMeshResampleGrid->getAPQueryComponentIndices();
  NMP_VERIFY(apQueryComponentIndices);
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_apQueryComponentIndices[i] = apQueryComponentIndices[i];
  }

  NMP::Memory::Format memReqsAPComplement(sizeof(uint32_t) * numAPQueryComplementComponentIndices, NMP_NATURAL_TYPE_ALIGNMENT);  
  result->m_apQueryComplementComponentIndices = (uint32_t*)resource.alignAndIncrement(memReqsAPComplement);
  const uint32_t* apQueryComplementComponentIndices = nDMeshResampleGrid->getAPQueryComplementComponentIndices();
  NMP_VERIFY(apQueryComplementComponentIndices);
  for (uint32_t i = 0; i < numAPQueryComplementComponentIndices; ++i)
  {
    result->m_apQueryComplementComponentIndices[i] = apQueryComplementComponentIndices[i];
  }

  // Sample counts
  result->m_sampleCountsPerDimension = (uint32_t*)resource.alignAndIncrement(memReqsSampleCount);
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_sampleCountsPerDimension[i] = sampleCountsPerDimension[i];
  }

  // Grid range information
  NMP::Memory::Format memReqsGridValues(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apGridStartValues = (float*)resource.alignAndIncrement(memReqsGridValues);
  result->m_apGridEndValues = (float*)resource.alignAndIncrement(memReqsGridValues);
  result->m_apGridSampleScales = (float*)resource.alignAndIncrement(memReqsGridValues);

  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_apGridStartValues[i] = apGridStartValues[i];
    result->m_apGridEndValues[i] = apGridEndValues[i];
    float sampleRange = apGridEndValues[i] - apGridStartValues[i];
    uint32_t numIntervals = sampleCountsPerDimension[i] - 1;
    if (NMP::nmfabs(sampleRange) > 1e-7f)
    {
      result->m_apGridSampleScales[i] = (float)numIntervals / sampleRange;
    }
    else
    {
      result->m_apGridSampleScales[i] = 0.0f;
    }
  }

  //------------------------------
  // Interpolation grid data
  NMP::Memory::Format memReqsInterpBlockSizes(sizeof(uint32_t) * (numDimensions + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_interpolationVertexBlockSizes = (uint32_t*)resource.alignAndIncrement(memReqsInterpBlockSizes);
  result->m_interpolationCellBlockSizes = (uint32_t*)resource.alignAndIncrement(memReqsInterpBlockSizes);
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_interpolationVertexBlockSizes[i] = vertexBlockSizes[i];
    result->m_interpolationCellBlockSizes[i] = cellBlockSizes[i];
  }
  result->m_interpolationVertexBlockSizes[numDimensions] = numVertexSamples;
  result->m_interpolationCellBlockSizes[numDimensions] = numCellSamples;

  NMP::Memory::Format memReqsInterpQData(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_interpolationVertexCPScales = (float*)resource.alignAndIncrement(memReqsInterpQData);
  result->m_interpolationVertexCPOffsets = (float*)resource.alignAndIncrement(memReqsInterpQData);
  const float* interpQScales = interpolationQuantisationData->getStepSizes();
  const float* interpQOffsets = interpolationQuantisationData->getQMinValues();
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_interpolationVertexCPScales[i] = interpQScales[i];
    result->m_interpolationVertexCPOffsets[i] = interpQOffsets[i];
  }

  result->m_interpolationVertexCPSamples = MR::PackedArrayUInt32::init(
    resource,
    interpolationQuantisationData->getVertexCPSamples());

  // Initialise the interpolation cell flags
  const bool* cellFlags = interpolationData->getCellFlags();
  result->m_interpolationCellFlags = NMP::BitArray::init(resource, numCellSamples);
  for (uint32_t i = 0; i < numCellSamples; ++i)
  {
    result->m_interpolationCellFlags->setBit(i, cellFlags[i]);
  }

  //------------------------------
  // Projection resample maps
  if (M > 0)
  {
    //------------------------------
    // Projection grid data
    NMP::Memory::Format memReqsPrjTable(sizeof(void*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_projectionWhichQueryComponentIndices = (uint32_t**)resource.alignAndIncrement(memReqsPrjTable);
    result->m_projectionCellBlockSizes = (uint32_t**)resource.alignAndIncrement(memReqsPrjTable);

    NMP::Memory::Format memReqsPrjQueryIndices(sizeof(uint32_t) * M, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsPrjBlockSizes(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);

    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      const NDMeshResampleMapGridExportData* projectionGrid = nDMeshResampleGrid->getProjectionGrid(whichDim);
      NMP_VERIFY(projectionGrid);

      const uint32_t* whichQueryComponentIndices = projectionGrid->getWhichQueryComponentIndices();
      cellBlockSizes = projectionGrid->getCellBlockSizes();
      numCellSamples = projectionGrid->getNumCellSamples();

      // Projection grid information
      uint32_t* whichQueryComponentIndicesDst = (uint32_t*)resource.alignAndIncrement(memReqsPrjQueryIndices);
      result->m_projectionWhichQueryComponentIndices[whichDim] = whichQueryComponentIndicesDst;
      for (uint32_t i = 0; i < M; ++i)
      {
        whichQueryComponentIndicesDst[i] = whichQueryComponentIndices[i];
      }

      uint32_t* cellBlockSizesDst = (uint32_t*)resource.alignAndIncrement(memReqsPrjBlockSizes);
      result->m_projectionCellBlockSizes[whichDim] = cellBlockSizesDst;
      for (uint32_t i = 0; i < M; ++i)
      {
        cellBlockSizesDst[i] = cellBlockSizes[i];
      }
      cellBlockSizesDst[M] = numCellSamples;
    }

    //------------------------------
    // Projection along dimension resample map data
    result->m_projectionAlongDimensionData =
      (MR::ScatteredData::NDMeshAPResampleMapProjectionData**)resource.alignAndIncrement(memReqsPrjTable);

    for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
    {
      const NDMeshResampleMapProjectionExportData* projectionData = nDMeshResampleGrid->getProjectAlongDimensionData(whichDim);
      NMP_VERIFY(projectionData);
      result->m_projectionAlongDimensionData[whichDim] = NDMeshAPResampleMapProjectionDataBuilder::init(
        resource,
        numDimensions,
        projectionData);
    }

    //------------------------------
    // Projection through point resample map data
    bool projectionThroughPointEnable = nDMeshResampleGrid->getBuildProjectThroughSampleCentreMap();
    result->m_projectionThroughPointEnable = projectionThroughPointEnable;
    if (projectionThroughPointEnable)
    {
      // Pointers table
      result->m_projectionThroughPointData[0] =
        (MR::ScatteredData::NDMeshAPResampleMapProjectionData**)resource.alignAndIncrement(memReqsPrjTable);
      result->m_projectionThroughPointData[1] =
        (MR::ScatteredData::NDMeshAPResampleMapProjectionData**)resource.alignAndIncrement(memReqsPrjTable);

      for (uint32_t whichDim = 0; whichDim < numDimensions; ++whichDim)
      {
        const NDMeshResampleMapProjectionExportData* projectionDataA = nDMeshResampleGrid->getProjectThroughPointData(whichDim, 0);
        NMP_VERIFY(projectionDataA);
        result->m_projectionThroughPointData[0][whichDim] = NDMeshAPResampleMapProjectionDataBuilder::init(
          resource,
          numDimensions,
          projectionDataA);

        const NDMeshResampleMapProjectionExportData* projectionDataB = nDMeshResampleGrid->getProjectThroughPointData(whichDim, 1);
        NMP_VERIFY(projectionDataB);
        result->m_projectionThroughPointData[1][whichDim] = NDMeshAPResampleMapProjectionDataBuilder::init(
          resource,
          numDimensions,
          projectionDataB);
      }
    }
  }
  else
  {
    result->m_projectionThroughPointEnable = false;
    result->m_projectionWhichQueryComponentIndices = NULL;
    result->m_projectionCellBlockSizes = NULL;
    result->m_projectionAlongDimensionData = NULL;
    result->m_projectionThroughPointData[0] = NULL;
    result->m_projectionThroughPointData[1] = NULL;
  }

  // Multiple of the data alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource NDMeshAPResampleMapBuilder::create(
  const MR::ScatteredData::NDMesh* nDMesh,
  const NDMeshResampleMapExportData* nDMeshResampleGrid)
{
  NMP_VERIFY(nDMesh);
  NMP_VERIFY(nDMeshResampleGrid);

  //-------------------------------
  NMP::Memory::Format memReqs = NDMeshAPResampleMapBuilder::getMemoryRequirements(nDMeshResampleGrid);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  NMP::Memory::Resource result = memRes;
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshAPResampleMapBuilder::init(memRes, nDMeshResampleGrid);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "The NDMeshAPResampleMapBuilder did not initialise all the allocated memory: %d bytes remain.",
    memRes.format.size);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
