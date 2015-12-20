//----------------------------------------------------------------------------------------------------------------------
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
#include "NodeOperatorFallOverWallBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorFallOverWall.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorFallOverWallBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorFallOverWallBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_FALLOVERWALL);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorFallOverWallBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  // Control parameter inputs
  readDataPinChildNodeID(nodeDefDataBlock, "Enable", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "RightEndEdgePoint", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "LeftEndEdgePoint", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "BalanceAmount", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "LegKickAmount", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "TargetApproachVel", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "TargetRollOverVel", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorFallOverWallBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Enable", 0, true, MR::ATTRIB_SEMANTIC_CP_BOOL);  // Optional
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "RightEndEdgePoint", 1, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "LeftEndEdgePoint", 2, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "BalanceAmount", 3, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  // Optional, dataPin
  declareDataPin(netDefCompilationInfo, nodeDefExport, "LegKickAmount", 4, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "TargetApproachVel", 5, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "TargetRollOverVel", 6, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorFallOverWallBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Check we've been called in the right context
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_FALLOVERWALL, "Expecting node type FALLOVERWALL");

  // Access to the node def data
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Work out how many attrib datas we have.
  uint32_t numAnimSets = 0;
  nodeDefDataBlock->readUInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);

  //---------------------------
  // Space for the NodeDef itself.
  uint32_t numChildren = 0;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    numChildren,            // numChildren
    7,                      // numInputCPConnections
    numAnimSets,            // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Space for def data (one per animset)
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    result += ER::AttribDataOperatorFallOverWallDef::getMemoryRequirements();
  }

  //---------------------------
  // Emitted messages

  // Find how many messages have been specified and thus the size of the map to hold them
  //
  uint32_t numMessageSlots = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(numMessageSlots, "NumMessageSlots");
  NMP_VERIFY((numMessageSlots != 0xFFFFFFFF) && (numMessageSlots < 33));
  if (numMessageSlots > 0)
  {
    result += MR::AttribDataEmittedMessagesMap::getMemoryRequirements(numMessageSlots);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorFallOverWallBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  //---------------------------
  // Check we've been called in the right context
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_FALLOVERWALL);

  //---------------------------
  // Initialise the NodeDef itself 
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Number of output cp's is the number of pins plus one if the node has messages to emit
  uint8_t numOutputCPs = ER::FOW_CP_OUT_COUNT;

  //---------------------------
  // Find how many messages have been specified and verify within limits.
  uint32_t numMessageSlots = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(numMessageSlots, "NumMessageSlots");
  NMP_VERIFY((numMessageSlots != 0xFFFFFFFF) && (numMessageSlots < 33));

  // Add one to numOutputCPs if we'll be emitting messages
  if (numMessageSlots > 0)  // Every node that can emit messages must have an extra output CP function 
  {                         //  that is called by the function Network::updateMessageEmitterNodes.
    ++numOutputCPs;         //  This function is designed primarily to allow a node to emit messages when it
  }                         //  has no control param or child/parent connections and hence has no update functions.
                            //  The registered function can be empty if the messages are actually sent elsewhere.

  const int16_t numAnimSets(readNumAnimSets(nodeDefDataBlock));

  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,                          // numChildren
    0,                          // max num active child nodes
    7,                          // numInputCPConnections
    numOutputCPs,               // numOutputCPPins
    numAnimSets,                // numAnimSetEntries
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.
    
  //---------------------------
  // Animation set dependent attribute data.
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 1; animSetIndex < numAnimSets + 1; ++animSetIndex)
  {
    float reachTriggerDistance = 0.0f;
    sprintf_s(paramName, "ReachTriggerDistance_%d", animSetIndex);
    nodeDefDataBlock->readFloat(reachTriggerDistance, paramName);

    float reachTriggerVelocity = 0.0f;
    sprintf_s(paramName, "ReachTriggerVelocity_%d", animSetIndex);
    nodeDefDataBlock->readFloat(reachTriggerVelocity, paramName);

    float fallTriggerDistance = 0.0f;
    sprintf_s(paramName, "FallTriggerDistance_%d", animSetIndex);
    nodeDefDataBlock->readFloat(fallTriggerDistance, paramName);

    float attractorTriggerDistance = 0.0f;
    sprintf_s(paramName, "AttractorTriggerDistance_%d", animSetIndex);
    nodeDefDataBlock->readFloat(attractorTriggerDistance, paramName);

    float maxWallHeightOffset = 0.0f;
    sprintf_s(paramName, "MaxWallHeightOffset_%d", animSetIndex);
    nodeDefDataBlock->readFloat(maxWallHeightOffset, paramName);

    float targetRollSpeed = 0.0f;
    sprintf_s(paramName, "TargetRollSpeed_%d", animSetIndex);
    nodeDefDataBlock->readFloat(targetRollSpeed, paramName);

    float sideRollAmount = 0.0f;
    sprintf_s(paramName, "SideRollAmount_%d", animSetIndex);
    nodeDefDataBlock->readFloat(sideRollAmount, paramName);

    float overWallTimeout = FLT_MAX;
    sprintf_s(paramName, "CompletedTimeout_%d", animSetIndex);
    nodeDefDataBlock->readFloat(overWallTimeout, paramName);

    float pendingTimeout = FLT_MAX;
    sprintf_s(paramName, "PendingTimeout_%d", animSetIndex);
    nodeDefDataBlock->readFloat(pendingTimeout, paramName);

    float stalledTimeout = FLT_MAX;
    sprintf_s(paramName, "StalledTimeout_%d", animSetIndex);
    nodeDefDataBlock->readFloat(stalledTimeout, paramName);

    float settleTimeout = FLT_MAX;
    sprintf_s(paramName, "SettleTimeout_%d", animSetIndex);
    nodeDefDataBlock->readFloat(settleTimeout, paramName);

    float legKickAmountDefault = 1.0f;
    sprintf_s(paramName, "LegKickAmount", animSetIndex);
    nodeDefDataBlock->readFloat(legKickAmountDefault, paramName);
    
    float targetApproachVelDefault = 1.5f;
    sprintf_s(paramName, "TargetApproachVel", animSetIndex);
    nodeDefDataBlock->readFloat(targetApproachVelDefault, paramName);

    float targetRollOverVelDefault = 1.7f;
    sprintf_s(paramName, "TargetRollOverVel", animSetIndex);
    nodeDefDataBlock->readFloat(targetRollOverVelDefault, paramName);


    NMP_VERIFY(targetRollSpeed != 0.0f)
      ER::AttribDataOperatorFallOverWallDef* fallOverWallAttribData = 
      ER::AttribDataOperatorFallOverWallDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);
    fallOverWallAttribData->m_reachTriggerDistance = reachTriggerDistance;
    fallOverWallAttribData->m_reachTriggerVelocity = reachTriggerVelocity;
    fallOverWallAttribData->m_fallTriggerDistance = fallTriggerDistance;
    fallOverWallAttribData->m_attractorTriggerDistance = attractorTriggerDistance;
    fallOverWallAttribData->m_maxWallHeightOffset = maxWallHeightOffset;
    fallOverWallAttribData->m_targetRollSpeed = targetRollSpeed;
    fallOverWallAttribData->m_sideRollAmount = sideRollAmount;
    fallOverWallAttribData->m_overWallTimeout = overWallTimeout;
    fallOverWallAttribData->m_fallenNotTriggeredTimeout = pendingTimeout;
    fallOverWallAttribData->m_stalledTimeout = stalledTimeout;
    fallOverWallAttribData->m_settleTimeout = settleTimeout;
    fallOverWallAttribData->m_legKickAmountDefault = legKickAmountDefault;
    fallOverWallAttribData->m_targetApproachVelDefault = targetApproachVelDefault;
    fallOverWallAttribData->m_targetRollOverVelDefault = targetRollOverVelDefault;
    

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                        // semantic
      animSetIndex - 1,                                             // animSetIndex (-1 for 0 based cpp array offset)
      fallOverWallAttribData,                                       // attribData
      ER::AttribDataOperatorFallOverWallDef::getMemoryRequirements()); // attribMemReqs
  }



  //---------------------------
  // Initialise the emitted messages map attrib data.
  if (numMessageSlots > 0)
  {
    MR::AttribDataEmittedMessagesMap* emittedMessageMapAttribData =
      MR::AttribDataEmittedMessagesMap::init(memRes, numMessageSlots, MR::IS_DEF_ATTRIB_DATA);

    //---------------------------
    // Initialise each map entry
    for (uint32_t i = 0; i < numMessageSlots; ++i)
    {
      initMessageMapEntry(i, emittedMessageMapAttribData, netDefExport, nodeDefDataBlock);
    }

    // Add message map to the node's AttribData array.
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP,                                   // semantic,    
      0,                                                                          // animSetIndex,
      emittedMessageMapAttribData,                                                // attribData,  
      MR::AttribDataEmittedMessagesMap::getMemoryRequirements(numMessageSlots));  // attribMemReqs
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    (numMessageSlots > 0),
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorFallOverWallBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  bool                        emitMessages,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();  

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_ARM0REACHIMPORTANCE,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_ARM0REACHTARGET,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_ARM0REACHNORMAL,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_ARM1REACHIMPORTANCE,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_ARM1REACHTARGET,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_ARM1REACHNORMAL,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_LEG0WRITHESTRENGTH,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_LEG1WRITHESTRENGTH,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_LEG0STRENGTH,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_LEG1STRENGTH,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_SPINESTRENGTH,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_OVERWALLTIME,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_STALLEDTIME,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_FALLENNOTTRIGGEREDTIME,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_COMANGLETOWALLNORMAL,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::FOW_CP_OUT_TARGET_VELOCITY,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorFallOverWallOutputCPUpdate),
    logger);

  if (emitMessages)
  {
    nodeDef->setOutputCPTaskFnId(
      outputCPTaskFns,
      ER::FOW_CP_OUT_COUNT,
      MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES,
      ER_FN_NAME(nodeOperatorFallOverWallEmitMessageUpdate),
      logger);
  }
  
  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(ER_FN_NAME(nodeOperatorFallOverWallInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
