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
#include "NodeScatterBlend1DBuilder.h"
#include "NetworkDefBuilder.h"
#include "NodeScatterBlend1DAnnotationExport.h"
#include "Analysis/ScatterBlend1DTask.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/Nodes/mrNodeScatterBlend1D.h"
#include "morpheme/Nodes/mrScatterBlend1DUtils.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NodeScatterBlend1DPreInitData
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeScatterBlend1DPreInitData::getMemoryRequirements(uint32_t numAnimSets)
{
  // Header
  NMP::Memory::Format result(sizeof(NodeScatterBlend1DPreInitData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Attrib data handles
  NMP_VERIFY(numAnimSets > 0);
  NMP::Memory::Format memReqsHandles(sizeof(MR::AttribDataHandle) * numAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsHandles;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeScatterBlend1DPreInitData* NodeScatterBlend1DPreInitData::init(
  NMP::Memory::Resource& resource,
  uint32_t numAnimSets)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NodeScatterBlend1DPreInitData), NMP_NATURAL_TYPE_ALIGNMENT);
  NodeScatterBlend1DPreInitData* result = (NodeScatterBlend1DPreInitData*)resource.alignAndIncrement(memReqsHdr);  

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
NodeScatterBlend1DPreInitData* NodeScatterBlend1DPreInitData::createAndInit(uint32_t numAnimSets)
{
  NMP::Memory::Format memReqs = NodeScatterBlend1DPreInitData::getMemoryRequirements(numAnimSets);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  NodeScatterBlend1DPreInitData* result = NodeScatterBlend1DPreInitData::init(memRes, numAnimSets);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlend1DPreInitData::releaseAndDestroy()
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
// ScatterBlend1DExportInfo
//----------------------------------------------------------------------------------------------------------------------
void ScatterBlend1DExportInfo::readHeaderInfo(
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

  // Unit scaling
  float unitScale = 1.0f;
  bool status = nodeDefDataBlock->readFloat(unitScale, "WeightScale");
  NMP_VERIFY_MSG(status, "Unable to read WeightScale attribute");
  m_unitScale = unitScale;
}

//----------------------------------------------------------------------------------------------------------------------
// NodeScatterBlend1DBuilder
//----------------------------------------------------------------------------------------------------------------------
bool NodeScatterBlend1DBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeScatterBlend1DBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_SCATTER_BLEND_1D);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BLEND_FLAGS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlend1DBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2);

  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    NMP_VERIFY(sourceNodeID != MR::INVALID_NODE_ID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Control parameter
  readDataPinChildNodeID(nodeDefDataBlock, "Weight", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle NodeScatterBlend1DBuilder::createAndInitAttribDataScatterBlend1DDef(
  MR::AnimSetIndex NMP_UNUSED(animSetIndex),
  const ScatterBlend1DExportInfo& scatterBlendExportInfo,
  const SB1DAnnotationExportDataPacked& annotationExportData,
  const SB1DAnnotationSubSampleRangeData& NMP_UNUSED(annotationRangeData))
{
  //---------------------------
  // Create the memory for the def data
  uint32_t numSubDivisions = annotationExportData.getNumSubDivisions();
  uint32_t numSubSamplesPerLine = annotationExportData.getNumSubSamplesPerLine();
  uint32_t numVertices = annotationExportData.getNumVertices();
  uint32_t numLines = annotationExportData.getNumLines();
  uint32_t numSubSamples = annotationExportData.getNumSubSamples();

  NMP::Memory::Format memReqs = MR::AttribDataScatterBlend1DDef::getMemoryRequirements(
    numSubDivisions,
    numSubSamplesPerLine,
    numVertices,
    numLines,
    numSubSamples);

  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(memReqs);
  NMP_ASSERT(resource.ptr);
  ZeroMemory(resource.ptr, memReqs.size);

  MR::AttribDataScatterBlend1DDef* attribDef = MR::AttribDataScatterBlend1DDef::init(
    resource,
    numSubDivisions,
    numSubSamplesPerLine,
    numVertices,
    numLines,
    numSubSamples);
  NMP_VERIFY_MSG(
    resource.format.size == 0,
    "Not all the allocated memory was used in AttribDataScatterBlend1DDef: %d bytes remain",
    resource.format.size);

  MR::AttribDataHandle attribDataHandle;
  attribDataHandle.m_format = memReqs;
  attribDataHandle.m_attribData = attribDef;

  //---------------------------
  // Unit Scaling
  attribDef->m_unitScale = scatterBlendExportInfo.m_unitScale;

  //---------------------------
  // Vertex source indices
  const uint32_t* vertexSourceIndices = annotationExportData.getVertexSourceIndices();
  for (uint32_t i = 0; i < numVertices; ++i)
  {
    attribDef->m_vertexSourceIndices[i] = (uint16_t)vertexSourceIndices[i];
  }

  //---------------------------
  // Motion parameter sample data
  uint32_t numSamples = annotationExportData.getNumSamples();
  if (numSamples > 0)
  {
    const float* samples = annotationExportData.getSamples();
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      attribDef->m_samples[i] = samples[i];
    }
  }

  //---------------------------
  // Line sample indices
  const uint32_t* lineSampleIndices = annotationExportData.getLineSampleIndices();
  for (uint32_t lineIndex = 0; lineIndex < numLines; ++lineIndex)
  {
    uint32_t offset = lineIndex * numSubSamplesPerLine;
    uint16_t* lineSampleIndicesDst = &attribDef->m_lineSampleIndices[offset];
    const uint32_t* lineSampleIndicesSrc = &lineSampleIndices[offset];
    for (uint32_t i = 0; i < numSubSamplesPerLine; ++i)
    {
      lineSampleIndicesDst[i] = (uint16_t)lineSampleIndicesSrc[i];
    }
  }

  // Pad the remaining block of four lines with the last entry
  if (numLines & 0x03)
  {
    uint32_t offsetSrc = (numLines - 1) * numSubSamplesPerLine;
    const uint16_t* lineSampleIndicesSrc = &attribDef->m_lineSampleIndices[offsetSrc];

    for (uint32_t lineIndex = numLines; lineIndex & 0x03; ++lineIndex)
    {
      uint32_t offsetDst = lineIndex * numSubSamplesPerLine;
      uint16_t* lineSampleIndicesDst = &attribDef->m_lineSampleIndices[offsetDst];
      for (uint32_t i = 0; i < numSubSamplesPerLine; ++i)
      {
        lineSampleIndicesDst[i] = lineSampleIndicesSrc[i];
      }
    }
  }

  return attribDataHandle;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlend1DBuilder::buildAttribDataForAnalysis(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Create the pre-init data for analysis
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlend1DPreInitData* preInitData = NodeScatterBlend1DPreInitData::createAndInit(1);
  netDefCompilationInfo->attachUserData(nodeID, preInitData);
  preInitData->m_exportForAnalysis = true;

  // Create the memory for the attrib data
  NMP::Memory::Format memReqsAttribDef = MR::AttribDataScatterBlendAnalysisDef::getMemoryRequirements(1, 2);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(memReqsAttribDef);
  NMP_ASSERT(resource.ptr);
  MR::AttribDataScatterBlendAnalysisDef* attribDef = MR::AttribDataScatterBlendAnalysisDef::init(
    resource,
    1, 2,
    MR::IS_DEF_ATTRIB_DATA);

  // Set the user data
  preInitData->m_attribDataHandles[0].m_format = memReqsAttribDef;
  preInitData->m_attribDataHandles[0].m_attribData = attribDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlend1DBuilder::buildAttribDataForPreview(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  // Read the scatter blend options from the export data
  ScatterBlend1DExportInfo scatterBlendExportInfo;
  scatterBlendExportInfo.readHeaderInfo(
    nodeDefExport,
    netDefExport,
    processor);

  // Find the analysed data
  const ME::AnalysisNodeExport* taskAnalysisNode = NULL;
  uint32_t numAnalysisNodes = nodeDefExport->getNumAnalysisNodes();
  for (uint32_t i = 0; i < numAnalysisNodes; ++i)
  {
    const ME::AnalysisNodeExport* analysisNode = nodeDefExport->getAnalysisNode(i);
    if (strcmp(analysisNode->getTypeName(), "ScatterBlend1DTask") == 0)
    {
      taskAnalysisNode = analysisNode;
      break;
    }
  }
  NMP_VERIFY_MSG(
    taskAnalysisNode,
    "Unable to find the analysisNode data for the analysis task");

  // Allocate and initialise the pre-init data for preview
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlend1DPreInitData* preInitData = NodeScatterBlend1DPreInitData::createAndInit(scatterBlendExportInfo.m_numAnimSets);
  netDefCompilationInfo->attachUserData(nodeID, preInitData);
  preInitData->m_exportForAnalysis = false;

  //---------------------------
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < scatterBlendExportInfo.m_numAnimSets; ++animSetIndex)
  {
    // Import the intermediate export data
    const ME::DataBlockExport* analysisNodeDataBlock = taskAnalysisNode->getDataBlock();
    SB1DAnnotationExportDataPacked annotationExportData;
    SB1DAnnotationSubSampleRangeData annotationRangeData;

    ScatterBlend1DTask::readPerAnimSetIntermediateExportData(
      analysisNodeDataBlock,
      animSetIndex,
      annotationExportData,
      annotationRangeData);

    // Create and initialise the pre-init data
    preInitData->m_attribDataHandles[animSetIndex] = createAndInitAttribDataScatterBlend1DDef(
      animSetIndex,
      scatterBlendExportInfo,
      annotationExportData,
      annotationRangeData);
    NMP_VERIFY(preInitData->m_attribDataHandles[animSetIndex].m_attribData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScatterBlend1DBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_1D, "Expecting node type SCATTER_BLEND_1D");

  //---------------------------
  // Set up connected source weight control param node data.
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Weight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);

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
NMP::Memory::Format NodeScatterBlend1DBuilder::getNodeDefMemoryRequirementsForAttribData(
  NetworkDefCompilationInfo* netDefCompilationInfo,
  NodeScatterBlend1DPreInitData* preInitData,
  const ME::NodeExport* nodeDefExport)
{
  NMP_VERIFY(preInitData);
  MR::AnimSetIndex numAnimSets = (MR::AnimSetIndex)preInitData->m_numAnimSets;
  NMP_VERIFY(numAnimSets > 0);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2);

  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    sourceNodeCount,        // numChildren
    1,                      // numInputCPConnections
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
MR::NodeDef* NodeScatterBlend1DBuilder::initForAnalysis(
  NMP::Memory::Resource&          memRes,
  NetworkDefCompilationInfo*      netDefCompilationInfo,
  NodeScatterBlend1DPreInitData*  preInitData,
  const ME::NodeExport*           nodeDefExport,
  AssetProcessor*                 processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_1D, "Expecting node type SCATTER_BLEND_1D");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();  
  NMP_VERIFY(preInitData);
  
  // Only a single anim set is used for analysis
  MR::AnimSetIndex numAnimSets = (MR::AnimSetIndex)preInitData->m_numAnimSets;
  NMP_VERIFY(numAnimSets == 1);

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2);

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
    2,                // maxNumActiveChildNodes
    1,                // numInputCPConnections
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
    MR::AttribDataScatterBlendAnalysisDef::relocate(defAttribData);

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

  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeScatterBlend1DOfflineUpdateConnections), logger);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeScatterBlend1DBuilder::initForPreview(
  NMP::Memory::Resource&          memRes,
  NetworkDefCompilationInfo*      netDefCompilationInfo,
  NodeScatterBlend1DPreInitData*  preInitData,
  const ME::NodeExport*           nodeDefExport,
  AssetProcessor*                 processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCATTER_BLEND_1D, "Expecting node type SCATTER_BLEND_1D");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  NMP_VERIFY(preInitData);
  MR::AnimSetIndex numAnimSets = (MR::AnimSetIndex)preInitData->m_numAnimSets;
  NMP_VERIFY(numAnimSets > 0);

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2);

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
    2,                        // maxNumActiveChildNodes
    1,                        // numInputCPConnections
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
    const MR::AttribDataScatterBlend1DDef* attribDef = (const MR::AttribDataScatterBlend1DDef*)preInitData->m_attribDataHandles[animSetIndex].m_attribData;
    NMP_VERIFY(attribDef);

    MR::AttribDataScatterBlend1DDef* defAttribData = (MR::AttribDataScatterBlend1DDef*)memRes.alignAndIncrement(memReqsAttribDef);
    NMP::Memory::memcpy(defAttribData, attribDef, memReqsAttribDef.size);
    MR::AttribDataScatterBlend1DDef::relocate(defAttribData);

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

  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeScatterBlend1DUpdateConnections), logger);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeScatterBlend1DBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Get the precomputed user data
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlend1DPreInitData* preInitData = (NodeScatterBlend1DPreInitData*)netDefCompilationInfo->getUserData(nodeID);
  NMP_VERIFY(preInitData);
  return getNodeDefMemoryRequirementsForAttribData(
    netDefCompilationInfo,
    preInitData,
    nodeDefExport);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeScatterBlend1DBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  // Get the precomputed user data
  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeScatterBlend1DPreInitData* preInitData = (NodeScatterBlend1DPreInitData*)netDefCompilationInfo->detachUserData(nodeID);
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
void NodeScatterBlend1DBuilder::initTaskQueuingFnsBase(
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
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeBlend2SyncEventsQueueSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    // Events
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2SyncEventsQueueSampledEventsBuffers), logger);

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
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2QueueSampledEventsBuffers), logger);

    // We don't deal with sync event tracks so let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, FN_NAME(queuePassThroughChild0), logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNFindGeneratingNodeForSemantic), logger);
  }

  //---------------------------
  // Transforms
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos), logger);

  //---------------------------
  // Trajectory
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
  }

  //---------------------------
  // Trajectory delta and transforms
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj), logger);
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
