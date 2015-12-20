// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#include "NodeHipsIKBuilder.h"
#include "morpheme/Nodes/mrNodeHipsIK.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool AP::NodeHipsIKBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* AP::NodeHipsIKBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_HIPS_IK);
  NMP_ASSERT(semanticLookupTable);

  // Add look up index for the general node def data
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Add look up index for the per-anim-set node def data
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeHipsIKBuilder::getNodeDefInputConnections(
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
  readDataPinChildNodeID(nodeDefDataBlock, "PositionDelta", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "RotationDeltaQuatNodeID", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "RotationDeltaEulerNodeID", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "FootTurnWeight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "Weight", childNodeIDs, true);

}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeHipsIKBuilder::calculateNodeDefMetrics(
  const ME::DataBlockExport* NMP_UNUSED(nodeDefDatablock),
  uint32_t& numberOfChildren,
  uint32_t& numberOfInputControlParams,
  uint8_t& numberOfOutputControlParams)
{
  // Decide on number of children/cparams, either statically or by reading data from nodeDefDatablock.
  numberOfChildren = 1;
  numberOfInputControlParams = 5;
  numberOfOutputControlParams = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AP::NodeHipsIKBuilder::getNodeDefMemoryRequirements( 
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

  //---------------------------
  // Node-specific attrib data
  //   General node specific attributes
  result += MR::AttribDataHipsIKDef::getMemoryRequirements();
  //
  //   Animation set specific attrib data - the per-anim-set node def data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    result += MR::AttribDataHipsIKAnimSetDef::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeHipsIKBuilder::preInit( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PositionDelta", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "RotationDeltaQuatNodeID", 1, true, MR::ATTRIB_SEMANTIC_CP_VECTOR4);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "RotationDeltaEulerNodeID", 2, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "FootTurnWeight", 3, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Weight", 4, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* AP::NodeHipsIKBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_HIPS_IK, "Expecting node type HIPS_IK");

  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  uint32_t numberOfChildren, numberOfInputControlParams;
  uint8_t numberOfOutputControlParams;
  calculateNodeDefMetrics(nodeDefDatablock, numberOfChildren, numberOfInputControlParams, numberOfOutputControlParams);

  // Set the number of animation set entries.
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numberOfAnimSetEntries = (uint16_t)animLibraryExport->getNumAnimationSets();

  // Initialise the nodedef itself.
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
  readNodeID(nodeDefDatablock, "SourceNodeID", SourceNodeID);
  nodeDef->setChildNodeID(0, SourceNodeID);

  //---------------------------
  // Attribute data

  // General node attributes
  MR::AttribDataHipsIKDef* hipsIKDef = MR::AttribDataHipsIKDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
    0,
    hipsIKDef,
    MR::AttribDataHipsIKDef::getMemoryRequirements());

  // Up axis
  uint32_t upAxisIndex = 1;
  nodeDefDatablock->readUInt(upAxisIndex, "UpAxisIndex");
  upAxisIndex = NMP::clampValue(upAxisIndex, 0u, 2u);
  hipsIKDef->m_upAxis.setToZero();
  hipsIKDef->m_upAxis[upAxisIndex] = 1.0f;

  // Orientation weight
  bool keepWorldFootOrientation;
  nodeDefDatablock->readBool(keepWorldFootOrientation, "KeepWorldFootOrientation");
  hipsIKDef->m_keepWorldFootOrientation = keepWorldFootOrientation;

  // Swivel orientation weight
  nodeDefDatablock->readFloat(hipsIKDef->m_swivelOrientationWeight, "KneeSwivelWeight");
  hipsIKDef->m_swivelOrientationWeight = NMP::clampValue(hipsIKDef->m_swivelOrientationWeight, 0.0f, 1.0f);

  // Local reference frame
  nodeDefDatablock->readBool(hipsIKDef->m_localReferenceFrame, "LocalReferenceFrame");

  // Per-anim-set attributes
  CHAR paramName[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    const ME::RigExport* rigExport = animSetExport->getRig();

    MR::AttribDataHipsIKAnimSetDef* hipsIKAnimSetDef =
      MR::AttribDataHipsIKAnimSetDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,
      animSetIndex,
      hipsIKAnimSetDef,
      MR::AttribDataHipsIKAnimSetDef::getMemoryRequirements());

    // Hips index
    sprintf_s(paramName, "HipsIndex_%d", animSetIndex + 1);
    nodeDefDatablock->readInt(hipsIKAnimSetDef->m_hipsIndex, paramName);

    // Get the left leg knee axis
    sprintf_s(paramName, "LeftKneeRotationAxisX_%d", animSetIndex + 1);
    nodeDefDatablock->readFloat(hipsIKAnimSetDef->m_leftKneeRotationAxis.x, paramName);
    sprintf_s(paramName, "LeftKneeRotationAxisY_%d", animSetIndex + 1);
    nodeDefDatablock->readFloat(hipsIKAnimSetDef->m_leftKneeRotationAxis.y, paramName);
    sprintf_s(paramName, "LeftKneeRotationAxisZ_%d", animSetIndex + 1);
    nodeDefDatablock->readFloat(hipsIKAnimSetDef->m_leftKneeRotationAxis.z, paramName);
    hipsIKAnimSetDef->m_leftKneeRotationAxis.normalise();

    // Get the right leg knee axis
    sprintf_s(paramName, "RightKneeRotationAxisX_%d", animSetIndex + 1);
    nodeDefDatablock->readFloat(hipsIKAnimSetDef->m_rightKneeRotationAxis.x, paramName);
    sprintf_s(paramName, "RightKneeRotationAxisY_%d", animSetIndex + 1);
    nodeDefDatablock->readFloat(hipsIKAnimSetDef->m_rightKneeRotationAxis.y, paramName);
    sprintf_s(paramName, "RightKneeRotationAxisZ_%d", animSetIndex + 1);
    nodeDefDatablock->readFloat(hipsIKAnimSetDef->m_rightKneeRotationAxis.z, paramName);
    hipsIKAnimSetDef->m_rightKneeRotationAxis.normalise();

    // Flag for use/don't use ball joints
    sprintf_s(paramName, "UseBallJoints_%d", animSetIndex + 1);
    nodeDefDatablock->readBool(hipsIKAnimSetDef->m_useBallJoints, paramName);

    // Recover the left leg joint indices as a sequential hierarchy
    int32_t j = 0;
    if (hipsIKAnimSetDef->m_useBallJoints)
    {
      sprintf_s(paramName, "LeftBallIndex_%d", animSetIndex + 1);
      nodeDefDatablock->readInt(j, paramName);
      NMP_VERIFY(j > 0);
      hipsIKAnimSetDef->m_leftLegIndex[3] = j;
      j = rigExport->getJoint(j)->getParentIndex();
    }
    else
    {
      sprintf_s(paramName, "LeftAnkleIndex_%d", animSetIndex + 1);
      nodeDefDatablock->readInt(j, paramName);
    }
    NMP_VERIFY(j > 0);
    hipsIKAnimSetDef->m_leftLegIndex[2] = j;
    j = rigExport->getJoint(j)->getParentIndex();
    NMP_VERIFY(j > 0);
    hipsIKAnimSetDef->m_leftLegIndex[1] = j;
    j = rigExport->getJoint(j)->getParentIndex();
    NMP_VERIFY(j > 0);
    hipsIKAnimSetDef->m_leftLegIndex[0] = j;

    // Recover the right leg joint indices as a sequential hierarchy
    j = 0;
    if (hipsIKAnimSetDef->m_useBallJoints)
    {
      sprintf_s(paramName, "RightBallIndex_%d", animSetIndex + 1);
      nodeDefDatablock->readInt(j, paramName);
      NMP_VERIFY(j > 0);
      hipsIKAnimSetDef->m_rightLegIndex[3] = j;
      j = rigExport->getJoint(j)->getParentIndex();
    }
    else
    {
      sprintf_s(paramName, "RightAnkleIndex_%d", animSetIndex + 1);
      nodeDefDatablock->readInt(j, paramName);
    }
    NMP_VERIFY(j > 0);
    hipsIKAnimSetDef->m_rightLegIndex[2] = j;
    j = rigExport->getJoint(j)->getParentIndex();
    NMP_VERIFY(j > 0);
    hipsIKAnimSetDef->m_rightLegIndex[1] = j;
    j = rigExport->getJoint(j)->getParentIndex();
    NMP_VERIFY(j > 0);
    hipsIKAnimSetDef->m_rightLegIndex[0] = j;
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeHipsIKBuilder::initTaskQueuingFns(
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

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeHipsIKQueueUpdateTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    FN_NAME(nodeHipsIKQueueUpdateTrajectoryDeltaAndTransforms),
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

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
