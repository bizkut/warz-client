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
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/NDMeshResampleMapExportData.h"
#include "Analysis/ControlParameterRegularSampleGrid.h"
#include "export/mcExport.h"
#include "morpheme/Prediction/mrScatteredDataUtils.h"
#include "morpheme/Prediction/mrPredictionModelNDMesh.h"
#include "NMNumerics/NMUniformQuantisation.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
static const uint32_t gMaxNumNodeEntries = 128;

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapGridExportData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleMapGridExportData::getMemoryRequirements(
  uint32_t numDimensions,
  const uint32_t* sampleCounts)
{
  NMP_VERIFY_MSG(
    numDimensions > 0,
    "NDMeshResampleMapGridExportData requires one or more dimensions");
  NMP_VERIFY(sampleCounts);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleMapGridExportData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Block sizes
  NMP::Memory::Format memReqsBlockSizes(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsBlockSizes * 2);

  // Which sample components
  result += memReqsBlockSizes;

  // Analysis property resample grid
  NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsSampleCounts;

  NMP::Memory::Format memReqsRng(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsRng * 2);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapGridExportData* NDMeshResampleMapGridExportData::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  const uint32_t* sampleCounts)
{
  NMP_VERIFY_MSG(
    numDimensions > 0,
    "NDMeshResampleMapGridExportData requires one or more dimensions");
  NMP_VERIFY(sampleCounts);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleMapGridExportData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleMapGridExportData* result = (NDMeshResampleMapGridExportData*)resource.alignAndIncrement(memReqsHdr);
  result->m_numDimensions = numDimensions;

  // Block sizes
  NMP::Memory::Format memReqsBlockSizes(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_nDVertexBlockSizes = (uint32_t*)resource.alignAndIncrement(memReqsBlockSizes);
  result->m_nDCellBlockSizes = (uint32_t*)resource.alignAndIncrement(memReqsBlockSizes);

  uint32_t numVertexSamples = 1;
  uint32_t numGridCells = 1;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    NMP_VERIFY(sampleCounts[i] > 1);
    result->m_nDVertexBlockSizes[i] = numVertexSamples;
    numVertexSamples *= sampleCounts[i];
 
    result->m_nDCellBlockSizes[i] = numGridCells;
    numGridCells *= (sampleCounts[i] - 1);
  }
  result->m_numVertexSamples = numVertexSamples;
  result->m_numCellSamples = numGridCells;

  // Which sample components
  result->m_whichQueryComponentIndices = (uint32_t*)resource.alignAndIncrement(memReqsBlockSizes);

  // Analysis property resample grid
  NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sampleCountsPerDimension = (uint32_t*)resource.alignAndIncrement(memReqsSampleCounts);
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_sampleCountsPerDimension[i] = sampleCounts[i];
  }

  NMP::Memory::Format memReqsRng(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_startRanges = (float*)resource.alignAndIncrement(memReqsRng);
  result->m_endRanges = (float*)resource.alignAndIncrement(memReqsRng);
 
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapGridExportData* NDMeshResampleMapGridExportData::create(
  uint32_t numDimensions,
  const uint32_t* sampleCounts)
{
  // Allocate the memory for the resample grid
  NMP::Memory::Format memReqs = NDMeshResampleMapGridExportData::getMemoryRequirements(numDimensions, sampleCounts);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshResampleMapGridExportData* result = NDMeshResampleMapGridExportData::init(memRes, numDimensions, sampleCounts);
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleMapGridExportData did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapCPSampleExportData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleMapCPSampleExportData::getMemoryRequirements(
  uint32_t numDimensions,
  const uint32_t* sampleCounts,
  uint32_t numComponentsPerSample)
{
  NMP_VERIFY(numDimensions > 0);
  NMP_VERIFY(sampleCounts);
  NMP_VERIFY(numComponentsPerSample > 0);

  uint32_t numVertexSamples = 1;
  uint32_t numGridCells = 1;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    NMP_VERIFY(sampleCounts[i] > 1);
    numVertexSamples *= sampleCounts[i];
    numGridCells *= (sampleCounts[i] - 1);
  }

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleMapCPSampleExportData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Control parameters
  NMP::Memory::Format memReqsCPSampleTable(sizeof(float*) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCPSampleTable;

  NMP::Memory::Format memReqsSamples(sizeof(float) * numVertexSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsSamples * numComponentsPerSample);

  // Grid cell flags
  NMP::Memory::Format memReqsFlags(sizeof(bool) * numGridCells, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsFlags;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapCPSampleExportData* NDMeshResampleMapCPSampleExportData::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  const uint32_t* sampleCounts,
  uint32_t numComponentsPerSample)
{
  NMP_VERIFY(numDimensions > 0);
  NMP_VERIFY(sampleCounts);
  NMP_VERIFY(numComponentsPerSample > 0);

  uint32_t numVertexSamples = 1;
  uint32_t numGridCells = 1;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    NMP_VERIFY(sampleCounts[i] > 1);
    numVertexSamples *= sampleCounts[i];
    numGridCells *= (sampleCounts[i] - 1);
  }

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleMapCPSampleExportData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleMapCPSampleExportData* result = (NDMeshResampleMapCPSampleExportData*)resource.alignAndIncrement(memReqsHdr);
  result->m_numDimensions = numDimensions;
  result->m_numComponentsPerSample = numComponentsPerSample;

  // Control parameters
  NMP::Memory::Format memReqsCPSampleTable(sizeof(float*) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_vertexCPSamples = (float**)resource.alignAndIncrement(memReqsCPSampleTable);

  NMP::Memory::Format memReqsSamples(sizeof(float) * numVertexSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    result->m_vertexCPSamples[i] = (float*)resource.alignAndIncrement(memReqsSamples);
  }

  // Grid cell flags
  NMP::Memory::Format memReqsFlags(sizeof(bool) * numGridCells, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_cellFlags = (bool*)resource.alignAndIncrement(memReqsFlags);

  // Multiple of the data alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapCPSampleExportData* NDMeshResampleMapCPSampleExportData::create(
  uint32_t numDimensions,
  const uint32_t* sampleCounts,
  uint32_t numComponentsPerSample)
{
  // Allocate the memory for the resample grid
  NMP::Memory::Format memReqs = NDMeshResampleMapCPSampleExportData::getMemoryRequirements(
    numDimensions,
    sampleCounts,
    numComponentsPerSample);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshResampleMapCPSampleExportData* result = NDMeshResampleMapCPSampleExportData::init(
    memRes,
    numDimensions,
    sampleCounts,
    numComponentsPerSample);
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleMapCPSampleExportData did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  return result;
}


//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapCPSampleQuantisationData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleMapCPSampleQuantisationData::getMemoryRequirements(
  const NDMeshResampleMapGridExportData* interpolationGrid,
  uint32_t numBitsPerSample)
{
  NMP_VERIFY(interpolationGrid);
  uint32_t numDimensions = interpolationGrid->getNumDimensions();
  uint32_t numVertexSamples = interpolationGrid->getNumVertexSamples();

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleMapCPSampleQuantisationData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Quantisation ranges
  NMP::Memory::Format memReqsQInfo(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsQInfo * 4);

  // Quantised vertex samples
  NMP::Memory::Format memReqsVertexData = MR::PackedArrayUInt32::getMemoryRequirements(
    numVertexSamples * numDimensions,
    numBitsPerSample);
  result += memReqsVertexData;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapCPSampleQuantisationData* NDMeshResampleMapCPSampleQuantisationData::init(
  NMP::Memory::Resource& resource,
  const NDMeshResampleMapGridExportData* interpolationGrid,
  uint32_t numBitsPerSample)
{
  NMP_VERIFY(interpolationGrid);
  uint32_t numDimensions = interpolationGrid->getNumDimensions();
  uint32_t numVertexSamples = interpolationGrid->getNumVertexSamples();

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleMapCPSampleQuantisationData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleMapCPSampleQuantisationData* result =
    (NDMeshResampleMapCPSampleQuantisationData*)resource.alignAndIncrement(memReqsHdr);

  result->m_numDimensions = numDimensions;
  result->m_numVertexSamples = numVertexSamples;

  // Quantisation ranges
  NMP::Memory::Format memReqsQInfo(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_qMinValues = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_qMaxValues = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_stepSizes = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_recipStepSizes = (float*)resource.alignAndIncrement(memReqsQInfo);

  // Quantised vertex samples
  result->m_vertexCPSamples = MR::PackedArrayUInt32::init(
    resource,
    numVertexSamples * numDimensions,
    numBitsPerSample);

  // Multiple of the data alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapCPSampleQuantisationData* NDMeshResampleMapCPSampleQuantisationData::create(
  const NDMeshResampleMapGridExportData* interpolationGrid,
  const NDMeshResampleMapCPSampleExportData* interpolationData,
  uint32_t numBitsPerSample)
{
  NMP_VERIFY(interpolationData);
  uint32_t numDimensions = interpolationGrid->getNumDimensions();
  uint32_t numVertexSamples = interpolationGrid->getNumVertexSamples();

  //------------------------------
  // Allocate the memory
  NMP::Memory::Format memReqs = NDMeshResampleMapCPSampleQuantisationData::getMemoryRequirements(
    interpolationGrid,
    numBitsPerSample);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampleMapCPSampleQuantisationData* result = NDMeshResampleMapCPSampleQuantisationData::init(
    memRes,
    interpolationGrid,
    numBitsPerSample);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "The NDMeshResampleMapCPSampleQuantisationData did not initialise all the allocated memory: %d bytes remain.",
    memRes.format.size);

  //------------------------------
  // Compute the quantisation ranges
  const float* const* vertexCPSamples = interpolationData->getVertexCPSamples();
  NMP_VERIFY(vertexCPSamples);

  for (uint32_t cpIndex = 0; cpIndex < numDimensions; ++cpIndex)
  {
    result->m_qMinValues[cpIndex] = FLT_MAX;
    result->m_qMaxValues[cpIndex] = -FLT_MAX;

    for (uint32_t vertex = 0; vertex < numVertexSamples; ++vertex)
    {
      float cpValue = vertexCPSamples[cpIndex][vertex];
      result->m_qMinValues[cpIndex] = NMP::minimum(result->m_qMinValues[cpIndex], cpValue);
      result->m_qMaxValues[cpIndex] = NMP::maximum(result->m_qMaxValues[cpIndex], cpValue);
    }
  }

  // Compute the quantisation step size for N-bit samples
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    result->m_stepSizes[i] = NMP::UniformQuantisation::stepSize(
      result->m_qMinValues[i],
      result->m_qMaxValues[i],
      numBitsPerSample);
    result->m_recipStepSizes[i] = NMP::UniformQuantisation::recipStepSize(
      result->m_qMinValues[i],
      result->m_qMaxValues[i],
      numBitsPerSample);
  }

  //------------------------------
  // Quantise the vertex CP samples
  uint32_t sampleIndex = 0;
  for (uint32_t vertex = 0; vertex < numVertexSamples; ++vertex)
  {
    for (uint32_t cpIndex = 0; cpIndex < numDimensions; ++cpIndex)
    {
      float cpValue = vertexCPSamples[cpIndex][vertex];
      uint32_t qValue = NMP::UniformQuantisation::quantise(
        result->m_qMinValues[cpIndex],
        result->m_qMaxValues[cpIndex],
        result->m_recipStepSizes[cpIndex],
        cpValue);

      result->m_vertexCPSamples->setEntry(sampleIndex, qValue);

      sampleIndex++;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapCPSampleQuantisationData::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}


//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapProjectionExportData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleMapProjectionExportData::getMemoryRequirements(
  uint32_t numCellSamples,
  uint32_t numEntries)
{
  NMP_VERIFY(numCellSamples > 0);
  NMP_VERIFY(numEntries > 0);

  NMP::Memory::Format result(sizeof(NDMeshResampleMapProjectionExportData), NMP_NATURAL_TYPE_ALIGNMENT);

  NMP::Memory::Format memReqsCEC(sizeof(uint32_t) * numCellSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCEC;

  NMP::Memory::Format memReqsCE(sizeof(uint32_t) * numEntries);
  result += (memReqsCE * 3);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapProjectionExportData* NDMeshResampleMapProjectionExportData::init(
  NMP::Memory::Resource& resource,
  uint32_t numCellSamples,
  uint32_t numEntries)
{
  NMP_VERIFY(numCellSamples > 0);
  NMP_VERIFY(numEntries > 0);

  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleMapProjectionExportData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleMapProjectionExportData* result = (NDMeshResampleMapProjectionExportData*)resource.alignAndIncrement(memReqsHdr);
  
  result->m_numCellSamples = numCellSamples;
  result->m_numEntries = numEntries;

  NMP::Memory::Format memReqsCEC(sizeof(uint32_t) * numCellSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_cellEntryCounts = (uint32_t*)resource.alignAndIncrement(memReqsCEC);

  NMP::Memory::Format memReqsCE(sizeof(uint32_t) * numEntries);
  result->m_entriesCA = (uint32_t*)resource.alignAndIncrement(memReqsCE);
  result->m_entriesWD = (uint32_t*)resource.alignAndIncrement(memReqsCE);
  result->m_entriesWB = (uint32_t*)resource.alignAndIncrement(memReqsCE);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapProjectionExportData* NDMeshResampleMapProjectionExportData::create(
  uint32_t numCellSamples,
  uint32_t numEntries)
{
  // Allocate the memory for the resample grid
  NMP::Memory::Format memReqs = NDMeshResampleMapProjectionExportData::getMemoryRequirements(
    numCellSamples,
    numEntries);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshResampleMapProjectionExportData* result = NDMeshResampleMapProjectionExportData::init(
    memRes,
    numCellSamples,
    numEntries);
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleMapProjectionExportData did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapProjectionExportData::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleGridExtrapolationData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleGridExtrapolationData::getMemoryRequirements(
  uint32_t numDimensions,
  uint32_t maxNumEntries)
{
  NMP_VERIFY(numDimensions > 0);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleGridExtrapolationData), NMP_NATURAL_TYPE_ALIGNMENT);

  if (maxNumEntries > 0)
  {
    // CP component entry table
    NMP::Memory::Format memReqsTable(sizeof(float*) * maxNumEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsTable;

    // CP components
    NMP::Memory::Format memReqsCPs(sizeof(float) * (numDimensions * maxNumEntries), NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsCPs; // CPs

    // Entry weights
    NMP::Memory::Format memReqsWs(sizeof(float) * maxNumEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsWs * 2);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleGridExtrapolationData* NDMeshResampleGridExtrapolationData::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  uint32_t maxNumEntries)
{
  NMP_VERIFY(numDimensions > 0);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleGridExtrapolationData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleGridExtrapolationData* result = (NDMeshResampleGridExtrapolationData*)resource.alignAndIncrement(memReqsHdr);

  result->m_numDimensions = numDimensions;
  result->m_maxNumEntries = maxNumEntries;
  result->m_numEntries = 0;
  result->m_cpComponentEntries = NULL;
  result->m_costEntries = NULL;
  result->m_weightEntries = NULL;

  if (maxNumEntries > 0)
  {
    // CP component entry table
    NMP::Memory::Format memReqsTable(sizeof(float*) * maxNumEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_cpComponentEntries = (float**)resource.alignAndIncrement(memReqsTable);

    // CP components
    NMP::Memory::Format memReqsCPs(sizeof(float) * (numDimensions * maxNumEntries), NMP_NATURAL_TYPE_ALIGNMENT);
    float* cpData = (float*)resource.alignAndIncrement(memReqsCPs);
    uint32_t offset = 0;
    for (uint32_t i = 0; i < maxNumEntries; ++i, offset += numDimensions)
    {
      result->m_cpComponentEntries[i] = &cpData[offset];
    }

    // Entry weights
    NMP::Memory::Format memReqsWs(sizeof(float) * maxNumEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_costEntries = (float*)resource.alignAndIncrement(memReqsWs);
    result->m_weightEntries = (float*)resource.alignAndIncrement(memReqsWs);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleGridExtrapolationData* NDMeshResampleGridExtrapolationData::create(
  uint32_t numDimensions,
  uint32_t maxNumEntries)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(numDimensions, maxNumEntries);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampleGridExtrapolationData* result = init(memRes, numDimensions, maxNumEntries);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleGridExtrapolationData did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleGridExtrapolationData::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleGridExtrapolationData::reset()
{
  m_numEntries = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleGridExtrapolationData::addEntry(
  const float* cpComponents,
  const float* ts)
{
  NMP_VERIFY(cpComponents);
  NMP_VERIFY(ts);
  NMP_VERIFY(m_numEntries < m_maxNumEntries);

  float* cpComponentsDst = m_cpComponentEntries[m_numEntries];

  // Store the components
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    cpComponentsDst[i] = cpComponents[i];
  }

  // Calculate the cost entry
  float cost = 0.0f;
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    // Residual error outside the bound
    float w = 0.0f;
    if (ts[i] < 0.0f)
    {
      w = -ts[i];
    }
    else if (ts[i] > 1.0f)
    {
      w = ts[i] - 1.0f;
    }

    // Update the weight
    cost += w;
  }
  m_costEntries[m_numEntries] = cost;

  // Update the entry count
  m_numEntries++;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleGridExtrapolationData::computeWeightedAverage(float* cpComponents)
{
  NMP_VERIFY(cpComponents);
  NMP_VERIFY(m_numEntries > 0);

  if (m_numEntries == 1)
  {
    const float* cps = m_cpComponentEntries[0];
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      cpComponents[i] = cps[i];
    }
  }
  else
  {
    //---------------------------
    // Compute the weight entries
    float sum = 0.0f;
    for (uint32_t entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
    {
      sum += m_costEntries[entryIndex];
    }
    float recipAv = (float)m_numEntries / sum;

    // Calculate the entry weights
    sum = 0.0f;
    for (uint32_t entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
    {
      float u = m_costEntries[entryIndex] * recipAv;
      m_weightEntries[entryIndex] = expf(-u * u);
      sum += m_weightEntries[entryIndex];
    }
    float normFac = 1.0f / sum;

    // Normalise to get a PDF
    for (uint32_t entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
    {
      m_weightEntries[entryIndex] *= normFac;
    }

    //---------------------------
    // Compute the weighted average CP values
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      cpComponents[i] = 0.0f;
      for (uint32_t entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
      {
        const float* cps = m_cpComponentEntries[entryIndex];
        cpComponents[i] += (cps[i] * m_weightEntries[entryIndex]);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleGridInterpolationData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleGridInterpolationData::getMemoryRequirements(
  uint32_t numDimensions,
  uint32_t numVertexSamples,
  uint32_t numGridCells)
{
  NMP_VERIFY(numDimensions > 0);
  NMP_VERIFY(numVertexSamples > 0);
  NMP_VERIFY(numGridCells > 0);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleGridInterpolationData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Per resample vertex compilation information
  NMP::Memory::Format memReqsVertexBins(sizeof(VertexBinEntry) * numVertexSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsVertexBins;

  // Per resample cell compilation information
  NMP::Memory::Format memReqsCellBins(sizeof(GridCellBinEntry) * numGridCells, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCellBins;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleGridInterpolationData* NDMeshResampleGridInterpolationData::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  uint32_t numVertexSamples,
  uint32_t numGridCells)
{
  NMP_VERIFY(numDimensions > 0);
  NMP_VERIFY(numVertexSamples > 0);
  NMP_VERIFY(numGridCells > 0);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleGridInterpolationData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleGridInterpolationData* result = (NDMeshResampleGridInterpolationData*)resource.alignAndIncrement(memReqsHdr);

  result->m_numDimensions = numDimensions;
  result->m_numVertexSamples = numVertexSamples;
  result->m_numGridCells = numGridCells;

  // Per resample vertex compilation information
  NMP::Memory::Format memReqsVertexBins(sizeof(VertexBinEntry) * numVertexSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_vertexData = (VertexBinEntry*)resource.alignAndIncrement(memReqsVertexBins);
  for (uint32_t i = 0; i < numVertexSamples; ++i)
  {
    VertexBinEntry* entry = &(result->m_vertexData[i]);
    entry->m_vertexValid = false;
    entry->m_numGridCellEntries = 0;
    entry->m_gridCellAddresses = NULL;
  }

  // Per resample cell compilation information
  NMP::Memory::Format memReqsCellBins(sizeof(GridCellBinEntry) * numGridCells, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_gridCellData = (GridCellBinEntry*)resource.alignAndIncrement(memReqsCellBins);
  for (uint32_t i = 0; i < numGridCells; ++i)
  {
    GridCellBinEntry* entry = &(result->m_gridCellData[i]);
    entry->m_numNodeEntries = 0;
    entry->m_nodeEntries = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleGridInterpolationData* NDMeshResampleGridInterpolationData::create(
  uint32_t numDimensions,
  uint32_t numVertexSamples,
  uint32_t numGridCells)
{
  NMP::Memory::Format memReqs = NDMeshResampleGridInterpolationData::getMemoryRequirements(
    numDimensions,
    numVertexSamples,
    numGridCells);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampleGridInterpolationData* result = NDMeshResampleGridInterpolationData::init(
    memRes,
    numDimensions,
    numVertexSamples,
    numGridCells);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleGridInterpolationData did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleGridInterpolationData::releaseAndDestroy()
{
  // Tidy up the resample vertex bin data
  for (uint32_t i = 0; i < m_numVertexSamples; ++i)
  {
    if (m_vertexData[i].m_gridCellAddresses)
    {
      NMP::Memory::memFree(m_vertexData[i].m_gridCellAddresses);
    }
  }

  // Tidy up the grid cell bin data
  for (uint32_t i = 0; i < m_numGridCells; ++i)
  {
    if (m_gridCellData[i].m_nodeEntries)
    {
      NMP::Memory::memFree(m_gridCellData[i].m_nodeEntries);
    }
  }

  // Delete the memory for this container
  NMP::Memory::memFree(this);
}


//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleGridProjectionData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleGridProjectionData::getMemoryRequirements(
  uint32_t numDimensions,
  uint32_t numGridCells)
{
  NMP_VERIFY(numDimensions > 0);
  NMP_VERIFY(numGridCells > 0);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleGridProjectionData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Per resample cell compilation information
  NMP::Memory::Format memReqsCellBins(sizeof(GridCellBinEntry) * numGridCells, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCellBins;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleGridProjectionData* NDMeshResampleGridProjectionData::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  uint32_t numGridCells)
{
  NMP_VERIFY(numDimensions > 0);
  NMP_VERIFY(numGridCells > 0);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleGridProjectionData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleGridProjectionData* result = (NDMeshResampleGridProjectionData*)resource.alignAndIncrement(memReqsHdr);

  result->m_numDimensions = numDimensions;
  result->m_numGridCells = numGridCells;

  // Per resample cell compilation information
  NMP::Memory::Format memReqsCellBins(sizeof(GridCellBinEntry) * numGridCells, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_gridCellData = (GridCellBinEntry*)resource.alignAndIncrement(memReqsCellBins);
  for (uint32_t i = 0; i < numGridCells; ++i)
  {
    GridCellBinEntry* entry = &(result->m_gridCellData[i]);
    entry->m_numNodeEntries = 0;
    entry->m_nodeEntries = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleGridProjectionData* NDMeshResampleGridProjectionData::create(
  uint32_t numDimensions,
  uint32_t numGridCells)
{
  NMP::Memory::Format memReqs = NDMeshResampleGridProjectionData::getMemoryRequirements(
    numDimensions,
    numGridCells);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampleGridProjectionData* result = NDMeshResampleGridProjectionData::init(
    memRes,
    numDimensions,
    numGridCells);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleGridProjectionData did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleGridProjectionData::releaseAndDestroy()
{
  // Tidy up the grid cell bin data
  for (uint32_t i = 0; i < m_numGridCells; ++i)
  {
    if (m_gridCellData[i].m_nodeEntries)
    {
      NMP::Memory::memFree(m_gridCellData[i].m_nodeEntries);
    }
  }

  // Delete the memory for this container
  NMP::Memory::memFree(this);
}


//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampleMapExportData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampleMapExportData::getMemoryRequirements(
  uint32_t numDimensions,
  uint32_t numAPQueryComplementComponentIndices,
  const uint32_t* sampleCounts)
{
  NMP_VERIFY_MSG(
    numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM,
    "NDMeshResampleMapExportData requires one or more dimensions");
  NMP_VERIFY(numAPQueryComplementComponentIndices > 0 && numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);
  NMP_VERIFY(sampleCounts);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampleMapExportData), NMP_NATURAL_TYPE_ALIGNMENT);

  // AP query information
  NMP::Memory::Format memReqsComponents(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsComponents;

  NMP::Memory::Format memReqsAPComplement(sizeof(uint32_t) * numAPQueryComplementComponentIndices, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsAPComplement;

  //------------------------------
  // Interpolation grid data
  NMP::Memory::Format memReqsInterpGrid = NDMeshResampleMapGridExportData::getMemoryRequirements(
    numDimensions,
    sampleCounts);
  result += memReqsInterpGrid;

  NMP::Memory::Format memReqsInterpData = NDMeshResampleMapCPSampleExportData::getMemoryRequirements(
    numDimensions,
    sampleCounts,
    numDimensions);
  result += memReqsInterpData;

  //------------------------------
  // Projection grid data
  uint32_t M = numDimensions - 1;
  if (M > 0)
  {
    //------------------------------
    // Projection data pointer tables
    NMP::Memory::Format memReqsProjGridTable(sizeof(NDMeshResampleMapGridExportData*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsProjGridTable;
    NMP::Memory::Format memReqsProjDataTable(sizeof(NDMeshResampleMapProjectionExportData*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsProjDataTable * 3); // projectAlongDimension + (projectThroughPoint * 2)
 
    //------------------------------
    // Projection grid cell sampling
    uint32_t sampleCountsPrj[SCATTERED_DATA_MAX_DIM];
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      // Build the sample count array
      uint32_t index = 0;
      for (uint32_t i = 0; i < numDimensions; ++i)
      {
        if (i != dim)
        {
          sampleCountsPrj[index] = sampleCounts[i];
          index++;
        }
      }

      // Projection query resample map
      NMP::Memory::Format memReqsProjGrid = NDMeshResampleMapGridExportData::getMemoryRequirements(
        M,
        sampleCountsPrj);
      result += memReqsProjGrid;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapExportData* NDMeshResampleMapExportData::init(
  NMP::Memory::Resource& resource,
  uint32_t numDimensions,
  uint32_t numAPQueryComplementComponentIndices,
  const uint32_t* sampleCounts,
  uint32_t numBitsPerSample,
  bool buildProjectThroughSampleCentreMap)
{
  NMP_VERIFY_MSG(
    numDimensions > 0 && numDimensions <= SCATTERED_DATA_MAX_DIM,
    "NDMeshResampleMapExportData requires one or more dimensions");
  NMP_VERIFY(numAPQueryComplementComponentIndices > 0 && numAPQueryComplementComponentIndices <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);
  NMP_VERIFY(sampleCounts);

  uint32_t whichComponents[SCATTERED_DATA_MAX_DIM];

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleMapExportData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshResampleMapExportData* result = (NDMeshResampleMapExportData*)resource.alignAndIncrement(memReqsHdr);
  result->m_numDimensions = numDimensions;
  result->m_numAPQueryComplementComponentIndices = numAPQueryComplementComponentIndices;

  // Query information
  NMP::Memory::Format memReqsComponents(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apQueryComponentIndices = (uint32_t*)resource.alignAndIncrement(memReqsComponents);

  NMP::Memory::Format memReqsAPComplement(sizeof(uint32_t) * numAPQueryComplementComponentIndices, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_apQueryComplementComponentIndices = (uint32_t*)resource.alignAndIncrement(memReqsAPComplement);

  //------------------------------
  // Interpolation grid data
  result->m_interpolationGrid = NDMeshResampleMapGridExportData::init(
    resource,
    numDimensions,
    sampleCounts);

  result->m_interpolationData = NDMeshResampleMapCPSampleExportData::init(
    resource,
    numDimensions,
    sampleCounts,
    numDimensions);

  result->m_numBitsPerSample = numBitsPerSample;
  result->m_interpolationQuantisationData = NULL;

  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    whichComponents[i] = i;
  }
  result->m_interpolationGrid->setWhichQueryComponentIndices(numDimensions, whichComponents);

  //------------------------------
  // Projection query resample maps
  uint32_t M = numDimensions - 1;
  if (M > 0)
  {
    result->m_buildProjectThroughSampleCentreMap = buildProjectThroughSampleCentreMap;

    //------------------------------
    // Projection data pointer tables
    NMP::Memory::Format memReqsProjGridTable(sizeof(NDMeshResampleMapGridExportData*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_projectionGrids = (NDMeshResampleMapGridExportData**)resource.alignAndIncrement(memReqsProjGridTable);

    NMP::Memory::Format memReqsProjDataTable(sizeof(NDMeshResampleMapProjectionExportData*) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_projectAlongDimensionData = (NDMeshResampleMapProjectionExportData**)resource.alignAndIncrement(memReqsProjDataTable);
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      result->m_projectAlongDimensionData[i] = NULL;
    }

    result->m_projectThroughPointData[0] = (NDMeshResampleMapProjectionExportData**)resource.alignAndIncrement(memReqsProjDataTable);
    result->m_projectThroughPointData[1] = (NDMeshResampleMapProjectionExportData**)resource.alignAndIncrement(memReqsProjDataTable);
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      result->m_projectThroughPointData[0][i] = NULL;
      result->m_projectThroughPointData[1][i] = NULL;
    }

    //------------------------------
    // Projection grid cell sampling
    uint32_t sampleCountsPrj[SCATTERED_DATA_MAX_DIM];
    for (uint32_t dim = 0; dim < numDimensions; ++dim)
    {
      // Build the sample count array
      uint32_t index = 0;
      for (uint32_t i = 0; i < numDimensions; ++i)
      {
        if (i != dim)
        {
          whichComponents[index] = i;
          sampleCountsPrj[index] = sampleCounts[i];
          index++;
        }
      }

      // Projection query resample map
      result->m_projectionGrids[dim] = NDMeshResampleMapGridExportData::init(
        resource,
        M,
        sampleCountsPrj);

      result->m_projectionGrids[dim]->setWhichQueryComponentIndices(M, whichComponents);
    }
  }
  else
  {
    result->m_buildProjectThroughSampleCentreMap = false;
    result->m_projectionGrids = NULL;
    result->m_projectAlongDimensionData = NULL;
    result->m_projectThroughPointData[0] = NULL;
    result->m_projectThroughPointData[1] = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapExportData* NDMeshResampleMapExportData::createForAnalysis(
  const ME::AnalysisNodeExport* resampleGridExport,
  const ME::AnalysisNodeExport* ndMeshNodeExport)
{
  NMP_VERIFY(resampleGridExport);
  const ME::DataBlockExport* resampleGridExportDataBlock = resampleGridExport->getDataBlock();
  NMP_VERIFY(resampleGridExportDataBlock);
  NMP_VERIFY(ndMeshNodeExport);
  const ME::DataBlockExport* ndMeshNodeExportDataBlock = ndMeshNodeExport->getDataBlock();
  NMP_VERIFY(ndMeshNodeExportDataBlock);
  bool status;

  //------------------------------
  // General information
  uint32_t numDimensions = 0;
  status = resampleGridExportDataBlock->readUInt(numDimensions, "NumDimensions");
  NMP_VERIFY_MSG(
    status,
    "Could not find NumDimensions attribute in export");

  NMP_VERIFY_MSG(
    numDimensions > 0,
    "You can't build an inverse AP Resample map for a zero dimension prediction model.");

  uint32_t analysisPropertyCount = 0;
  status = ndMeshNodeExportDataBlock->readUInt(analysisPropertyCount, "AnalysisPropertyCount");
  NMP_VERIFY_MSG(
    status,
    "Could not find AnalysisPropertyCount attribute in the export");
  NMP_VERIFY_MSG(
    analysisPropertyCount > 0,
    "Could not find any analysis properties");

  // Sample counts
  NMP_VERIFY(numDimensions < 8);
  uint32_t sampleCounts[8];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    char attrName[256];
    sprintf_s(attrName, 256 - 1, "NumSamples%d", i);
    status = resampleGridExportDataBlock->readUInt(sampleCounts[i], attrName);
    NMP_VERIFY_MSG(
      status,
      "Could not find %s attribute in export",
      attrName);
  }

  //------------------------------
  // Options
  uint32_t numBitsPerSample = 10;
  status = resampleGridExportDataBlock->readUInt(numBitsPerSample, "NumBitsPerSample");
  NMP_VERIFY_MSG(
    status,
    "Could not find NumBitsPerSample attribute in the export");

  bool buildProjectThroughSampleCentreMap = false;
  status = resampleGridExportDataBlock->readBool(buildProjectThroughSampleCentreMap, "BuildProjectThroughSampleCentreMap");
  NMP_VERIFY_MSG(
    status,
    "Could not find BuildProjectThroughSampleCentreMap attribute in the export");

  //------------------------------
  // Allocate the memory for the resample grid
  NMP::Memory::Format memReqs = NDMeshResampleMapExportData::getMemoryRequirements(
    numDimensions,
    analysisPropertyCount,
    sampleCounts);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshResampleMapExportData* resampleGrid = NDMeshResampleMapExportData::init(
    memRes,
    numDimensions,
    analysisPropertyCount,
    sampleCounts,
    numBitsPerSample,
    buildProjectThroughSampleCentreMap);
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleGrid did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  //------------------------------
  // Find the indices of the APs in the NDMesh node corresponding to the connected
  // analysis properties to the resample grid node
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    // Analysis property connected to the resample grid
    char attrName[256];
    sprintf_s(attrName, 256 - 1, "AnalysisProperty%d", i);
    std::string analysisPropertyPath;
    status = resampleGridExportDataBlock->readString(analysisPropertyPath, attrName);
    NMP_VERIFY_MSG(
      status,
      "Could not find %s attribute in export",
      attrName);

    // Find the corresponding AP index of the connection to the NDMesh node
    uint32_t apIndex = findNDMeshNodeAPIndex(ndMeshNodeExport, analysisPropertyPath.c_str());
    NMP_VERIFY_MSG(
      apIndex != 0xFFFFFFFF,
      "Analysis property %s connected to the resample grid was not also connected to the NDMeshNode",
      analysisPropertyPath.c_str());

    // NDMesh stores control parameter samples before analysis properties
    uint32_t independentVarIndex = numDimensions + apIndex;
    resampleGrid->setAPQueryComponentIndex(i, independentVarIndex);
  }
  resampleGrid->calculateAPQueryComplementComponentIndices();

  return resampleGrid;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapExportData* NDMeshResampleMapExportData::createForAnalysisUsingDefaults(
  const ME::AnalysisNodeExport* ndMeshNodeExport,
  const ControlParameterRegularSampleGrid* regularSampleGrid)
{
  NMP_VERIFY(ndMeshNodeExport);
  const ME::DataBlockExport* ndMeshNodeExportDataBlock = ndMeshNodeExport->getDataBlock();
  NMP_VERIFY(regularSampleGrid);
  bool status;

  //------------------------------
  // Options
  uint32_t numDimensions = regularSampleGrid->getNumControlParameters();
  const uint32_t* regularSampleGridCounts = regularSampleGrid->getSampleCounts();

  uint32_t analysisPropertyCount = 0;
  status = ndMeshNodeExportDataBlock->readUInt(analysisPropertyCount, "AnalysisPropertyCount");
  NMP_VERIFY_MSG(
    status,
    "Could not find AnalysisPropertyCount attribute in the export");
  NMP_VERIFY_MSG(
    analysisPropertyCount > 0,
    "Could not find any analysis properties");

  uint32_t numBitsPerSample = 10;
  status = ndMeshNodeExportDataBlock->readUInt(numBitsPerSample, "BuildAPResampleMapNumBitsPerSample");
  NMP_VERIFY_MSG(
    status,
    "Could not find BuildAPResampleMapNumBitsPerSample attribute in the export");

  bool buildAPResampleMapProjectThroughSampleCentre = false;
  status = ndMeshNodeExportDataBlock->readBool(buildAPResampleMapProjectThroughSampleCentre, "BuildAPResampleMapProjectThroughSampleCentre");
  NMP_VERIFY_MSG(
    status,
    "Could not find BuildAPResampleMapProjectThroughSampleCentre attribute in the export");

  float buildAPResampleMapSampleFactor = 2.0f;
  status = ndMeshNodeExportDataBlock->readFloat(buildAPResampleMapSampleFactor, "BuildAPResampleMapSampleFactor");
  NMP_VERIFY_MSG(
    status,
    "Could not find BuildAPResampleMapSampleFactor attribute in the export");

  // Compute the sample counts for the resample grid
  uint32_t sampleCounts[8];
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    sampleCounts[i] = (uint32_t)(buildAPResampleMapSampleFactor * regularSampleGridCounts[i] + 0.5f);
    NMP_VERIFY(sampleCounts[i] >= 2);
  }

  //------------------------------
  // Allocate the memory for the resample grid
  NMP::Memory::Format memReqs = NDMeshResampleMapExportData::getMemoryRequirements(
    numDimensions,
    analysisPropertyCount,
    sampleCounts);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshResampleMapExportData* resampleGrid = NDMeshResampleMapExportData::init(
    memRes,
    numDimensions,
    analysisPropertyCount,
    sampleCounts,
    numBitsPerSample,
    buildAPResampleMapProjectThroughSampleCentre);
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleGrid did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  //------------------------------
  // Set the indices of the APs in the NDMesh node corresponding to the first
  // numDimensions connected analysis properties
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    uint32_t independentVarIndex = numDimensions + i;
    resampleGrid->setAPQueryComponentIndex(i, independentVarIndex);
  }
  resampleGrid->calculateAPQueryComplementComponentIndices();

  return resampleGrid;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapExportData* NDMeshResampleMapExportData::createForPreview(
  const ME::AnalysisNodeExport* ndMeshNodeExport,
  uint32_t resampleMapIndex,
  MR::AnimSetIndex animSetIndex)
{
  NMP_VERIFY(ndMeshNodeExport);
  const ME::DataBlockExport* ndMeshNodeExportDataBlock = ndMeshNodeExport->getDataBlock();
  NMP_VERIFY(ndMeshNodeExportDataBlock);

  // Non anim set data
  uint32_t numDimensions;
  uint32_t numAnalysisProperties;
  uint32_t numBitsPerSample;
  bool buildProjectThroughSampleCentreMap;
  uint32_t sampleCounts[SCATTERED_DATA_MAX_DIM];
  uint32_t apQueryComponentIndices[SCATTERED_DATA_MAX_DIM];

  NDMeshResampleMapExportData::readExportData(
    resampleMapIndex,
    ndMeshNodeExportDataBlock,
    SCATTERED_DATA_MAX_DIM,
    numDimensions,
    numAnalysisProperties,
    numBitsPerSample,
    buildProjectThroughSampleCentreMap,
    sampleCounts,
    apQueryComponentIndices);

  // Allocate the memory for the resample grid
  NMP::Memory::Format memReqs = NDMeshResampleMapExportData::getMemoryRequirements(
    numDimensions,
    numAnalysisProperties,
    sampleCounts);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshResampleMapExportData* result = NDMeshResampleMapExportData::init(
    memRes,
    numDimensions,
    numAnalysisProperties,
    sampleCounts,
    numBitsPerSample,
    buildProjectThroughSampleCentreMap);
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "NDMeshResampleGrid did not use all of the requested memory: %d bytes remain",
    memRes.format.size);

  // Query component indices
  result->setAPQueryComponentIndices(numDimensions, apQueryComponentIndices);
  result->calculateAPQueryComplementComponentIndices();

  // Anim set data
  result->readPerSetExportData(
    resampleMapIndex,
    animSetIndex,
    ndMeshNodeExportDataBlock);

  // Compile the quantisation data
  result->compileQuantisationData();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NDMeshResampleMapExportData::findNDMeshNodeAPIndex(
  const ME::AnalysisNodeExport* ndMeshNodeExport,
  const char* apPathIn)
{
  NMP_VERIFY(ndMeshNodeExport);
  NMP_VERIFY(apPathIn);
  const ME::DataBlockExport* analysisNodeDataBlock = ndMeshNodeExport->getDataBlock();
  bool status;

  uint32_t analysisPropertyCount = 0;
  status = analysisNodeDataBlock->readUInt(analysisPropertyCount, "AnalysisPropertyCount");
  NMP_VERIFY_MSG(
    status,
    "Could not find AnalysisPropertyCount attribute in the export");
  NMP_VERIFY_MSG(
    analysisPropertyCount > 0,
    "Could not find any analysis properties");

  for (uint32_t i = 0; i < analysisPropertyCount; ++i)
  {
    char attrName[256];
    sprintf_s(attrName, 256 - 1, "AnalysisProperty%d", i);

    std::string analysisPropertyPath;
    status = analysisNodeDataBlock->readString(analysisPropertyPath, attrName);
    NMP_VERIFY_MSG(
      status,
      "Could not find %s attribute in the export",
      attrName);

    if (analysisPropertyPath == apPathIn)
    {
      return i;
    }
  }

  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::releaseAndDestroy()
{
  tidyPerSetExportData();

  // Free up the header data
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateInterpolationGridVertexSamples(
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridInterpolationData* tempData)
{
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);
  const MR::ScatteredData::NDMeshAPSearchMap* apSearchMap = ndMeshModel->getAPSearchMap();
  NMP_VERIFY_MSG(
    apSearchMap,
    "The NDMeshModel must be built with an APSearch map to perform resampling analysis");

  // Get the sample centre
  float sampleCentre[SCATTERED_DATA_MAX_DIM];
  nDMesh->getSampleCentre(m_numDimensions, m_apQueryComponentIndices, sampleCentre);

  const uint32_t* sampleCountsPerDimension = m_interpolationGrid->getSampleCountsPerDimension();
  MR::ScatteredData::InterpolationReturnVal resultStatus;
  float* const* vertexCPSamples = m_interpolationData->getVertexCPSamples();

  // Get the indices of the control parameter components
  float cpComponentValues[SCATTERED_DATA_MAX_DIM];
  uint32_t cpComponentIndices[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    cpComponentIndices[i] = i;
  }

  // Initialise the resampling state
  bool statusFlag = true;
  uint32_t coordinate[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    coordinate[i] = 0;
  }

  // Resample the analysis property space
  while (statusFlag)
  {
    // Convert the Nd coordinate to a flat sample address
    uint32_t vertexAddress = m_interpolationGrid->coordinateToVertexAddress(coordinate);

    // Get the resample grid vertex point
    float queryPoint[SCATTERED_DATA_MAX_DIM];
    m_interpolationGrid->getGridVertexFromCoordinate(coordinate, queryPoint);

    // Set the output CP values to the sample centre in case the query fails. When we
    // quantise the data this ensures that the range is not affected by any invalid
    // vertex samples
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      cpComponentValues[i] = sampleCentre[i];
    }

    // Compute the control parameters for the query point (Don't perform projection)
    MR::ScatteredData::NDMeshQueryResult ndMeshQueryResult;
    resultStatus = apSearchMap->mapFromAPs(
      nDMesh,
      queryPoint, // IN / OUT
      m_apQueryComponentIndices,
      &ndMeshQueryResult,
      m_numDimensions,
      cpComponentIndices,
      cpComponentValues, // OUT
      false); // No projection

    // Set the valid flags
    NDMeshResampleGridInterpolationData::VertexBinEntry* entry = &(tempData->m_vertexData[vertexAddress]);
    entry->m_vertexValid = (resultStatus == MR::ScatteredData::INTERPOLATION_SUCCESS);

    // Set the resampled CP values
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      vertexCPSamples[i][vertexAddress] = cpComponentValues[i];
    }

    // Update the sampling values
    statusFlag = false;
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      coordinate[i]++;
      if (coordinate[i] < sampleCountsPerDimension[i])
      {
        statusFlag = true;
        break;
      }
      coordinate[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateNDCellsIntersectingInterpolationGrid(
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridInterpolationData* tempData)
{
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);
  const MR::ScatteredData::NDMeshAPSearchMap* apSearchMap = ndMeshModel->getAPSearchMap();
  NMP_VERIFY_MSG(
    apSearchMap,
    "The NDMeshModel must be built with an APSearch map to perform resampling analysis");

  const uint32_t* sampleCountsPerDimension = m_interpolationGrid->getSampleCountsPerDimension();

  // The number of vertices in an Nd cell
  uint32_t numVertices = 1 << m_numDimensions;

  // Workspace memory
  float aabbComponentMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbComponentMaxVals[SCATTERED_DATA_MAX_DIM];
  MR::ScatteredData::NDCellEntry nodeEntries[gMaxNumNodeEntries];

  // Initialise
  bool statusFlag = true;
  uint32_t coordinate[SCATTERED_DATA_MAX_DIM];
  uint32_t topLeftCoord[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    topLeftCoord[i] = 0;
  }

  // Cell iteration
  while (statusFlag)
  {
    uint32_t cellAddress = m_interpolationGrid->coordinateToCellAddress(topLeftCoord);
    NDMeshResampleGridInterpolationData::GridCellBinEntry* entry = &(tempData->m_gridCellData[cellAddress]);

    //---------------------------
    // Calculate the number of invalid vertices in the cell
    uint32_t numInvalidVertices = 0;
    for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
    {
      // Convert vertex index into a coordinate offset from the 'top left' of the cell for each dimension.      
      for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
      {
        // Calculate the coordinate offset for the component of the vertex. This is an offset
        // depending on whether the bit for this element is 1 or 0.
        uint32_t offset = (vertex >> dim) & 0x01;
        NMP_VERIFY(topLeftCoord[dim] + offset < sampleCountsPerDimension[dim]);
        coordinate[dim] = topLeftCoord[dim] + offset;
      }
      uint32_t vertexSampleAddress = m_interpolationGrid->coordinateToVertexAddress(coordinate);

      // Check if the cell vertex is valid
      if (!tempData->m_vertexData[vertexSampleAddress].m_vertexValid)
      {
        numInvalidVertices++;
      }
    }

    //---------------------------
    // Determine the set of NDCell annotations that intersect the resample grid cell
    if (numInvalidVertices > 0)
    {
      // Get the resample grid cell AABB
      m_interpolationGrid->getGridCellAABBFromCoordinate(
        topLeftCoord,
        aabbComponentMinVals,
        aabbComponentMaxVals);

      // Compute the set of annotation cells that intersect the query AABB
      uint32_t numNodeEntries = apSearchMap->getNDCellsIntersectingQueryAABBInterpolate(
          nDMesh,
          m_apQueryComponentIndices,
          aabbComponentMinVals,
          aabbComponentMaxVals,
          gMaxNumNodeEntries,
          nodeEntries);

      // Store the intersection results
      if (numNodeEntries > 0)
      {
        // Copy the result data
        NMP::Memory::Format memReqsData(
          sizeof(MR::ScatteredData::NDCellEntry) * numNodeEntries,
          NMP_NATURAL_TYPE_ALIGNMENT);
        NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
        NMP::Memory::memcpy(memResData.ptr, nodeEntries, memReqsData.size);
        MR::ScatteredData::NDCellEntry* intersectionData = (MR::ScatteredData::NDCellEntry*)memResData.ptr;

        // Store the result data
        entry->m_numNodeEntries = numNodeEntries;
        entry->m_nodeEntries = intersectionData;
      }
    }

    //---------------------------
    // Update the cell coordinate
    statusFlag = false;
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      topLeftCoord[i]++;
      if (topLeftCoord[i] < sampleCountsPerDimension[i] - 1)
      {
        statusFlag = true;
        break;
      }
      topLeftCoord[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateInterpolationGridVertexConnections(
  NDMeshResampleGridInterpolationData* tempData)
{
  NMP_VERIFY(tempData);

  const uint32_t* sampleCountsPerDimension = m_interpolationGrid->getSampleCountsPerDimension();

  // The number of vertices in an Nd cell
  uint32_t numVertices = 1 << m_numDimensions;

  uint32_t gridCellAddresses[SCATTERED_DATA_MAX_CELL_VERTICES];
  int32_t coordinate[SCATTERED_DATA_MAX_DIM];

  // Initialise
  bool statusFlag = true;
  uint32_t vertexCoord[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    vertexCoord[i] = 0;
  }

  // Resample vertex iteration
  while (statusFlag)
  {
    // Check for an invalid resample vertex
    uint32_t vertexSampleAddress = m_interpolationGrid->coordinateToVertexAddress(vertexCoord);
    NDMeshResampleGridInterpolationData::VertexBinEntry* vertexEntry = &(tempData->m_vertexData[vertexSampleAddress]);
    if (!vertexEntry->m_vertexValid)
    {
      // Search the neighbouring grid cells connected to the resample vertex to see if they
      // contain intersection data.
      uint32_t numGridCellEntries = 0;
      for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
      {
        // Convert vertex index into a coordinate offset from the 'top left' of the cell for each dimension.
        bool searchFlag = true;
        for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
        {
          // Calculate the coordinate offset for the component of the vertex. This is an offset
          // depending on whether the bit for this element is 1 or 0.
          uint32_t offset = (vertex >> dim) & 0x01;
          coordinate[dim] = (int32_t)vertexCoord[dim] - (int32_t)offset;

          // Ensure that we only search the cells inside the resample grid
          if (coordinate[dim] < 0 || coordinate[dim] >= (int32_t)(sampleCountsPerDimension[dim] - 1))
          {
            searchFlag = false;
            break;
          }
        }

        // Perform the search
        if (searchFlag)
        {
          uint32_t cellAddress = m_interpolationGrid->coordinateToCellAddress((uint32_t*)coordinate);
          NDMeshResampleGridInterpolationData::GridCellBinEntry* cellEntry = &(tempData->m_gridCellData[cellAddress]);
          if (cellEntry->m_numNodeEntries > 0)
          {
            // NOTE: Each cell entry contains a list of topLeftCoords for NDCells that intersect the resample
            // grid cell. Neighbouring resample grid cells can intersect the same NDCells, so we often find
            // ourselves extrapolating invalid resample vertices multiple times using the same NDCell.
            gridCellAddresses[numGridCellEntries] = cellAddress;
            numGridCellEntries++;
          }
        }
      }

      // Store the connection results
      if (numGridCellEntries > 0)
      {
        NMP::Memory::Format memReqsData(
          sizeof(uint32_t) * numGridCellEntries,
          NMP_NATURAL_TYPE_ALIGNMENT);
        NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
        NMP::Memory::memcpy(memResData.ptr, gridCellAddresses, memReqsData.size);

        vertexEntry->m_numGridCellEntries = numGridCellEntries;
        vertexEntry->m_gridCellAddresses = (uint32_t*)memResData.ptr;
      }
    }

    //---------------------------
    // Update the cell coordinate
    statusFlag = false;
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      vertexCoord[i]++;
      if (vertexCoord[i] < sampleCountsPerDimension[i])
      {
        statusFlag = true;
        break;
      }
      vertexCoord[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::extrapolateInterpolationGridVertexSamples(
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridInterpolationData* tempData)
{
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);
  const MR::ScatteredData::NDMeshAPSearchMap* apSearchMap = ndMeshModel->getAPSearchMap();
  NMP_VERIFY_MSG(
    apSearchMap,
    "The NDMeshModel must be built with an APSearch map to perform resampling analysis");

  const uint32_t* sampleCountsPerDimension = m_interpolationGrid->getSampleCountsPerDimension();
  uint32_t numGridCells = m_interpolationGrid->getNumCellSamples();

  // Get the sample centre
  float sampleCentre[SCATTERED_DATA_MAX_DIM];
  nDMesh->getSampleCentre(m_numDimensions, m_apQueryComponentIndices, sampleCentre);

  float cpComponentValues[SCATTERED_DATA_MAX_DIM];
  float ts[SCATTERED_DATA_MAX_DIM];
  float queryPoint[SCATTERED_DATA_MAX_DIM];

  uint32_t cpComponentIndices[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    cpComponentIndices[i] = i;
  }

  //---------------------------
  // Calculate the maximum number of extrapolation entries
  uint32_t numVertexSamples = tempData->m_numVertexSamples;
  uint32_t maxNumExtrapolateEntries = 0;
  for (uint32_t vertexAddr = 0; vertexAddr < numVertexSamples; ++vertexAddr)
  {
    NDMeshResampleGridInterpolationData::VertexBinEntry* vertexEntry = &(tempData->m_vertexData[vertexAddr]);
    uint32_t numGridCellEntries = vertexEntry->m_numGridCellEntries;

    if (!vertexEntry->m_vertexValid && numGridCellEntries > 0)
    {
      // Compute the number of extrapolation entries for this resample vertex
      uint32_t count = 0;
      for (uint32_t cellIndex = 0; cellIndex < numGridCellEntries; ++cellIndex)
      {
        uint32_t cellAddr = vertexEntry->m_gridCellAddresses[cellIndex];
        NMP_VERIFY(cellAddr < numGridCells);
        NDMeshResampleGridInterpolationData::GridCellBinEntry* cellEntry = &(tempData->m_gridCellData[cellAddr]);
        count += cellEntry->m_numNodeEntries;
      }

      // Update the maximum number of extrapolation entries
      maxNumExtrapolateEntries = NMP::maximum(maxNumExtrapolateEntries, count);
    }
  }

  // Allocate the memory for the extrapolation
  NDMeshResampleGridExtrapolationData* extrapolationData =
    NDMeshResampleGridExtrapolationData::create(m_numDimensions, maxNumExtrapolateEntries);

  //---------------------------
  // Extrapolate the invalid resample vertices
  bool statusFlag = true;
  uint32_t vertexCoord[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    vertexCoord[i] = 0;
  }

  // Resample vertex iteration
  while (statusFlag)
  {
    // Check for an invalid resample vertex
    uint32_t vertexAddr = m_interpolationGrid->coordinateToVertexAddress(vertexCoord);
    NDMeshResampleGridInterpolationData::VertexBinEntry* vertexEntry = &(tempData->m_vertexData[vertexAddr]);
    uint32_t numGridCellEntries = vertexEntry->m_numGridCellEntries;

    if (!vertexEntry->m_vertexValid && numGridCellEntries > 0)
    {
      // Get the resample vertex query point      
      m_interpolationGrid->getGridVertexFromCoordinate(
        vertexCoord,  // IN
        queryPoint);  // OUT

      // Extrapolate the CPs for each intersecting annotation
      extrapolationData->reset();
      for (uint32_t cellIndex = 0; cellIndex < numGridCellEntries; ++cellIndex)
      {
        // Get the grid cell containing intersecting annotation data
        uint32_t cellAddr = vertexEntry->m_gridCellAddresses[cellIndex];
        NMP_VERIFY(cellAddr < numGridCells);
        NDMeshResampleGridInterpolationData::GridCellBinEntry* cellBinEntry = &(tempData->m_gridCellData[cellAddr]);

        uint32_t numNodeEntries = cellBinEntry->m_numNodeEntries;
        for (uint32_t entryIndex = 0; entryIndex < numNodeEntries; ++entryIndex)
        {
          MR::ScatteredData::NDCellEntry* nodeEntry = &(cellBinEntry->m_nodeEntries[entryIndex]);

          // Compute the CPs for the query point AP by extrapolation
          bool found = nDMesh->extrapolateCPs(
            nodeEntry->m_topLeftCoord,
            queryPoint,
            m_apQueryComponentIndices,
            ts,     // OUT
            false,  // Don't use initial interpolants
            m_numDimensions,
            cpComponentIndices,
            cpComponentValues);

          if (found)
          {
            // Store the extrapolated CP point and weights
            extrapolationData->addEntry(cpComponentValues, ts);
          }
        }
      }

      // We were unable to extrapolate a result. However, we should at least compute
      // some sort of result otherwise the resample cell will be marked as invalid. The best
      // thing to do is compute the CPs by projecting the APs towards the sample centre.
      if (extrapolationData->getNumEntries() == 0)
      {
        MR::ScatteredData::NDMeshQueryResult ndMeshQueryResult;
        bool found = apSearchMap->projectThroughPoint(
          nDMesh,
          queryPoint,
          m_apQueryComponentIndices,
          sampleCentre,
          &ndMeshQueryResult,
          m_numDimensions,
          cpComponentIndices,
          cpComponentValues);

        if (found)
        {
          // Recover the Nd cell interpolants
          nDMesh->ndCellInterpolants(&ndMeshQueryResult, ts);

          // Store the extrapolated CP point and weights
          extrapolationData->addEntry(cpComponentValues, ts);
        }
      }

      if (extrapolationData->getNumEntries() > 0)
      {
        // Compute a weighted average of the extrapolated CPs
        extrapolationData->computeWeightedAverage(cpComponentValues);

        // Store the CP sample data for the resample vertex
        float* const* vertexCPSamples = m_interpolationData->getVertexCPSamples();
        for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
        {
          vertexCPSamples[dim][vertexAddr] = cpComponentValues[dim];
        }

        // Flag the resample vertex as valid
        vertexEntry->m_vertexValid = true;
      }
    }

    //---------------------------
    // Update the cell coordinate
    statusFlag = false;
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      vertexCoord[i]++;
      if (vertexCoord[i] < sampleCountsPerDimension[i])
      {
        statusFlag = true;
        break;
      }
      vertexCoord[i] = 0;
    }
  }

  //---------------------------
  // Tidy up
  extrapolationData->releaseAndDestroy();
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateInterpolationGridCellFlags(
  NDMeshResampleGridInterpolationData* tempData)
{
  NMP_VERIFY(tempData);
  const uint32_t* sampleCountsPerDimension = m_interpolationGrid->getSampleCountsPerDimension();

  // The number of vertices in an Nd cell
  uint32_t numVertices = 1 << m_numDimensions;

  // Initialise
  bool statusFlag = true;
  uint32_t coordinate[SCATTERED_DATA_MAX_DIM];
  uint32_t topLeftCoord[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    topLeftCoord[i] = 0;
  }

  // Cell iteration
  while (statusFlag)
  {
    uint32_t cellAddress = m_interpolationGrid->coordinateToCellAddress(topLeftCoord);

    //---------------------------
    // Calculate the number of invalid vertices in the cell
    uint32_t numInvalidVertices = 0;
    for (uint32_t vertex = 0; vertex < numVertices; ++vertex)
    {
      // Convert vertex index into a coordinate offset from the 'top left' of the cell for each dimension.      
      for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
      {
        // Calculate the coordinate offset for the component of the vertex. This is an offset
        // depending on whether the bit for this element is 1 or 0.
        uint32_t offset = (vertex >> dim) & 0x01;
        NMP_VERIFY(topLeftCoord[dim] + offset < sampleCountsPerDimension[dim]);
        coordinate[dim] = topLeftCoord[dim] + offset;
      }
      uint32_t vertexSampleAddress = m_interpolationGrid->coordinateToVertexAddress(coordinate);

      // Check if the cell vertex is valid
      if (!tempData->m_vertexData[vertexSampleAddress].m_vertexValid)
      {
        numInvalidVertices++;
      }
    }

    // Update the grid cell flags.
    bool* cellValidFlags = m_interpolationData->getCellFlags();
    if (numInvalidVertices > 0)
    {
      cellValidFlags[cellAddress] = false;
    }
    else
    {
      cellValidFlags[cellAddress] = true;
    }

    //---------------------------
    // Update the cell coordinate
    statusFlag = false;
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      topLeftCoord[i]++;
      if (topLeftCoord[i] < sampleCountsPerDimension[i] - 1)
      {
        statusFlag = true;
        break;
      }
      topLeftCoord[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::setNDCellFacetEntryBufferFromDistance(
  const MR::ScatteredData::NDMesh* nDMesh,
  const MR::ScatteredData::ProjectionBasis* projectionBasis,
  MR::ScatteredData::ProjectRayResult* projectionResult,
  uint32_t numNodeEntries,
  const MR::ScatteredData::NDCellFacetEntry* nodeEntriesSrc,
  MR::ScatteredData::NDCellFacetEntry* nodeEntriesDst,
  SetNDCellFacetEntryBufferStrategy strategy)
{
  NMP_VERIFY(nDMesh);
  NMP_VERIFY(projectionBasis);
  NMP_VERIFY(projectionResult);
  NMP_VERIFY(numNodeEntries > 0);
  NMP_VERIFY(numNodeEntries <= gMaxNumNodeEntries);
  NMP_VERIFY(nodeEntriesSrc);
  NMP_VERIFY(nodeEntriesDst);

  float distanceEntries[gMaxNumNodeEntries];
  uint32_t rankOrderEntries[gMaxNumNodeEntries];
  float cellFacetMidPoint[SCATTERED_DATA_MAX_DIM];
  float midPointInterpolants[SCATTERED_DATA_MAX_DIM];

  uint32_t numDimensions = nDMesh->getNumDimensions();
  uint32_t M = numDimensions - 1;
  const MR::ScatteredData::ScatteredDataManager::DataBlock& managerDataBlock = 
    MR::ScatteredData::ScatteredDataManager::getDataBlock(M);

  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    midPointInterpolants[i] = 0.5f;
  }

  // Compute the distances of each set of cell facets (mid-points) from the query AABB mid-point.
  // This will be used to re-order the entries for optimal runtime query performance. i.e. We want
  // to minimise the number of expensive multi-linear coefficient computations by ensuring that the
  // AABB tests early out as much as possible.
  for (uint32_t i = 0; i < numNodeEntries; ++i)
  {
    uint32_t whichDimFlags = nodeEntriesSrc[i].m_whichDimFlags;
    uint32_t whichBoundaryFlags = nodeEntriesSrc[i].m_whichBoundaryFlags;
    const uint32_t* topLeftCoord = nodeEntriesSrc[i].m_topLeftCoord;

    float distSum = 0.0f;
    uint32_t count = 0;
    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      if ((whichDimFlags >> whichDim) & 0x01)
      {
        uint32_t whichBoundary = (whichBoundaryFlags >> whichDim) & 0x01;

        // Get the ND cell vertex samples
        nDMesh->getNDCellFacetVertexComponents(
          topLeftCoord,
          whichDim,
          whichBoundary,
          numDimensions,
          m_apQueryComponentIndices,
          projectionResult->getFacetVertexComponents());

        // Interpolate the cell facet mid-point
        managerDataBlock.m_multilinearInterpFn(
          M,
          numDimensions,
          cellFacetMidPoint,
          midPointInterpolants,
          projectionResult->getFacetVertexComponents());

        // Compute the distance of the cell facet mid point from the query point
        // in the projection direction
        float dist = projectionBasis->distanceAlongProjectionDirection(cellFacetMidPoint);
        distSum += dist;
        count++;
      }
    }

    NMP_VERIFY(count > 0);
    distanceEntries[i] = distSum / count;
    rankOrderEntries[i] = i;
  }

  // Sort the entries by distance
  for (uint32_t i = 0; i < numNodeEntries - 1; ++i)
  {
    for (uint32_t j = i + 1; j < numNodeEntries; ++j)
    {
      uint32_t indexA = rankOrderEntries[i];
      uint32_t indexB = rankOrderEntries[j];
      if (distanceEntries[indexB] < distanceEntries[indexA])
      {
        NMP::nmSwap(rankOrderEntries[i], rankOrderEntries[j]);
      }
    }
  }

  //------------------------------
  // Set the destination buffer
  if (strategy == kClosestToFurthest)
  {
    for (uint32_t i = 0; i < numNodeEntries; ++i)
    {        
      uint32_t index = rankOrderEntries[i];
      nodeEntriesDst[i] = nodeEntriesSrc[index];
    }
  }
  else if (strategy == kStaggeredRearAndFore)
  {
    // Set the entry buffer by staggering entries: front, rear, front, rear, ...
    // At runtime this ensures that the first few entries will perform the expensive
    // multi-linear coefficients calculation, while the remainder will have an increased
    // likelihood of an early-out from the simple AABB test.
    uint32_t numHalfEntries = numNodeEntries >> 1;
    uint32_t lastNodeEntry = numNodeEntries - 1;
    for (uint32_t i = 0; i < numHalfEntries; ++i)
    {        
      uint32_t indexA = rankOrderEntries[i]; // Rear
      uint32_t indexB = rankOrderEntries[lastNodeEntry - i]; // Fore
      uint32_t index = 2 * i;
      nodeEntriesDst[index] = nodeEntriesSrc[indexA];
      nodeEntriesDst[index + 1] = nodeEntriesSrc[indexB];
    }

    // Singleton entry
    if (numNodeEntries & 0x01)
    {
      uint32_t indexA = rankOrderEntries[numHalfEntries];
      nodeEntriesDst[lastNodeEntry] = nodeEntriesSrc[indexA];
    }
  }
  else
  {
    NMP_VERIFY_FAIL("Invalid SetNDCellFacetEntryBufferStrategy");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateNDCellsIntersectingProjectionGridProjectAlongDimension(
  uint32_t whichDim,
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridProjectionData* tempData)
{
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);
  const MR::ScatteredData::NDMeshAPSearchMap* apSearchMap = ndMeshModel->getAPSearchMap();
  NMP_VERIFY_MSG(
    apSearchMap,
    "The NDMeshModel must be built with an APSearch map to perform resampling analysis");

  NDMeshResampleMapGridExportData* projectionGrid = getProjectionGrid(whichDim);
  NMP_VERIFY(projectionGrid);
  uint32_t M = projectionGrid->getNumDimensions();
  const uint32_t* sampleCountsPerDimension = projectionGrid->getSampleCountsPerDimension();
  const uint32_t* whichQueryComponentIndices = projectionGrid->getWhichQueryComponentIndices();

  // Workspace memory
  float aabbComponentMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbComponentMaxVals[SCATTERED_DATA_MAX_DIM];
  MR::ScatteredData::NDCellFacetEntry nodeEntries[gMaxNumNodeEntries];

  MR::ScatteredData::ProjectRayResult projectionResult;
  projectionResult.init(
    m_numDimensions,
    m_apQueryComponentIndices,
    false);

  MR::ScatteredData::ProjectionBasis projectionBasis;
  projectionBasis.initAlongDimension(
    m_numDimensions,
    whichDim);

  // Initialisation (for iteration over the resample grid)
  bool statusFlag = true;
  uint32_t topLeftCoord[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < M; ++i)
  {
    topLeftCoord[i] = 0;
  }

  // Cell iteration
  while (statusFlag)
  {
    // Get the projection cell address for the coordinate
    uint32_t apCellAddress = projectionGrid->coordinateToCellAddress(topLeftCoord);
    NDMeshResampleGridProjectionData::GridCellBinEntry* entry = &(tempData->m_gridCellData[apCellAddress]);

    //---------------------------
    // Determine the set of NDCell border facets that intersect the projection grid cell

    // Get the AABB components of the projection grid cell
    for (uint32_t dim = 0; dim < M; ++dim)
    {
      uint32_t index = whichQueryComponentIndices[dim]; // Nd component
      NMP_VERIFY(index < m_numDimensions);
      NMP_VERIFY(index != whichDim);

      projectionGrid->getGridCellAABBComponent(
        dim,
        topLeftCoord[dim],
        aabbComponentMinVals[index],
        aabbComponentMaxVals[index]);
    }
    // Shouldn't matter what these components are (they lay on the sacrifice dimension).
    aabbComponentMinVals[whichDim] = 0.0f;
    aabbComponentMaxVals[whichDim] = 0.0f;

    // Compute the set of annotation cells that intersect the query AABB
    uint32_t numNodeEntries = apSearchMap->getNDCellFacetsIntersectingQueryAABBProjectAlongOneDimension(
      nDMesh,
      m_apQueryComponentIndices,
      whichDim,
      aabbComponentMinVals,
      aabbComponentMaxVals,
      gMaxNumNodeEntries,
      nodeEntries);

    //---------------------------
    // Store the intersection results
    if (numNodeEntries > 0)
    {
      // Create a cell facet entry buffer
      NMP::Memory::Format memReqsData(
        sizeof(MR::ScatteredData::NDCellFacetEntry) * numNodeEntries,
        NMP_NATURAL_TYPE_ALIGNMENT);
      NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);      
      MR::ScatteredData::NDCellFacetEntry* intersectionData = (MR::ScatteredData::NDCellFacetEntry*)memResData.ptr;

      // Add the buffer to the cell entry bin
      entry->m_numNodeEntries = numNodeEntries;
      entry->m_nodeEntries = intersectionData;

      // Set the buffer data
      setNDCellFacetEntryBufferFromDistance(
        nDMesh,
        &projectionBasis,
        &projectionResult,
        numNodeEntries,
        nodeEntries,
        intersectionData,
        kStaggeredRearAndFore); // Optimise for Extremes type searches
    }

    //---------------------------
    // Update the cell coordinate
    statusFlag = false;
    for (uint32_t i = 0; i < M; ++i)
    {
      topLeftCoord[i]++;
      if (topLeftCoord[i] < sampleCountsPerDimension[i] - 1)
      {
        statusFlag = true;
        break;
      }
      topLeftCoord[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::compileProjectAlongDimensionExportData(
  uint32_t whichDim,
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridProjectionData* tempData)
{
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);

  NDMeshResampleMapGridExportData* projectionGrid = getProjectionGrid(whichDim);
  NMP_VERIFY(projectionGrid);
  uint32_t numCellSamples = projectionGrid->getNumCellSamples();

  // Calculate the number of entries in the table
  uint32_t numEntries = 0;
  for (uint32_t sampleIndex = 0; sampleIndex < numCellSamples; ++sampleIndex)
  {
    NDMeshResampleGridProjectionData::GridCellBinEntry* tempDataEntry = &(tempData->m_gridCellData[sampleIndex]);
    numEntries += tempDataEntry->m_numNodeEntries;
  }

  // Create the projection data
  NDMeshResampleMapProjectionExportData* projectionData = NDMeshResampleMapProjectionExportData::create(numCellSamples, numEntries);
  setProjectAlongDimensionData(whichDim, projectionData);

  uint32_t* cellEntryCounts = projectionData->getCellEntryCounts();
  uint32_t* entriesCA = projectionData->getEntriesCA();
  uint32_t* entriesWD = projectionData->getEntriesWD();
  uint32_t* entriesWB = projectionData->getEntriesWB();

  // Compile the projection data entries
  numEntries = 0;
  for (uint32_t sampleIndex = 0; sampleIndex < numCellSamples; ++sampleIndex)
  {
    NDMeshResampleGridProjectionData::GridCellBinEntry* tempDataEntry = &(tempData->m_gridCellData[sampleIndex]);
    cellEntryCounts[sampleIndex] = tempDataEntry->m_numNodeEntries;

    for (uint32_t i = 0; i < tempDataEntry->m_numNodeEntries; ++i)
    {
      // Get the addresss for the topLeft NDCell coordinate
      uint32_t cpCellAddress = nDMesh->coordinateToAddress(tempDataEntry->m_nodeEntries[i].m_topLeftCoord);
      entriesCA[numEntries + i] = cpCellAddress;

      uint32_t whichDimFlags = tempDataEntry->m_nodeEntries[i].m_whichDimFlags;
      entriesWD[numEntries + i] = whichDimFlags;

      uint32_t whichBoundaryFlags = tempDataEntry->m_nodeEntries[i].m_whichBoundaryFlags;
      entriesWB[numEntries + i] = whichBoundaryFlags;
    }
    numEntries += tempDataEntry->m_numNodeEntries;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateNDCellsIntersectingProjectionGridProjectThroughPoint(
  const float* projectionCentre,
  uint32_t whichDim,
  uint32_t whichBoundary,
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridProjectionData* tempData)
{
  NMP_VERIFY(projectionCentre);
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);
  const MR::ScatteredData::NDMeshAPSearchMap* apSearchMap = ndMeshModel->getAPSearchMap();
  NMP_VERIFY_MSG(
    apSearchMap,
    "The NDMeshModel must be built with an APSearch map to perform resampling analysis");

  NDMeshResampleMapGridExportData* interpolationGrid = getInterpolationGrid();
  NMP_VERIFY(interpolationGrid);
  NDMeshResampleMapGridExportData* projectionGrid = getProjectionGrid(whichDim);
  NMP_VERIFY(projectionGrid);
  uint32_t M = projectionGrid->getNumDimensions();
  const uint32_t* sampleCountsPerDimension = projectionGrid->getSampleCountsPerDimension();
  const uint32_t* whichQueryComponentIndices = projectionGrid->getWhichQueryComponentIndices();

  // Workspace memory
  float sampleAABBComponent = interpolationGrid->getSampleAABBComponent(whichDim, whichBoundary);
  float aabbComponentMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbComponentMaxVals[SCATTERED_DATA_MAX_DIM];
  MR::ScatteredData::NDCellFacetEntry nodeEntries[gMaxNumNodeEntries];

  MR::ScatteredData::ProjectionBasis projectionBasis;
  MR::ScatteredData::ProjectRayResult projectionResult;
  projectionResult.init(
    m_numDimensions,
    m_apQueryComponentIndices,
    false);

  // Initialisation (for iteration over the resample grid)
  bool statusFlag = true;
  uint32_t topLeftCoord[SCATTERED_DATA_MAX_DIM];
  for (uint32_t i = 0; i < M; ++i)
  {
    topLeftCoord[i] = 0;
  }

  // Cell iteration
  while (statusFlag)
  {
    // Get the projection cell address for the coordinate
    uint32_t apCellAddress = projectionGrid->coordinateToCellAddress(topLeftCoord);
    NDMeshResampleGridProjectionData::GridCellBinEntry* entry = &(tempData->m_gridCellData[apCellAddress]);

    //---------------------------
    // Determine the set of NDCell border facets that intersect the projection grid cell

    // Get the AABB components of the projection grid cell
    for (uint32_t dim = 0; dim < M; ++dim)
    {
      uint32_t index = whichQueryComponentIndices[dim]; // Nd component
      NMP_VERIFY(index < m_numDimensions);
      NMP_VERIFY(index != whichDim);

      projectionGrid->getGridCellAABBComponent(
        dim,
        topLeftCoord[dim],
        aabbComponentMinVals[index],
        aabbComponentMaxVals[index]);
    }
    // Set the AABB component for (whichDim, whichBoundary) hyper-plane
    aabbComponentMinVals[whichDim] = sampleAABBComponent;
    aabbComponentMaxVals[whichDim] = sampleAABBComponent;

    // Compute the query AABB mid-point
    float queryAABBMidPoint[SCATTERED_DATA_MAX_DIM];
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      queryAABBMidPoint[i] = 0.5f * (aabbComponentMinVals[i] + aabbComponentMaxVals[i]);
    }

    // Initialise the projection basis (In the principal projection direction only)
    projectionBasis.initThroughPointProjectionDirectionOnly(
      m_numDimensions,
      queryAABBMidPoint,
      projectionCentre);

    // Compute the set of annotation cells that intersect the query AABB
    uint32_t numNodeEntries = apSearchMap->getNDCellFacetsIntersectingQueryAABBProjectThroughPoint(
      nDMesh,
      projectionCentre,
      m_apQueryComponentIndices,
      whichDim,
      aabbComponentMinVals,
      aabbComponentMaxVals,
      gMaxNumNodeEntries,
      nodeEntries);

    // Store the intersection results
    if (numNodeEntries > 0)
    {
      // Create a cell facet entry buffer
      NMP::Memory::Format memReqsData(
        sizeof(MR::ScatteredData::NDCellFacetEntry) * numNodeEntries,
        NMP_NATURAL_TYPE_ALIGNMENT);
      NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
      MR::ScatteredData::NDCellFacetEntry* intersectionData = (MR::ScatteredData::NDCellFacetEntry*)memResData.ptr;

      // Add the buffer to the cell entry bin
      entry->m_numNodeEntries = numNodeEntries;
      entry->m_nodeEntries = intersectionData;

      // Set the buffer data
      setNDCellFacetEntryBufferFromDistance(
        nDMesh,
        &projectionBasis,
        &projectionResult,
        numNodeEntries,
        nodeEntries,
        intersectionData,
        kClosestToFurthest); // Optimise for closest point type searches
    }

    //---------------------------
    // Update the cell coordinate
    statusFlag = false;
    for (uint32_t i = 0; i < M; ++i)
    {
      topLeftCoord[i]++;
      if (topLeftCoord[i] < sampleCountsPerDimension[i] - 1)
      {
        statusFlag = true;
        break;
      }
      topLeftCoord[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::compileProjectThroughPointExportData(
  uint32_t whichDim,
  uint32_t whichBoundary,
  const MR::PredictionModelNDMesh* ndMeshModel,
  NDMeshResampleGridProjectionData* tempData)
{
  NMP_VERIFY(ndMeshModel);
  NMP_VERIFY(tempData);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);

  NDMeshResampleMapGridExportData* projectionGrid = getProjectionGrid(whichDim);
  NMP_VERIFY(projectionGrid);
  uint32_t numCellSamples = projectionGrid->getNumCellSamples();

  // Calculate the number of entries in the table
  uint32_t numEntries = 0;
  for (uint32_t sampleIndex = 0; sampleIndex < numCellSamples; ++sampleIndex)
  {
    NDMeshResampleGridProjectionData::GridCellBinEntry* tempDataEntry = &(tempData->m_gridCellData[sampleIndex]);
    numEntries += tempDataEntry->m_numNodeEntries;
  }

  // Create the projection data
  NDMeshResampleMapProjectionExportData* projectionData = NDMeshResampleMapProjectionExportData::create(numCellSamples, numEntries);
  setProjectThroughPointData(whichDim, whichBoundary, projectionData);

  uint32_t* cellEntryCounts = projectionData->getCellEntryCounts();
  uint32_t* entriesCA = projectionData->getEntriesCA();
  uint32_t* entriesWD = projectionData->getEntriesWD();
  uint32_t* entriesWB = projectionData->getEntriesWB();

  // Compile the projection data entries
  numEntries = 0;
  for (uint32_t sampleIndex = 0; sampleIndex < numCellSamples; ++sampleIndex)
  {
    NDMeshResampleGridProjectionData::GridCellBinEntry* tempDataEntry = &(tempData->m_gridCellData[sampleIndex]);
    cellEntryCounts[sampleIndex] = tempDataEntry->m_numNodeEntries;

    for (uint32_t i = 0; i < tempDataEntry->m_numNodeEntries; ++i)
    {
      // Get the addresss for the topLeft NDCell coordinate
      uint32_t cpCellAddress = nDMesh->coordinateToAddress(tempDataEntry->m_nodeEntries[i].m_topLeftCoord);
      entriesCA[numEntries + i] = cpCellAddress;

      uint32_t whichDimFlags = tempDataEntry->m_nodeEntries[i].m_whichDimFlags;
      entriesWD[numEntries + i] = whichDimFlags;

      uint32_t whichBoundaryFlags = tempDataEntry->m_nodeEntries[i].m_whichBoundaryFlags;
      entriesWB[numEntries + i] = whichBoundaryFlags;
    }
    numEntries += tempDataEntry->m_numNodeEntries;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::setSampleAABB(
  uint32_t numDimensions,
  const float* aabbComponentMinVals,
  const float* aabbComponentMaxVals)
{
  NMP_VERIFY(numDimensions == m_numDimensions);
  NMP_VERIFY(aabbComponentMinVals);
  NMP_VERIFY(aabbComponentMaxVals);

  // Interpolation resample grid
  for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
  {
    m_interpolationGrid->setStartRange(dim, aabbComponentMinVals[dim]);
    m_interpolationGrid->setEndRange(dim, aabbComponentMaxVals[dim]);
  }

  // Projection resample maps
  uint32_t M = m_numDimensions - 1;
  if (M > 0)
  {
    for (uint32_t dim = 0; dim < m_numDimensions; ++dim)
    {
      const uint32_t* whichQueryComponentIndices = m_projectionGrids[dim]->getWhichQueryComponentIndices();
      for (uint32_t i = 0; i < M; ++i)
      {
        uint32_t index = whichQueryComponentIndices[i];
        NMP_VERIFY(index < numDimensions);
        m_projectionGrids[dim]->setStartRange(i, aabbComponentMinVals[index]);
        m_projectionGrids[dim]->setEndRange(i, aabbComponentMaxVals[index]);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::calculateAPQueryComplementComponentIndices()
{
  bool isIndependentVar[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];

  uint32_t numComponentsPerSample = m_numDimensions + m_numAPQueryComplementComponentIndices;
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    isIndependentVar[i] = false;
  }
  for (uint32_t i = 0; i < m_numDimensions; ++i)
  {
    NMP_VERIFY(m_apQueryComponentIndices[i] < numComponentsPerSample);
    isIndependentVar[m_apQueryComponentIndices[i]] = true;
  }
  uint32_t outputComponentsSize = 0;
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    if (!isIndependentVar[i])
    {
      m_apQueryComplementComponentIndices[outputComponentsSize] = i;
      ++outputComponentsSize;
    }
  }
  NMP_VERIFY(outputComponentsSize == m_numAPQueryComplementComponentIndices);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::analyse(const MR::PredictionModelNDMesh* ndMeshModel)
{
  NMP_VERIFY(ndMeshModel);
  const MR::ScatteredData::NDMesh* nDMesh = ndMeshModel->getNDMesh();
  NMP_VERIFY(nDMesh);

  // Compute the sample ranges
  float aabbComponentMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbComponentMaxVals[SCATTERED_DATA_MAX_DIM];
  ndMeshModel->getSampleAABB(
    m_numDimensions,
    m_apQueryComponentIndices,
    aabbComponentMinVals,
    aabbComponentMaxVals);
  setSampleAABB(
    m_numDimensions,
    aabbComponentMinVals,
    aabbComponentMaxVals);

  //---------------------------
  // Interpolation resample map
  {
    NDMeshResampleGridInterpolationData* tempData = NDMeshResampleGridInterpolationData::create(
      m_interpolationGrid->getNumDimensions(),
      m_interpolationGrid->getNumVertexSamples(),
      m_interpolationGrid->getNumCellSamples());

    // Resample the analysis property space by interpolating the NDMesh at the regular
    // sample grid vertex sites. Projection is not performed so that the valid resampled
    // vertices actually correspond to interpolated values.
    calculateInterpolationGridVertexSamples(ndMeshModel, tempData);

    // For each resample grid cell that contains at least one invalid vertex sample, determine the
    // set of annotation cells that intersect it.
    calculateNDCellsIntersectingInterpolationGrid(ndMeshModel, tempData);

    // For each invalid resample vertex, determine the set of resample cells connected to it that
    // contain intersecting annotation data.
    calculateInterpolationGridVertexConnections(tempData);

    // For each invalid resample vertex, extrapolate the CP values from the annotations that intersect
    // the neighbouring resample grid cells; then compute a weighted average of the contributing CP values.
    extrapolateInterpolationGridVertexSamples(ndMeshModel, tempData);

    // Determine the set interpolation cell flags for the recovered sample data
    calculateInterpolationGridCellFlags(tempData);

    // Tidy up
    tempData->releaseAndDestroy();
  }

  //---------------------------  
  uint32_t M = m_numDimensions - 1;
  if (M > 0)
  {
    //---------------------------
    // Projection along dimension resample maps
    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      NDMeshResampleMapGridExportData* projectionGrid = m_projectionGrids[whichDim];
      NMP_VERIFY(projectionGrid);

      NDMeshResampleGridProjectionData* tempData = NDMeshResampleGridProjectionData::create(
        projectionGrid->getNumDimensions(),
        projectionGrid->getNumCellSamples());

      // Compute the NDCells intersecting the projection grid cells
      calculateNDCellsIntersectingProjectionGridProjectAlongDimension(
        whichDim,
        ndMeshModel,
        tempData);

      // Compile the intersection export data
      compileProjectAlongDimensionExportData(
        whichDim,
        ndMeshModel,
        tempData);

      // Tidy up
      tempData->releaseAndDestroy();
    }

    //---------------------------
    // Projection through sample centre resample maps
    if (m_buildProjectThroughSampleCentreMap)
    {
      // Get the sample centre
      float sampleCentre[SCATTERED_DATA_MAX_DIM];
      nDMesh->getSampleCentre(m_numDimensions, m_apQueryComponentIndices, sampleCentre);

      for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
      {
        NDMeshResampleMapGridExportData* projectionGrid = m_projectionGrids[whichDim];
        NMP_VERIFY(projectionGrid);

        for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
        {
          // Create the intermediate projection data
          NDMeshResampleGridProjectionData* tempData = NDMeshResampleGridProjectionData::create(
            projectionGrid->getNumDimensions(),
            projectionGrid->getNumCellSamples());

          // Compute the NDCells intersecting the projection grid cells
          calculateNDCellsIntersectingProjectionGridProjectThroughPoint(
            sampleCentre,
            whichDim,
            whichBoundary,
            ndMeshModel,
            tempData);

          // Compile the intersection export data
          compileProjectThroughPointExportData(
            whichDim,
            whichBoundary,
            ndMeshModel,
            tempData);

          // Tidy up
          tempData->releaseAndDestroy();
        } // whichBoundary
      } // whichDim
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::readExportData(
  uint32_t resampleMapIndex,
  const ME::DataBlockExport* ndMeshNodeExportDataBlock,
  uint32_t bufferSize,
  uint32_t& numDimensions,
  uint32_t& numAnalysisProperties,
  uint32_t& numBitsPerSample,
  bool& buildProjectThroughSampleCentreMap,
  uint32_t* sampleCounts,
  uint32_t* apQueryComponentIndices)
{
  NMP_VERIFY(ndMeshNodeExportDataBlock);
  bool status;
  size_t length;
  char attrName[256];

  //------------------------------
  // Num dimensions
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_NumDimensions", resampleMapIndex);
  status = ndMeshNodeExportDataBlock->readUInt(numDimensions, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);
  NMP_VERIFY(numDimensions <= bufferSize);

  //------------------------------
  // Num analusis properties
  status = ndMeshNodeExportDataBlock->readUInt(numAnalysisProperties, "NumAnalysisProperties");
  NMP_VERIFY_MSG(
    status,
    "Could not find NumAnalysisProperties attribute in the export");

  //------------------------------
  // Sample counts
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_SampleCounts", resampleMapIndex);
  status = ndMeshNodeExportDataBlock->readUIntArray(sampleCounts, bufferSize, length, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);
  NMP_VERIFY_MSG(
    length == numDimensions,
    "Invalid sample counts array size: expecting %d entries, got %d",
    numDimensions,
    length);

  //------------------------------
  // Independent var indices
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_APComponentIndices", resampleMapIndex);
  status = ndMeshNodeExportDataBlock->readUIntArray(apQueryComponentIndices, bufferSize, length, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);
  NMP_VERIFY_MSG(
    length == numDimensions,
    "Invalid independentVarIndices array size: expecting %d entries, got %d",
    numDimensions,
    length);

  //------------------------------
  // Num bits per resampled vertex
  numBitsPerSample = 10;
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_NumBitsPerSample", resampleMapIndex);
  status = ndMeshNodeExportDataBlock->readUInt(numBitsPerSample, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);

  //------------------------------
  // Build project through sample centre map
  buildProjectThroughSampleCentreMap = false;
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_ProjectionThroughPointData_Enable", resampleMapIndex);
  status = ndMeshNodeExportDataBlock->readBool(buildProjectThroughSampleCentreMap, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::readPerSetExportData(
  uint32_t resampleMapIndex,
  MR::AnimSetIndex animSetIndex,
  const ME::DataBlockExport* dataBlock)
{
  NMP_VERIFY(dataBlock);
  bool status;
  size_t length;
  char attrName[256];
  char attrNameCA[256];
  char attrNameWD[256];
  char attrNameWB[256];
  const uint32_t bufferSize = SCATTERED_DATA_MAX_DIM;
  float aabbComponentMinVals[SCATTERED_DATA_MAX_DIM];
  float aabbComponentMaxVals[SCATTERED_DATA_MAX_DIM];

  //---------------------------
  // Sample information
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_StartRanges_Set%d", resampleMapIndex, animSetIndex);
  status = dataBlock->readFloatArray(aabbComponentMinVals, bufferSize, length, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);
  NMP_VERIFY_MSG(
    length == m_numDimensions,
    "Invalid startRanges array size: expecting %d entries, got %d",
    m_numDimensions,
    length);

  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_EndRanges_Set%d", resampleMapIndex, animSetIndex);
  status = dataBlock->readFloatArray(aabbComponentMaxVals, bufferSize, length, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);
  NMP_VERIFY_MSG(
    length == m_numDimensions,
    "Invalid endRanges array size: expecting %d entries, got %d",
    m_numDimensions,
    length);

  setSampleAABB(m_numDimensions, aabbComponentMinVals, aabbComponentMaxVals);

  //---------------------------
  // Interpolation grid data
  uint32_t numVertexSamples = m_interpolationGrid->getNumVertexSamples();
  uint32_t numGridCells = m_interpolationGrid->getNumCellSamples();

  for (uint32_t cpIndex = 0; cpIndex < m_numDimensions; ++cpIndex)
  {
    float* controlParameterSamples = m_interpolationData->getVertexCPSamples(cpIndex);
    sprintf_s(
      attrName, 256 - 1,
      "ResampleGrid%d_InterpolationData_CP%d_Samples_Set%d",
      resampleMapIndex, cpIndex, animSetIndex);
    status = dataBlock->readFloatArray(controlParameterSamples, numVertexSamples, length, attrName);
    NMP_VERIFY_MSG(
      status,
      "Could not find %s attribute in the export",
      attrName);
  }

  bool* gridCellFlags = m_interpolationData->getCellFlags();
  sprintf_s(
    attrName, 256 - 1,
    "ResampleGrid%d_InterpolationData_CellFlag_Samples_Set%d",
    resampleMapIndex, animSetIndex);
  status = dataBlock->readBoolArray(gridCellFlags, numGridCells, length, attrName);
  NMP_VERIFY_MSG(
    status,
    "Could not find %s attribute in the export",
    attrName);

  //---------------------------
  // Projection grid data
  uint32_t M = m_numDimensions - 1;
  if (M > 0)
  {
    size_t byteCount;

    //---------------------------
    // Projection along dimension resample maps
    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      // Get the size of the cell entry counts buffer
      uint32_t numCellSamples = 0;
      sprintf_s(
        attrName, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntryCounts_Set%d",
        resampleMapIndex, whichDim, animSetIndex);
      byteCount = dataBlock->getByteCount(attrName);
      NMP_VERIFY_MSG(
        byteCount > 0,
        "Could not find %s attribute in the export",
        attrName);
      numCellSamples = (uint32_t)(byteCount / sizeof(uint32_t));

      // Get the size of the cell entry buffer
      uint32_t numEntries = 0;
      sprintf_s(
        attrNameCA, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntriesCA_Set%d",
        resampleMapIndex, whichDim, animSetIndex);
      byteCount = dataBlock->getByteCount(attrNameCA);
      NMP_VERIFY_MSG(
        byteCount > 0,
        "Could not find %s attribute in the export",
        attrNameCA);
      numEntries = (uint32_t)(byteCount / sizeof(uint32_t));

      sprintf_s(
        attrNameWD, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntriesWD_Set%d",
        resampleMapIndex, whichDim, animSetIndex);

      sprintf_s(
        attrNameWB, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntriesWB_Set%d",
        resampleMapIndex, whichDim, animSetIndex);

      // Create a new projection export data structure
      NDMeshResampleMapProjectionExportData* projectionData = NDMeshResampleMapProjectionExportData::create(numCellSamples, numEntries);
      setProjectAlongDimensionData(whichDim, projectionData);
      uint32_t* cellEntryCounts = projectionData->getCellEntryCounts();
      uint32_t* entriesCA = projectionData->getEntriesCA();
      uint32_t* entriesWD = projectionData->getEntriesWD();
      uint32_t* entriesWB = projectionData->getEntriesWB();

      // Read the cell entry counts
      status = dataBlock->readUIntArray(cellEntryCounts, numCellSamples, length, attrName);
      NMP_VERIFY_MSG(
        status,
        "Could not find %s attribute in the export",
        attrName);
      NMP_VERIFY(length == numCellSamples);

      // Read the cell entries
      status = dataBlock->readUIntArray(entriesCA, numEntries, length, attrNameCA);
      NMP_VERIFY_MSG(
        status,
        "Could not find %s attribute in the export",
        attrNameCA);
      NMP_VERIFY(length == numEntries);

      status = dataBlock->readUIntArray(entriesWD, numEntries, length, attrNameWD);
      NMP_VERIFY_MSG(
        status,
        "Could not find %s attribute in the export",
        attrNameWD);
      NMP_VERIFY(length == numEntries);

      status = dataBlock->readUIntArray(entriesWB, numEntries, length, attrNameWB);
      NMP_VERIFY_MSG(
        status,
        "Could not find %s attribute in the export",
        attrNameWB);
      NMP_VERIFY(length == numEntries);
    }

    //---------------------------
    // Projection through point resample maps
    if (m_buildProjectThroughSampleCentreMap)
    {
      for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
      {
        for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
        {
          // Get the size of the cell entry counts buffer
          uint32_t numCellSamples = 0;
          sprintf_s(
            attrName, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntryCounts_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);
          byteCount = dataBlock->getByteCount(attrName);
          NMP_VERIFY_MSG(
            byteCount > 0,
            "Could not find %s attribute in the export",
            attrName);
          numCellSamples = (uint32_t)(byteCount / sizeof(uint32_t));

          // Get the size of the cell entry buffer
          uint32_t numEntries = 0;
          sprintf_s(
            attrNameCA, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntriesCA_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);
          byteCount = dataBlock->getByteCount(attrNameCA);
          NMP_VERIFY_MSG(
            byteCount > 0,
            "Could not find %s attribute in the export",
            attrNameCA);
          numEntries = (uint32_t)(byteCount / sizeof(uint32_t));

          sprintf_s(
            attrNameWD, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntriesWD_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);

          sprintf_s(
            attrNameWB, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntriesWB_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);

          // Create a new projection export data structure
          NDMeshResampleMapProjectionExportData* projectionData = NDMeshResampleMapProjectionExportData::create(numCellSamples, numEntries);
          setProjectThroughPointData(whichDim, whichBoundary, projectionData);
          uint32_t* cellEntryCounts = projectionData->getCellEntryCounts();
          uint32_t* entriesCA = projectionData->getEntriesCA();
          uint32_t* entriesWD = projectionData->getEntriesWD();
          uint32_t* entriesWB = projectionData->getEntriesWB();

          // Read the cell entry counts
          status = dataBlock->readUIntArray(cellEntryCounts, numCellSamples, length, attrName);
          NMP_VERIFY_MSG(
            status,
            "Could not find %s attribute in the export",
            attrName);
          NMP_VERIFY(length == numCellSamples);

          // Read the cell entries
          status = dataBlock->readUIntArray(entriesCA, numEntries, length, attrNameCA);
          NMP_VERIFY_MSG(
            status,
            "Could not find %s attribute in the export",
            attrNameCA);
          NMP_VERIFY(length == numEntries);

          status = dataBlock->readUIntArray(entriesWD, numEntries, length, attrNameWD);
          NMP_VERIFY_MSG(
            status,
            "Could not find %s attribute in the export",
            attrNameWD);
          NMP_VERIFY(length == numEntries);

          status = dataBlock->readUIntArray(entriesWB, numEntries, length, attrNameWB);
          NMP_VERIFY_MSG(
            status,
            "Could not find %s attribute in the export",
            attrNameWB);
          NMP_VERIFY(length == numEntries);
        }
      }
    }
  } // M > 0
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::writeExportData(uint32_t resampleMapIndex, ME::DataBlockExport* resultData) const
{
  NMP_VERIFY(resultData);

  char attrName[256];
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_NumDimensions", resampleMapIndex);
  resultData->writeUInt(m_numDimensions, attrName);

  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_APComponentIndices", resampleMapIndex);
  resultData->writeUIntArray(m_apQueryComponentIndices, m_numDimensions, attrName);

  const uint32_t* sampleCountsPerDimension = m_interpolationGrid->getSampleCountsPerDimension();
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_SampleCounts", resampleMapIndex);
  resultData->writeUIntArray(sampleCountsPerDimension, m_numDimensions, attrName);

  uint32_t numBitsPerSample = getNumBitsPerSample();
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_NumBitsPerSample", resampleMapIndex);
  resultData->writeUInt(numBitsPerSample, attrName);

  bool buildProjectThroughSampleCentreMap = getBuildProjectThroughSampleCentreMap();
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_ProjectionThroughPointData_Enable", resampleMapIndex);
  resultData->writeBool(buildProjectThroughSampleCentreMap, attrName);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::writePerSetExportData(
  uint32_t resampleMapIndex,
  MR::AnimSetIndex animSetIndex,
  ME::DataBlockExport* resultData) const
{
  //---------------------------
  // Sample information
  const float* startRanges = m_interpolationGrid->getStartRanges();
  const float* endRanges = m_interpolationGrid->getEndRanges();

  char attrName[256];
  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_StartRanges_Set%d", resampleMapIndex, animSetIndex);
  resultData->writeFloatArray(startRanges, m_numDimensions, attrName);

  sprintf_s(attrName, 256 - 1, "ResampleGrid%d_EndRanges_Set%d", resampleMapIndex, animSetIndex);
  resultData->writeFloatArray(endRanges, m_numDimensions, attrName);

  //---------------------------
  // Interpolation grid data
  uint32_t numVertexSamples = m_interpolationGrid->getNumVertexSamples();
  uint32_t numGridCells = m_interpolationGrid->getNumCellSamples();

  for (uint32_t cpIndex = 0; cpIndex < m_numDimensions; ++cpIndex)
  {
    const float* controlParameterSamples = m_interpolationData->getVertexCPSamples(cpIndex);
    sprintf_s(
      attrName, 256 - 1,
      "ResampleGrid%d_InterpolationData_CP%d_Samples_Set%d",
      resampleMapIndex, cpIndex, animSetIndex);
    resultData->writeFloatArray(controlParameterSamples, numVertexSamples, attrName);
  }

  const bool* gridCellFlags = m_interpolationData->getCellFlags();
  sprintf_s(
    attrName, 256 - 1,
    "ResampleGrid%d_InterpolationData_CellFlag_Samples_Set%d",
    resampleMapIndex, animSetIndex);
  resultData->writeBoolArray(gridCellFlags, numGridCells, attrName);

  //---------------------------
  // Projection grid data
  uint32_t M = m_numDimensions - 1;
  if (M > 0)
  {
    //---------------------------
    // Projection along dimension resample maps
    for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
    {
      const NDMeshResampleMapProjectionExportData* projectAlongDimensionData = getProjectAlongDimensionData(whichDim);
      NMP_VERIFY(projectAlongDimensionData);
      uint32_t numCellSamples = projectAlongDimensionData->getNumCellSamples();
      uint32_t numEntries = projectAlongDimensionData->getNumEntries();
      const uint32_t* cellEntryCounts = projectAlongDimensionData->getCellEntryCounts();
      const uint32_t* cellEntriesCA = projectAlongDimensionData->getEntriesCA();
      const uint32_t* cellEntriesWD = projectAlongDimensionData->getEntriesWD();
      const uint32_t* cellEntriesWB = projectAlongDimensionData->getEntriesWB();

      sprintf_s(
        attrName, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntryCounts_Set%d",
        resampleMapIndex, whichDim, animSetIndex);
      resultData->writeUIntArray(cellEntryCounts, numCellSamples, attrName);

      sprintf_s(
        attrName, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntriesCA_Set%d",
        resampleMapIndex, whichDim, animSetIndex);
      resultData->writeUIntArray(cellEntriesCA, numEntries, attrName);

      sprintf_s(
        attrName, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntriesWD_Set%d",
        resampleMapIndex, whichDim, animSetIndex);
      resultData->writeUIntArray(cellEntriesWD, numEntries, attrName);

      sprintf_s(
        attrName, 256 - 1,
        "ResampleGrid%d_ProjectionAlongDimensionData_Dim%d_CellEntriesWB_Set%d",
        resampleMapIndex, whichDim, animSetIndex);
      resultData->writeUIntArray(cellEntriesWB, numEntries, attrName);
    }

    //---------------------------
    // Projection through sample centre resample maps
    if (m_buildProjectThroughSampleCentreMap)
    {
      for (uint32_t whichDim = 0; whichDim < m_numDimensions; ++whichDim)
      {
        for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
        {
          const NDMeshResampleMapProjectionExportData* projectThroughPointData = getProjectThroughPointData(whichDim, whichBoundary);
          NMP_VERIFY(projectThroughPointData);

          uint32_t numCellSamples = projectThroughPointData->getNumCellSamples();
          uint32_t numEntries = projectThroughPointData->getNumEntries();
          const uint32_t* cellEntryCounts = projectThroughPointData->getCellEntryCounts();
          const uint32_t* cellEntriesCA = projectThroughPointData->getEntriesCA();
          const uint32_t* cellEntriesWD = projectThroughPointData->getEntriesWD();
          const uint32_t* cellEntriesWB = projectThroughPointData->getEntriesWB();

          sprintf_s(
            attrName, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntryCounts_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);
          resultData->writeUIntArray(cellEntryCounts, numCellSamples, attrName);

          sprintf_s(
            attrName, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntriesCA_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);
          resultData->writeUIntArray(cellEntriesCA, numEntries, attrName);

          sprintf_s(
            attrName, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntriesWD_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);
          resultData->writeUIntArray(cellEntriesWD, numEntries, attrName);

          sprintf_s(
            attrName, 256 - 1,
            "ResampleGrid%d_ProjectionThroughPointData_Dim%d_Boundary%d_CellEntriesWB_Set%d",
            resampleMapIndex, whichDim, whichBoundary, animSetIndex);
          resultData->writeUIntArray(cellEntriesWB, numEntries, attrName);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::tidyPerSetExportData()
{
  //------------------------------
  // Release the interpolation data
  if (m_interpolationQuantisationData)
  {
    m_interpolationQuantisationData->releaseAndDestroy();
    m_interpolationQuantisationData = NULL;
  }

  //------------------------------
  // Release the projection data
  uint32_t M = m_numDimensions - 1;
  if (M > 0)
  {
    //------------------------------
    // Project along dimension
    NMP_VERIFY(m_projectAlongDimensionData);
    for (uint32_t i = 0; i < m_numDimensions; ++i)
    {
      if (m_projectAlongDimensionData[i])
      {
        m_projectAlongDimensionData[i]->releaseAndDestroy();
        m_projectAlongDimensionData[i] = NULL;
      }
    }

    //------------------------------
    // Project through point
    for (uint32_t whichBoundary = 0; whichBoundary < 2; ++whichBoundary)
    {
      NDMeshResampleMapProjectionExportData** projectThroughPointData = m_projectThroughPointData[whichBoundary];
      if (projectThroughPointData)
      {
        for (uint32_t i = 0; i < m_numDimensions; ++i)
        {
          if (projectThroughPointData[i])
          {
            projectThroughPointData[i]->releaseAndDestroy();
            projectThroughPointData[i] = NULL;
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapExportData::compileQuantisationData()
{
  //---------------------------
  // Create the quantisation data for interpolation
  NMP_VERIFY(!m_interpolationQuantisationData);
  m_interpolationQuantisationData = NDMeshResampleMapCPSampleQuantisationData::create(
    m_interpolationGrid,
    m_interpolationData,
    m_numBitsPerSample);
}

}
