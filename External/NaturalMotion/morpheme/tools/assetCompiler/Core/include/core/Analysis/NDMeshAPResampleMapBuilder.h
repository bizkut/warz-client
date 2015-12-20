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
#ifndef MR_ND_MESH_AP_RESAMPLE_MAP_BUILDER_H
#define MR_ND_MESH_AP_RESAMPLE_MAP_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Prediction/mrNDMesh.h"
#include "morpheme/Prediction/mrNDMeshAPResampleMap.h"
#include "Analysis/NDMeshQuantisationData.h"
#include "Analysis/NDMeshResampler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class NDMeshResampleMapProjectionExportData;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NDMeshAPResampleMapBuilder
/// \brief For construction of MR::ScatteredData::NDMeshAPResampleMap in the asset compiler.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshAPResampleMapProjectionDataBuilder : public MR::ScatteredData::NDMeshAPResampleMapProjectionData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDimensions,
    const NDMeshResampleMapProjectionExportData* projectionData);

  static MR::ScatteredData::NDMeshAPResampleMapProjectionData* init(
    NMP::Memory::Resource& resource,
    uint32_t numDimensions,
    const NDMeshResampleMapProjectionExportData* projectionData);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NDMeshAPResampleMapBuilder
/// \brief For construction of MR::ScatteredData::NDMeshAPResampleMap in the asset compiler.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshAPResampleMapBuilder : public MR::ScatteredData::NDMeshAPResampleMap
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const NDMeshResampleMapExportData* nDMeshResampleGrid);

  static MR::ScatteredData::NDMeshAPResampleMap* init(
    NMP::Memory::Resource& resource,
    const NDMeshResampleMapExportData* nDMeshResampleGrid);

  static NMP::Memory::Resource create(
    const MR::ScatteredData::NDMesh* nDMesh,
    const NDMeshResampleMapExportData* nDMeshResampleGrid);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ND_MESH_AP_RESAMPLE_MAP_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
