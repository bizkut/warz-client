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
#include <tchar.h>
#include "NodeScatterBlendBuilder.h"
#include "NetworkDefBuilder.h"
#include "NodeScatterBlend2DAnnotation.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/AnalysisParameteriser.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeScatterBlend2D.h"
#include "morpheme/Nodes/mrNodeScatterBlend2DSyncEvents.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
#include "NMNumerics/NMUniformQuantisation.h"
//----------------------------------------------------------------------------------------------------------------------

#define DEBUG_SCATTER_BLEND_MEMORYx

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NodeScatterBlendPreInitData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeScatterBlendPreInitData::getMemoryRequirements(uint32_t numAnimSets)
{
  // Header
  NMP::Memory::Format result(sizeof(NodeScatterBlendPreInitData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Attrib data handles
  NMP_VERIFY(numAnimSets > 0);
  NMP::Memory::Format memReqsHandles(sizeof(MR::AttribDataHandle) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsHandles;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeScatterBlendPreInitData* NodeScatterBlendPreInitData::init(
  NMP::Memory::Resource& resource,
  uint32_t numAnimSets)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NodeScatterBlendPreInitData), NMP_NATURAL_TYPE_ALIGNMENT);
  NodeScatterBlendPreInitData* result = (NodeScatterBlendPreInitData*)resource.alignAndIncrement(memReqsHdr);

  result->m_exportForAnalysis = false;
  result->m_numAnimSets = numAnimSets;

  // Attrib data handles
  NMP_VERIFY(numAnimSets > 0);
  NMP::Memory::Format memReqsHandles(sizeof(MR::AttribDataHandle) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_attribDataHandles = (MR::AttribDataHandle*)resource.alignAndIncrement(memReqsHandles);
  for (uint32_t i = 0; i < numAnimSets; ++i)
  {
    result->m_attribDataHandles[i].m_format.set(0, MR_ATTRIB_DATA_ALIGNMENT);
    result->m_attribDataHandles[i].m_attribData = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeScatterBlendPreInitData* NodeScatterBlendPreInitData::createAndInit(uint32_t numAnimSets)
{
  NMP::Memory::Format memReqs = NodeScatterBlendPreInitData::getMemoryRequirements(numAnimSets);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  NodeScatterBlendPreInitData* result = NodeScatterBlendPreInitData::init(memRes, numAnimSets);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendPreInitData::releaseAndDestroy()
{
  for (uint32_t i = 0; i < m_numAnimSets; ++i)
  {
    if (m_attribDataHandles[i].m_attribData)
    {
      NMP::Memory::memFree(m_attribDataHandles[i].m_attribData);
    }
  }
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlendExportInfo
//----------------------------------------------------------------------------------------------------------------------
void ScatterBlendExportInfo::readHeaderInfo(
  const ME::NodeExport* nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor* NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  NMP_VERIFY_MSG(
    animLibraryExport,
    "Unable to locate the animation library export");
  m_numAnimSets = (MR::AnimSetIndex)animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(m_numAnimSets > 0);

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  m_nodeName = nodeDefExport->getName();
  NMP_VERIFY(m_nodeName);

  //------------------------
  // Projection
  m_projectionMode = MR::kScatterBlend2DInvalidProjectionMode;
  m_projectionUseCentre = false;
  uint32_t projectionMode = 0xFFFFFFFF;
  bool status = nodeDefDataBlock->readUInt(projectionMode, "ProjectionMode");
  NMP_VERIFY_MSG(
    status,
    "ProjectionMode attribute is not defined");

  NMP::Vector3 projectionVector(NMP::Vector3::InitZero);
  bool statusVx = nodeDefDataBlock->readFloat(projectionVector.x, "ProjectionVectorX");
  bool statusVy = nodeDefDataBlock->readFloat(projectionVector.y, "ProjectionVectorY");
  NMP_VERIFY_MSG(
    statusVx && statusVy,
    "ProjectionVector attribute is not defined");

  switch (projectionMode)
  {
  case 0: // Closest Approach    
    m_projectionMode = MR::kScatterBlend2DClosestApproach;
    m_projectionVector.setToZero();
    break;
  case 1: // Towards Zero
    m_projectionMode = MR::kScatterBlend2DProjectTowardsTarget;
    m_projectionVector.setToZero();
    break;
  case 2: // Towards Centre
    m_projectionMode = MR::kScatterBlend2DProjectTowardsTarget;
    m_projectionVector.setToZero();
    m_projectionUseCentre = true; // Projection vector needs to be computed
    break;
  case 3: // Towards Target
    m_projectionMode = MR::kScatterBlend2DProjectTowardsTarget;
    m_projectionVector = projectionVector;
    break;
  case 4: // In X
    m_projectionMode = MR::kScatterBlend2DProjectInDirection;
    m_projectionVector.setToZero();
    m_projectionVector.x = 1.0f;
    break;
  case 5: // In Y
    m_projectionMode = MR::kScatterBlend2DProjectInDirection;
    m_projectionVector.setToZero();
    m_projectionVector.y = 1.0f;
    break;
  case 6: // In Direction
    m_projectionMode = MR::kScatterBlend2DProjectInDirection;
    NMP_VERIFY_MSG(
      projectionVector.magnitude() > FLT_MIN,
      "The projection vector normal must not have zero length");
    m_projectionVector = projectionVector;
    m_projectionVector.normalise();
    break;
  case 7: // In Axis For Facet
    m_projectionMode = MR::kScatterBlend2DProjectInAxisForFacet;
    m_projectionVector.setToZero();
    break;
  default:
    NMP_VERIFY_FAIL("Unknown projection mode");
  }

  //------------------------
  // Display scales
  char paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < m_numAnimSets; ++animSetIndex)
  {
    float displayScaleX = 1.0f;
    sprintf_s(paramName, "DisplayScaleX_%d", animSetIndex);
    nodeDefDataBlock->readFloat(displayScaleX, paramName);
    m_displayScalesX[animSetIndex] = displayScaleX;

    float displayScaleY = 1.0f;
    sprintf_s(paramName, "DisplayScaleY_%d", animSetIndex);
    nodeDefDataBlock->readFloat(displayScaleY, paramName);
    m_displayScalesY[animSetIndex] = displayScaleY;
  }

  //------------------------
  // Unit scaling
  float unitScaleX = 1.0f;
  status = nodeDefDataBlock->readFloat(unitScaleX, "WeightXScale");
  NMP_VERIFY_MSG(status, "Unable to read WeightXScale attribute");
  m_unitScaleX = unitScaleX;

  float unitScaleY = 1.0f;
  status = nodeDefDataBlock->readFloat(unitScaleY, "WeightYScale");
  NMP_VERIFY_MSG(status, "Unable to read WeightYScale attribute");
  m_unitScaleY = unitScaleY;
}

//----------------------------------------------------------------------------------------------------------------------
// NodeScatterBlendBuilder
//----------------------------------------------------------------------------------------------------------------------
bool NodeScatterBlendBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeScatterBlendBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_SCATTER_BLEND_2D);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BLEND_FLAGS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 3); // It is not valid to have a node with only one input (this is checked in validate).

  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "WeightX", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "WeightY", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendBuilder::loadScatterBlendVertexSources(
  MR::AnimSetIndex                animSetIndex,
  const ME::NodeExport*           nodeDefExport,
  std::vector<MR::NodeID>&        sourceNodeIDs,
  SB2DAnnotationExportDataPacked& annotationExportData)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  bool status;

  // Number of vertex sources
  uint32_t sourceNodeCount = 0;
  status = nodeDefDataBlock->readUInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY_MSG(
    status,
    "SourceNodeCount attribute is not defined");
  NMP_VERIFY(sourceNodeCount >= 3);

  // Sub-sampling
  uint32_t numSubDivisions = 0;
  status = nodeDefDataBlock->readUInt(numSubDivisions, "NumSubDivisions");
  NMP_VERIFY_MSG(
    status,
    "NumSubDivisions attribute is not defined");
  NMP_VERIFY(numSubDivisions < 3);

  // Initialise the annotation export data for the vertex sources
  annotationExportData.init(
    numSubDivisions,
    0,
    sourceNodeCount,
    0, 0, 0); // No triangles
  sourceNodeIDs.resize(sourceNodeCount);

  //------------------------
  // Read the vertex source motion parameter data
  char paramName[256];
  for (uint32_t index = 0; index < sourceNodeCount; ++index)
  {
    // Get the child node ID
    sprintf_s(paramName, "Source%dNodeID", index);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeIDs[index]);

    // Get the child motion parameters
    float x = 0.0f;
    sprintf_s(paramName, "Source%dNodeIDParameterX_%d", index, animSetIndex);
    nodeDefDataBlock->readFloat(x, paramName);

    float y = 0.0f;
    sprintf_s(paramName, "Source%dNodeIDParameterY_%d", index, animSetIndex);
    nodeDefDataBlock->readFloat(y, paramName);

    // Set the motion parameter sample
    annotationExportData.setSample(index, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeScatterBlendBuilder::loadScatterBlendTriangleAnnotations(
  MR::AnimSetIndex                      animSetIndex,
  const ME::NodeExport*                 nodeDefExport,
  AssetProcessor*                       processor,
  const SB2DAnnotationExportDataPacked& annotationExportData,
  SB2DAnnotation&                       annotation,
  std::vector<MR::NodeID>&              sourceNodeIDs,
  bool                                  loadForAnalysis)
{
  NMP::BasicLogger* errorLogger = processor->getErrorLogger();
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  uint32_t numVertices = annotationExportData.getNumVertices();
  const float* samplesX = annotationExportData.getSamplesX();
  const float* samplesY = annotationExportData.getSamplesY();
  bool status;

  // Get the number of sub-divisions
  uint32_t numSubDivisions = 0;
  status = nodeDefDataBlock->readUInt(numSubDivisions, "NumSubDivisions");
  NMP_VERIFY_MSG(
    status,
    "NumSubDivisions attribute data does not exist");
  NMP_VERIFY(numSubDivisions < 3);

  SB2DTriangleSubSampleData triangleSubSampleData;
  triangleSubSampleData.init(numSubDivisions);
  SB2DTriangleSubSampleData triangleSubSampleDataCCW;
  triangleSubSampleDataCCW.init(numSubDivisions);

  std::string parameterGraphPath = nodeDefExport->getName();
  parameterGraphPath += '|';
  size_t parameterGraphPathLength = 0;

  if(!loadForAnalysis)
  {
    char paramGraphPath[256];
    sprintf_s(paramGraphPath, 255, "AnimSet_%d_ParameterGraph", animSetIndex);
    
    std::string shortParamName; 
    status = nodeDefDataBlock->readString(shortParamName, paramGraphPath);
    NMP_VERIFY_MSG(status, "Unable to load parameter graph path for animation set %d", animSetIndex);
    
    parameterGraphPath += shortParamName;
    parameterGraphPathLength = parameterGraphPath.length();
  }

  uint32_t numAnalysisNodes = nodeDefExport->getNumAnalysisNodes();
  for (uint32_t i = 0; i < numAnalysisNodes; ++i)
  {
    const ME::AnalysisNodeExport* analysisNodeExport = nodeDefExport->getAnalysisNode(i);
    const char* analysisNodeName = analysisNodeExport->getName();
    const char* analysisNodeType = analysisNodeExport->getTypeName();

    if (strcmp(analysisNodeType, "Annotation") == 0)
    {
      // Check for the correct anim set
      const ME::DataBlockExport* dataBlock = analysisNodeExport->getDataBlock();
      uint32_t samplesAnimSetIndex = 0xFFFFFFFF;
      dataBlock->readUInt(samplesAnimSetIndex, "AnalysisAnimSetIndex");
      NMP_VERIFY(samplesAnimSetIndex != 0xFFFFFFFF);

      // load if the annotation is parented to the parameter graph, or if analysing,
      // if the AnalysisAnimSetIndex value is correct.
      //
      // parameter graphs can be shared in the data if no specific animation set data is held
      bool shouldLoad = false;
      if (!loadForAnalysis)
      {
        int cmpResult = strncmp(analysisNodeName, parameterGraphPath.c_str(), parameterGraphPathLength);
        shouldLoad = (cmpResult == 0);
      }
      else
      {
        // load if the analysis anim set is correct
        shouldLoad = (samplesAnimSetIndex == animSetIndex);
      }

      if (shouldLoad)
      {
        // Check if the sub-sample data is dirty
        bool dirty = true;
        status = dataBlock->readBool(dirty, "DirtyFlag");
        NMP_VERIFY_MSG(
          status,
          "DirtyFlag attribute data does not exist");
        NMP_VERIFY_MSG(
          !dirty,
          "Scatter Blend Annotations are marked as dirty, analyse the nodes first in connect before exporting the network.");

        NMP_VERIFY(sourceNodeIDs.size() > 0);

        // Read the sub-sample data
        triangleSubSampleData.readDataBlock(dataBlock, &sourceNodeIDs.front(), (uint32_t) sourceNodeIDs.size());
        const uint32_t* vertexIndicesIn = triangleSubSampleData.getTriangleVertexIDs();

        // Ensure that the vertex ordering has a counter-clockwise geometry
        uint32_t vertexIndicesOut[3];
        bool isCollinear = SB2DTriangle::getVertexOrderingCCW(
          numVertices,
          samplesX,
          samplesY,
          vertexIndicesIn,
          vertexIndicesOut);

        // Check if the vertex ordering is collinear
        if (isCollinear && errorLogger)
        {
          errorLogger->output(
            "Triangle annotation %s has collinear vertices\n",
            analysisNodeName);
        }

        // Permute the sub-sample data for the desired CCW vertex order
        triangleSubSampleDataCCW.permute(vertexIndicesOut, triangleSubSampleData);

        // Add a triangle to the annotation
        annotation.addTriangle(&triangleSubSampleDataCCW);
      }
    }
  }

  // Tidy up
  triangleSubSampleData.release();
  triangleSubSampleDataCCW.release();

  // Validate the annotation geometry. Note that triangles should always
  // have vertices that have a counter-clockwise geometry (This will be
  // an important runtime property).
  NMP_VERIFY(annotation.isValidVertexOrdering());

  // Remove redundant triangles
  annotation.removeRedundantTriangles();

  // Check for a valid annotation
  NMP_VERIFY_MSG(
    annotation.getNumTriangles() > 0,
    "There are no valid triangles in the annotation");

  // Validate that the triangles are properly connected
  NMP_VERIFY(annotation.isValidTriangleConnectivity());

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const ME::AnalysisNodeExport* NodeScatterBlendBuilder::findScatterBlendBSPTree(
  MR::AnimSetIndex                      animSetIndex,
  const ME::NodeExport*                 nodeDefExport)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const char* nodeName = nodeDefExport->getName();
  std::string parameterGraphPath = nodeName;
  parameterGraphPath += '|';

  char paramGraphPath[256];
  sprintf_s(paramGraphPath, 255, "AnimSet_%d_ParameterGraph", animSetIndex);
  std::string shortParamName;
  bool status = nodeDefDataBlock->readString(shortParamName, paramGraphPath);
  NMP_VERIFY_MSG(status, "Unable to load parameter graph path for animation set %d", animSetIndex);

  parameterGraphPath += shortParamName;
  std::string bspPath = parameterGraphPath;
  bspPath += "|ScatterBlend2DBSPTreeAnalyser1";

  uint32_t numAnalysisNodes = nodeDefExport->getNumAnalysisNodes();
  for (uint32_t i = 0; i < numAnalysisNodes; ++i)
  {
    const ME::AnalysisNodeExport* analysisNodeExport = nodeDefExport->getAnalysisNode(i);
    const char* analysisNodeName = analysisNodeExport->getName();
    if (strcmp(analysisNodeName, bspPath.c_str()) == 0)
    {
      return analysisNodeExport;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeScatterBlendBuilder::loadScatterBlendBSPTree(
  const ME::AnalysisNodeExport* bspAnalysisNode,
  const SB2DAnnotationExportDataPacked& annotationExportData,
  SB2DAnnotationBSPTreeExportData&      annotationBSPTreeExportData)
{
  NMP_VERIFY_MSG(
    bspAnalysisNode,
    "Invalid BSP annotation analysis node");

  // Check if the BSP analysis node is dirty
  const ME::DataBlockExport* bspDataBlock = bspAnalysisNode->getDataBlock();
  bool dirty = true;
  bool status = bspDataBlock->readBool(dirty, "DirtyFlag");
  NMP_VERIFY_MSG(
    status,
    "DirtyFlag attribute data does not exist");
  NMP_VERIFY_MSG(
    !dirty,
    "You can't build a scatter blend using dirty BSP annotation data.");

  // Initialise the BSP annotation data
  annotationBSPTreeExportData.init(bspDataBlock);

  // Validate that the loaded BSP data is consistent with the annotation
  NMP_VERIFY_MSG(
    annotationBSPTreeExportData.getNumSubSimplexesPerTriangle() == annotationExportData.getNumSubSimplexesPerTriangle(),
    "The BSP tree is inconsistent with the sub-division level of the annotation data");

  // Vertex locations indexed by the hyper planes
  uint32_t numVertices = annotationExportData.getNumVertices();
  uint32_t numBranchNodes = annotationBSPTreeExportData.getNumBranchNodes();
  const uint32_t* hyperPlaneSampleIndicesA = annotationBSPTreeExportData.getBranchNodeHyperPlaneSampleIndicesA();
  const uint32_t* hyperPlaneSampleIndicesB = annotationBSPTreeExportData.getBranchNodeHyperPlaneSampleIndicesB();
  for (uint32_t i = 0; i < numBranchNodes; ++i)
  {
    NMP_VERIFY_MSG(
      hyperPlaneSampleIndicesA[i] < numVertices && hyperPlaneSampleIndicesB[i] < numVertices,
      "Invalid BSP annotation hyper-plane vertex entries");
  }

  // Leaf node triangle annotation indices
  uint32_t numTriangles = annotationExportData.getNumTriangles();
  uint32_t numLeafNodeEntries = annotationBSPTreeExportData.getNumLeafNodeEntries();
  const uint32_t* leafNodeTriangleIndices = annotationBSPTreeExportData.getLeafNodeTriangleIndices();
  for (uint32_t i = 0; i < numLeafNodeEntries; ++i)
  {
    NMP_VERIFY_MSG(
      leafNodeTriangleIndices[i] < numTriangles,
      "Invalid BSP triangle annotation entries");
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendBuilder::computeRangeOfAnnotationSubSamples(
  const ME::NodeExport* nodeDefExport,
  AssetProcessor* processor,
  const SB2DAnnotationExportDataPacked& annotationExportData,
  SB2DAnnotationSubSampleRangeData& annotationRangeData)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //------------------------
  // Get the analysis property builders
  std::string nodeBasePath = nodeDefExport->getName();
  nodeBasePath += '|';

  bool status;
  std::string analysisProperty0ShortPath, analysisProperty0Path;
  status = nodeDefDataBlock->readString(analysisProperty0ShortPath, "AnalysisProperty0");
  NMP_VERIFY_MSG(
    status,
    "The AnalysisProperty0 attribute does not exist");

  analysisProperty0Path = nodeBasePath + analysisProperty0ShortPath; 
  const ME::AnalysisNodeExport* analysisProperty0Export = AnalysisProcessor::findAnalysisNodeExport(
    nodeDefExport,
    analysisProperty0Path.c_str());
  NMP_VERIFY_MSG(
    analysisProperty0Export,
    "Could not find analysis node %s in the node def export",
    analysisProperty0Path.c_str());

  AnalysisPropertyBuilder* apBuilder0 = processor->getAnalysisPropertyBuilderForType(
    analysisProperty0Export->getTypeName());
  NMP_VERIFY_MSG(
    apBuilder0,
    "Invalid analysis property type %s",
    analysisProperty0Export->getTypeName());

  //------------------------
  std::string analysisProperty1ShortPath, analysisProperty1Path;
  
  status = nodeDefDataBlock->readString(analysisProperty1ShortPath, "AnalysisProperty1");
  NMP_VERIFY_MSG(
    status,
    "The AnalysisProperty1 attribute does not exist");
  analysisProperty1Path = nodeBasePath + analysisProperty1ShortPath;

  const ME::AnalysisNodeExport* analysisProperty1Export = AnalysisProcessor::findAnalysisNodeExport(
    nodeDefExport,
    analysisProperty1Path.c_str());
  NMP_VERIFY_MSG(
    analysisProperty1Export,
    "Could not find analysis node %s in the node def export",
    analysisProperty1Path.c_str());

  AnalysisPropertyBuilder* apBuilder1 = processor->getAnalysisPropertyBuilderForType(
    analysisProperty1Export->getTypeName());
  NMP_VERIFY_MSG(
    apBuilder1,
    "Invalid analysis property type %s",
    analysisProperty1Export->getTypeName());

  //------------------------
  // Compute the range distribution of the motion parameter samples
  uint32_t numSamples = annotationExportData.getNumSamples();
  const float* samplesX = annotationExportData.getSamplesX();
  const float* samplesY = annotationExportData.getSamplesY();

  apBuilder0->calculateRangeDistribution(
    processor,
    numSamples,
    samplesX,
    annotationRangeData.m_samplesRangeMin[0],
    annotationRangeData.m_samplesRangeMax[0],
    annotationRangeData.m_samplesCentre[0]);

  apBuilder0->calculateRangeDistribution(
    processor,
    numSamples,
    samplesY,
    annotationRangeData.m_samplesRangeMin[1],
    annotationRangeData.m_samplesRangeMax[1],
    annotationRangeData.m_samplesCentre[1]);
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle NodeScatterBlendBuilder::createAndInitAttribDataScatterBlend2DDef(
  MR::AnimSetIndex animSetIndex,
  const ScatterBlendExportInfo& scatterBlendExportInfo,
  const SB2DAnnotationExportDataPacked& annotationExportData,
  const SB2DAnnotationBSPTreeExportData& annotationBSPTreeExportData,
  const SB2DAnnotationSubSampleRangeData& annotationRangeData)
{
  uint32_t numTriangleSubDivisions = annotationExportData.getNumTriangleSubDivisions();
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(numTriangleSubDivisions);

  //---------------------------
  // Create the memory for the def data
  uint32_t numSubSamplesPerTriangle = annotationExportData.getNumSubSamplesPerTriangle();
  uint32_t numVertices = annotationExportData.getNumVertices();
  uint32_t numTriangles = annotationExportData.getNumTriangles();
  uint32_t numSubSamples = annotationExportData.getNumSubSamples();
  uint32_t numExteriorEdgeFacets = annotationExportData.getNumExteriorEdgeFacets();

  uint32_t bspNumBranchNodes = annotationBSPTreeExportData.getNumBranchNodes();
  uint32_t bspNumLeafNodes = annotationBSPTreeExportData.getNumLeafNodes();
  uint32_t bspNumLeafNodeEntries = annotationBSPTreeExportData.getNumLeafNodeEntries();

  NMP::Memory::Format memReqs = MR::AttribDataScatterBlend2DDef::getMemoryRequirements(
    numTriangleSubDivisions,
    numSubSamplesPerTriangle,
    numVertices,
    numTriangles,
    numSubSamples,
    numExteriorEdgeFacets,
    bspNumBranchNodes,
    bspNumLeafNodes,
    bspNumLeafNodeEntries);

  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(memReqs);
  NMP_ASSERT(resource.ptr);
  ZeroMemory(resource.ptr, memReqs.size);

  MR::AttribDataScatterBlend2DDef* attribDef = MR::AttribDataScatterBlend2DDef::init(
    resource,
    numTriangleSubDivisions,
    numSubSamplesPerTriangle,
    numVertices,
    numTriangles,
    numSubSamples,
    numExteriorEdgeFacets,
    bspNumBranchNodes,
    bspNumLeafNodes,
    bspNumLeafNodeEntries);
  NMP_VERIFY_MSG(
    resource.format.size == 0,
    "Not all the allocated memory was used in AttribDataScatterBlend2DDef");

  MR::AttribDataHandle attribDataHandle;
  attribDataHandle.m_format = memReqs;
  attribDataHandle.m_attribData = attribDef;

  //---------------------------
  // Projection
  attribDef->m_projectionMode = scatterBlendExportInfo.m_projectionMode;
  if (scatterBlendExportInfo.m_projectionUseCentre)
  {
    attribDef->m_projectionVector[0] = annotationRangeData.m_samplesCentre[0];
    attribDef->m_projectionVector[1] = annotationRangeData.m_samplesCentre[1];
  }
  else
  {
    // Set the projection vector from the export info
    attribDef->m_projectionVector[0] = scatterBlendExportInfo.m_projectionVector.x;
    attribDef->m_projectionVector[1] = scatterBlendExportInfo.m_projectionVector.y;
  }

  //---------------------------
  // Display scale
  attribDef->m_displayScale[0] = scatterBlendExportInfo.m_displayScalesX[animSetIndex];
  attribDef->m_displayScale[1] = scatterBlendExportInfo.m_displayScalesY[animSetIndex];

  //---------------------------
  // Unit Scaling
  attribDef->m_unitScale[0] = scatterBlendExportInfo.m_unitScaleX;
  attribDef->m_unitScale[1] = scatterBlendExportInfo.m_unitScaleY;

  //---------------------------
  // Motion parameter sample data
  uint32_t numSamples = annotationExportData.getNumSamples();
  if (numSamples > 0)
  {
    float qMin[3];
    float qMax[3];
    float recipStepSize[3];

    // Compute the quantisation ranges of the sample data. Note that this
    // may differ from the annotation range data, since the annotation
    // range data may encode the wrapping range for certain parameterisations.
    float x, y;
    annotationExportData.getSample(0, x, y);
    qMin[0] = qMax[0] = x;
    qMin[1] = qMax[1] = y;

    for (uint32_t i = 1; i < numSamples; ++i)
    {
      annotationExportData.getSample(i, x, y);
      qMin[0] = NMP::minimum(qMin[0], x);
      qMin[1] = NMP::minimum(qMin[1], y);
      qMax[0] = NMP::maximum(qMax[0], x);
      qMax[1] = NMP::maximum(qMax[1], y);
    }

    // Compute the quantisation step size for 16-bit samples
    attribDef->m_qScale[0] = NMP::UniformQuantisation::stepSize(qMin[0], qMax[0], 16);
    attribDef->m_qScale[1] = NMP::UniformQuantisation::stepSize(qMin[1], qMax[1], 16);
    attribDef->m_qOffset[0] = qMin[0];
    attribDef->m_qOffset[1] = qMin[1];
    recipStepSize[0] = NMP::UniformQuantisation::recipStepSize(qMin[0], qMax[0], 16);
    recipStepSize[1] = NMP::UniformQuantisation::recipStepSize(qMin[1], qMax[1], 16);

    // Quantise the sample data into 16-bits
    uint32_t qVal[2];
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      annotationExportData.getSample(i, x, y);
      qVal[0] = NMP::UniformQuantisation::quantise(qMin[0], qMax[0], recipStepSize[0], x);
      qVal[1] = NMP::UniformQuantisation::quantise(qMin[1], qMax[1], recipStepSize[1], y);
      attribDef->m_samples[0][i] = (uint16_t)qVal[0];
      attribDef->m_samples[1][i] = (uint16_t)qVal[1];
    }
  }

  //---------------------------
  // Triangle sample indices
  uint32_t numTriangleSampleIndices = annotationExportData.getNumTriangleSampleIndices();
  const uint32_t* triangleSampleIndices = annotationExportData.getTriangleSampleIndices();
  for (uint32_t i = 0; i < numTriangleSampleIndices; ++i)
  {
    attribDef->m_triangleSampleIndices[i] = (uint16_t)triangleSampleIndices[i];
  }

  //---------------------------
  // Exterior edge facets
  const uint32_t* exteriorTriangleIndices = annotationExportData.getExteriorTriangleIndices();
  const uint32_t* exteriorEdgeFacetIndices = annotationExportData.getExteriorEdgeFacetIndices();
  for (uint32_t i = 0; i < numExteriorEdgeFacets; ++i)
  {
    NMP_VERIFY(exteriorTriangleIndices[i] < numTriangles);
    NMP_VERIFY(exteriorEdgeFacetIndices[i] < 3);
    attribDef->m_exteriorTriangleIndices[i] = (uint16_t)exteriorTriangleIndices[i];
    attribDef->m_exteriorEdgeFacetIndices[i] = (uint8_t)exteriorEdgeFacetIndices[i];
  }

  // Pad the remaining exterior triangle facets to a block of four
  for (uint32_t i = numExteriorEdgeFacets; i & 0x03; ++i)
  {
    attribDef->m_exteriorTriangleIndices[i] = attribDef->m_exteriorTriangleIndices[i - 1];
    attribDef->m_exteriorEdgeFacetIndices[i] = attribDef->m_exteriorEdgeFacetIndices[i - 1];
  }

  //---------------------------
  // BSP tree
  if (attribDef->hasBSPTree())
  {
    NMP_VERIFY(annotationBSPTreeExportData.getNumSubSimplexesPerTriangle() == subDivisionDataBlock.m_numSubSimplexesPerTriangle);

    // Information
    uint32_t bspMaxNumSubTrianglesInPartition = annotationBSPTreeExportData.computeMaxNumSubTrianglesInPartition();
    attribDef->m_bspMaxNumSubTrianglesInPartition = (uint16_t)bspMaxNumSubTrianglesInPartition;

    const uint32_t* bspBranchNodeLeft = annotationBSPTreeExportData.getBranchNodeLeft();
    const uint32_t* bspBranchNodeRight = annotationBSPTreeExportData.getBranchNodeRight();
    const uint32_t* bspBranchNodeHyperPlaneSampleIndicesA = annotationBSPTreeExportData.getBranchNodeHyperPlaneSampleIndicesA();
    const uint32_t* bspBranchNodeHyperPlaneSampleIndicesB = annotationBSPTreeExportData.getBranchNodeHyperPlaneSampleIndicesB();
    const uint32_t* bspLeafNodeEntryOffsets = annotationBSPTreeExportData.getLeafNodeEntryOffsets();
    const uint32_t* bspLeafNodeTriangleIndices = annotationBSPTreeExportData.getLeafNodeTriangleIndices();

    for (uint32_t i = 0; i < bspNumBranchNodes; ++i)
    {
      // Left
      attribDef->m_bspBranchNodeLeft[i] = (uint16_t)bspBranchNodeLeft[i];
      // Right
      attribDef->m_bspBranchNodeRight[i] = (uint16_t)bspBranchNodeRight[i];
      // Hyper-planes
      attribDef->m_bspBranchNodeHyperPlaneSampleIndices[0][i] = (uint16_t)bspBranchNodeHyperPlaneSampleIndicesA[i];
      attribDef->m_bspBranchNodeHyperPlaneSampleIndices[1][i] = (uint16_t)bspBranchNodeHyperPlaneSampleIndicesB[i];
    }

    // Leaf node entry offsets (+1 element required for forward difference)
    for (uint32_t i = 0; i <= bspNumLeafNodes; ++i)
    {
      attribDef->m_bspLeafNodeEntryOffsets[i] = (uint16_t)bspLeafNodeEntryOffsets[i];
    }

    // Leaf node triangle entries
    if (numTriangles <= 256)
    {
      // 8-bits
      uint8_t* leafNodeTriangleIndices = (uint8_t*)attribDef->m_bspLeafNodeTriangleIndices;
      for (uint32_t i = 0; i < bspNumLeafNodeEntries; ++i)
      {
        leafNodeTriangleIndices[i] = (uint8_t)bspLeafNodeTriangleIndices[i];
      }
    }
    else
    {
      // 16-bits
      uint16_t* leafNodeTriangleIndices = (uint16_t*)attribDef->m_bspLeafNodeTriangleIndices;
      for (uint32_t i = 0; i < bspNumLeafNodeEntries; ++i)
      {
        leafNodeTriangleIndices[i] = (uint16_t)bspLeafNodeTriangleIndices[i];
      }
    }

    // Leaf node sub-triangle bit-masks
    if (numTriangleSubDivisions == 1)
    {
      // 8-bits (4 sub-triangles)
      uint8_t* subTriangleFlagsDst = (uint8_t*)attribDef->m_bspLeafNodeSubTriangleFlags;
      for (uint32_t entryIndex = 0; entryIndex < bspNumLeafNodeEntries; ++entryIndex)
      {
        const bool* subTriangleFlagsSrc = annotationBSPTreeExportData.getLeafNodeSubTriangleFlags(entryIndex);
        uint32_t flags = 0;
        for (uint32_t i = 0; i < 4; ++i)
        {
          if (subTriangleFlagsSrc[i])
          {
            flags |= (1 << i);
          }
        }
        subTriangleFlagsDst[entryIndex] = (uint8_t)flags;
      }
    }
    else if (numTriangleSubDivisions == 2)
    {
      // 16-bits (16 sub-triangles)
      uint16_t* subTriangleFlagsDst = (uint16_t*)attribDef->m_bspLeafNodeSubTriangleFlags;
      for (uint32_t entryIndex = 0; entryIndex < bspNumLeafNodeEntries; ++entryIndex)
      {
        const bool* subTriangleFlagsSrc = annotationBSPTreeExportData.getLeafNodeSubTriangleFlags(entryIndex);
        uint32_t flags = 0;
        for (uint32_t i = 0; i < 16; ++i)
        {
          if (subTriangleFlagsSrc[i])
          {
            flags |= (1 << i);
          }
        }
        subTriangleFlagsDst[entryIndex] = (uint16_t)flags;
      }
    }
  }
  else
  {
    // Set the max number of triangles to search
    attribDef->m_bspMaxNumSubTrianglesInPartition = subDivisionDataBlock.m_numSubSimplexesPerTriangle * attribDef->m_numTriangles;
  }

  //-------------------------------
  // Memory logging
#ifdef DEBUG_SCATTER_BLEND_MEMORY
  FILE* fp = fopen("C:\\output.txt", "w");
  if (fp)
  {
    fprintf(fp, "Node name = %s\n", scatterBlendExportInfo.m_nodeName);

    fprintf(fp, "Num sub-divisions = %d\n", attribDef->m_numTriangleSubDivisions);

    fprintf(fp, "Num vertex sources = %d\n", attribDef->m_numVertices);

    fprintf(fp, "AttribDef memory requirements = %d bytes (%fK)\n",
      (uint32_t)attribDataHandle.m_format.size,
      (float)attribDataHandle.m_format.size / 1024.0f);

    fclose(fp);
  }
#endif

  return attribDataHandle;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendBuilder::buildAttribDataForAnalysis(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Create the pre-init data for analysis
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlendPreInitData* preInitData = NodeScatterBlendPreInitData::createAndInit(1);
  netDefCompilationInfo->attachUserData(nodeID, preInitData);
  preInitData->m_exportForAnalysis = true;

  // Create the memory for the attrib data
  NMP::Memory::Format memReqsAttribDef = MR::AttribDataScatterBlendAnalysisDef::getMemoryRequirements(2, 3);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(memReqsAttribDef);
  NMP_ASSERT(resource.ptr);
  MR::AttribDataScatterBlendAnalysisDef* attribDef = MR::AttribDataScatterBlendAnalysisDef::init(
    resource,
    2, 3,
    MR::IS_DEF_ATTRIB_DATA);

  // Set the user data
  preInitData->m_attribDataHandles[0].m_format = memReqsAttribDef;
  preInitData->m_attribDataHandles[0].m_attribData = attribDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendBuilder::buildAttribDataForPreview(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  // Read the scatter blend options from the export data
  ScatterBlendExportInfo scatterBlendExportInfo;
  scatterBlendExportInfo.readHeaderInfo(
    nodeDefExport,
    netDefExport,
    processor);

  // Allocate and initialise the pre-init data for preview
  bool status;
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlendPreInitData* preInitData = NodeScatterBlendPreInitData::createAndInit(scatterBlendExportInfo.m_numAnimSets);
  netDefCompilationInfo->attachUserData(nodeID, preInitData);
  preInitData->m_exportForAnalysis = false;

  //---------------------------
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < scatterBlendExportInfo.m_numAnimSets; ++animSetIndex)
  {
    // Find the BSP tree analysis node. If this does not exist then the animation set data is
    // inherited from the base animation set data
    const ME::AnalysisNodeExport* bspAnalysisNode = findScatterBlendBSPTree(animSetIndex, nodeDefExport);
    if (bspAnalysisNode)
    {
      // Read the vertex samples from the export data
      std::vector<MR::NodeID> sourceNodeIDs;
      SB2DAnnotationExportDataPacked annotationVertexData;
      loadScatterBlendVertexSources(
        animSetIndex,
        nodeDefExport,
        sourceNodeIDs,
        annotationVertexData);

      // Initialise the annotation graph with the vertex samples
      SB2DAnnotation annotation;
      annotation.setAnnotation(annotationVertexData);

      // Read the triangle annotations from the export data
      status = loadScatterBlendTriangleAnnotations(
        animSetIndex,
        nodeDefExport,
        processor,
        annotationVertexData,
        annotation,
        sourceNodeIDs,
        false);
      NMP_VERIFY_MSG(
        status,
        "In NodeScatterBlendBuilder: Failed to load the triangle annotations.");

      // Convert the annotation graph into the packed export format
      SB2DAnnotationExportDataPacked annotationExportData;
      annotation.getAnnotation(annotationExportData);

      // Compute the ranges of the sub-sample data
      SB2DAnnotationSubSampleRangeData annotationRangeData;
      computeRangeOfAnnotationSubSamples(
        nodeDefExport,
        processor,
        annotationExportData,
        annotationRangeData);

      // Read the BSP tree data from the export data
      SB2DAnnotationBSPTreeExportData annotationBSPTreeExportData;
      status = loadScatterBlendBSPTree(
        bspAnalysisNode,
        annotationExportData,
        annotationBSPTreeExportData);
      NMP_VERIFY_MSG(
        status,
        "In NodeScatterBlendBuilder: Failed to load the BSP Tree data.");

      // Create and initialise the pre-init data
      preInitData->m_attribDataHandles[animSetIndex] = createAndInitAttribDataScatterBlend2DDef(
        animSetIndex,
        scatterBlendExportInfo,
        annotationExportData,
        annotationBSPTreeExportData,
        annotationRangeData);
      NMP_VERIFY(preInitData->m_attribDataHandles[animSetIndex].m_attribData);
    }
    else
    {      
      MR::AttribData* defAttribData0 = (MR::AttribData*)preInitData->m_attribDataHandles[0].m_attribData;
      NMP_VERIFY_MSG(
        defAttribData0,
        "The BSP tree must exist for the first animation set");

      // Make a copy of the attribute data.
      // TODO: Support resource sharing in the nodeDef attribute data
      MR::AttribDataHandle handle;
      handle.m_format = preInitData->m_attribDataHandles[0].m_format;
      NMP_VERIFY(handle.m_format.size > 0)
      NMP::Memory::Resource memResAttribData = NMPMemoryAllocateFromFormat(handle.m_format);
      handle.m_attribData = (MR::AttribData*)memResAttribData.ptr;
      NMP::Memory::memcpy(handle.m_attribData, defAttribData0, handle.m_format.size);
      MR::AttribDataScatterBlend2DDef::relocate(handle.m_attribData, handle.m_attribData);

      // Set the pre-init data
      preInitData->m_attribDataHandles[animSetIndex] = handle;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlendBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_2D, "Expecting node type SCATTER_BLEND_2D");

  declareDataPin(netDefCompilationInfo, nodeDefExport, "WeightX", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "WeightY", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  bool exportForAnalysis = false;
  nodeDefDataBlock->readBool(exportForAnalysis, "ExportForAnalysis");
  if (exportForAnalysis)
  {
    buildAttribDataForAnalysis(netDefCompilationInfo, nodeDefExport, netDefExport, processor);
  }
  else
  {
    buildAttribDataForPreview(netDefCompilationInfo, nodeDefExport, netDefExport, processor);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeScatterBlendBuilder::getNodeDefMemoryRequirementsForAttribData(
  NetworkDefCompilationInfo* netDefCompilationInfo,
  NodeScatterBlendPreInitData* preInitData,
  const ME::NodeExport* nodeDefExport)
{
  NMP_VERIFY(preInitData);
  MR::AnimSetIndex numAnimSets = (MR::AnimSetIndex)preInitData->m_numAnimSets;
  NMP_VERIFY(numAnimSets > 0);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 3);

  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    sourceNodeCount,        // numChildren
    2,                      // numInputCPConnections
    numAnimSets,            // numAnimSetEntries
    nodeDefExport);

  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  result += getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);

  // Blending flags.
  result += MR::AttribDataBlendFlags::getMemoryRequirements();

  // Reserve space for the attrib data definition
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    const NMP::Memory::Format& memReqsAttribDef = preInitData->m_attribDataHandles[animSetIndex].m_format;
    NMP_VERIFY(memReqsAttribDef.size > 0);
    result += memReqsAttribDef;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeScatterBlendBuilder::initForAnalysis(
  NMP::Memory::Resource&        memRes,
  NetworkDefCompilationInfo*    netDefCompilationInfo,
  NodeScatterBlendPreInitData*  preInitData,
  const ME::NodeExport*         nodeDefExport,
  AssetProcessor*               processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_2D, "Expecting node type SCATTER_BLEND_2D");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();  
  NMP_VERIFY(preInitData);

  // Only a single anim set is used for analysis
  MR::AnimSetIndex numAnimSets = (MR::AnimSetIndex)preInitData->m_numAnimSets;
  NMP_VERIFY(numAnimSets == 1);

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 3);

  //---------------------------
  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    sourceNodeCount,  // numChildren
    3,                // maxNumActiveChildNodes
    2,                // numInputCPConnections
    0,                // numOutputCPPins
    numAnimSets,      // numAnimSetEntries
    nodeDefExport);

  // Flag as a blend node that supports blend optimisation
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE);

  //---------------------------
  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }



  //---------------------------
  // Initialise the attrib datas.

  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);

  // Blend mode attributes.
  bool slerpDeltaTrajectoryTranslation = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajectoryTranslation, "SphericallyInterpolateTrajectoryPosition");

  // Blending flags.
  initBlendNodeBlendFlags(
    memRes,
    nodeDefExport,
    nodeDef);

  //---------------------------
  // Animation set dependent attribute data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    const NMP::Memory::Format& memReqsAttribDef = preInitData->m_attribDataHandles[animSetIndex].m_format;
    NMP_VERIFY(memReqsAttribDef.size > 0);
    const MR::AttribDataScatterBlendAnalysisDef* attribDef = (const MR::AttribDataScatterBlendAnalysisDef*)preInitData->m_attribDataHandles[animSetIndex].m_attribData;
    NMP_VERIFY(attribDef);

    MR::AttribDataScatterBlendAnalysisDef* defAttribData = (MR::AttribDataScatterBlendAnalysisDef*)memRes.alignAndIncrement(memReqsAttribDef);
    NMP::Memory::memcpy(defAttribData, attribDef, memReqsAttribDef.size);
    MR::AttribDataScatterBlendAnalysisDef::relocate(defAttribData, defAttribData);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, // semantic,
      animSetIndex,                               // animSetIndex,
      defAttribData,                              // attribData,
      memReqsAttribDef);                          // attribMemReqs
  }

  //---------------------------
  // Initialise the task function tables for offline evaluation
  NMP::BasicLogger* logger = processor->getMessageLogger();
  initTaskQueuingFnsBase(
    nodeDef,
    netDefCompilationInfo,
    timeStretchMode,
    slerpDeltaTrajectoryTranslation,
    logger);

  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeScatterBlend2DOfflineUpdateConnections), logger);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeScatterBlendBuilder::initForPreview(
  NMP::Memory::Resource&        memRes,
  NetworkDefCompilationInfo*    netDefCompilationInfo,
  NodeScatterBlendPreInitData*  preInitData,
  const ME::NodeExport*         nodeDefExport,
  AssetProcessor*               processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_2D, "Expecting node type SCATTER_BLEND_2D");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  NMP_VERIFY(preInitData);
  MR::AnimSetIndex numAnimSets = (MR::AnimSetIndex)preInitData->m_numAnimSets;
  NMP_VERIFY(numAnimSets > 0);

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 3);

  //---------------------------
  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    sourceNodeCount,          // numChildren
    3,                        // maxNumActiveChildNodes
    2,                        // numInputCPConnections
    0,                        // numOutputCPPins
    numAnimSets,              // numAnimSetEntries
    nodeDefExport);

  // Flag as a blend node that supports blend optimisation
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE);

  //---------------------------
  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }

  //---------------------------
  // Initialise the attrib datas.

  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);

  // Blend mode attributes.
  bool slerpDeltaTrajectoryTranslation = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajectoryTranslation, "SphericallyInterpolateTrajectoryPosition");

  // Blending flags.
  initBlendNodeBlendFlags(
    memRes,
    nodeDefExport,
    nodeDef);

  //---------------------------
  // Animation set dependent attribute data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    const NMP::Memory::Format& memReqsAttribDef = preInitData->m_attribDataHandles[animSetIndex].m_format;
    NMP_VERIFY(memReqsAttribDef.size > 0);
    const MR::AttribDataScatterBlend2DDef* attribDef = (const MR::AttribDataScatterBlend2DDef*)preInitData->m_attribDataHandles[animSetIndex].m_attribData;
    NMP_VERIFY(attribDef);

    MR::AttribDataScatterBlend2DDef* defAttribData = (MR::AttribDataScatterBlend2DDef*)memRes.alignAndIncrement(memReqsAttribDef);
    NMP::Memory::memcpy(defAttribData, attribDef, memReqsAttribDef.size);
    MR::AttribDataScatterBlend2DDef::relocate(defAttribData, defAttribData);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, // semantic,
      animSetIndex,                               // animSetIndex,
      defAttribData,                              // attribData,
      memReqsAttribDef);                          // attribMemReqs
  }

  //---------------------------
  // Initialise the task function tables
  NMP::BasicLogger* logger = processor->getMessageLogger();
  initTaskQueuingFnsBase(
    nodeDef,
    netDefCompilationInfo,
    timeStretchMode,
    slerpDeltaTrajectoryTranslation,
    logger);

  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeScatterBlend2DUpdateConnections), logger);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeScatterBlendBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Get the precomputed user data
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlendPreInitData* preInitData = (NodeScatterBlendPreInitData*)netDefCompilationInfo->getUserData(nodeID);
  NMP_VERIFY(preInitData);
  return getNodeDefMemoryRequirementsForAttribData(
    netDefCompilationInfo,
    preInitData,
    nodeDefExport);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeScatterBlendBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  // Get the precomputed user data
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlendPreInitData* preInitData = (NodeScatterBlendPreInitData*)netDefCompilationInfo->detachUserData(nodeID);
  NMP_VERIFY(preInitData);

  MR::NodeDef* nodeDef = NULL;
  if (preInitData->m_exportForAnalysis)
  {
    nodeDef = initForAnalysis(
      memRes,
      netDefCompilationInfo,
      preInitData,
      nodeDefExport,
      processor);
  }
  else
  {
    nodeDef = initForPreview(
      memRes,
      netDefCompilationInfo,
      preInitData,
      nodeDefExport,
      processor);
  }

  // Tidy up the user data
  preInitData->releaseAndDestroy();

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
// Initialises the task queuing functions excluding the update connections function.
void NodeScatterBlendBuilder::initTaskQueuingFnsBase(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NodeTimeStretchModes        timeStretchMode,
  bool                        slerpDeltaTrajectoryTranslation,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_VERIFY(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  // Output mask
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);

  //---------------------------
  // MATCH EVENTS
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    // Time
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeScatterBlend2DSyncEventsQueueSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    // Events
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeScatterBlend2DSyncEventsQueueBlend3DurationEventTrackSets), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeScatterBlend2DSyncEventsQueueSampledEventsBuffers), logger);

    // We deal with sync event tracks so don't let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, NULL, NULL, logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNSyncEventsFindGeneratingNodeForSemantic), logger);
  }
  //---------------------------
  // NOT MATCH EVENTS
  else if (timeStretchMode == kNodeTimeStretchNone)
  {
    // Time
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queuePassThroughChild0), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(queuePassThroughChild0), logger);

    // Events
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queuePassThroughChild0), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeScatterBlend2DQueueSampledEventsBuffers), logger);

    // We don't deal with sync event tracks so let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, FN_NAME(queuePassThroughChild0), logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNFindGeneratingNodeForSemantic), logger);
  }

  //---------------------------
  // Transforms
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeScatterBlend2DQueueBlendTransformBuffsInterpAttInterpPos), logger);

  //---------------------------
  // Trajectory
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
  }

  //---------------------------
  // Trajectory delta and transforms
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj), logger);
  }

  //---------------------------
  // Handle Data Pins
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }
  //---------------------------
  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);
}


} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
