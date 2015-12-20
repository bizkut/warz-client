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
#include "defaultDataManager.h"

#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "morpheme/mrRuntimeNodeInspector.h"
#include "comms/commsServer.h"
#include "comms/debugPackets.h"
#include "comms/networkManagementUtils.h"
#include "iControllerMgr.h"
#include "iPhysicsMgr.h"

#include "networkDefRecordManager.h"
#include "networkInstanceRecordManager.h"
#include "runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

namespace
{
IPhysicsMgr* g_physicsMgr = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// DefaultDataManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
DefaultDataManager::DefaultDataManager(
  RuntimeTargetContext*              context,
  MCOMMS::NetworkLifecycleInterface* networkLifecycle,
  IPhysicsMgr*                       physicsMgr) :
  MCOMMS::DataManagementInterface(),
  m_networkLifecycle(networkLifecycle),
  m_networkDefManager(context->getNetworkDefManager()),
  m_networkInstanceManager(context->getNetworkInstanceManager()),
  m_frameIndex(0),
  m_stepInterval(1.0f / 60.0f),
  m_frameRate(60.0f),
  m_commandHandler(NULL),
  m_isSimulating(false)
{
  NMP_ASSERT(!g_physicsMgr);
  g_physicsMgr = physicsMgr;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultDataManager::onConnectionClosed(MCOMMS::Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getFrameIndex()
{
  return m_frameIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultDataManager::setFrameIndex(uint32_t frame)
{
  m_frameIndex = frame;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::executeCommand(MCOMMS::InstanceID instanceID, const char* command)
{
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (!instanceRecord)
  {
    return false;
  }

  if (m_commandHandler)
  {
    return m_commandHandler(command);
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
IControllerMgr* DefaultDataManager::getControllerMgr()
{
  return g_physicsMgr->getControllerManager();
}

//----------------------------------------------------------------------------------------------------------------------
IPhysicsMgr* DefaultDataManager::getPhysicsMgr()
{
  return g_physicsMgr;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::serializeTxAnimRigDefData(const MCOMMS::GUID& networkGUID, MR::AnimSetIndex index, MCOMMS::Connection* connection)
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkGUID);
  if (netDefRecord)
  {
    MR::NetworkDef* netDef = netDefRecord->getNetDef();
    MR::AnimRigDef* animRigDef = netDef->getRig(index);
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
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getMaxTransformCount(MCOMMS::InstanceID id)
{
  MR::Network* network = m_networkInstanceManager->findInstanceRecord(id)->getNetwork(); ;
  if (network == 0)
  {
#ifdef NM_DEBUG
    printf("   No such instance in getMaxTransformCount for id %u\n", id);
#endif
    return 0;
  }
  return network->getMaxTransformCount();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getCurrentTransformCount(MCOMMS::InstanceID id)
{
  MR::Network* network = m_networkInstanceManager->findInstanceRecord(id)->getNetwork(); ;
  if (network == 0)
  {
#ifdef NM_DEBUG
    printf("   No such instance in getCurrentTransformCount for id %d\n", id);
#endif
    return 0;
  }
  return network->getCurrentTransformCount();
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::getLocalTransforms(MCOMMS::InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer)
{
  NetworkInstanceRecord* const record = m_networkInstanceManager->findInstanceRecord(id);
  if (!record)
  {
    return false;
  }

  MR::Network* const network = record->getNetwork();
  if (!network)
  {
    return false;
  }

  NMP::DataBuffer* transforms = network->getTransforms();
  if (numTransforms > 0 && numTransforms >= network->getCurrentTransformCount())
  {
    buffer[0].m_pos = DefaultDataManager::getControllerMgr()->getPosition(id);
    buffer[0].m_quat = DefaultDataManager::getControllerMgr()->getOrientation(id);

    MR::AnimRigDef* rig = network->getActiveRig();
    NMP::DataBuffer* bindPose = rig->getBindPose()->m_transformBuffer;
    NMP_ASSERT(!transforms || bindPose->getLength() == transforms->getLength());
    for (uint32_t i = 1; i < bindPose->getLength(); ++i)
    {
      if (transforms && transforms->hasChannel(i))
      {
        buffer[i].m_pos = *(NMP::Vector3*)transforms->getChannelPos(i);
        buffer[i].m_quat = *(NMP::Quat*)transforms->getChannelQuat(i);
      }
      else
      {
        buffer[i].m_pos = *(NMP::Vector3*)bindPose->getChannelPos(i);
        buffer[i].m_quat = *(NMP::Quat*)bindPose->getChannelQuat(i);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::getWorldTransforms(
  MCOMMS::InstanceID id,
  uint32_t           numTransforms,
  NMP::PosQuat*      buffer)
{

  NetworkInstanceRecord* const record = m_networkInstanceManager->findInstanceRecord(id);
  if (!record)
  {
    return false;
  }

  MR::Network* const network = record->getNetwork();
  if (!network)
  {
    return false;
  }

  if (numTransforms >= network->getCurrentTransformCount())
  {
    record->computeWorldTransforms(numTransforms, buffer);
    return true;  
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::getTransformChange(
  MCOMMS::InstanceID id,
  NMP::Vector3&      deltaTranslation,
  NMP::Quat&         deltaOrientation)
{
  NMP_ASSERT(m_networkInstanceManager->findInstanceRecord(id));

  deltaTranslation = getControllerMgr()->getPositionDelta(id);
  deltaOrientation = getControllerMgr()->getOrientationDelta(id);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// if type is kFloat cparamData points to a single 32bit float
// if type is kBool cparamData points to a single 32bit int with value 0 or 1
// if type is kVector3 cparamData points to 32bit float[3]
// if type is kVector4 cparamData points to 32bit float[4]
// if type is kQuat cparamData points to 32bit float[4] in x, y, z, w order
// if type is kPhysicsObjectPointer cparamData points to a 32 bit Object ID
bool DefaultDataManager::setControlParameter(
  MCOMMS::InstanceID      instanceID,
  MCOMMS::commsNodeID     commsNodeID,
  MR::NodeOutputDataType  type,
  const void*             cparamDataToSet)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  if (type == NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER)
  {
    // Need to convert the value to an actual physics object pointer - need to do it here where
    // we have access to the physics manager.
    uint32_t physicsObjectID = (*(const uint32_t*)cparamDataToSet);
    cparamDataToSet = g_physicsMgr->getPhysicsObjectPointerFromPhysicsID(physicsObjectID);
  }

  MR::Network* network = instanceRecord->getNetwork();
  return MCOMMS::setNetworkControlParameter(network, (MR::NodeID)commsNodeID, type, cparamDataToSet);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getActiveAnimationSet(MCOMMS::InstanceID instanceID)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return 0xFFFFFFFF;

  return instanceRecord->getNetwork()->getActiveAnimSetIndex();
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setActiveAnimationSet(
  MCOMMS::InstanceID instanceID,
  uint32_t           animSetIndex)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  instanceRecord->requestAnimSetIndex(animSetIndex);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::broadcastMessage(MCOMMS::InstanceID instanceID, const MR::Message& message)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  return instanceRecord->getNetwork()->broadcastMessage(message) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::sendMessage(MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID nodeID, const MR::Message& message)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  return instanceRecord->getNetwork()->sendMessage((MR::NodeID)nodeID, message);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeTypeID(
  MCOMMS::InstanceID  instanceID,
  MCOMMS::commsNodeID nodeID)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord)
  {
    NetworkDefRecord* defRecord = instanceRecord->getNetworkDefRecord();
    MR::NetworkDef* def = defRecord->getNetDef();
    MR::NodeDef* nodeDef = def->getNodeDef((MR::NodeID)nodeID);
    if (nodeDef)
      return nodeDef->getNodeTypeID();
  }
  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getAnimSetCount(MCOMMS::InstanceID instanceID) const
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord)
  {
    NetworkDefRecord* defRecord = instanceRecord->getNetworkDefRecord();
    MR::NetworkDef* def = defRecord->getNetDef();
    return def->getNumAnimSets();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeCount(MCOMMS::InstanceID instanceID)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord)
  {
    NetworkDefRecord* defRecord = instanceRecord->getNetworkDefRecord();
    MR::NetworkDef* def = defRecord->getNetDef();
    return def->getNumNodeDefs();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultDataManager::getNodeName(
  MCOMMS::InstanceID     instanceID,
  MCOMMS::commsNodeID    nodeID)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();

  if (net == 0)
    return 0;

  return net->getNetworkDef()->getNodeNameFromNodeID((MR::NodeID)nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeTagCount(
  MCOMMS::InstanceID instanceID, 
  MCOMMS::commsNodeID nodeID)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();

  if (net == 0)
    return 0;

  return net->getNetworkDef()->getNodeTagTable()->getNumTagsOnNode((MR::NodeID)nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultDataManager::getNodeTag(
  MCOMMS::InstanceID instanceID, 
  MCOMMS::commsNodeID nodeID,
  uint32_t index)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();

  if (net == 0)
    return 0;

  return net->getNetworkDef()->getNodeTagTable()->getTagOnNode(nodeID, (uint16_t) index);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getAnimSetCount(const MCOMMS::GUID& guid) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(guid);
  if (netDefRecord)
  {
    MR::NetworkDef* netDef = netDefRecord->getNetDef();
    return netDef->getNumAnimSets();
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeCount(const MCOMMS::GUID& networkDef) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    return netDefRecord->getNetDef()->getNumNodeDefs();
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeTypeID(
  const MCOMMS::GUID& networkDef,
  MCOMMS::commsNodeID nodeID) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    MR::NetworkDef* def = netDefRecord->getNetDef();
    const MR::NodeDef* nodeDef = def->getNodeDef((MR::NodeID)nodeID);
    if (nodeDef)
      return nodeDef->getNodeTypeID();
  }
  return 0xFFFFFFFF;
}


//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeTagCount(
   const MCOMMS::GUID& networkDef,
  MCOMMS::commsNodeID nodeID) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    return netDefRecord->getNetDef()->getNodeTagTable()->getNumTagsOnNode((MR::NodeID)nodeID);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultDataManager::getNodeTag(
  const MCOMMS::GUID& networkDef,
  MCOMMS::commsNodeID nodeID,
  uint32_t index) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    return netDefRecord->getNetDef()->getNodeTagTable()->getTagOnNode(nodeID, (uint16_t) index);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getMessageCount(const MCOMMS::GUID& networkDef) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    return netDefRecord->getNetDef()->getNumMessages();
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultDataManager::getNodeName(const MCOMMS::GUID& networkDef, MCOMMS::commsNodeID nodeID) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    MR::NetworkDef* def = netDefRecord->getNetDef();
    return def->getNodeNameFromNodeID((MR::NodeID)nodeID);
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultDataManager::getMessageName(const MCOMMS::GUID& networkDef, MCOMMS::NetworkMessageID messageID) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    MR::NetworkDef* def = netDefRecord->getNetDef();
    return def->getMessageNameFromMessageID(messageID);
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getMessageIDs(
  const MCOMMS::GUID&       networkDef,
  MCOMMS::NetworkMessageID* messageIDs,
  uint32_t                  maxMessageIDs) const
{
  NetworkDefRecord* netDefRecord = m_networkDefManager->findNetworkDefRecord(networkDef);
  if (netDefRecord)
  {
    const MR::NetworkDef* def = netDefRecord->getNetDef();
    const NMP::OrderedStringTable* stringTable = (const NMP::OrderedStringTable*)def->getMessageIDNamesTable();
    stringTable->getIDs(messageIDs, maxMessageIDs);
    return stringTable->getNumEntries();
  }

  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultDataManager::getMessageName(
  MCOMMS::InstanceID       instanceID,
  MCOMMS::NetworkMessageID messageID)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();

  if (net == 0)
    return 0;

  return net->getNetworkDef()->getMessageNameFromMessageID(messageID);
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::NetworkMessageID DefaultDataManager::getMessageID(
  MCOMMS::InstanceID  instanceID,
  const char*         utf8Name)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();

  if (net == 0)
    return 0xFFFFFFFF;

  return net->getNetworkDef()->getMessageIDFromMessageName(utf8Name);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getActiveNodeCount(MCOMMS::InstanceID instanceID)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();

  if (net == 0)
    return 0;

  return net->getActiveNodeCount();
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::commsNodeID DefaultDataManager::getRootStateNodeID(
  MCOMMS::InstanceID     instanceID,
  MCOMMS::commsNodeID    statMachineNodeID)
{
  MR::Network* network = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();
  if (network)
    return (MCOMMS::commsNodeID) network->getStateMachineCurrentStateNodeId((MR::NodeID)statMachineNodeID);

  return 0xFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getTriggeredNodeEventMessageCount(MCOMMS::InstanceID instanceID)
{
  MR::Network* network = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();
  if (network)
    return network->getTriggeredNodeEventMessageCount();

  return (uint32_t)-1;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getTriggeredNodeEventMessages(MCOMMS::InstanceID instanceID, MCOMMS::NetworkMessageID* messageIDs, uint32_t maxMessages)
{
  MR::Network* network = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();
  
  if (network)
  {
    // Allocate a message ID buffer using the network's temporary memory allocator.
    NMP::TempMemoryAllocator* const tempAllocator = network->getTempMemoryAllocator();
    NMP_ASSERT(tempAllocator->getUsedBytes() == 0);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

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
      messageIDs[i] = (MCOMMS::NetworkMessageID)mrMessageIDs[i];
    }

    // Free the node ID buffer and reset the networks temporary memory allocator.
    childAllocator->memFree(mrMessageIDs);
    tempAllocator->destroyChildAllocator(childAllocator);

    // ...
    return (uint32_t)numTriggeredMessages;
  }

  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::getScatterBlendWeightData(
  MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID scatterBlendNodeID,
  float (&barycentricWeights)[3], MCOMMS::commsNodeID (&childNodeIDs)[3],
  float (&desiredMotionParams)[2],
  float (&achievedMotionParams)[2],
  bool& wasProjected)
{
  MR::Network* network = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();
  if (network)
  {
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

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getActiveNodes(
  MCOMMS::InstanceID   instanceID,
  MCOMMS::commsNodeID* nodeIDs,
  uint32_t             maxNodeIDs)
{
  MR::Network* net = m_networkInstanceManager->findInstanceRecord(instanceID)->getNetwork();
  if (net == 0)
  {
    return false;
  }

  NMP::TempMemoryAllocator* allocator = net->getTempMemoryAllocator();
  NMP_ASSERT(allocator);
  NMP::TempMemoryAllocator* childAllocator = allocator->createChildAllocator();

  MR::NodeID* mrNodeIDs = (MR::NodeID*)NMPAllocatorMemAlloc(childAllocator, sizeof(MR::NodeID) * maxNodeIDs, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(mrNodeIDs);

  const uint32_t numActiveNodesAvailable = net->getActiveNodes(mrNodeIDs, maxNodeIDs);
  for (uint32_t i = 0; i < maxNodeIDs; i++)
  {
    nodeIDs[i] = (MCOMMS::commsNodeID) mrNodeIDs[i];
  }

  childAllocator->memFree(mrNodeIDs);
  allocator->destroyChildAllocator(childAllocator);

  return numActiveNodesAvailable;
}

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
// TODO: This mapping function is a side affect of the way we are currently passing control param attrib data to connect.
//       Connect code needs to be updated to remove this nasty necessity.
MR::AttribDataSemantic getAttribSemanticFromType(MR::AttribDataType attribType)
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
  case MR::ATTRIB_TYPE_PHYSICS_OBJECT_POINTER:
    attribSemantic = MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER;
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
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setDebugOutputFlags(
  MCOMMS::InstanceID  MR_OUTPUT_DEBUG_ARG(id),
  uint32_t            MR_OUTPUT_DEBUG_ARG(flags))
{
#ifdef MR_OUTPUT_DEBUGGING
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(id);
  if (instanceRecord && instanceRecord->getNetwork() && instanceRecord->getNetwork()->getDispatcher() && instanceRecord->getNetwork()->getDispatcher()->getDebugInterface())
  {
    instanceRecord->getNetwork()->getDispatcher()->getDebugInterface()->debugOutputsOnlyTurnOn(flags);
    return true;
  }
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setDebugOutputOnNodes(
  MCOMMS::InstanceID       MR_OUTPUT_DEBUG_ARG(instanceID),
  bool                     MR_OUTPUT_DEBUG_ARG(on),
  bool                     MR_OUTPUT_DEBUG_ARG(allNodes),
  bool                     MR_OUTPUT_DEBUG_ARG(allSemantics),
  uint32_t                 MR_OUTPUT_DEBUG_ARG(numNodeIDs),
  MCOMMS::commsNodeID*     MR_OUTPUT_DEBUG_ARG(nodeIDs),
  uint32_t                 MR_OUTPUT_DEBUG_ARG(numSemantics),
  MR::AttribDataSemantic*  MR_OUTPUT_DEBUG_ARG(semantics))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord && instanceRecord->getNetwork())
  {
    MR::Network* net = instanceRecord->getNetwork();
    MR::NetworkDef* netDef = net->getNetworkDef();

    // the 'everything on/off case'
    if (allNodes && allSemantics)
    {
      uint32_t numNodes = netDef->getNumNodeDefs();
      for (MR::NodeID n = 0; n < numNodes; ++n)
      {
        MR::NodeBin* nodeBin = net->getNodeBin(n);
        nodeBin->setDebugOutputAllSemantics(on);
      }
    }
    else if (allNodes && !allSemantics)
    {
      NMP_ASSERT(numSemantics > 0);
      uint32_t numNodes = netDef->getNumNodeDefs();
      for (MR::NodeID n = 0; n < numNodes; ++n)
      {
        for (uint32_t s = 0; s < numSemantics; s++)
        {
          MR::NodeBin* nodeBin = net->getNodeBin(n);
          nodeBin->setDebugOutputSemantic(semantics[s], on);
        }
      }
    }
    else if (allSemantics && !allNodes)
    {
      NMP_ASSERT(numNodeIDs > 0);
      for (uint32_t n = 0; n < numNodeIDs; n++)
      {
        MR::NodeBin* nodeBin = net->getNodeBin(nodeIDs[n]);
        nodeBin->setDebugOutputAllSemantics(on);
      }
    }
    else // !allNodes && !allSemantics .. the precision case
    {
      NMP_ASSERT(numNodeIDs > 0);
      NMP_ASSERT(numSemantics > 0);
      for (uint32_t n = 0; n < numNodeIDs; n++)
      {
        for (uint32_t s = 0; s < numSemantics; s++)
        {
          MR::NodeBin* nodeBin = net->getNodeBin(nodeIDs[n]);
          nodeBin->setDebugOutputSemantic(semantics[s], on);
        }
      }
    }

    return true;
  }
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::enableOutputDataBuffering(
  MCOMMS::InstanceID     MR_OUTPUT_DEBUG_ARG(id),
  MCOMMS::commsNodeID    MR_OUTPUT_DEBUG_ARG(nodeID),
  MR::NodeOutputDataType MR_OUTPUT_DEBUG_ARG(dataType),
  bool                   MR_OUTPUT_DEBUG_ARG(enable))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(id);
  if (!instanceRecord)
  {
    return false;
  }

  MR::Network* const net = instanceRecord->getNetwork();
  NMP_ASSERT(net);

  // Enable/Disable buffering on the node.
  if (!MR::RuntimeNodeInspector::enableNodeDataBuffering(net, (MR::NodeID)nodeID, dataType, enable))
  {
    return false;
  }

  if (enable)
  {
    instanceRecord->addNodeOutputRecord(nodeID, dataType);
  }
  else
  {
    instanceRecord->removeNodeOutputRecord(nodeID, dataType);
  }

  return true;
#else
  return false;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeOutputCount(
  MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(instanceID))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);

  if (!instanceRecord)
  {
    return 0;
  }
  else
  {
    return instanceRecord->getNumNodeOutputs();
  }
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::commsNodeID DefaultDataManager::getNodeOutputNodeID(
  MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t           MR_OUTPUT_DEBUG_ARG(outputIndex))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);

  if (!instanceRecord)
  {
    return MR::INVALID_NODE_ID;
  }
  else
  {
    const NodeOutputRecord* const outRecord = instanceRecord->getNodeOutputRecord(outputIndex);
    return outRecord->getNodeID();
  }
#else
  return MR::INVALID_NODE_ID;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeOutputDataType DefaultDataManager::getNodeOutputDataType(
  MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t           MR_OUTPUT_DEBUG_ARG(outputIndex))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);

  if (!instanceRecord)
  {
    return MR::INVALID_NODE_OUTPUT_DATA_TYPE;
  }
  else
  {
    const NodeOutputRecord* const outRecord = instanceRecord->getNodeOutputRecord(outputIndex);
    return outRecord->getNodeOutputDataType();
  }
#else
  return MR::INVALID_NODE_OUTPUT_DATA_TYPE;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultDataManager::getNodeOutputDataLength(
  MCOMMS::InstanceID      MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t                MR_OUTPUT_DEBUG_ARG(outputIndex),
  MR::NodeID&             MR_OUTPUT_DEBUG_ARG(owningNodeID),
  MR::NodeID&             MR_OUTPUT_DEBUG_ARG(targetNodeID),
  MR::AttribDataType&     MR_OUTPUT_DEBUG_ARG(attribType),
  MR::AttribDataSemantic& MR_OUTPUT_DEBUG_ARG(attribSemantic),
  MR::AnimSetIndex&       MR_OUTPUT_DEBUG_ARG(animSetIndex),
  MR::FrameCount&         MR_OUTPUT_DEBUG_ARG(validFrame))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (!instanceRecord)
  {
    return 0;
  }

  const NodeOutputRecord* const outRecord = instanceRecord->getNodeOutputRecord(outputIndex);
  MR::Network* const net = instanceRecord->getNetwork();

  owningNodeID = (MR::NodeID)outRecord->getNodeID();

  const uint32_t dataSize = MR::RuntimeNodeInspector::getNodeBufferedDataTypeSize(// TODO: unify these ids
    net,
    owningNodeID,
    (MR::NodeOutputDataType) outRecord->getNodeOutputDataType(),
    targetNodeID,
    attribType,
    attribSemantic,
    animSetIndex,
    validFrame);

  return dataSize;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::serializeTxNodeTimings(
  MCOMMS::InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  MCOMMS::Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  MR::Network* net = instanceRecord->getNetwork();
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

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::serializeTxControlParamAndOpNodeAttribData(
  MCOMMS::InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  MCOMMS::Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  MR::Network* net = instanceRecord->getNetwork();
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
bool DefaultDataManager::getNodeOutputData(
  MCOMMS::InstanceID MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t           MR_OUTPUT_DEBUG_ARG(outputIndex),
  void*              MR_OUTPUT_DEBUG_ARG(destBuffer),
  uint32_t           MR_OUTPUT_DEBUG_ARG(bufferLength))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (!instanceRecord)
  {
    return false;
  }

  const NodeOutputRecord* const outRecord = instanceRecord->getNodeOutputRecord(outputIndex);
  MR::Network* const net = instanceRecord->getNetwork();
  MR::NodeOutputDataType nodeOutputDataType = (MR::NodeOutputDataType) outRecord->getNodeOutputDataType();

  MR::AttribData* attribData = MR::RuntimeNodeInspector::getNodeAttribDataForOutputDataType(
    net,
    (MR::NodeID)outRecord->getNodeID(),
    nodeOutputDataType);

  if (attribData)
  {
    // For Physics object IDs swap the pointer for an ID
    MR::AttribDataPhysicsObjectPointer attribDataPhysicsObjectPointer;
    if (nodeOutputDataType == NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER)
    {
      attribDataPhysicsObjectPointer = *((MR::AttribDataPhysicsObjectPointer*) attribData);
      attribData = &attribDataPhysicsObjectPointer;

      MR::PhysicsObjectID physicsObjectID = g_physicsMgr->getPhysicsObjectIDFromPhysicsObjectPointer(
        attribDataPhysicsObjectPointer.m_value);

      attribDataPhysicsObjectPointer.m_value = (void*)(size_t)physicsObjectID;
    }


    return MR::RuntimeNodeInspector::serialiseAttribData( net,
                                                          (MR::NodeID)outRecord->getNodeID(),
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
void DefaultDataManager::serialiseAndBufferDebugAttribData(
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
void DefaultDataManager::serialiseAndBufferDebugOutputCPAttribData(
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
bool DefaultDataManager::serializeTxTreeNodeAttribData(MCOMMS::InstanceID  instanceID, MCOMMS::Connection *connection)
{
#ifdef MR_ATTRIB_DEBUG_BUFFERING
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  MR::Network* net = instanceRecord->getNetwork();
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
           !nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE) && // Don't send back control param or operator node data here (handled else where).
           net->nodeIsActive(i)) ||                                     // Only send back active node data.
           nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE)     // Always send back state machine data (even if it is inactive).
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
const char* DefaultDataManager::getSemanticName(MR::AttribDataSemantic semantic)
{
  return MR::AttribAddress::getAttribSemanticName(semantic);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setStepInterval(float dt)
{
  m_stepInterval = dt;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setFrameRate(float fps)
{
  m_frameRate = fps;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
float DefaultDataManager::getStepInterval()
{
  return m_stepInterval;
}

//----------------------------------------------------------------------------------------------------------------------
float DefaultDataManager::getFrameRate() const
{
  return m_frameRate;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setCurrentState(
  MCOMMS::InstanceID     instanceID,
  MCOMMS::commsNodeID    stateMachineNodeID,
  MCOMMS::commsNodeID    newRootStateID)
{
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(instanceID);
  if (instanceRecord == 0)
    return false;

  instanceRecord->getNetwork()->queueSetStateMachineStateByNodeID(
    (MR::NodeID)stateMachineNodeID,
    (MR::NodeID)newRootStateID);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::canSetControlParameters() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::canSetNetworkAnimSet() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::canSendStateMachineRequests() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::setRootTransform(MCOMMS::InstanceID id, const NMP::PosQuat& rootTransform)
{
  if (!g_physicsMgr)
  {
    return false;
  }

  if (!g_physicsMgr->getControllerManager())
  {
    return false;
  }

  IControllerMgr* controllerMgr = g_physicsMgr->getControllerManager();
  controllerMgr->teleport(id, rootTransform);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultDataManager::isSimulating() const
{
  return m_isSimulating;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultDataManager::endSimulation()
{
  m_isSimulating = false;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultDataManager::beginSimulation()
{
  m_isSimulating = true;
}
