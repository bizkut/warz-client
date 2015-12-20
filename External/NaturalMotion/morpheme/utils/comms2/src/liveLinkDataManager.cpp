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
#include "comms/liveLinkDataManager.h"
#include "comms/liveLinkNetworkManager.h"
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrRuntimeNodeInspector.h"
//----------------------------------------------------------------------------------------------------------------------
#include <algorithm>
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
LiveLinkDataManager* LiveLinkDataManager::sm_instance = NULL;

//----------------------------------------------------------------------------------------------------------------------
LiveLinkDataManager::LiveLinkDataManager(uint32_t maxOutputListEntries, uint32_t maxNumInstances) :
  SimpleDataManager(),
  m_cachedInstanceId(0xFFFFFFFF),
  m_cachedNodeOutputs(NULL),
  m_maxOutputListEntries(maxOutputListEntries),
  m_maxNumInstances(maxNumInstances)
{
  NMP_ASSERT(!sm_instance);
  sm_instance = this;

  NMP_ASSERT(maxNumInstances > 0);

  NMP::Memory::Format instNodeOutputsFormat = InstanceNodeOutputs::getMemoryRequirements(maxNumInstances);
  NMP::Memory::Resource instNodeOutputsRes = NMPMemoryAllocateFromFormat(instNodeOutputsFormat);  
  m_instanceNodeOutputs = InstanceNodeOutputs::init(instNodeOutputsRes, maxNumInstances);

  NMP::Memory::Format exclSetFormat = ControlExclusionSet::getMemoryRequirements(maxNumInstances);
  NMP::Memory::Resource exclSetRes = NMPMemoryAllocateFromFormat(exclSetFormat);  
  m_controlExclusionSet = ControlExclusionSet::init(exclSetRes, maxNumInstances);
}

//----------------------------------------------------------------------------------------------------------------------
LiveLinkDataManager::~LiveLinkDataManager()
{
  NMP_ASSERT(sm_instance == this);
  sm_instance = NULL;

  for (InstanceNodeOutputs::iterator iter = m_instanceNodeOutputs->begin(); iter != m_instanceNodeOutputs->end(); ++iter)
  {
    NodeOutputList* nodeOutputList = (*iter).value;
    NMP::Memory::memFree(nodeOutputList);
  }

  NMP::Memory::memFree(m_instanceNodeOutputs);
  NMP::Memory::memFree(m_controlExclusionSet);
}

//----------------------------------------------------------------------------------------------------------------------
const NodeOutputList* LiveLinkDataManager::findNodeOutputList(MCOMMS::InstanceID id)
{
  //----------------------------
  // Check the cached (MRU) value first ...
  if (id == m_cachedInstanceId)
  {
    return m_cachedNodeOutputs;
  }

  //----------------------------
  // ... then check the full list.
  InstanceNodeOutputs::iterator it = m_instanceNodeOutputs->find(id);
  if (it == m_instanceNodeOutputs->end())
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected (id=%u)!", id);
    return NULL;
  }

  //----------------------------
  // Cache the new value.
  m_cachedInstanceId = id;
  m_cachedNodeOutputs = (*it).value;

  // ...
  return (*it).value;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::enableControlParameterBuffering(MCOMMS::InstanceID id)
{
  //----------------------------
  // Find the network instance.
  MR::Network* const network = findNetworkByInstanceID(id);
  if (!network)
  {
    return false;
  }

  //----------------------------
  // Find or create the node output list for the instance.
  NodeOutputList* nodeOutputs = 0;
  InstanceNodeOutputs::iterator it = m_instanceNodeOutputs->find(id);
  if (it == m_instanceNodeOutputs->end())
  {
    //----------------------------
    // Create a list of node outputs for the given instance if one does not already exist.
    NMP_ASSERT(m_maxOutputListEntries > 0);
    NMP::Memory::Format outputListFormat = NodeOutputList::getMemoryRequirements(m_maxOutputListEntries);
    NMP::Memory::Resource outputListRes = NMPMemoryAllocateFromFormat(outputListFormat);  
    NodeOutputList* nodeOutputList = NodeOutputList::init(outputListRes, m_maxOutputListEntries);
    NMP_ASSERT(nodeOutputList);
    m_instanceNodeOutputs->insert(id, nodeOutputList);
    nodeOutputs = nodeOutputList;
  }
  else
  {
    nodeOutputs = (*it).value;
  }

  //----------------------------
  // Identify control parameter nodes in the network and node outputs for them.
  const MR::NetworkDef* const networkDef = network->getNetworkDef();
  const uint32_t numNodes = networkDef->getNumNodeDefs();

  NodeOutput nodeOutput;

  for (uint32_t nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
  {
    const MR::NodeID nodeId = (MR::NodeID)nodeIndex;

    //----------------------------
    // Is the node a control parameter ?
    if (networkDef->getNodeDef(nodeId)->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM)
    {
      MR::NodeBin* nodeBin = network->getNodeBin(nodeId);
      MR::OutputCPPin* const outputPin = nodeBin->getOutputCPPin(0);
      if (outputPin)
      {
        const MR::AttribData* const attribData = outputPin->getAttribData();
        if (attribData)
        {
          MR::NodeOutputDataType dataType = NODE_OUTPUT_DATA_TYPE_RUNTIME_TYPE;
          {
            switch( attribData->getType())
            {
            case MR::ATTRIB_TYPE_BOOL:    dataType = NODE_OUTPUT_DATA_TYPE_BOOL;    break;
            case MR::ATTRIB_TYPE_INT:     dataType = NODE_OUTPUT_DATA_TYPE_INT;     break;
            case MR::ATTRIB_TYPE_UINT:    dataType = NODE_OUTPUT_DATA_TYPE_UINT;    break;
            case MR::ATTRIB_TYPE_FLOAT:   dataType = NODE_OUTPUT_DATA_TYPE_FLOAT;   break;
            case MR::ATTRIB_TYPE_VECTOR3: dataType = NODE_OUTPUT_DATA_TYPE_VECTOR3; break;
            case MR::ATTRIB_TYPE_VECTOR4: dataType = NODE_OUTPUT_DATA_TYPE_VECTOR4; break;
            default:                  NMP_ASSERT_FAIL();                        break;
            }
          }

          //----------------------------
          // Create a node output that matches the control parameter's type.
          nodeOutput.m_nodeID = nodeId;
          nodeOutput.m_dataType = dataType;

          NMP_ASSERT(nodeOutputs->find(nodeOutput) == nodeOutputs->end());
          nodeOutputs->push_back(nodeOutput);
        }
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkDataManager::disableNetworkControl(MCOMMS::InstanceID id)
{
  m_controlExclusionSet->insert(id, true);
}

//----------------------------------------------------------------------------------------------------------------------
MR::Network* LiveLinkDataManager::findNetworkByInstanceID(MCOMMS::InstanceID id) const
{
  if (LiveLinkNetworkManager::getInstance())
  {
    const char* name = NULL;
    MR::Network* network = LiveLinkNetworkManager::getInstance()->findNetworkByInstanceID(id, name);
    return network;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* LiveLinkDataManager::findNetworkDefByGuid(const MCOMMS::GUID& guid) const
{
  if (LiveLinkNetworkManager::getInstance())
  {
    MR::NetworkDef* const gameCharacterDef = LiveLinkNetworkManager::getInstance()->findNetworkDefByGuid(guid);
    return gameCharacterDef;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::sendMessage(
  MCOMMS::InstanceID       id,
  MCOMMS::commsNodeID      stateMachineNodeID,
  const MR::Message&       message)
{
  if (m_controlExclusionSet->find(id) != m_controlExclusionSet->end())
  {
    return false;
  }

  return MCOMMS::SimpleDataManager::sendMessage(id, stateMachineNodeID, message);
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::broadcastMessage(MCOMMS::InstanceID id, const MR::Message& message)
{
  if (m_controlExclusionSet->find(id) != m_controlExclusionSet->end())
  {
    return false;
  }

  return MCOMMS::SimpleDataManager::broadcastMessage(id, message);
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::setCurrentState(
  MCOMMS::InstanceID  id,
  MCOMMS::commsNodeID stateMachineNodeID,
  MCOMMS::commsNodeID newRootStateID)
{
  if (m_controlExclusionSet->find(id) != m_controlExclusionSet->end())
  {
    return false;
  }

  return MCOMMS::SimpleDataManager::setCurrentState(id, stateMachineNodeID, newRootStateID);
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::setControlParameter(
  MCOMMS::InstanceID     id,
  MCOMMS::commsNodeID    nodeID,
  MR::NodeOutputDataType type,
  const void*            cparamData)
{
  if (m_controlExclusionSet->find(id) != m_controlExclusionSet->end())
  {
    return false;
  }

  return MCOMMS::SimpleDataManager::setControlParameter(id, nodeID, type, cparamData);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkDataManager::getNodeOutputCount(MCOMMS::InstanceID id)
{
  InstanceNodeOutputs::iterator it = m_instanceNodeOutputs->find(id);
  if (it == m_instanceNodeOutputs->end())
  {
    //----------------------------
    // Don't throw an error, the instance may simply not have any debug outputs yet.
    return 0;
  }

  return (*it).value->size();
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::commsNodeID LiveLinkDataManager::getNodeOutputNodeID(MCOMMS::InstanceID id, uint32_t outputIndex)
{
  const NodeOutputList* const nodeOutputs = findNodeOutputList(id);
  if (!nodeOutputs)
  {
    return (MCOMMS::commsNodeID)MR::INVALID_NODE_ID;
  }

  return (uint32_t)(*nodeOutputs)[outputIndex].m_nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeOutputDataType LiveLinkDataManager::getNodeOutputDataType(MCOMMS::InstanceID id, uint32_t outputIndex)
{
  const NodeOutputList* const nodeOutputs = findNodeOutputList(id);
  if (!nodeOutputs)
  {
    return MR::INVALID_NODE_OUTPUT_DATA_TYPE;
  }

  return (uint32_t)(*nodeOutputs)[outputIndex].m_dataType;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::enableOutputDataBuffering(
  MCOMMS::InstanceID     id,
  MCOMMS::commsNodeID    nodeID,
  MR::NodeOutputDataType dataType,
  bool                   enable)
{
  //----------------------------
  // Let SimpleDataManager take care of updating the morpheme network ...
  if (!SimpleDataManager::enableOutputDataBuffering(id, nodeID, dataType, enable))
  {
    return false;
  }

  //----------------------------
  // ... then update our list of node outputs.
  InstanceNodeOutputs::iterator it = m_instanceNodeOutputs->find(id);
  NodeOutputList* nodeOutputs = 0;
  if (it == m_instanceNodeOutputs->end())
  {
    //----------------------------
    // Create a list of node outputs for the given instance if one does not already exist.
    NMP_ASSERT(m_maxOutputListEntries > 0);
    NMP::Memory::Format nodeOutputsFormat = NodeOutputList::getMemoryRequirements(m_maxOutputListEntries);
    NMP::Memory::Resource nodeOutputsRes = NMPMemoryAllocateFromFormat(nodeOutputsFormat);
    NodeOutputList* nodeOutputsList = NodeOutputList::init(nodeOutputsRes, m_maxOutputListEntries);
    NMP_ASSERT(nodeOutputsList);
    m_instanceNodeOutputs->insert(id, nodeOutputsList);
    nodeOutputs = nodeOutputsList;
  }
  else
  {
    nodeOutputs = (*it).value;
  }

  //----------------------------
  // Setup a node output to represent this entry.
  NodeOutput nodeOutput;
  {
    nodeOutput.m_nodeID = nodeID;
    nodeOutput.m_dataType = dataType;
  }

  //----------------------------
  // Either add or remove the node output.
  if (enable)
  {
    if (nodeOutputs->find(nodeOutput) == nodeOutputs->end())
    {
      nodeOutputs->push_back(nodeOutput);
    }
  }
  else
  {
    NodeOutputList::iterator iter = nodeOutputs->find(nodeOutput);
    if (iter != nodeOutputs->end())
    {
      nodeOutputs->erase(iter);
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkDataManager::getNodeOutputDataLength(
  MCOMMS::InstanceID      MR_OUTPUT_DEBUG_ARG(id),
  uint32_t                MR_OUTPUT_DEBUG_ARG(outputIndex),
  MR::NodeID&             MR_OUTPUT_DEBUG_ARG(owningNodeID),
  MR::NodeID&             MR_OUTPUT_DEBUG_ARG(targetNodeID),
  MR::AttribDataType&     MR_OUTPUT_DEBUG_ARG(attribType),
  MR::AttribDataSemantic& MR_OUTPUT_DEBUG_ARG(attribSemantic),
  MR::AnimSetIndex&       MR_OUTPUT_DEBUG_ARG(animSetIndex),
  MR::FrameCount&         MR_OUTPUT_DEBUG_ARG(validFrame))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* const network = findNetworkByInstanceID(id);
  const NodeOutputList* const nodeOutputs = findNodeOutputList(id);
  if (!network || !nodeOutputs)
  {
    return 0;
  }

  const NodeOutput& nodeOutput = (*nodeOutputs)[outputIndex];
  owningNodeID = (MR::NodeID)nodeOutput.m_nodeID;

  return MR::RuntimeNodeInspector::getNodeBufferedDataTypeSize(
           network, (MR::NodeID)nodeOutput.m_nodeID,
           nodeOutput.m_dataType, targetNodeID, attribType, attribSemantic, animSetIndex, validFrame);
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::getNodeOutputData(
  MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(id),
  uint32_t           MR_OUTPUT_DEBUG_ARG(outputIndex),
  void*              MR_OUTPUT_DEBUG_ARG(destBuffer),
  uint32_t           MR_OUTPUT_DEBUG_ARG(bufferLength))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* const network = findNetworkByInstanceID(id);
  const NodeOutputList* nodeOutputs = findNodeOutputList(id);
  if (!network || !nodeOutputs)
  {
    return false;
  }

  const NodeOutput& nodeOutput = (*nodeOutputs)[outputIndex];

  MR::AttribData* attribData = MR::RuntimeNodeInspector::getNodeAttribDataForOutputDataType(
    network, 
    (MR::NodeID)nodeOutput.m_nodeID,
    nodeOutput.m_dataType);
  if (attribData)
  {
    return MR::RuntimeNodeInspector::serialiseAttribData( network,
                                                          (MR::NodeID)nodeOutput.m_nodeID,
                                                          attribData,
                                                          destBuffer,
                                                          bufferLength );
  }
  return false;
#else
  return false;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkDataManager::serialiseAndBufferDebugOutputCPAttribData(
  MR::Network                  *MR_OUTPUT_DEBUG_ARG(net),
  const MR::OutputCPPin        *MR_OUTPUT_DEBUG_ARG(outputCPPin),
  const MR::NodeID              MR_OUTPUT_DEBUG_ARG(owningNodeID),
  const MR::PinIndex            MR_OUTPUT_DEBUG_ARG(pinIndex),
  const MR::AttribDataSemantic  MR_OUTPUT_DEBUG_ARG(semantic),
  MCOMMS::Connection           *MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Manager &manager = MR::Manager::getInstance();

  // Find the required buffer size for serialising this attribute in to.
  MR::AttribData *attribData = outputCPPin->m_attribDataHandle.m_attribData;
  MR::AttribSerializeTxFn serializeFn = manager.getAttribSerializeTxFn(attribData->getType());

  if (serializeFn)
  {
    uint32_t attribBufferSize = serializeFn(net, owningNodeID, attribData, 0, 0);
    if (attribBufferSize > 0)
    {
      const char *semanticName = getSemanticName(semantic);
      MCOMMS::NodeOutputDataPacket *nodeOutputData =  MCOMMS::NodeOutputDataPacket::create(
        connection->getDataBuffer(),
        attribBufferSize,
        owningNodeID,
        MR::INVALID_NODE_ID,
        attribData->getType(),
        semantic,
        MCOMMS::CommsServer::getInstance()->getTokenForString(semanticName),
        MR::ANIMATION_SET_ANY,
        outputCPPin->m_lastUpdateFrame,
        pinIndex);
      // Serialise output AttribData on to packet buffer.
      serializeFn(net, owningNodeID, attribData, nodeOutputData->getData(), attribBufferSize);
      mcommsSerializeDataPacket(*nodeOutputData);
    }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataSemantic LiveLinkDataManager::getAttribSemanticFromType(MR::AttribDataType attribType)
{
  MR::AttribDataSemantic attribSemantic;

  switch (attribType)
  {
  case MR::ATTRIB_TYPE_BOOL:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_BOOL;
    break;
  case MR::ATTRIB_TYPE_UINT:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_UINT;
    break;
  case MR::ATTRIB_TYPE_INT:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_INT;
    break;
  case MR::ATTRIB_TYPE_FLOAT:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_FLOAT;
    break;
  case MR::ATTRIB_TYPE_VECTOR3:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_VECTOR3;
    break;
  case MR::ATTRIB_TYPE_VECTOR4:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_VECTOR4;
    break;
  default:
    attribSemantic = MR::ATTRIB_SEMANTIC_NA;
    NMP_ASSERT_FAIL(); // Un-handled control parameter AttribDataType.
    break;
  }
  return attribSemantic;
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkDataManager::serialiseAndBufferDebugAttribData(
  MR::Network*            MR_OUTPUT_DEBUG_ARG(net),
  const MR::NodeBinEntry* MR_OUTPUT_DEBUG_ARG(entry),
  MCOMMS::Connection*     MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Manager& manager = MR::Manager::getInstance();

  // Find the required buffer size for serialising this attribute in to.
  MR::AttribData* attribData = entry->m_attribDataHandle.m_attribData;
  MR::AttribSerializeTxFn serializeFn = manager.getAttribSerializeTxFn(attribData->getType());

  if (serializeFn)
  {
    uint32_t attribBufferSize = serializeFn(net, entry->m_address.m_owningNodeID, attribData, 0, 0);
    if (attribBufferSize > 0)
    {
      const char* semanticName = entry->m_address.getAttribSemanticName();
      MCOMMS::NodeOutputDataPacket* nodeOutputData = MCOMMS::NodeOutputDataPacket::create(
        connection->getDataBuffer(),
        attribBufferSize,
        entry->m_address.m_owningNodeID,
        entry->m_address.m_targetNodeID,
        attribData->getType(),
        entry->m_address.m_semantic,
        MCOMMS::CommsServer::getInstance()->getTokenForString(semanticName),
        entry->m_address.m_animSetIndex,
        entry->m_address.m_validFrame);
      // Serialise output AttribData on to packet buffer.
      serializeFn(net, entry->m_address.m_owningNodeID, attribData, nodeOutputData->getData(), attribBufferSize);
      mcommsSerializeDataPacket(*nodeOutputData);
    }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::serializeTxControlParamAndOpNodeAttribData( MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(instanceID), MCOMMS::Connection* MR_OUTPUT_DEBUG_ARG(connection) )
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* net = findNetworkByInstanceID(instanceID);
  MR::NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(net && netDef && net->getDispatcher());

  // Only transmit if debug output filtering allows.
  MR::InstanceDebugInterface* debugInterface = net->getDispatcher()->getDebugInterface();
  if (debugInterface && debugInterface->debugOutputsAreOn(MR::DEBUG_OUTPUT_CONTROL_PARAMS))
  {
    MCOMMS::BeginInstanceSectionPacket beginInstanceCPData(MCOMMS::kControlParamsDataSection);
    mcommsBufferDataPacket(beginInstanceCPData, connection);

    for (MR::NodeID i = 0; i < netDef->getNumNodeDefs(); ++i)
    {
      // Find all active control params and operator nodes.
      MR::NodeDef* nodeDef = netDef->getNodeDef(i);
      MR::NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
      if (nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) ||
        (nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE) && net->operatorCPNodeIsActive(i)))
      {
        // For all output control parameter pin attributes on this node.
        const uint32_t numOutputCPPins = nodeDef->getNumOutputCPPins();
        for (MR::PinIndex sourceCPPPinIndex = 0; sourceCPPPinIndex < numOutputCPPins; sourceCPPPinIndex++)
        {
          MR::NodeBin* nodeBin = net->getNodeBin(i);
          const MR::OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(sourceCPPPinIndex);
          // generates an attribute semantic using a simple mapping from attribute type
          MR::AttribData* attribData = outputCPPin->m_attribDataHandle.m_attribData;
          if(attribData)
          {
            MR::AttribDataType attribType = attribData->getType();
            serialiseAndBufferDebugOutputCPAttribData(
              net,
              outputCPPin,
              i,
              sourceCPPPinIndex,
              getAttribSemanticFromType(attribType),
              connection);
          }
        }
      }
    }

    MCOMMS::EndInstanceSectionPacket endInstanceCPData(MCOMMS::kControlParamsDataSection);
    mcommsBufferDataPacket(endInstanceCPData, connection);
  }

  return true;
#else
  return false;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::serializeTxTreeNodeAttribData( MCOMMS::InstanceID instanceID, MCOMMS::Connection *connection )
{
#ifdef MR_ATTRIB_DEBUG_BUFFERING
  MR::Network* net = findNetworkByInstanceID(instanceID);
  MR::NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(net && netDef && net->getDispatcher());

  // Only transmit if debug output filtering allows.
  MR::InstanceDebugInterface* debugInterface = net->getDispatcher()->getDebugInterface();
  if (debugInterface && debugInterface->debugOutputsAreOn(MR::DEBUG_OUTPUT_TREE_NODES))
  {
    MCOMMS::BeginInstanceSectionPacket beginInstanceTreeData(MCOMMS::kTreeNodesDataSection);
    mcommsBufferDataPacket(beginInstanceTreeData, connection);

    for (MR::NodeID i = 0; i < netDef->getNumNodeDefs(); ++i)
    {
      // Find all active tree nodes.
      MR::NodeDef* nodeDef = netDef->getNodeDef(i);
      MR::NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
      if ((!nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) &&
          !nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE) &&  // Don't send back control param or operator node data here (handled else where).
          net->nodeIsActive(i)) ||                                      // Only send back active node data.)
        nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE)        // Always send back state machine data (even if it is inactive).
        )
      {
        MR::NodeBin* nodeBin = net->getNodeBin(i);
        const MR::NodeBinEntry* entry = nodeBin->getEntries();

        // For all attributes on this node find if it has been flagged for debug output.
        while (entry)
        {
          MR::AttribDataSemantic semantic = entry->m_address.m_semantic;
          if (nodeBin->isDebugOutputSemanticSet(semantic))
          {
            serialiseAndBufferDebugAttribData(net, entry, connection);
          }
          entry = entry->m_next;
        }
      }
    }

    MCOMMS::EndInstanceSectionPacket endInstanceTreeData(MCOMMS::kTreeNodesDataSection);
    mcommsBufferDataPacket(endInstanceTreeData, connection);
  }

  return true;

#else // MR_ATTRIB_DEBUG_BUFFERING
  (void) instanceID;
  (void) connection;
  return false;
#endif // MR_ATTRIB_DEBUG_BUFFERING
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkDataManager::serializeTxNodeTimings( MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(instanceID), MCOMMS::Connection* MR_OUTPUT_DEBUG_ARG(connection) )
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* net = findNetworkByInstanceID(instanceID);
#ifdef NMP_ENABLE_ASSERTS
  MR::NetworkDef* netDef = net->getNetworkDef();
#endif
  NMP_ASSERT(net && netDef && net->getDispatcher());

  // Only transmit if debug output filtering allows.
  MR::InstanceDebugInterface* debugInterface = net->getDispatcher()->getDebugInterface();
  if (debugInterface && debugInterface->debugOutputsAreOn(MR::DEBUG_OUTPUT_PROFILING))
  {
    MCOMMS::FrameNodeTimingsPacket* packet = MCOMMS::FrameNodeTimingsPacket::create(
      connection->getDataBuffer(),
      instanceID,
      debugInterface->getNumNodeTimings(),
      debugInterface->getNodeTimings());
    mcommsSerializeDataPacket(*packet);
  } 

  return true;
#else
  return false;
#endif
}

} // namespace COMMS
