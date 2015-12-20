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
#include "comms/simpleDataManager.h"

#include "comms/connection.h"
#include "comms/corePackets.h"
#include "comms/debugPackets.h"

#include "morpheme/mrCharacterControllerInterface.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrRuntimeNodeInspector.h"
#include "morpheme/mrUtils.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
// SimpleDataManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
#define SIMPLE_DATA_MANAGER_GET_NETWORK(onInvalidIdExp_) \
  findNetworkByInstanceID(id); \
  if (!network) \
  { \
    NMP_DEBUG_MSG("error: Valid instance ID expected (id=%u)!", id); \
    onInvalidIdExp_; \
  }

//----------------------------------------------------------------------------------------------------------------------
#define SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(onInvalidIdExp_) \
  findNetworkDefByGuid(guid); \
  if (!networkDef) \
  { \
    NMP_DEBUG_MSG("error: Valid network GUID expected!"); \
    onInvalidIdExp_; \
  }

//----------------------------------------------------------------------------------------------------------------------
SimpleDataManager::SimpleDataManager() :
  DataManagementInterface(),
  m_frameIndex(0),
  m_stepInterval(1.0f / 30.0f),
  m_frameRate(30.0f),
  m_isSimulating(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getSemanticName(MR::AttribDataSemantic semantic)
{
  return MR::AttribAddress::getAttribSemanticName(semantic);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::serializeTxAnimRigDefData(
  const GUID&      networkGUID,
  MR::AnimSetIndex index,
  Connection*      connection)
{
  MR::NetworkDef* networkDef = findNetworkDefByGuid(networkGUID);
  if (!networkDef)
  {
    NMP_DEBUG_MSG("error: Valid network GUID expected!");
    return false;
  }

  MR::AnimRigDef* animRigDef = networkDef->getRig(index);
  if (animRigDef)
  {
    uint32_t dataSize = animRigDef->serializeTx(index, 0, 0);

    if (dataSize > 0)
    {
      MCOMMS::BeginPersistentPacket beginPersistentData(MR::kAnimRigDefPersistentData);
      mcommsBufferDataPacket(beginPersistentData, connection);

      MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();
      MCOMMS::AnimRigDefPacket* animRigDefPacket = MCOMMS::AnimRigDefPacket::create(
        networkDataBuffer,
        networkGUID,
        dataSize);
      
      void* data = animRigDefPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
      uint32_t usedSize =
#endif
        animRigDef->serializeTx(index, data, dataSize);
      NMP_ASSERT(usedSize <= dataSize);

      mcommsSerializeDataPacket(*animRigDefPacket);

      MCOMMS::EndPersistentPacket endPersistentData;
      mcommsBufferDataPacket(endPersistentData, connection);

      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getMaxTransformCount(InstanceID id)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  return network->getMaxTransformCount();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getCurrentTransformCount(InstanceID id)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  return network->getCurrentTransformCount();
}


//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::getLocalTransforms(InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  NMP::Vector3 rootPosition(NMP::Vector3::InitZero);
  NMP::Quat rootOrientation(NMP::Quat::kIdentity);

  MR::CharacterControllerInterface* const characterController = network->getCharacterController();
  if (characterController)
  {
    rootPosition = network->getCharacterPropertiesWorldRootTransform().translation();
    rootOrientation = network->getCharacterPropertiesWorldRootTransform().toQuat();
  }

  NMP::DataBuffer* const sourceBuffer = network->getTransforms();

  if (numTransforms > 0 && numTransforms <= sourceBuffer->getLength())
  {
    buffer[0].m_pos = rootPosition;
    buffer[0].m_quat = rootOrientation;

    MR::AnimRigDef* rig = network->getActiveRig();
    NMP::DataBuffer* bindPose = rig->getBindPose()->m_transformBuffer;
    NMP_ASSERT(bindPose->getLength() == sourceBuffer->getLength());
    for (uint32_t i = 1; i < numTransforms; ++i)
    {
      if (sourceBuffer->hasChannel(i))
      {
        buffer[i].m_pos = *sourceBuffer->getPosQuatChannelPos(i);
        buffer[i].m_quat = *sourceBuffer->getPosQuatChannelQuat(i);
      }
      else
      {
        buffer[i].m_pos = *(NMP::Vector3*)bindPose->getPosQuatChannelPos(i);
        buffer[i].m_quat = *(NMP::Quat*)bindPose->getPosQuatChannelQuat(i);
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::getWorldTransforms(InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  NMP::Vector3 rootPosition(NMP::Vector3::InitZero);
  NMP::Quat rootOrientation(NMP::Quat::kIdentity);
 
  computeNetworkWorldTransforms(
          network,
          rootPosition,
          rootOrientation,
          numTransforms,
          buffer);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::getTransformChange(InstanceID id, NMP::Vector3& deltaTranslation, NMP::Quat& deltaOrientation)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  deltaTranslation = network->getTranslationChange();
  deltaOrientation = network->getOrientationChange();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getActiveNodeCount(InstanceID id)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  return network->getActiveNodeCount();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getActiveNodes(InstanceID id, commsNodeID* nodeIDs, uint32_t maxNodeIDs)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);

  // Allocate a node ID buffer using the network's temporary memory allocator.
  NMP::TempMemoryAllocator* const tempAllocator = network->getTempMemoryAllocator();
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  MR::NodeID* const mrNodeIDs = (MR::NodeID*) NMPAllocatorMemAlloc(
    childAllocator, maxNodeIDs * sizeof(MR::NodeID), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(mrNodeIDs);

  // Retrieve list of active nodes from the network.
  const uint32_t numActiveNodesAvailable = network->getActiveNodes(mrNodeIDs, maxNodeIDs);

  // Copy into output buffer (and convert to COMMS node IDs in the process).
  const uint32_t numNodeIDsToCopy = NMP::minimum(numActiveNodesAvailable, maxNodeIDs);
  for (uint32_t i = 0; i < numNodeIDsToCopy; ++i)
  {
    nodeIDs[i] = (commsNodeID)mrNodeIDs[i];
  }

  // Free the node ID buffer and reset the network's temporary memory allocator.
  childAllocator->memFree(mrNodeIDs);
  tempAllocator->destroyChildAllocator(childAllocator);

  // ...
  return numActiveNodesAvailable;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getTriggeredNodeEventMessageCount(InstanceID id)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  return network->getTriggeredNodeEventMessageCount();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getTriggeredNodeEventMessages(InstanceID id, NetworkMessageID* messageIDs, uint32_t maxMessages)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  
  // Allocate a message ID buffer using the network's temporary memory allocator.
  NMP::TempMemoryAllocator* const tempAllocator = network->getTempMemoryAllocator();
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();
  NMP_ASSERT(tempAllocator->getUsedBytes() == 0);

  MR::MessageID* const mrMessageIDs = (MR::MessageID*)NMPAllocatorMemAlloc(
    childAllocator, 
    maxMessages * sizeof(MR::MessageID), 
    NMP_NATURAL_TYPE_ALIGNMENT);

  // Retrieve list of triggered messages from the network.
  const uint32_t numTriggeredMessages = network->getTriggeredNodeEventMessages(mrMessageIDs, maxMessages);

  // Copy into output buffer (and convert to COMMS IDs in the process).
  const uint32_t numMessageIDsToCopy = NMP::minimum(numTriggeredMessages, maxMessages);
  for (uint32_t i = 0; i < numMessageIDsToCopy; ++i)
  {
    messageIDs[i] = (NetworkMessageID)mrMessageIDs[i];
  }

  // Free the node ID buffer and reset the networks temporary memory allocator.
  childAllocator->memFree(mrMessageIDs);
  tempAllocator->destroyChildAllocator(childAllocator);

  // ...
  return numTriggeredMessages;
}

//----------------------------------------------------------------------------------------------------------------------
commsNodeID SimpleDataManager::getRootStateNodeID(InstanceID id, commsNodeID stateMachineNodeID)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return (commsNodeID)MR::INVALID_NODE_ID);
  return (commsNodeID)network->getStateMachineCurrentStateNodeId((MR::NodeID)stateMachineNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::getScatterBlendWeightData(
  MCOMMS::InstanceID id, MCOMMS::commsNodeID scatterBlendNodeID,
  float (&barycentricWeights)[3],
  MCOMMS::commsNodeID (&childNodeIDs)[3],
  float (&desiredMotionParams)[2],
  float (&achievedMotionParams)[2],
  bool& wasProjected)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  // Check it's a scatter blend node they are asking for the weight data on.
  NMP_ASSERT(network->getNetworkDef()->getNodeDef(scatterBlendNodeID)->getNodeTypeID() == NODE_TYPE_SCATTER_BLEND_2D);

  // Get the AttribDataBlendMap2DState
  MR::NodeBinEntry* stateEntry = network->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    network->getNetworkDef()->getNodeDef(scatterBlendNodeID)->getNodeID(),
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME);

  NMP_ASSERT(stateEntry); // State data must also already exist.
  MR::AttribDataScatterBlend2DState* stateData = stateEntry->getAttribData<MR::AttribDataScatterBlend2DState>();
  barycentricWeights[0] = stateData->m_parameteriserInfo.m_childNodeWeights[0];
  barycentricWeights[1] = stateData->m_parameteriserInfo.m_childNodeWeights[1];
  barycentricWeights[2] = stateData->m_parameteriserInfo.m_childNodeWeights[2];

  childNodeIDs[0] = stateData->m_parameteriserInfo.m_childNodeIDs[0];
  childNodeIDs[1] = stateData->m_parameteriserInfo.m_childNodeIDs[1];
  childNodeIDs[2] = stateData->m_parameteriserInfo.m_childNodeIDs[2];

  // These in the space of the scatter blend native units. i.e.
  // Angle - radians
  // Distance - runtime units
  desiredMotionParams[0] = stateData->m_parameteriserInfo.m_desiredParameters[0];
  desiredMotionParams[1] = stateData->m_parameteriserInfo.m_desiredParameters[1];
  achievedMotionParams[0] = stateData->m_parameteriserInfo.m_achievedParameters[0];
  achievedMotionParams[1] = stateData->m_parameteriserInfo.m_achievedParameters[1];
  wasProjected = stateData->m_parameteriserInfo.m_wasProjected;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getActiveAnimationSet(InstanceID id)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0xFFFFFFFF);
  return network->getActiveAnimSetIndex();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getAnimSetCount(InstanceID id) const
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  return network->getNetworkDef()->getNumAnimSets();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getNodeCount(InstanceID id)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  return network->getNetworkDef()->getNumNodeDefs();
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeType SimpleDataManager::getNodeTypeID(InstanceID id, commsNodeID nodeID)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return MR::INVALID_NODE_TYPE_ID);

  if (nodeID >= network->getNetworkDef()->getNumNodeDefs())
  {
    NMP_DEBUG_MSG(
      "error: Valid node ID expected (id=%u, numNodes=%u)!",
      nodeID, network->getNetworkDef()->getNumNodeDefs());

    return MR::INVALID_NODE_TYPE_ID;
  }

  MR::NodeDef* const nodeDef = network->getNetworkDef()->getNodeDef((MR::NodeID)nodeID);
  NMP_ASSERT(nodeDef);

  return nodeDef->getNodeTypeID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getNodeName(InstanceID id, commsNodeID nodeID)
{
  // "Unknown" is also returned by MR::NetworkDef::getNodeNameFromNodeID().
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return "Unknown");
  return network->getNetworkDef()->getNodeNameFromNodeID((MR::NodeID)nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getNodeTagCount(InstanceID id, commsNodeID nodeID)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return 0);
  NMP_ASSERT(network->getNetworkDef()->getNodeTagTable());
  return network->getNetworkDef()->getNodeTagTable()->getNumTagsOnNode((MR::NodeID)nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getNodeTag(InstanceID id, commsNodeID nodeID, uint32_t index)
{
  // "Unknown" is also returned by MR::NetworkDef::getNodeNameFromNodeID().
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return "Unknown");
  NMP_ASSERT(network->getNetworkDef()->getNodeTagTable());
  return network->getNetworkDef()->getNodeTagTable()->getTagOnNode((MR::NodeID)nodeID, (uint16_t) index); 
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getMessageName(InstanceID id, NetworkMessageID requestID)
{
  // "Unknown" is also returned by MR::NetworkDef::getRequestNameFromRequestID().
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return "Unknown");
  return network->getNetworkDef()->getMessageNameFromMessageID((MR::MessageID)requestID);
}

//----------------------------------------------------------------------------------------------------------------------
NetworkMessageID SimpleDataManager::getMessageID(InstanceID id, const char* utf8Name)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return (NetworkMessageID)MR::INVALID_MESSAGE_ID);
  return network->getNetworkDef()->getMessageIDFromMessageName(utf8Name);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getAnimSetCount(const GUID& guid) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return 0);
  return networkDef->getNumAnimSets();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getNodeCount(const GUID& guid) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return 0);
  return networkDef->getNumNodeDefs();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getMessageCount(const GUID& guid) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return 0);
  return networkDef->getNumMessages();
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getNodeName(const GUID& guid, commsNodeID nodeID) const
{
  // "Unknown" is also returned by MR::NetworkDef::getNodeNameFromNodeID().
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return "Unknown");
  return networkDef->getNodeNameFromNodeID((MR::NodeID)nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getNodeTagCount(const GUID& guid, commsNodeID nodeID) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return 0);
  NMP_ASSERT(networkDef->getNodeTagTable());
  return networkDef->getNodeTagTable()->getNumTagsOnNode((MR::NodeID)nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getNodeTag(const GUID& guid, commsNodeID nodeID, uint32_t index) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return "Unknown");
  NMP_ASSERT(networkDef->getNodeTagTable());
  return networkDef->getNodeTagTable()->getTagOnNode((MR::NodeID)nodeID, (uint16_t)index); 
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeType SimpleDataManager::getNodeTypeID(const GUID& guid, commsNodeID nodeID) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return 0xFFFFFFFF);

  MR::NodeDef* nodeDef = networkDef->getNodeDef((MR::NodeID)nodeID);
  if (nodeDef)
    return nodeDef->getNodeTypeID();

  return MR::INVALID_NODE_TYPE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleDataManager::getMessageName(const GUID& guid, NetworkMessageID requestID) const
{
  // "Unknown" is also returned by MR::NetworkDef::getRequestNameFromRequestID().
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return "Unknown");
  return networkDef->getMessageNameFromMessageID((MR::NodeID)requestID);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getMessageIDs(const GUID& guid, NetworkMessageID* requestIDs, uint32_t maxRequestIDs) const
{
  MR::NetworkDef* const networkDef = SIMPLE_DATA_MANAGER_GET_NETWORK_DEF(return 0);

  const NMP::OrderedStringTable* const stringTable = networkDef->getMessageIDNamesTable();
  stringTable->getIDs(requestIDs, maxRequestIDs);
  return stringTable->getNumEntries();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getFrameIndex()
{
  return m_frameIndex;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setStepInterval(float dt)
{
  m_stepInterval = dt;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setFrameRate(float fps)
{
  m_frameRate = fps;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::sendMessage(InstanceID id, commsNodeID nodeID, const MR::Message& message)
{
  NMP_ASSERT(canSendStateMachineRequests());
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);
  return network->sendMessage((MR::NodeID)nodeID, message);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::broadcastMessage(InstanceID id, const MR::Message& message)
{
  NMP_ASSERT(canSendStateMachineRequests());
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);
  return network->broadcastMessage(message) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setCurrentState(
  InstanceID  id,
  commsNodeID stateMachineNodeID,
  commsNodeID newRootStateID)
{
  NMP_ASSERT(canSendStateMachineRequests());
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  network->queueSetStateMachineStateByNodeID((MR::NodeID)stateMachineNodeID, (MR::NodeID)newRootStateID);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setControlParameter(
  InstanceID             id,
  commsNodeID            nodeID,
  MR::NodeOutputDataType type,
  const void*            cparamData)
{
  NMP_ASSERT(canSetControlParameters());
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  switch (type)
  {
  case NODE_OUTPUT_DATA_TYPE_FLOAT:
  {
    MR::AttribDataFloat floatAttribData;
    floatAttribData.m_value = (*(const float*)cparamData);
    network->setControlParameter((MR::NodeID)nodeID, &floatAttribData);
  }
  break;

  case NODE_OUTPUT_DATA_TYPE_VECTOR3:
  {
    MR::AttribDataVector3 vector3AttribData;
    const float* const floatData = (const float*)cparamData;
    vector3AttribData.m_value.set(floatData[0], floatData[1], floatData[2]);
    network->setControlParameter((MR::NodeID)nodeID, &vector3AttribData);
  }
  break;

  case NODE_OUTPUT_DATA_TYPE_VECTOR4:
  {
    MR::AttribDataVector4 vector4AttribData;
    const float* const floatData = (const float*)cparamData;
    vector4AttribData.m_value.setXYZW(floatData[0], floatData[1], floatData[2], floatData[3]);
    network->setControlParameter((MR::NodeID)nodeID, &vector4AttribData);
  }
  break;

  case NODE_OUTPUT_DATA_TYPE_BOOL:
  {
    MR::AttribDataBool boolAttribData;
    boolAttribData.m_value = (*(const bool*)cparamData);
    network->setControlParameter((MR::NodeID)nodeID, &boolAttribData);
  }
  break;

  case NODE_OUTPUT_DATA_TYPE_INT:
  {
    MR::AttribDataInt intAttribData;
    intAttribData.m_value = (*(const int32_t*)cparamData);
    network->setControlParameter((MR::NodeID)nodeID, &intAttribData);
  }
  break;

  case NODE_OUTPUT_DATA_TYPE_UINT:
  {
    MR::AttribDataUInt uintAttribData;
    uintAttribData.m_value = (*(const uint32_t*)cparamData);
    network->setControlParameter((MR::NodeID)nodeID, &uintAttribData);
  }
  break;

  case NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER:
  {
    // Note that the calling function should have done the conversion to the actual pointer.
    MR::AttribDataPhysicsObjectPointer ptrAttribData; 
    ptrAttribData.m_value = (void*) cparamData; 
    network->setControlParameter((MR::NodeID)nodeID, &ptrAttribData); 
  }
  break;

  default:
    NMP_ASSERT_FAIL();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setDebugOutputFlags(
  InstanceID  MR_OUTPUT_DEBUG_ARG(id),
  uint32_t    MR_OUTPUT_DEBUG_ARG(flags))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  if (!network->getDispatcher() || !network->getDispatcher()->getDebugInterface())
  {
    NMP_DEBUG_MSG("info: Core debug interface not supported.");
    return false;
  }

  MR::Dispatcher* dispatcher = network->getDispatcher();
  MR::InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();
  debugInterface->debugOutputsOnlyTurnOn(flags);
#endif // MR_OUTPUT_DEBUGGING

  return true;
}

#ifdef MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setDebugOutputOnNodes(
  InstanceID              MR_OUTPUT_DEBUG_ARG(id),
  bool                    MR_OUTPUT_DEBUG_ARG(on),
  bool                    MR_OUTPUT_DEBUG_ARG(allNodes),
  bool                    MR_OUTPUT_DEBUG_ARG(allSemantics),
  uint32_t                MR_OUTPUT_DEBUG_ARG(numNodeIDs),
  commsNodeID*            MR_OUTPUT_DEBUG_ARG(nodeIDs),
  uint32_t                MR_OUTPUT_DEBUG_ARG(numSemantics),
  MR::AttribDataSemantic* MR_OUTPUT_DEBUG_ARG(semantics))
{
#ifdef MR_OUTPUT_DEBUGGING
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);
  MR::NetworkDef* networkDef = network->getNetworkDef();

  if (allNodes && allSemantics)
  {
    uint32_t numNodes = networkDef->getNumNodeDefs();
    for (MR::NodeID i = 0; i < numNodes; ++i)
    {
      MR::NodeBin* nodeBin = network->getNodeBin(i);
      nodeBin->setDebugOutputAllSemantics(on);
    }
  }
  else if (allNodes && !allSemantics)
  {
    NMP_ASSERT(numSemantics > 0);
    uint32_t numNodes = networkDef->getNumNodeDefs();
    for (MR::NodeID i = 0; i < numNodes; ++i)
    {
      for (uint32_t j = 0; j < numSemantics; ++j)
      {
        MR::NodeBin* nodeBin = network->getNodeBin(i);
        nodeBin->setDebugOutputSemantic(semantics[j], on);
      }
    }
  }
  else if (allSemantics && !allNodes)
  {
    NMP_ASSERT(numNodeIDs > 0);
    for (uint32_t i = 0; i < numNodeIDs; ++i)
    {
      MR::NodeBin* nodeBin = network->getNodeBin(nodeIDs[i]);
      nodeBin->setDebugOutputAllSemantics(on);
    }
  }
  else
  {
    NMP_ASSERT(numNodeIDs > 0);
    NMP_ASSERT(numSemantics > 0);
    for (uint32_t i = 0; i < numNodeIDs; ++i)
    {
      for (uint32_t j = 0; j < numSemantics; ++j)
      {
        MR::NodeBin* nodeBin = network->getNodeBin(nodeIDs[i]);
        nodeBin->setDebugOutputSemantic(semantics[j], on);
      }
    }
  }
#endif // MR_OUTPUT_DEBUGGING

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::enableOutputDataBuffering(
  InstanceID             id,
  commsNodeID            nodeID,
  MR::NodeOutputDataType dataType,
  bool                   enable)
{
  MR::Network* const network = SIMPLE_DATA_MANAGER_GET_NETWORK(return false);

  if (!MR::RuntimeNodeInspector::enableNodeDataBuffering(network, (MR::NodeID)nodeID, dataType, enable))
  {
    NMP_DEBUG_MSG(
      "error: Enabling node data buffering failed (nodeId=%u, dataType=%u, enable=%s)!",
      nodeID, dataType, (enable ? "true" : "false"));

    return false;
  }

  return true;
}

#else // defined (MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setDebugOutputOnNodes(
  InstanceID              NMP_UNUSED(id),
  bool                    NMP_UNUSED(on),
  bool                    NMP_UNUSED(allNodes),
  bool                    NMP_UNUSED(allSemantics),
  uint32_t                NMP_UNUSED(numNodeIDs),
  commsNodeID*            NMP_UNUSED(nodeIDs),
  uint32_t                NMP_UNUSED(numSemantics),
  MR::AttribDataSemantic* NMP_UNUSED(semantics))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::enableOutputDataBuffering(
  InstanceID             NMP_UNUSED(id),
  commsNodeID            NMP_UNUSED(nodeID),
  MR::NodeOutputDataType NMP_UNUSED(dataType),
  bool                   NMP_UNUSED(enable))
{
  return false;
}

#endif // defined (MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
void SimpleDataManager::onConnectionClosed(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getNodeOutputCount(InstanceID NMP_UNUSED(id))
{
  // Not supported by this implementation.
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::commsNodeID SimpleDataManager::getNodeOutputNodeID(InstanceID NMP_UNUSED(id), uint32_t NMP_UNUSED(outputIndex))
{
  // Not supported by this implementation, zero is returned from getNodeOutputCount() so this should never get called.
  NMP_ASSERT_FAIL();
  return (MCOMMS::commsNodeID)MR::INVALID_NODE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeOutputDataType SimpleDataManager::getNodeOutputDataType(
  InstanceID NMP_UNUSED(id),
  uint32_t NMP_UNUSED(outputIndex))
{
  // Not supported by this implementation, zero is returned from getNodeOutputCount() so this should never get called.
  NMP_ASSERT_FAIL();
  return MR::INVALID_NODE_OUTPUT_DATA_TYPE;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleDataManager::getNodeOutputDataLength(
  InstanceID NMP_UNUSED(id),
  uint32_t NMP_UNUSED(outputIndex),
  MR::NodeID& NMP_UNUSED(owningNodeID),
  MR::NodeID& NMP_UNUSED(targetNodeID),
  MR::AttribDataType& NMP_UNUSED(attribType),
  MR::AttribDataSemantic& NMP_UNUSED(attribSemantic),
  MR::AnimSetIndex& NMP_UNUSED(animSetIndex),
  MR::FrameCount& NMP_UNUSED(validFrame))
{
  // Not supported by this implementation, zero is returned from getNodeOutputCount() so this should never get called.
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::getNodeOutputData(
  InstanceID NMP_UNUSED(id),
  uint32_t NMP_UNUSED(outputIndex),
  void* NMP_UNUSED(destBuffer),
  uint32_t NMP_UNUSED(bufferLength))
{
  // Not supported by this implementation, zero is returned from getNodeOutputCount() so this should never get called.
  NMP_ASSERT_FAIL();
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::serializeTxNodeTimings(InstanceID NMP_UNUSED(id), Connection* NMP_UNUSED(connection))
{
  // Not supported by this implementation.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::serializeTxControlParamAndOpNodeAttribData(
  InstanceID NMP_UNUSED(id),
  Connection* NMP_UNUSED(connection))
{
  // Not supported by this implementation.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::serializeTxTreeNodeAttribData(InstanceID NMP_UNUSED(id), Connection* NMP_UNUSED(connection))
{
  // Not supported by this implementation.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setActiveAnimationSet(InstanceID NMP_UNUSED(id), uint32_t NMP_UNUSED(animSetIndex))
{
  // Not supported by this implementation, see canSetNetworkAnimSet().
  NMP_ASSERT(!canSetNetworkAnimSet());
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::executeCommand(InstanceID NMP_UNUSED(id), const char* NMP_UNUSED(command))
{
  // Not supported by this implementation.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::setRootTransform(InstanceID NMP_UNUSED(id), const NMP::PosQuat& NMP_UNUSED(rootTransform))
{
  // Not supported by this implementation.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleDataManager::isSimulating() const
{
  return m_isSimulating;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleDataManager::beginSimulation()
{
  m_isSimulating = true;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleDataManager::endSimulation()
{
  m_isSimulating = false;
}

} // namespace COMMS
