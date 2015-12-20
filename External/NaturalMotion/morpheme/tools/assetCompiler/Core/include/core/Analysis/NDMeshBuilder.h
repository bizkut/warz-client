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
#ifndef MR_ND_MESH_BUILDER_H
#define MR_ND_MESH_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Prediction/mrNDMesh.h"
#include "Analysis/NDMeshAnalysisData.h"
#include "Analysis/NDMeshQuantisationData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NDMeshBuilder
/// \brief For construction of MR::ScatteredData::NDMesh in the asset compiler.
//----------------------------------------------------------------------------------------------------------------------
class NDMeshBuilder : public MR::ScatteredData::NDMesh
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t              numDimensions,
    uint32_t              numAnalysisProperties,
    const uint32_t*       sampleCountsPerDimension);

  static NDMesh* init(
    NMP::Memory::Resource& memRes,
    uint32_t              numDimensions,
    uint32_t              numAnalysisProperties,
    const uint32_t*       sampleCountsPerDimension);

  static NMP::Memory::Resource create(    
    const NDMeshAnalysisData* resultDataNDMesh,
    const NDMeshQuantisationData* quantisationData);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ND_MESH_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
