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
#include "Analysis/AnalysisPropertySpeed.h"
#include "morpheme/mrNetworkDef.h"
#include "NMNumerics/NMNumericUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertySpeedBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertySpeed), NMP_VECTOR_ALIGNMENT);

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsValue;

  // Extracted features
  NMP::Memory::Format memReqsFeatures = AnalysisPropertySampleBuffer::getMemoryRequirements(
    numAnimSets,
    numFrames);
  result += memReqsFeatures;

  // Data Model
  NMP::Memory::Format memReqsDataModel = AnalysisPropertyDataModel::getMemoryRequirements();
  result += memReqsDataModel;

  // Sub-Range
  NMP::Memory::Format memReqsSubRange = AnalysisPropertySubRange::getMemoryRequirements(numFrames);
  result += memReqsSubRange;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisPropertySpeedBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertySpeed), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertySpeed* result = (AnalysisPropertySpeed*)memRes.alignAndIncrement(memReqsHdr);

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

  // Data Model
  result->m_dataModel = AnalysisPropertyDataModel::init(memRes);
  result->m_dataModel->readExport(analysisNodeDataBlock);

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySpeedBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertySpeed* target = (AnalysisPropertySpeed*)property;

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
void AnalysisPropertySpeedBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertySpeed* target = (AnalysisPropertySpeed*)property;

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
void AnalysisPropertySpeedBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertySpeed* target = (AnalysisPropertySpeed*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  uint32_t numSamples = sampleBuffer->m_numFrames - 1;
  NMP_VERIFY_MSG(
    numSamples > 0,
    "There are an insufficient number of samples to evaluate TravelDirectionInPlane");

  // Default value
  target->m_value[0] = 0.0f;

  NMP::Memory::Format memReqs(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResF = NMPMemoryAllocateFromFormat(memReqs);
  float* featureData = (float*)memResF.ptr;

  if (sampleBuffer->m_mode == kAPModeTrajectory &&
      sampleBuffer->m_trajectorySpace == kAPSpaceTrajectoryLocal)
  {
    // Using per frame trajectory delta
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      // Note that the [0] entry should be zero since it was set using absolute time
      NMP::Vector3 deltaPos = sampleBuffer->m_channelPos[i + 1];
      featureData[i] = deltaPos.magnitude() * sampleBuffer->m_sampleFrequency;
    }
  }
  else
  {
    // Using accumulated joint position (in appropriate space)
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      NMP::Vector3 deltaPos = sampleBuffer->m_channelPos[i + 1] - sampleBuffer->m_channelPos[i];
      featureData[i] = deltaPos.magnitude() * sampleBuffer->m_sampleFrequency;
    }
  }

  // Sample range buffer
  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleDiffRange(rangeStart, rangeEnd);

  // Process the sample data
  target->m_value[0] = target->m_dataModel->fitDataModel(
    numSamples,
    featureData,
    rangeStart,
    rangeEnd);

  // Tidy up
  NMP::Memory::memFree(featureData);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertySpeedBuilder::calculateRangeDistribution(
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
