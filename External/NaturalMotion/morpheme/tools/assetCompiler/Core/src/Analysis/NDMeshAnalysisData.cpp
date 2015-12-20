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
#include "Analysis/NDMeshAnalysisData.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshAnalysisData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshAnalysisData::getMemoryRequirements(
  uint32_t numControlParameters,
  uint32_t numAnalysisProperties,
  const uint32_t* sampleCounts)
{
  // Total number of samples
  uint32_t numAnalysedSamples = 1;
  for (uint32_t i = 0; i < numControlParameters; ++i)
  {
    NMP_VERIFY(sampleCounts[i] > 1);
    numAnalysedSamples *= sampleCounts[i];
  }
  NMP::Memory::Format memReqsSamples(sizeof(float) * numAnalysedSamples, NMP_NATURAL_TYPE_ALIGNMENT);

  // Header
  NMP::Memory::Format result(sizeof(NDMeshAnalysisData), NMP_NATURAL_TYPE_ALIGNMENT);

  if (numControlParameters > 0)
  {
    // Regular grid
    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsSampleCounts;

    NMP::Memory::Format memReqsRng(sizeof(float) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsRng * 2);

    // Control parameters
    NMP::Memory::Format memReqsCPSampleTable(sizeof(float*) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsCPSampleTable;
    result += (memReqsSamples * numControlParameters);
  }

  // Analysis properties
  NMP::Memory::Format memReqsAPSampleTable(sizeof(float*) * numAnalysisProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsAPSampleTable;
  result += (memReqsSamples * numAnalysisProperties);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshAnalysisData* NDMeshAnalysisData::init(
  NMP::Memory::Resource& resource,
  uint32_t numControlParameters,
  uint32_t numAnalysisProperties,
  const uint32_t* sampleCounts)
{
  // Total number of samples
  uint32_t numAnalysedSamples = 1;
  for (uint32_t i = 0; i < numControlParameters; ++i)
  {
    NMP_VERIFY(sampleCounts[i] > 1);
    numAnalysedSamples *= sampleCounts[i];
  }
  NMP::Memory::Format memReqsSamples(sizeof(float) * numAnalysedSamples, NMP_NATURAL_TYPE_ALIGNMENT);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshAnalysisData), NMP_NATURAL_TYPE_ALIGNMENT);
  NDMeshAnalysisData* result = (NDMeshAnalysisData*)resource.alignAndIncrement(memReqsHdr);

  result->m_numControlParameters = numControlParameters;
  result->m_numAnalysisProperties = numAnalysisProperties;
  result->m_numAnalysedSamples = numAnalysedSamples;

  if (numControlParameters > 0)
  {
    // We can only perform inverse prediction if we have at least as many APs as there are CPs.
    result->m_buildInverseMap = (numAnalysisProperties >= numControlParameters);

    // Regular grid
    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_sampleCounts = (uint32_t*)resource.alignAndIncrement(memReqsSampleCounts);
    for (uint32_t i = 0; i < numControlParameters; ++i)
    {
      result->m_sampleCounts[i] = sampleCounts[i];
    }

    NMP::Memory::Format memReqsRng(sizeof(float) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_startRanges = (float*)resource.alignAndIncrement(memReqsRng);
    result->m_endRanges = (float*)resource.alignAndIncrement(memReqsRng);

    // Control parameters
    NMP::Memory::Format memReqsCPSampleTable(sizeof(float*) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_controlParameterSamples = (float**)resource.alignAndIncrement(memReqsCPSampleTable);
    for (uint32_t i = 0; i < numControlParameters; ++i)
    {
      result->m_controlParameterSamples[i] = (float*)resource.alignAndIncrement(memReqsSamples);
    }
  }
  else
  {
    result->m_buildInverseMap = false;
    result->m_sampleCounts = NULL;
    result->m_startRanges = NULL;
    result->m_endRanges = NULL;
    result->m_controlParameterSamples = NULL;
  }

  // Analysis properties
  NMP::Memory::Format memReqsAPSampleTable(sizeof(float*) * numAnalysisProperties, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_analysisPropertySamples = (float**)resource.alignAndIncrement(memReqsAPSampleTable);
  for (uint32_t i = 0; i < numAnalysisProperties; ++i)
  {
    result->m_analysisPropertySamples[i] = (float*)resource.alignAndIncrement(memReqsSamples);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshAnalysisData* NDMeshAnalysisData::createForAnalysis(
  const ControlParameterRegularSampleGrid* regularSampleGrid,
  uint32_t numAnalysisProperties)
{
  NMP_VERIFY(regularSampleGrid);
  NMP_VERIFY(numAnalysisProperties > 0);

  uint32_t numControlParameters = regularSampleGrid->getNumControlParameters();
  const uint32_t* sampleCounts = regularSampleGrid->getSampleCounts();

  NMP::Memory::Format memReqs = getMemoryRequirements(
    numControlParameters,
    numAnalysisProperties,
    sampleCounts);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshAnalysisData* result = init(
    memRes,
    numControlParameters,
    numAnalysisProperties,
    sampleCounts);

  // Regular grid
  for (uint32_t i = 0; i < result->m_numControlParameters; ++i)
  {
    result->m_startRanges[i] = regularSampleGrid->getStartRange(i);
    result->m_endRanges[i] = regularSampleGrid->getEndRange(i);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshAnalysisData* NDMeshAnalysisData::createForPreview(
  const ME::AnalysisNodeExport* analysisExport,
  MR::AnimSetIndex              animSetIndex)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  bool status;

  // Regular grid
  uint32_t numAnalysisProperties = 0;
  status = analysisNodeDataBlock->readUInt(numAnalysisProperties, "NumAnalysisProperties");
  NMP_VERIFY_MSG(
    status,
    "Unable to find NumAnalysisProperties attribute in the export");

  if (numAnalysisProperties == 0)
    return NULL;

  uint32_t numControlParameters = 0;
  status = analysisNodeDataBlock->readUInt(numControlParameters, "NumControlParameters");
  NMP_VERIFY_MSG(
              status,
              "Unable to find NumControlParameters attribute in the export");

  size_t length;
  char paramName[256];
  uint32_t sampleCounts[8];
  bool buildInverseMap = false;

  if (numControlParameters > 0)
  {
    // Inverse prediction maps can only be built if there are at least as many analysis
    // properties as there are controlling parameters.    
    if (numAnalysisProperties >= numControlParameters)
    {
      status = analysisNodeDataBlock->readBool(buildInverseMap, "BuildInverseMap");
      NMP_VERIFY_MSG(
        status,
        "Unable to find BuildInverseMap attribute in the export");
    }

    status = analysisNodeDataBlock->readUIntArray(sampleCounts, 8, length, "SampleCounts");
    NMP_VERIFY_MSG(
      status,
      "Unable to find SampleCounts attribute in the export");
  }

  // Create the analysis data structure
  NMP::Memory::Format memReqs = getMemoryRequirements(
                                numControlParameters,
                                numAnalysisProperties,
                                sampleCounts);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);

  NDMeshAnalysisData* result = init(
                                memRes,
                                numControlParameters,
                                numAnalysisProperties,
                                sampleCounts);

  // Inverse prediction map
  result->m_buildInverseMap = buildInverseMap;

  if (numControlParameters > 0)
  {
    // Regular grid
    status = analysisNodeDataBlock->readFloatArray(
      result->m_startRanges,
      numControlParameters,
      length,
      "StartRanges");
    NMP_VERIFY_MSG(
      status,
      "Unable to find StartRanges attribute in the export");

    status = analysisNodeDataBlock->readFloatArray(
      result->m_endRanges,
      numControlParameters,
      length,
      "EndRanges");
    NMP_VERIFY_MSG(
      status,
      "Unable to find EndRanges attribute in the export");

    // Control parameters
    for (uint32_t i = 0; i < numControlParameters; ++i)
    {
      sprintf_s(paramName, 255, "ControlParameter%d_Samples", i);
      status = analysisNodeDataBlock->readFloatArray(
        result->m_controlParameterSamples[i],
        result->m_numAnalysedSamples,
        length,
        paramName);
      NMP_VERIFY_MSG(
        status,
        "Unable to find %s attribute in the export",
        paramName);
    }
  }

  // Analysis properties
  for (uint32_t i = 0; i < numAnalysisProperties; ++i)
  {
    sprintf_s(paramName, 255, "AnalysisProperty%d_Samples_Set%d", i, animSetIndex);
    status = analysisNodeDataBlock->readFloatArray(
                                        result->m_analysisPropertySamples[i],
                                        result->m_numAnalysedSamples,
                                        length,
                                        paramName);
    if (status == false)
    {
      // There are no analysis properties for this animation set so wipe down and early out.
      result->releaseAndDestroy();
      return NULL;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAnalysisData::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAnalysisData::writeExportData(ME::DataBlockExport* resultData) const
{
  NMP_VERIFY(resultData);

  resultData->writeUInt(m_numControlParameters, "NumControlParameters");
  resultData->writeUInt(m_numAnalysisProperties, "NumAnalysisProperties");
  resultData->writeUInt(m_numAnalysedSamples, "NumAnalysedSamples");

  resultData->writeUIntArray(m_sampleCounts, m_numControlParameters, "SampleCounts");
  resultData->writeFloatArray(m_startRanges, m_numControlParameters, "StartRanges");
  resultData->writeFloatArray(m_endRanges, m_numControlParameters, "EndRanges");

  // Control parameters
  char paramName[256];
  for (uint32_t i = 0; i < m_numControlParameters; ++i)
  {
    sprintf_s(paramName, 255, "ControlParameter%d_Samples", i);
    resultData->writeFloatArray(m_controlParameterSamples[i], m_numAnalysedSamples, paramName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshAnalysisData::writePerSetExportData(MR::AnimSetIndex setIndex, ME::DataBlockExport* resultData) const
{
  // Analysis properties
  char paramName[256];
  for (uint32_t i = 0; i < m_numAnalysisProperties; ++i)
  {
    sprintf_s(paramName, 255, "AnalysisProperty%d_Samples_Set%d", i, setIndex);
    resultData->writeFloatArray(m_analysisPropertySamples[i], m_numAnalysedSamples, paramName);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------