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
#include "Analysis/AnalysisPropertyStrideLength.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertyStrideLengthBuilder
//----------------------------------------------------------------------------------------------------------------------
uint32_t AnalysisPropertyStrideLengthBuilder::readSyncEventRange(
  const ME::DataBlockExport* analysisNodeDataBlock,
  float& eventRangeStart,
  float& eventRangeEnd)
{
  bool status;
  eventRangeStart = 0.0f;
  eventRangeEnd = 0.0f;
  status = analysisNodeDataBlock->readFloat(eventRangeStart, "EventRangeStart");
  NMP_VERIFY_MSG(
    status,
    "Invalid EventRangeStart attribute");
  status = analysisNodeDataBlock->readFloat(eventRangeEnd, "EventRangeEnd");
  NMP_VERIFY_MSG(
    status,
    "Invalid EventRangeEnd attribute");
  uint32_t numSyncEventsToWatch = 2;
  if (NMP::nmfabs(eventRangeEnd - eventRangeStart) < 1e-5f)
  {
    numSyncEventsToWatch = 1;
  }

  return numSyncEventsToWatch;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyStrideLengthBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* analysisExport,
  uint32_t                      numFrames)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  NMP_VERIFY(analysisNodeDataBlock);
  MR::AnimSetIndex numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyStrideLength), NMP_VECTOR_ALIGNMENT);

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsValue;

  // Sync event range
  float eventRangeStart;
  float eventRangeEnd;
  uint32_t numSyncEventsToWatch = readSyncEventRange(
    analysisNodeDataBlock,
    eventRangeStart,
    eventRangeEnd);

  // Sync event watch buffer
  NMP::Memory::Format memReqsSyncEvtBuf = AnalysisPropertySyncEventWatchBuffer::getMemoryRequirements(
    numFrames, numSyncEventsToWatch);
  result += memReqsSyncEvtBuf;

  // Sample buffer
  NMP::Memory::Format memReqsSampleBuf = AnalysisPropertySampleBuffer::getMemoryRequirements(
    numAnimSets,
    numFrames);
  result += memReqsSampleBuf;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisPropertyStrideLengthBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyStrideLength), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyStrideLength* result = (AnalysisPropertyStrideLength*)memRes.alignAndIncrement(memReqsHdr);

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

  // Sync event range
  float eventRangeStart;
  float eventRangeEnd;
  uint32_t numSyncEventsToWatch = readSyncEventRange(
    analysisNodeDataBlock,
    eventRangeStart,
    eventRangeEnd);

  // Operation
  uint32_t operation = (uint32_t)AnalysisPropertyStrideLength::kAPInvalidOperation;
  analysisNodeDataBlock->readUInt(operation, "Operation");
  NMP_VERIFY_MSG(
    operation < AnalysisPropertyStrideLength::kAPNumOperations,
    "Invalid Operation attribute");
  result->m_operation = (AnalysisPropertyStrideLength::AnalysisOperation)operation;

  // Sync watch buffer
  AnalysisPropertySyncEventWatchBuffer* syncEventWatchBuffer = AnalysisPropertySyncEventWatchBuffer::init(
    memRes, numFrames, numSyncEventsToWatch);
  result->m_syncEventWatchBuffer = syncEventWatchBuffer;
  syncEventWatchBuffer->setSyncEventToWatch(0, eventRangeStart);
  if (numSyncEventsToWatch > 1)
  {
    syncEventWatchBuffer->setSyncEventToWatch(1, eventRangeEnd);
  }

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = AnalysisPropertySampleBuffer::init(memRes, numAnimSets, numFrames);
  result->m_sampleBuffer = sampleBuffer;
  sampleBuffer->m_sampleFrequency = sampleFrequency;
  sampleBuffer->readExport(networkDef, analysisExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyStrideLengthBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyStrideLength* target = (AnalysisPropertyStrideLength*)property;

  // Sync event watch buffer
  AnalysisPropertySyncEventWatchBuffer* syncEventWatchBuffer = target->m_syncEventWatchBuffer;
  NMP_VERIFY(syncEventWatchBuffer);
  syncEventWatchBuffer->instanceInit(network);

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);
  sampleBuffer->instanceInit(network);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyStrideLengthBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyStrideLength* target = (AnalysisPropertyStrideLength*)property;

  // Sync event watch buffer
  AnalysisPropertySyncEventWatchBuffer* syncEventWatchBuffer = target->m_syncEventWatchBuffer;
  NMP_VERIFY(syncEventWatchBuffer);
  syncEventWatchBuffer->instanceUpdate(network);

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);
  sampleBuffer->instanceUpdate(network);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyStrideLengthBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyStrideLength* target = (AnalysisPropertyStrideLength*)property;
  AnalysisPropertySyncEventWatchBuffer* syncEventWatchBuffer = target->m_syncEventWatchBuffer;
  NMP_VERIFY(syncEventWatchBuffer);
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  uint32_t numSamples = sampleBuffer->m_numFrames;

  // Default value
  target->m_value[0] = 0.0f;

  // Condition the sync event watch buffer to remove consecutive triggered events
  syncEventWatchBuffer->conditionBuffer();

  uint32_t numTriggeredEvents = syncEventWatchBuffer->getNumTriggeredEvents();
  if (numTriggeredEvents < 2)
  {
    return;
  }

  uint32_t startEventWatchIndex = 0;
  uint32_t endEventWatchIndex = 1;
  if (syncEventWatchBuffer->getNumSyncEventsToWatch() == 1)
  {
    endEventWatchIndex = 0;
  }

  //------------------------
  // Build the frequency data
  NMP::Memory::Format memReqsDist(sizeof(float) * (numTriggeredEvents - 1), NMP_NATURAL_TYPE_ALIGNMENT);  
  NMP::Memory::Resource memResDists = NMPMemoryAllocateFromFormat(memReqsDist);
  float* dists = (float*)memResDists.ptr;

  // Find the first occurrence of the start event
  uint32_t numBins = 0;
  uint32_t frameIndexStart = syncEventWatchBuffer->findNextTriggeredEventOfType(0, startEventWatchIndex);
  while (frameIndexStart != 0xFFFFFFFF)
  {
    // Find the next occurrence of the end event
    uint32_t frameIndexEnd = syncEventWatchBuffer->findNextTriggeredEventOfType(frameIndexStart + 1, endEventWatchIndex);
    if (frameIndexEnd != 0xFFFFFFFF)
    {
      // Compute the arc distance between the triggered events
      float dist = 0.0f;
      for (uint32_t i = frameIndexStart + 1; i <= frameIndexEnd; ++i)
      {
        NMP::Vector3 deltaPos = sampleBuffer->m_channelPos[i] - sampleBuffer->m_channelPos[i - 1];
        float d = deltaPos.magnitude();
        dist += d;
      }

      // Update the frequency data
      dists[numBins] = dist;
      numBins++;
    }
    else
    {
      break;
    }

    // Find the next occurrence of the start event
    if (startEventWatchIndex != endEventWatchIndex)
    {
      NMP_VERIFY(frameIndexEnd < numSamples);
      frameIndexStart = syncEventWatchBuffer->findNextTriggeredEventOfType(frameIndexEnd + 1, startEventWatchIndex);
    }
    else
    {
      frameIndexStart = frameIndexEnd;
    }
  }

  //------------------------
  // Apply the analysis operation
  if (numBins > 0)
  {
    switch (target->m_operation)
    {
    case AnalysisPropertyStrideLength::kAPOperationMin:
      {
        float minValue = dists[0];
        for (uint32_t i = 1; i < numBins; ++i)
        {
          minValue = NMP::minimum(minValue, dists[i]);
        }
        target->m_value[0] = minValue;
      }
      break;

    case AnalysisPropertyStrideLength::kAPOperationMax:
      {
        float maxValue = dists[0];
        for (uint32_t i = 1; i < numBins; ++i)
        {
          maxValue = NMP::maximum(maxValue, dists[i]);
        }
        target->m_value[0] = maxValue;
      }
      break;

    case AnalysisPropertyStrideLength::kAPOperationAverage:
      {
        float sum = 0.0f;
        for (uint32_t i = 0; i < numBins; ++i)
        {
          sum += dists[i];
        }
        target->m_value[0] = sum / numBins;
      }    
      break;

    case AnalysisPropertyStrideLength::kAPOperationMedian:
      {
        // Sort the buffer
        for (uint32_t i = 0; i < numBins - 1; ++i)
        {
          for (uint32_t j = i + 1; j < numBins; ++j)
          {
            if (dists[j] < dists[i])
            {
              NMP::nmSwap(dists[i], dists[j]);
            }
          }
        }

        uint32_t midIndex = numBins >> 1;
        if (numBins & 0x01)
        {
          target->m_value[0] = dists[midIndex];
        }
        else
        {
          target->m_value[0] = 0.5f * (dists[midIndex] + dists[midIndex + 1]);
        }
      }
      break;
    }
  }

  //------------------------
  // Tidy up
  NMP::Memory::memFree(dists);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyStrideLengthBuilder::calculateRangeDistribution(
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
