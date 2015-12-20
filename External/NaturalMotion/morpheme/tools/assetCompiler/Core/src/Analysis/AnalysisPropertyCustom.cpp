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
#include "Analysis/AnalysisPropertyCustom.h"
#include "morpheme/mrNetworkDef.h"
#include "NMNumerics/NMNumericUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyCustomBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      NMP_UNUSED(numFrames))
{
  // Header
  NMP::Memory::Format result(sizeof(AnalysisProperty), NMP_VECTOR_ALIGNMENT);

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsValue;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisPropertyCustomBuilder::init(
  NMP::Memory::Resource&        memRes,
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* analysisExport,
  float                         NMP_UNUSED(sampleFrequency),
  uint32_t                      NMP_UNUSED(numFrames))
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  NMP_VERIFY(analysisNodeDataBlock);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisProperty), NMP_VECTOR_ALIGNMENT);
  AnalysisProperty* result = (AnalysisProperty*)memRes.alignAndIncrement(memReqsHdr);

  // Set the network instance and evaluation handlers
  result->m_instanceInitFn = instanceInit;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceReleaseFn = AnalysisPropertyBuilder::defaultReleaseFn;
  result->m_evaluatePropertyFn = evaluateProperty;

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_value = (float*)memRes.alignAndIncrement(memReqsValue);
  result->m_numDims = 1;
  result->m_value[0] = 0.0f;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyCustomBuilder::instanceInit(
  AnalysisProperty* NMP_UNUSED(property),
  MR::Network* NMP_UNUSED(network))
{
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyCustomBuilder::instanceUpdate(
  AnalysisProperty* NMP_UNUSED(property),
  MR::Network* NMP_UNUSED(network))
{
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyCustomBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisProperty* target = (AnalysisProperty*)property;

  // Default value
  target->m_value[0] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyCustomBuilder::calculateRangeDistribution(
  AssetProcessor*             NMP_UNUSED(processor),
  uint32_t                    numSamples,
  const float*                samples,
  float&                      minVal,
  float&                      maxVal,
  float&                      centreVal)
{
  return processSampleRangeDistribution(
    numSamples,
    samples,
    minVal,
    maxVal,
    centreVal);
}

}
