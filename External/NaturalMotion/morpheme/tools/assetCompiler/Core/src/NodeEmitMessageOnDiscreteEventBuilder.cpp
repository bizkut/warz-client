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
#include "NetworkDefBuilder.h"
#include "NodeEmitMessageOnDiscreteEventBuilder.h"
#include "morpheme/Nodes/mrNodeEmitMessageOnDiscreteEvent.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeEmitMessageOnDiscreteEventBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeEmitMessageOnDiscreteEventBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeEmitMessageOnDiscreteEventBuilder::getNodeDefInputConnections(
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
void NodeEmitMessageOnDiscreteEventBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeEmitMessageOnDiscreteEventBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Find how many messages have been specified.
  uint32_t numMessageSlots = 0xFFFFFFFF;
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  nodeDefDataBlock->readUInt(numMessageSlots, "NumMessageSlots");
  NMP_VERIFY(numMessageSlots < 33);

  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  uint32_t numChildren = 1;
  uint32_t numInputCPs = 0;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    numChildren,     // numChildren
    numInputCPs,     // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Allocate space for Def AttribData.
  if (numMessageSlots > 0)
  {
    result += MR::AttribDataIntArray::getMemoryRequirements(numMessageSlots);
    result += MR::AttribDataEmittedMessagesMap::getMemoryRequirements(numMessageSlots);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeEmitMessageOnDiscreteEventBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT, "Expecting node type NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT");
  
  //---------------------------
  // Find how many messages have been specified.
  uint32_t numMessageSlots = 0xFFFFFFFF;
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  nodeDefDataBlock->readUInt(numMessageSlots, "NumMessageSlots");
  NMP_VERIFY(numMessageSlots < 33);
    
  //---------------------------
  // Initialise the NodeDef itself
  uint8_t numOutputCPs = 1;   // Every node that can emit messages must have an extra output CP function 
                              //  that is called by the function Network::updateMessageEmitterNodes.
                              //  This function is designed primarily to allow a node to emit messages when it
                              //  has no control param or child/parent connections and hence has no update functions.
                              //  The registered function can be empty if the messages are actually sent elsewhere.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,              // numChildren
    1,              // max num active child nodes
    0,              // numInputCPConnections
    numOutputCPs,   // numOutputCPPins
    1,              // numAnimSetEntries                     
    nodeDefExport);

  //---------------------------
  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set the child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  nodeDef->setChildNodeID(0, sourceNodeID);

  if (numMessageSlots > 0)
  {
    //---------------------------
    // Initialise the corresponding event user IDs array.
    MR::AttribDataIntArray* eventUserIDsAttrib = MR::AttribDataIntArray::init(memRes, numMessageSlots, MR::IS_DEF_ATTRIB_DATA);

    //---------------------------
    // Initialise the emitted messages map attrib data.
    MR::AttribDataEmittedMessagesMap* emittedMessageMapAttribData =
      MR::AttribDataEmittedMessagesMap::init(memRes, numMessageSlots, MR::IS_DEF_ATTRIB_DATA);

    //---------------------------
    // Initialise each map entry and each event user ID.
    CHAR paramName[256];
    for(uint32_t i = 0; i < numMessageSlots; ++i)
    {
      sprintf_s(paramName, "EventUserData_%d", i);
      nodeDefDataBlock->readInt(eventUserIDsAttrib->m_values[i], paramName);
      initMessageMapEntry(i, emittedMessageMapAttribData, netDefExport, nodeDefDataBlock);
    }

    //---------------------------
    // Add message map to the nodes AttribData array.
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP,                                  // semantic,    
      0,                                                                         // animSetIndex,
      emittedMessageMapAttribData,                                               // attribData,  
      MR::AttribDataEmittedMessagesMap::getMemoryRequirements(numMessageSlots)); // attribMemReqs
   
    //---------------------------
    // Add event user data IDs to trigger on.
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                           // semantic,    
      0,                                                               // animSetIndex,
      eventUserIDsAttrib,                                              // attribData,  
      MR::AttribDataIntArray::getMemoryRequirements(numMessageSlots)); // attribMemReqs
  }

  // Initialise the nodedef.
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger(), numMessageSlots);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeEmitMessageOnDiscreteEventBuilder::initTaskQueuingFns(
  MR::NodeDef*               nodeDef,
  NetworkDefCompilationInfo* netDefCompilationInfo,
  NMP::BasicLogger*          logger,
  uint32_t                   numMessageSlots)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();


  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // Only try to emit messages if we have any messages to emit.
  if (numMessageSlots > 0)
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeEmitMessageOnDiscreteEventQueueSampledEventsBuffers), logger);
 
  // Every node that can emit messages must have an extra output CP function 
  //  that is called by the function Network::updateMessageEmitterNodes.
  //  This function is designed primarily to allow a node to emit messages when it
  //  has no control param or child/parent connections and hence has no update functions.
  //  In this case the function is empty as the messages are actually sent from a registered task.
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES, FN_NAME(nodeNullEmitMessageOutputCPUpdate), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildren), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
