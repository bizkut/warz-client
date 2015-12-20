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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodeOperatorTimeLagInputBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorTimeLagInput.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorTimeLagInputBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorTimeLagInputBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_TIME_LAG_CP);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorTimeLagInputBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Input control parameter connections.
  readVariableTypeDataPinChildNodeID(nodeDefDataBlock, "Input", childNodeIDs, false);
  readDataPinChildNodeID(nodeDefDataBlock, "LagTime", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorTimeLagInputBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_TIME_LAG_CP, "Expecting node type NODE_TYPE_CP_OP_TIME_LAG_CP");
  
  MR::AttribDataSemantic possibleTypes[] = 
            {
              MR::ATTRIB_SEMANTIC_CP_FLOAT,
              MR::ATTRIB_SEMANTIC_CP_INT,
              MR::ATTRIB_SEMANTIC_CP_VECTOR3,
              MR::ATTRIB_SEMANTIC_CP_VECTOR4,
              MR::ATTRIB_SEMANTIC_CP_BOOL
            };
  declareVariableTypeDataPin(netDefCompilationInfo, nodeDefExport, "Input", 0, true, possibleTypes, sizeof(possibleTypes));
  declareDataPin(netDefCompilationInfo, nodeDefExport, "LagTime", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorTimeLagInputBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,               // numChildren
    2,               // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Add node def state data.
  result += MR::AttribDataTimeLagInputDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorTimeLagInputBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,      // numChildren
    0,      // max num active child nodes
    2,      // numInputCPConnections
    2,      // numOutputCPPins
    1,      // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.

  // Frame buffer size value.
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_TIME_LAG_CP, "Expecting node type NODE_TYPE_CP_OP_TIME_LAG_CP");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
    
  MR::AttribDataTimeLagInputDef::BufferingMode bufferingMode = MR::AttribDataTimeLagInputDef::BUFFERING_MODE_EVERY_FRAME;  // [1] = "Every Frame", [2] = "Fixed Interval"
  bool readErr = nodeDefDataBlock->readInt((int32_t &)bufferingMode, "BufferingMode");
  NMP_VERIFY(readErr);


  float sampleFrequency = 10.0f;
  uint32_t sampleBufferSize = 32;
  if (bufferingMode == MR::AttribDataTimeLagInputDef::BUFFERING_MODE_EVERY_FRAME)
  {
    nodeDefDataBlock->readUInt(sampleBufferSize, "SampleBufferSize");
  }
  else
  {
    nodeDefDataBlock->readFloat(sampleFrequency, "SampleFrequency"); // How frequently samples will be taken from the input.
    NMP_VERIFY(sampleFrequency > 0.0f);
    float maximumSamplingPeriod = 1.0f;
    NMP_VERIFY(maximumSamplingPeriod > 0.0f);
    nodeDefDataBlock->readFloat(maximumSamplingPeriod, "MaximumSamplingPeriod");  // The max period over which this node can lag its input.
    NMP_VERIFY(maximumSamplingPeriod > 0.0f);
    float sampleInterval = 1.0f / sampleFrequency;        // What is the interval between each recorded sample.
    sampleBufferSize = (uint32_t)(maximumSamplingPeriod / sampleInterval) + 1;      // What is the maximum number of samples we can store at the above frequency.
  }
  MR::AttribDataTimeLagInputDef* stateDefData = MR::AttribDataTimeLagInputDef::init(
                                                                        memRes,
                                                                        bufferingMode,
                                                                        sampleFrequency,
                                                                        sampleBufferSize,
                                                                        MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                   // semantic
    0,                                                       // animSetIndex
    stateDefData,                                            // attribData
    MR::AttribDataTimeLagInputDef::getMemoryRequirements()); // attribMemReqs
      
  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    nodeDefDataBlock,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorTimeLagInputBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::DataBlockExport*  nodeDefDataBlock,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Find which type of pin data we should be using.
  uint32_t pinDataType;
  nodeDefDataBlock->readUInt(pinDataType, "InputAndOutputType");
  
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  // Set up different functions for each type of pin data.
  switch (pinDataType)
  {
    case 1: // float
    {
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    0,
                    MR::ATTRIB_SEMANTIC_CP_FLOAT,
                    "nodeOperatorTimeLagInputOutputCPUpdateFloat",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<float, MR::AttribDataFloat, MR::ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>,
                    logger);
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    1,
                    MR::ATTRIB_SEMANTIC_CP_FLOAT,
                    "nodeOperatorTimeLagInputOutputCPUpdateFloat",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<float, MR::AttribDataFloat, MR::ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>,
                    logger);
      nodeDef->setInitNodeInstanceFnId(
                    "nodeOperatorTimeLagInputInitInstanceFloat",
                    MR::nodeOperatorTimeLagInputInitInstance<float, MR::AttribDataFloat, MR::ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>,
                    logger);     
      break;
    }
    case 2: // int
    {
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    0,
                    MR::ATTRIB_SEMANTIC_CP_INT,
                    "nodeOperatorTimeLagInputOutputCPUpdateInt",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<int32_t, MR::AttribDataInt, MR::ATTRIB_TYPE_TIME_LAG_INPUT_INT>,
                    logger);
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    1,
                    MR::ATTRIB_SEMANTIC_CP_INT,
                    "nodeOperatorTimeLagInputOutputCPUpdateInt",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<int32_t, MR::AttribDataInt, MR::ATTRIB_TYPE_TIME_LAG_INPUT_INT>,
                    logger);
      nodeDef->setInitNodeInstanceFnId(
                    "nodeOperatorTimeLagInputInitInstanceInt",
                    MR::nodeOperatorTimeLagInputInitInstance<int32_t, MR::AttribDataInt, MR::ATTRIB_TYPE_TIME_LAG_INPUT_INT>, 
                    logger);
      break;
    }
    case 3: // vector3
    {
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    0,
                    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
                    "nodeOperatorTimeLagInputOutputCPUpdateVector3",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<NMP::Vector3, MR::AttribDataVector3, MR::ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>,
                    logger);
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    1,
                    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
                    "nodeOperatorTimeLagInputOutputCPUpdateVector3",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<NMP::Vector3, MR::AttribDataVector3, MR::ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>,
                    logger);
      nodeDef->setInitNodeInstanceFnId(
                    "nodeOperatorTimeLagInputInitInstanceVector3",
                    MR::nodeOperatorTimeLagInputInitInstance<NMP::Vector3, MR::AttribDataVector3, MR::ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>,
                    logger);
      break;
    }
    case 4: // vector4
    {
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    0,
                    MR::ATTRIB_SEMANTIC_CP_VECTOR4,
                    "nodeOperatorTimeLagInputOutputCPUpdateVector4",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<NMP::Quat, MR::AttribDataVector4, MR::ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>,
                    logger);
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    1,
                    MR::ATTRIB_SEMANTIC_CP_VECTOR4,
                    "nodeOperatorTimeLagInputOutputCPUpdateVector4",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<NMP::Quat, MR::AttribDataVector4, MR::ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>,
                    logger);
      nodeDef->setInitNodeInstanceFnId(
                    "nodeOperatorTimeLagInputInitInstanceVector4",
                    MR::nodeOperatorTimeLagInputInitInstance<NMP::Quat, MR::AttribDataVector4, MR::ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>,
                    logger);
      break;
    }
    case 5: // bool
    {
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    0,
                    MR::ATTRIB_SEMANTIC_CP_BOOL,
                    "nodeOperatorTimeLagInputOutputCPUpdateBool",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<bool, MR::AttribDataBool, MR::ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>,
                    logger);
      nodeDef->setOutputCPTaskFnId(
                    outputCPTaskFns,
                    1,
                    MR::ATTRIB_SEMANTIC_CP_BOOL,
                    "nodeOperatorTimeLagInputOutputCPUpdateBool",
                    MR::nodeOperatorTimeLagInputOutputCPUpdate<bool, MR::AttribDataBool, MR::ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>,
                    logger);
      nodeDef->setInitNodeInstanceFnId(
                    "nodeOperatorTimeLagInputInitInstanceBool",
                    MR::nodeOperatorTimeLagInputInitInstance<bool, MR::AttribDataBool, MR::ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>,
                    logger);
      break;
    }
    default:
    {
      NMP_VERIFY_FAIL("Unrecognised type on time lag operator");
      break;
    }
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
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
