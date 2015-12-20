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
#include "NetworkDefBuilder.h"
#include "NodeOperatorIntToFloatBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorIntToFloat.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

  //----------------------------------------------------------------------------------------------------------------------
  void NodeOperatorIntToFloatBuilder::getNodeDefInputConnections(
    std::vector<MR::NodeID>&    childNodeIDs,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             NMP_UNUSED(processor))
  {
    const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

    // Control parameters
    readDataPinChildNodeID(nodeDefDataBlock, "Int",childNodeIDs, true);
  }

  //----------------------------------------------------------------------------------------------------------------------
  void NodeOperatorIntToFloatBuilder::preInit(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             NMP_UNUSED(processor))
  {
    //---------------------------
    // Input Control Param Connections.
    declareDataPin(netDefCompilationInfo, nodeDefExport, "Int", 0, true,  MR::ATTRIB_SEMANTIC_CP_INT);
  }

  //----------------------------------------------------------------------------------------------------------------------
  NMP::Memory::Format NodeOperatorIntToFloatBuilder::getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             NMP_UNUSED(processor))
  {
    NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
      netDefCompilationInfo, 
      0,     // numChildren
      1,     // numInputCPConnections
      0,     // numAnimSetEntries
      nodeDefExport);
    return result;
  }

  //----------------------------------------------------------------------------------------------------------------------
  MR::NodeDef* NodeOperatorIntToFloatBuilder::init(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             processor)
  {
    // Get attrib data from connects XML output.
    NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_INT_TO_FLOAT);

    //---------------------------
    // Initialise the NodeDef itself
    MR::NodeDef* nodeDef = initCoreNodeDef(
      memRes,                  
      netDefCompilationInfo,   
      0,      // numChildren
      0,      // max num active child nodes
      1,      // numInputCPConnections
      1,      // numOutputCPPins
      0,      // numAnimSetEntries                     
      nodeDefExport);

    // Flag node as an operator.
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

    //---------------------------
    // Initialise the task function tables
    initTaskQueuingFns(
      nodeDef,
      netDefCompilationInfo,
      processor->getMessageLogger());

    return nodeDef;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void NodeOperatorIntToFloatBuilder::initTaskQueuingFns(
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

    nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_FLOAT, FN_NAME(nodeOperatorIntToFloatOutputCPUpdateFloat), logger);

    // Register the shared task function tables
    nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
    nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

    // Tidy up
    NMP::Memory::memFree(outputCPTaskFns);

    //------------------------------------
    // Other manager registered functions.
    nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
    nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceCreateFloatOutputAttributeInitPinAttrib), logger);
  }

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
