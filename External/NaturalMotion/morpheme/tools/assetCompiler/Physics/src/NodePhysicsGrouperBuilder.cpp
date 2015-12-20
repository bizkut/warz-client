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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodePhysicsGrouperBuilder.h"
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodePhysicsGrouperBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodePhysicsGrouperBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_PHYSICS_GROUPER);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodePhysicsGrouperBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node IDs.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "NodeBaseConnectedNodeID", source0NodeID);
  childNodeIDs.push_back(source0NodeID);

  MR::NodeID source1NodeID;
  readNodeID(nodeDefDataBlock, "NodeOverrideConnectedNodeID", source1NodeID);
  childNodeIDs.push_back(source1NodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodePhysicsGrouperBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodePhysicsGrouperBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t controlParamCount = 0;

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = 0;
  nodeDefDataBlock->readUInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);
 
  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    2,                  // numChildren
    controlParamCount,  // numInputCPConnections
    numAnimSets,        // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Prefer base trajectory
  result += MR::AttribDataBool::getMemoryRequirements();

  //---------------------------
  // Animation set dependent attribute data
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    sprintf_s(paramName, "NumChannelIndicesSet_%d", animSetIndex + 1);
    uint32_t numChannelIndices = 0;
    nodeDefDataBlock->readUInt(numChannelIndices, paramName);
    result += MR::AttribDataUIntArray::getMemoryRequirements(numChannelIndices);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodePhysicsGrouperBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_PHYSICS_GROUPER, "Expecting node type PHYSICS_GROUPER");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int16_t numAnimSets = readNumAnimSets(nodeDefDataBlock);
    
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    2,             // numChildren
    2,             // max num active child nodes
    0,             // numInputCPConnections
    0,             // numOutputCPPins
    numAnimSets,   // numAnimSetEntries
    nodeDefExport);

  // Flag node as a filter.
  int32_t passThroughMode = 0;
  nodeDefDataBlock->readInt(passThroughMode, "PassThroughMode");
  // Flag node as a grouper and as a filter node.
  nodeDef->setPassThroughChildIndex((uint16_t)passThroughMode);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER | MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node IDs.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "NodeBaseConnectedNodeID", source0NodeID);
  nodeDef->setChildNodeID(0, source0NodeID);

  MR::NodeID source1NodeID;
  readNodeID(nodeDefDataBlock, "NodeOverrideConnectedNodeID", source1NodeID);
  nodeDef->setChildNodeID(1, source1NodeID);

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Prefer base trajectory
  bool preferBaseTrajectory = true;
  nodeDefDataBlock->readBool(preferBaseTrajectory, "PreferBaseTrajectory");

  MR::AttribDataBool* preferBaseTrajectoryAttribData = MR::AttribDataBool::init(memRes, preferBaseTrajectory, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,        // semantic
    0,                                            // animSetIndex
    preferBaseTrajectoryAttribData,               // attribData
    MR::AttribDataBool::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Animation set dependent attribute data
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    sprintf_s(paramName, "NumChannelIndicesSet_%d", animSetIndex + 1);
    uint32_t numChannelIndices = 0;
    nodeDefDataBlock->readUInt(numChannelIndices, paramName);

    MR::AttribDataUIntArray* channelIndicesAttribData = MR::AttribDataUIntArray::init(memRes, numChannelIndices, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,                     // semantic
      animSetIndex,                                                       // animSetIndex
      channelIndicesAttribData,                                           // attribData
      MR::AttribDataUIntArray::getMemoryRequirements(numChannelIndices)); // attribMemReqs

    int32_t channelValue = 0;
    for (uint32_t i = 0; i < numChannelIndices; ++i)
    {
      sprintf_s(paramName, "ChannelIndex_%d_Set_%d", i + 1, animSetIndex + 1);
      nodeDefDataBlock->readInt(channelValue, paramName);
      channelIndicesAttribData->m_values[i] = channelValue;
    }
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodePhysicsGrouperBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodePhysicsGrouperQueueUpdateTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodePhysicsGrouperQueueUpdateTrajectory), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(nodePhysicsGrouperQueueGetOutputMask), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, FN_NAME(nodePhysicsGrouperQueueUpdateTransformsPrePhysics), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodePhysicsGrouperUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodePhysicsGrouperFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
