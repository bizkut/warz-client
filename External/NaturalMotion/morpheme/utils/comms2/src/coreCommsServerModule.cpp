// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "comms/coreCommsServerModule.h"
#include "comms/assetManagerCommandsHandler.h"
#include "comms/coreCommandsHandler.h"
#include "comms/runtimeTargetInterface.h"
#include "comms/corePackets.h"
#include "comms/connection.h"
#include "comms/commsServer.h"
#include "comms/debugPackets.h"

#include "NMPlatform/NMFastHeapAllocator.h"
#include "morpheme/mrAttribAddress.h"

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
CoreCommsServerModule::CoreCommsServerModule() :
  m_sizeOfSynchronizedObjectsCache(0),
  m_numSceneObjectsAtLastSynch(0),
  m_sceneObjectsAtLastSynch(0)
{
  RuntimeTargetInterface* runtimeTarget = getRuntimeTarget();
  m_coreCmdHandler = new CoreCommandsHandler(runtimeTarget, this);
  addCommandsHandler(m_coreCmdHandler);

  m_assetManagerCmdHandler = new AssetManagerCommandsHandler(runtimeTarget, this);
  addCommandsHandler(m_assetManagerCmdHandler);
}

//----------------------------------------------------------------------------------------------------------------------
CoreCommsServerModule::~CoreCommsServerModule()
{
  removeCommandsHandler(m_assetManagerCmdHandler);

  delete m_assetManagerCmdHandler;
  m_assetManagerCmdHandler = 0;

  removeCommandsHandler(m_coreCmdHandler);

  delete m_coreCmdHandler;
  m_coreCmdHandler = 0;

  if (m_sizeOfSynchronizedObjectsCache)
  {
    NMP::Memory::memFree(m_sceneObjectsAtLastSynch);
  }

  m_sceneObjectsAtLastSynch = 0;
  m_numSceneObjectsAtLastSynch = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::sendInstanceFrameData(InstanceID instanceID, Connection* connection)
{
  RuntimeTargetInterface* target = getRuntimeTarget();

  MCOMMS::NetworkInstancePacket nip;
  nip.m_networkId = instanceID;

  DataManagementInterface* dataMgr = target->getDataManager();
  NMP_ASSERT(dataMgr);

  nip.m_frameIndex = dataMgr->getFrameIndex();
#if defined (MR_OUTPUT_DEBUGGING)
  nip.m_nodeOutputCount = dataMgr->getNodeOutputCount(instanceID);
#else
  nip.m_nodeOutputCount = 0;
#endif
  nip.m_animSetIndex = dataMgr->getActiveAnimationSet(instanceID);
  nip.m_maxTransformCount = dataMgr->getMaxTransformCount(instanceID);
  uint32_t numActiveNodes = dataMgr->getActiveNodeCount(instanceID);
  nip.m_numActiveNodes = numActiveNodes;

  mcommsBufferDataPacket(nip, connection);

  BeginInstanceSectionPacket beginInstanceData(MCOMMS::kCoreDataSectionType);
  mcommsBufferDataPacket(beginInstanceData, connection);

  {
    //-----------------------------------------
    // Send world transform buffer.
    uint32_t numTransforms = dataMgr->getCurrentTransformCount(instanceID);
    if (numTransforms > 0)
    {
      MCOMMS::TransformBufferPacket* tbp =
        MCOMMS::TransformBufferPacket::create(connection->getDataBuffer(), numTransforms);

      dataMgr->getLocalTransforms(instanceID, numTransforms, tbp->getTransforms());

      NMP::Quat oriChange;
      NMP::Vector3 transChange;
      dataMgr->getTransformChange(instanceID, transChange, oriChange);

      tbp->m_poseChange.m_pos = transChange;
      tbp->m_poseChange.m_quat = oriChange;
      mcommsSerializeDataPacket(*tbp);
    }
    else
    {
      NMP_MSG("MorphemeComms: getCurrentTransformCount = 0");
      MCOMMS::TransformBufferPacket tbp;
      tbp.m_numTransforms = 0;
      tbp.m_poseChange.m_pos = NMP::Vector3::InitZero;
      tbp.m_poseChange.m_quat = NMP::Quat::kIdentity;
      mcommsBufferDataPacket(tbp, connection);
      NMP_ASSERT_FAIL();
    }
  }

  {
    //-----------------------------------------
    // Send the list of active nodes.
    NMP::TempMemoryAllocator* tempAllocator = connection->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

    MCOMMS::commsNodeID* nodeIDs =
      (MCOMMS::commsNodeID*)NMPAllocatorMemAlloc(childAllocator,
       numActiveNodes * sizeof(MCOMMS::commsNodeID), NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(nodeIDs);

    const uint32_t numActiveNodesAvailable = dataMgr->getActiveNodes(
          instanceID,
          nodeIDs,
          numActiveNodes);

    NMP_ASSERT(numActiveNodesAvailable == numActiveNodes);

    MCOMMS::ActiveNodesPacket* activeNodes =
      MCOMMS::ActiveNodesPacket::create(connection->getDataBuffer(), numActiveNodes);
    memcpy(activeNodes->getIDs(), nodeIDs, numActiveNodes * sizeof(MCOMMS::commsNodeID));
    mcommsSerializeDataPacket(*activeNodes);

    childAllocator->memFree(nodeIDs);

    //-----------------------------------------
    // Send the list of any triggered messages which were emitted from node events this frame.

    uint32_t numTriggeredMessages = dataMgr->getTriggeredNodeEventMessageCount(instanceID);
    if(numTriggeredMessages != 0 && numTriggeredMessages != (uint32_t) -1)
    {
      MCOMMS::NetworkMessageID* messageIDs = (MCOMMS::NetworkMessageID*)
        childAllocator->memAlloc(numTriggeredMessages * sizeof(MCOMMS::NetworkMessageID), NMP_VECTOR_ALIGNMENT NMP_MEMORY_TRACKING_ARGS);
#ifdef NMP_ENABLE_ASSERTS
      uint32_t numTriggeredMessageIDs = 
#endif // NMP_ENABLE_ASSERTS
        dataMgr->getTriggeredNodeEventMessages(instanceID, messageIDs, numActiveNodes);
      NMP_ASSERT(numTriggeredMessageIDs == numTriggeredMessages);

      MCOMMS::NodeEventMessagesPacket* nodeEventMessages =
        MCOMMS::NodeEventMessagesPacket::create(connection->getDataBuffer(), numTriggeredMessages);
      memcpy(nodeEventMessages->getIDs(), messageIDs, numTriggeredMessages * sizeof(MCOMMS::NetworkMessageID));
      mcommsSerializeDataPacket(*nodeEventMessages);

      childAllocator->memFree(messageIDs);
    }

    if (numActiveNodesAvailable != (uint32_t) - 1)
    {
      // Count the root nodes
      // A state machine will always be on the active node list if it is being used
      // by the network.  This means we can restrict a search to those nodes on the active list
      uint32_t numRootStateMachineNodes = 0;
      for (uint32_t testIdx = 0; testIdx < numActiveNodes; ++testIdx)
      {
        if (dataMgr->getNodeTypeID(instanceID, nodeIDs[testIdx]) == NODE_TYPE_STATE_MACHINE)
        {
          ++numRootStateMachineNodes;
        }
      }

      MCOMMS::commsNodeID* rootIDs =
        (MCOMMS::commsNodeID*)NMPAllocatorMemAlloc(childAllocator, 
          numRootStateMachineNodes * sizeof(MCOMMS::commsNodeID), NMP_VECTOR_ALIGNMENT);
      NMP_ASSERT(rootIDs);

      unsigned int rootStatePos = 0;
      for (uint32_t testIdx = 0; testIdx < numActiveNodes; ++testIdx)
      {
        MCOMMS::commsNodeID nodeID = nodeIDs[testIdx];
        if (dataMgr->getNodeTypeID(instanceID, nodeID) == NODE_TYPE_STATE_MACHINE)
        {
          MCOMMS::commsNodeID rootID = dataMgr->getRootStateNodeID(instanceID, nodeID);
          rootIDs[rootStatePos] = rootID;
          rootStatePos++;
        }
      }

      MCOMMS::RootStateMachineNodesPacket* rootStates =
        MCOMMS::RootStateMachineNodesPacket::create(connection->getDataBuffer(), numRootStateMachineNodes);
      memcpy(rootStates->getIDs(), rootIDs, numRootStateMachineNodes * sizeof(MCOMMS::commsNodeID));
      mcommsSerializeDataPacket(*rootStates);

      childAllocator->memFree(rootIDs);

      for (uint32_t testIdx = 0; testIdx < numActiveNodes; ++testIdx)
      {
        if (dataMgr->getNodeTypeID(instanceID, nodeIDs[testIdx]) == NODE_TYPE_SCATTER_BLEND_2D)
        {
          MCOMMS::commsNodeID nodeID = nodeIDs[testIdx];

          float barycentricWeights[3];
          MCOMMS::commsNodeID childNodeIDs[3];
          float desiredMotionParams[2];
          float achievedMotionParams[2];
          bool wasProjected;

          bool scatterResult = dataMgr->getScatterBlendWeightData(instanceID, nodeID, barycentricWeights, childNodeIDs, desiredMotionParams, achievedMotionParams, wasProjected);
          if(scatterResult)
          {
            MCOMMS::ScatterBlendWeightsPacket* scatterBlendWeights =
              MCOMMS::ScatterBlendWeightsPacket::create(connection->getDataBuffer(), nodeID, barycentricWeights, childNodeIDs, desiredMotionParams, achievedMotionParams, wasProjected);
            mcommsSerializeDataPacket(*scatterBlendWeights);
          }
        }
      }

#if defined (MR_OUTPUT_DEBUGGING)
      //-----------------------------------------
      // Send a packet for each node output
      {
        uint32_t numOutputs = dataMgr->getNodeOutputCount(instanceID);
        for (uint32_t outputIndex = 0; outputIndex < numOutputs; ++outputIndex)
        {
          MR::NodeOutputDataType dataType = dataMgr->getNodeOutputDataType(instanceID, outputIndex);

          MR::NodeID             owningNodeID = MR::INVALID_NODE_ID;
          MR::NodeID             targetNodeID = MR::INVALID_NODE_ID;
          MR::AttribDataType     attribType = MR::INVALID_ATTRIB_TYPE;
          MR::AttribDataSemantic attribSemantic = MR::ATTRIB_SEMANTIC_NA;
          MR::AnimSetIndex       animSetIndex = MR::ANIMATION_SET_ANY;
          MR::FrameCount         validFrame = MR::VALID_FOREVER;
          uint32_t dataLength = dataMgr->getNodeOutputDataLength(
                                  instanceID,
                                  outputIndex,
                                  owningNodeID,
                                  targetNodeID,
                                  attribType,
                                  attribSemantic,
                                  animSetIndex,
                                  validFrame);
          MCOMMS::NodeOutputDataPacket* nodeOutputData = MCOMMS::NodeOutputDataPacket::create(
                connection->getDataBuffer(),
                dataLength,
                owningNodeID,
                targetNodeID,
                attribType,
                attribSemantic,
                MCOMMS::CommsServer::getInstance()->getTokenForString(dataMgr->getSemanticName(attribSemantic)),
                animSetIndex,
                validFrame);
          nodeOutputData->m_nodeOutputType = dataType;

          // Is there any data to get?
          if( dataLength != 0 )
          {
            // copy data into packet
            bool nodeOutputResult = dataMgr->getNodeOutputData(
                                      instanceID,
                                      outputIndex,
                                      nodeOutputData->getData(),
                                      dataLength);

            NMP_ASSERT(nodeOutputResult); (void)nodeOutputResult;
          }

          mcommsSerializeDataPacket(*nodeOutputData);
        }
      }
#endif // defined (MR_OUTPUT_DEBUGGING)
    }

    tempAllocator->destroyChildAllocator(childAllocator);
  }

  EndInstanceSectionPacket endInstanceData(MCOMMS::kCoreDataSectionType);
  mcommsBufferDataPacket(endInstanceData, connection);

#if defined (MR_OUTPUT_DEBUGGING)
  //-----------------------------------------
  // Send a control params debug output section.
  dataMgr->serializeTxControlParamAndOpNodeAttribData(instanceID, connection);

  //-----------------------------------------
  // Send a tree node debug output section.
  dataMgr->serializeTxTreeNodeAttribData(instanceID, connection);

  //-----------------------------------------
  // Send a node profile timings section.
  dataMgr->serializeTxNodeTimings(instanceID, connection);
#endif // defined (MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::sendSceneObjectsUpdate(MCOMMS::Connection* connection)
{
  BeginFrameSegmentPacket beginFrameSegment(MCOMMS::FRAMESEGTYPE_SCENEOBJECTS);
  mcommsBufferDataPacket(beginFrameSegment, connection);

  RuntimeTargetInterface* const target = getRuntimeTarget();
  SceneObjectManagementInterface* const sceneObjectMgr = target->getSceneObjectManager();
  if (sceneObjectMgr)
  {
    NMP::TempMemoryAllocator* const tempAllocator = connection->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

    const uint32_t numSceneObjects = sceneObjectMgr->getNumSceneObjects();

    MCOMMS::SceneObject** const sceneObjects = (MCOMMS::SceneObject**)
        NMPAllocatorMemAlloc(childAllocator, numSceneObjects * sizeof(MCOMMS::SceneObject*), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(sceneObjects);

    const uint32_t numSceneObjectsAvailable = sceneObjectMgr->getSceneObjects(sceneObjects, numSceneObjects);
    NMP_ASSERT(numSceneObjectsAvailable == numSceneObjects);

    // Send list of active scene object IDs first
    MCOMMS::ActiveSceneObjectsPacket* activeSceneObjects =
      MCOMMS::ActiveSceneObjectsPacket::create(connection->getDataBuffer(), numSceneObjectsAvailable);

    for (uint32_t i = 0; i < numSceneObjectsAvailable; ++i)
    {
      activeSceneObjects->getIDs()[i] = sceneObjects[i]->getSceneObjectID();
    }
    mcommsSerializeDataPacket(*activeSceneObjects);

    // Then send the attributes of each active scene object
    for (uint32_t i = 0; i < numSceneObjectsAvailable; ++i)
    {
      sendSceneObjectUpdate(connection, sceneObjects[i]);
    }

    childAllocator->memFree(sceneObjects);
    tempAllocator->destroyChildAllocator(childAllocator);
  }

  EndFrameSegmentPacket endFrameSegment(MCOMMS::FRAMESEGTYPE_SCENEOBJECTS);
  mcommsBufferDataPacket(endFrameSegment, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::sendSceneObjectUpdate(MCOMMS::Connection* connection, const SceneObject* sceneObject)
{
  const bool isUpdated = !sceneObject->isFirstUpdate();
  const uint32_t numAttributes = sceneObject->getNumAttributes();
  uint32_t numAttributesToSend = 0;

  // Count dynamic attributes.
  for (uint32_t i = 0; i < numAttributes; ++i)
  {
    const MCOMMS::Attribute* attribute = sceneObject->getAttribute(i);
    if (attribute->isDynamic() || isUpdated)
    {
      ++numAttributesToSend;
    }
  }

  MCOMMS::SceneObjectUpdatePacket sceneObj;
  sceneObj.m_numDynamicAttributes = numAttributesToSend;
  sceneObj.m_sceneObjectId = sceneObject->getSceneObjectID();
  mcommsBufferDataPacket(sceneObj, connection);

  for (uint32_t i = 0; i < numAttributes; ++i)
  {
    if (sceneObject->getAttribute(i)->isDynamic())
    {
      MCOMMS::AttributeUpdatePacket* attrPkt =
        MCOMMS::AttributeUpdatePacket::create(connection->getDataBuffer(), sceneObject->getAttribute(i));
      mcommsSerializeDataPacket(*attrPkt);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::sendFrameData(Connection* connection)
{
  sendSceneObjectsUpdate(connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::preFrameUpdate()
{
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::postFrameUpdate()
{
  RuntimeTargetInterface* target = getRuntimeTarget();
  SceneObjectManagementInterface* const sceneObjectMgr = target->getSceneObjectManager();

  if (sceneObjectMgr)
  {
    if (CommsServer::getInstance()->getNumConnections() != 0)
    {
      // Need to use a temp allocator, get the one owned by the first connection.
      Connection* const connection = CommsServer::getInstance()->getConnection(0);
      NMP::TempMemoryAllocator* const tempAllocator = connection->getTempAllocator();
      NMP_ASSERT(tempAllocator);
      NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

      const uint32_t numSceneObjects = sceneObjectMgr->getNumSceneObjects();

      MCOMMS::SceneObject** const sceneObjects = (MCOMMS::SceneObject**)
          NMPAllocatorMemAlloc(childAllocator, numSceneObjects * sizeof(MCOMMS::SceneObject*), NMP_NATURAL_TYPE_ALIGNMENT);
      NMP_ASSERT(sceneObjects);

      const uint32_t numSceneObjectsAvailable = sceneObjectMgr->getSceneObjects(sceneObjects, numSceneObjects);
      NMP_ASSERT(numSceneObjectsAvailable == numSceneObjects);

      for (uint32_t i = 0; i < numSceneObjectsAvailable; ++i)
      {
        SceneObject* const sceneObject = sceneObjects[i];

        if (sceneObject)
        {
          sceneObject->setUpdated();
        }
        else
        {
          NMP_ASSERT_FAIL();
        }
      }

      childAllocator->memFree(sceneObjects);
      tempAllocator->destroyChildAllocator(childAllocator);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::onStartSession(Connection* connection)
{
  // End the simulation if the master connection closes
  if (connection->isMaster())
  {
    DataManagementInterface* dataMgr = getRuntimeTarget()->getDataManager();
    if (dataMgr)
    {
      dataMgr->beginSimulation();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::onStopSession(Connection* connection)
{
  // End the simulation if the master connection closes
  if (connection->isMaster())
  {
    DataManagementInterface* dataMgr = getRuntimeTarget()->getDataManager();
    if (dataMgr)
    {
      dataMgr->endSimulation();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::onConnectionClosed(Connection* connection)
{
  m_coreCmdHandler->onConnectionClosed(connection);

  // End the simulation if the master connection closes
  if (connection->isMaster())
  {
    DataManagementInterface* dataMgr = getRuntimeTarget()->getDataManager();
    if (dataMgr)
    {
      dataMgr->endSimulation();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::describeSceneObjects(MCOMMS::Connection* connection)
{
  RuntimeTargetInterface* const target = getRuntimeTarget();
  SceneObjectManagementInterface* const sceneObjectMgr = target->getSceneObjectManager();

  if (sceneObjectMgr)
  {
    const uint32_t numSceneObjects = sceneObjectMgr->getNumSceneObjects();

    NMP::TempMemoryAllocator* const tempAllocator = connection->getTempAllocator();
    NMP_ASSERT(tempAllocator->getUsedBytes() == 0);

    MCOMMS::SceneObject** const sceneObjects = (MCOMMS::SceneObject**)
      NMPAllocatorMemAlloc(tempAllocator, numSceneObjects * sizeof(MCOMMS::SceneObject*), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(sceneObjects);

    const uint32_t numSceneObjectsAvailable = sceneObjectMgr->getSceneObjects(sceneObjects, numSceneObjects);
    NMP_ASSERT(numSceneObjectsAvailable == numSceneObjects);

    for (uint32_t i = 0; i < numSceneObjectsAvailable; ++i)
    {
      describeSceneObject(sceneObjects[i], connection);
    }

    tempAllocator->reset();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::broadcastSceneObjectDescription(const SceneObject* sceneObject)
{
  uint32_t numConnections = CommsServer::getInstance()->getNumConnections();
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    MCOMMS::Connection* connection = MCOMMS::CommsServer::getInstance()->getConnection(i);
    if (connection->isSessionRunning())
    {
      describeSceneObject(sceneObject, connection);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::broadcastSceneObjectDestruction(MCOMMS::SceneObjectID objectID)
{
  MCOMMS::SceneObjectDestroyedPacket soDestr;
  soDestr.m_sceneObjectId = objectID;

  uint32_t numConnections = CommsServer::getInstance()->getNumConnections();
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    MCOMMS::Connection* connection = MCOMMS::CommsServer::getInstance()->getConnection(i);
    if (connection->isSessionRunning())
    {
      mcommsSendDataPacket(soDestr, connection);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::sendSceneObjectDestruction(SceneObjectID objectID, Connection* connection)
{
  MCOMMS::SceneObjectDestroyedPacket soDestr;
  soDestr.m_sceneObjectId = objectID;
  mcommsSendDataPacket(soDestr, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::describeSceneObject(const SceneObject* sceneObject, Connection* connection)
{
  MCOMMS::BeginPersistentPacket beginDefPacket(MCOMMS::kSceneObjectPersistentData);
  mcommsBufferDataPacket(beginDefPacket, connection);

  uint32_t numAttributes = sceneObject->getNumAttributes();

  MCOMMS::SceneObjectDefPacket sceneObj;
  sceneObj.m_numAttributes = numAttributes;
  sceneObj.m_sceneObjectId = sceneObject->getSceneObjectID();
  mcommsBufferDataPacket(sceneObj, connection);

  for (uint32_t i = 0; i < numAttributes; ++i)
  {
    MCOMMS::AttributeDefPacket* attrPkt =
      MCOMMS::AttributeDefPacket::create(connection->getDataBuffer(), sceneObject->getAttribute(i));

    mcommsSerializeAndSendDataPacket(*attrPkt, connection);
  }

  MCOMMS::EndPersistentPacket endDefPacket;
  mcommsSendDataPacket(endDefPacket, connection); // force the buffer to flush.
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::sendDefData(const GUID& networkGUID, Connection* connection)
{
  RuntimeTargetInterface* target = getRuntimeTarget();
  DataManagementInterface* dataMgr = target->getDataManager();
  NMP_ASSERT(dataMgr);

  MR::AnimSetIndex count = (MR::AnimSetIndex)dataMgr->getAnimSetCount(networkGUID);
  for (MR::AnimSetIndex index = 0; index != count; ++index)
  {
    dataMgr->serializeTxAnimRigDefData(networkGUID, index, connection);
  }

  // Describe network definition.
  MCOMMS::BeginPersistentPacket bdp(MR::kNetworkDefPersistentData);
  mcommsBufferDataPacket(bdp, connection);

  MCOMMS::NetworkDefPacket ndp;
  ndp.m_networkGUID = networkGUID;
  ndp.m_nameToken = CommsServer::getInstance()->getTokenForString("NetworkDef");
  ndp.m_numAnimSets = dataMgr->getAnimSetCount(networkGUID);
  mcommsBufferDataPacket(ndp, connection);

  uint32_t numNodes = dataMgr->getNodeCount(networkGUID);

  for (MCOMMS::commsNodeID iNode = 0; iNode < numNodes; ++iNode)
  {
    // Describe node
    uint32_t numTags = dataMgr->getNodeTagCount(networkGUID, iNode);
    
    if (connection->getDataBuffer()->getAvailableMemory() < sizeof(NodeDefPacket) + numTags * sizeof(StringToken))
    {
      connection->getDataBuffer()->flush(); 
    }

    StringToken nameToken = CommsServer::getInstance()->getTokenForString(dataMgr->getNodeName(networkGUID, (MR::NodeID)iNode));
    MCOMMS::NodeDefPacket* nodeDefPacket =
      MCOMMS::NodeDefPacket::create(connection->getDataBuffer(),
      iNode,
      dataMgr->getNodeTypeID(networkGUID, (MR::NodeID)iNode),
      nameToken,
      numTags);

    StringToken* tagTokenArray = nodeDefPacket->getTagTokens();
    for (uint32_t iTag = 0; iTag < numTags; ++iTag)
    {
      const char* curTagValue = dataMgr->getNodeTag(networkGUID, iNode,iTag);
      StringToken curToken =  CommsServer::getInstance()->getTokenForString(curTagValue);
      tagTokenArray[iTag] = curToken;
    }
    mcommsSerializeDataPacket(*nodeDefPacket);
  }

  // Describe requests.
  uint32_t numMessages = dataMgr->getMessageCount(networkGUID);
  NMP::TempMemoryAllocator *tempAllocator = connection->getTempAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  MCOMMS::NetworkMessageID* requestIDs = (MCOMMS::NetworkMessageID*)
    NMPAllocatorMemAlloc(childAllocator, numMessages * sizeof(MCOMMS::NetworkMessageID), NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(requestIDs);

  const uint32_t numMessagesAvailable = dataMgr->getMessageIDs(networkGUID, requestIDs, numMessages);
  NMP_ASSERT(numMessagesAvailable == numMessages);

  for (uint32_t iRequest = 0; iRequest < numMessagesAvailable; ++iRequest)
  {
    MCOMMS::MessageDefPacket reqDefPacket;
    reqDefPacket.m_messageID = requestIDs[iRequest];
    reqDefPacket.m_nameToken =
      CommsServer::getInstance()->getTokenForString(dataMgr->getMessageName(networkGUID, requestIDs[iRequest]));
    mcommsBufferDataPacket(reqDefPacket, connection);
  }

  childAllocator->memFree(requestIDs);
  tempAllocator->destroyChildAllocator(childAllocator);

  // Mark the end of the definition.
  MCOMMS::EndPersistentPacket edp;
  mcommsBufferDataPacket(edp, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::initializeSceneObjectsSyncCache()
{
  // Do nothing if the are connections available. In this case the sync mechanism does the necessary work.
  if (CommsServer::getInstance()->getNumConnections() > 0)
  {
    return;
  }

  RuntimeTargetInterface* target = getRuntimeTarget();
  NMP_ASSERT(target);
  SceneObjectManagementInterface* sceneObjectMgr = target->getSceneObjectManager();
  if (sceneObjectMgr)
  {
    const uint32_t numSceneObjectsOnTarget = sceneObjectMgr->getNumSceneObjects();

    SceneObject** const sceneObjects = (SceneObject**)NMPMemoryAllocAligned(
      numSceneObjectsOnTarget * sizeof(SceneObject*),
      NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(sceneObjects);

    const uint32_t numSceneObjectsAvailable = sceneObjectMgr->getSceneObjects(sceneObjects, numSceneObjectsOnTarget);
    NMP_ASSERT(numSceneObjectsAvailable == numSceneObjectsOnTarget);

    SceneObjectID* sceneObjectIDs = (SceneObjectID*)NMPMemoryAllocAligned(
      numSceneObjectsAvailable * sizeof(SceneObjectID),
      NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(sceneObjectIDs);

    for (uint32_t i = 0; i < numSceneObjectsAvailable; ++i)
    {
      sceneObjectIDs[i] = sceneObjects[i]->getSceneObjectID();
    }

    // Now store the current list of scene objects in the synchronized one.
    // Resize the cache if needed.
    if (numSceneObjectsAvailable > m_sizeOfSynchronizedObjectsCache)
    {
      if (m_sceneObjectsAtLastSynch)
      {
        NMP::Memory::memFree(m_sceneObjectsAtLastSynch);
      }

      uint32_t newSizeOfCache = NMP::maximum(numSceneObjectsOnTarget, 2 * m_sizeOfSynchronizedObjectsCache);
      m_sceneObjectsAtLastSynch = (SceneObjectID*)NMPMemoryAlloc(sizeof(SceneObjectID) * newSizeOfCache);
      m_sizeOfSynchronizedObjectsCache = newSizeOfCache;
    }

    // Copy the data.
    memcpy(m_sceneObjectsAtLastSynch, sceneObjectIDs, sizeof(SceneObjectID) * numSceneObjectsAvailable);

    m_numSceneObjectsAtLastSynch = numSceneObjectsAvailable;

    // Release any temporary allocated memory.
    NMP::Memory::memFree(sceneObjectIDs);
    NMP::Memory::memFree(sceneObjects);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommsServerModule::synchronizeSceneObjects()
{
  if (CommsServer::getInstance()->getNumConnections() == 0)
  {
    return;
  }

  RuntimeTargetInterface* target = getRuntimeTarget();
  NMP_ASSERT(target);
  SceneObjectManagementInterface* sceneObjectMgr = target->getSceneObjectManager();
  if (sceneObjectMgr)
  {
    // Need to use a temp allocator, get the one owned by the first connection.
    Connection* const connection = CommsServer::getInstance()->getConnection(0);
    NMP::TempMemoryAllocator* const tempAllocator = connection->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();
    
    const uint32_t numSceneObjectsOnTarget = sceneObjectMgr->getNumSceneObjects();

    SceneObject** const sceneObjects = (SceneObject**)
      NMPAllocatorMemAlloc(childAllocator, numSceneObjectsOnTarget * sizeof(SceneObject*), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(sceneObjects);

    const uint32_t numSceneObjectsAvailable = sceneObjectMgr->getSceneObjects(sceneObjects, numSceneObjectsOnTarget);
    NMP_ASSERT(numSceneObjectsAvailable == numSceneObjectsOnTarget);

    SceneObjectID* sceneObjectIDs = (SceneObjectID*)
      NMPAllocatorMemAlloc(childAllocator, numSceneObjectsAvailable * sizeof(SceneObjectID), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(sceneObjectIDs);

    for (uint32_t i = 0; i < numSceneObjectsAvailable; ++i)
    {
      sceneObjectIDs[i] = sceneObjects[i]->getSceneObjectID();
    }

    // Small optimization: count the number of running connections. As broadcasting works for running connections only
    // there's no point doing all the work and then sending it to nobody.
    if (CommsServer::getInstance()->getNumRunningConnections())
    {
      // Send a message to tell the connections about all the objects that were destroyed.
      for (uint32_t i = 0; i < m_numSceneObjectsAtLastSynch; ++i)
      {
        SceneObjectID objectID = m_sceneObjectsAtLastSynch[i];
        if (!findSceneObject(sceneObjectIDs, numSceneObjectsAvailable, objectID))
        {
          broadcastSceneObjectDestruction(objectID);
        }
      }

      // Describe new objects to all the connections.
      for (uint32_t i = 0; i < numSceneObjectsOnTarget; ++i)
      {
        SceneObjectID objectID = sceneObjects[i]->getSceneObjectID();
        if (!findSceneObject(m_sceneObjectsAtLastSynch, m_numSceneObjectsAtLastSynch, objectID))
        {
          broadcastSceneObjectDescription(sceneObjects[i]);
        }
      }
    }

    // Now store the current list of scene objects in the synchronized one.
    // Resize the cache if needed.
    if (numSceneObjectsAvailable > m_sizeOfSynchronizedObjectsCache)
    {
      if (m_sceneObjectsAtLastSynch)
      {
        NMP::Memory::memFree(m_sceneObjectsAtLastSynch);
      }

      uint32_t newSizeOfCache = NMP::maximum(numSceneObjectsOnTarget, 2 * m_sizeOfSynchronizedObjectsCache);
      m_sceneObjectsAtLastSynch = (SceneObjectID*)NMPMemoryAlloc(sizeof(SceneObjectID) * newSizeOfCache);
      NMP_ASSERT(m_sceneObjectsAtLastSynch);
      m_sizeOfSynchronizedObjectsCache = newSizeOfCache;
    }

    // Copy the data.
    memcpy(m_sceneObjectsAtLastSynch, sceneObjectIDs, sizeof(SceneObjectID) * numSceneObjectsAvailable);

    m_numSceneObjectsAtLastSynch = numSceneObjectsAvailable;

    // Release any temporary allocated memory.
    childAllocator->memFree(sceneObjects);
    childAllocator->memFree(sceneObjectIDs);
    tempAllocator->destroyChildAllocator(childAllocator);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool CoreCommsServerModule::findSceneObject(
  const SceneObjectID* sceneObjectIDs,
  uint32_t             numSceneObjects,
  SceneObjectID        objectID) const
{
  for (uint32_t i = 0; i < numSceneObjects; ++i)
  {
    if (objectID == sceneObjectIDs[i])
    {
      return true;
    }
  }

  return false;
}

} // namespace MCOMMS
