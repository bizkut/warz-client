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
#include "Analysis/ScatterBlendVertexSourceAnalyser.h"
#include "Analysis/AnalysisParameteriser.h"
#include "NMPlatform/NMString.h"
#include "NMNumerics/NMNumericUtils.h"
#include "Analysis/NetworkControlPlayer.h"
#include "Analysis/XMDNetworkControlTake.h"
#include "assetProcessor/AssetProcessor.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlendVertexSourceAnalyser::analyse(
  AP::AssetProcessor* processor,
  const ME::AnalysisTaskExport* task,
  ME::DataBlockExport* resultData)
{
  // The first two analysisNodes are the ScatterBlendVertexSourceAnalyser task and RunNetworkScriptedAssetCompiler.
  // The remaining analysisNodes are the analysis conditions and analysis properties
  NMP_VERIFY_MSG(
    task->getNumAnalysisNodes() >= 2,
    "Expected at least two analysisNodes as input for analysis, got %d",
    task->getNumAnalysisNodes());

  //---------------------------
  // The ScatterBlendVertexSourceAnalyser analysisNode
  const ME::AnalysisNodeExport* analyseTrajectoryExport = task->getAnalysisNode(0);
  NMP_VERIFY(analyseTrajectoryExport);
  NMP_VERIFY_MSG(
    strcmp(analyseTrajectoryExport->getTypeName(), "ScatterBlendVertexSourceAnalyser") == 0,
    "Expected \"ScatterBlendVertexSourceAnalyser\" as the first analysisNode input, got '%s'",
    analyseTrajectoryExport->getTypeName());

  //---------------------------
  // Initialise the network control player with the RunNetworkScriptedAssetCompiler and
  // AnalysisCondition analysisNodes
  NetworkControlPlayer networkControlPlayer;
  networkControlPlayer.init(processor, task);
  MR::NetworkDef* networkDef = networkControlPlayer.getNetworkDef();
  MR::AnimSetIndex numAnimationSets = networkDef->getNumAnimSets();

  uint32_t frameCount = networkControlPlayer.getFrameCount();
  float deltaTime = networkControlPlayer.getDeltaTime();
  float sampleFrequency = networkControlPlayer.getSampleFrequency();

  // The number of sample dimensions should match the number of motion parameters
  uint32_t dimensionCount = networkControlPlayer.getNumDimensions();
  NMP_VERIFY_MSG(
    dimensionCount == 1,
    "The number of sample dimensions (%d) should be 1 for a trajectory analysis",
    dimensionCount);

  uint32_t numSamples = networkControlPlayer.getNumTakes();
  NMP_VERIFY_MSG(
    numSamples == 1,
    "Expecting a single sample for trajectory analysis");

  AnalysisInterval* analysisInterval = networkControlPlayer.getAnalysisInterval();

  //---------------------------
  AP::AssetProcessor::InitNetworkInstanceFunc initNetworkFunc = processor->getInitNetworkInstanceFunc();
  AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkFunc = processor->getReleaseNetworkInstanceFunc();

  AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
  initArgs.m_startStateNodeID = networkControlPlayer.getStartStateNodeID();

  //---------------------------
  // Initialise the analysis parameteriser with the analysis properties
  AnalysisParameteriser* analysisParameteriser = AnalysisParameteriser::create(16);
  analysisParameteriser->readAnalysisProperties(
    processor,
    networkDef,
    sampleFrequency,
    frameCount,
    task,
    analyseTrajectoryExport);
  uint32_t numAnalysisProperties = analysisParameteriser->getNumAnalysisProperties();
  NMP_VERIFY_MSG(
    numAnalysisProperties > 0,
    "There are no analysis properties to evaluate");

  //---------------------------
  // Get the take for this sample.
  const ControlTake* take = networkControlPlayer.getTake(0);
  NMP_VERIFY(take);

  bool analysisResult = true;
  for(MR::AnimSetIndex setIndex = 0; setIndex < numAnimationSets; ++setIndex)
  {
    // Initialise a Network instance for this take. Note that this also ticks the
    // network with absolute time. This is required to set the start state correctly
    // so that messages broadcast on the first frame are handled appropriately.
    initArgs.m_startingAnimSetIndex = setIndex;
    bool status = initNetworkFunc(processor, networkDef, initArgs);
    NMP_VERIFY(status);
    MR::Network* network = processor->getNetwork();
    NMP_VERIFY(network);

    // Add any post update access attribs that are required by the interval conditions.
    analysisInterval->instanceInit(network);
    analysisInterval->instanceReset();
    analysisParameteriser->instanceInit(network);

    analysisResult = analyseVertexSource(
      processor,
      setIndex,
      take,
      frameCount,
      deltaTime,
      network,
      analysisInterval,
      analysisParameteriser,
      resultData);

    // Release the analysis network
    analysisParameteriser->instanceRelease(network);
    releaseNetworkFunc(processor);

    if (!analysisResult)
    {
      break;
    }
  }

  // Tidy up
  analysisParameteriser->releaseAndDestroy();

  return analysisResult;
}

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlendVertexSourceAnalyser::analyseVertexSource(
  AssetProcessor* processor,
  uint32_t setIndex,
  const ControlTake* take,
  uint32_t frameCount,
  float deltaTime,
  MR::Network *network,
  AnalysisInterval *analysisInterval,
  AnalysisParameteriser *analysisParameteriser,
  ME::DataBlockExport* resultData)
{
  uint32_t frameIndex = 0;

  // Set the relevant control parameters and requests for this frame
  take->updateNetwork(network, frameIndex);

  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkFunc = processor->getUpdateNetworkInstanceFunc();

  // Make the first update of the network with absolute time
  AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;
  updateArgs.m_updateTime = 0.0f;
  updateArgs.m_absTime = true;
  updateNetworkFunc(processor, updateArgs);

  // Update the analysis interval
  analysisInterval->instanceUpdate(network, frameIndex);

  //---------------------------
  // Update the network until the start analysis interval conditions are satisfied.
  updateArgs.m_updateTime = deltaTime;
  updateArgs.m_absTime = false;
  while (!analysisInterval->getStartTriggered())
  {
    // Start another frame update
    frameIndex++;
    NMP_VERIFY_MSG(
      frameIndex < frameCount,
      "The start analysis interval conditions were not satisfied over the specified number of update frames");

    // Set the relevant control parameters and requests for this frame
    take->updateNetwork(network, frameIndex);

    // Update the network itself.
    updateNetworkFunc(processor, updateArgs);

    // Update the analysis interval
    analysisInterval->instanceUpdate(network, frameIndex);
  }

  //---------------------------
  // Initialise the first sample of the analysis property feature data
  analysisParameteriser->instanceUpdate(network);

  //---------------------------
  // Iterate over the take frames extracting the data we are interested in
  // until the stop analysis interval conditions are satisfied.
  while (!analysisInterval->getStopTriggered())
  {
    frameIndex++;

    NMP_VERIFY_MSG(
      analysisInterval->getStopConditions()->size() == 0 || frameIndex < frameCount,
      "The stop analysis interval conditions were not satisfied over the specified number of update frames");

    // Set the relevant control parameters and requests for this frame
    take->updateNetwork(network, frameIndex);

    // Update the network itself.
    updateNetworkFunc(processor, updateArgs);

    // Update the analysis property feature data
    analysisParameteriser->instanceUpdate(network);

    // Update the analysis interval
    analysisInterval->instanceUpdate(network, frameIndex);
  }

  //---------------------------
  // Evaluate the analysis properties
  analysisParameteriser->evaluateProperties();

  // Write the analysis properties to the analysis library result
  uint32_t numAnalysisProperties = analysisParameteriser->getNumAnalysisProperties();
  for (uint32_t idx = 0; idx < numAnalysisProperties; ++idx)
  {
    AnalysisProperty* analysisProperty = analysisParameteriser->getAnlysisProperty(idx);
    NMP_VERIFY_MSG(
      analysisProperty->m_numDims == 1,
      "Expecting scalar analysis properties only");
    char attrName[1024];
    sprintf_s(attrName, 1024 - 1, "Property%d_Set%d", idx, setIndex);
    resultData->writeFloat(analysisProperty->m_value[0], attrName);
  }
  return true;
}

}