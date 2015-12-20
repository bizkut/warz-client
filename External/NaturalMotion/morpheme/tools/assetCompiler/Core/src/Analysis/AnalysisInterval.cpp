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
#include "Analysis/AnalysisInterval.h"
#include "Analysis/AnalysisConditionAtFrame.h"
#include "NMPlatform/NMString.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnalysisInterval
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisInterval::getMemoryRequirements(
  uint32_t maxNumConditions,
  uint32_t NMP_USED_FOR_VERIFY(maxNumFrames))
{
  NMP_VERIFY(maxNumConditions > 0);
  NMP_VERIFY(maxNumFrames > 0);

  NMP::Memory::Format result(sizeof(AnalysisInterval), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Format memReqsConds = NMP::VectorContainer<AnalysisCondition*>::getMemoryRequirements(maxNumConditions);
  result += (memReqsConds * 2);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisInterval* AnalysisInterval::init(
  NMP::Memory::Resource& resource,
  uint32_t maxNumConditions,
  uint32_t maxNumFrames)
{
  NMP_VERIFY(maxNumConditions > 0);
  NMP_VERIFY(maxNumFrames > 0);

  NMP::Memory::Format memReqsHdr(sizeof(AnalysisInterval), NMP_VECTOR_ALIGNMENT);
  AnalysisInterval* result = (AnalysisInterval*)resource.alignAndIncrement(memReqsHdr);

  NMP::Memory::Format memReqsConds = NMP::VectorContainer<AnalysisCondition*>::getMemoryRequirements(maxNumConditions);
  result->m_startConditions = NMP::VectorContainer<AnalysisCondition*>::init(resource, maxNumConditions);
  result->m_stopConditions = NMP::VectorContainer<AnalysisCondition*>::init(resource, maxNumConditions);

  result->m_maxNumFrames = maxNumFrames;
  result->m_startFrame = 0;
  result->m_stopFrame = 0;
  result->m_startTriggered = false;
  result->m_stopTriggered = false;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisInterval* AnalysisInterval::create(
  uint32_t maxNumConditions,
  uint32_t maxNumFrames)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(maxNumConditions, maxNumFrames);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  AnalysisInterval* result = init(memRes, maxNumConditions, maxNumFrames);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisInterval::releaseAndDestroy()
{
  // Release all of the start analysis conditions
  NMP::VectorContainer<AnalysisCondition*>::iterator startIt = m_startConditions->begin();
  for (; startIt != m_startConditions->end(); ++startIt)
  {
    AnalysisCondition* analysisCondition = *startIt;
    if (analysisCondition)
    {
      NMP::Memory::memFree(analysisCondition);
    }
  }

  // Release all of the stop analysis conditions
  NMP::VectorContainer<AnalysisCondition*>::iterator stopIt = m_stopConditions->begin();
  for (; stopIt != m_stopConditions->end(); ++stopIt)
  {
    AnalysisCondition* analysisCondition = *stopIt;
    if (analysisCondition)
    {
      NMP::Memory::memFree(analysisCondition);
    }
  }

  // Release the analysis interval
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisInterval::readAnalysisConditions(
  AP::AssetProcessor* processor,
  const MR::NetworkDef* networkDef,
  const ME::AnalysisTaskExport* task,
  const ME::AnalysisNodeExport* networkControl)
{
  NMP_VERIFY(networkControl);
  const ME::DataBlockExport* analysisNodeDataBlock = networkControl->getDataBlock();

  //---------------------------
  // Start analysis conditions
  uint32_t startConditionCount = 0;
  analysisNodeDataBlock->readUInt(startConditionCount, "StartConditionCount");

  if (startConditionCount > 0)
  {
    for (uint32_t i = 0; i < startConditionCount; ++i)
    {
      char attrName[256];
      sprintf_s(attrName, 256 - 1, "StartCondition%d", i);

      std::string startConditionPath;
      analysisNodeDataBlock->readString(startConditionPath, attrName);
      NMP_VERIFY_MSG(
        startConditionPath.length(),
        "Invalid start condition export path");

      const ME::AnalysisNodeExport* analysisExport = AnalysisProcessor::findAnalysisNodeExport(
        task,
        startConditionPath.c_str());
      NMP_VERIFY_MSG(
        analysisExport,
        "Unable to locate start condition export %s",
        startConditionPath.c_str());

      AnalysisConditionBuilder* analysisCondtionBuilder = processor->getAnalysisConditionBuilderForType(analysisExport->getTypeName());
      NMP_VERIFY_MSG(
        analysisCondtionBuilder,
        "Unable to locate the builder for the start analysis condition %s",
        startConditionPath.c_str());

      AnalysisCondition* analysisCondition = analysisCondtionBuilder->create(processor, networkDef, analysisExport);
      m_startConditions->push_back(analysisCondition);
    }
  }
  else
  {
    // Add a default AtFrame condition at the start frame
    AnalysisCondition* analysisCondition = AnalysisConditionAtFrameBuilder::create(
      AnalysisConditionAtFrame::kFirstFrame,
      0);
    m_startConditions->push_back(analysisCondition);
  }

  //---------------------------
  // Stop analysis conditions
  uint32_t stopConditionCount = 0;
  analysisNodeDataBlock->readUInt(stopConditionCount, "StopConditionCount");

  if (stopConditionCount > 0)
  {
    for (uint32_t i = 0; i < stopConditionCount; ++i)
    {
      char attrName[256];
      sprintf_s(attrName, 256 - 1, "StopCondition%d", i);

      std::string stopConditionPath;
      analysisNodeDataBlock->readString(stopConditionPath, attrName);
      NMP_VERIFY_MSG(
        stopConditionPath.length(),
        "Invalid stop condition export path");

      const ME::AnalysisNodeExport* analysisExport = AnalysisProcessor::findAnalysisNodeExport(
        task,
        stopConditionPath.c_str());
      NMP_VERIFY_MSG(
        analysisExport,
        "Unable to locate stop condition export %s",
        stopConditionPath.c_str());

      AnalysisConditionBuilder* analysisCondtionBuilder = processor->getAnalysisConditionBuilderForType(analysisExport->getTypeName());
      NMP_VERIFY_MSG(
        analysisCondtionBuilder,
        "Unable to locate the builder for the stop analysis condition %s",
        stopConditionPath.c_str());

      AnalysisCondition* analysisCondition = analysisCondtionBuilder->create(processor, networkDef, analysisExport);
      m_stopConditions->push_back(analysisCondition);
    }
  }
  else
  {
    // Add a default AtFrame condition at the last frame
    AnalysisCondition* analysisCondition = AnalysisConditionAtFrameBuilder::create(
      AnalysisConditionAtFrame::kLastFrame,
      0);
    m_stopConditions->push_back(analysisCondition);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisInterval::instanceInit(MR::Network* network)
{
  // Initialise the network instance with the start analysis conditions
  NMP::VectorContainer<AnalysisCondition*>::iterator startIt = m_startConditions->begin();
  for (; startIt != m_startConditions->end(); ++startIt)
  {
    AnalysisCondition* analysisCondition = *startIt;
    NMP_VERIFY(analysisCondition);
    if (analysisCondition->m_instanceInitFn)
    {
      analysisCondition->m_instanceInitFn(analysisCondition, network, this);
    }
  }

  // Initialise the network instance with the stop analysis conditions
  NMP::VectorContainer<AnalysisCondition*>::iterator stopIt = m_stopConditions->begin();
  for (; stopIt != m_stopConditions->end(); ++stopIt)
  {
    AnalysisCondition* analysisCondition = *stopIt;
    NMP_VERIFY(analysisCondition);
    if (analysisCondition->m_instanceInitFn)
    {
      analysisCondition->m_instanceInitFn(analysisCondition, network, this);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisInterval::instanceUpdate(MR::Network* network, uint32_t frameIndex)
{
  //---------------------------
  // Check if the start analysis condition has triggered
  if (!m_startTriggered)
  {
    NMP_VERIFY(!m_stopTriggered); // Must trigger start before stop

    bool triggered = true;
    NMP::VectorContainer<AnalysisCondition*>::iterator startIt = m_startConditions->begin();
    for (; startIt != m_startConditions->end(); ++startIt)
    {
      AnalysisCondition* analysisCondition = *startIt;
      NMP_VERIFY(analysisCondition);
      if (!analysisCondition->m_instanceUpdateFn(analysisCondition, network, this, frameIndex))
      {
        triggered = false;
        break;
      }
    }

    if (triggered)
    {
      m_startTriggered = true;
      m_startFrame = frameIndex;
    }
  }

  //---------------------------
  // Check if the stop analysis condition has triggered
  if (!m_stopTriggered && m_startTriggered)
  {
    bool triggered = false;
    NMP::VectorContainer<AnalysisCondition*>::iterator stopIt = m_stopConditions->begin();
    for (; stopIt != m_stopConditions->end(); ++stopIt)
    {
      AnalysisCondition* analysisCondition = *stopIt;
      NMP_VERIFY(analysisCondition);
      if (analysisCondition->m_instanceUpdateFn(analysisCondition, network, this, frameIndex))
      {
        triggered = true;
        break;
      }
    }

    if (triggered)
    {
      m_stopTriggered = true;
      m_stopFrame = frameIndex;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisInterval::instanceReset()
{
  // Reset the interval triggers
  m_startTriggered = m_stopTriggered = false;

  // Reset the start analysis condition internal state
  NMP::VectorContainer<AnalysisCondition*>::iterator startIt = m_startConditions->begin();
  for (; startIt != m_startConditions->end(); ++startIt)
  {
    AnalysisCondition* analysisCondition = *startIt;
    NMP_VERIFY(analysisCondition);
    if (analysisCondition->m_instanceResetFn)
    {
      analysisCondition->m_instanceResetFn(analysisCondition);
    }
  }

  // Reset the stop analysis condition internal state
  NMP::VectorContainer<AnalysisCondition*>::iterator stopIt = m_stopConditions->begin();
  for (; stopIt != m_stopConditions->end(); ++stopIt)
  {
    AnalysisCondition* analysisCondition = *stopIt;
    NMP_VERIFY(analysisCondition);
    if (analysisCondition->m_instanceResetFn)
    {
      analysisCondition->m_instanceResetFn(analysisCondition);
    }
  }
}

}
