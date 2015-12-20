//----------------------------------------------------------------------------------------------------------------------
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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodeOperatorRollDownStairsBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorRollDownStairs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorRollDownStairsBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorRollDownStairsBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_ROLLDOWNSTAIRS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRollDownStairsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Control parameter
  readDataPinChildNodeID(nodeDefDataBlock, "Enable", childNodeIDs, true); // optional
  readDataPinChildNodeID(nodeDefDataBlock, "BalanceAmount", childNodeIDs, true); // optional
  readDataPinChildNodeID(nodeDefDataBlock, "SlopeNormal", childNodeIDs, true); 
  readDataPinChildNodeID(nodeDefDataBlock, "MaxPush", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRollDownStairsBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Enable", 0, true, MR::ATTRIB_SEMANTIC_CP_BOOL);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "SlopeNormal", 1, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "BalanceAmount", 2, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "MaxPush", 3, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorRollDownStairsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    0,                    // numChildren
    4,                    // numInputCPConnections
    1,                    // numAnimSetEntries
    nodeDefExport);

  // Space for my AttribData
  result += ER::AttribDataOperatorRollDownStairsDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorRollDownStairsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_ROLLDOWNSTAIRS);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    0,                                                    // numChildren
    0,                                                    // max num active child nodes
    4,                                                    // numInputCPConnections
    ER::NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_COUNT,  // numOutputCPPins
    1,                                                    // numAnimSetEntries
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.

  // Read in the node attributes
  float minEnterAngleWithNormalToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(minEnterAngleWithNormalToApplyRollPose, "MinEnterAngleWithNormalToApplyRollPose");

  float maxEnterAngleWithNormalToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(maxEnterAngleWithNormalToApplyRollPose, "MaxEnterAngleWithNormalToApplyRollPose");

  float minExitAngleWithNormalToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(minExitAngleWithNormalToApplyRollPose, "MinExitAngleWithNormalToApplyRollPose");

  float maxExitAngleWithNormalToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(maxExitAngleWithNormalToApplyRollPose, "MaxExitAngleWithNormalToApplyRollPose");

  float minTangentialVelocityToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(minTangentialVelocityToApplyRollPose, "MinTangentialVelocityToApplyRollPose");

  float maxTangentialVelocityToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(maxTangentialVelocityToApplyRollPose, "MaxTangentialVelocityToApplyRollPose");

  float minAngularVelocityToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(minAngularVelocityToApplyRollPose, "MinAngularVelocityToApplyRollPose");

  float maxAngularVelocityToApplyRollPose = 0.0f;
  nodeDefDataBlock->readFloat(maxAngularVelocityToApplyRollPose, "MaxAngularVelocityToApplyRollPose");

  float minMotionRatioToBeConsideredRolling = 0.0f;
  nodeDefDataBlock->readFloat(minMotionRatioToBeConsideredRolling, "MinMotionRatioToBeConsideredRolling");

  float poseWeightMultiplier = 0.0f;
  nodeDefDataBlock->readFloat(poseWeightMultiplier, "PoseWeightMultiplier");

  float sideRollAmount = 0.0f;
  nodeDefDataBlock->readFloat(sideRollAmount, "SideRollAmount");

  NMP_VERIFY(minEnterAngleWithNormalToApplyRollPose != 0.0f)
  NMP_VERIFY(minExitAngleWithNormalToApplyRollPose != 0.0f)
  ER::AttribDataOperatorRollDownStairsDef* rollDownStairsAttribData =
    ER::AttribDataOperatorRollDownStairsDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  rollDownStairsAttribData->m_minEnterAngleWithNormalToApplyRollPose = minEnterAngleWithNormalToApplyRollPose;
  rollDownStairsAttribData->m_maxEnterAngleWithNormalToApplyRollPose = maxEnterAngleWithNormalToApplyRollPose;
  rollDownStairsAttribData->m_minExitAngleWithNormalToApplyRollPose = minExitAngleWithNormalToApplyRollPose;
  rollDownStairsAttribData->m_maxExitAngleWithNormalToApplyRollPose = maxExitAngleWithNormalToApplyRollPose;
  rollDownStairsAttribData->m_minTangentialVelocityToApplyRollPose = minTangentialVelocityToApplyRollPose;
  rollDownStairsAttribData->m_maxTangentialVelocityToApplyRollPose = maxTangentialVelocityToApplyRollPose;
  rollDownStairsAttribData->m_minAngularVelocityToApplyRollPose = minAngularVelocityToApplyRollPose;
  rollDownStairsAttribData->m_maxAngularVelocityToApplyRollPose = maxAngularVelocityToApplyRollPose;
  rollDownStairsAttribData->m_minMotionRatioToBeConsideredRolling = minMotionRatioToBeConsideredRolling;
  rollDownStairsAttribData->m_poseWeightMultiplier = poseWeightMultiplier;
  rollDownStairsAttribData->m_sideRollAmount = sideRollAmount;

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                          // semantic
    0,                                                              // animSetIndex
    rollDownStairsAttribData,                                       // attribData
    ER::AttribDataOperatorRollDownStairsDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRollDownStairsBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_ROLLPOSEANIMWEIGHT,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorRollDownStairsOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_BALANCEDESIREDVELOCITY,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorRollDownStairsOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_ISROLLING,
    MR::ATTRIB_SEMANTIC_CP_BOOL,
    ER_FN_NAME(nodeOperatorRollDownStairsOutputCPUpdate),
    logger);
  
  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
