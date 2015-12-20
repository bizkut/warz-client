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
#include "NodeExtractJointInfoBuilder.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeExtractJointInfoBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeExtractJointInfoBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_EMIT_JOINT_CP_INFO);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_UINT);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_INT);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_BOOL);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeExtractJointInfoBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  childNodeIDs.push_back(sourceNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeExtractJointInfoBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeExtractJointInfoBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = 0;
  nodeDefDataBlock->readUInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,              // numChildren
    1,              // numInputCPConnections
    numAnimSets,    // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Animation set dependent attribute data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    result += MR::AttribDataUInt::getMemoryRequirements();
    result += MR::AttribDataInt::getMemoryRequirements();
    result += MR::AttribDataBool::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeExtractJointInfoBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_EMIT_JOINT_CP_INFO, "Expecting node type NODE_TYPE_OUTPUT_JOINT_CP_INFO");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int16_t numAnimSets = readNumAnimSets(nodeDefDataBlock);
  
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,                                           // numChildren
    1,                                           // max num active child nodes
    1,                                           // numInputCPConnections
    MR::NODE_EXTRACT_JOINT_INFO_OUT_PINID_COUNT, // numOutputCPPins
    numAnimSets,                                 // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  nodeDef->setChildNodeID(0, sourceNodeID);
  //---------------------------
  // Initialise the attrib datas.
 
  //---------------------------
  // Animation set dependent attribute data.
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    // Joint index -----
    MR::AttribDataUInt* jointIndexAttr = MR::AttribDataUInt::init(memRes, 0, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_CP_UINT,                  // semantic,    
      animSetIndex,                                 // animSetIndex,
      jointIndexAttr,                               // attribData,  
      MR::AttribDataUInt::getMemoryRequirements()); // attribMemReqs

    // Read and set the joint index.
    uint32_t jointIndex = 0;
    sprintf_s(paramName, "JointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(jointIndex, paramName);
    jointIndexAttr->m_value = jointIndex;

    // Angle type: [0] = "total", [1] = "eulerX", [2] = "eulerY", [3] = "eulerZ"
    MR::AttribDataInt* angleTypeAttr = MR::AttribDataInt::init(memRes, 0, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_CP_INT,                  // semantic,    
      animSetIndex,                                // animSetIndex,
      angleTypeAttr,                               // attribData,  
      MR::AttribDataInt::getMemoryRequirements()); // attribMemReqs

    // Read and set the angle type.
    int32_t angleType = 0;
    nodeDefDataBlock->readInt(angleType, "AngleType");
    angleTypeAttr->m_value = angleType;

    // Measure units: true = radians, false = degrees
    MR::AttribDataBool* unitsAttr = MR::AttribDataBool::init(memRes, true, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_CP_BOOL,                 // semantic,    
      animSetIndex,                                // animSetIndex,
      unitsAttr,                                   // attribData,  
      MR::AttribDataInt::getMemoryRequirements()); // attribMemReqs

    // Read and set the measure units.
    bool measureUnits = true;
    nodeDefDataBlock->readBool(measureUnits, "MeasureUnit");
    unitsAttr->m_value = measureUnits;
  }

  // Read what transform space the user wants the outputs to be in (object(true) or local(false)).
  bool outputSpace = false;
  nodeDefDataBlock->readBool(outputSpace, "OutputSpace");

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    outputSpace,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeExtractJointInfoBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  bool                        outputSpace,
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

  //------------------------------------
  // The only attribute queuing function is for transforms.
  // Do the decision making here so that we don't have to do it at runtime.
  if (nodeDef->getInputCPConnectionSourceNodeID(0) == MR::INVALID_NODE_ID)
  {
    // Use the def selected value to determine the joint to output.
    if (outputSpace == true) // Object space
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoObjectQueueTransforms), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoObjectQueueTrajectoryDeltaAndTransforms), logger);
    }
    else
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoLocalQueueTransforms), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoLocalQueueTrajectoryDeltaAndTransforms), logger);
    }

    nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildren), logger);
  }
  else
  {
    // Use this one only when we are dynamically selecting the joint to output.
    if (outputSpace == true) // Object space
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoObjectJointSelectQueueTransforms), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoObjectJointSelectQueueTrajectoryDeltaAndTransforms), logger);
    }
    else
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoLocalJointSelectQueueTransforms), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeExtractJointInfoLocalJointSelectQueueTrajectoryDeltaAndTransforms), logger);
    }

    nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeExtractJointInfoJointSelectUpdateConnections), logger);
  }

  //------------------------------------
  // Register the shared task function tables.
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up.
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
