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
#include "NodeOperatorHitBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorHit.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

// All possible CPInputPin names
// (This operator has rather a lot of possible (optional) connections
// by rolling a display like this we can automate hookups below)
//
static const char* s_connectionToCPInputs[] = {
  "CPInputLookAtWoundOrHitSource",
  "TriggerDeath",
  "StandingStillTime",
  "FallenTime",
  "SteppingTime",
  "StepCount",
};
static const uint32_t maxInputConnections = sizeof(s_connectionToCPInputs) / sizeof(char*);

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorHitBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorHitBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_HIT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorHitBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Input Control Param Connections.
  for (uint32_t i = 0; i < maxInputConnections; ++i)
  {
    readDataPinChildNodeID(nodeDefDataBlock, s_connectionToCPInputs[i], childNodeIDs, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorHitBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "CPInputLookAtWoundOrHitSource", 0, true, MR::ATTRIB_SEMANTIC_CP_BOOL);  // Optional
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "TriggerDeath", 1, true, MR::ATTRIB_SEMANTIC_CP_BOOL);
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "StandingStillTime", 2, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "FallenTime", 3, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "SteppingTime", 4, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "StepCount", 5, true, MR::ATTRIB_SEMANTIC_CP_INT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorHitBuilder::getNodeDefMemoryRequirements(
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
    maxInputConnections,  // numInputCPConnections
    1,                    // numAnimSetEntries
    nodeDefExport);

  // Attribute def data
  result += ER::AttribDataOperatorHitDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorHitBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_HIT);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    0,                                          // numChildren
    0,                                          // max num active child nodes
    maxInputConnections,                        // numInputCPConnections
    ER::CP_OUT_COUNT,   // numOutputCPPins
    1,                                          // numAnimSetEntries
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);


  //---------------------------
  // register interest in the desired shot message
  uint32_t messageID = MR::INVALID_MESSAGE_ID;
  nodeDefDataBlock->readUInt(messageID, "HitMessageID");

  netDefCompilationInfo->registerMessageInterest(messageID, nodeDef->getNodeID());

  //---------------------------
  // Initialise the attrib datas.

  // Read in the node attributes
  const float NOT_FOUND_VAL = 12345.678910f;
  float standingStillTimeout        = NOT_FOUND_VAL; // dummy value to verify against
  float recoveredTimeout            = NOT_FOUND_VAL;
  float fallenAliveTimeout          = NOT_FOUND_VAL;
  float fallenDeadTimeout           = NOT_FOUND_VAL;
  float preFallTargetStepVelocity   = NOT_FOUND_VAL;
  float fallTargetStepVelocity      = NOT_FOUND_VAL;
  float fallBodyPushMagnitude       = NOT_FOUND_VAL;

  bool prioritiseReaches      = false;

  nodeDefDataBlock->readFloat(standingStillTimeout, "StandingStillTimeoutAttrib");
  nodeDefDataBlock->readFloat(recoveredTimeout, "RecoveredTimeoutAttrib");
  nodeDefDataBlock->readFloat(fallenAliveTimeout, "FallenAliveTimeoutAttrib");
  nodeDefDataBlock->readFloat(fallenDeadTimeout, "FallenDeadTimeoutAttrib");
  nodeDefDataBlock->readBool(prioritiseReaches, "PrioritiseReachesAttrib");
  nodeDefDataBlock->readFloat(preFallTargetStepVelocity, "PreFallTargetStepVelocityAttrib");
  nodeDefDataBlock->readFloat(fallTargetStepVelocity, "FallTargetStepVelocityAttrib");
  nodeDefDataBlock->readFloat(fallBodyPushMagnitude, "FallBodyPushMagnitudeAttrib");

  NMP_VERIFY(standingStillTimeout       != NOT_FOUND_VAL);
  NMP_VERIFY(recoveredTimeout           != NOT_FOUND_VAL);
  NMP_VERIFY(fallenAliveTimeout         != NOT_FOUND_VAL);
  NMP_VERIFY(fallenDeadTimeout          != NOT_FOUND_VAL);
  NMP_VERIFY(preFallTargetStepVelocity  != NOT_FOUND_VAL);
  NMP_VERIFY(fallTargetStepVelocity     != NOT_FOUND_VAL);
  NMP_VERIFY(fallBodyPushMagnitude      != NOT_FOUND_VAL);

  ER::AttribDataOperatorHitDef* hitAttribs = ER::AttribDataOperatorHitDef::init(
    memRes,
    standingStillTimeout,
    recoveredTimeout,
    fallenAliveTimeout,
    fallenDeadTimeout,
    prioritiseReaches,
    preFallTargetStepVelocity,
    fallTargetStepVelocity,
    fallBodyPushMagnitude,
    MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                    // semantic
    0,                                                        // animSetIndex
    hitAttribs,                                              // attribData
    ER::AttribDataOperatorHitDef::getMemoryRequirements());  // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorHitBuilder::initTaskQueuingFns(
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

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_NEWREACH0,
    MR::ATTRIB_SEMANTIC_CP_BOOL,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_NEWREACH1,
    MR::ATTRIB_SEMANTIC_CP_BOOL,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHLIMBINDEX0,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHLIMBINDEX1,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHPARTINDEX0,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);  

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHPARTINDEX1,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHPOSITION0,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHPOSITION1,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHNORMAL0,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHNORMAL1,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHSTRENGTH0,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHSTRENGTH1,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHSPEEDLIMIT0,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_REACHSPEEDLIMIT1,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_IMPULSESPINEDAMPING,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);


  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_LOOKPOSITION,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_LOOKWEIGHT,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_BODYSTRENGTH,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_STEPVELOCITY,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_LEGSTRENGTH0,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_LEGSTRENGTH1,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_HITREFLEXANIMIDWEIGHT,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_HITREFLEXANIMWEIGHT,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
      ER::CP_OUT_PERFORMANCESTATEEXIT,
      MR::ATTRIB_SEMANTIC_CP_FLOAT,
      ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
      logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_BALANCEASSISTANCEPOSITION,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_BALANCEASSISTANCEORIENTATION,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::CP_OUT_BALANCEASSISTANCEVELOCITY,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    ER_FN_NAME(nodeOperatorHitOutputCPUpdate),
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
  nodeDef->setInitNodeInstanceFnId(ER_FN_NAME(nodeOperatorHitInitInstance), logger);
  nodeDef->setMessageHandlerFnId(ER_FN_NAME(nodeOperatorHitMessageHandler), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
