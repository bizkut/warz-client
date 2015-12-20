//----------------------------------------------------------------------------------------------------------------------
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
#include "NodeOperatorOrientationInFreeFallBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorOrientationInFreeFall.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
  //----------------------------------------------------------------------------------------------------------------------
  bool NodeOperatorOrientationInFreeFallBuilder::hasSemanticLookupTable()
  {
    return true;
  }

  //----------------------------------------------------------------------------------------------------------------------
  MR::SemanticLookupTable* NodeOperatorOrientationInFreeFallBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
  {
    MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL);
    NMP_ASSERT(semanticLookupTable);

    semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

    return semanticLookupTable;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void NodeOperatorOrientationInFreeFallBuilder::getNodeDefInputConnections(
    std::vector<MR::NodeID>&    childNodeIDs,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             NMP_UNUSED(processor))
  {
    const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
    // Control parameter
    readDataPinChildNodeID(nodeDefDataBlock, "Orientation", childNodeIDs, true);
    readDataPinChildNodeID(nodeDefDataBlock, "TimeToImpact", childNodeIDs, true);
  }

  //----------------------------------------------------------------------------------------------------------------------
  void NodeOperatorOrientationInFreeFallBuilder::preInit(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             NMP_UNUSED(processor))
  {
    //---------------------------

    // Control parameter
    declareDataPin(netDefCompilationInfo, nodeDefExport, "Orientation", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
    declareDataPin(netDefCompilationInfo, nodeDefExport, "TimeToImpact", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  }

  //----------------------------------------------------------------------------------------------------------------------
  NMP::Memory::Format NodeOperatorOrientationInFreeFallBuilder::getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             NMP_UNUSED(processor))
  {
    // Allocate space for the NodeDef itself.
    // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
    NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
      netDefCompilationInfo,
      0,                                                        // numChildren
      ER::NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_MAX,  // numInputCPConnections
      1,                                                        // numAnimSetEntries
      nodeDefExport);

    // Space for my AttribData
    result += ER::AttribDataOperatorOrientationInFreeFallDef::getMemoryRequirements();

    return result;
  }

  //----------------------------------------------------------------------------------------------------------------------
  MR::NodeDef* NodeOperatorOrientationInFreeFallBuilder::init(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
    AssetProcessor*             processor)
  {
    NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL);
    const ME::DataBlockExport* nodeDefBlock = nodeDefExport->getDataBlock();

    //---------------------------
    // Initialise the NodeDef itself
    MR::NodeDef* nodeDef = initCoreNodeDef(
      memRes,
      netDefCompilationInfo,
      0,                                                        // numChildren
      0,
      ER::NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_MAX,  // numInputCPConnections
      ER::NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_MAX, // numOutputCPPins
      1,                                                        // numAnimSetEntries
      nodeDefExport);

    // Flag node as an operator.
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

    //---------------------------
    // Initialise the attrib datas.

    // Read in the node attributes
    ER::AttribDataOperatorOrientationInFreeFallDef* attribNodeDef =
      ER::AttribDataOperatorOrientationInFreeFallDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);

    nodeDefBlock->readFloat(attribNodeDef->m_startOrientationTime, "StartOrientationTime");
    nodeDefBlock->readFloat(attribNodeDef->m_stopOrientationTime, "StopOrientationTime");
    nodeDefBlock->readFloat(attribNodeDef->m_startOrientationTransitionTime, "StartOrientationTransitionTime");
    nodeDefBlock->readFloat(attribNodeDef->m_stopOrientationTransitionTime, "StopOrientationTransitionTime");
    nodeDefBlock->readFloat(attribNodeDef->m_weightOutputBeforeOrientation, "WeightOutputBeforeOrientation");
    nodeDefBlock->readFloat(attribNodeDef->m_weightOutputDuringOrientation, "WeightOutputDuringOrientation");
    nodeDefBlock->readFloat(attribNodeDef->m_weightOutputAfterOrientation,  "WeightOutputAfterOrientation");
    nodeDefBlock->readBool(attribNodeDef->m_startOrientationAtTimeBeforeImpact, "StartOrientationAtTimeBeforeImpact");
    nodeDefBlock->readBool(attribNodeDef->m_stopOrientationAtTimeBeforeImpact, "StopOrientationAtTimeBeforeImpact");

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                                    // semantic
      0,                                                                        // animSetIndex
      attribNodeDef,                                                            // attribData
      ER::AttribDataOperatorOrientationInFreeFallDef::getMemoryRequirements()); // attribMemReqs

    //---------------------------
    // Initialise the task function tables
    initTaskQueuingFns(
      nodeDef,
      netDefCompilationInfo,
      processor->getMessageLogger());

    return nodeDef;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void NodeOperatorOrientationInFreeFallBuilder::initTaskQueuingFns(
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
      ER::NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_ORIENTATION,
      MR::ATTRIB_SEMANTIC_CP_VECTOR3,
      ER_FN_NAME(nodeOperatorOrientationInFreeFallOutputCPUpdate),
      logger);
    nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
      ER::NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_WEIGHT,
      MR::ATTRIB_SEMANTIC_CP_FLOAT,
      ER_FN_NAME(nodeOperatorOrientationInFreeFallOutputCPUpdate),
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
    nodeDef->setInitNodeInstanceFnId(ER_FN_NAME(nodeOperatorOrientationInFreeFallInitInstance), logger);
  }

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
