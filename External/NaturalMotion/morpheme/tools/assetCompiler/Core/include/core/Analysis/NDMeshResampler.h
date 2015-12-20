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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef AP_NDMESH_RESAMPLER_H
#define AP_NDMESH_RESAMPLER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVectorContainer.h"
#include "NMPlatform/NMMemory.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class PredictionModelNDMesh;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class AssetProcessor;
class NDMeshResampleMapExportData;
class ControlParameterRegularSampleGrid;

//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampler
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampler
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static NDMeshResampler* init(NMP::Memory::Resource& resource);

  /// \brief Initialises a resampler from the analysis task export. The NDMesh export
  /// is used to recover the export blocks from the connected resample grid nodes. These
  /// resample grid export blocks provide the description of how the analysis property space
  /// is resampled.
  static NDMeshResampler* createForAnalysis(
    const ME::AnalysisTaskExport* task,
    const ME::AnalysisNodeExport* ndMeshNodeExport,
    const ControlParameterRegularSampleGrid* regularSampleGrid);

  /// \brief Initialises a resampler from the NDMesh result data. The resample grid
  /// result data is stored on the NDMesh node in connect and is used to re-create
  /// the intermediate runtime data.
  static NDMeshResampler* createForPreview(
    const ME::AnalysisNodeExport* ndMeshNodeExport,
    MR::AnimSetIndex animSetIndex);

  void releaseAndDestroy();

  void analyse(const MR::PredictionModelNDMesh* ndMeshModel);

  NM_INLINE NMP::VectorContainer<NDMeshResampleMapExportData*>* getResampleMapExportData() const;

  NM_INLINE uint32_t getNumResampleMaps() const;

  NDMeshResampleMapExportData* getResampleMapData(uint32_t idx) const;

  // Write the result data to the analysis task result data export
  void writeExportData(
    ME::DataBlockExport* resultData) const;

  void writePerSetExportData(
    MR::AnimSetIndex animSetIndex,
    ME::DataBlockExport* resultData) const;

  void tidyPerSetExportData();

private:
  static const uint32_t                                 m_maxNumResampleMaps = 8;

  NMP::VectorContainer<NDMeshResampleMapExportData*>*   m_resampleMapExportData;
};


//----------------------------------------------------------------------------------------------------------------------
// NDMeshResampler inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::VectorContainer<NDMeshResampleMapExportData*>* NDMeshResampler::getResampleMapExportData() const
{
  return m_resampleMapExportData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshResampler::getNumResampleMaps() const
{
  return m_resampleMapExportData->size();
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_NDMESH_RESAMPLER_H
//----------------------------------------------------------------------------------------------------------------------
