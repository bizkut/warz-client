// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDefBuilder.h"
#include "NodeModifyJointTransformBuilder.h"
#include "morpheme/Nodes/mrNodeModifyJointTransform.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool AP::NodeModifyJointTransformBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* AP::NodeModifyJointTransformBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_MODIFY_JOINT_TRANSFORM);
  NMP_ASSERT(semanticLookupTable);

  // Add look up indexes for our nodes def data.
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeModifyJointTransformBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set the child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  childNodeIDs.push_back(sourceNodeID);

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "Enable", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "Linear", childNodeIDs, true);
  readVariableTypeDataPinChildNodeID(nodeDefDataBlock, "Angular", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeModifyJointTransformBuilder::calculateNodeDefMetrics(
  const ME::DataBlockExport* NMP_UNUSED(nodeDefDatablock),
  uint32_t& numberOfChildren,
  uint32_t& numberOfControlParams,
  uint8_t& numberOfOutputControlParams)
{
  // One source and 3 inputs.
  numberOfChildren = 1; 
  numberOfControlParams = 3; 
  numberOfOutputControlParams = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AP::NodeModifyJointTransformBuilder::getNodeDefMemoryRequirements( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  uint32_t numberOfChildren, numberOfInputControlParams;
  uint8_t numberOfOutputControlParams;
  calculateNodeDefMetrics(nodeDefDatablock, numberOfChildren, numberOfInputControlParams, numberOfOutputControlParams);

  // Set the number of animation set entries.
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numAnimSetEntries = animLibraryExport->getNumAnimationSets();

  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
                                    netDefCompilationInfo,
                                    numberOfChildren,
                                    numberOfInputControlParams,
                                    numAnimSetEntries,
                                    nodeDefExport);

  // Anim set independent def data.
  result += MR::AttribDataModifyJointDef::getMemoryRequirements();
  
  // Per anim set joint index.
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSetEntries; ++animSetIndex)
  {
    result += MR::AttribDataInt::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeModifyJointTransformBuilder::preInit( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Enable", 0, true, MR::ATTRIB_SEMANTIC_CP_BOOL);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Linear", 1, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);

  MR::AttribDataSemantic possibleTypes[] = 
  {
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    MR::ATTRIB_SEMANTIC_CP_VECTOR4
  };

  declareVariableTypeDataPin(netDefCompilationInfo, nodeDefExport, "Angular", 2, true, possibleTypes, sizeof(possibleTypes));
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* AP::NodeModifyJointTransformBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_MODIFY_JOINT_TRANSFORM, "Expecting node type SET_TRANSORM");
  
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t numberOfChildren, numberOfInputControlParams;
  uint8_t numberOfOutputControlParams;
  calculateNodeDefMetrics(nodeDefDataBlock, numberOfChildren, numberOfInputControlParams, numberOfOutputControlParams);

  // Set the number of animation set entries.
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numberOfAnimSetEntries = (uint16_t)animLibraryExport->getNumAnimationSets();

  // Initialize the nodedef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
                              memRes,
                              netDefCompilationInfo,
                              numberOfChildren,
                              (uint16_t)numberOfChildren,   // max num active child nodes
                              numberOfInputControlParams,
                              numberOfOutputControlParams,
                              numberOfAnimSetEntries,
                              nodeDefExport);

  // Flag as being a filter node.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set the child node ID.
  MR::NodeID SourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", SourceNodeID);
  nodeDef->setChildNodeID(0, SourceNodeID);
      

  //---------------------------
  // Def state data.
  MR::AttribDataModifyJointDef::RotationType rotationType = MR::AttribDataModifyJointDef::ROTATION_TYPE_EULER;  // [1] = "Euler Angle", [2] = "Quaternion"
  bool readErr = nodeDefDataBlock->readInt((int32_t &)rotationType, "InputRotationType");
  NMP_VERIFY(readErr);
  MR::AttribDataModifyJointDef::CoordinateSpace coordinateSpace = MR::AttribDataModifyJointDef::COORD_SPACE_LOCAL;  // [1] = "Local", [2] = "Character", [3] = "World"
  readErr = nodeDefDataBlock->readInt((int32_t &)coordinateSpace, "CoordinateSpace");
  NMP_VERIFY(readErr);
  MR::AttribDataModifyJointDef::OperationType linearOperation = MR::AttribDataModifyJointDef::OPERATION_TYPE_ADD;  // [1] = "Add", [2] = "Set"
  readErr = nodeDefDataBlock->readInt((int32_t &)linearOperation, "LinearOperation");
  NMP_VERIFY(readErr);
  MR::AttribDataModifyJointDef::OperationType angularOperation = MR::AttribDataModifyJointDef::OPERATION_TYPE_ADD;  // [1] = "Add", [2] = "Set"
  readErr = nodeDefDataBlock->readInt((int32_t &)angularOperation, "AngularOperation");
  NMP_VERIFY(readErr);
  MR::AttribDataModifyJointDef* modifyJointDefAttrib = MR::AttribDataModifyJointDef::init(
                                                                 memRes,
                                                                 rotationType,
                                                                 coordinateSpace,
                                                                 linearOperation,
                                                                 angularOperation,
                                                                 MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
              nodeDef,
              MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,       // semantic
              0,                                           // animSetIndex
              modifyJointDefAttrib,                        // attribData
              MR::AttribDataInt::getMemoryRequirements()); // attribMemReqs
  
  //---------------------------
  // Per anim set Joint Index.
  char paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numberOfAnimSetEntries; ++animSetIndex)
  {
    // Joint index
    MR::AttribDataInt* jointIndexAttrib = MR::AttribDataInt::init(memRes, 0, MR::IS_DEF_ATTRIB_DATA);
    sprintf_s(paramName, "JointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readInt(jointIndexAttrib->m_value, paramName);

    initAttribEntry(
                nodeDef,
                MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,
                animSetIndex,
                jointIndexAttrib,
                MR::AttribDataInt::getMemoryRequirements());
  }
  
  //---------------------------
  // Initialize the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeModifyJointTransformBuilder::initTaskQueuingFns(
  MR::NodeDef*               nodeDef,
  NetworkDefCompilationInfo* netDefCompilationInfo,
  NMP::BasicLogger*          logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  //------------------------------------
  // Most queuing requests get passed on to our child
  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }
   
  nodeDef->setTaskQueuingFnId(taskQueuingFns, 
                              MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, 
                              FN_NAME(nodeModifyJointTransformQueueTransforms), 
                              logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, 
                              MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, 
                              FN_NAME(nodeModifyJointTransformQueueTransforms), 
                              logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns,
                              MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
                              FN_NAME(nodeModifyJointTransformQueueTrajectoryDeltaAndTransforms),
                              logger);
  
  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren));
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildrenOptionalInputCPs));
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic));
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace AP
