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
#include "Analysis/ScatterBlend1DTask.h"
#include "Analysis/NetworkControlPlayer.h"
#include "Analysis/XMDNetworkControlTake.h"
#include "NMNumerics/NMNumericUtils.h"
#include "XMD/Model.h"
#include "export/mcExportXml.h"
#include "assetProcessor/AssetProcessor.h"
#include "morpheme/Nodes/mrScatterBlend1DUtils.h"
#include "NodeScatterBlend1DAnnotationExport.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlend1DTask
//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlend1DTask::analyse(
  AP::AssetProcessor* processor,
  const ME::AnalysisTaskExport* task,
  ME::DataBlockExport* resultData)
{
  // The first two analysisNodes are the ScatterBlend1DTask and RunNetworkScriptedAssetCompiler.
  // The remaining analysisNodes are the analysis conditions
  NMP_VERIFY_MSG(
    task->getNumAnalysisNodes() >= 2,
    "Expected at least two analysisNodes as input for analysis, got %d",
    task->getNumAnalysisNodes());

  //---------------------------
  // The ScatterBlend1DTask analysisNode
  const ME::AnalysisNodeExport* taskAnalysisNode = task->getAnalysisNode(0);
  NMP_VERIFY(taskAnalysisNode);
  NMP_VERIFY_MSG(
    strcmp(taskAnalysisNode->getTypeName(), "ScatterBlend1DTask") == 0,
    "Expected \"ScatterBlend1DTask\" as the first analysisNode input, got '%s'",
    taskAnalysisNode->getTypeName());

  //---------------------------
  // Initialise the network control player with the RunNetworkScriptedAssetCompiler and
  // AnalysisCondition analysisNodes
  NetworkControlPlayer networkControlPlayer;
  networkControlPlayer.init(processor, task);
  MR::NetworkDef* networkDef = networkControlPlayer.getNetworkDef();

  //---------------------------
  // Read the scatter blend export definition
  const ME::AnalysisNodeExport* networkControlAnalysisNode = networkControlPlayer.getNetworkControlAnalysisNode();
  const ME::DataBlockExport* statDataBlock = networkControlAnalysisNode->getDataBlock();
  NMP_VERIFY(statDataBlock);
  std::string exportedNetworkPath;
  statDataBlock->readString(exportedNetworkPath, "ExportedNetworkPath");

  // Attempt to load the exported asset
  ME::ExportFactoryXML exportFactory;
  ME::AssetExport* loadedAsset = exportFactory.loadAsset(exportedNetworkPath.c_str());
  NMP_VERIFY(loadedAsset);

  // find the scatter blend export
  ME::NetworkDefExport* networkExport = dynamic_cast<ME::NetworkDefExport*>(loadedAsset);
  NMP_VERIFY(networkExport);

  // grab the scatter blend node from the root of the exported network.
  ME::NodeExport* nodeDefExport = networkExport->getNode(networkExport->getRootNodeID());
  NMP_VERIFY(nodeDefExport);
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_1D);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Sub-sampling (i.e. the target sub-division level for analysis)
  ScatterBlend1DParamAttributes paramAttribs;
  paramAttribs.m_numSubDivisions = 0;
  bool status = nodeDefDataBlock->readUInt(paramAttribs.m_numSubDivisions, "NumSubDivisions");
  NMP_VERIFY_MSG(
    status,
    "NumSubDivisions attribute is not defined");
  NMP_VERIFY(paramAttribs.m_numSubDivisions < 3);

  // Network control information
  uint32_t frameCount = networkControlPlayer.getFrameCount();
  float sampleFrequency = networkControlPlayer.getSampleFrequency();

  // The number of sample dimensions should match the number of motion parameters
  uint32_t dimensionCount = networkControlPlayer.getNumDimensions();
  NMP_VERIFY_MSG(
    dimensionCount == 1,
    "The number of sample dimensions (%d) should be 1 for a trajectory analysis",
    dimensionCount);

  uint32_t numTakes = networkControlPlayer.getNumTakes();
  NMP_VERIFY_MSG(
    numTakes == 1,
    "Expecting a single sample for trajectory analysis");

  AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
  initArgs.m_startStateNodeID = networkControlPlayer.getStartStateNodeID();
  AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;

  //---------------------------
  // Initialise the analysis parameteriser with the analysis properties
  AnalysisParameteriser* analysisParameteriser = AnalysisParameteriser::create(16);
  analysisParameteriser->readAnalysisProperties(
    processor,
    networkDef,
    sampleFrequency,
    frameCount,
    task,
    taskAnalysisNode);

  uint32_t numAnalysisProperties = analysisParameteriser->getNumAnalysisProperties();
  NMP_VERIFY_MSG(
    numAnalysisProperties == 1,
    "Expected one analysis property to analyse");

  AnalysisPropertyBuilder* apBuilder = analysisParameteriser->getAnalysisPropertyBuilder(processor, 0, task, taskAnalysisNode);
  NMP_VERIFY_MSG(
    apBuilder,
    "Invalid analysis property type.");

  //---------------------------
  // Calculate the line sub-sample weights
  const MR::ScatterBlend1DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend1DSubSampleManager::getSubDivisionDataBlock(paramAttribs.m_numSubDivisions);
  uint32_t numSubSamplesPerLine = subDivisionDataBlock.getNumSubSamplesPerLine();

  float weights[5];
  for (uint32_t subSampleIndex = 0; subSampleIndex < numSubSamplesPerLine; ++subSampleIndex)
  {
    // Compute the blend weight
    float subSampleWeights[2];
    subDivisionDataBlock.getLineSubSampleWeights(
      subSampleIndex,
      subSampleWeights[0],
      subSampleWeights[1]);
    weights[subSampleIndex] = subSampleWeights[1];
    NMP_VERIFY(weights[subSampleIndex] >= 0.0f && weights[subSampleIndex] <= 1.0f);
  }

  //---------------------------
  MR::AnimSetIndex numAnimSets = networkDef->getNumAnimSets();
  for(MR::AnimSetIndex animSet = 0; animSet < numAnimSets; ++animSet)
  {    
    // Evaluate the vertex sources (including wrapping samples). The vertex sample data
    // is evaluated in the same order as the connected source nodes. Any additional wrapping
    // vertex samples are appended to the end of the sample array.
    std::vector<float> vertexSamples;
    bool vertexResult = evaluateVertexSamples(
      &networkControlPlayer,
      animSet,
      task,
      analysisParameteriser,
      &paramAttribs,
      vertexSamples);

    if (!vertexResult)
    {
      return false;
    }

    //---------------------------
    // Try to append wrapping vertex samples
    bool leftWrappingSamplesValid;
    bool rightWrappingSamplesValid;
    float leftLineWrappingSamples[5];
    float rightLineWrappingSamples[5];
    uint32_t leftVertexSourceIndex;
    uint32_t rightVertexSourceIndex;

    evaluateWrappingSamples(
      processor,
      &networkControlPlayer,
      animSet,
      analysisParameteriser,
      apBuilder,
      &paramAttribs,
      vertexSamples,
      weights,
      leftWrappingSamplesValid,
      rightWrappingSamplesValid,
      leftVertexSourceIndex,
      rightVertexSourceIndex,
      leftLineWrappingSamples,
      rightLineWrappingSamples);

    //---------------------------
    // Allocate the memory of the sample export data. The export data is stored with a specific order:
    //   1) Vertex samples corresponding to the connected source nodes.
    //   2) Vertex samples of any appended wrapping samples
    //   3) Sub-samples that in-between the annotated line segments (increasing in value)   
    uint32_t numSourceNodes = (uint32_t)vertexSamples.size();
    uint32_t numVertices = numSourceNodes;
    if (leftWrappingSamplesValid)
      numVertices++;
    if (rightWrappingSamplesValid)
      numVertices++;
    uint32_t numLines = numVertices - 1;
    uint32_t numSubSamples = (numSubSamplesPerLine - 2) * numLines;

    SB1DAnnotationExportDataPacked annotationExportData;
    annotationExportData.init(
      paramAttribs.m_numSubDivisions,
      numSubSamplesPerLine,
      numVertices,
      numLines,
      numSubSamples);

    // Set the export vertex sample data
    for (uint32_t i = 0; i < numSourceNodes; ++i)
    {
      annotationExportData.setSample(i, vertexSamples[i]);
      annotationExportData.setVertexSourceIndex(i, i);
    }

    // Set the left wrapping vertex sample
    uint32_t sampleEntryIndex = numSourceNodes;
    if (leftWrappingSamplesValid)
    {
      annotationExportData.setSample(sampleEntryIndex, leftLineWrappingSamples[0]);
      annotationExportData.setVertexSourceIndex(sampleEntryIndex, leftVertexSourceIndex);
      sampleEntryIndex++;
    }

    // Set the right wrapping vertex sample
    if (rightWrappingSamplesValid)
    {
      annotationExportData.setSample(sampleEntryIndex, rightLineWrappingSamples[numSubSamplesPerLine - 1]);
      annotationExportData.setVertexSourceIndex(sampleEntryIndex, rightVertexSourceIndex);
      sampleEntryIndex++;
    }

    // Annotate the line segments between vertex samples
    annotateVertexSamples(annotationExportData);

    //---------------------------
    // Set the interior sub-samples for the left wrapping line
    uint32_t leftIndex = 0;
    if (leftWrappingSamplesValid)
    {
      uint32_t sampleIndexA, sampleIndexB;
      annotationExportData.getLineSubSampleIndices(
        leftIndex,
        sampleIndexA, // sample entry index for the line segment left end-point
        sampleIndexB); // sample entry index for the line segment right end-point

      for (uint32_t subSampleIndex = 1; subSampleIndex < numSubSamplesPerLine - 1; ++subSampleIndex)
      {
        // Set the sub-sample data
        annotationExportData.setSample(
          sampleEntryIndex,
          leftLineWrappingSamples[subSampleIndex]);

        // Set the sample entry index for the line segment
        annotationExportData.setLineSubSampleIndex(
          leftIndex,
          subSampleIndex,
          sampleEntryIndex);

        sampleEntryIndex++;
      }

      leftIndex++;
    }

    //---------------------------
    // Set the interior sub-samples for the right wrapping line
    uint32_t rightIndex = annotationExportData.getNumLines() - 1;
    if (rightWrappingSamplesValid)
    {
      uint32_t sampleIndexA, sampleIndexB;
      annotationExportData.getLineSubSampleIndices(
        rightIndex,
        sampleIndexA, // sample entry index for the line segment left end-point
        sampleIndexB); // sample entry index for the line segment right end-point

      for (uint32_t subSampleIndex = 1; subSampleIndex < numSubSamplesPerLine - 1; ++subSampleIndex)
      {
        // Set the sub-sample data
        annotationExportData.setSample(
          sampleEntryIndex,
          rightLineWrappingSamples[subSampleIndex]);

        // Set the sample entry index for the line segment
        annotationExportData.setLineSubSampleIndex(
          rightIndex,
          subSampleIndex,
          sampleEntryIndex);

        sampleEntryIndex++;
      }

      rightIndex--;
    }

    //---------------------------
    // Evaluate the interior sub-samples for the remaining line segments
    float lineSamples[5];    
    for (uint32_t lineIndex = leftIndex; lineIndex <= rightIndex; ++lineIndex)
    {
      uint32_t sampleIndexA, sampleIndexB;
      annotationExportData.getLineSubSampleIndices(
        lineIndex,
        sampleIndexA, // sample entry index for the line segment left end-point
        sampleIndexB); // sample entry index for the line segment right end-point

      NMP_VERIFY(sampleIndexA < numVertices);
      NMP_VERIFY(sampleIndexB < numVertices);
      uint32_t vertexSourceIndexA = annotationExportData.getVertexSourceIndex(sampleIndexA);
      uint32_t vertexSourceIndexB = annotationExportData.getVertexSourceIndex(sampleIndexB);

      bool statusResult = evaluateLineInteriorSubSamples(
        &networkControlPlayer,
        animSet,
        analysisParameteriser,
        vertexSourceIndexA,
        vertexSourceIndexB,
        numSubSamplesPerLine - 2,
        &weights[1],
        &lineSamples[1]);

      if (!statusResult)
      {
        return false;
      }

      for (uint32_t subSampleIndex = 1; subSampleIndex < numSubSamplesPerLine - 1; ++subSampleIndex)
      {
        // Set the sub-sample data
        annotationExportData.setSample(
          sampleEntryIndex,
          lineSamples[subSampleIndex]);

        // Set the sample entry index for the line segment
        annotationExportData.setLineSubSampleIndex(
          lineIndex,
          subSampleIndex,
          sampleEntryIndex);

        sampleEntryIndex++;
      }
    }

    //---------------------------
    // Set the valid flags
    annotationExportData.setAnnotationValidFlags(true);

    // Compute the ranges of the parametric sub-samples
    SB1DAnnotationSubSampleRangeData annotationRangeData;

    apBuilder->calculateRangeDistribution(
      processor,
      annotationExportData.getNumSamples(),
      annotationExportData.getSamples(),
      annotationRangeData.m_samplesRangeMin,
      annotationRangeData.m_samplesRangeMax,
      annotationRangeData.m_samplesCentre);

    //---------------------------
    // Write out the pre-init export data
    writePerAnimSetIntermediateExportData(
      resultData,
      animSet,
      annotationExportData,
      annotationRangeData);
  }

  writeIntermediateExportData(
    resultData,
    paramAttribs.m_numSubDivisions);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlend1DTask::annotateVertexSamples(
  SB1DAnnotationExportDataPacked& annotationExportData)
{
  uint32_t numSubSamplesPerLine = annotationExportData.getNumSubSamplesPerLine();
  uint32_t numVertices = annotationExportData.getNumVertices();
  uint32_t numLines = numVertices - 1;

  // Sort the vertices by their motion parameter
  std::vector<uint32_t> vertexIndices;
  vertexIndices.resize(numVertices);
  for (uint32_t i = 0; i < numVertices; ++i)
  {
    vertexIndices[i] = i;
  }

  // Exchange sort
  for (uint32_t i = 0; i < numVertices - 1; ++i)
  {
    for (uint32_t j = i + 1; j < numVertices; ++j)
    {
      float valA = annotationExportData.getSample(vertexIndices[i]);
      float valB = annotationExportData.getSample(vertexIndices[j]);
      if (valB < valA)
      {
        NMP::nmSwap(vertexIndices[i], vertexIndices[j]);
      }
    }
  }

  // Annotate the vertex samples
  for (uint32_t lineIndex = 0; lineIndex < numLines; ++lineIndex)
  {
    annotationExportData.setLineSubSampleIndex(lineIndex, 0, vertexIndices[lineIndex]);
    annotationExportData.setLineSubSampleIndex(lineIndex, numSubSamplesPerLine - 1, vertexIndices[lineIndex + 1]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlend1DTask::evaluateVertexSamples(
  NetworkControlPlayer* networkControlPlayer,
  MR::AnimSetIndex animSet,
  const ME::AnalysisTaskExport* task,
  AnalysisParameteriser* analysisParameteriser,
  ScatterBlend1DParamAttributes* paramAttribs,
  std::vector<float>& vertexSamples)
{
  NMP_VERIFY(networkControlPlayer);
  NMP_VERIFY(analysisParameteriser);
  NMP_VERIFY(paramAttribs);

  AP::AssetProcessor* processor = networkControlPlayer->getAssetProcessor();
  AP::AssetProcessor::InitNetworkInstanceFunc initNetworkFunc = processor->getInitNetworkInstanceFunc();
  AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkFunc = processor->getReleaseNetworkInstanceFunc();
  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkFunc = processor->getUpdateNetworkInstanceFunc();

  MR::NetworkDef* networkDef = networkControlPlayer->getNetworkDef();
  NMP_VERIFY(networkDef);

  uint32_t frameCount = networkControlPlayer->getFrameCount();
  float deltaTime = networkControlPlayer->getDeltaTime();

  MR::NodeDef* scatterBlendNodeDef = networkDef->getNodeDef(networkDef->getRootNodeID());
  NMP_VERIFY(scatterBlendNodeDef && scatterBlendNodeDef->getNodeTypeID() == NODE_TYPE_SCATTER_BLEND_1D);

  uint32_t numSourceNodes = scatterBlendNodeDef->getNumChildNodes();
  NMP_VERIFY(numSourceNodes > 0);
  vertexSamples.resize(numSourceNodes);

  AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
  initArgs.m_startStateNodeID = networkControlPlayer->getStartStateNodeID();
  AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;

  AnalysisProperty* analysisProperty = analysisParameteriser->getAnlysisProperty(0);
  NMP_VERIFY(analysisProperty);
  NMP_VERIFY_MSG(
    analysisProperty->m_numDims == 1,
    "Expecting scalar analysis properties only");

  NMP_VERIFY(networkControlPlayer->getNumTakes() == 1);
  const ControlTake* take = networkControlPlayer->getTake(0);

  //---------------------------
  // Iterate over the vertex sources
  for (uint32_t iSource = 0; iSource < numSourceNodes; ++iSource)
  {
    // Temporarily set a new root node for the network def. (A bit dodgy messing with an
    // asset that isn't ours, but if we put everything back where we found it who cares).
    MR::NodeID sourceNodeID = scatterBlendNodeDef->getChildNodeID(iSource);
    MR::NodeDef* newRootNodeDef = networkDef->getNodeDef(sourceNodeID);
    MR::NodeID oldParentNodeID = newRootNodeDef->getParentNodeID();
    MR::NodeID oldRootNodeID = networkDef->getRootNodeID();
    bool oldCanCombineTrajectoryTransformTasks = networkDef->canCombineTrajectoryAndTransformSemantics();
    networkDef->setCanCombineTrajectoryAndTransformSemantics(false);
    networkDef->setChildNodeID(0, sourceNodeID);
    newRootNodeDef->setParentNodeID(MR::NETWORK_NODE_ID);

    // Create the analysis interval and trigger conditions. We need to do this here so
    // that the post update attrib access for data targeted at the root node can be
    // set correctly now that the root has changed to the child source.
    AnalysisInterval* analysisInterval = AnalysisInterval::create(16, frameCount);
    analysisInterval->readAnalysisConditions(processor, networkDef, task, networkControlPlayer->getNetworkControlAnalysisNode());

    // Initialise a Network instance for this take. Note that this also ticks the
    // network with absolute time. This is required to set the start state correctly
    // so that messages broadcast on the first frame are handled appropriately.
    uint32_t frameIndex = 0;
    initArgs.m_startingAnimSetIndex = animSet;
    bool initNetworkResult = initNetworkFunc(processor, networkDef, initArgs);
    NMP_VERIFY(initNetworkResult);
    MR::Network* network = processor->getNetwork();
    NMP_VERIFY(network);

    // Add any post update access attribs that are required by the interval conditions.
    analysisInterval->instanceInit(network);
    analysisInterval->instanceReset();
    analysisParameteriser->instanceInit(network);

    // Set the relevant control parameters and requests for this frame
    take->updateNetwork(network, frameIndex);

    // Make the first update of the network with absolute time
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
    vertexSamples[iSource] = analysisProperty->m_value[0];

    // Release the analysis network
    analysisParameteriser->instanceRelease(network);
    releaseNetworkFunc(processor);

    // Release the analysis interval
    analysisInterval->releaseAndDestroy();

    // Restore starting NetworkDef connectivity state.
    newRootNodeDef->setParentNodeID(oldParentNodeID);
    networkDef->setChildNodeID(0, oldRootNodeID);
    networkDef->setCanCombineTrajectoryAndTransformSemantics(oldCanCombineTrajectoryTransformTasks);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlend1DTask::evaluateLineInteriorSubSamples(
  NetworkControlPlayer* networkControlPlayer,
  MR::AnimSetIndex animSet,
  AnalysisParameteriser* analysisParameteriser,
  uint32_t vertexSourceIndexA,
  uint32_t vertexSourceIndexB,
  uint32_t numSamples,
  const float* weights,
  float* apSamples)
{
  NMP_VERIFY(networkControlPlayer);
  NMP_VERIFY(analysisParameteriser);
  NMP_VERIFY(weights);
  NMP_VERIFY(apSamples);
  if (numSamples == 0)
  {
    return true;
  }

  AP::AssetProcessor* processor = networkControlPlayer->getAssetProcessor();
  AP::AssetProcessor::InitNetworkInstanceFunc initNetworkFunc = processor->getInitNetworkInstanceFunc();
  AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkFunc = processor->getReleaseNetworkInstanceFunc();
  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkFunc = processor->getUpdateNetworkInstanceFunc();

  MR::NetworkDef* networkDef = networkControlPlayer->getNetworkDef();
  NMP_VERIFY(networkDef);

  AnalysisInterval* analysisInterval = networkControlPlayer->getAnalysisInterval();
  uint32_t frameCount = networkControlPlayer->getFrameCount();
  float deltaTime = networkControlPlayer->getDeltaTime();

  AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
  initArgs.m_startStateNodeID = networkControlPlayer->getStartStateNodeID();

  AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;
  updateArgs.m_updateTime = deltaTime;
  updateArgs.m_absTime = false;

  NMP_VERIFY(networkControlPlayer->getNumTakes() == 1);
  const ControlTake* take = networkControlPlayer->getTake(0);

  AnalysisProperty* analysisProperty = analysisParameteriser->getAnlysisProperty(0);
  NMP_VERIFY(analysisProperty);
  NMP_VERIFY_MSG(
    analysisProperty->m_numDims == 1,
    "Expecting scalar analysis properties only");

  // Although it's generally considered bad to modify the Def data, the analysis
  // attribute data should be considered as genuine playground memory.
  MR::NodeDef* scatterBlendNodeDef = networkDef->getNodeDef(networkDef->getRootNodeID());
  NMP_VERIFY(scatterBlendNodeDef && scatterBlendNodeDef->getNodeTypeID() == NODE_TYPE_SCATTER_BLEND_1D);
  MR::AttribDataScatterBlendAnalysisDef* attribDef = scatterBlendNodeDef->getAttribData<MR::AttribDataScatterBlendAnalysisDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, 0);
  attribDef->m_numNodeIDs = 2;
  attribDef->m_numWeights = 1;

  // Evaluate the interior sub-samples
  attribDef->m_nodeIDs[0] = scatterBlendNodeDef->getChildNodeID(vertexSourceIndexA);
  attribDef->m_nodeIDs[1] = scatterBlendNodeDef->getChildNodeID(vertexSourceIndexB);

  for (uint32_t sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
  {
    attribDef->m_weights[0] = weights[sampleIndex];
    NMP_VERIFY(attribDef->m_weights[0] >= 0.0f && attribDef->m_weights[0] <= 1.0f);

    // Initialise a new network instance from the network definition
    uint32_t frameIndex = 0;
    initArgs.m_startingAnimSetIndex = animSet;
    bool initNetworkResult = initNetworkFunc(processor, networkDef, initArgs);
    NMP_VERIFY(initNetworkResult);
    MR::Network* network = processor->getNetwork();
    NMP_VERIFY(network);

    // Add any post update access attribs that are required by the interval conditions.
    analysisInterval->instanceInit(network);
    analysisInterval->instanceReset();
    analysisParameteriser->instanceInit(network);

    // Set the relevant control parameters and requests for this frame
    take->updateNetwork(network, frameIndex);

    // Make the first update of the network with absolute time
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

    // Set the line sub-sample
    apSamples[sampleIndex] = analysisProperty->m_value[0];

    // Release the analysis network
    analysisParameteriser->instanceRelease(network);
    releaseNetworkFunc(processor);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlend1DTask::calculateAdjustedWrappingSample(
  float& apValue, // IN / OUT
  float rangeMinVal,
  float rangeMaxVal,
  float vertexSourceValueA,
  float vertexSourceValueB)
{
  NMP_VERIFY(rangeMinVal < rangeMaxVal);
  NMP_VERIFY(vertexSourceValueA < vertexSourceValueB);
  float rangeDiff = rangeMaxVal - rangeMinVal;

  float wrappedValues[3];
  wrappedValues[0] = apValue - rangeDiff;
  wrappedValues[1] = apValue;
  wrappedValues[2] = apValue + rangeDiff;

  bool found = false;
  for (uint32_t i = 0; i < 3; ++i)
  {
    if (wrappedValues[i] >= vertexSourceValueA && wrappedValues[i] <= vertexSourceValueB)
    {
      found = true;
      apValue = wrappedValues[i];
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlend1DTask::evaluateWrappingSamples(
  AP::AssetProcessor* processor,
  NetworkControlPlayer* networkControlPlayer,
  MR::AnimSetIndex animSet,
  AnalysisParameteriser* analysisParameteriser,
  AnalysisPropertyBuilder* apBuilder,
  const ScatterBlend1DParamAttributes* paramAttribs,
  std::vector<float>& vertexSamples, // IN / OUT - Snaps endpoint vertices (within tolerance) to the wrapping bounds
  const float* weights,
  bool& leftWrappingSamplesValid,
  bool& rightWrappingSamplesValid,
  uint32_t& leftVertexSourceIndex,
  uint32_t& rightVertexSourceIndex,
  float* leftLineSamples,
  float* rightLineSamples)
{
  NMP_VERIFY(processor);
  NMP_VERIFY(networkControlPlayer);
  NMP_VERIFY(analysisParameteriser);
  NMP_VERIFY(apBuilder);
  NMP_VERIFY(paramAttribs);
  NMP_VERIFY(weights);
  NMP_VERIFY(leftLineSamples);
  NMP_VERIFY(rightLineSamples);

  leftWrappingSamplesValid = false;
  rightWrappingSamplesValid = false;

  //---------------------------
  // Check if the analysis property supports range wrapping
  uint32_t numSourceNodes = (uint32_t)vertexSamples.size();
  float rangeMinVal, rangeMaxVal, rangeCentreVal;
  bool rangeWrapping = apBuilder->calculateRangeDistribution(
    processor,
    (uint32_t)vertexSamples.size(),
    &vertexSamples[0],
    rangeMinVal,
    rangeMaxVal,
    rangeCentreVal);
  if (!rangeWrapping)
  {
    return;
  }

  //---------------------------
  // Compute the min and max of the sample distribution
  const MR::ScatterBlend1DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend1DSubSampleManager::getSubDivisionDataBlock(paramAttribs->m_numSubDivisions);

  uint32_t numSubSamplesPerLine = subDivisionDataBlock.getNumSubSamplesPerLine();
  float rangeDiff = rangeMaxVal - rangeMinVal;
  const float tol = 1e-4f;

  //---------------------------
  // Compute the min motion parameter
  uint32_t minValIndex = 0;
  for (uint32_t i = 1; i < numSourceNodes; ++i)
  {
    if (vertexSamples[i] < vertexSamples[minValIndex])
      minValIndex = i;
  }
  // Snap the min value to the wrapping bounds
  if (NMP::nmfabs(vertexSamples[minValIndex] - rangeMinVal) < tol)
  {
    vertexSamples[minValIndex] = rangeMinVal;
  }

  // Compute the max motion parameter
  uint32_t maxValIndex = 0;
  for (uint32_t i = 1; i < numSourceNodes; ++i)
  {
    if (vertexSamples[i] > vertexSamples[maxValIndex])
      maxValIndex = i;
  }
  // Snap the max value to the wrapping bounds
  if (NMP::nmfabs(vertexSamples[maxValIndex] - rangeMaxVal) < tol)
  {
    vertexSamples[maxValIndex] = rangeMaxVal;
  }

  //---------------------------
  // Check if we need to add additional vertices for the left wrapping region
  if (NMP::nmfabs(vertexSamples[minValIndex] - rangeMinVal) >= tol)
  {
    uint32_t vertexSourceIndexA = maxValIndex;
    uint32_t vertexSourceIndexB = minValIndex;

    // Initialise the line segment endpoints
    leftLineSamples[0] = vertexSamples[vertexSourceIndexA] - rangeDiff;
    leftLineSamples[numSubSamplesPerLine - 1] = vertexSamples[vertexSourceIndexB];
    NMP_VERIFY(leftLineSamples[0] <= leftLineSamples[numSubSamplesPerLine - 1]);
    leftVertexSourceIndex = vertexSourceIndexA;
    leftWrappingSamplesValid = true;

    // NOTE: There are no internal sub-samples for zero sub-divisions so we need to determine
    // if the added line segment produces valid blending
    if (paramAttribs->m_numSubDivisions == 0)
    {
      // Evaluate the mid sample point
      float midWeight = 0.5f;
      float midValue;
      bool statusResult = evaluateLineInteriorSubSamples(
        networkControlPlayer,
        animSet,
        analysisParameteriser,
        vertexSourceIndexA,
        vertexSourceIndexB,
        1,
        &midWeight,
        &midValue);

      if (statusResult)
      {
        bool found = calculateAdjustedWrappingSample(
          midValue,
          rangeMinVal,
          rangeMaxVal,
          leftLineSamples[0],
          leftLineSamples[numSubSamplesPerLine - 1]);
        if (!found)
        {
          leftWrappingSamplesValid = false;
        }
      }
      else
      {
        leftWrappingSamplesValid = false;
      }
    }
    else
    {
      // Evaluate the interior sub-samples for the left wrapping line
      bool statusResult = evaluateLineInteriorSubSamples(
        networkControlPlayer,
        animSet,
        analysisParameteriser,
        vertexSourceIndexA,
        vertexSourceIndexB,
        numSubSamplesPerLine - 2,
        &weights[1],
        &leftLineSamples[1]);

      if (statusResult)
      {
        // Adjust the interior sub-samples to lay within the line segment
        for (uint32_t subSampleIndex = 1; subSampleIndex < numSubSamplesPerLine - 1; ++subSampleIndex)
        {
          bool found = calculateAdjustedWrappingSample(
            leftLineSamples[subSampleIndex],
            rangeMinVal,
            rangeMaxVal,
            leftLineSamples[0],
            leftLineSamples[numSubSamplesPerLine - 1]);

          if (!found)
          {
            leftWrappingSamplesValid = false;
            break;
          }
        }
      }
      else
      {
        leftWrappingSamplesValid = false;
      }
    }
  }

  //---------------------------
  // Check if we need to add additional vertices for the right wrapping region
  if (NMP::nmfabs(vertexSamples[maxValIndex] - rangeMaxVal) >= tol)
  {
    uint32_t vertexSourceIndexA = maxValIndex;
    uint32_t vertexSourceIndexB = minValIndex;

    // Initialise the line segment endpoints
    rightLineSamples[0] = vertexSamples[vertexSourceIndexA];
    rightLineSamples[numSubSamplesPerLine - 1] = vertexSamples[vertexSourceIndexB] + rangeDiff;
    NMP_VERIFY(rightLineSamples[0] <= rightLineSamples[numSubSamplesPerLine - 1]);
    rightVertexSourceIndex = vertexSourceIndexB;
    rightWrappingSamplesValid = true;

    // NOTE: There are no internal sub-samples for zero sub-divisions so we need to determine
    // if the added line segment produces valid blending
    if (paramAttribs->m_numSubDivisions == 0)
    {
      // Evaluate the mid sample point
      float midWeight = 0.5f;
      float midValue;
      bool statusResult = evaluateLineInteriorSubSamples(
        networkControlPlayer,
        animSet,
        analysisParameteriser,
        vertexSourceIndexA,
        vertexSourceIndexB,
        1,
        &midWeight,
        &midValue);

      if (statusResult)
      {
        bool found = calculateAdjustedWrappingSample(
          midValue,
          rangeMinVal,
          rangeMaxVal,
          rightLineSamples[0],
          rightLineSamples[numSubSamplesPerLine - 1]);
        if (!found)
        {
          rightWrappingSamplesValid = false;
        }
      }
      else
      {
        rightWrappingSamplesValid = false;
      }
    }
    else
    {
      // Evaluate the interior sub-samples for the left wrapping line
      bool statusResult = evaluateLineInteriorSubSamples(
        networkControlPlayer,
        animSet,
        analysisParameteriser,
        vertexSourceIndexA,
        vertexSourceIndexB,
        numSubSamplesPerLine - 2,
        &weights[1],
        &rightLineSamples[1]);

      if (statusResult)
      {
        // Adjust the interior sub-samples to lay within the line segment
        for (uint32_t subSampleIndex = 1; subSampleIndex < numSubSamplesPerLine - 1; ++subSampleIndex)
        {
          bool found = calculateAdjustedWrappingSample(
            rightLineSamples[subSampleIndex],
            rangeMinVal,
            rangeMaxVal,
            rightLineSamples[0],
            rightLineSamples[numSubSamplesPerLine - 1]);

          if (!found)
          {
            rightWrappingSamplesValid = false;
            break;
          }
        }
      }
      else
      {
        rightWrappingSamplesValid = false;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlend1DTask::writeIntermediateExportData(
  ME::DataBlockExport* exportData,
  uint32_t numSubDivisions)
{
  NMP_VERIFY(exportData);

  exportData->writeUInt(
    numSubDivisions,
    "NumSubDivisions");
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlend1DTask::writePerAnimSetIntermediateExportData(
  ME::DataBlockExport* exportData,
  MR::AnimSetIndex animSet,
  const SB1DAnnotationExportDataPacked& annotationExportData,
  const SB1DAnnotationSubSampleRangeData& annotationRangeData)
{
  NMP_VERIFY(exportData);
  char desc[256];

  //---------------------------
  // NumVertices
  sprintf_s(desc, 255, "NumVertices_%d", animSet);
  uint32_t numVertices = annotationExportData.getNumVertices();
  exportData->writeUInt(
    numVertices,
    desc);

  //---------------------------
  // Samples
  sprintf_s(desc, 255, "SamplesCount_%d", animSet);
  uint32_t numSamples = annotationExportData.getNumSamples();
  const float* samples = annotationExportData.getSamples();
  exportData->writeUInt(
    numSamples,
    desc);

  sprintf_s(desc, 255, "Samples_%d", animSet);
  exportData->writeFloatArray(
    samples,
    numSamples,
    desc);

  //---------------------------
  // Vertex source indices
  sprintf_s(desc, 255, "VertexSourceIndicesCount_%d", animSet);
  const uint32_t* vertexSourceIndices = annotationExportData.getVertexSourceIndices();
  exportData->writeUInt(
    numVertices,
    desc);

  sprintf_s(desc, 255, "VertexSourceIndices_%d", animSet);
  exportData->writeUIntArray(
    vertexSourceIndices,
    numVertices,
    desc);

  //---------------------------
  // Lines
  uint32_t numLineSubSampleIndices = annotationExportData.getNumLineSampleIndices();
  const uint32_t* lineSubSampleIndices = annotationExportData.getLineSampleIndices();

  sprintf_s(desc, 255, "LineSubSampleIndicesCount_%d", animSet);
  exportData->writeUInt(
    numLineSubSampleIndices,
    desc);

  sprintf_s(desc, 255, "LineSubSampleIndices_%d", animSet);
  exportData->writeUIntArray(
    lineSubSampleIndices,
    numLineSubSampleIndices,
    desc);

  //---------------------------
  // Motion parameter sample distribution range. This is used to return
  // the sample distribution range back to connect.
  sprintf_s(desc, 255, "MotionParameterSamplesMin_%d", animSet);
  exportData->writeFloat(
    annotationRangeData.m_samplesRangeMin,
    desc);

  sprintf_s(desc, 255, "MotionParameterSamplesMax_%d", animSet);
  exportData->writeFloat(
    annotationRangeData.m_samplesRangeMax,
    desc);
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlend1DTask::readPerAnimSetIntermediateExportData(
  const ME::DataBlockExport* exportData,
  MR::AnimSetIndex animSet,
  SB1DAnnotationExportDataPacked& annotationExportData,
  SB1DAnnotationSubSampleRangeData& annotationRangeData)
{
  NMP_VERIFY(exportData);
  size_t length;
  bool status;
  char desc[256];

  //---------------------------
  // Release the old export data
  annotationExportData.release();
  annotationRangeData.clear();

  //---------------------------
  // Read the number of sub-divisions
  uint32_t numSubDivisions = 0xFFFFFFFF;
  exportData->readUInt(numSubDivisions, "NumSubDivisions");
  NMP_VERIFY_MSG(
    numSubDivisions < 3,
    "We only support less than three sub-division levels");

  //---------------------------
  // Initialise the annotation export data
  const MR::ScatterBlend1DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend1DSubSampleManager::getSubDivisionDataBlock(numSubDivisions);
  uint32_t numSubSamplesPerLine = subDivisionDataBlock.getNumSubSamplesPerLine();

  uint32_t numVertices = 0;
  sprintf_s(desc, 255, "NumVertices_%d", animSet);
  status = exportData->readUInt(
    numVertices,
    desc);
  NMP_VERIFY(status);
  NMP_VERIFY(numVertices > 0);

  uint32_t numLines = numVertices - 1;
  uint32_t numSubSamples = (numSubSamplesPerLine - 2) * numLines;

  annotationExportData.init(
    numSubDivisions,
    numSubSamplesPerLine,
    numVertices,
    numLines,
    numSubSamples);

  //---------------------------
  // Samples
  uint32_t numSamples = annotationExportData.getNumSamples();
  if (numSamples > 0)
  {
    sprintf_s(desc, 255, "Samples_%d", animSet);
    status = exportData->readFloatArray(
      annotationExportData.getSamples(),
      numSamples,
      length,
      desc);
    NMP_VERIFY(status);
  }

  //---------------------------
  // Vertex source indices
  if (numVertices > 0)
  {
    uint32_t* vertexSourceIndices = annotationExportData.getVertexSourceIndices();

    sprintf_s(desc, 255, "VertexSourceIndices_%d", animSet);
    status = exportData->readUIntArray(
      vertexSourceIndices,
      numVertices,
      length,
      desc);
    NMP_VERIFY(status);
  }

  //---------------------------
  // Lines
  if (numLines > 0)
  {
    uint32_t numLineSubSampleIndices = annotationExportData.getNumLineSampleIndices();
    uint32_t* lineSubSampleIndices = annotationExportData.getLineSampleIndices();

    sprintf_s(desc, 255, "LineSubSampleIndices_%d", animSet);
    status = exportData->readUIntArray(
      lineSubSampleIndices,
      numLineSubSampleIndices,
      length,
      desc);
    NMP_VERIFY(status);
  }

  //---------------------------
  // Motion parameter sample distribution range. This is used to return
  // the sample distribution range back to connect.
  sprintf_s(desc, 255, "MotionParameterSamplesMin_%d", animSet);
  exportData->readFloat(
    annotationRangeData.m_samplesRangeMin,
    desc);

  sprintf_s(desc, 255, "MotionParameterSamplesMax_%d", animSet);
  exportData->readFloat(
    annotationRangeData.m_samplesRangeMax,
    desc);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
