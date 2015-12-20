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
#include "Analysis/NDMeshResampleMapIntermediteData.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NDMeshResampleMapIntermediateData* NDMeshResampleMapIntermediateData::create(
  const NDMeshResampleMapExportData* resampleMapExportData)
{
  NMP_VERIFY(resampleMapExportData);

  //-------------------------------
  // Get the memory requirements for the header
  NMP::Memory::Format memReqsHdr(sizeof(NDMeshResampleMapIntermediateData), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Format memReqs = memReqsHdr;

  //-------------------------------
  // Initialise the memory for the header
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  NDMeshResampleMapIntermediateData* result = (NDMeshResampleMapIntermediateData*)memRes.alignAndIncrement(memReqsHdr);

  //-------------------------------
  // Compile the resample map data
  NMP::Memory::Format memReqsT(sizeof(bool) * 16, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResT = NMPMemoryAllocateFromFormat(memReqsT);
  ZeroMemory(memResT.ptr, memResT.format.size);
  result->m_gridCellFlags = (bool*)memResT.ptr;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NDMeshResampleMapIntermediateData::releaseAndDestroy()
{
  // Free the resample map data
  if (m_gridCellFlags)
  {
    NMP::Memory::memFree(m_gridCellFlags);
  }

  // Free the header data
  NMP::Memory::memFree(this);
}

}
