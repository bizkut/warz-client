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
#include "Analysis/AnalysisPropertyUtils.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrRig.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
#include "NMNumerics/NMLinearRegression.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnalysisAnimSetDependencyMap
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisAnimSetDependencyMap::getMemoryRequirements(uint32_t numAnimSets)
{
  NMP_VERIFY(numAnimSets > 0);

  // Header
  NMP::Memory::Format result(sizeof(AnalysisAnimSetDependencyMap), NMP_NATURAL_TYPE_ALIGNMENT);
  
  // Entries
  NMP::Memory::Format memReqsT(sizeof(uint32_t) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsT * 3);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisAnimSetDependencyMap* AnalysisAnimSetDependencyMap::init(
  NMP::Memory::Resource& resource,
  uint32_t numAnimSets,
  const uint32_t* animSetDependencyIndices)
{
  NMP_VERIFY(numAnimSets > 0);
  NMP_VERIFY(animSetDependencyIndices);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisAnimSetDependencyMap), NMP_NATURAL_TYPE_ALIGNMENT);
  AnalysisAnimSetDependencyMap* result = (AnalysisAnimSetDependencyMap*)resource.alignAndIncrement(memReqsHdr);
  result->m_numAnimSets = numAnimSets;

  // Entries
  NMP::Memory::Format memReqsT(sizeof(uint32_t) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_animSetDependencyIndices = (uint32_t*)resource.alignAndIncrement(memReqsT);
  result->m_usedAnimSetIndices = (uint32_t*)resource.alignAndIncrement(memReqsT);
  result->m_animSetEntryMap = (uint32_t*)resource.alignAndIncrement(memReqsT);

  // Dependency indices
  for (uint32_t i = 0; i < numAnimSets; ++i)
  {
    result->m_animSetDependencyIndices[i] = animSetDependencyIndices[i];
    result->m_usedAnimSetIndices[i] = 0xFFFFFFFF;
    result->m_animSetEntryMap[i] = 0xFFFFFFFF;
  }

  // Compute the used anim sets  
  uint32_t numUsedAnimSets = 0;
  for (uint32_t i = 0; i < numAnimSets; ++i)
  {
    if (animSetDependencyIndices[i] == i)
    {
      result->m_usedAnimSetIndices[numUsedAnimSets] = i;
      numUsedAnimSets++;
    }
  }
  result->m_numUsedAnimSets = numUsedAnimSets;

  // Compute the anim set entry map
  for (uint32_t i = 0; i < numUsedAnimSets; ++i)
  {
    uint32_t idx = result->m_usedAnimSetIndices[i];
    result->m_animSetEntryMap[idx] = i;
  }
  for (uint32_t i = 0; i < numAnimSets; ++i)
  {
    uint32_t depIdx = animSetDependencyIndices[i];
    if (depIdx != 0xFFFFFFFF)
    {
      result->m_animSetEntryMap[i] = result->m_animSetEntryMap[depIdx];
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisAnimSetDependencyMap* AnalysisAnimSetDependencyMap::createFromExport(
  const ME::DataBlockExport* analysisNodeDataBlock)
{
  NMP_VERIFY(analysisNodeDataBlock);

  // Read the anim set dependency indices from the export
  uint32_t animSetDependencyIndices[128];
  size_t numAnimSets = 0;
  bool status = analysisNodeDataBlock->readUIntArray(
    animSetDependencyIndices,
    128,
    numAnimSets,
    "AnimSetDependencyIndices");
  NMP_VERIFY_MSG(
    status,
    "The attribute AnimSetDependencyIndices does not exist in the export");
  NMP_VERIFY_MSG(
    numAnimSets > 0,
    "Expecting one or more animation sets");

  // Create the anim set dependency map
  NMP::Memory::Format memReqs = getMemoryRequirements((uint32_t)numAnimSets);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  AnalysisAnimSetDependencyMap* result = init(
    memRes,
    (uint32_t)numAnimSets,
    animSetDependencyIndices);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "Not all of the allocated memory was used when creating the anim set dependency map: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisAnimSetDependencyMap::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertyDataModel
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyDataModel::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyDataModel), NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertyDataModel* AnalysisPropertyDataModel::init(NMP::Memory::Resource& resource)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyDataModel), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyDataModel* result = (AnalysisPropertyDataModel*)resource.alignAndIncrement(memReqsHdr);

  result->m_dataModelMode = kAPDataModelConstant;
  result->m_dataModelParam = 0.0f;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyDataModel::readExport(const ME::DataBlockExport* dataBlock)
{
  NMP_VERIFY(dataBlock);
  bool status;
  uint32_t iValue;
  float fValue;

  iValue = (uint32_t)kAPInvalidDataModel;
  status = dataBlock->readUInt(iValue, "DataModel");
  NMP_VERIFY_MSG(
    status,
    "Unable to locate the DataModel mode attribute in the export");

  NMP_VERIFY_MSG(
    iValue < kAPDataModelMax,
    "Invalid data model mode attribute");
  m_dataModelMode = (AnalysisPropertyDataModelMode)iValue;

  m_dataModelParam = 0.0f;
  if (m_dataModelMode == kAPDataModelLinear)
  {
    iValue = (uint32_t)kAPInvalidDataModelEvaluation;
    status = dataBlock->readUInt(iValue, "DataModelEvaluation");
    NMP_VERIFY_MSG(
      status,
      "Unable to locate data model evaluation attribute in the export");
    NMP_VERIFY_MSG(
      iValue < kAPDataModelEvaluationMax,
      "Invalid data model evaluation attribute");

    switch (iValue)
    {
    case kAPDataModelEvaluationAtStart:
      m_dataModelParam = 0.0f;
      break;

    case kAPDataModelEvaluationAtEnd:
      m_dataModelParam = 1.0f;
      break;

    case kAPDataModelEvaluationCustom:
      fValue = 0.0f;
      status = dataBlock->readFloat(fValue, "DataModelParam");
      NMP_VERIFY_MSG(
        status,
        "Unable to locate data model param attribute in export");
      m_dataModelParam = fValue;
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyDataModel::fitDataModelConstant(
  uint32_t numSamples,
  const float* samples,
  uint32_t rangeStart,
  uint32_t rangeEnd)
{
  NMP_VERIFY(numSamples > 0);
  NMP_VERIFY(samples);
  NMP_VERIFY(rangeEnd >= rangeStart);
  NMP_VERIFY(rangeEnd < numSamples);

  uint32_t sampleRangeCount = rangeEnd - rangeStart + 1;
  NMP::Memory::Format memReqs(sizeof(float) * sampleRangeCount, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResS = NMPMemoryAllocateFromFormat(memReqs);
  float* smoothedData = (float*)memResS.ptr;

  // Averaging smoothing kernel
  const uint32_t wndHalfSize = 5;
  float kernel[wndHalfSize] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; // Kernel need not be unit norm

  // Get the active kernel size
  uint32_t activeWndHalfSize = NMP::minimum(wndHalfSize, numSamples);

  //---------------------------
  // Apply smoothing over the sample range to reduce the effects of one off outliers.
  for (uint32_t i = 0; i < sampleRangeCount; ++i)
  {
    smoothedData[i] = AnalysisPropertyUtils::applySmoothingFilterAtFrame(
      activeWndHalfSize,
      kernel,
      rangeStart + i,
      numSamples,
      samples);
  }

  // Fit the smoothed sample data to a constant value
  float result = AnalysisPropertyUtils::averageOfSamples(
    sampleRangeCount,
    smoothedData);

  // Tidy up
  NMP::Memory::memFree(smoothedData);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyDataModel::fitDataModelLinear(
  uint32_t numSamples,
  const float* samples,
  uint32_t rangeStart,
  uint32_t rangeEnd,
  float paramU)
{
  NMP_VERIFY(numSamples > 0);
  NMP_VERIFY(samples);
  NMP_VERIFY(rangeEnd >= rangeStart);
  NMP_VERIFY(rangeEnd < numSamples);
  NMP_VERIFY(paramU >= 0.0f && paramU <= 1.0f);

  uint32_t sampleRangeCount = rangeEnd - rangeStart + 1;
  NMP::Memory::Format memReqs(sizeof(float) * sampleRangeCount, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResS = NMPMemoryAllocateFromFormat(memReqs);
  float* smoothedData = (float*)memResS.ptr;

  // Get the active kernel size
  const uint32_t wndHalfSize = 5;
  uint32_t activeWndHalfSize = NMP::minimum(wndHalfSize, numSamples);

  //---------------------------
  // Apply data model preserving smoothing over the sample range to reduce the effects
  // of one off outliers.
  for (uint32_t i = 0; i < sampleRangeCount; ++i)
  {
    smoothedData[i] = AnalysisPropertyUtils::applyLinearFilterAtFrame(
      activeWndHalfSize,
      rangeStart + i,
      numSamples,
      samples);
  }

  // Fit the smoothed sample data to a straight line and evaluate at the model parameter
  float fFrame = paramU * (sampleRangeCount - 1);
  float result = AnalysisPropertyUtils::linearRegressionSample(
    sampleRangeCount,
    smoothedData,
    fFrame);

  // Tidy up
  NMP::Memory::memFree(smoothedData);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyDataModel::fitDataModel(
  uint32_t numSamples,
  const float* samples,
  uint32_t rangeStart,
  uint32_t rangeEnd) const
{
  float result = 0.0f;
  switch(m_dataModelMode)
  {
  case kAPDataModelConstant:
    result = fitDataModelConstant(
      numSamples,
      samples,
      rangeStart,
      rangeEnd);
    break;

  case kAPDataModelLinear:
    result = fitDataModelLinear(
      numSamples,
      samples,
      rangeStart,
      rangeEnd,
      m_dataModelParam);
    break;

  default:
    NMP_VERIFY_FAIL("Invalid data model mode");
  }

  return result;
}


//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertySubRange
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertySubRange::getMemoryRequirements(uint32_t numFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertySubRange), NMP_VECTOR_ALIGNMENT);

  // Extracted features
  if (numFrames > 0)
  {
    NMP::Memory::Format memReqsFlags(sizeof(bool) * numFrames, NMP_VECTOR_ALIGNMENT);
    result += memReqsFlags;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertySubRange* AnalysisPropertySubRange::init(
  NMP::Memory::Resource& resource,
  uint32_t numFrames)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertySubRange), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertySubRange* result = (AnalysisPropertySubRange*)resource.alignAndIncrement(memReqsHdr);

  result->m_sampleRangeMode = kAPSampleRangeDefault;
  result->m_maxNumFrames = numFrames;
  result->m_numFrames = 0;

  // Extracted features
  if (numFrames > 0)
  {
    NMP::Memory::Format memReqsFlags(sizeof(bool) * numFrames, NMP_VECTOR_ALIGNMENT);
    result->m_channelFlags = (bool*)resource.alignAndIncrement(memReqsFlags);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySubRange::readExport(const ME::DataBlockExport* dataBlock)
{
  NMP_VERIFY(dataBlock);
  bool status;
  uint32_t iValue;
  float fValue;

  //---------------------------
  // Sample Range Mode
  iValue = (uint32_t)kAPInvalidSampleRange;
  status = dataBlock->readUInt(iValue, "SampleRangeMode");
  NMP_VERIFY_MSG(
    status,
    "Unable to locate sample range mode attribute in the export");
  NMP_VERIFY_MSG(
    iValue < kAPSampleRangeMax,
    "Invalid sample range mode attribute");
  m_sampleRangeMode = (AnalysisPropertySampleRange)iValue;

  //---------------------------
  // Between sync events
  if (m_sampleRangeMode == kAPSampleRangeBetweenSyncEvents)
  {
    fValue = 0.0f;
    status = dataBlock->readFloat(fValue, "SampleRangeSyncEventStart");
    NMP_VERIFY_MSG(
      status,
      "Unable to locate sample range sync event start attribute in export");
    m_syncEventStart = fValue;

    fValue = 0.0f;
    status = dataBlock->readFloat(fValue, "SampleRangeSyncEventEnd");
    NMP_VERIFY_MSG(
      status,
      "Unable to locate sample range sync event end attribute in export");
    m_syncEventEnd = fValue;
  }
  else
  {
    m_syncEventStart = m_syncEventEnd = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySubRange::instanceInit(MR::Network* network)
{
  // Reset the frame count
  m_numFrames = 0;

  // Analysis root node post update attrib access
  if (m_sampleRangeMode == kAPSampleRangeBetweenSyncEvents)
  {
    const MR::NetworkDef* networkDef = network->getNetworkDef();
    MR::NodeID sourceNodeID = networkDef->getRootNodeID();
    NMP_VERIFY(sourceNodeID != MR::INVALID_NODE_ID);

    // Attrib access
    m_sourceNodeSyncEventPlaybackPosAddress.init(
      MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
      sourceNodeID,
      MR::INVALID_NODE_ID,
      MR::VALID_FRAME_ANY_FRAME,
      MR::ANIMATION_SET_ANY);
    network->addPostUpdateAccessAttrib(
      m_sourceNodeSyncEventPlaybackPosAddress.m_owningNodeID,
      m_sourceNodeSyncEventPlaybackPosAddress.m_semantic,
      1);

    m_sourceNodeSyncEventTrackAddress.init(
      MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
      sourceNodeID,
      MR::INVALID_NODE_ID,
      MR::VALID_FRAME_ANY_FRAME,
      MR::ANIMATION_SET_ANY);
    network->addPostUpdateAccessAttrib(
      m_sourceNodeSyncEventTrackAddress.m_owningNodeID,
      m_sourceNodeSyncEventTrackAddress.m_semantic,
      1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySubRange::instanceUpdate(MR::Network* network)
{
  NMP_VERIFY_MSG(
    m_numFrames < m_maxNumFrames,
    "Attempting to set a sample outside of the allocated buffer size");

  // Sample Range
  if (m_sampleRangeMode == kAPSampleRangeBetweenSyncEvents)
  {
    MR::AttribDataUpdateSyncEventPlaybackPos* syncPlaybackPos = static_cast<MR::AttribDataUpdateSyncEventPlaybackPos*>(
      network->getAttribDataRecurseFilterNodes(m_sourceNodeSyncEventPlaybackPosAddress, true));

    bool result = false;
    if (syncPlaybackPos)
    {
      MR::AttribDataSyncEventTrack* syncEventTrack = static_cast<MR::AttribDataSyncEventTrack*>(
        network->getAttribDataRecurseFilterNodes(m_sourceNodeSyncEventTrackAddress, true));

      float positionRangeEnd = syncPlaybackPos->m_absPosReal.index() + syncPlaybackPos->m_absPosReal.fraction();
      float delta = syncPlaybackPos->m_deltaPos.fraction();
      float positionRangeStart = positionRangeEnd - delta;

      // Evaluate the transition condition
      result = MR::TransitConditionDefInSyncEventRange::evaluateCondition(
        m_syncEventStart,
        m_syncEventEnd,
        false,
        positionRangeStart,
        positionRangeEnd,
        syncEventTrack);
    }

    m_channelFlags[m_numFrames] = result;
  }
  else
  {
    m_channelFlags[m_numFrames] = true;
  }

  // Increment the sample count
  m_numFrames++;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySubRange::findSampleRange(uint32_t& rangeStart, uint32_t& rangeEnd) const
{
  NMP_VERIFY(m_numFrames > 0);

  switch (m_sampleRangeMode)
  {
  case kAPSampleRangeDefault:
    rangeStart = 0;
    rangeEnd = m_numFrames - 1;
    break;

  case kAPSampleRangeAtStart:
    rangeStart = rangeEnd = 0;
    break;

  case kAPSampleRangeAtEnd:
    rangeStart = rangeEnd = m_numFrames - 1;
    break;

  case kAPSampleRangeBetweenSyncEvents:
    rangeStart = 0;
    for (uint32_t i = 0; i < m_numFrames; ++i)
    {
      if (m_channelFlags[i])
      {
        rangeStart = i;
        break;
      }
    }

    rangeEnd = m_numFrames - 1;
    for (int32_t i = m_numFrames - 1; i >= 0; --i)
    {
      if (m_channelFlags[i])
      {
        rangeEnd = i;
        break;
      }
    }
    break;

  default:
    NMP_VERIFY_FAIL("Invalid sample range mode");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySubRange::findSampleDiffRange(uint32_t& rangeStart, uint32_t& rangeEnd) const
{
  NMP_VERIFY(m_numFrames > 1);
  uint32_t numSamples = m_numFrames - 1;

  switch (m_sampleRangeMode)
  {
  case kAPSampleRangeDefault:
    rangeStart = 0;
    rangeEnd = numSamples - 1;
    break;

  case kAPSampleRangeAtStart:
    rangeStart = rangeEnd = 0;
    break;

  case kAPSampleRangeAtEnd:
    rangeStart = rangeEnd = numSamples - 1;
    break;

  case kAPSampleRangeBetweenSyncEvents:
    rangeStart = 0;
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      if (m_channelFlags[i] || m_channelFlags[i + 1])
      {
        rangeStart = i;
        break;
      }
    }

    rangeEnd = numSamples - 1;
    for (int32_t i = numSamples - 1; i >= 0; --i)
    {
      if (m_channelFlags[i] || m_channelFlags[i + 1])
      {
        rangeEnd = i;
        break;
      }
    }
    break;

  default:
    NMP_VERIFY_FAIL("Invalid sample range mode");
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertySyncEventWatchBuffer
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertySyncEventWatchBuffer::getMemoryRequirements(
  uint32_t numFrames,
  uint32_t numSyncEventsToWatch)
{
  NMP_VERIFY(numFrames > 0);
  NMP_VERIFY(numSyncEventsToWatch > 0);

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertySyncEventWatchBuffer), NMP_VECTOR_ALIGNMENT);

  // Sync events to watch
  NMP::Memory::Format memReqsEvts(sizeof(float) * numSyncEventsToWatch, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsEvts;

  // Extracted features
  NMP::Memory::Format memReqsWatchBuffer(sizeof(uint32_t) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsWatchBuffer;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertySyncEventWatchBuffer* AnalysisPropertySyncEventWatchBuffer::init(
  NMP::Memory::Resource& resource,
  uint32_t numFrames,
  uint32_t numSyncEventsToWatch)
{
  NMP_VERIFY(numFrames > 0);
  NMP_VERIFY(numSyncEventsToWatch > 0);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertySyncEventWatchBuffer), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertySyncEventWatchBuffer* result = (AnalysisPropertySyncEventWatchBuffer*)resource.alignAndIncrement(memReqsHdr);

  result->m_numSyncEventsToWatch = numSyncEventsToWatch;
  result->m_maxNumFrames = numFrames;
  result->m_numFrames = 0;

  // Sync events to watch
  NMP::Memory::Format memReqsEvts(sizeof(float) * numSyncEventsToWatch, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_syncEventsToWatch = (float*)resource.alignAndIncrement(memReqsEvts);

  // Extracted features
  NMP::Memory::Format memReqsWatchBuffer(sizeof(uint32_t) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_syncEventWatchBuffer = (uint32_t*)resource.alignAndIncrement(memReqsWatchBuffer);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertySyncEventWatchBuffer::getSyncEventToWatch(uint32_t watchIndex) const
{
  NMP_VERIFY(watchIndex < m_numSyncEventsToWatch);
  return m_syncEventsToWatch[watchIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySyncEventWatchBuffer::setSyncEventToWatch(uint32_t watchIndex, float syncEventPos)
{
  NMP_VERIFY(watchIndex < m_numSyncEventsToWatch);
  m_syncEventsToWatch[watchIndex] = syncEventPos;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySyncEventWatchBuffer::instanceInit(MR::Network* network)
{
  // Reset the frame count
  m_numFrames = 0;

  // Analysis root node post update attrib access
  const MR::NetworkDef* networkDef = network->getNetworkDef();
  MR::NodeID sourceNodeID = networkDef->getRootNodeID();
  NMP_VERIFY(sourceNodeID != MR::INVALID_NODE_ID);

  // Attrib access
  m_sourceNodeSyncEventPlaybackPosAddress.init(
    MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
    sourceNodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME,
    MR::ANIMATION_SET_ANY);
  network->addPostUpdateAccessAttrib(
    m_sourceNodeSyncEventPlaybackPosAddress.m_owningNodeID,
    m_sourceNodeSyncEventPlaybackPosAddress.m_semantic,
    1);

  m_sourceNodeSyncEventTrackAddress.init(
    MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
    sourceNodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME,
    MR::ANIMATION_SET_ANY);
  network->addPostUpdateAccessAttrib(
    m_sourceNodeSyncEventTrackAddress.m_owningNodeID,
    m_sourceNodeSyncEventTrackAddress.m_semantic,
    1);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySyncEventWatchBuffer::instanceUpdate(MR::Network* network)
{
  NMP_VERIFY_MSG(
    m_numFrames < m_maxNumFrames,
    "Attempting to set a sample outside of the allocated buffer size");

  MR::AttribDataUpdateSyncEventPlaybackPos* syncPlaybackPos = static_cast<MR::AttribDataUpdateSyncEventPlaybackPos*>(
    network->getAttribDataRecurseFilterNodes(m_sourceNodeSyncEventPlaybackPosAddress, true));

  uint32_t watchIndex = 0xFFFFFFFF;
  if (syncPlaybackPos)
  {
    MR::AttribDataSyncEventTrack* syncEventTrack = static_cast<MR::AttribDataSyncEventTrack*>(
      network->getAttribDataRecurseFilterNodes(m_sourceNodeSyncEventTrackAddress, true));

    float positionRangeEnd = syncPlaybackPos->m_absPosReal.index() + syncPlaybackPos->m_absPosReal.fraction();
    float delta = syncPlaybackPos->m_deltaPos.fraction();
    float positionRangeStart = positionRangeEnd - delta;

    // Evaluate the transition condition
    for (uint32_t i = 0; i < m_numSyncEventsToWatch; ++i)
    {
      if (MR::TransitConditionDefInSyncEventRange::evaluateCondition(
        m_syncEventsToWatch[i],
        m_syncEventsToWatch[i],
        false,
        positionRangeStart,
        positionRangeEnd,
        syncEventTrack))
      {
        watchIndex = i;
        break;
      }
    }
  }

  m_syncEventWatchBuffer[m_numFrames] = watchIndex;

  // Increment the sample count
  m_numFrames++;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySyncEventWatchBuffer::conditionBuffer()
{
  for (uint32_t i = 1; i < m_numFrames; ++i)
  {
    // Prevent two consecutive triggered events of the same watch type
    if (m_syncEventWatchBuffer[i] != 0xFFFFFFFF)
    {
      if (m_syncEventWatchBuffer[i] == m_syncEventWatchBuffer[i - 1])
      {
        m_syncEventWatchBuffer[i] = 0xFFFFFFFF;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnalysisPropertySyncEventWatchBuffer::getNumTriggeredEvents() const
{
  uint32_t count = 0;
  for (uint32_t i = 0; i < m_numFrames; ++i)
  {
    if (m_syncEventWatchBuffer[i] != 0xFFFFFFFF)
    {
      count++;
    }
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnalysisPropertySyncEventWatchBuffer::getNumTriggeredEventsOfType(uint32_t watchIndex) const
{
  NMP_VERIFY(watchIndex < m_numSyncEventsToWatch);

  uint32_t count = 0;
  for (uint32_t i = 0; i < m_numFrames; ++i)
  {
    if (m_syncEventWatchBuffer[i] == watchIndex)
    {
      count++;
    }
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnalysisPropertySyncEventWatchBuffer::findNextTriggeredEvent(uint32_t frameIndex) const
{
  for (; frameIndex < m_numFrames; ++frameIndex)
  {
    if (m_syncEventWatchBuffer[frameIndex] != 0xFFFFFFFF)
    {
      return frameIndex;
    }
  }
  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnalysisPropertySyncEventWatchBuffer::findNextTriggeredEventOfType(uint32_t frameIndex, uint32_t watchIndex) const
{
  NMP_VERIFY(watchIndex < m_numSyncEventsToWatch);

  for (; frameIndex < m_numFrames; ++frameIndex)
  {
    if (m_syncEventWatchBuffer[frameIndex] == watchIndex)
    {
      return frameIndex;
    }
  }
  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertySampleBuffer
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertySampleBuffer::getMemoryRequirements(
  uint32_t numAnimSets,
  uint32_t maxNumFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertySampleBuffer), NMP_VECTOR_ALIGNMENT);

  // Joint indices
  NMP_VERIFY(numAnimSets > 0);
  NMP::Memory::Format memReqsJoint(sizeof(uint32_t) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsJoint;

  // Extracted features
  if (maxNumFrames > 0)
  {
    NMP::Memory::Format memReqsPos(sizeof(NMP::Vector3) * maxNumFrames, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Format memReqsAtt(sizeof(NMP::Quat) * maxNumFrames, NMP_VECTOR_ALIGNMENT);
    result += memReqsPos;
    result += memReqsAtt;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertySampleBuffer* AnalysisPropertySampleBuffer::init(
  NMP::Memory::Resource& resource,
  uint32_t numAnimSets,
  uint32_t maxNumFrames)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertySampleBuffer), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertySampleBuffer* result = (AnalysisPropertySampleBuffer*)resource.alignAndIncrement(memReqsHdr);

  // Character trajectory position
  result->m_characterPosition.setToZero();
  result->m_characterOrientation.identity();

  // Definition data
  result->m_mode = kAPInvalidMode;
  result->m_trajectorySpace = kAPInvalidTrajectorySpace;

  // Rig joint properties (per anim set)
  result->m_jointSpace = kAPInvalidJointSpace;
  result->m_numAnimSets = numAnimSets;
  result->m_rigJointIndices = NULL;

  // Extracted features
  result->m_animSetIndex = 0;
  result->m_sampleFrequency = 0.0f;
  result->m_maxNumFrames = maxNumFrames;
  result->m_numFrames = 0;
  result->m_channelPos = NULL;
  result->m_channelAtt = NULL;

  // Joint indices
  NMP_VERIFY(numAnimSets > 0);
  NMP::Memory::Format memReqsJoint(sizeof(uint32_t) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_rigJointIndices = (uint32_t*)resource.alignAndIncrement(memReqsJoint);

  // Extracted features
  if (maxNumFrames > 0)
  {
    NMP::Memory::Format memReqsPos(sizeof(NMP::Vector3) * maxNumFrames, NMP_VECTOR_ALIGNMENT);
    result->m_channelPos = (NMP::Vector3*)resource.alignAndIncrement(memReqsPos);

    NMP::Memory::Format memReqsAtt(sizeof(NMP::Quat) * maxNumFrames, NMP_VECTOR_ALIGNMENT);
    result->m_channelAtt = (NMP::Quat*)resource.alignAndIncrement(memReqsAtt);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertySampleBuffer* AnalysisPropertySampleBuffer::create(
  uint32_t numAnimSets,
  uint32_t maxNumFrames)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(numAnimSets, maxNumFrames);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ZeroMemory(memRes.ptr, memRes.format.size);
  AnalysisPropertySampleBuffer* result = init(memRes, numAnimSets, maxNumFrames);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySampleBuffer::readExport(
  MR::NetworkDef* networkDef,
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  NMP_VERIFY(analysisNodeDataBlock);
  MR::AnimSetIndex numAnimSets = networkDef->getNumAnimSets();

  //---------------------------
  // Mode
  uint32_t mode = (uint32_t)kAPInvalidMode;
  analysisNodeDataBlock->readUInt(mode, "Mode");
  NMP_VERIFY_MSG(
    mode < kAPModeMax,
    "Invalid mode attribute");
  m_mode = (AnalysisPropertyMode)mode;

  //---------------------------
  if (m_mode == kAPModeTrajectory)
  {
    uint32_t space = (uint32_t)kAPInvalidTrajectorySpace;
    analysisNodeDataBlock->readUInt(space, "TrajectorySpace");
    NMP_VERIFY_MSG(
      space < kAPSpaceTrajectoryMax,
      "Invalid trajectory space attribute");
    m_trajectorySpace = (AnalysisPropertyTrajectorySpace)space;

    for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
    {
      m_rigJointIndices[animSetIndex] = MR::AnimRigDef::INVALID_BONE_INDEX;
    }
  }
  else if (m_mode == kAPModeRigJoint)
  {
    uint32_t space = (uint32_t)kAPInvalidJointSpace;
    analysisNodeDataBlock->readUInt(space, "JointSpace");
    NMP_VERIFY_MSG(
      space < kAPSpaceJointMax,
      "Invalid joint space attribute");
    m_jointSpace = (AnalysisPropertyJointSpace)space;

    CHAR paramName[256];
    for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
    {
      sprintf_s(paramName, "JointIndex_%d", animSetIndex);
      uint32_t rigJointIndex = MR::AnimRigDef::INVALID_BONE_INDEX;
      analysisNodeDataBlock->readUInt(rigJointIndex, paramName);
      m_rigJointIndices[animSetIndex] = rigJointIndex;

      const MR::AnimRigDef* rigDef = networkDef->getRig(animSetIndex);
      NMP_VERIFY(rigDef);
      NMP_VERIFY(rigJointIndex < rigDef->getNumBones());
    }
  }
  else
  {
    NMP_VERIFY_FAIL("Invalid analysis property mode");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySampleBuffer::writeFile(const char* filename) const
{
  NMP_VERIFY(filename);
  FILE* fp = fopen(filename, "w");
  if (fp)
  {
    // Extracted features
    fprintf(fp, "animSetIndex = %d\n", m_animSetIndex);
    fprintf(fp, "numFrames = %d\n", m_numFrames);

    for (uint32_t i = 0; i < m_numFrames; ++i)
    {
      fprintf(fp,
        "frame = %d, pos = [%f, %f, %f], quat = [%f, %f, %f, %f]\n",
        i,
        m_channelPos[i].x, m_channelPos[i].y, m_channelPos[i].z,
        m_channelAtt[i].x, m_channelAtt[i].y, m_channelAtt[i].z, m_channelAtt[i].w);
    }

    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySampleBuffer::setTrajectoryMode(
  MR::NetworkDef* networkDef,
  AnalysisPropertyTrajectorySpace space)
{
  MR::AnimSetIndex numAnimSets = networkDef->getNumAnimSets();

  m_mode = kAPModeTrajectory;
  m_trajectorySpace = space;

  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    m_rigJointIndices[animSetIndex] = MR::AnimRigDef::INVALID_BONE_INDEX;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySampleBuffer::instanceInit(MR::Network* network)
{
  m_numFrames = 0;
  m_animSetIndex = network->getActiveAnimSetIndex();
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySampleBuffer::instanceUpdate(MR::Network* network)
{
  NMP_VERIFY(m_animSetIndex == network->getActiveAnimSetIndex());
  NMP_VERIFY_MSG(
    m_numFrames < m_maxNumFrames,
    "Attempting to set a sample outside of the allocated buffer size");

  //---------------------------
  // Update the accumulated trajectory position. Note that the character
  // location should be relative to the start sampling frame. The trajectory
  // delta is a transform that moves the character from its last frame position
  // to this frames position.
  NMP::Vector3 trajectoryDeltaPos = network->getTranslationChange();
  NMP::Quat trajectoryDeltaAtt = network->getOrientationChange();
  if (m_numFrames > 0)
  {
    NMP::Vector3 deltaPosTemp;
    m_characterOrientation.rotateVector(trajectoryDeltaPos, deltaPosTemp);
    m_characterPosition += deltaPosTemp;

    m_characterOrientation = m_characterOrientation * trajectoryDeltaAtt;
    m_characterOrientation.normalise();
  }
  else
  {
    m_characterPosition.setToZero();
    m_characterOrientation.identity();
  }

  //---------------------------
  NMP::Vector3 channelPos(NMP::Vector3::InitZero);
  NMP::Quat channelAtt(NMP::Quat::kIdentity);
  if (m_mode == kAPModeTrajectory)
  {
    if (m_trajectorySpace == kAPSpaceTrajectoryWorld)
    {
      // Accumulated trajectory position relative to the start frame of
      // the sample buffer.
      channelPos = m_characterPosition;
      channelAtt = m_characterOrientation;
    }
    else if (m_trajectorySpace == kAPSpaceTrajectoryLocal)
    {
      // Joint local and Character local space transforms don't make sense for
      // the trajectory joint, since the character root doesn't have a parent
      // joint and the Character local coordinate system is just at the origin.
      // If the space is not the Character World (located at the start frame of
      // the sample buffer) then we assume that this mode of operation means
      // the space is relative to the previous frame's character root.
      channelPos = trajectoryDeltaPos;
      channelAtt = trajectoryDeltaAtt;
    }
    else
    {
      NMP_VERIFY_FAIL("Invalid trajectory space");
    }
  }
  else if (m_mode == kAPModeRigJoint)
  {
    // Accumulated joint position (in the appropriate space)
    NMP::DataBuffer* sourceBuffer = network->getTransforms();
    const NMP::Vector3* bufferPos = sourceBuffer->getPosQuatChannelPos(0);
    const NMP::Quat* bufferAtt = sourceBuffer->getPosQuatChannelQuat(0);

    const MR::AnimRigDef* rig = network->getActiveRig();
    NMP_VERIFY(rig);
    uint32_t rigJointIndex = m_rigJointIndices[m_animSetIndex];
    NMP_VERIFY_MSG(
      rigJointIndex < rig->getNumBones(),
      "Invalid rig joint index");

    const MR::AttribDataTransformBuffer* bindPoseBufferAttrib = rig->getBindPose();
    NMP_VERIFY(bindPoseBufferAttrib);
    const NMP::DataBuffer* bindPoseBuffer = bindPoseBufferAttrib->m_transformBuffer;
    NMP_VERIFY(bindPoseBuffer);
    const NMP::Vector3* bindChannelPos = bindPoseBuffer->getPosQuatChannelPos(0);
    const NMP::Quat* bindChannelAtt = bindPoseBuffer->getPosQuatChannelQuat(0);

    if (m_jointSpace == kAPSpaceJointLocal)
    {
      if (sourceBuffer->hasChannel(rigJointIndex))
      {
        channelPos = bufferPos[rigJointIndex];
        channelAtt = bufferAtt[rigJointIndex];
      }
      else
      {
        channelPos = bindChannelPos[rigJointIndex];
        channelAtt = bindChannelAtt[rigJointIndex];
      }
    }
    else if (m_jointSpace == kAPSpaceCharacterLocal)
    {
      NMP::Vector3 rootPos(NMP::Vector3::InitZero);
      NMP::Quat rootQuat(NMP::Quat::kIdentity);
      MR::BlendOpsBase::accumulateTransform(
        rigJointIndex,
        rootPos,
        rootQuat,
        sourceBuffer,
        rig,
        channelPos,
        channelAtt);
    }
    else if (m_jointSpace == kAPSpaceCharacterWorld)
    {
      MR::BlendOpsBase::accumulateTransform(
        rigJointIndex,
        m_characterPosition,
        m_characterOrientation,
        sourceBuffer,
        rig,
        channelPos,
        channelAtt);
    }
  }
  else
  {
    NMP_VERIFY_FAIL("Invalid analysis property mode");
  }

  // Update the sample features
  channelAtt.normalise();
  m_channelPos[m_numFrames] = channelPos;
  m_channelAtt[m_numFrames] = channelAtt;
  m_numFrames++;
}


//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertyUtils
//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyUtils::averageOfSamples(
  uint32_t numSamples,
  const float* samples)
{
  float sum = 0.0f;
  for (uint32_t iFrame = 0; iFrame < numSamples; ++iFrame)
  {
    sum += samples[iFrame];
  }
  return sum / (float)numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyUtils::linearRegressionSample(
  uint32_t numSamples,
  const float* samples,
  float fFrame)
{
  NMP_VERIFY(numSamples > 0);
  NMP_VERIFY(samples);

  NMP::LinearRegression<float> linearFilter;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    float x = (float)i;
    linearFilter.add(x, samples[i]);
  }
  linearFilter.solve();
  float result = linearFilter.evaluate(fFrame);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyUtils::applySmoothingFilterAtFrame(
  uint32_t wndHalfSize,
  const float* kernel,
  uint32_t iFrame,
  uint32_t numFrames,
  const float* dataIn)
{
  NMP_VERIFY(wndHalfSize > 0);
  NMP_VERIFY(kernel);
  NMP_VERIFY(numFrames >= wndHalfSize);
  NMP_VERIFY(iFrame < numFrames);
  NMP_VERIFY(dataIn);

  // Convolve data with the kernel
  float sumKrnl = kernel[0];
  float x = dataIn[iFrame] * kernel[0];
  for (uint32_t k = 1; k < wndHalfSize; ++k)
  {
    // Left side of the kernel window
    if (iFrame >= k)
    {
      sumKrnl += kernel[k];
      x += dataIn[iFrame - k] * kernel[k];
    }

    // Right side of the kernel window
    if (iFrame + k < numFrames)
    {
      sumKrnl += kernel[k];
      x += dataIn[iFrame + k] * kernel[k];
    }
  }
  float result = x / sumKrnl;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyUtils::applyLinearFilterAtFrame(
  uint32_t wndHalfSize,
  uint32_t iFrame,
  uint32_t numFrames,
  const float* dataIn)
{
  NMP_VERIFY(wndHalfSize > 0);
  NMP_VERIFY(numFrames >= wndHalfSize);
  NMP_VERIFY(iFrame < numFrames);
  NMP_VERIFY(dataIn);
  float fFrame = (float)iFrame;

  // Fit the data samples about the frame site to a straight line
  NMP::LinearRegression<float> linearFilter;
  linearFilter.add(fFrame, dataIn[iFrame]);
  for (uint32_t k = 1; k < wndHalfSize; ++k)
  {
    // Left side of the kernel window
    if (iFrame >= k)
    {
      uint32_t iFrameKrnl = iFrame - k;
      float fFrameKrnl = (float)iFrameKrnl;
      linearFilter.add(fFrameKrnl, dataIn[iFrameKrnl]);
    }

    // Right side of the kernel window
    if (iFrame + k < numFrames)
    {
      uint32_t iFrameKrnl = iFrame + k;
      float fFrameKrnl = (float)iFrameKrnl;
      linearFilter.add(fFrameKrnl, dataIn[iFrameKrnl]);
    }
  }
  linearFilter.solve();

  // Evaluate the line at the frame sample
  float result = linearFilter.evaluate(fFrame);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyUtils::applyMedianFilterAtFrame(
  uint32_t wndHalfSize,
  uint32_t iFrame,
  uint32_t numFrames,
  const float* dataIn,
  float* medianBuffer)
{
  NMP_VERIFY(wndHalfSize > 0);
  NMP_VERIFY(numFrames >= wndHalfSize);
  NMP_VERIFY(iFrame < numFrames);
  NMP_VERIFY(dataIn);
  NMP_VERIFY(medianBuffer);

  //---------------------------
  // Recover the buffer of samples
  uint32_t numSamples = 1;
  medianBuffer[0] = dataIn[iFrame];
  for (uint32_t k = 1; k < wndHalfSize; ++k)
  {
    // Left side of the kernel window
    if (iFrame >= k)
    {
      medianBuffer[numSamples] = dataIn[iFrame - k];
      numSamples++;
    }

    // Right side of the kernel window
    if (iFrame + k < numFrames)
    {
      medianBuffer[numSamples] = dataIn[iFrame + k];
      numSamples++;
    }
  }

  //---------------------------
  // Compute the median of the buffer samples
  for (uint32_t i = 0; i < numSamples - 1; ++i)
  {
    for (uint32_t j = i + 1; j < numSamples; ++j)
    {
      if (medianBuffer[j] < medianBuffer[i])
      {
        NMP::nmSwap(medianBuffer[i], medianBuffer[j]);
      }
    }
  }
  float paramValue;
  uint32_t index = numSamples >> 1;
  if (numSamples & 0x01)
  {
    paramValue = medianBuffer[index];
  }
  else
  {
    paramValue = 0.5f * (medianBuffer[index] + medianBuffer[index - 1]);
  }

  return paramValue;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyUtils::processSmoothedSamples(
  uint32_t numSamples,
  const float* featureData,
  float* smoothedData)
{
  // Averaging smoothing kernel
  const uint32_t wndHalfSize = 5;
  float kernel[wndHalfSize] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; // Kernel need not be unit norm

  // Get the active kernel size
  uint32_t activeWndHalfSize = NMP::minimum(wndHalfSize, numSamples);

  // Extract the smoothed sample data
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    smoothedData[i] = applySmoothingFilterAtFrame(
      activeWndHalfSize,
      kernel,
      i,
      numSamples,
      featureData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyUtils::processSmoothedSamples(
  uint32_t numSamples,
  const float* featureData,
  uint32_t rangeStart,
  uint32_t rangeEnd,
  float* smoothedData)
{
  NMP_VERIFY(rangeEnd >= rangeStart);

  // Averaging smoothing kernel
  const uint32_t wndHalfSize = 5;
  float kernel[wndHalfSize] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; // Kernel need not be unit norm

  // Get the active kernel size
  uint32_t activeWndHalfSize = NMP::minimum(wndHalfSize, numSamples);

  // Extract the smoothed sample data
  uint32_t sampleCount = rangeEnd - rangeStart + 1;
  for (uint32_t i = 0; i < sampleCount; ++i)
  {
    smoothedData[i] = applySmoothingFilterAtFrame(
      activeWndHalfSize,
      kernel,
      rangeStart + i,
      numSamples,
      featureData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float AnalysisPropertyUtils::processSmoothedSampleAtFrame(
  uint32_t numSamples,
  const float* featureData,
  uint32_t iFrame)
{
  // Averaging smoothing kernel
  const uint32_t wndHalfSize = 5;
  float kernel[wndHalfSize] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; // Kernel need not be unit norm

  // Get the active kernel size
  uint32_t activeWndHalfSize = NMP::minimum(wndHalfSize, numSamples);

  // Extract the smoothed sample data at the start
  float result = applySmoothingFilterAtFrame(
    activeWndHalfSize,
    kernel,
    iFrame,
    numSamples,
    featureData);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyUtils::calculateWeightsForNearZeroValuedSamples(
  uint32_t numSamples,
  const float* samples,
  float* weights)
{
  NMP_VERIFY(numSamples > 1);
  NMP_VERIFY(samples);
  NMP_VERIFY(weights);

  NMP::Memory::Format memReqsHist(sizeof(uint32_t) * numSamples, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResHist = NMPMemoryAllocateFromFormat(memReqsHist);
  uint32_t* histData = (uint32_t*)memResHist.ptr;

  // Find the min and max bounds of the smoothed data
  float minVal = samples[0];
  float maxVal = samples[0];
  for (uint32_t i = 1; i < numSamples; ++i)
  {
    minVal = NMP::minimum(minVal, samples[i]);
    maxVal = NMP::maximum(maxVal, samples[i]);
  }

  //---------------------------
  // Compute a histogram of the smoothed data (10-partitions)
  float histBinDelta = (maxVal - minVal) / 10;
  float recipHistBinDelta = 1.0f;
  if (histBinDelta != 0.0f)
    recipHistBinDelta = 1.0f / histBinDelta;

  float hist[10];
  for (uint32_t i = 0; i < 10; ++i)
    hist[i] = 0.0f;

  for (uint32_t i = 0; i < numSamples; ++i)
  {
    // Determine which histogram bin the sample lies. Ensure that any samples at the
    // max value get put into the last bin.
    int32_t histBinIndex = (int32_t)((samples[i] - minVal) * recipHistBinDelta);
    if (histBinIndex < 0)
      histBinIndex = 0;
    else if (histBinIndex > 9)
      histBinIndex = 9;
    histData[i] = histBinIndex;

    // Update the bin count
    hist[histBinIndex] += 1.0f;
  }

  // Normalise to recover the PDF of the sample data
  float normFac = 1.0f / (float)numSamples;
  for (uint32_t i = 0; i < 10; ++i)
  {
    hist[i] *= normFac;
  }

  //---------------------------
  // Find the set of histogram bins that neighbour the zero measurement samples,
  // and contain at least 68.3% (1-sigma) of the sample data.
  uint32_t histBinIndices[10];
  uint32_t numHistBinIndices = 0;
  float sum = 0.0f;

  // Add the zero measurement samples bin
  int32_t zeroBinIndex = (int32_t)((0.0f - minVal) * recipHistBinDelta);
  if (zeroBinIndex < 0)
    zeroBinIndex = 0;
  else if (zeroBinIndex > 9)
    zeroBinIndex = 9;
  histBinIndices[0] = zeroBinIndex;
  numHistBinIndices++;
  sum += hist[zeroBinIndex];

  // Add the neighbours
  const float histSumTol = 0.683f; // 1 sigma
  for (int32_t i = 1; i < 10; ++i)
  {
    // Left
    if (sum >= histSumTol)
      break;
    if (i <= zeroBinIndex)
    {
      uint32_t histBinIndex = zeroBinIndex - i;
      histBinIndices[numHistBinIndices] = histBinIndex;
      numHistBinIndices++;
      sum += hist[histBinIndex];
    }

    // Right
    if (sum >= histSumTol)
      break;
    if (zeroBinIndex + i < 10)
    {
      uint32_t histBinIndex = zeroBinIndex + i;
      histBinIndices[numHistBinIndices] = histBinIndex;
      numHistBinIndices++;
      sum += hist[histBinIndex];
    }
  }

  // Compute the standard deviation of the steady state samples (assume zero mean)
  float sumSq = 0.0f;
  uint32_t sumN = 0;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    // Update the variance if the samples is included in the set of steady state histogram bins
    uint32_t histBinIndex = histData[i];
    for (uint32_t k = 0; k < numHistBinIndices; ++k)
    {
      if (histBinIndex == histBinIndices[k])
      {
        float val = samples[i];
        sumSq += val * val;
        sumN++;
        break;
      }
    }
  }
  NMP_VERIFY(sumN > 0);
  float sigma = sqrtf(sumSq / sumN);
  float recipSigma = 1.0f;
  if (sigma > 0.0f)
    recipSigma = 1.0f / sigma;

  //---------------------------
  // Compute the per frame steady state sample weights
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    float u = samples[i] * recipSigma;
    weights[i] = expf(-0.5f * u * u);  
  }

  //---------------------------
  // Tidy up
  NMP::Memory::memFree(histData);
}

}
