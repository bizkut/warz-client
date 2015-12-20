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
#include "Analysis/ControlParameterRegularSampleGrid.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "morpheme/mrNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ControlParameterRegularSampleGrid
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ControlParameterRegularSampleGrid::getMemoryRequirements(
  uint32_t numControlParameters)
{
  NMP::Memory::Format result(sizeof(ControlParameterRegularSampleGrid), NMP_NATURAL_TYPE_ALIGNMENT);

  if (numControlParameters > 0)
  {
    NMP::Memory::Format memReqsCParamIDs(sizeof(MR::NodeID) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsCParamIDs;

    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsSampleCounts;

    NMP::Memory::Format memReqsRng(sizeof(float) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result += (memReqsRng * 2);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlParameterRegularSampleGrid* ControlParameterRegularSampleGrid::init(
  NMP::Memory::Resource& resource,
  uint32_t numControlParameters)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ControlParameterRegularSampleGrid), NMP_NATURAL_TYPE_ALIGNMENT);
  ControlParameterRegularSampleGrid* result = (ControlParameterRegularSampleGrid*)resource.alignAndIncrement(memReqsHdr);

  result->m_numControlParameters = numControlParameters;
  result->m_controlParamNodeIDs = NULL;
  result->m_sampleCounts = NULL;
  result->m_startRanges = NULL;
  result->m_endRanges = NULL;

  if (numControlParameters > 0)
  {
    NMP::Memory::Format memReqsCParamIDs(sizeof(MR::NodeID) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_controlParamNodeIDs = (MR::NodeID*)resource.alignAndIncrement(memReqsCParamIDs);

    NMP::Memory::Format memReqsSampleCounts(sizeof(uint32_t) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_sampleCounts = (uint32_t*)resource.alignAndIncrement(memReqsSampleCounts);

    NMP::Memory::Format memReqsRng(sizeof(float) * numControlParameters, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_startRanges = (float*)resource.alignAndIncrement(memReqsRng);
    result->m_endRanges = (float*)resource.alignAndIncrement(memReqsRng);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlParameterRegularSampleGrid* ControlParameterRegularSampleGrid::createFromExport(
  const ME::AnalysisNodeExport* analysisNodeExport,
  const MR::NetworkDef* networkDef)
{
  NMP_VERIFY(analysisNodeExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisNodeExport->getDataBlock();

  uint32_t numControlParameters = 0;
  bool status = analysisNodeDataBlock->readUInt(numControlParameters, "NumControlParameters");
  NMP_VERIFY_MSG(
    status,
    "Unable to locate regularSampleGrid attribute NumControlParameters");

  NMP::Memory::Format memReqs = getMemoryRequirements(numControlParameters);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ControlParameterRegularSampleGrid* result = init(memRes, numControlParameters);

  CHAR paramName[256];
  for (uint32_t i = 0; i < numControlParameters; ++i)
  {
    // Control parameter node IDs
    std::string cparamName;
    sprintf_s(paramName, "ControlParameter%d", i);
    analysisNodeDataBlock->readString(cparamName, paramName);
    NMP_VERIFY_MSG(
      !cparamName.empty(),
      "Invalid or empty ControlParameter attribute %s",
      paramName);

    // Get the corresponding node ID
    MR::NodeID cparamNodeID = networkDef->getNodeIDFromNodeName(cparamName.c_str());
    NMP_VERIFY_MSG(
      cparamNodeID != MR::INVALID_NODE_ID,
      "Invalid nodeID for attribute %s",
      paramName);
    result->m_controlParamNodeIDs[i] = cparamNodeID;

    // Sample counts
    uint32_t numSamples = 0;
    sprintf_s(paramName, "NumSamples%d", i);
    analysisNodeDataBlock->readUInt(numSamples, paramName);
    NMP_VERIFY_MSG(
      numSamples > 0,
      "There are zero samples for dimension %d", i);
    result->m_sampleCounts[i] = numSamples;

    // Start ranges
    float startRange = 0;
    sprintf_s(paramName, "StartRange%d", i);
    status = analysisNodeDataBlock->readFloat(startRange, paramName);
    NMP_VERIFY_MSG(
      status,
      "Unable to locate regularSampleGrid attribute StartRange%d", i);
    result->m_startRanges[i] = startRange;

    // End ranges
    float endRange = 0;
    sprintf_s(paramName, "EndRange%d", i);
    status = analysisNodeDataBlock->readFloat(endRange, paramName);
    NMP_VERIFY_MSG(
      status,
      "Unable to locate regularSampleGrid attribute EndRange%d", i);
    result->m_endRanges[i] = endRange;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlParameterRegularSampleGrid::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------