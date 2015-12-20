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
#include "Analysis/NetworkControlPlayer.h"
#include "NMNumerics/NMNumericUtils.h"
#include "XMD/Model.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
#include "Analysis/XMDNetworkControlTake.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NetworkControlPlayer::NetworkControlPlayer() :
  m_processor(NULL),
  m_networkControlAnalysisNode(NULL),
  m_processedNetDef(NULL),
  m_frameCount(0),
  m_dimensionCount(0),
  m_dimensionSamplesCount(NULL),
  m_analysisInterval(NULL)
{
  m_controlTakes.clear();
}

//----------------------------------------------------------------------------------------------------------------------
NetworkControlPlayer::~NetworkControlPlayer()
{
  term();
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkControlPlayer::init(
  AP::AssetProcessor*           processor,
  const ME::AnalysisTaskExport* task)
{
  m_processor = processor;

  //---------------------------
  // Find a compatible network control analysisNode within the task.
  // i.e. RunNetworkScriptedAssetCompiler or SimpleNetworkControlAssetCompiler
  NMP_VERIFY(task);
  uint32_t numAnalysisNodes = task->getNumAnalysisNodes();
  m_networkControlAnalysisNode = NULL;
  for (uint32_t i = 0; i < numAnalysisNodes; ++i)
  {
    const ME::AnalysisNodeExport* analysisNode = task->getAnalysisNode(i);
    NMP_VERIFY(analysisNode);
    if (strcmp(analysisNode->getTypeName(), "RunNetworkScriptedAssetCompiler") == 0 ||
        strcmp(analysisNode->getTypeName(), "SimpleNetworkControlAssetCompiler") == 0)
    {
      m_networkControlAnalysisNode = analysisNode;
      break;
    }
  }
  if (!m_networkControlAnalysisNode)
  {
    processor->getErrorLogger()->output("Could not find a compatible network control analysisNode in the task: %s", task->getName());
    return false;
  }
  const ME::DataBlockExport* analysisNodeDataBlock = m_networkControlAnalysisNode->getDataBlock();

  //---------------------------
  // Load the exported network definition and process the asset
  std::string networkExport;
  analysisNodeDataBlock->readString(networkExport, "ExportedNetworkPath");
  if (!networkExport.length())
  {
    NMP_VERIFY_FAIL("A valid network control recording has not been exported in task: %s, we cannot proceed", task->getName());
    return false;
  }

  ME::ExportFactoryXML exportFactory;
  ME::AssetExport* loadedAsset = exportFactory.loadAsset(networkExport.c_str());
  if (!loadedAsset)
  {
    NMP_VERIFY_FAIL("Unable to load the exported network definition for task: %s", task->getName());
    return false;
  }
  
  // Create the Network to be used for playback.
  processor->processAsset(loadedAsset);

  // Locate the NetworkDef in the compiled assets library.
  const ProcessedAsset* processedAsset = processor->findProcessedAsset(loadedAsset->getGUID(), MR::Manager::kAsset_NetworkDef);
  if (!processedAsset)
  {
    NMP_VERIFY_FAIL("Failed to process the exported network definition");
    return false;
  }
  NMP_VERIFY(processedAsset->assetType == MR::Manager::kAsset_NetworkDef);
  m_processedNetDef = (MR::NetworkDef*)processedAsset->assetMemory.ptr;

  //---------------------------
  // Dimensional sampling
  m_dimensionCount = 1;
  m_dimensionSamplesCount = 0;
  analysisNodeDataBlock->readUInt(m_dimensionCount, "DimensionCount");
  if (m_dimensionCount > 0)
  {
    m_dimensionSamplesCount = (uint32_t*)NMPMemoryAlloc(sizeof(uint32_t) * m_dimensionCount);

    size_t lengthOut;
    analysisNodeDataBlock->readUIntArray(m_dimensionSamplesCount, m_dimensionCount, lengthOut, "DimensionSamples");
    NMP_VERIFY(((uint32_t)lengthOut) == m_dimensionCount);

    // Allocate the control take pointer array
    uint32_t numTakes = m_dimensionSamplesCount[0];
    for (uint32_t i = 1; i < m_dimensionCount; ++i)
    {
      numTakes *= m_dimensionSamplesCount[i];
    }
    m_controlTakes.resize(numTakes);
  }
  else
  {
    // Special case handling for 0 dimension control takes
    m_controlTakes.resize(1);
  }

  //---------------------------
  // Load the network control takes
  std::string xmdFile;
  analysisNodeDataBlock->readString(xmdFile, "ControlXMDPath");

  if (xmdFile.length())
  {
    // Load the source animation file into an xmd structure. (Import it from XMD, FBX or other translator).
    XMD::XModel controlFileModel;
    XMD::XFileError::eType fileResult = controlFileModel.LoadAnim(xmdFile.c_str());
    if (fileResult == XMD::XFileError::Success)
    {
      // Extract the control takes from the XMD file.
      std::vector<std::string> controlTakeNames;
      std::vector<ControlTake*> controlTakes;     
      ControlTake::extractTakes(controlFileModel, controlTakeNames, controlTakes);

      // Reorder the extracted control takes into the correct dimension sample order
      uint32_t numExtractedTakes = (uint32_t)controlTakeNames.size();
      if (numExtractedTakes > 1)
      {
        uint32_t takeAddress[8];
        for (uint32_t i = 0; i < numExtractedTakes; ++i)
        {
          getTakeAddress(controlTakeNames[i].c_str(), takeAddress);
          uint32_t takeIndex = getTakeIndex(takeAddress);
          NMP_VERIFY(!m_controlTakes[takeIndex]); // Take has already been set for this address
          m_controlTakes[takeIndex] = controlTakes[i];
        }
      }
      else
      {
        // Special case handling for 0 dimension control takes
        m_controlTakes[0] = controlTakes[0];
      }
    }
  }

  // Root state machine starting state
  m_startStateNodeID = MR::INVALID_NODE_ID;
  std::string startStateName;
  analysisNodeDataBlock->readString(startStateName, "StartState");
  if (!startStateName.empty())
  {
    m_startStateNodeID = m_processedNetDef->getNodeIDFromStateName(startStateName.c_str());
  }

  //---------------------------
  // Analysis interval
  m_deltaTime = 1.0f;
  m_frameCount = 1;
  analysisNodeDataBlock->readFloat(m_deltaTime, "TimeStep");
  NMP_VERIFY(m_deltaTime > 0.0f);
  analysisNodeDataBlock->readUInt(m_frameCount, "FrameCount");
  NMP_VERIFY(m_frameCount > 0);

  m_analysisInterval = AnalysisInterval::create(16, m_frameCount);
  m_analysisInterval->readAnalysisConditions(
                            processor,
                            m_processedNetDef,
                            task,
                            m_networkControlAnalysisNode);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlPlayer::term()
{
  // tidy up take objects
  uint32_t numTakes = getNumTakes();
  for (uint32_t i = 0; i < numTakes; ++i)
  {
    NMP::Memory::memFree(m_controlTakes[i]);
  }
  m_controlTakes.clear();

  m_dimensionCount = 0;
  if (m_dimensionSamplesCount)
  {
    NMP::Memory::memFree(m_dimensionSamplesCount);
  }
  m_dimensionSamplesCount = NULL;
  
  if (m_analysisInterval)
  {
    m_analysisInterval->releaseAndDestroy();
  }

  m_networkControlAnalysisNode = NULL;
  m_frameCount = 0;
  m_deltaTime = 0.0f;
  m_processedNetDef = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlPlayer::getTakeAddressName(char* takeName, uint32_t* takeAddress) const
{
  strcpy(takeName, "Take");
  char paramValue[64];
  for (uint32_t i = 0; i < m_dimensionCount; ++i)
  {
    sprintf_s(paramValue, 64, "_%d", takeAddress[i]);
    strcat(takeName, paramValue);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlPlayer::getTakeAddress(const char* takeName, uint32_t* takeAddress) const
{
  NMP_VERIFY(takeName);
  NMP_VERIFY(takeAddress);
  NMP_VERIFY(strlen(takeName) > 5);
  NMP_VERIFY(strncmp("Take_", takeName, 5) == 0);
  for (uint32_t i = 0; i < m_dimensionCount; ++i)
    takeAddress[i] = 0;

  std::string tokenBuffer = &takeName[5];
  char* buffer = (char*)tokenBuffer.c_str();

  const char seps[] = "_";
  char* token = strtok(buffer, seps);
  for (uint32_t i = 0; i < m_dimensionCount; ++i)
  {
    takeAddress[i] = (uint32_t)atoi(token) - 1;
    NMP_VERIFY(takeAddress[i] < m_dimensionSamplesCount[i]);
    token = strtok(0, seps);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlPlayer::initTakeAddress(uint32_t* takeAddress) const
{
  NMP_VERIFY(takeAddress);
  for (uint32_t i = 0; i < m_dimensionCount; ++i)
    takeAddress[i] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkControlPlayer::incrementTakeAddress(uint32_t* takeAddress) const
{
  NMP_VERIFY(takeAddress);
  for (uint32_t i = 0; i < m_dimensionCount; ++i)
  {
    takeAddress[i]++;
    if (takeAddress[i] < m_dimensionSamplesCount[i])
    {
      return true;
    }
    takeAddress[i] = 0;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlPlayer::getTakeIndex(const uint32_t* takeAddress) const
{
  NMP_VERIFY(takeAddress);
  uint32_t result = takeAddress[0];
  uint32_t dimSpan = 1;
  for (uint32_t i = 1; i < m_dimensionCount; ++i)
  {
    dimSpan *= m_dimensionSamplesCount[i - 1];
    result += (takeAddress[i] * dimSpan);
  }
  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
