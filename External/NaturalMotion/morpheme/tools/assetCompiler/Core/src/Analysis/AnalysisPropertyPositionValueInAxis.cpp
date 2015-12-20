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
#include "Analysis/AnalysisPropertyPositionValueInAxis.h"
#include "morpheme/mrNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyPositionValueInAxisBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyPositionValueInAxis), NMP_VECTOR_ALIGNMENT);

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
AnalysisProperty* AnalysisPropertyPositionValueInAxisBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyPositionValueInAxis), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyPositionValueInAxis* result = (AnalysisPropertyPositionValueInAxis*)memRes.alignAndIncrement(memReqsHdr);

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

  // Axis
  uint32_t axis = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(axis, "Axis");
  NMP_VERIFY_MSG(
    axis < kAPAxisMax,
    "Invalid axis attribute");
  result->m_axis = (AnalysisPropertyAxis)axis;

  bool flipAxisDirection = false;
  bool status = analysisNodeDataBlock->readBool(flipAxisDirection, "FlipAxisDirection");
  NMP_VERIFY_MSG(
    status,
    "Invalid flipAxisDirection attribute");
  result->m_flipAxisDirection = flipAxisDirection;

  // Operation
  uint32_t operation = (uint32_t)AnalysisPropertyPositionValueInAxis::kAPInvalidOperation;
  analysisNodeDataBlock->readUInt(operation, "Operation");
  NMP_VERIFY_MSG(
    operation < AnalysisPropertyPositionValueInAxis::kAPNumOperations,
    "Invalid Operation attribute");
  result->m_operation = (AnalysisPropertyPositionValueInAxis::AnalysisOperation)operation;

  // Extracted features
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
void AnalysisPropertyPositionValueInAxisBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyPositionValueInAxis* target = (AnalysisPropertyPositionValueInAxis*)property;
  
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
void AnalysisPropertyPositionValueInAxisBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyPositionValueInAxis* target = (AnalysisPropertyPositionValueInAxis*)property;
  
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
void AnalysisPropertyPositionValueInAxisBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyPositionValueInAxis* target = (AnalysisPropertyPositionValueInAxis*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  
  NMP_VERIFY(target->m_axis < 3);
  NMP::Vector3 axis(0.0f, 0.0f, 0.0f);
  if (target->m_flipAxisDirection)
    axis[target->m_axis] = -1.0f;
  else
    axis[target->m_axis] = 1.0f;

  uint32_t numSamples = 0;
  float* featureData = NULL;
  uint32_t rangeStart, rangeEnd;

  //------------------------
  // Recover the feature data
  if (sampleBuffer->m_mode == kAPModeTrajectory &&
      sampleBuffer->m_trajectorySpace == kAPSpaceTrajectoryLocal)
  {
    numSamples = sampleBuffer->m_numFrames - 1;
    NMP_VERIFY_MSG(
      numSamples > 0,
      "There are an insufficient number of samples to evaluate PositionValueInAxis");

    NMP::Memory::Format memReqs(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource memResF = NMPMemoryAllocateFromFormat(memReqs);
    featureData = (float*)memResF.ptr;

    // Using per frame trajectory delta
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      // Note that the [0] entry should be zero since it was set using absolute time
      NMP::Vector3 deltaPos = sampleBuffer->m_channelPos[i + 1];
      featureData[i] = deltaPos.dot(axis);
    }

    // Sample range buffer
    target->m_subRange->findSampleDiffRange(rangeStart, rangeEnd);
  }
  else
  {
    numSamples = sampleBuffer->m_numFrames;
    NMP::Memory::Format memReqs(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource memResF = NMPMemoryAllocateFromFormat(memReqs);
    featureData = (float*)memResF.ptr;

    // Using accumulated joint position (in appropriate space)
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      NMP::Vector3 pos = sampleBuffer->m_channelPos[i];
      featureData[i] = pos.dot(axis);
    }

    // Sample range buffer
    target->m_subRange->findSampleRange(rangeStart, rangeEnd);
  }

  //------------------------
  // Apply the analysis operation
  target->m_value[0] = 0.0f;
  switch (target->m_operation)
  {
  case AnalysisPropertyPositionValueInAxis::kAPOperationMin:
    {
      float minValue = featureData[rangeStart];
      for (uint32_t i = rangeStart + 1; i <= rangeEnd; ++i)
      {
        minValue = NMP::minimum(minValue, featureData[i]);
      }
      target->m_value[0] = minValue;
    }
    break;

  case AnalysisPropertyPositionValueInAxis::kAPOperationMax:
    {
      float maxValue = featureData[rangeStart];
      for (uint32_t i = rangeStart + 1; i <= rangeEnd; ++i)
      {
        maxValue = NMP::maximum(maxValue, featureData[i]);
      }
      target->m_value[0] = maxValue;
    }
    break;

  case AnalysisPropertyPositionValueInAxis::kAPOperationAverage:
    {
      float sum = 0.0f;
      for (uint32_t i = rangeStart; i <= rangeEnd; ++i)
      {
        sum += featureData[i];
      }
      uint32_t sampleCount = rangeEnd - rangeStart + 1;
      target->m_value[0] = sum / sampleCount;
    }    
    break;

  case AnalysisPropertyPositionValueInAxis::kAPOperationMedian:
    {
      // Allocate a buffer for sorting the data
      NMP::Memory::Format memReqsBuf(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
      NMP::Memory::Resource memResBuf = NMPMemoryAllocateFromFormat(memReqsBuf);
      float* buffer = (float*)memResBuf.ptr;

      // Extract the data using the sub-range
      uint32_t bufferCount = 0;
      for (uint32_t i = rangeStart; i <= rangeEnd; ++i)
      {
        buffer[bufferCount] = featureData[i];
        bufferCount++;
      }

      // Sort the buffer
      for (uint32_t i = 0; i < bufferCount - 1; ++i)
      {
        for (uint32_t j = i + 1; j < bufferCount; ++j)
        {
          if (buffer[j] < buffer[i])
          {
            NMP::nmSwap(buffer[i], buffer[j]);
          }
        }
      }

      uint32_t midIndex = bufferCount >> 1;
      if (bufferCount & 0x01)
      {
        target->m_value[0] = buffer[midIndex];
      }
      else
      {
        target->m_value[0] = 0.5f * (buffer[midIndex] + buffer[midIndex + 1]);
      }

      // Tidy up
      NMP::Memory::memFree(buffer);
    }
    break;
  }

  // Tidy up
  NMP::Memory::memFree(featureData);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyPositionValueInAxisBuilder::calculateRangeDistribution(
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
