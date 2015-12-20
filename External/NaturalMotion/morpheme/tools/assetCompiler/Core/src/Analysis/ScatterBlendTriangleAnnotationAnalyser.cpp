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
#include "Analysis/ScatterBlendTriangleAnnotationAnalyser.h"
#include "Analysis/NetworkControlPlayer.h"
#include "NodeScatterBlendBuilder.h"
#include "NodeScatterBlend2DAnnotationExport.h"
#include "NodeScatterBlend2DAnnotation.h"
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
// ScatterBlendTriangleAnnotationAnalyser
//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlendTriangleAnnotationAnalyser::analyse(
  AP::AssetProcessor* processor,
  const ME::AnalysisTaskExport* task,
  ME::DataBlockExport* resultData)
{
  NMP::BasicLogger* errorLogger = processor->getErrorLogger();

  // The first two analysisNodes are the ScatterBlendTriangleAnnotationAnalyser and RunNetworkScriptedAssetCompiler.
  // The remaining analysisNodes are the analysis conditions and analysis properties
  NMP_VERIFY_MSG(
    task->getNumAnalysisNodes() >= 2,
    "Expected at least two analysisNodes as input for analysis, got %d",
    task->getNumAnalysisNodes());

  //---------------------------
  // The ScatterBlendTriangleAnnotationAnalyser analysisNode task
  const ME::AnalysisNodeExport* annotationExport = task->getAnalysisNode(0);
  NMP_VERIFY(annotationExport);
  const char* annotationNodeName = annotationExport->getName();
  const char* annotationNodeType = annotationExport->getTypeName();
  NMP_VERIFY_MSG(
    strcmp(annotationNodeType, "Annotation") == 0,
    "Expected \"Annotation\" as the first analysisNode input, got '%s'",
    annotationNodeType);

  // Read the animation set index for analysis
  const ME::DataBlockExport* annotationDataBlock = annotationExport->getDataBlock();
  uint32_t analysisAnimSetIndex = 0xFFFFFFFF;
  annotationDataBlock->readUInt(analysisAnimSetIndex, "AnalysisAnimSetIndex");
  NMP_VERIFY_MSG(
    analysisAnimSetIndex != 0xFFFFFFFF,
    "Invalid animation set for analysis");

  //---------------------------
  // Initialise the network control player with the RunNetworkScriptedAssetCompiler and
  // AnalysisCondition analysisNodes
  NetworkControlPlayer networkControlPlayer;
  networkControlPlayer.init(processor, task);
  MR::NetworkDef* networkDef = networkControlPlayer.getNetworkDef();

  //---------------------------
  // Read the vertex samples from the exported scatter blend node
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
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_2D);

  // grab the annotation we are exporting from the network export,
  // this node has the correct NodeIDs for the location nodes
  const ME::AnalysisNodeExport* annotationNetworkExport = 0;
  uint32_t analysisNodeCount = nodeDefExport->getNumAnalysisNodes();
  for(uint32_t analysisNodeIndex = 0; analysisNodeIndex < analysisNodeCount; ++analysisNodeIndex)
  {
    ME::AnalysisNodeExport* analysisNode = nodeDefExport->getAnalysisNode(analysisNodeIndex);

    const char* name = analysisNode->getName();
    if(strcmp(name, annotationNodeName) == 0)
    {
      annotationNetworkExport = analysisNode;
      break;
    }
  }

  NMP_VERIFY(annotationNetworkExport != 0);

  uint32_t frameCount = networkControlPlayer.getFrameCount();
  float sampleFrequency = networkControlPlayer.getSampleFrequency();

  // The number of sample dimensions should match the number of motion parameters
  uint32_t dimensionCount = networkControlPlayer.getNumDimensions();
  NMP_VERIFY_MSG(
    dimensionCount == 1,
    "The number of sample dimensions (%d) should be 1 for scatter blend annotation analysis",
    dimensionCount);

  uint32_t numSamples = networkControlPlayer.getNumTakes();
  NMP_VERIFY_MSG(
    numSamples == 1,
    "Expecting a single sample for the scatter blend annotation analysis");

  //---------------------------
  // Initialise the analysis parameteriser with the analysis properties
  AnalysisParameteriser* analysisParameteriser = AnalysisParameteriser::create(16);
  analysisParameteriser->readAnalysisProperties(
    processor,
    networkDef,
    sampleFrequency,
    frameCount,
    task,
    annotationExport);
  uint32_t numAnalysisProperties = analysisParameteriser->getNumAnalysisProperties();
  NMP_VERIFY_MSG(
    numAnalysisProperties == 2,
    "Expecting to process 2 analysis properties, instead found %d properties",
    numAnalysisProperties);

  // Read the vertex source data from the scatter blend node export.
  std::vector<MR::NodeID> sourceNodeIDs;
  SB2DAnnotationExportDataPacked annotationVertexData;
  NodeScatterBlendBuilder::loadScatterBlendVertexSources(
    (MR::AnimSetIndex)analysisAnimSetIndex,
    nodeDefExport,
    sourceNodeIDs,
    annotationVertexData);

  NMP_VERIFY(sourceNodeIDs.size() > 0);

  // Populate the annotation graph from the export data
  SB2DAnnotation annotation;
  annotation.setAnnotation(annotationVertexData);

  // Read the annotation data
  SB2DTriangleSubSampleData triangleSubSampleData;
  triangleSubSampleData.init(annotationVertexData.getNumTriangleSubDivisions());

  // use the network exported data block here, as we read off exported runtime ids from the annotation, and these
  // are only correct when in a network export (not an analysis library export)
  const ME::DataBlockExport* annotationNetworkDataExport = annotationNetworkExport->getDataBlock();
  triangleSubSampleData.readDataBlock(annotationNetworkDataExport, &sourceNodeIDs.front(), (uint32_t) sourceNodeIDs.size());

  // Add the triangle annotation. Note that we don't care about the counter-clockwise
  // geometry of the triangle annotation at this time.
  annotation.addTriangle(triangleSubSampleData.getTriangleVertexIDs());

  //---------------------------
  // Evaluate the interior sub-samples
  if (!evaluateInteriorSubSamples(
    &networkControlPlayer,
    analysisParameteriser,
    (MR::AnimSetIndex)analysisAnimSetIndex,
    sourceNodeIDs,
    annotation))
  {
    // Tidy up
    analysisParameteriser->releaseAndDestroy();

    return false;
  }

  // Convert the annotation graph into the export format
  SB2DAnnotationExportDataUnpacked annotationExportData;
  annotation.getAnnotation(annotationExportData);

  //---------------------------
  // Write out the pre-init export data
  NMP_VERIFY(annotationExportData.getNumTriangles() == 1);
  annotationExportData.getTriangleSubSampleData(0, triangleSubSampleData);
  triangleSubSampleData.writeDataBlock(resultData);

  // Tidy up
  analysisParameteriser->releaseAndDestroy();

  //---------------------------
  // Check if the triangle vertices are collinear
  bool isCollinear = triangleSubSampleData.isVertexOrderingCollinear();
  if (isCollinear && errorLogger)
  {
    errorLogger->output(
      "Triangle annotation %s has collinear vertices\n",
      annotationNodeName);
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ScatterBlendTriangleAnnotationAnalyser::evaluateInteriorSubSamples(
  NetworkControlPlayer* networkControlPlayer,
  AnalysisParameteriser* analysisParameteriser,
  MR::AnimSetIndex analysisAnimSetIndex,
  const std::vector<MR::NodeID>& sourceNodeIDs,
  SB2DAnnotation& annotation)
{
  NMP_VERIFY(networkControlPlayer);
  NMP_VERIFY(analysisParameteriser);

  AP::AssetProcessor* processor = networkControlPlayer->getAssetProcessor();
  AP::AssetProcessor::InitNetworkInstanceFunc initNetworkFunc = processor->getInitNetworkInstanceFunc();
  AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkFunc = processor->getReleaseNetworkInstanceFunc();
  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkFunc = processor->getUpdateNetworkInstanceFunc();

  MR::NetworkDef* networkDef = networkControlPlayer->getNetworkDef();
  NMP_VERIFY(networkDef);
  const ControlTake* take = networkControlPlayer->getTake(0);
  NMP_VERIFY(take);

  AnalysisInterval* analysisInterval = networkControlPlayer->getAnalysisInterval();
  uint32_t frameCount = networkControlPlayer->getFrameCount();
  float deltaTime = networkControlPlayer->getDeltaTime();

  AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
  initArgs.m_startStateNodeID = networkControlPlayer->getStartStateNodeID();
  initArgs.m_startingAnimSetIndex = analysisAnimSetIndex;
  AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;

  uint32_t numSubDivisions = annotation.getNumTriangleSubDivisions();
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numSubDivisions);

  // Although it's generally considered bad to modify the Def data, the analysis
  // attribute data should be considered as genuine playground memory.
  MR::NodeDef* scatterBlendNodeDef = networkDef->getNodeDef(networkDef->getRootNodeID());
  NMP_VERIFY(scatterBlendNodeDef && scatterBlendNodeDef->getNodeTypeID() == NODE_TYPE_SCATTER_BLEND_2D);
  MR::AttribDataScatterBlendAnalysisDef* attribDef = scatterBlendNodeDef->getAttribData<MR::AttribDataScatterBlendAnalysisDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, 0);
  attribDef->m_numNodeIDs = 3;
  attribDef->m_numWeights = 2;

  SB2DTriangleSubSampleElementMap elementMap;
  SB2DTriangleSubSampleData triangleSubSampleData;
  elementMap.init(numSubDivisions);
  triangleSubSampleData.init(numSubDivisions);

  //---------------------------
  const std::list<SB2DTriangle*> triangleList = annotation.getTriangleList();
  std::list<SB2DTriangle*>::const_iterator tit;
  for (tit = triangleList.begin(); tit != triangleList.end(); ++tit)
  {
    SB2DTriangle* triangle = *tit;
    NMP_VERIFY(triangle);

    // Get the triangle sub-sample element map
    triangle->getTriangleSubSampleElementMap(elementMap);

    // Get the triangle sub-sample data
    triangle->getTriangleSubSampleData(elementMap, triangleSubSampleData);

    // Recover the node IDs corresponding to the triangle vertices
    uint32_t vertexIDs[3];
    triangleSubSampleData.getTriangleVertexIDs(
      vertexIDs[0],
      vertexIDs[1],
      vertexIDs[2]);
    for (uint32_t i = 0; i < 3; ++i)
    {
      NMP_VERIFY(vertexIDs[i] < (uint32_t)sourceNodeIDs.size());
      attribDef->m_nodeIDs[i] = sourceNodeIDs[vertexIDs[i]];
    }

    // Evaluate the sub-samples
    uint32_t numSubSamplesPerTriangle = triangleSubSampleData.getNumSubSamplesPerTriangle();
    for (uint32_t i = 0; i < numSubSamplesPerTriangle; ++i)
    {
      bool isValidFlag = triangleSubSampleData.getTriangleSubSampleValidFlag(i);
      if (!isValidFlag)
      {
        // Compute the blend weights
        float subSampleWeights[3];
        float blendWeights[2];
        subDivisionDataBlock.getTriangleSubSampleWeights(
          i,
          subSampleWeights[0],
          subSampleWeights[1],
          subSampleWeights[2]);
        NMP::triangleBarycentricToBilinear(
          subSampleWeights[0],
          subSampleWeights[1],
          subSampleWeights[2],
          blendWeights[0],
          blendWeights[1]);
        attribDef->m_weights[0] = NMP::clampValue(blendWeights[0], 0.0f, 1.0f);
        attribDef->m_weights[1] = NMP::clampValue(blendWeights[1], 0.0f, 1.0f);

        // Initialise a Network instance for this take. Note that this also ticks the
        // network with absolute time. This is required to set the start state correctly
        // so that messages broadcast on the first frame are handled appropriately.
        uint32_t frameIndex = 0;
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

        // TODO: Adjust the motion parameter for the wrapping range

        // Set the triangle sub-sample data
        AnalysisProperty* analysisPropertyX = analysisParameteriser->getAnlysisProperty(0);
        NMP_VERIFY_MSG(
          analysisPropertyX->m_numDims == 1,
          "Expecting scalar analysis properties only");
        float valueX = analysisPropertyX->m_value[0];

        AnalysisProperty* analysisPropertyY = analysisParameteriser->getAnlysisProperty(1);
        NMP_VERIFY_MSG(
          analysisPropertyY->m_numDims == 1,
          "Expecting scalar analysis properties only");
        float valueY = analysisPropertyY->m_value[0];

        triangleSubSampleData.setTriangleSubSample(i, valueX, valueY);

        // Release the analysis network
        releaseNetworkFunc(processor);
      }
    }

    // Toggle the state of the valid flags so that we only add the entries that
    // have just been computed for the sub-network evaluation
    triangleSubSampleData.toggleTriangleSubSampleValidFlags();

    // Set the triangle sub-sample data
    triangle->setTriangleSubSampleData(elementMap, triangleSubSampleData);
  }

  // Validation
  NMP_VERIFY(annotation.isValidElementSubSampleData());
  return true;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
