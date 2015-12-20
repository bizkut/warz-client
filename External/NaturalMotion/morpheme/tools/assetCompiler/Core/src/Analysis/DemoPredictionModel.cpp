// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/DemoPredictionModel.h"
#include "NMNumerics/NMNumericUtils.h"
#include "XMD/Model.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
#include "Analysis/XMDNetworkControlTake.h"
#include "Analysis/AnalysisParameteriser.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool DemoPredictionModel::analyse(
  AP::AssetProcessor* processor,
  const ME::AnalysisTaskExport* task,
  ME::DataBlockExport* resultData)
{
  NMP_ASSERT_FAIL();
  (void)resultData;

  // The first two analysisNodes are the DemoPredictionModel and RunNetworkScriptedAssetCompiler.
  // The remaining analysisNodes are the analysis conditions and analysis properties
  NMP_VERIFY_MSG(
    task->getNumAnalysisNodes() >= 2,
    "Expected at least two analysisNodes as input for analysis, got %d",
    task->getNumAnalysisNodes());

  //---------------------------
  // The DemoPredictionModel analysisNode
  const ME::AnalysisNodeExport* demoAnalysisNode = task->getAnalysisNode(0);
  NMP_VERIFY(demoAnalysisNode);
  NMP_VERIFY_MSG(
    strcmp(demoAnalysisNode->getTypeName(), "DemoPredictionModel") == 0,
    "Expected \"DemoPredictionModel\" as the first analysisNode input, got '%s'",
    demoAnalysisNode->getTypeName());
  const ME::DataBlockExport* analysisNodeDataBlock = demoAnalysisNode->getDataBlock();

  uint32_t parameterCount = 0;
  analysisNodeDataBlock->readUInt(parameterCount, "ParameterCount");
  NMP_VERIFY(parameterCount > 0);

  //---------------------------
  // Initialise the network control networkControlPlayer with the RunNetworkScriptedAssetCompiler and
  // AnalysisCondition analysisNodes
  NetworkControlPlayer networkControlPlayer;
  networkControlPlayer.init(processor, task);
  MR::NetworkDef* networkDef = networkControlPlayer.getNetworkDef();

  uint32_t dimensionCount = networkControlPlayer.getNumDimensions();
  NMP_VERIFY_MSG(
    dimensionCount == parameterCount,
    "The number of sample dimensions (%d) should match the number of motion parameters (%d)",
    dimensionCount,
    parameterCount);

  uint32_t frameCount = networkControlPlayer.getFrameCount();
  float deltaTime = networkControlPlayer.getDeltaTime();
  float sampleFrequency = networkControlPlayer.getSampleFrequency();

  // expecting more than one sample in the only dimension
  uint32_t dimensionSamples = networkControlPlayer.getNumTakesForDimension(0);
  NMP_VERIFY(dimensionSamples > 0);

  AnalysisInterval* analysisInterval = networkControlPlayer.getAnalysisInterval();

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
    demoAnalysisNode);
  uint32_t numAnalysisProperties = analysisParameteriser->getNumAnalysisProperties();
  NMP_VERIFY_MSG(
    numAnalysisProperties > 0,
    "There are no analysis properties to evaluate");

  //---------------------------
  // Write out how many samples in total will we produce.
  uint32_t numSamples = networkControlPlayer.getNumTakes();
  resultData->writeUInt(numSamples, "NumSamples");

  bool analysisResult = true;

  // Iterate over each take (sample point).
  for (uint32_t sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
  {
    //---------------------------
    // Get the take for this sample.
    // Note: The network player sorts the takes from the exported file into the correct order.
    const ControlTake* take = networkControlPlayer.getTake(sampleIndex);
    NMP_VERIFY(take);

    // Initialise a Network instance for this take. Note that this also ticks the
    // network with absolute time. This is required to set the start state correctly
    // so that messages broadcast on the first frame are handled appropriately.
    bool status = initNetworkFunc(processor, networkDef, initArgs);
    NMP_VERIFY(status);
    MR::Network* network = processor->getNetwork();
    NMP_VERIFY(network);

    // Add any post update access attribs that are required by the interval conditions.
    analysisInterval->instanceInit(network);
    analysisInterval->instanceReset();
    analysisParameteriser->instanceInit(network);

    // do the analysis
    analysisResult = analysePredictionModel(
      processor,
      take,
      frameCount,
      deltaTime,
      network,
      analysisInterval,
      analysisParameteriser);

    // Write the analysis properties to the analysis library result

    // Release the analysis network
    analysisParameteriser->instanceRelease(network);
    releaseNetworkFunc(processor);

    if(!analysisResult)
    {
      break;
    }
  }

  // Tidy up
  analysisParameteriser->releaseAndDestroy();

  return analysisResult;
}

bool DemoPredictionModel::analysePredictionModel(
  AssetProcessor* processor,
  const ControlTake* take,
  uint32_t frameCount,
  float deltaTime,
  MR::Network *network,
  AnalysisInterval *analysisInterval,
  AnalysisParameteriser *analysisParameteriser)
{
  uint32_t frameIndex = 0;

  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkFunc = processor->getUpdateNetworkInstanceFunc();

  // Set the relevant control parameters and requests for this frame
  take->updateNetwork(network, frameIndex);

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
  //  until the stop analysis interval conditions are satisfied.
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
  // if there was no stop condition hit then
  // report an error to connect, and stop processing.
  if (!analysisInterval->getStopTriggered() &&
    analysisInterval->getStopConditions()->size() != 0)
  {
    NMP::BasicLogger* log = processor->getErrorLogger();
    log->output("The stop analysis interval conditions were not satisfied over the specified number of update frames");

    // Release the analysis network and tidy up
    return false;
  }

  //---------------------------
  // Evaluate the analysis properties
  analysisParameteriser->evaluateProperties();
  return true;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------