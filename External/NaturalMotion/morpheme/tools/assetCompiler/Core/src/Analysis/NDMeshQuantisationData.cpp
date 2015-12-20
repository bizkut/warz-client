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
#include "Analysis/NDMeshQuantisationData.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "NMPlatform/NMSystem.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshQuantisationData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshQuantisationData::getMemoryRequirements(
  const NDMeshAnalysisData* resultDataNDMesh)
{
  NMP_VERIFY(resultDataNDMesh);
  uint32_t numControlParameters = resultDataNDMesh->getNumControlParameters();
  uint32_t numAnalysisProperties = resultDataNDMesh->getNumAnalysisProperties();
  uint32_t numComponentsPerSample = numControlParameters + numAnalysisProperties;

  // Header
  NMP::Memory::Format result(sizeof(NDMeshQuantisationData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Data
  NMP::Memory::Format memReqsQInfo(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsQInfo * 6);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshQuantisationData* NDMeshQuantisationData::init(
  NMP::Memory::Resource& resource,
  const NDMeshAnalysisData* resultDataNDMesh)
{
  NMP_VERIFY(resultDataNDMesh);
  uint32_t numControlParameters = resultDataNDMesh->getNumControlParameters();
  uint32_t numAnalysisProperties = resultDataNDMesh->getNumAnalysisProperties();
  uint32_t numComponentsPerSample = numControlParameters + numAnalysisProperties;

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshQuantisationData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshQuantisationData* result = (NDMeshQuantisationData*)resource.alignAndIncrement(memReqsHdr);
  result->m_numComponentsPerSample = numComponentsPerSample;

  // Data
  NMP::Memory::Format memReqsQInfo(sizeof(float) * numComponentsPerSample, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_qMinValues = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_qMaxValues = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_stepSizes = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_recipStepSizes = (float*)resource.alignAndIncrement(memReqsQInfo);
 
  result->m_scalesRealToNorm = (float*)resource.alignAndIncrement(memReqsQInfo);
  result->m_offsetsRealToNorm = (float*)resource.alignAndIncrement(memReqsQInfo);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshQuantisationData* NDMeshQuantisationData::create(
  const NDMeshAnalysisData* resultDataNDMesh)
{
  NMP_VERIFY(resultDataNDMesh);
  uint32_t numControlParameters = resultDataNDMesh->getNumControlParameters();
  uint32_t numAnalysisProperties = resultDataNDMesh->getNumAnalysisProperties();
  uint32_t numAnalysedSamples = resultDataNDMesh->getNumAnalysedSamples();
  uint32_t numComponentsPerSample = numControlParameters + numAnalysisProperties;

  //------------------------------
  // Allocate the memory
  NMP::Memory::Format memReqs = getMemoryRequirements(resultDataNDMesh);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshQuantisationData* result = init(memRes, resultDataNDMesh);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "The NDMeshQuantisationData did not initialise all the allocated memory: %d bytes remain.",
    memRes.format.size);

  //------------------------------
  // Compute the quantisation data
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    result->m_qMinValues[i] = FLT_MAX;
    result->m_qMaxValues[i] = -FLT_MAX;
  }

  for (uint32_t vertexSampleIndex = 0; vertexSampleIndex < numAnalysedSamples; ++vertexSampleIndex)
  {
    // Control parameters
    for (uint32_t cpIndex = 0; cpIndex < numControlParameters; ++cpIndex)
    {
      float cpValue = resultDataNDMesh->getControlParameterSample(cpIndex, vertexSampleIndex);
      result->m_qMinValues[cpIndex] = NMP::minimum(result->m_qMinValues[cpIndex], cpValue);
      result->m_qMaxValues[cpIndex] = NMP::maximum(result->m_qMaxValues[cpIndex], cpValue);
    }

    // Analysis properties
    for (uint32_t apIndex = 0; apIndex < numAnalysisProperties; ++apIndex)
    {
      uint32_t index = apIndex + numControlParameters;
      float apValue = resultDataNDMesh->getAnalysisPropertySample(apIndex, vertexSampleIndex);
      result->m_qMinValues[index] = NMP::minimum(result->m_qMinValues[index], apValue);
      result->m_qMaxValues[index] = NMP::maximum(result->m_qMaxValues[index], apValue);
    }
  }

  // Compute the quantisation step size for 16-bit samples
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    result->m_stepSizes[i] = NMP::UniformQuantisation::stepSize(
      result->m_qMinValues[i],
      result->m_qMaxValues[i],
      16);
    result->m_recipStepSizes[i] = NMP::UniformQuantisation::recipStepSize(
      result->m_qMinValues[i],
      result->m_qMaxValues[i],
      16);
  }

  //------------------------------
  // Compute the normalisation transforms that maps the sample AABB to the range [-1 : 1]
  for (uint32_t i = 0; i < numComponentsPerSample; ++i)
  {
    // Get the AABB
    float minValue = result->m_qMinValues[i];
    float maxValue = result->m_stepSizes[i] * 65535.0f + result->m_qMinValues[i];

    // Compute the normalisation transform
    float diffValue = maxValue - minValue;
    if (diffValue > 1e-7f)
    {
      result->m_scalesRealToNorm[i] = 2.0f / diffValue;
      result->m_offsetsRealToNorm[i] = -(minValue + maxValue) / diffValue;
    }
    else
    {
      result->m_scalesRealToNorm[i] = 0.0f;
      result->m_offsetsRealToNorm[i] = 0.0f;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshQuantisationData::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------