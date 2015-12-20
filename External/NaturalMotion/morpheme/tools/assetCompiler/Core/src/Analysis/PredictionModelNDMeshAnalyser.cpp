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
#include "Analysis/PredictionModelNDMeshAnalyser.h"
#include "Analysis/ControlParameterRegularSampleGrid.h"
#include "Analysis/NDMeshAnalysisData.h"
#include "Analysis/XMDNetworkControlTake.h"
#include "Analysis/AnalysisParameteriser.h"
#include "Analysis/NDMeshResampler.h"
#include "Analysis/PredictionModelNDMeshBuilder.h"
#include "NMNumerics/NMNumericUtils.h"
#include "XMD/Model.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "morpheme/Prediction/mrScatteredDataUtils.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// PredictionModelNDMeshAnalyser functions.
//----------------------------------------------------------------------------------------------------------------------
void PredictionModelNDMeshAnalyser::tidyWorkingData(AP::AssetProcessor* processor)
{
  if (m_regularSampleGrid)
  {
    m_regularSampleGrid->releaseAndDestroy();
    m_regularSampleGrid = NULL;
  }
  if (m_analysisParameteriser)
  {
    // Release the analysis network.
    MR::Network* net = processor->getNetwork();
    if (net)
    {
      m_analysisParameteriser->instanceRelease(net);
      AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkFunc = processor->getReleaseNetworkInstanceFunc();
      if (releaseNetworkFunc)
      {
        releaseNetworkFunc(processor);
      }
    }

    // Clean up the parameteriser.
    m_analysisParameteriser->releaseAndDestroy();
    m_analysisParameteriser = NULL;
  }
  if (m_ndMeshResampler)
  {
    m_ndMeshResampler->releaseAndDestroy();
    m_ndMeshResampler = NULL;
  }
  if (m_resultDataNDMesh)
  {
    m_resultDataNDMesh->releaseAndDestroy();
    m_resultDataNDMesh = NULL;
  }

  NMP_ASSERT(!processor->getNetwork());
}

//----------------------------------------------------------------------------------------------------------------------
bool PredictionModelNDMeshAnalyser::analysisValidationFail(
  AP::AssetProcessor* processor,
  const char*         format,      // If non-zero, a printf-style formatted explanation of the validation error.
  ...)
{
  va_list argList;
  va_start(argList, format);
  processor->getErrorLogger()->output(format, argList);
  va_end(argList);
  
  processor->setProcessingFailFlag(true);
  tidyWorkingData(processor);

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool PredictionModelNDMeshAnalyser::analyse(
  AP::AssetProcessor*           processor,
  const ME::AnalysisTaskExport* task,
  ME::DataBlockExport*          resultData)
{
  NMP_VERIFY(processor);
  NMP_VERIFY(task);
  NMP_VERIFY(resultData);

  // Wipe working data structures which are created at some point within this function.
  m_regularSampleGrid = NULL;
  m_analysisParameteriser = NULL;
  m_ndMeshResampler = NULL;
  m_resultDataNDMesh = NULL;

  const ME::DataBlockExport* analysisTaskDataBlock = task->getDataBlock();
  int32_t usedAnimSets[128];
  size_t usedAnimSetCount = 0;
  analysisTaskDataBlock->readIntArray(usedAnimSets, 128, usedAnimSetCount, "UsedAnimSets");

  //---------------------------
  // The first three analysisNodes are the PredictionModelNDMesh, SimpleNetworkControlAssetCompiler
  // and the ControlParameterRegularSampleGrid. The remaining analysisNodes are the analysis conditions,
  // analysis properties and resample grids.
  NMP_VERIFY_MSG(
              task->getNumAnalysisNodes() >= 3,
              "Expected at least three analysisNodes as input for analysis, got %d",
              task->getNumAnalysisNodes());

  //---------------------------
  // The PredictionModelNDMesh analysisNode
  const ME::AnalysisNodeExport* ndMeshNodeExport = task->getAnalysisNode(0);
  NMP_VERIFY(ndMeshNodeExport);
  NMP_VERIFY_MSG(
              strcmp(ndMeshNodeExport->getTypeName(), "PredictionModelNDMesh") == 0,
              "Expected \"PredictionModelNDMesh\" as the first analysisNode input, got '%s'",
              ndMeshNodeExport->getTypeName());

  //---------------------------
  // Initialise the network control networkControlPlayer with the RunNetworkScriptedAssetCompiler and
  // AnalysisCondition analysisNodes
  NetworkControlPlayer networkControlPlayer;
  if (!networkControlPlayer.init(processor, task))
  {
    return false;
  }

  MR::NetworkDef* netDef = networkControlPlayer.getNetworkDef();
  uint32_t dimensionCount = networkControlPlayer.getNumDimensions();
  uint32_t frameCount = networkControlPlayer.getFrameCount();
  float deltaTime = networkControlPlayer.getDeltaTime();
  float sampleFrequency = networkControlPlayer.getSampleFrequency();

  AnalysisInterval* analysisInterval = networkControlPlayer.getAnalysisInterval();

  AP::AssetProcessor::InitNetworkInstanceFunc initNetworkFunc = processor->getInitNetworkInstanceFunc();
  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkFunc = processor->getUpdateNetworkInstanceFunc();
  AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkFunc = processor->getReleaseNetworkInstanceFunc();

  AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
  initArgs.m_startStateNodeID = networkControlPlayer.getStartStateNodeID();
  AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;

  //---------------------------
  // The ControlParameterRegularSampleGrid
  const ME::AnalysisNodeExport* regularGridExport = task->getAnalysisNode(2);
  NMP_VERIFY(regularGridExport);
  NMP_VERIFY_MSG(
              strcmp(regularGridExport->getTypeName(), "ControlParameterRegularSampleGrid") == 0,
              "Expected \"ControlParameterRegularSampleGrid\" as the second analysisNode input, got '%s'",
              regularGridExport->getTypeName());
  m_regularSampleGrid = ControlParameterRegularSampleGrid::createFromExport(regularGridExport, netDef);
  uint32_t numControllingParameters = m_regularSampleGrid->getNumControlParameters();

  // Verify the NetworkControl and the regular grid samples are identical
  NMP_VERIFY_MSG(
              dimensionCount == numControllingParameters,
              "The number of NetworkControl sample dimensions (%d) should match the number of controlling parameters (%d)",
              dimensionCount,
              numControllingParameters);

  for (uint32_t i = 0; i < numControllingParameters; ++i)
  {
    uint32_t takesForDimension = networkControlPlayer.getNumTakesForDimension(i);
    uint32_t sampleCountForCP = m_regularSampleGrid->getSampleCount(i);
    NMP_VERIFY_MSG(
                takesForDimension == sampleCountForCP,
                "The sample counts differ between the NetworkControl and the RegularGrid for dimention %d",
                i);
  }

  //---------------------------
  // Initialise the analysis parameteriser with the analysis properties
  m_analysisParameteriser = AnalysisParameteriser::create(16);
  m_analysisParameteriser->readAnalysisProperties(
                            processor,
                            netDef,
                            sampleFrequency,
                            frameCount,
                            task,
                            ndMeshNodeExport);
  uint32_t numAnalysisProperties = m_analysisParameteriser->getNumAnalysisProperties();
  if (numAnalysisProperties == 0) // Validation.
  {
    return analysisValidationFail(processor, "There are no analysis properties to evaluate");
  }


  //---------------------------
  // Create the NDMesh result sample data
  m_resultDataNDMesh = NDMeshAnalysisData::createForAnalysis(m_regularSampleGrid, numAnalysisProperties);
  uint32_t numAnalysedSamples = m_resultDataNDMesh->getNumAnalysedSamples();

  //---------------------------
  // Initialise the analysis resampler with the resample grids
  m_ndMeshResampler = m_ndMeshResampler->createForAnalysis(
    task,
    ndMeshNodeExport,
    m_regularSampleGrid);

  //---------------------------
  bool analysisResult = true;
  for(uint32_t usedAnimSetIt = 0; usedAnimSetIt < (uint32_t)usedAnimSetCount; ++usedAnimSetIt)
  {
    MR::AnimSetIndex setIndex = (MR::AnimSetIndex)usedAnimSets[usedAnimSetIt];
    NMP_VERIFY(setIndex < netDef->getNumAnimSets());
    initArgs.m_startingAnimSetIndex = setIndex;

    //---------------------------
    // Iterate over each take (sample point).
    for (uint32_t sampleIndex = 0; sampleIndex < numAnalysedSamples; ++sampleIndex)
    {
      //---------------------------
      // Get the take for this sample.
      // Note: The network player sorts the takes from the exported file into the correct order.
      const ControlTake* take = networkControlPlayer.getTake(sampleIndex);
      NMP_VERIFY(take);

      // Initialise a Network instance for this take. Note that this also ticks the
      // network with absolute time. This is required to set the start state correctly
      // so that messages broadcast on the first frame are handled appropriately.
      uint32_t frameIndex = 0;
      bool status = initNetworkFunc(processor, netDef, initArgs);
      NMP_VERIFY(status);
      MR::Network* net = processor->getNetwork();
      NMP_VERIFY(net);

      // Add any post update access attribs that are required by the interval conditions.
      analysisInterval->instanceInit(net);
      analysisInterval->instanceReset();
      m_analysisParameteriser->instanceInit(net);

      // Set the relevant control parameters and requests for this frame
      take->updateNetwork(net, frameIndex);

      // Make the first update of the network with absolute time
      updateArgs.m_updateTime = 0.0f;
      updateArgs.m_absTime = true;
      updateNetworkFunc(processor, updateArgs);

      // Update the analysis interval
      analysisInterval->instanceUpdate(net, frameIndex);

      //---------------------------
      // Update the network until the start analysis interval conditions are satisfied.
      updateArgs.m_updateTime = deltaTime;
      updateArgs.m_absTime = false;
      while (!analysisInterval->getStartTriggered())
      {
        // Start another frame update
        frameIndex++;         
        if (frameIndex >= frameCount) // Validation.
          return analysisValidationFail(processor, "The start analysis interval conditions were not satisfied over the specified number of update frames");

        // Set the relevant control parameters and requests for this frame
        take->updateNetwork(net, frameIndex);

        // Update the network itself.
        updateNetworkFunc(processor, updateArgs);

        // Update the analysis interval
        analysisInterval->instanceUpdate(net, frameIndex);
      }

      //---------------------------
      // Initialise the first sample of the analysis property feature data
      m_analysisParameteriser->instanceUpdate(net);

      //---------------------------
      // Iterate over the take frames extracting the data we are interested in
      //  until the stop analysis interval conditions are satisfied.
      while (!analysisInterval->getStopTriggered())
      {
        frameIndex++;

        if (!(analysisInterval->getStopConditions()->size() == 0 || frameIndex < frameCount)) // Validation.
          return analysisValidationFail(processor, "The stop analysis interval conditions were not satisfied over the specified number of update frames");
        
        // Set the relevant control parameters and requests for this frame
        take->updateNetwork(net, frameIndex);

        // Update the network itself.
        updateNetworkFunc(processor, updateArgs);

        // Update the analysis property feature data
        m_analysisParameteriser->instanceUpdate(net);

        // Update the analysis interval
        analysisInterval->instanceUpdate(net, frameIndex);
      }

      // The specified start and end sampling analysis conditions have produced no frames for sampling.
      if ((analysisInterval->getStartFrame() == 0) && (analysisInterval->getStopFrame() == 0)) // Validation.
      {
        return analysisValidationFail(processor, "The specified start and end sampling analysis conditions have produced no frames for sampling");
      }

      //---------------------------
      // Evaluate the analysis properties
      m_analysisParameteriser->evaluateProperties();

      // Control parameters
      for (uint32_t cpIndex = 0; cpIndex < numControllingParameters; ++cpIndex)
      {
        // Extract the current control param value from the network.
        MR::NodeID cpNodeID = m_regularSampleGrid->getControlParamNodeID(cpIndex);
        MR::AttribDataFloat* floatCP_Data = (MR::AttribDataFloat*)net->getControlParameter(cpNodeID);
        NMP_VERIFY(floatCP_Data);
        NMP_VERIFY_MSG(
                    floatCP_Data->getType() == MR::ATTRIB_TYPE_FLOAT,
                    "Prediction model builder does not yet cope with non float control parameters.");

        m_resultDataNDMesh->setControlParameterSample(cpIndex, sampleIndex, floatCP_Data->m_value);
      }

      // Analysis properties
      for (uint32_t apIndex = 0; apIndex < numAnalysisProperties; ++apIndex)
      {
        AnalysisProperty* analysisProperty = m_analysisParameteriser->getAnlysisProperty(apIndex);
        NMP_VERIFY(analysisProperty);
        NMP_VERIFY_MSG(
                    analysisProperty->m_numDims == 1,
                    "Prediction model builder does not yet cope with non float analysis properties.");

        m_resultDataNDMesh->setAnalysisPropertySample(apIndex, sampleIndex, analysisProperty->m_value[0]);
      }

      // Release the analysis network
      m_analysisParameteriser->instanceRelease(net);
      releaseNetworkFunc(processor);
    }

    //---------------------------
    // Write out the per set result data.
    m_resultDataNDMesh->writePerSetExportData(setIndex, resultData);

    //---------------------------
    // Resampling
    if (m_ndMeshResampler->getNumResampleMaps() > 0)
    {
      NMP_VERIFY_MSG(
        m_resultDataNDMesh->getBuildInverseMap(),
        "Expecting the buildInverseMap option to be set when resampling");

      // Build the NDMesh prediction model
      NMP::Memory::Resource memResModel = PredictionModelNDMeshBuilder::init(
        m_resultDataNDMesh,
        NULL);
      const MR::PredictionModelNDMesh* ndMeshModel = (const MR::PredictionModelNDMesh*)memResModel.ptr;
      NMP_VERIFY(ndMeshModel);

      // Resample the prediction model
      m_ndMeshResampler->analyse(ndMeshModel);

      // Write out the per set result data.
      m_ndMeshResampler->writePerSetExportData(setIndex, resultData);

      // Tidy up
      m_ndMeshResampler->tidyPerSetExportData();
      NMP::Memory::memFree(memResModel.ptr);
    }
  }

  //---------------------------
  // Write out the result data.
  m_resultDataNDMesh->writeExportData(resultData);
  m_ndMeshResampler->writeExportData(resultData);

  //---------------------------
  // Tidy up.
  tidyWorkingData(processor);

  return analysisResult;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------