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
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// MR::NodeDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeDef::getMemoryRequirementsExcludeBaseMem(
  uint32_t numChildren,
  uint32_t numInputCPConnections,
  uint32_t numAttribDatas,
  uint32_t numOnEnterMessages,
  MR::DataPinInfo* nodeDataPinInfo/*=NULL*/)
{
  // Reserve some space for the child node IDs
  NMP::Memory::Format result(sizeof(NodeID) * numChildren, NMP_NATURAL_TYPE_ALIGNMENT);
 
  // Reserve space for the input control param connections.
  result += NMP::Memory::Format(sizeof(CPConnection) * numInputCPConnections, NMP_NATURAL_TYPE_ALIGNMENT);

  // Reserve some space for the attrib data entries
  result += NMP::Memory::Format(sizeof(AttribDataHandle) * numAttribDatas, NMP_NATURAL_TYPE_ALIGNMENT);

  if (nodeDataPinInfo && nodeDataPinInfo->m_reflexiveCPCount > 0)
  {
    // Reserve some space for the pin attrib data entries
    result += NMP::Memory::Format(sizeof(AttribDataHandle) * nodeDataPinInfo->m_numPinAttribData, NMP_NATURAL_TYPE_ALIGNMENT);

    // Reserve some space for the pin attrib data info entries
    result += NMP::Memory::Format(sizeof(PinAttribDataInfo) * nodeDataPinInfo->m_reflexiveCPCount, NMP_NATURAL_TYPE_ALIGNMENT);
  }

  // Reserve space for the event message structures
  result += NMP::Memory::Format(sizeof(NodeEventMessage) * numOnEnterMessages, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  // Include any (precomputed) space for pin attrib data
  if (nodeDataPinInfo && nodeDataPinInfo->m_reflexiveCPCount > 0)
  {
    result += nodeDataPinInfo->m_pinAtttribDataMemory;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeDef::getMemoryRequirements(
  uint32_t      numChildren,
  uint32_t      numInputCPConnections,
  uint32_t      numAttribDatas,
  uint32_t      numOnEnterMessages,
  DataPinInfo*  nodeDataPinInfo)
{
  NMP::Memory::Format result(sizeof(NodeDef), NMP_NATURAL_TYPE_ALIGNMENT);
  result += getMemoryRequirementsExcludeBaseMem(
                numChildren,
                numInputCPConnections,
                numAttribDatas,
                numOnEnterMessages,
                nodeDataPinInfo);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::initExcludeBaseMem(
  NodeDef*               target,
  NMP::Memory::Resource& resource,
  uint32_t               numChildren,
  uint32_t               numInputCPConnections,
  uint16_t               numAttribDatas,
  uint16_t               maxNumActiveChildNodes,
  uint8_t                numOutputCPPins,
  uint32_t               numOnEnterMessages,
  DataPinInfo*           nodeDataPinInfo)
{
  NMP_ASSERT(maxNumActiveChildNodes <= numChildren);

  target->m_numOnEnterMessages = (uint8_t)numOnEnterMessages;

  //---------------------------
  // Init header information
  relocateExcludeBaseMem(
    target,
    resource,
    numChildren,
    numInputCPConnections,
    numAttribDatas,
    nodeDataPinInfo);

  target->m_nodeTypeID = INVALID_NODE_TYPE_ID;
  target->m_nodeFlags = NODE_FLAG_IS_NONE;
  target->m_nodeID = INVALID_NODE_ID;
  target->m_parentNodeID = INVALID_NODE_ID;
  target->m_maxNumActiveChildNodes = maxNumActiveChildNodes;
  target->m_passThroughChildIndex = 0;
  // total output pins as specified plus reflexive
  target->m_numOutputCPPins = numOutputCPPins + (uint8_t)nodeDataPinInfo->m_reflexiveCPCount;
  target->m_numReflexiveCPPins = nodeDataPinInfo->m_reflexiveCPCount;
  target->m_numPinAttribDataHandles = (uint16_t)nodeDataPinInfo->m_numPinAttribData;
  target->m_lastPinAttribDataOffset = 0;
  target->m_lastPinAttribDataIndex = 0;
  target->m_numAttribDataHandles = numAttribDatas;

  //---------------------------
  // Init and wipe child node IDs.
  if (numChildren > 0)
  {
    for (uint32_t i = 0; i < numChildren; ++i)
      target->m_childNodeIDs[i] = INVALID_NODE_ID;
  }

  //---------------------------
  if (numInputCPConnections > 0)
  {
    // Init and wipe the input control param connections.
    for (uint32_t i = 0; i < numInputCPConnections; ++i)
      target->m_inputCPConnections[i].set(INVALID_NODE_ID, INVALID_PIN_INDEX);
  }

  //---------------------------
  if (numAttribDatas > 0)
  {
    // Init and wipe the AttribDataHandles
    for (uint32_t i = 0; i < numAttribDatas; ++i)
      target->m_nodeAttribDataHandles[i].set(NULL, NMP::Memory::Format(0, NMP_NATURAL_TYPE_ALIGNMENT));
  }

  //---------------------------
  if (nodeDataPinInfo->m_reflexiveCPCount > 0)
  {
    // Init and wipe the pin AttribDataHandles
    for (uint32_t i = 0; i < nodeDataPinInfo->m_numPinAttribData; ++i)
      target->m_nodePinAttribDataHandles[i].set(NULL, NMP::Memory::Format(0, NMP_NATURAL_TYPE_ALIGNMENT));

    // Init the pin attribData infos, set these pin indexes to be after regular output pins.
    for (uint32_t i = 0; i < nodeDataPinInfo->m_reflexiveCPCount; ++i)
      target->m_nodePinAttribDataInfo[i].init((MR::PinIndex)(numOutputCPPins + i));
  }

  //---------------------------
  // Init and wipe down event messages.
  if (numOnEnterMessages > 0)
  {
    for (uint32_t i = 0; i < numOnEnterMessages; ++i)
    {
      target->m_onEnterMessages[i].m_external = false;
      target->m_onEnterMessages[i].m_msgID = 0;
    }
  }

  //---------------------------
  // Task functions
  target->m_deleteNodeInstanceFn = NULL;
  target->m_updateNodeConnectionsFn = NULL;
  target->m_findGeneratingNodeForSemanticFn = NULL;
  target->m_initNodeInstanceFn = NULL;
  target->m_messageHandlerFn = NULL;

  target->m_taskQueuingFnsID = 0xFFFF;
  target->m_outputCPTasksID = 0xFFFF;
  target->m_taskQueuingFns = NULL;
  target->m_outputCPTasks = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::relocateExcludeBaseMem(
  NodeDef*               target,
  NMP::Memory::Resource& resource,
  uint32_t               numChildren,
  uint32_t               numInputCPConnections,
  uint32_t               numAttribDatas,
  MR::DataPinInfo*       nodeDataPinInfo)
{
  //---------------------------
  // Init header information
  target->m_owningNetworkDef = NULL;
  target->m_childNodeIDs = NULL;
  target->m_inputCPConnections = NULL;
  target->m_nodeAttribDataHandles = NULL;
  target->m_nodePinAttribDataHandles = NULL;
  target->m_nodePinAttribDataInfo = NULL;

  //---------------------------
  // Init child node IDs.
  target->m_numChildNodeIDs = (uint16_t)numChildren;
  if (numChildren > 0)
  {
    target->m_childNodeIDs = (NodeID*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(NodeID) * numChildren, NMP_NATURAL_TYPE_ALIGNMENT));
  }

  //---------------------------
  target->m_numInputCPConnections = (uint8_t)numInputCPConnections;
  if (numInputCPConnections > 0)
  {
    // Init input control param connections.
    target->m_inputCPConnections = (CPConnection*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(CPConnection) * numInputCPConnections, NMP_NATURAL_TYPE_ALIGNMENT));
  }

  //---------------------------
  // Attrib data entries
  if (numAttribDatas > 0)
  {
    target->m_nodeAttribDataHandles = (AttribDataHandle*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(AttribDataHandle) * numAttribDatas, NMP_NATURAL_TYPE_ALIGNMENT));
  }

  //---------------------------
  // Pin attrib data entries
  if (nodeDataPinInfo->m_reflexiveCPCount > 0)
  {
    target->m_nodePinAttribDataHandles = (AttribDataHandle*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(AttribDataHandle) * nodeDataPinInfo->m_numPinAttribData, NMP_NATURAL_TYPE_ALIGNMENT));
    target->m_nodePinAttribDataInfo = (PinAttribDataInfo*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(PinAttribDataInfo) * nodeDataPinInfo->m_reflexiveCPCount, NMP_NATURAL_TYPE_ALIGNMENT));
  }

  //---------------------------
  // OnEnter messages
  if (target->m_numOnEnterMessages > 0)
  {
    target->m_onEnterMessages = (NodeEventMessage*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(NodeEventMessage) * target->m_numOnEnterMessages, NMP_NATURAL_TYPE_ALIGNMENT));
  }
  else
  {
    target->m_onEnterMessages = NULL;
  }

  // Multiple of the NodeDef alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
NodeDef* NodeDef::init(
  NMP::Memory::Resource& resource,
  uint32_t               numChildren,
  uint32_t               numInputCPConnections,
  uint16_t               numAttribDatas,  
  uint16_t               maxNumActiveChildNodes,
  uint8_t                numOutputCPPins,
  uint32_t               numOnEnterMessages,
  DataPinInfo*           nodeDataPinInfo)
{
  NodeDef* result = (NodeDef*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(NodeDef), NMP_NATURAL_TYPE_ALIGNMENT));

  initExcludeBaseMem(
    result,
    resource,
    numChildren,
    numInputCPConnections,
    numAttribDatas,
    maxNumActiveChildNodes,
    numOutputCPPins,
    numOnEnterMessages,
    nodeDataPinInfo);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::locate(NetworkDef* owningNetworkDef)
{
  size_t fnID;
  MR::Manager& manager = MR::Manager::getInstance();

  //---------------------------
  // Header information
  NMP::endianSwap(m_nodeID);
  NMP::endianSwap(m_parentNodeID);
  NMP::endianSwap(m_nodeTypeID);
  NMP::endianSwap(m_nodeFlags);
  NMP::endianSwap(m_passThroughChildIndex);

  // Set the owning network definition from the input parameter. Note we do not
  // refix the stored pointer. This allows us to assemble a new network definition
  // from the individual node defs during asset compilation.
  m_owningNetworkDef = owningNetworkDef;

  //---------------------------
  // Child node IDs.
  NMP::endianSwap(m_maxNumActiveChildNodes);
  NMP::endianSwap(m_numChildNodeIDs);
  if (m_childNodeIDs != NULL)
  {
    REFIX_SWAP_PTR(NodeID, m_childNodeIDs);
    for (uint32_t i = 0; i < m_numChildNodeIDs; ++i)
    {
      NMP::endianSwap(m_childNodeIDs[i]);
    }
  }

  //---------------------------
  // Input control param connections and output pins count.
  NMP::endianSwap(m_numInputCPConnections);
  if (m_inputCPConnections != NULL)
  {
    // Input control param connections.
    REFIX_SWAP_PTR(CPConnection, m_inputCPConnections);
    for (uint32_t i = 0; i < m_numInputCPConnections; ++i)
    {
      m_inputCPConnections[i].endianSwap();
    }
  }
  NMP::endianSwap(m_numOutputCPPins);

  //---------------------------
  // PinAttribData data. 
  NMP::endianSwap(m_numReflexiveCPPins);
  if (m_numReflexiveCPPins != 0)
  {
    // reflexive CP pin connections.
    REFIX_SWAP_PTR(PinAttribDataInfo, m_nodePinAttribDataInfo);
    for (uint32_t i = 0; i < m_numReflexiveCPPins; ++i)
    {
      m_nodePinAttribDataInfo[i].endianSwap();
    }
  }
  NMP::endianSwap(m_numPinAttribDataHandles);
  NMP::endianSwap(m_lastPinAttribDataOffset);
  NMP::endianSwap(m_lastPinAttribDataIndex);

  if (m_numPinAttribDataHandles != 0)
  {
    REFIX_SWAP_PTR(AttribDataHandle, m_nodePinAttribDataHandles);
    for (uint16_t i = 0; i < m_numPinAttribDataHandles; ++i)
    {
      m_nodePinAttribDataHandles[i].endianSwap();
      if (m_nodePinAttribDataHandles[i].m_attribData)
      {
        REFIX_PTR(AttribData, m_nodePinAttribDataHandles[i].m_attribData);

        // Locate the attrib data itself
        AttribDataType type = m_nodePinAttribDataHandles[i].m_attribData->getTypeUnlocated();

        MR::AttribLocateFn locateFn = manager.getAttribLocateFn(type);
        NMP_ASSERT(locateFn);
        locateFn(m_nodePinAttribDataHandles[i].m_attribData);

        NMP_ASSERT_MSG(m_nodePinAttribDataHandles[i].m_attribData->getRefCount() == MR::IS_DEF_ATTRIB_DATA,
          "Invalid ref count in node[%i] def data.  Make sure the ref count is set to MR::IS_DEF_ATTRIB_DATA in the "
          "asset compiler.", m_nodeID);
        NMP_ASSERT(m_nodePinAttribDataHandles[i].m_attribData->m_allocator == NULL);
      }
    }
  }

  //---------------------------
  // AttribData data. 
  NMP::endianSwap(m_numAttribDataHandles);
  if (m_nodeAttribDataHandles != NULL)
  {
    REFIX_SWAP_PTR(AttribDataHandle, m_nodeAttribDataHandles);
    for (uint16_t i = 0; i < m_numAttribDataHandles; ++i)
    {
      m_nodeAttribDataHandles[i].endianSwap();
      if (m_nodeAttribDataHandles[i].m_attribData)
      {
        REFIX_PTR(AttribData, m_nodeAttribDataHandles[i].m_attribData);

        // Locate the attrib data itself
        AttribDataType type = m_nodeAttribDataHandles[i].m_attribData->getTypeUnlocated();

        MR::AttribLocateFn locateFn = manager.getAttribLocateFn(type);
        NMP_ASSERT(locateFn);
        locateFn(m_nodeAttribDataHandles[i].m_attribData);

        NMP_ASSERT_MSG(m_nodeAttribDataHandles[i].m_attribData->getRefCount() == MR::IS_DEF_ATTRIB_DATA,
          "Invalid ref count in node[%i] def data.  Make sure the ref count is set to MR::IS_DEF_ATTRIB_DATA in the "
          "asset compiler.", m_nodeID);
        NMP_ASSERT(m_nodeAttribDataHandles[i].m_attribData->m_allocator == NULL);
      }
    }
  }

  //---------------------------
  // Locate the message handler function.
  if (m_messageHandlerFn)
  {
    NMP::endianSwap(m_messageHandlerFn);
    m_messageHandlerFn = manager.getMessageHandlerFn((uint32_t)(ptrdiff_t)m_messageHandlerFn);
  }

  //---------------------------
  // Locate the init function.
  if (m_initNodeInstanceFn)
  {
    NMP::endianSwap(m_initNodeInstanceFn);
    fnID = (size_t) m_initNodeInstanceFn;
    m_initNodeInstanceFn = manager.getInitNodeInstanceFn((uint32_t) fnID);
  }

  //---------------------------
  // Locate the delete function.
  NMP_ASSERT(m_deleteNodeInstanceFn);
  NMP::endianSwap(m_deleteNodeInstanceFn);
  fnID = (size_t) m_deleteNodeInstanceFn;
  m_deleteNodeInstanceFn = manager.getDeleteNodeInstanceFn((uint32_t) fnID);

  //---------------------------
  // Locate the update connections function.
  NMP_ASSERT(m_updateNodeConnectionsFn);
  NMP::endianSwap(m_updateNodeConnectionsFn);
  fnID = (size_t) m_updateNodeConnectionsFn;
  m_updateNodeConnectionsFn = manager.getUpdateNodeConnectionsFn((uint32_t) fnID);

  //---------------------------
  // Locate the get generatesAttribSemanticType function.
  if (m_findGeneratingNodeForSemanticFn)
  {
    NMP::endianSwap(m_findGeneratingNodeForSemanticFn);
    fnID = (size_t) m_findGeneratingNodeForSemanticFn;
    m_findGeneratingNodeForSemanticFn = manager.getFindGeneratingNodeForSemanticFn((uint32_t) fnID);
  }

  //---------------------------
  // Locate the shared function tables
  NMP::endianSwap(m_taskQueuingFnsID);
  NMP::endianSwap(m_outputCPTasksID);
  if (m_owningNetworkDef)
  {
    const SharedTaskFnTables* taskQueuingFnTables = m_owningNetworkDef->getTaskQueuingFnTables();
    NMP_ASSERT(taskQueuingFnTables);
    m_taskQueuingFns = (QueueAttrTaskFn*)taskQueuingFnTables->getTaskFnTable(m_taskQueuingFnsID);

    const SharedTaskFnTables* outputCPTaskFnTables = m_owningNetworkDef->getOutputCPTaskFnTables();
    NMP_ASSERT(outputCPTaskFnTables);
    m_outputCPTasks = (OutputCPTask*)outputCPTaskFnTables->getTaskFnTable(m_outputCPTasksID);

    // Find the semantic lookup table for faster access.
    m_semanticLookupTable = m_owningNetworkDef->findSemanticLookupTable(m_nodeTypeID);
  }

  //---------------------------
  // Locate the event lists
  if(m_onEnterMessages)
  {
    NMP::endianSwap(m_numOnEnterMessages);
    REFIX_SWAP_PTR(NodeEventMessage, m_onEnterMessages);
    for(uint32_t i = 0; i < m_numOnEnterMessages; i++)
    {
      // We don't need to swap m_onEnterMessages->m_external as it's a bool
      NMP::endianSwap(m_onEnterMessages[i].m_msgID);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::dislocate()
{
  MR::Manager& manager = MR::Manager::getInstance();

  //---------------------------
  // Task function tables
  NMP::endianSwap(m_taskQueuingFnsID);
  NMP::endianSwap(m_outputCPTasksID);
  m_taskQueuingFns = NULL;
  m_outputCPTasks = NULL;

  //---------------------------
  // Dislocate the get generatesAttribSemanticType function.
  if (m_findGeneratingNodeForSemanticFn)
  {
    m_findGeneratingNodeForSemanticFn = (FindGeneratingNodeForSemanticFn)(size_t)manager.getFindGeneratingNodeForSemanticFnID(m_findGeneratingNodeForSemanticFn);
    NMP::endianSwap(m_findGeneratingNodeForSemanticFn);
  }

  //---------------------------
  // Dislocate the update connections function.
  if (m_updateNodeConnectionsFn)
  {
    m_updateNodeConnectionsFn = (UpdateNodeConnections)(size_t)manager.getUpdateNodeConnectionsFnID(m_updateNodeConnectionsFn);
    NMP::endianSwap(m_updateNodeConnectionsFn);
  }

  //---------------------------
  // Dislocate the delete function. NOTE: not all nodes need a delete operator because once created they are permanent
  // until the network instance is destroyed. e.g control parameter node.
  if (m_deleteNodeInstanceFn)
  {
    m_deleteNodeInstanceFn = (DeleteNodeInstance)(size_t)manager.getDeleteNodeInstanceFnID(m_deleteNodeInstanceFn);
    NMP::endianSwap(m_deleteNodeInstanceFn);
  }

  //---------------------------
  // Dislocate the init function.
  if (m_initNodeInstanceFn)
  {
    m_initNodeInstanceFn = (InitNodeInstance)(size_t)manager.getInitNodeInstanceFnID(m_initNodeInstanceFn);
    NMP::endianSwap(m_initNodeInstanceFn);
  }

  //---------------------------
  // Dislocate the message handler function
  if (m_messageHandlerFn)
  {
    m_messageHandlerFn = (MessageHandlerFn)(size_t)manager.getMessageHandlerFnID(m_messageHandlerFn);
    NMP::endianSwap(m_messageHandlerFn);
  }

  //---------------------------
  // Attrib data
  if (m_nodeAttribDataHandles != NULL)
  {
    for (uint16_t i = 0; i < m_numAttribDataHandles; ++i)
    {
      if (m_nodeAttribDataHandles[i].m_attribData)
      {
        // Dislocate the attrib data itself
        AttribDataType type = m_nodeAttribDataHandles[i].m_attribData->getType();

        MR::AttribDislocateFn dislocateFn = manager.getAttribDislocateFn(type);
        NMP_ASSERT(dislocateFn);
        dislocateFn(m_nodeAttribDataHandles[i].m_attribData);

        UNFIX_PTR(AttribData, m_nodeAttribDataHandles[i].m_attribData);          
      }
      m_nodeAttribDataHandles[i].endianSwap();
    }
    UNFIX_SWAP_PTR(AttribDataHandle, m_nodeAttribDataHandles);
  }
  NMP::endianSwap(m_numAttribDataHandles);


  //---------------------------
  // PinAttribData data. 
  NMP::endianSwap(m_lastPinAttribDataIndex);
  NMP::endianSwap(m_lastPinAttribDataOffset);

  if (m_numPinAttribDataHandles != 0)
  {
    for (uint16_t i = 0; i < m_numPinAttribDataHandles; ++i)
    {
      if (m_nodePinAttribDataHandles[i].m_attribData)
      {
        // Locate the attrib data itself
        AttribDataType type = m_nodePinAttribDataHandles[i].m_attribData->getType();

        MR::AttribDislocateFn dislocateFn = manager.getAttribDislocateFn(type);
        NMP_ASSERT(dislocateFn);
        dislocateFn(m_nodePinAttribDataHandles[i].m_attribData);

        UNFIX_PTR(AttribData, m_nodePinAttribDataHandles[i].m_attribData);
      }
      m_nodePinAttribDataHandles[i].endianSwap();
    }
    UNFIX_SWAP_PTR(AttribDataHandle, m_nodePinAttribDataHandles);
  }
  NMP::endianSwap(m_numPinAttribDataHandles);

  if (m_numReflexiveCPPins != 0)
  {
    // reflexive CP pin connections.
    for (uint32_t i = 0; i < m_numReflexiveCPPins; ++i)
    {
      m_nodePinAttribDataInfo[i].endianSwap();
    }
    UNFIX_SWAP_PTR(PinAttribDataInfo, m_nodePinAttribDataInfo);
  }
  NMP::endianSwap(m_numReflexiveCPPins);  

  //---------------------------
  // Input control param connections and output pins count.
  NMP::endianSwap(m_numOutputCPPins);
  if (m_inputCPConnections != NULL)
  {
    for (uint32_t i = 0; i < m_numInputCPConnections; ++i)
    {
      m_inputCPConnections[i].endianSwap();
    }
    UNFIX_SWAP_PTR(CPConnection, m_inputCPConnections);
  }
  NMP::endianSwap(m_numInputCPConnections);
  

  //---------------------------
  // Child node IDs.
  if (m_childNodeIDs != NULL)
  {
    for (uint32_t i = 0; i < m_numChildNodeIDs; ++i)
    {
      NMP::endianSwap(m_childNodeIDs[i]);
    }
    UNFIX_SWAP_PTR(NodeID, m_childNodeIDs);
  }
  NMP::endianSwap(m_numChildNodeIDs);
  NMP::endianSwap(m_maxNumActiveChildNodes);

  //---------------------------
  // Header information
  NMP::endianSwap(m_nodeID);
  NMP::endianSwap(m_parentNodeID);
  NMP::endianSwap(m_nodeTypeID);
  NMP::endianSwap(m_nodeFlags);
  NMP::endianSwap(m_passThroughChildIndex);

  // We do not unfix the owning node pointer, in order to be easily able to
  // assemble a complete network definition from the node def parts.
  m_owningNetworkDef = NULL;

  // There is no need to worry about the semantic lookup table as it's going to be fixed using the node type.
  m_semanticLookupTable = NULL;

  // Dislocate the event lists
  if(m_onEnterMessages)
  {
    for(uint32_t i = 0; i < m_numOnEnterMessages; i++)
    {
      // We don't need to swap m_onEnterMessages[i].m_external as it's a bool
      NMP::endianSwap(m_onEnterMessages[i].m_msgID);
    }
    UNFIX_SWAP_PTR(NodeEventMessage, m_onEnterMessages);
    NMP::endianSwap(m_numOnEnterMessages);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::registerEmptyTaskQueuingFns(SharedTaskFnTables* taskQueuingFnTables)
{
  NMP_ASSERT(taskQueuingFnTables);
  m_taskQueuingFnsID = (uint16_t)taskQueuingFnTables->registerEmptyTaskFnTable();
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::registerTaskQueuingFns(
  SharedTaskFnTables*    taskQueuingFnTables,
  const QueueAttrTaskFn* taskQueuingFns)
{
  NMP_ASSERT(taskQueuingFnTables);
  NMP_ASSERT(taskQueuingFns);
  m_taskQueuingFnsID = (uint16_t)taskQueuingFnTables->registerTaskFnTable((const SharedTaskFnTables::SharedTaskFn*)taskQueuingFns);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::registerEmptyOutputCPTasks(SharedTaskFnTables* outputCPTaskFnTables)
{
  NMP_ASSERT(outputCPTaskFnTables);
  m_outputCPTasksID = (uint16_t)outputCPTaskFnTables->registerEmptyTaskFnTable();
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::registerOutputCPTasks(
  SharedTaskFnTables* outputCPTaskFnTables,
  const OutputCPTask* outputCPTaskFns)
{
  NMP_ASSERT(outputCPTaskFnTables);
  NMP_ASSERT(outputCPTaskFns);
  m_outputCPTasksID = (uint16_t)outputCPTaskFnTables->registerTaskFnTable((const SharedTaskFnTables::SharedTaskFn*)outputCPTaskFns);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setTaskQueuingFnId(
  QueueAttrTaskFn*  taskQueuingFns,
  uint32_t          semantic,
  const char*       fnName,
  QueueAttrTaskFn   NMP_UNUSED(queuingFn),
  NMP::BasicLogger* logger)
{
  NMP_ASSERT(taskQueuingFns);

#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  NMP_ASSERT(semantic < numEntries);
#endif // NMP_ENABLE_ASSERTS

  // We store the id of the function directly. In the dislocate stage, there's no need to look it up
  // again.
  if (!fnName)
  {
    taskQueuingFns[semantic] = NULL;
    return;
  }
  uint32_t fnId = MR::Manager::getInstance().getTaskQueuingFnID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("ERROR: Invalid Task Queuing function %s in node %i. Please check if the function was registered properly.\n", fnName, this->getNodeID());
  }
  taskQueuingFns[semantic] = MR::Manager::getInstance().getTaskQueuingFn(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setOutputCPTaskFnId(
  OutputCPTask*     outputCPTaskFns,
  PinIndex          outputCPPinIndex,
  uint32_t          NMP_USED_FOR_ASSERTS(semantic),
  const char*       fnName,
  OutputCPTask      NMP_UNUSED(outputCPTask),
  NMP::BasicLogger* logger)
{
  NMP_ASSERT(semantic < MR::Manager::getInstance().getNumRegisteredAttribSemantics());

  // We store the id of the function directly. In the dislocate stage, there's no need to look it up
  // again.
  if (!fnName)
  {
    outputCPTaskFns[outputCPPinIndex] = NULL;
    return;
  }
  uint32_t fnId = MR::Manager::getInstance().getOutputCPTaskID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("Invalid output control param Task function %s in node %i, pin %d. \n", fnName, this->getNodeID(), outputCPPinIndex);
  }
  outputCPTaskFns[outputCPPinIndex] = MR::Manager::getInstance().getOutputCPTask(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setDeleteNodeInstanceId(
  const char*        fnName,
  DeleteNodeInstance NMP_UNUSED(deleteFn),
  NMP::BasicLogger*  logger)
{
  if (!fnName)
  {
    m_deleteNodeInstanceFn = NULL;    
    return;
  }

  uint32_t fnId = MR::Manager::getInstance().getDeleteNodeInstanceFnID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("Invalid Delete Node Instance function %s in node %i. \n", fnName, this->getNodeID());
  }
  m_deleteNodeInstanceFn = MR::Manager::getInstance().getDeleteNodeInstanceFn(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setUpdateConnectionsFnId(
  const char*           fnName, 
  UpdateNodeConnections NMP_UNUSED(updateNodeConnectionsFn),
  NMP::BasicLogger*     logger)
{
  if (!fnName)
  {
    m_updateNodeConnectionsFn = NULL;
    return;
  }

  uint32_t fnId = MR::Manager::getInstance().getUpdateNodeConnectionsFnID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("Invalid Update Connections function %s in node %i. \n", fnName, this->getNodeID());
  }
  m_updateNodeConnectionsFn = MR::Manager::getInstance().getUpdateNodeConnectionsFn(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setFindGeneratingNodeForSemanticFnId(
  const char*                     fnName,
  FindGeneratingNodeForSemanticFn NMP_UNUSED(generatesAttribSemanticTypeFn),
  NMP::BasicLogger*               logger)
{
  NMP_ASSERT_MSG(fnName, "Queuing Function is required!");

  uint32_t fnId = MR::Manager::getInstance().getFindGeneratingNodeForSemanticFnID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("Invalid Has Queuing Function For Semantic function %s in node %i. \n", fnName, this->getNodeID());
  }
  m_findGeneratingNodeForSemanticFn = MR::Manager::getInstance().getFindGeneratingNodeForSemanticFn(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setInitNodeInstanceFnId(
  const char*       fnName,
  InitNodeInstance  NMP_UNUSED(initNodeFn),
  NMP::BasicLogger* logger)
{
  if (!fnName)
  {
    m_initNodeInstanceFn = NULL;
    return;
  }

  uint32_t fnId = MR::Manager::getInstance().getInitNodeInstanceFnID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("Invalid Init Node Instance function in node %i %s in node %i. \n", fnName, this->getNodeID());
  }
  m_initNodeInstanceFn = MR::Manager::getInstance().getInitNodeInstanceFn(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::setMessageHandlerFnId(
  const char*       fnName,
  MessageHandlerFn  NMP_UNUSED(messageHandlerFn),
  NMP::BasicLogger* logger)
{
  if (!fnName)
  {
    m_messageHandlerFn = NULL;
    return;
  }

  uint32_t fnId = MR::Manager::getInstance().getMessageHandlerFnID(fnName);

  // If this assert is hit, the function is probably not registered properly.
  // Check the calling node builder for typos, or the asset compiler plugin for correct
  // function initialization.
  NMP_ASSERT(fnId != INVALID_FUNCTION_ID);
  if (fnId == INVALID_FUNCTION_ID && logger != 0)
  {
    logger->output("Invalid Message Handler function in node %i %s in node %i. \n", fnName, this->getNodeID());
  }
  m_messageHandlerFn = MR::Manager::getInstance().getMessageHandlerFn(fnId);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDef::deleteNodeInstance(Network* net)
{
  NMP_ASSERT(net && m_deleteNodeInstanceFn);
  // This is not a runtime referenced node.
  m_deleteNodeInstanceFn(this, net);
}

//----------------------------------------------------------------------------------------------------------------------
// SemanticLookupTables functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SemanticLookupTable::getSemanticLookupTablesMemoryRequirements(uint32_t numSemanticLookupTables)
{
  // Table pointers.
  NMP::Memory::Format result(sizeof(MR::SemanticLookupTable*) * numSemanticLookupTables, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for tables themselves.
  NMP::Memory::Format tableReqs = MR::SemanticLookupTable::getMemoryRequirements();
  tableReqs.size *= numSemanticLookupTables;
  result += tableReqs;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SemanticLookupTable** SemanticLookupTable::initSemanticLookupTables(
  NMP::Memory::Resource& resource,
  uint32_t numSemanticLookupTables)
{
  // Table pointers.
  NMP::Memory::Format memReqsPtrs(sizeof(MR::SemanticLookupTable*) * numSemanticLookupTables, NMP_NATURAL_TYPE_ALIGNMENT);
  SemanticLookupTable** result = (SemanticLookupTable**)resource.alignAndIncrement(memReqsPtrs);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// SemanticLookupTable functions.
//----------------------------------------------------------------------------------------------------------------------
SemanticLookupTable* SemanticLookupTable::init(
  NMP::Memory::Resource& resource,
  NodeType               nodeType)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(SemanticLookupTable), NMP_NATURAL_TYPE_ALIGNMENT);
  SemanticLookupTable* result = (SemanticLookupTable*)resource.alignAndIncrement(memReqsHdr);
  result->m_nodeType = nodeType;
  result->m_numSemantics = (uint16_t) Manager::getInstance().getNumRegisteredAttribSemantics();
  result->m_numAttribsPerAnimSet = 0;

  // Table data
  NMP::Memory::Format memReqsTable(
    sizeof(uint8_t) * Manager::getInstance().getNumRegisteredAttribSemantics(),
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_semanticLookup = (uint8_t*)resource.alignAndIncrement(memReqsTable);
  memset(result->m_semanticLookup, INVALID_LOOKUP_INDEX, result->m_numSemantics);

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SemanticLookupTable* SemanticLookupTable::initFrom(
  NMP::Memory::Resource&     resource,
  SemanticLookupTable* const sourceTable)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(SemanticLookupTable), NMP_NATURAL_TYPE_ALIGNMENT);
  SemanticLookupTable* result = (SemanticLookupTable*)resource.alignAndIncrement(memReqsHdr);
  memcpy(result, sourceTable, sizeof(SemanticLookupTable));

  // Table data
  NMP::Memory::Format memReqsTable(
    sizeof(uint8_t) * sourceTable->m_numSemantics,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_semanticLookup = (uint8_t*)resource.alignAndIncrement(memReqsTable);
  memcpy(result->m_semanticLookup, sourceTable->m_semanticLookup, memReqsTable.size);

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SemanticLookupTable::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(SemanticLookupTable), NMP_NATURAL_TYPE_ALIGNMENT);

  // Table data
  NMP::Memory::Format memReqsTable(
    sizeof(uint8_t) * Manager::getInstance().getNumRegisteredAttribSemantics(),
    NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsTable;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SemanticLookupTable::locate()
{
  NMP::endianSwap(m_nodeType);
  NMP::endianSwap(m_numSemantics);
  NMP::endianSwap(m_numAttribsPerAnimSet);
  REFIX_SWAP_PTR(uint8_t, m_semanticLookup);
  NMP::endianSwapArray(m_semanticLookup, m_numSemantics, sizeof(uint8_t));
}

//----------------------------------------------------------------------------------------------------------------------
void SemanticLookupTable::dislocate()
{
  NMP::endianSwapArray(m_semanticLookup, m_numAttribsPerAnimSet, sizeof(uint8_t));
  UNFIX_SWAP_PTR(uint8_t, m_semanticLookup);
  NMP::endianSwap(m_nodeType);
  NMP::endianSwap(m_numSemantics);
  NMP::endianSwap(m_numAttribsPerAnimSet);
}

//----------------------------------------------------------------------------------------------------------------------
void SemanticLookupTable::initFrom(const SemanticLookupTable* const sourceTable)
{
  NMP_IS_ALIGNED(this, NMP_NATURAL_TYPE_ALIGNMENT);
  void* ptr = this;

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(SemanticLookupTable), NMP_NATURAL_TYPE_ALIGNMENT);
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);
  m_nodeType = sourceTable->m_nodeType;
  m_numSemantics = sourceTable->m_numSemantics;
  m_numAttribsPerAnimSet = sourceTable->m_numAttribsPerAnimSet;

  // Table data
  NMP::Memory::Format memReqsTable(
    sizeof(uint8_t) * sourceTable->m_numSemantics,
    NMP_NATURAL_TYPE_ALIGNMENT);
  m_semanticLookup = (uint8_t*)NMP::Memory::alignAndIncrement(ptr, memReqsTable);
  memcpy(m_semanticLookup, sourceTable->m_semanticLookup, memReqsTable.size);
}

//----------------------------------------------------------------------------------------------------------------------
void SemanticLookupTable::setLookupIndex(AttribDataSemantic semantic, uint8_t lookupIndex)
{
  NMP_ASSERT(lookupIndex < m_numSemantics);
  NMP_ASSERT(m_semanticLookup[semantic] == INVALID_LOOKUP_INDEX); // Make sure the lookup for this semantic has not already been set.
  m_semanticLookup[semantic] = lookupIndex;
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t SemanticLookupTable::addLookupIndex(AttribDataSemantic semantic)
{
  NMP_ASSERT(semantic < m_numSemantics);
  NMP_ASSERT(m_numAttribsPerAnimSet < m_numSemantics);
  NMP_ASSERT(m_numAttribsPerAnimSet < INVALID_LOOKUP_INDEX);
  NMP_ASSERT(m_semanticLookup[semantic] == INVALID_LOOKUP_INDEX); // Make sure the lookup for this semantic has not already been set.
  m_semanticLookup[semantic] = m_numAttribsPerAnimSet;
  ++m_numAttribsPerAnimSet;
  return m_numAttribsPerAnimSet - 1;
}

//----------------------------------------------------------------------------------------------------------------------
// MR::SharedTaskFnTables functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SharedTaskFnTables::getMemoryRequirementsExcludeData(uint32_t numTaskFnTables)
{
  // Header
  NMP::Memory::Format result(sizeof(SharedTaskFnTables), NMP_NATURAL_TYPE_ALIGNMENT);
  // Task function table pointers
  result += NMP::Memory::Format(sizeof(SharedTaskFn*) * numTaskFnTables, NMP_NATURAL_TYPE_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SharedTaskFnTables* SharedTaskFnTables::initExcludeData(uint32_t numTaskFnTables)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(SharedTaskFnTables), NMP_NATURAL_TYPE_ALIGNMENT);
  // Task function table pointers
  NMP::Memory::Format memReqs = memReqsHdr;
  memReqs += NMP::Memory::Format(sizeof(SharedTaskFn*) * numTaskFnTables, NMP_NATURAL_TYPE_ALIGNMENT);

  // Allocate the memory for the shared task function tables
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  // Initialise the data
  SharedTaskFnTables* result = (SharedTaskFnTables*)memRes.ptr;
  memRes.increment(memReqsHdr);
  result->m_taskFnTables = (SharedTaskFn**)memRes.ptr;

  // Clear the tables
  for (uint32_t i = 0; i < numTaskFnTables; ++i)
    result->m_taskFnTables[i] = NULL;
  result->m_numTaskFnTables = 0;
  result->m_numEntriesPerTable = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SharedTaskFnTables::SharedTaskFn* SharedTaskFnTables::createSharedTaskFnTable()
{
  uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  NMP::Memory::Format memReqs(sizeof(SharedTaskFn) * numSemantics, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  SharedTaskFn* result = (SharedTaskFn*)memRes.ptr;
  for (uint32_t i = 0; i < numSemantics; ++i)
    result[i] = NULL;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SharedTaskFnTables::findCompatibleTaskFnTable(const SharedTaskFn* taskFnTbl) const
{
  uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();

  // Check if the table created matches any other tables already stored for that node
  // type ID. Add a new table otherwise. Return index of the table
  for (uint32_t tbl = 0; tbl < m_numTaskFnTables; ++tbl)
  {
    SharedTaskFn* ourTaskFnTbl = m_taskFnTables[tbl];
    NMP_ASSERT(ourTaskFnTbl);

    bool isFound = true;
    for (uint32_t sem = 0; sem < numSemantics; ++sem)
    {
      if (ourTaskFnTbl[sem] != taskFnTbl[sem])
      {
        // Need to register a new table
        isFound = false;
        break;
      }
    }

    if (isFound)
      return tbl;
  }

  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SharedTaskFnTables::findEmptyTaskFnTable() const
{
  uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();

  // Find an empty task function table
  for (uint32_t tbl = 0; tbl < m_numTaskFnTables; ++tbl)
  {
    SharedTaskFn* taskFnTbl = m_taskFnTables[tbl];
    NMP_ASSERT(taskFnTbl);

    bool isFound = true;
    for (uint32_t sem = 0; sem < numSemantics; ++sem)
    {
      if (taskFnTbl[sem] != NULL)
      {
        isFound = false;
        break;
      }
    }

    if (isFound)
      return tbl;
  }

  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SharedTaskFnTables::registerTaskFnTable(const SharedTaskFnTables::SharedTaskFn* taskFnTbl)
{
  // Check if the table created matches any other tables already stored for that node
  // type ID. Add a new table otherwise. Return index of the table
  uint32_t result = findCompatibleTaskFnTable(taskFnTbl);
  if (result == 0xFFFFFFFF)
  {
    // Create a new task function table
    uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
    NMP::Memory::Format memReqs(sizeof(SharedTaskFn) * numSemantics, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

    // Copy the task function table
    SharedTaskFn* newTaskFnTbl = (SharedTaskFn*)memRes.ptr;
    NMP::Memory::memcpy(newTaskFnTbl, taskFnTbl, memReqs.size);

    // Add to the tables array
    result = m_numTaskFnTables;
    m_taskFnTables[result] = newTaskFnTbl;
    m_numTaskFnTables++;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SharedTaskFnTables::registerEmptyTaskFnTable()
{
  uint32_t result = findEmptyTaskFnTable();
  if (result == 0xFFFFFFFF)
  {
    // Create an empty task function table
    result = m_numTaskFnTables;
    m_taskFnTables[result] = createSharedTaskFnTable();
    m_numTaskFnTables++;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SharedTaskFnTables::freeRegisteredTaskFnTables()
{
  for (uint32_t i = 0; i < m_numTaskFnTables; ++i)
  {
    NMP::Memory::memFree(m_taskFnTables[i]);
  }
  m_numTaskFnTables = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SharedTaskFnTables::freeLastRegisteredTaskFnTable()
{
  NMP_ASSERT(m_numTaskFnTables > 0);
  NMP_ASSERT(m_taskFnTables[m_numTaskFnTables - 1]);
  NMP::Memory::memFree(m_taskFnTables[m_numTaskFnTables - 1]);
  --m_numTaskFnTables;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SharedTaskFnTables::getMemoryRequirements(uint32_t numTaskFnTables)
{
  // Header
  NMP::Memory::Format result(sizeof(SharedTaskFnTables), NMP_NATURAL_TYPE_ALIGNMENT);

  // Task function table pointers
  result += NMP::Memory::Format(sizeof(SharedTaskFn*) * numTaskFnTables, NMP_NATURAL_TYPE_ALIGNMENT);

  // Task function tables
  uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  NMP::Memory::Format memReqTaskFnTable(sizeof(SharedTaskFn) * numSemantics, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqTaskFnTable * numTaskFnTables);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SharedTaskFnTables* SharedTaskFnTables::init(
  NMP::Memory::Resource& memRes,
  const SharedTaskFnTables* taskFnTables)
{
  NMP_ASSERT(taskFnTables);

  //---------------------------
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(SharedTaskFnTables), NMP_NATURAL_TYPE_ALIGNMENT);

  // Task function table pointers
  NMP::Memory::Format memReqsTablePtrs(sizeof(SharedTaskFn*) * taskFnTables->m_numTaskFnTables, NMP_NATURAL_TYPE_ALIGNMENT);

  // Task function tables
  uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  NMP::Memory::Format memReqTaskFnTable(sizeof(SharedTaskFn) * numSemantics, NMP_NATURAL_TYPE_ALIGNMENT);

  //---------------------------
  // Header
  SharedTaskFnTables* result = (SharedTaskFnTables*)memRes.alignAndIncrement(memReqsHdr);

  // Task function table pointers
  result->m_taskFnTables = (SharedTaskFn**)memRes.alignAndIncrement(memReqsTablePtrs);

  // Task function tables
  result->m_numEntriesPerTable = numSemantics;
  result->m_numTaskFnTables = taskFnTables->m_numTaskFnTables;
  for (uint32_t tbl = 0; tbl < taskFnTables->m_numTaskFnTables; ++tbl)
  {
    SharedTaskFn* taskFnTable = (SharedTaskFn*)memRes.alignAndIncrement(memReqTaskFnTable);
    result->m_taskFnTables[tbl] = taskFnTable;

    const SharedTaskFn* taskFnTableSrc = taskFnTables->m_taskFnTables[tbl];
    for (uint32_t sem = 0; sem < numSemantics; ++sem)
    {
      taskFnTable[sem] = taskFnTableSrc[sem];
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SharedTaskFnTables::locateTaskQueuingFnTables()
{
  const MR::Manager& manager = MR::Manager::getInstance();

  NMP::endianSwap(m_numEntriesPerTable);
  // Some validation that we have compiled with the correct asset compiler.
  if (m_numEntriesPerTable != manager.getNumRegisteredAttribSemantics())
  {
    NMP_MSG(          
      "WARNING: On dislocate (in asset complier) the num semantics was: %d, but on locate (at runtime) the num semantics is: %d",
      m_numEntriesPerTable,
      manager.getNumRegisteredAttribSemantics());
  }

  NMP::endianSwap(m_numTaskFnTables);
  REFIX_SWAP_PTR(SharedTaskFn*, m_taskFnTables);
  for (uint32_t i = 0; i < m_numTaskFnTables; ++i)
  {
    REFIX_SWAP_PTR(SharedTaskFn, m_taskFnTables[i]);
    SharedTaskFn* taskFnTable = m_taskFnTables[i];
    NMP::endianSwapArray(taskFnTable, m_numEntriesPerTable, sizeof(SharedTaskFn));
    for (uint32_t sem = 0; sem < m_numEntriesPerTable; sem++)
    {
      taskFnTable[sem] = (SharedTaskFn)manager.getTaskQueuingFn((uint32_t) (ptrdiff_t)taskFnTable[sem]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SharedTaskFnTables::dislocateTaskQueuingFnTables()
{
  const MR::Manager& manager = MR::Manager::getInstance();
  NMP_VERIFY(manager.getNumRegisteredAttribSemantics() == m_numEntriesPerTable);

  for (uint32_t i = 0; i < m_numTaskFnTables; ++i)
  {
    SharedTaskFn* taskFnTable = m_taskFnTables[i];
    for (uint32_t sem = 0; sem < m_numEntriesPerTable; sem++)
    {
      taskFnTable[sem] = (SharedTaskFn)(size_t)(manager.getTaskQueuingFnID((QueueAttrTaskFn)taskFnTable[sem]));
    }
    NMP::endianSwapArray(taskFnTable, m_numEntriesPerTable, sizeof(SharedTaskFn));
    UNFIX_SWAP_PTR(SharedTaskFn, m_taskFnTables[i]);
  }
  NMP::endianSwap(m_numEntriesPerTable);
  NMP::endianSwap(m_numTaskFnTables);
  UNFIX_SWAP_PTR(SharedTaskFn*, m_taskFnTables);
}

//----------------------------------------------------------------------------------------------------------------------
void SharedTaskFnTables::locateOutputCPTaskFnTables()
{
  const MR::Manager& manager = MR::Manager::getInstance();

  NMP::endianSwap(m_numEntriesPerTable);
  // Some validation that we have compiled with the correct asset compiler.
  if (m_numEntriesPerTable != manager.getNumRegisteredAttribSemantics())
  {
    NMP_MSG(          
      "WARNING: On dislocate (in asset complier) the num semantics was: %d, but on locate (at runtime) the num semantics is: %d",
      m_numEntriesPerTable,
      manager.getNumRegisteredAttribSemantics());
  }

  NMP::endianSwap(m_numTaskFnTables);
  REFIX_SWAP_PTR(SharedTaskFn*, m_taskFnTables);
  for (uint32_t i = 0; i < m_numTaskFnTables; ++i)
  {
    REFIX_SWAP_PTR(SharedTaskFn, m_taskFnTables[i]);
    SharedTaskFn* taskFnTable = m_taskFnTables[i];
    NMP::endianSwapArray(taskFnTable, m_numEntriesPerTable, sizeof(SharedTaskFn));
    for (uint32_t sem = 0; sem < m_numEntriesPerTable; sem++)
    {
      taskFnTable[sem] = (SharedTaskFn)manager.getOutputCPTask((uint32_t) (ptrdiff_t)taskFnTable[sem]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SharedTaskFnTables::dislocateOutputCPTaskFnTables()
{
  const MR::Manager& manager = MR::Manager::getInstance();
  NMP_VERIFY(manager.getNumRegisteredAttribSemantics() == m_numEntriesPerTable);

  for (uint32_t i = 0; i < m_numTaskFnTables; ++i)
  {
    SharedTaskFn* taskFnTable = m_taskFnTables[i];
    for (uint32_t sem = 0; sem < m_numEntriesPerTable; sem++)
    {
      taskFnTable[sem] = (SharedTaskFn)(size_t)(manager.getOutputCPTaskID((OutputCPTask)taskFnTable[sem]));
    }
    NMP::endianSwapArray(taskFnTable, m_numEntriesPerTable, sizeof(SharedTaskFn));
    UNFIX_SWAP_PTR(SharedTaskFn, m_taskFnTables[i]);
  }
  NMP::endianSwap(m_numEntriesPerTable);
  NMP::endianSwap(m_numTaskFnTables);
  UNFIX_SWAP_PTR(SharedTaskFn*, m_taskFnTables);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
