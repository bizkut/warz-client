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
#ifndef AP_NDMESH_QUANTISATION_DATA_H
#define AP_NDMESH_QUANTISATION_DATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "Analysis/NDMeshAnalysisData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class NDMeshQuantisationData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const NDMeshAnalysisData* resultDataNDMesh);

  static NDMeshQuantisationData* init(
    NMP::Memory::Resource& resource,
    const NDMeshAnalysisData* resultDataNDMesh);

  static NDMeshQuantisationData* create(
    const NDMeshAnalysisData* resultDataNDMesh);

  void releaseAndDestroy();

public:
  uint32_t  m_numComponentsPerSample;
  float*    m_qMinValues;
  float*    m_qMaxValues;
  float*    m_stepSizes;
  float*    m_recipStepSizes;

  float*    m_scalesRealToNorm;
  float*    m_offsetsRealToNorm;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_NDMESH_QUANTISATION_DATA_H
//----------------------------------------------------------------------------------------------------------------------
