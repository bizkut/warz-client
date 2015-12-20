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
#include "Analysis/NDMeshBuilder.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "NMPlatform/NMSystem.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshBuilder::getMemoryRequirements(
  uint32_t              numDimensions,
  uint32_t              numAnalysisProperties,
  const uint32_t*       sampleCountsPerDimension)
{
  NMP_VERIFY(numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_VERIFY(numAnalysisProperties > 0 && numAnalysisProperties <= SCATTERED_DATA_MAX_DIM);
  uint32_t numComponentsPerSample = numDimensions + numAnalysisProperties;
  NMP_VERIFY(numComponentsPerSample <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);

  // Calculate the total number of samples required.
  uint32_t numSamples = 1;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    NMP_VERIFY(sampleCountsPerDimension[i] > 0);
    numSamples *= sampleCountsPerDimension[i];
  }

  // The class instance itself.
  NMP::Memory::Format result(sizeof(NDMesh), NMP_NATURAL_TYPE_ALIGNMENT);

  // Quantisation information
  NMP::Memory::Format memReqsQInfo(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);

  if (numDimensions > 0)
  {
    // Sample counts in each dimension
    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsSampleCounts;

    // ND Block sizes
    result += memReqsSampleCounts;

    // Control component start and end values
    NMP::Memory::Format memReqsCPRanges(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsCPRanges * 3); // Start, end, scales

    // Query normalisation
    result += (memReqsQInfo * 4); // Real->Norm, Quantised->Norm
  }

  // Quantisation information
  result += (memReqsQInfo * 2); // Real (scale, offset)

  // Vertex samples
  NMP::Memory::Format memReqsSamples(sizeof(uint16_t) * numComponentsPerSample * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsSamples;

  // Definition data for convenience
  NMP::Memory::Format memReqsAPIndices(sizeof(uint32_t) * numAnalysisProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsAPIndices;

  // Sample centre
  NMP::Memory::Format memReqsCentre(sizeof(uint16_t) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsCentre;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::ScatteredData::NDMesh* NDMeshBuilder::init(
  NMP::Memory::Resource& memRes,
  uint32_t              numDimensions,
  uint32_t              numAnalysisProperties,
  const uint32_t*       sampleCountsPerDimension)
{
  NMP_VERIFY(numDimensions <= SCATTERED_DATA_MAX_DIM);
  NMP_VERIFY(numAnalysisProperties > 0 && numAnalysisProperties <= SCATTERED_DATA_MAX_DIM);
  uint32_t numComponentsPerSample = numDimensions + numAnalysisProperties;
  NMP_VERIFY(numComponentsPerSample <= SCATTERED_DATA_MAX_SAMPLE_COMPONENTS);

  // Calculate the total number of samples required.
  uint32_t numSamples = 1;
  for (uint32_t i = 0; i < numDimensions; ++i)
  {
    NMP_VERIFY(sampleCountsPerDimension[i] > 0);
    numSamples *= sampleCountsPerDimension[i];
  }

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::ScatteredData::NDMesh), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshBuilder* result = (NDMeshBuilder*)memRes.alignAndIncrement(memReqsHdr);

  result->m_numDimensions = numDimensions;
  result->m_numAnalysisProperties = numAnalysisProperties;
  result->m_numComponentsPerSample = numComponentsPerSample;
  result->m_numSamples = numSamples;

  // Quantisation information
  NMP::Memory::Format memReqsQInfo(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);

  if (numDimensions > 0)
  {
    // Sample counts in each dimension
    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_sampleCountsPerDimension = (uint32_t*)memRes.alignAndIncrement(memReqsSampleCounts);

    // ND Block sizes
    result->m_nDBlockSizes = (uint32_t*)memRes.alignAndIncrement(memReqsSampleCounts);

    uint32_t blockSize = 1;
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      NMP_VERIFY(sampleCountsPerDimension[i] > 0);
      result->m_sampleCountsPerDimension[i] = sampleCountsPerDimension[i];
      result->m_nDBlockSizes[i] = blockSize;
      blockSize *= sampleCountsPerDimension[i];
    }
    NMP_VERIFY(blockSize == numSamples);

    // Control component start and end values
    NMP::Memory::Format memReqsCPRanges(sizeof(float) * numDimensions, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_controlComponentStartValues = (float*)memRes.alignAndIncrement(memReqsCPRanges);
    result->m_controlComponentEndValues = (float*)memRes.alignAndIncrement(memReqsCPRanges);
    result->m_controlComponentSampleScales = (float*)memRes.alignAndIncrement(memReqsCPRanges);

    // Query normalisation
    result->m_scalesRealToNorm = (float*)memRes.alignAndIncrement(memReqsQInfo);
    result->m_offsetsRealToNorm = (float*)memRes.alignAndIncrement(memReqsQInfo);

    result->m_scalesQuantisedToNorm = (float*)memRes.alignAndIncrement(memReqsQInfo);
    result->m_offsetsQuantisedToNorm = (float*)memRes.alignAndIncrement(memReqsQInfo);
  }
  else
  {
    result->m_sampleCountsPerDimension = NULL;
    result->m_nDBlockSizes = NULL;
    result->m_controlComponentStartValues = NULL;
    result->m_controlComponentEndValues = NULL;
    result->m_controlComponentSampleScales = NULL;

    // Query normalisation
    result->m_scalesRealToNorm = NULL;
    result->m_offsetsRealToNorm = NULL;
    result->m_scalesQuantisedToNorm = NULL;
    result->m_offsetsQuantisedToNorm = NULL;
  }

  // Quantisation information
  result->m_scalesQuantisedToReal = (float*)memRes.alignAndIncrement(memReqsQInfo);
  result->m_offsetsQuantisedToReal = (float*)memRes.alignAndIncrement(memReqsQInfo); 

  // Vertex samples
  NMP::Memory::Format memReqsSamples(sizeof(uint16_t) * numComponentsPerSample * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_vertexSamples = (uint16_t*)memRes.alignAndIncrement(memReqsSamples);

  // Definition data for convenience
  NMP::Memory::Format memReqsAPIndices(sizeof(uint32_t) * numAnalysisProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_analysisPropertyComponentIndices = (uint32_t*)memRes.alignAndIncrement(memReqsAPIndices);

  // Sample centre
  NMP::Memory::Format memReqsCentre(sizeof(uint16_t) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sampleCentre = (uint16_t*)memRes.alignAndIncrement(memReqsCentre);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource NDMeshBuilder::create(
  const NDMeshAnalysisData* resultDataNDMesh,
  const NDMeshQuantisationData* quantisationData)
{
  NMP_VERIFY(resultDataNDMesh);
  NMP_VERIFY(quantisationData);

  uint32_t numControlParameters = resultDataNDMesh->getNumControlParameters();
  NMP_VERIFY(numControlParameters <= SCATTERED_DATA_MAX_DIM);
  uint32_t numAnalysisProperties = resultDataNDMesh->getNumAnalysisProperties();
  NMP_VERIFY(numAnalysisProperties > 0);
  uint32_t numComponentsPerSample = numControlParameters + numAnalysisProperties;
  uint32_t numAnalysedSamples = resultDataNDMesh->getNumAnalysedSamples();

  const uint32_t* sampleCountsPerDimension = resultDataNDMesh->getSampleCounts();
  const float* startRanges = resultDataNDMesh->getStartRanges();
  const float* endRanges = resultDataNDMesh->getEndRanges();

  NMP::Memory::Format memReqsNDMesh = getMemoryRequirements(
    numControlParameters,
    numAnalysisProperties,
    sampleCountsPerDimension);

  NMP::Memory::Resource memResNDMesh = NMPMemoryAllocateFromFormat(memReqsNDMesh);
  NMP::Memory::Resource result = memResNDMesh;
  ZeroMemory(memResNDMesh.ptr, memResNDMesh.format.size);

  NDMeshBuilder* nDMesh = (NDMeshBuilder*)init(
    memResNDMesh,
    numControlParameters,
    numAnalysisProperties,
    sampleCountsPerDimension);

  NMP_VERIFY_MSG(
    memResNDMesh.format.size == 0,
    "The NDMeshBuilder did not initialise all the allocated memory: %d bytes remain.",
    memResNDMesh.format.size);

  //-------------------------------
  // Start and end sample ranges
  for (uint32_t cpIndex = 0; cpIndex < numControlParameters; ++cpIndex)
  {
    NMP_VERIFY(sampleCountsPerDimension[cpIndex] == nDMesh->m_sampleCountsPerDimension[cpIndex]);
    nDMesh->m_controlComponentStartValues[cpIndex] = startRanges[cpIndex];
    nDMesh->m_controlComponentEndValues[cpIndex] = endRanges[cpIndex];

    float sampleRange = endRanges[cpIndex] - startRanges[cpIndex];
    uint32_t numIntervals = nDMesh->m_sampleCountsPerDimension[cpIndex] - 1;
    if (NMP::nmfabs(sampleRange) > 1e-7f)
    {
      nDMesh->m_controlComponentSampleScales[cpIndex] = (float)numIntervals / sampleRange;
    }
    else
    {
      nDMesh->m_controlComponentSampleScales[cpIndex] = 0.0f;
    }
  }

  //-------------------------------
  // Quantisation information
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    nDMesh->m_scalesQuantisedToReal[i] = quantisationData->m_stepSizes[i];
    nDMesh->m_offsetsQuantisedToReal[i] = quantisationData->m_qMinValues[i];
  }

  //-------------------------------
  // Query normalisation
  if (numControlParameters > 0)
  {
    for (uint32_t i = 0; i < numComponentsPerSample; ++i)
    {
      nDMesh->m_scalesRealToNorm[i] = quantisationData->m_scalesRealToNorm[i];
      nDMesh->m_offsetsRealToNorm[i] = quantisationData->m_offsetsRealToNorm[i];

      nDMesh->m_scalesQuantisedToNorm[i] = nDMesh->m_scalesRealToNorm[i] * nDMesh->m_scalesQuantisedToReal[i];
      nDMesh->m_offsetsQuantisedToNorm[i] = nDMesh->m_scalesRealToNorm[i] * nDMesh->m_offsetsQuantisedToReal[i] + nDMesh->m_offsetsRealToNorm[i];
    }
  }

  //-------------------------------
  // Vertex samples
  float sampleCentre[SCATTERED_DATA_MAX_SAMPLE_COMPONENTS];
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    sampleCentre[i] = 0.0f;
  }

  for (uint32_t vertexSampleIndex = 0; vertexSampleIndex < numAnalysedSamples; ++vertexSampleIndex)
  {
    // Get the vertex sample quantised data
    uint32_t offset = vertexSampleIndex * nDMesh->m_numComponentsPerSample;
    uint16_t* vertex = &(nDMesh->m_vertexSamples[offset]);

    // Control parameters
    for (uint32_t cpIndex = 0; cpIndex < numControlParameters; ++cpIndex)
    {
      float cpValue = resultDataNDMesh->getControlParameterSample(cpIndex, vertexSampleIndex);
      uint32_t qValue = NMP::UniformQuantisation::quantise(
        quantisationData->m_qMinValues[cpIndex],
        quantisationData->m_qMaxValues[cpIndex],
        quantisationData->m_recipStepSizes[cpIndex],
        cpValue);
      vertex[cpIndex] = (uint16_t)qValue;

      // Update sample centre
      sampleCentre[cpIndex] += cpValue;
    }

    // Analysis properties
    for (uint32_t apIndex = 0; apIndex < numAnalysisProperties; ++apIndex)
    {
      uint32_t index = apIndex + numControlParameters;
      float apValue = resultDataNDMesh->getAnalysisPropertySample(apIndex, vertexSampleIndex);
      uint32_t qValue = NMP::UniformQuantisation::quantise(
        quantisationData->m_qMinValues[index],
        quantisationData->m_qMaxValues[index],
        quantisationData->m_recipStepSizes[index],
        apValue);
      vertex[index] = (uint16_t)qValue;

      // Update sample centre
      sampleCentre[index] += apValue;
    }
  }

  //-------------------------------
  // Definition data for convenience
  for (uint32_t i = 0; i < numAnalysisProperties; ++i)
  {
    nDMesh->m_analysisPropertyComponentIndices[i] = i + numControlParameters;
  }

  //-------------------------------
  // Sample centre
  float fNumSamples = (float)numAnalysedSamples;
  for (uint32_t index = 0; index < numComponentsPerSample; ++index)
  {
    sampleCentre[index] /= fNumSamples;

    uint32_t qValue;
    qValue = NMP::UniformQuantisation::quantise(
      quantisationData->m_qMinValues[index],
      quantisationData->m_qMaxValues[index],
      quantisationData->m_recipStepSizes[index],
      sampleCentre[index]);
    nDMesh->m_sampleCentre[index] = (uint16_t)qValue;
  }

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
