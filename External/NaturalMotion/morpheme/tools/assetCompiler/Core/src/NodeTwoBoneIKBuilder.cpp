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
#include "NodeTwoBoneIKBuilder.h"
#include "NetworkDefBuilder.h"
#include "morpheme/Nodes/mrNodeTwoBoneIK.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
  
//----------------------------------------------------------------------------------------------------------------------
bool NodeTwoBoneIKBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeTwoBoneIKBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_TWO_BONE_IK);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTwoBoneIKBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set the child node ID.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  childNodeIDs.push_back(inputNodeID);

  readDataPinChildNodeID(nodeDefDataBlock,"EffectorTarget", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock,"TargetOrientation", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock,"SwivelAngle", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock,"IkFkBlendWeight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock,"SwivelContributionToOrientation", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTwoBoneIKBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Control parameters
  declareDataPin(netDefCompilationInfo, nodeDefExport, "EffectorTarget",  0,  true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "TargetOrientation",  1,  true, MR::ATTRIB_SEMANTIC_CP_VECTOR4);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "SwivelAngle",  2,  true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "IkFkBlendWeight",  3,  true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "SwivelContributionToOrientation",  4,  true, MR::ATTRIB_SEMANTIC_CP_FLOAT);

}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeTwoBoneIKBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  uint32_t inputCPCount = 5;

  //---------------------------
  // Work out how many attrib data we actually have.
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,              // numChildren
    inputCPCount,   // numInputCPConnections
    numAnimSets,    // numAnimSetEntries. Although this Node has anim set specific attrib data we still pass in the total number
                    // of anim sets. This parameter is used to reserve the space for possible anim set specific data. Unused entries
                    // remain as NULL pointers. These are kept to make the modification simpler and debugging easier.
    nodeDefExport);

  //---------------------------
  // Two bone IK setup
  result += MR::AttribDataTwoBoneIKSetup::getMemoryRequirements();

  //---------------------------
  // Animation set specific attrib data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    result += MR::AttribDataTwoBoneIKChain::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeTwoBoneIKBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_TWO_BONE_IK, "Expecting node type TWO_BONE_IK");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,             // numChildren
    1,             // max num active child nodes
    5,             // numInputCPConnections
    0,             // numOutputCPPins
    numAnimSets,   // numAnimSetEntries                     
    nodeDefExport);

  // Flag as being a filter node.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set the child node ID.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  nodeDef->setChildNodeID(0, inputNodeID);

  //---------------------------
  // Initialise the attrib datas.

  //---------------------------
  // Two bone IK setup
  MR::AttribDataTwoBoneIKSetup* twoBoneIKSetup = MR::AttribDataTwoBoneIKSetup::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                  // semantic
    0,                                                      // animSetIndex
    twoBoneIKSetup,                                         // attribData
    MR::AttribDataTwoBoneIKSetup::getMemoryRequirements()); // attribMemReqs

  // Set default to use when control param is not connected.
  twoBoneIKSetup->m_defaultTargetOrientation.identity();

  // AssumeSimpleHierarchy
  bool assumeSimpleHierarchy;
  nodeDefDataBlock->readBool(assumeSimpleHierarchy, "AssumeSimpleHierarchy");
  twoBoneIKSetup->m_assumeSimpleHierarchy = assumeSimpleHierarchy;

  // KeepEndEffOrientation
  bool keepEndEffOrientation = true;
  nodeDefDataBlock->readBool(keepEndEffOrientation, "KeepEndEffOrientation");
  twoBoneIKSetup->m_keepEndEffOrientation = keepEndEffOrientation;

  // If "targetOrientationParamNodeID" cp is connected.
  twoBoneIKSetup->m_userControlledOrientation = (nodeDef->getInputCPConnectionSourceNodeID(1) != MR::INVALID_PIN_INDEX);

  // UpdateTargetByDeltas
  bool updateTargetByDeltas;
  nodeDefDataBlock->readBool(updateTargetByDeltas, "UpdateTargetByDeltas");
  twoBoneIKSetup->m_updateTargetByDeltas = updateTargetByDeltas;

  // WorldSpaceTarget
  nodeDefDataBlock->readBool(twoBoneIKSetup->m_worldSpaceTarget, "WorldSpaceTarget");

  // UseSpecifiedJointAsTarget
  nodeDefDataBlock->readBool(twoBoneIKSetup->m_useSpecifiedJointAsTarget, "UseSpecifiedJointAsTarget");

  // UseSpecifiedJointOrientation
  nodeDefDataBlock->readBool(twoBoneIKSetup->m_useSpecifiedJointOrientation, "UseSpecifiedJointOrientation");
  if (twoBoneIKSetup->m_useSpecifiedJointOrientation)
  {
    twoBoneIKSetup->m_userControlledOrientation = true;
  }

  //---------------------------
  // Animation set specific attrib data
  CHAR paramName[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    MR::AttribDataTwoBoneIKChain* twoBoneIKChain = MR::AttribDataTwoBoneIKChain::init(memRes, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,         // semantic
      animSetIndex,                                           // animSetIndex
      twoBoneIKChain,                                         // attribData
      MR::AttribDataTwoBoneIKChain::getMemoryRequirements()); // attribMemReqs

    // FlipMidJointRotationDirection
    bool flipMidJointRotationDirection = false;
    sprintf_s(paramName, "FlipMidJointRotationDirection_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(flipMidJointRotationDirection, paramName);
    float flipOrNot = flipMidJointRotationDirection ? -1.0f : 1.0f;

    // MidJointRotationAxis
    NMP::Vector3  midJointRotationAxis(0, 0, 0);
    sprintf_s(paramName, "MidJointRotationAxisX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(midJointRotationAxis.x, paramName);
    sprintf_s(paramName, "MidJointRotationAxisY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(midJointRotationAxis.y, paramName);
    sprintf_s(paramName, "MidJointRotationAxisZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(midJointRotationAxis.z, paramName);
    twoBoneIKChain->m_midJointRotAxis = flipOrNot * midJointRotationAxis;

    // MidJointReferenceAxis
    NMP::Vector3  midJointReferenceAxis(0, 0, 0);
    sprintf_s(paramName, "MidJointReferenceAxisX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(midJointReferenceAxis.x, paramName);
    sprintf_s(paramName, "MidJointReferenceAxisY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(midJointReferenceAxis.y, paramName);
    sprintf_s(paramName, "MidJointReferenceAxisZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(midJointReferenceAxis.z, paramName);
    twoBoneIKChain->m_midJointReferenceAxis = flipOrNot * midJointReferenceAxis;

    // EndJointIndex
    uint32_t endJointIndex = 0;
    sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(endJointIndex, paramName);
    twoBoneIKChain->m_endJointIndex = endJointIndex;

    // MidJointIndex
    uint32_t midJointIndex = 0;
    sprintf_s(paramName, "MidJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(midJointIndex, paramName);
    twoBoneIKChain->m_midJointIndex = midJointIndex;

    // RootJointIndex
    uint32_t rootJointIndex = 0;
    sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(rootJointIndex, paramName);
    twoBoneIKChain->m_rootJointIndex = rootJointIndex;

    // TargetJointIndex
    if (twoBoneIKSetup->m_useSpecifiedJointAsTarget)
    {
      uint32_t targetJointIndex = 0;
      sprintf_s(paramName, "TargetJointIndex_%d", animSetIndex + 1);
      nodeDefDataBlock->readUInt(targetJointIndex, paramName);
      twoBoneIKChain->m_targetJointIndex = targetJointIndex;
    }

    // GlobalReferenceAxis
    bool globalReferenceAxis;
    sprintf_s(paramName, "GlobalReferenceAxis_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(globalReferenceAxis, paramName);
    twoBoneIKChain->m_globalReferenceAxis = globalReferenceAxis;
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
void NodeTwoBoneIKBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM, FN_NAME(nodeTwoBoneIKQueueTwoBoneIKSetup), logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeTwoBoneIKQueueTwoBoneIKTransformBuffs), logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, 
    MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, 
    FN_NAME(nodeTwoBoneIKQueueTwoBoneIKTrajectoryDeltaAndTransformBuffs),
    logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);
  
  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeTwoBoneIKUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
