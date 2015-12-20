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
#include "Analysis/AnalysisPropertyPositionArcDistance.h"
#include "morpheme/mrNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyPositionArcDistanceBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyPositionArcDistance), NMP_VECTOR_ALIGNMENT);

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsValue;

  // Extracted features
  NMP::Memory::Format memReqsFeatures = AnalysisPropertySampleBuffer::getMemoryRequirements(
    numAnimSets,
    numFrames);
  result += memReqsFeatures;

  // Sub-Range
  NMP::Memory::Format memReqsSubRange = AnalysisPropertySubRange::getMemoryRequirements(numFrames);
  result += memReqsSubRange;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisPropertyPositionArcDistanceBuilder::init(
  NMP::Memory::Resource&        memRes,
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* analysisExport,
  float                         sampleFrequency,
  uint32_t                      numFrames)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  NMP_VERIFY(analysisNodeDataBlock);
  MR::AnimSetIndex numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyPositionArcDistance), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyPositionArcDistance* result = (AnalysisPropertyPositionArcDistance*)memRes.alignAndIncrement(memReqsHdr);

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

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = AnalysisPropertySampleBuffer::init(memRes, numAnimSets, numFrames);
  result->m_sampleBuffer = sampleBuffer;
  sampleBuffer->m_sampleFrequency = sampleFrequency;
  sampleBuffer->readExport(networkDef, analysisExport);

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyPositionArcDistanceBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyPositionArcDistance* target = (AnalysisPropertyPositionArcDistance*)property;
  
  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);
  sampleBuffer->instanceInit(network);

  // Sub-Range
  AnalysisPropertySubRange* subRange = target->m_subRange;
  NMP_VERIFY(subRange);
  subRange->instanceInit(network);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyPositionArcDistanceBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyPositionArcDistance* target = (AnalysisPropertyPositionArcDistance*)property;

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);
  sampleBuffer->instanceUpdate(network);

  // Sub-Range
  AnalysisPropertySubRange* subRange = target->m_subRange;
  NMP_VERIFY(subRange);
  subRange->instanceUpdate(network);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyPositionArcDistanceBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyPositionArcDistance* target = (AnalysisPropertyPositionArcDistance*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);

  // Sample range buffer
  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleRange(rangeStart, rangeEnd);

  // Accumulate the arc length distance
  float dist = 0.0f;
  for (uint32_t i = rangeStart + 1; i <= rangeEnd; ++i)
  {
    NMP::Vector3 deltaPos = sampleBuffer->m_channelPos[i] - sampleBuffer->m_channelPos[i - 1];
    float d = deltaPos.magnitude();
    dist += d;
  }
  target->m_value[0] = dist;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyPositionArcDistanceBuilder::calculateRangeDistribution(
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
