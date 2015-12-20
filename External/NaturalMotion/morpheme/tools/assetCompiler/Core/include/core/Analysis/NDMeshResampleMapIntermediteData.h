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
#ifndef AP_NDMESH_RESAMPLEMAP_INTERMEDIATEDATA_H
#define AP_NDMESH_RESAMPLEMAP_INTERMEDIATEDATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "Analysis/NDMeshResampleMapExportData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NDMeshResampleMapIntermediateData
//----------------------------------------------------------------------------------------------------------------------
class NDMeshResampleMapIntermediateData
{
public:
  static NDMeshResampleMapIntermediateData* create(
    const NDMeshResampleMapExportData* resampleMapExportData);

  void releaseAndDestroy();

public:
  uint32_t      m_numDimensions;
  uint32_t      m_numVertexSamples;   // i.e. NumSamples0 * NumSamples1 * ...
  uint32_t      m_numGridCells;       // i.e. (NumSamples0 - 1) * (NumSamples1 - 1) * ...

  uint32_t*     m_nDVertexBlockSizes; // Nd block sizes for the resample map vertex samples
  uint32_t*     m_nDCellBlockSizes;   // Nd block sizes for the resample map NDCell flags

  // Query information
  uint32_t*     m_cpComponentIndices;
  uint32_t*     m_apComponentIndices;

  // Analysis property resample grid
  uint32_t*     m_sampleCountsPerDimension;
  float*        m_startRanges;
  float*        m_endRanges;

  // Control parameters mapped from APs
  float**       m_controlParameterSamples;

  // Grid cell valid flags
  bool*         m_gridCellFlags;
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_NDMESH_RESAMPLEMAP_INTERMEDIATEDATA_H
//----------------------------------------------------------------------------------------------------------------------
