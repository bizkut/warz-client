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
#include "Analysis/PredictionModelNDMeshBuilder.h"
#include "Analysis/NDMeshBuilder.h"
#include "Analysis/NDMeshQuantisationData.h"
#include "Analysis/NDMeshAPSearchMapBuilder.h"
#include "Analysis/NDMeshResampler.h"
#include "Analysis/NDMeshAPResampleMapBuilder.h"
#include "Analysis/NDMeshResampleMapIntermediteData.h"
#include "morpheme/Prediction/mrPredictionModelNDMesh.h"
//----------------------------------------------------------------------------------------------------------------------

#define DEBUG_NDMESH_MEMORYx

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Class used to allow access to protected members from the builder.
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelNDMeshAccessor : public MR::PredictionModelNDMesh
{
  friend class PredictionModelNDMeshBuilder;
};


//----------------------------------------------------------------------------------------------------------------------
// PredictionModelNDMeshBuilder functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource PredictionModelNDMeshBuilder::init(
  const NDMeshAnalysisData* resultDataNDMesh,
  const NDMeshResampler* resultDataResampler)
{
  NMP::Memory::Resource result = { NULL, NMP::Memory::Format(0, 0) };
  if (!resultDataNDMesh)
    return result;

  // Compute the quantisation information. This is common for both the NDMesh and the APSearchMap.
  NDMeshQuantisationData* quantisationData = NDMeshQuantisationData::create(resultDataNDMesh);
  NMP_VERIFY(quantisationData);

  //-------------------------------
  // Build the ND Mesh
  NMP::Memory::Resource memResNDMesh = NDMeshBuilder::create(resultDataNDMesh, quantisationData);
  MR::ScatteredData::NDMesh* nDMesh = (MR::ScatteredData::NDMesh*)memResNDMesh.ptr;

  //-------------------------------
  // Build the AP search map
  NDMeshBBTree* nDMeshBBTree = NULL;
  NMP::Memory::Resource memResAPSearchMap;
  memResAPSearchMap.ptr = NULL;
  memResAPSearchMap.format.set(0, NMP_NATURAL_TYPE_ALIGNMENT);

  if (resultDataNDMesh->getBuildInverseMap())
  {
    NMP_VERIFY_MSG(
      resultDataNDMesh->getNumControlParameters() > 0,
      "You can't build an inverse AP Search map for a zero dimension prediction model.");

    NMP_VERIFY_MSG(
      resultDataNDMesh->getNumAnalysisProperties() >= resultDataNDMesh->getNumControlParameters(),
      "The prediction model should have at least as many or more analysis properties than control parameters when building an inverse query map.");

    nDMeshBBTree = NDMeshBBTree::create(nDMesh); // Intermediate format
    memResAPSearchMap = NDMeshAPSearchMapBuilder::create(nDMesh, quantisationData, nDMeshBBTree);
  }

  //-------------------------------
  // Build the resample maps
  uint32_t numResampleMaps = 0;
  NMP::Memory::Resource* resampleMapTable = NULL;
  if (resultDataResampler)
  {
    numResampleMaps = resultDataResampler->getNumResampleMaps();

    if (numResampleMaps > 0)
    {
      NMP_VERIFY_MSG(
        resultDataNDMesh->getNumControlParameters() > 0,
        "You can't build an inverse AP Resample map for a zero dimension prediction model.");

      NMP_VERIFY_MSG(
        resultDataNDMesh->getNumAnalysisProperties() >= resultDataNDMesh->getNumControlParameters(),
        "The prediction model should have at least as many or more analysis properties than control parameters when building an inverse query map.");

      // Allocate a temporary container for the resample map resources
      NMP::Memory::Format memReqsRMTable(sizeof(NMP::Memory::Resource) * numResampleMaps, NMP_NATURAL_TYPE_ALIGNMENT);
      NMP::Memory::Resource memResRMTable = NMPMemoryAllocateFromFormat(memReqsRMTable);
      ZeroMemory(memResRMTable.ptr, memResRMTable.format.size);
      resampleMapTable = (NMP::Memory::Resource*)memResRMTable.ptr;

      for (uint32_t i = 0; i < numResampleMaps; ++i)
      {
        const NDMeshResampleMapExportData* resampleMapExportData = resultDataResampler->getResampleMapData(i);
        NMP_VERIFY(resampleMapExportData);

        // Compile a runtime resample map from the intermediate data
        resampleMapTable[i] = NDMeshAPResampleMapBuilder::create(
          nDMesh,
          resampleMapExportData);
      }
    }
  }

  //-------------------------------
  // Get the memory requirements from the prediction model
  NMP::Memory::Format memReqsModelHdr(sizeof(MR::PredictionModelNDMesh), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Format memReqsModel = memReqsModelHdr; // Model container

  // ND Mesh
  memReqsModel += memResNDMesh.format;

  // AP search map
  if (resultDataNDMesh->getBuildInverseMap())
  {
    memReqsModel += memResAPSearchMap.format;
  }

  // Resample maps
  NMP::Memory::Format memReqsRMTable(0, NMP_NATURAL_TYPE_ALIGNMENT);
  if (numResampleMaps > 0)
  {
    // Resample map table
    memReqsRMTable.set(
      sizeof(MR::ScatteredData::NDMeshAPResampleMap*) * numResampleMaps,
      NMP_NATURAL_TYPE_ALIGNMENT);
    memReqsModel += memReqsRMTable;

    // Resample maps
    for (uint32_t i = 0; i < numResampleMaps; ++i)
    {
      memReqsModel += resampleMapTable[i].format;
    }
  }

  //-------------------------------
  NMP::Memory::Resource memResModel = NMPMemoryAllocateFromFormat(memReqsModel);
  ZeroMemory(memResModel.ptr, memResModel.format.size);
  result = memResModel;
  PredictionModelNDMeshAccessor* model = (PredictionModelNDMeshAccessor*)memResModel.alignAndIncrement(memReqsModelHdr);
  void* ptr;

  // Header
  model->setType(PREDICTMODELTYPE_ND_MESH);
  model->setName(NULL); // Will be set externally
  model->m_numAPResampleMaps = numResampleMaps;

  //-------------------------------
  // Relocate the ND mesh
  ptr = memResModel.alignAndIncrement(memResNDMesh.format);
  memcpy(ptr, nDMesh, memResNDMesh.format.size);
  model->m_nDMesh = MR::ScatteredData::NDMesh::relocate(ptr);

  // Relocate the AP search map
  model->m_apSearchMap = NULL;
  if (resultDataNDMesh->getBuildInverseMap())
  {
    ptr = memResModel.alignAndIncrement(memResAPSearchMap.format);
    memcpy(ptr, memResAPSearchMap.ptr, memResAPSearchMap.format.size);
    model->m_apSearchMap = MR::ScatteredData::NDMeshAPSearchMap::relocate(ptr);
  }

  // Relocate the resample maps
  if (numResampleMaps > 0)
  {
    // Resample map table
    model->m_apResampleMaps = (MR::ScatteredData::NDMeshAPResampleMap**)memResModel.alignAndIncrement(memReqsRMTable);

    // Resample maps
    for (uint32_t i = 0; i < numResampleMaps; ++i)
    {
      ptr = memResModel.alignAndIncrement(resampleMapTable[i].format);
      memcpy(ptr, resampleMapTable[i].ptr, resampleMapTable[i].format.size);
      model->m_apResampleMaps[i] = MR::ScatteredData::NDMeshAPResampleMap::relocate(ptr);
    }
  }

  //-------------------------------
  // Memory logging
#ifdef DEBUG_NDMESH_MEMORY
  FILE* fp = fopen("C:\\output.txt", "w");
  if (fp)
  {
    uint32_t numDimensions = model->m_nDMesh->getNumDimensions();
    fprintf(fp, "numDimensions = %d\n", numDimensions);

    const uint32_t* sampleCountsPerDimension = model->m_nDMesh->getSampleCountsPerDimension();
    fprintf(fp, "sampleCountsPerDimension = [");
    for (uint32_t i = 0; i < numDimensions; ++i)
    {
      fprintf(fp, "%d ", sampleCountsPerDimension[i]);
    }
    fprintf(fp, "]\n");

    uint32_t numComponentsPerSample = model->m_nDMesh->getNumComponentsPerSample();
    fprintf(fp, "numComponentsPerSample = %d\n", numComponentsPerSample);

    float D = 100.0f / (float)memReqsModel.size;
    fprintf(fp, "memReqsModelHdr = %d bytes (%f%%)\n",
      (uint32_t)memReqsModelHdr.size,
      (float)memReqsModelHdr.size * D);

    fprintf(fp, "memReqsNDMesh = %d bytes (%f%%)\n",
      (uint32_t)memResNDMesh.format.size,
      (float)memResNDMesh.format.size * D);

    fprintf(fp, "memReqsAPSearchMap = %d bytes (%f%%)\n",
      (uint32_t)memResAPSearchMap.format.size,
      (float)memResAPSearchMap.format.size * D);

    for (uint32_t i = 0; i < numResampleMaps; ++i)
    {
      if (resampleMapTable[i].ptr)
      {
        fprintf(fp, "memReqsAPResampleMap%d = %d bytes (%f%%)\n",
          i,
          (uint32_t)resampleMapTable[i].format.size,
          (float)resampleMapTable[i].format.size * D);
      }
    }

    fprintf(fp, "memReqsModel = %d bytes (%fK)\n",
      (uint32_t)memReqsModel.size,
      (float)memReqsModel.size / 1024.0f);

    fclose(fp);
  }
#endif

  //-------------------------------
  // Tidy up
  if (resampleMapTable)
  {
    // Free the compiled resample maps
    for (uint32_t i = 0; i < numResampleMaps; ++i)
    {
      if (resampleMapTable[i].ptr)
      {
        NMP::Memory::memFree(resampleMapTable[i].ptr);
      }
    }

    // Free the table
    NMP::Memory::memFree(resampleMapTable);
  }

  if (resultDataNDMesh->getBuildInverseMap())
  {
    NMP::Memory::memFree(memResAPSearchMap.ptr);
    NMP::Memory::memFree(nDMeshBBTree);
  }
  NMP::Memory::memFree(nDMesh);
  quantisationData->releaseAndDestroy();

  NMP_VERIFY_MSG(
    memResModel.format.size == 0,
    "The PredictionModelNDMeshBuilder did not initialise all the allocated memory: %d bytes remain.",
    memResModel.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource PredictionModelNDMeshBuilder::init(
  const ME::NodeExport*         NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport*   NMP_UNUSED(netDefExport),
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               NMP_UNUSED(netDef),
  const ME::AnalysisNodeExport* ndMeshNodeExport,
  MR::AnimSetIndex              animSetIndex)
{
  // Read data from the analysisNode that is required to build a PredictionModelNDMesh.
  // This will return NULL if either the number of control parameters or analysis properties is zero.
  NDMeshAnalysisData* resultDataNDMesh = NDMeshAnalysisData::createForPreview(ndMeshNodeExport, animSetIndex);
  NDMeshResampler* resultDataResampler = NDMeshResampler::createForPreview(ndMeshNodeExport, animSetIndex);

  NMP::Memory::Resource result = init(resultDataNDMesh, resultDataResampler);

  resultDataNDMesh->releaseAndDestroy();
  resultDataResampler->releaseAndDestroy();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelNDMeshBuilder::relocate(MR::PredictionModelDef* model)
{
  MR::PredictionModelNDMesh::relocate(model);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
