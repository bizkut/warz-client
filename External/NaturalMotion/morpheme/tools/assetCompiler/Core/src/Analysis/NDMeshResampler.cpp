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
#include "Analysis/NDMeshResampler.h"
#include "Analysis/NDMeshResampleMapExportData.h"
#include "Analysis/ControlParameterRegularSampleGrid.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampler
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NDMeshResampler::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(NDMeshResampler), NMP_VECTOR_ALIGNMENT);

  // Resample grids
  NMP::Memory::Format memReqsGrids = NMP::VectorContainer<NDMeshResampleMapExportData*>::getMemoryRequirements(m_maxNumResampleMaps);
  result += memReqsGrids;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampler* NDMeshResampler::init(NMP::Memory::Resource& resource)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampler), NMP_VECTOR_ALIGNMENT);
  NDMeshResampler* result = (NDMeshResampler*)resource.alignAndIncrement(memReqsHdr);

  // Resample grids
  result->m_resampleMapExportData = NMP::VectorContainer<NDMeshResampleMapExportData*>::init(resource, m_maxNumResampleMaps);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampler::releaseAndDestroy()
{
  // Release all of the resample grids
  NMP::VectorContainer<NDMeshResampleMapExportData*>::iterator it = m_resampleMapExportData->begin();
  for (; it != m_resampleMapExportData->end(); ++it)
  {
    NDMeshResampleMapExportData* resampleMap = *it;
    NMP_VERIFY(resampleMap);
    resampleMap->releaseAndDestroy();
  }

  // Release the analysis parameteriser
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampler* NDMeshResampler::createForAnalysis(
  const ME::AnalysisTaskExport* resampleGridTaskExport,
  const ME::AnalysisNodeExport* ndMeshNodeExport,
  const ControlParameterRegularSampleGrid* regularSampleGrid)
{
  NMP_VERIFY(ndMeshNodeExport);
  const ME::DataBlockExport* ndMeshNodeExportDataBlock = ndMeshNodeExport->getDataBlock();

  // Allocate the memory for the resample grids container
  uint32_t resampleGridCount = 0;
  bool status = ndMeshNodeExportDataBlock->readUInt(resampleGridCount, "ResampleGridCount");
  NMP_VERIFY_MSG(
    status,
    "Unable to find ResampleGridCount attribute in export");
  NMP_VERIFY(resampleGridCount < m_maxNumResampleMaps);

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampler* result = init(memRes);

  // You can't build inverse AP Search or resample maps for a zero dimension prediction model.
  uint32_t numControlParameters = regularSampleGrid->getNumControlParameters();
  if (numControlParameters > 0)
  {
    if (resampleGridCount > 0)
    {
      // Create the individual resample grids
      for (uint32_t i = 0; i < resampleGridCount; ++i)
      {
        char attrName[256];
        sprintf_s(attrName, 256 - 1, "ResampleGrid%d", i);

        std::string resampleGridPath;
        ndMeshNodeExportDataBlock->readString(resampleGridPath, attrName);
        NMP_VERIFY_MSG(
          resampleGridPath.length(),
          "Invalid resample grid export path");

        const ME::AnalysisNodeExport* resampleGridExport = AnalysisProcessor::findAnalysisNodeExport(
          resampleGridTaskExport,
          resampleGridPath.c_str());
        NMP_VERIFY_MSG(
          resampleGridExport,
          "Unable to locate resample grid export %s",
          resampleGridPath.c_str());

        NDMeshResampleMapExportData* resampleMap = NDMeshResampleMapExportData::createForAnalysis(
          resampleGridExport,
          ndMeshNodeExport);

        result->m_resampleMapExportData->push_back(resampleMap);
      }
    }
    else
    {
      // Use the default options on the NDMesh prediction model
      bool buildAPResampleMap = false;
      ndMeshNodeExportDataBlock->readBool(buildAPResampleMap, "BuildAPResampleMap");

      if (buildAPResampleMap)
      {
        NDMeshResampleMapExportData* resampleMap = NDMeshResampleMapExportData::createForAnalysisUsingDefaults(
          ndMeshNodeExport,
          regularSampleGrid);

        result->m_resampleMapExportData->push_back(resampleMap);
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampler* NDMeshResampler::createForPreview(
  const ME::AnalysisNodeExport* ndMeshNodeExport,
  MR::AnimSetIndex animSetIndex)
{
  NMP_VERIFY(ndMeshNodeExport);
  const ME::DataBlockExport* analysisNodeDataBlock = ndMeshNodeExport->getDataBlock();

  // Allocate the memory for the resample grids container
  uint32_t numResampleGrids = 0;
  bool status = analysisNodeDataBlock->readUInt(numResampleGrids, "NumResampleGrids");
  NMP_VERIFY_MSG(
    status,
    "Unable to find NumResampleGrids attribute in export");
  NMP_VERIFY(numResampleGrids < m_maxNumResampleMaps);

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampler* result = init(memRes);

  // Create the individual resample grids
  for (uint32_t resampleMapIndex = 0; resampleMapIndex < numResampleGrids; ++resampleMapIndex)
  {
    NDMeshResampleMapExportData* resampleMap = NDMeshResampleMapExportData::createForPreview(
      ndMeshNodeExport,
      resampleMapIndex,
      animSetIndex);

    result->m_resampleMapExportData->push_back(resampleMap);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampler::analyse(const MR::PredictionModelNDMesh* ndMeshModel)
{
  NMP::VectorContainer<NDMeshResampleMapExportData*>::iterator it = m_resampleMapExportData->begin();
  for (; it != m_resampleMapExportData->end(); ++it)
  {
    NDMeshResampleMapExportData* resampleMap = *it;
    NMP_VERIFY(resampleMap);
    resampleMap->analyse(ndMeshModel);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapExportData* NDMeshResampler::getResampleMapData(uint32_t idx) const
{
  NMP::VectorContainer<NDMeshResampleMapExportData*>::iterator it = m_resampleMapExportData->begin();
  for (uint32_t i = 0; i < idx; ++i)
  {
    if (it == m_resampleMapExportData->end())
    {
      return NULL;
    }
    ++it;
  }

  NDMeshResampleMapExportData* result = *it;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampler::writeExportData(
  ME::DataBlockExport* resultData) const
{
  uint32_t numResampleGrids = m_resampleMapExportData->size();
  resultData->writeUInt(numResampleGrids, "NumResampleGrids");

  NMP::VectorContainer<NDMeshResampleMapExportData*>::iterator it = m_resampleMapExportData->begin();
  for (uint32_t rgIndex = 0; it != m_resampleMapExportData->end(); ++it, ++rgIndex)
  {
    NDMeshResampleMapExportData* resampleMap = *it;
    NMP_VERIFY(resampleMap);
    resampleMap->writeExportData(rgIndex, resultData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampler::writePerSetExportData(
  MR::AnimSetIndex animSetIndex,
  ME::DataBlockExport* resultData) const
{
  NMP::VectorContainer<NDMeshResampleMapExportData*>::iterator it = m_resampleMapExportData->begin();
  for (uint32_t rgIndex = 0; it != m_resampleMapExportData->end(); ++it, ++rgIndex)
  {
    NDMeshResampleMapExportData* resampleMap = *it;
    NMP_VERIFY(resampleMap);
    resampleMap->writePerSetExportData(rgIndex, animSetIndex, resultData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampler::tidyPerSetExportData()
{
  NMP::VectorContainer<NDMeshResampleMapExportData*>::iterator it = m_resampleMapExportData->begin();
  for (; it != m_resampleMapExportData->end(); ++it)
  {
    NDMeshResampleMapExportData* resampleMap = *it;
    NMP_VERIFY(resampleMap);
    resampleMap->tidyPerSetExportData();
  }
}

}
