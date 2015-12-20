// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "NMPlatform/NMFastHeapAllocator.h"

#include "comms/runtimeTargetInterface.h"
#include "comms/mcomms.h"
#include "comms/packet.h"
#include "comms/connection.h"
#include "comms/commsServer.h"
#include "comms/corePackets.h"
#include "comms/coreCommandsHandler.h"
#include "comms/coreCommsServerModule.h"

#include "morpheme/mrNetwork.h"
#ifdef NMVERBOSE
  #include <stdio.h>
#endif

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE T* deserializeCommandPacket(CmdPacketBase* packet)
{
  T* destPacket = (T*)packet;
  destPacket->deserialize();
  return destPacket;
}

//----------------------------------------------------------------------------------------------------------------------
ObjectSetupHandler::ObjectSetupHandler(uint32_t objType, uint32_t numAttributes) :
  m_numAttributes(0),
  m_totalNumAttributes(numAttributes),
  m_objType(objType)
{
  m_attributes =
    (Attribute**) NMPMemoryAllocAligned(sizeof(void*) * numAttributes, sizeof(void*));
  NMP_ASSERT(m_attributes);
}

//----------------------------------------------------------------------------------------------------------------------
ObjectSetupHandler::~ObjectSetupHandler()
{
  for (uint32_t i = 0; i < m_numAttributes; ++i)
  {
    NMP::Memory::memFree(m_attributes[i]->getData());
    Attribute::destroy(m_attributes[i]);
  }

  NMP::Memory::memFree(m_attributes);
}

//----------------------------------------------------------------------------------------------------------------------
void ObjectSetupHandler::addAttribute(Attribute* attribute)
{
  m_attributes[m_numAttributes] = attribute;
  ++m_numAttributes;
}

//----------------------------------------------------------------------------------------------------------------------
CoreCommandsHandler::CoreCommandsHandler(RuntimeTargetInterface* target, CoreCommsServerModule* coreModule) :
  CommandsHandler(target),
  m_coreModule(coreModule)
{
}

//----------------------------------------------------------------------------------------------------------------------
CoreCommandsHandler::~CoreCommandsHandler()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::clear()
{
  // Check if there were pending scene object creations and clean them up.
  // If an handler is alive here, the attributes haven't been used to build the object, yet,
  // so we need to clear the data, too.
  for (ObjectSetupHandlers::iterator iter = m_objectsToSetup.begin();
       iter != m_objectsToSetup.end();
       ++iter)
  {
    ObjectSetupHandler* handler = iter->second;
    delete handler;
    iter->second = 0;
  }

  m_objectsToSetup.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::onConnectionClosed(Connection* connection)
{
  // Check if an object creation was done partially.
  // If an handler is alive here, the attributes haven't been used to build the object, yet,
  // so we need to clear the data, too.
  ObjectSetupHandlers::iterator iter = m_objectsToSetup.find(connection);
  if (iter != m_objectsToSetup.end())
  {
    ObjectSetupHandler* handler = iter->second;
    delete handler;
    m_objectsToSetup.erase(iter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool CoreCommandsHandler::doHandleCommand(CmdPacketBase* cmdPacket)
{
  uint16_t pktId = cmdPacket->hdr.m_id;
  NMP::netEndianSwap(pktId);

  switch (pktId)
  {
  case pk_IdentificationCmd:
    handleIdentificationCmd(cmdPacket);
    return true;
  case pk_LoadNetworkCmd:
    handleLoadNetworkCmd(cmdPacket);
    return true;
  case pk_CreateNetworkInstanceCmd:
    handleCreateNetworkInstanceCmd(cmdPacket);
    return true;
  case pk_ReferenceNetworkInstanceCmd:
    handleReferenceNetworkInstanceCmd(cmdPacket);
    return true;
  case pk_UnreferenceNetworkInstanceCmd:
    handleUnreferenceNetworkInstanceCmd(cmdPacket);
    return true;
  case pk_DownloadFrameDataCmd:
    handleDownloadFrameDataCmd(cmdPacket);
    return true;
  case pk_PauseSessionCmd:
    handlePauseSessionCmd(cmdPacket);
    return true;
  case pk_StartSessionCmd:
    handleStartSessionCmd(cmdPacket);
    return true;
  case pk_StopSessionCmd:
    handleStopSessionCmd(cmdPacket);
    return true;
  case pk_DownloadSceneObjectsCmd:
    handleDownloadSceneObjectsCmd(cmdPacket);
    return true;
  case pk_DownloadGlobalDataCmd:
    handleDownloadGlobalDataCmd(cmdPacket);
    return true;
  case pk_DownloadNetworkDefinitionCmd:
    handleDownloadNetworkDefinitionCmd(cmdPacket);
    return true;
  case pk_SetControlParameterCmd:
    handleSetControlParamCmd(cmdPacket);
    return true;
  case pk_SetRootTransformCmd:
    handleSetRootTransformCmd(cmdPacket);
    return true;
  case pk_SetAnimationSetCmd:
    handleSetAnimationSetCmd(cmdPacket);
    return true;
  case pk_SetAssetManagerAnimationSetCmd:
    handleSetAnimBrowserAnimationSetCmd(cmdPacket);
    return true;
  case pk_SendRequestCmd:
    handleSendRequestCmd(cmdPacket);
    return true;
  case pk_BroadcastRequestCmd:
    handleBroadcastRequestCmd(cmdPacket);
    return true;
  case pk_MessageBufferCmd:
    handleMessageCmd(cmdPacket);
    return true; 
  case pk_SetCurrentStateCmd:
    handleSetCurrentStateCmd(cmdPacket);
    return true;
  case pk_ExecuteCommandCmd:
    handleExecuteCommandCmd(cmdPacket);
    return true;
  case pk_DestroyNetworkDefinitionCmd:
    handleDestroyNetworkDefinitionCmd(cmdPacket);
    return true;
  case pk_DestroyNetworkInstanceCmd:
    handleDestroyNetworkInstanceCmd(cmdPacket);
    return true;
  case pk_EnableOutputDataCmd:
    handleEnableOutputDataCmd(cmdPacket);
    return true;
  case pk_SetDebugOutputFlagsCmd:
    handleSetDebugOutputFlagsCmd(cmdPacket);
    return true;
  case pk_SetDebugOutputOnNodesCmd:
    handleSetDebugOutputOnNodesCmd(cmdPacket);
    return true;
  case pk_StepCmd:
    handleStepCmd(cmdPacket);
    return true;
  case pk_StepModeCmd:
    handleStepModeCmd(cmdPacket);
    return true;
  case pk_BeginSceneObjectCmd:
    handleBeginSceneObjectCmd(cmdPacket);
    return true;
  case pk_AttributeCmd:
    handleSceneObjectAttributeCmd(cmdPacket);
    return true;
  case pk_EndSceneObjectCmd:
    handleEndSceneObjectCmd(cmdPacket);
    return true;
  case pk_DestroySceneObjectCmd:
    handleDestroySceneObjectCmd(cmdPacket);
    return true;
  case pk_SetAttributeCmd:
    handleSetAttributeCmd(cmdPacket);
    return true;
  case pk_SetEnvironmentAttributeCmd:
    handleSetEnvironmentAttributeCmd(cmdPacket);
    return true;
  case pk_ClearCachedDataCmd:
    handleClearCachedDataCmd(cmdPacket);
    return true;
  case pk_CreateConstraintCmd:
    handleCreateConstraintCmd(cmdPacket);
    return true;
  case pk_RemoveConstraintCmd:
    handleRemoveConstraintCmd(cmdPacket);
    return true; 
  case pk_MoveConstraintCmd:
    handleMoveConstraintCmd(cmdPacket);
    return true;
  case pk_ApplyForceCmd:
    handleApplyForceCmd(cmdPacket);
    return true;
  default:
    break;
  }

  // Can't handle this type of command.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleIdentificationCmd(CmdPacketBase* cmdPacket)
{
  IdentificationCmdPacket* identificationCmd = deserializeCommandPacket<IdentificationCmdPacket>(cmdPacket);

  if (identificationCmd->m_connectProtocolVersion == NM_PROTOCOL_VER)
  {
    CommsServer::getInstance()->sendTargetStatus(getCurrentConnection());
  }

  IdentificationReplyPacket identityPacket(identificationCmd->m_requestId);
  CommsServer::getInstance()->identify(identityPacket);

  mcommsSendDataPacket(identityPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetControlParamCmd(CmdPacketBase* cmdPacket)
{
  SetControlParameterCmdPacket* controlParamPkt = deserializeCommandPacket<SetControlParameterCmdPacket>(cmdPacket);

  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr && dataMgr->canSetControlParameters())
  {
    InstanceID instanceID = controlParamPkt->m_instanceId;
    commsNodeID nodeID = controlParamPkt->m_controlParamId;
    MR::NodeOutputDataType type = (MR::NodeOutputDataType)controlParamPkt->m_type;
    bool setResult = dataMgr->setControlParameter(
                       instanceID,
                       nodeID,
                       type,
                       (const void*) &controlParamPkt->m_controlParamValue.m_value);

    if (!setResult)
    {
      NMP_MSG("Error setting control param [Param: %d, Instance: %d]", nodeID, instanceID);
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: SetControlParam command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetRootTransformCmd(CmdPacketBase* cmdPacket)
{
  SetRootTransformCmdPacket* rootTransformPkt = deserializeCommandPacket<SetRootTransformCmdPacket>(cmdPacket);

  DataManagementInterface* dataMgr = m_target->getDataManager();

  if (dataMgr)
  {
    // This slightly round-about way of getting the transform data out of the packet is required so the 
    //  GHS compiler can deal with the different __packed type modifiers.  Please do not optimise it out.
    NMP::PosQuat transformPosQuat;
    transformPosQuat.m_pos = rootTransformPkt->m_transform.m_pos;
    transformPosQuat.m_quat = rootTransformPkt->m_transform.m_quat;

    bool result = dataMgr->setRootTransform(rootTransformPkt->m_instanceId, transformPosQuat);
    if (!result)
    {
      NMP_MSG("MorphemeComms: Error setting root transform [Instance: %d]", rootTransformPkt->m_instanceId);
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: SetRootTransform command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetAnimationSetCmd(CmdPacketBase* cmdPacket)
{
  SetAnimationSetCmdPacket* animSetPkt = deserializeCommandPacket<SetAnimationSetCmdPacket>(cmdPacket);

  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr && dataMgr->canSetNetworkAnimSet())
  {
    InstanceID instanceID = animSetPkt->m_instanceId;
    if (!dataMgr->setActiveAnimationSet(instanceID, animSetPkt->m_animSetIndex))
    {
      NMP_MSG("Error setting animation set [Set: %d, Instance: %d]", animSetPkt->m_animSetIndex, instanceID);
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: SetAnimationSet command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetAnimBrowserAnimationSetCmd(CmdPacketBase *cmdPacket)
{
  SetAssetManagerAnimationSetCmdPacket* animSetPkt = deserializeCommandPacket<SetAssetManagerAnimationSetCmdPacket>(cmdPacket);
  AnimationBrowserInterface* animBrowser = m_target->getAnimationBrowser(); 

  uint32_t index = animSetPkt->m_animSetIndex;
  animBrowser->setAnimationBrowserActiveAnimationSet((MR::AnimSetIndex)index, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSendRequestCmd(CmdPacketBase* cmdPacket)
{
  SendRequestCmdPacket* requestPkt = deserializeCommandPacket<SendRequestCmdPacket>(cmdPacket);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr && dataMgr->canSendStateMachineRequests())
  {
    InstanceID instanceID = requestPkt->m_instanceId;
    commsNodeID nodeID = requestPkt->m_nodeId;
    NetworkMessageID reqId = requestPkt->m_networkRequestID;
    MR::Message message(reqId, MESSAGE_TYPE_REQUEST, true, 0, 0);
    if (!dataMgr->sendMessage(instanceID, nodeID, message))
    {
      NMP_MSG("Error sending request [Request: %d, Node: %d, Instance: %d]", reqId, nodeID, instanceID);
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: SendRequest command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleBroadcastRequestCmd(CmdPacketBase* cmdPacket)
{
  BroadcastRequestCmdPacket* requestPkt = deserializeCommandPacket<BroadcastRequestCmdPacket>(cmdPacket);

  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr && dataMgr->canSendStateMachineRequests())
  {
    InstanceID instanceID = requestPkt->m_instanceId;
    NetworkMessageID reqId = requestPkt->m_networkRequestID;

    // Note that it's valid to send a request that doesn't get handled, so don't check the return
    // value here.
    MR::Message message(reqId, MESSAGE_TYPE_REQUEST, true, 0, 0);
    dataMgr->broadcastMessage(instanceID, message);
  }
  else
  {
    NMP_MSG("MorphemeComms: BroadcastRequest command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleMessageCmd(CmdPacketBase* cmdPacket)
{
  MessageBufferCmdPacket * messagePacket = deserializeCommandPacket<MessageBufferCmdPacket>(cmdPacket); 
  DataManagementInterface * dataMgr = m_target->getDataManager(); 
  AnimationBrowserInterface* animBrowser = m_target->getAnimationBrowser(); 
 
  MR::Message message(
    messagePacket->m_networkMessageID, 
    messagePacket->m_messageTypeID, 
    true, 
    messagePacket->getDataBuffer(), 
    messagePacket->m_dataBufferSize);

  if(messagePacket->m_sendToAssetManager && animBrowser)
  {
    if(messagePacket->m_broadcastOrNodeID == MR::INVALID_NODE_ID)
    {
      animBrowser->broadcastAnimationBrowserMessage(message, getCurrentConnection()); 
    }
    else
    {
      animBrowser->sendAnimationBrowserMessage(messagePacket->m_broadcastOrNodeID, message, getCurrentConnection()); 
    }
  }
  else if(dataMgr && dataMgr->canSendStateMachineRequests())
  {
    if(messagePacket->m_broadcastOrNodeID == MR::INVALID_NODE_ID)
    {
      dataMgr->broadcastMessage(messagePacket->m_instanceId, message); 
    }
    else
    {
      dataMgr->sendMessage(messagePacket->m_instanceId, messagePacket->m_broadcastOrNodeID, message); 
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: BroadcastRequest command not supported.");
  }

 
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetCurrentStateCmd(CmdPacketBase* cmdPacket)
{
  SetCurrentStateCmdPacket* currStatePkt = deserializeCommandPacket<SetCurrentStateCmdPacket>(cmdPacket);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr && dataMgr->canSendStateMachineRequests())
  {
    InstanceID instanceID = currStatePkt->m_instanceId;
    commsNodeID stateMachineID = currStatePkt->m_stateMachineID;
    commsNodeID newRootID = currStatePkt->m_rootStateID;
    if (!dataMgr->setCurrentState(instanceID, stateMachineID, newRootID))
    {
      NMP_MSG("Error setting current state [State: %d, State machine: %d, Instance: %d]",
              newRootID, stateMachineID, instanceID);
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: SetCurrentState command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleExecuteCommandCmd(CmdPacketBase* cmdPacket)
{
  ExecuteCommandCmdPacket* executeCommandPkt = deserializeCommandPacket<ExecuteCommandCmdPacket>(cmdPacket);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr)
  {
    InstanceID instanceID = executeCommandPkt->m_instanceId;
    const char* command = executeCommandPkt->getCommand();
    if (!dataMgr->executeCommand(instanceID, command))
    {
      NMP_MSG("Error executing command : %s", command);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleEnableOutputDataCmd(CmdPacketBase* MR_OUTPUT_DEBUG_ARG(cmdPacket))
{
#if defined (MR_OUTPUT_DEBUGGING)
  EnableOutputDataCmdPacket* enableOutDataCmd = deserializeCommandPacket<EnableOutputDataCmdPacket>(cmdPacket);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr)
  {
    InstanceID instanceID = enableOutDataCmd->m_instanceID;
    commsNodeID nodeID = enableOutDataCmd->m_nodeID;
    uint32_t dataType = enableOutDataCmd->m_outputDataType;
    bool enabled = (enableOutDataCmd->m_enable != 0);
    if (!dataMgr->enableOutputDataBuffering(instanceID, nodeID, (MR::NodeOutputDataType)dataType, enabled))
    {
      NMP_MSG("Error enabling output data [Node: %d, Type: %d, Instance: %d]", nodeID, dataType, instanceID);
    }
  }
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetDebugOutputFlagsCmd(CmdPacketBase* MR_OUTPUT_DEBUG_ARG(cmdPacket))
{
#if defined (MR_OUTPUT_DEBUGGING)
  SetDebugOutputFlagsCmdPacket* setDebugOutputFlagsCmd = deserializeCommandPacket<SetDebugOutputFlagsCmdPacket>(cmdPacket);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr)
  {
    dataMgr->setDebugOutputFlags(setDebugOutputFlagsCmd->m_instanceID, setDebugOutputFlagsCmd->m_debugOutputFlags);
  }
  else
  {
    NMP_MSG("Error setting debug output flags [Instance: %d]", setDebugOutputFlagsCmd->m_instanceID);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetDebugOutputOnNodesCmd(CmdPacketBase* MR_OUTPUT_DEBUG_ARG(cmdPacket))
{
#if defined (MR_OUTPUT_DEBUGGING)
  SetDebugOutputOnNodesCmdPacket* setDebugOutputOnNodesCmd = deserializeCommandPacket<SetDebugOutputOnNodesCmdPacket>(cmdPacket);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr)
  {
    dataMgr->setDebugOutputOnNodes(
      setDebugOutputOnNodesCmd->m_instanceID,
      setDebugOutputOnNodesCmd->m_on,
      setDebugOutputOnNodesCmd->m_allNodes,
      setDebugOutputOnNodesCmd->m_allSemantics,
      setDebugOutputOnNodesCmd->m_numNodeIDs,
      setDebugOutputOnNodesCmd->getNodeIDs(),
      setDebugOutputOnNodesCmd->m_numSemantics,
      setDebugOutputOnNodesCmd->getSemantics());
  }
  else
  {
    NMP_MSG("Error setting debug output semantics [Instance: %d]", setDebugOutputOnNodesCmd->m_instanceID);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDownloadNetworkDefinitionCmd(CmdPacketBase* cmdPacket)
{
  DownloadNetworkDefinitionCmdPacket* downNetDefPacket =
    deserializeCommandPacket<DownloadNetworkDefinitionCmdPacket>(cmdPacket);

  ReplyPacket replyPacket(downNetDefPacket->m_requestId, pk_DownloadNetworkDefinitionCmd);
  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr)
  {
    CommsServer* commsServer = CommsServer::getInstance();
    commsServer->sendDefData(downNetDefPacket->m_guid, getCurrentConnection());
  }
  else
  {
    replyPacket.m_result = ReplyPacket::kResultFailure;
    NMP_MSG("MorphemeComms: DownloadNetworkDefinition command not supported.");
  }

  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDownloadSceneObjectsCmd(CmdPacketBase* cmdPacket)
{
  DownloadSceneObjectsCmdPacket* downSOPacket = deserializeCommandPacket<DownloadSceneObjectsCmdPacket>(cmdPacket);

  SceneObjectManagementInterface* const sceneObjectMgr = m_target->getSceneObjectManager();
  ReplyPacket replyPacket(downSOPacket->m_requestId, pk_DownloadSceneObjectsCmd);

  if (sceneObjectMgr)
  {
    // Send description for all the scene objects currently on the target.
    m_coreModule->describeSceneObjects(getCurrentConnection());
  }
  else
  {
    replyPacket.m_result = ReplyPacket::kResultFailure;
    NMP_MSG("MorphemeComms: DownloadSceneObjects command not supported.");
  }

  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDownloadGlobalDataCmd(CmdPacketBase* cmdPacket)
{
  DownloadGlobalDataCmdPacket* downGlobalsPacket = deserializeCommandPacket<DownloadGlobalDataCmdPacket>(cmdPacket);

  // Send all the global data.
  if (downGlobalsPacket->m_globalDataFlags & DownloadGlobalDataCmdPacket::GBLDATAFLAGS_STRING_CACHE)
  {
    CommsServer::getInstance()->sendStringMap();
  }

  ReplyPacket replyPacket(downGlobalsPacket->m_requestId, pk_DownloadGlobalDataCmd);
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDownloadFrameDataCmd(CmdPacketBase* cmdPacket)
{
  DownloadFrameDataCmdPacket* downFDPacket = deserializeCommandPacket<DownloadFrameDataCmdPacket>(cmdPacket);

  CommsServer::getInstance()->sendFrameData(getCurrentConnection());

  ReplyPacket replyPacket(downFDPacket->m_requestId, pk_DownloadFrameDataCmd);
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleLoadNetworkCmd(CmdPacketBase* cmdPacket)
{
  LoadNetworkCmdPacket* loadNetPacket = deserializeCommandPacket<LoadNetworkCmdPacket>(cmdPacket);
  NetworkDefLifecycleInterface* networkDefLifcycle = m_target->getNetworkDefLifecycle();
  NetworkManagementInterface* networkMgr = m_target->getNetworkManager();
  if (networkDefLifcycle && networkDefLifcycle->canLoadNetworkDefinition() && networkMgr)
  {
    NMP_MSG(
      " LoadingNetwork.  name = '%s' guid = %s\n",
      loadNetPacket->m_networkName,
      guidToString(&loadNetPacket->m_networkGUID));

    CommsServer* commsServer = CommsServer::getInstance();

    bool loadSuccess = true;
    if (!networkMgr->isNetworkDefinitionLoaded(loadNetPacket->m_networkGUID))
    {
      loadSuccess = networkDefLifcycle->loadNetworkDefinition(
                      loadNetPacket->m_networkGUID,
                      loadNetPacket->m_networkName,
                      getCurrentConnection());

      NMP_MSG("%s", loadSuccess ? " Loaded.\n" : "  Could not load.\n");

      if (loadSuccess)
      {
        // Before sending back the reply to the command, send a full description of the network's def data.
        commsServer->sendDefData(loadNetPacket->m_networkGUID, getCurrentConnection());
      }
    }

    commsServer->broadcastTargetStatus();

    NetworkDefLoadedReplyPacket netLoaded(loadNetPacket->m_requestId);
    netLoaded.m_loaded = loadSuccess;
    netLoaded.m_networkGUID = loadNetPacket->m_networkGUID; // Send back the guid of the network def that has been loaded.
    mcommsSendDataPacket(netLoaded, getCurrentConnection());
  }
  else
  {
    NMP_MSG("MorphemeComms: LoadNetworkDefinition command not supported.");
    ReplyPacket replyPacket(loadNetPacket->m_requestId, pk_LoadNetworkCmd);
    mcommsSendDataPacket(replyPacket, getCurrentConnection());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleReferenceNetworkInstanceCmd(CmdPacketBase* cmdPacket)
{
  Connection* connection = getCurrentConnection();
  ReferenceNetworkInstanceCmdPacket* referenceNetPacket =
    deserializeCommandPacket<ReferenceNetworkInstanceCmdPacket>(cmdPacket);

  ReferenceNetworkInstanceReplyPacket replyPacket(referenceNetPacket->m_requestId);
  replyPacket.m_result = ReplyPacket::kResultFailure;

  NetworkManagementInterface* networkMgr = m_target->getNetworkManager();
  if (networkMgr)
  {
    const uint32_t netInstancesCount = networkMgr->getNetworkInstanceCount();

    NMP::TempMemoryAllocator* tempAllocator = getCurrentConnection()->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

    // Check if this is a valid instance, before adding a reference to it in the connection.
    InstanceID* instanceIDs = (InstanceID*)NMPAllocatorMemAlloc(childAllocator, sizeof(InstanceID) * netInstancesCount, 4);
    NMP_ASSERT(instanceIDs);

    networkMgr->getNetworkInstanceIDs(instanceIDs, netInstancesCount);

    for (uint32_t i = 0; i < netInstancesCount; ++i)
    {
      CommsServer* commsServer = CommsServer::getInstance();

      MCOMMS::GUID networkGUID;
      networkMgr->getNetworkInstanceDefinitionGUID(instanceIDs[i], networkGUID);
      commsServer->sendInstanceStaticData(networkGUID, instanceIDs[i], connection);

      if (instanceIDs[i] == referenceNetPacket->m_instanceId &&
          connection->startDebuggingNetwork(referenceNetPacket->m_instanceId))
      {
        ConnectionManagementInterface* connectionMgr = m_target->getConnectionManager();
        if (connectionMgr)
        {
          connectionMgr->startDebuggingNetworkInstance(connection, referenceNetPacket->m_instanceId);
        }

        uint32_t activeAnimSet = 0xFFFFFFFF;
        DataManagementInterface* dataMgr = m_target->getDataManager();
        if (dataMgr)
        {
          activeAnimSet = dataMgr->getActiveAnimationSet(referenceNetPacket->m_instanceId);
        }

        replyPacket.m_result = ReplyPacket::kResultSuccess;
        replyPacket.m_instanceId = referenceNetPacket->m_instanceId;
        replyPacket.m_initialAnimSetIndex = activeAnimSet;
        const char* instanceName = networkMgr->getNetworkInstanceName(referenceNetPacket->m_instanceId);
        replyPacket.m_nameToken = commsServer->getTokenForString(instanceName);
        networkMgr->getNetworkInstanceDefinitionGUID(referenceNetPacket->m_instanceId, replyPacket.m_guid);

        commsServer->sendInstanceStaticData(replyPacket.m_guid, referenceNetPacket->m_instanceId, connection);
        break;
      }
    }

    childAllocator->memFree(instanceIDs);
    tempAllocator->destroyChildAllocator(childAllocator);
  }

  mcommsSendDataPacket(replyPacket, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleUnreferenceNetworkInstanceCmd(CmdPacketBase* cmdPacket)
{
  Connection* connection = getCurrentConnection();
  UnreferenceNetworkInstanceCmdPacket* unreferenceNetPacket =
    deserializeCommandPacket<UnreferenceNetworkInstanceCmdPacket>(cmdPacket);

  ReplyPacket replyPacket(unreferenceNetPacket->m_requestId, pk_UnreferenceNetworkInstanceCmd);
  replyPacket.m_result = ReplyPacket::kResultSuccess;

  NetworkManagementInterface* networkMgr = m_target->getNetworkManager();
  if (networkMgr)
  {
    const uint32_t netInstancesCount = networkMgr->getNetworkInstanceCount();

    NMP::TempMemoryAllocator* tempAllocator = getCurrentConnection()->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

    InstanceID* instanceIDs = (InstanceID*)NMPAllocatorMemAlloc(childAllocator, sizeof(InstanceID) * netInstancesCount, 4);
    NMP_ASSERT(instanceIDs);

    networkMgr->getNetworkInstanceIDs(instanceIDs, netInstancesCount);

    for (uint32_t i = 0; i < netInstancesCount; ++i)
    {
      if (instanceIDs[i] == unreferenceNetPacket->m_instanceId)
      {
        break;
      }
    }

    childAllocator->memFree(instanceIDs);
    tempAllocator->destroyChildAllocator(childAllocator);

    connection->stopDebuggingNetwork(unreferenceNetPacket->m_instanceId);
    ConnectionManagementInterface* connectionMgr = m_target->getConnectionManager();
    if (connectionMgr)
    {
      connectionMgr->stopDebuggingNetworkInstance(connection, unreferenceNetPacket->m_instanceId);
    }
  }

  mcommsSendDataPacket(replyPacket, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleCreateNetworkInstanceCmd(CmdPacketBase* cmdPacket)
{
  Connection* connection = getCurrentConnection();
  const CreateNetworkInstanceCmdPacket* createNetPacket = deserializeCommandPacket<CreateNetworkInstanceCmdPacket>(cmdPacket);

  NetworkLifecycleInterface* networkLifecycle = m_target->getNetworkLifecycle();
  if (networkLifecycle && networkLifecycle->canCreateNetworkInstance())
  {
    const char* instanceName = createNetPacket->getNetworkInstanceName();
    if (NMP_STRLEN(instanceName) == 0)
    {
      instanceName = "DefaultInstanceName";
    }

    NMP::Vector3 characterStartPosition = vector3fromVec3(createNetPacket->m_characterStartPosition);
    NMP::Quat characterStartRotation = quatfromVec4(createNetPacket->m_characterStartRotation);

    InstanceID id = networkLifecycle->createNetworkInstance(
                      createNetPacket->m_networkGUID,
                      instanceName,
                      createNetPacket->m_initialAnimSetIndex,
                      connection,
                      characterStartPosition,
                      characterStartRotation);

    CommsServer* commsServer = CommsServer::getInstance();
    commsServer->sendInstanceStaticData(createNetPacket->m_networkGUID, id, connection);

    NMP_MSG("MorphemeComms: CreateNetworkInstance guid = %s.", guidToString(&createNetPacket->m_networkGUID));
    NMP_MSG("  instance name = \"%s\".", instanceName);
    NMP_MSG("  instance id = 0x%x.", id);

    // Send back the instance id and the guid of the instance that has just been created.
    NetworkManagementInterface* networkManager = m_target->getNetworkManager();
    StringToken nameToken = MCOMMS_INVALID_STRINGTOKEN;
    if (networkManager)
    {
      const char* networkInstanceName = networkManager->getNetworkInstanceName(id);
      nameToken = CommsServer::getInstance()->getTokenForString(networkInstanceName);
    }

    CommsServer::getInstance()->broadcastTargetStatus();

    connection->startDebuggingNetwork(id);
    NetworkCreatedReplyPacket netCreatedPkt(createNetPacket->m_requestId, id, createNetPacket->m_networkGUID, nameToken, createNetPacket->m_initialAnimSetIndex);

    mcommsSendDataPacket(netCreatedPkt, connection);
  }
  else
  {
    NMP_MSG("MorphemeComms: CreateNetworkInstance command not supported.");
    ReplyPacket replyPacket(createNetPacket->m_requestId, pk_CreateNetworkInstanceCmd);
    mcommsSendDataPacket(replyPacket, connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDestroyNetworkDefinitionCmd(CmdPacketBase* cmdPacket)
{
  DestroyNetworkDefinitionCmdPacket* destrNetDefPacket =
    deserializeCommandPacket<DestroyNetworkDefinitionCmdPacket>(cmdPacket);

  NetworkDefLifecycleInterface* networkDefLifecycle = m_target->getNetworkDefLifecycle();
  if (networkDefLifecycle && networkDefLifecycle->canLoadNetworkDefinition())
  {
    networkDefLifecycle->destroyNetworkDefinition(destrNetDefPacket->m_networkGUID);
    NMP_MSG("MorphemeComms: DestroyNetworkDefinition guid %s.", guidToString(&destrNetDefPacket->m_networkGUID));

    CommsServer::getInstance()->broadcastTargetStatus();

    NetworkDefDestroyedReplyPacket netDefDestroyedPkt(destrNetDefPacket->m_requestId, destrNetDefPacket->m_networkGUID);
    mcommsSendDataPacket(netDefDestroyedPkt, getCurrentConnection());
  }
  else
  {
    ReplyPacket replyPacket(destrNetDefPacket->m_requestId, pk_DestroyNetworkDefinitionCmd);
    replyPacket.m_result = ReplyPacket::kResultFailure;
    mcommsSendDataPacket(replyPacket, getCurrentConnection());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDestroyNetworkInstanceCmd(CmdPacketBase* cmdPacket)
{
  DestroyNetworkInstanceCmdPacket* destrNetInstPacket =
    deserializeCommandPacket<DestroyNetworkInstanceCmdPacket>(cmdPacket);

  NetworkLifecycleInterface* networkLifecycle = m_target->getNetworkLifecycle();
  if (networkLifecycle && networkLifecycle->canCreateNetworkInstance())
  {
    InstanceID id = destrNetInstPacket->m_instanceId;
    networkLifecycle->destroyNetworkInstance(id);

    NMP_MSG("MorphemeComms: DestroyNetworkInstance id 0x%d.", id);

    CommsServer::getInstance()->broadcastTargetStatus();

    getCurrentConnection()->stopDebuggingNetwork(id);
    // TODO this should become a simple ReplyPacket...
    NetworkDestroyedReplyPacket netDestroyedPkt(destrNetInstPacket->m_requestId ,id);
    mcommsSendDataPacket(netDestroyedPkt, getCurrentConnection());
  }
  else
  {
    ReplyPacket replyPacket(destrNetInstPacket->m_requestId, pk_DestroyNetworkInstanceCmd);
    replyPacket.m_result = ReplyPacket::kResultFailure;
    mcommsSendDataPacket(replyPacket, getCurrentConnection());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleStepCmd(CmdPacketBase* cmdPacket)
{
  StepCmdPacket* stepCmdPacket = deserializeCommandPacket<StepCmdPacket>(cmdPacket);

  // Non master connections get an early reply about the step, so that they can proceed.
  // The master connection is informed only when the step actually happens.
  Connection* connection = getCurrentConnection();
  if (connection->isMaster())
  {
    DataManagementInterface* dataMgr = m_target->getDataManager();
    if (dataMgr)
    {
      dataMgr->setFrameRate(stepCmdPacket->m_frameRate);
    }
  }
  else
  {
    ReplyPacket replyPacket(stepCmdPacket->m_requestId, pk_StepCmd);
    mcommsSendDataPacket(replyPacket, connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleStepModeCmd(CmdPacketBase* cmdPacket)
{
  StepModeCmdPacket* stepModeCmdPacket = deserializeCommandPacket<StepModeCmdPacket>(cmdPacket);

  getCurrentConnection()->enableAsynchronousStepping(stepModeCmdPacket->m_stepMode == StepModeCmdPacket::kAsynchronous);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleStartSessionCmd(CmdPacketBase* cmdPacket)
{
  StartSessionCmdPacket* startSessionPkt = deserializeCommandPacket<StartSessionCmdPacket>(cmdPacket);

  Connection* connection = getCurrentConnection();

  CommsServer::getInstance()->onStartSession(connection);

  // Also, resuscitate a session that might have been paused previously.
  connection->pauseSession(false);

  DataManagementInterface* dataMgr = m_target->getDataManager();
  if (dataMgr && connection->isMaster())
  {
    dataMgr->setFrameRate(startSessionPkt->m_frameRate);
  }

  // Send the reply back.
  ReplyPacket replyPacket(startSessionPkt->m_requestId, pk_StartSessionCmd);
  mcommsSendDataPacket(replyPacket, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleStopSessionCmd(CmdPacketBase* cmdPacket)
{
  StopSessionCmdPacket* stopSessionPkt = deserializeCommandPacket<StopSessionCmdPacket>(cmdPacket);
  CommsServer::getInstance()->onStopSession(getCurrentConnection());

  // Send the reply back.
  ReplyPacket replyPacket(stopSessionPkt->m_requestId, pk_StopSessionCmd);
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handlePauseSessionCmd(CmdPacketBase* cmdPacket)
{
  PauseSessionCmdPacket* pauseSessionPkt = deserializeCommandPacket<PauseSessionCmdPacket>(cmdPacket);

  getCurrentConnection()->pauseSession(pauseSessionPkt->m_pause);

  ReplyPacket replyPacket(pauseSessionPkt->m_requestId, pk_PauseSessionCmd);
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleCreateConstraintCmd(CmdPacketBase* cmdPacket)
{
  CreateConstraintCmdPacket* createConstraint = deserializeCommandPacket<CreateConstraintCmdPacket>(cmdPacket);
  PhysicsManipulationInterface* physicsManipMgr = m_target->getPhysicsManipManager(); 
  ReplyPacket replyPacket(createConstraint->m_requestId, pk_CreateConstraintCmd);
  bool success = false; 
  if(physicsManipMgr && physicsManipMgr->canCreateAndEditConstraints())
  {
    success = physicsManipMgr->createConstraint(
      createConstraint->m_constraintGUID, 
      createConstraint->m_physicsEngineObjectID,
      vector3fromVec3(createConstraint->m_localSpaceConstraintPosition),
      vector3fromVec3(createConstraint->m_worldSpaceConstraintPosition),
      (createConstraint->m_constraintFlags & CreateConstraintCmdPacket::CONSTRAIN_ORIENTATION) != 0,
      (createConstraint->m_constraintFlags & CreateConstraintCmdPacket::CONSTRAINT_AT_COM) != 0, 
      vector3fromVec3(createConstraint->m_sourceRayWSPositionHint), 
      vector3fromVec3(createConstraint->m_sourceRayWSDirectionHint)
      ); 
  }
  replyPacket.m_result = success ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure;
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleMoveConstraintCmd(CmdPacketBase* cmdPacket)
{
  MoveConstraintCmdPacket* moveConstraint = deserializeCommandPacket<MoveConstraintCmdPacket>(cmdPacket);
  PhysicsManipulationInterface* physicsManipMgr = m_target->getPhysicsManipManager();
  ReplyPacket replyPacket(moveConstraint->m_requestId, pk_MoveConstraintCmd);
  bool success = false; 
  if(physicsManipMgr && physicsManipMgr->canCreateAndEditConstraints())
  {
    success = physicsManipMgr->moveConstraint(moveConstraint->m_constraintGUID,
      vector3fromVec3(moveConstraint->m_worldSpaceConstraintPosition)
      );
  }
  replyPacket.m_result = success ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure;
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleRemoveConstraintCmd(CmdPacketBase* cmdPacket)
{
  RemoveConstraintCmdPacket* removeConstraint = deserializeCommandPacket<RemoveConstraintCmdPacket>(cmdPacket);
  PhysicsManipulationInterface* physicsManipMgr = m_target->getPhysicsManipManager();
  ReplyPacket replyPacket(removeConstraint->m_requestId, pk_RemoveConstraintCmd);
  bool success = false; 
  if(physicsManipMgr && physicsManipMgr->canCreateAndEditConstraints())
  {
    success = physicsManipMgr->removeConstraint(removeConstraint->m_constraintGUID);
  }
  replyPacket.m_result = success ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure;
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleApplyForceCmd(CmdPacketBase* cmdPacket)
{
  ApplyForceCmdPacket* applyForce = deserializeCommandPacket<ApplyForceCmdPacket>(cmdPacket);
  PhysicsManipulationInterface* physicsManipMgr = m_target->getPhysicsManipManager(); 
  ReplyPacket replyPacket(applyForce->m_requestId, pk_ApplyForceCmd);
  bool success = false; 
  if(physicsManipMgr && physicsManipMgr->canApplyForces())
  {
    PhysicsManipulationInterface::ForceMode forceMode = PhysicsManipulationInterface::kFORCE;
    if(applyForce->m_flags & ApplyForceCmdPacket::MODE_FORCE)
    {
      forceMode = PhysicsManipulationInterface::kFORCE; 
    }
    else if(applyForce->m_flags & ApplyForceCmdPacket::MODE_IMPULSE)
    {
      forceMode = PhysicsManipulationInterface::kIMPULSE; 
    }
    else if(applyForce->m_flags & ApplyForceCmdPacket::MODE_VELOCITY_CHANGE)
    {
      forceMode = PhysicsManipulationInterface::kVELOCITY_CHANGE; 
    }
    bool applyAtCom = (applyForce->m_flags & ApplyForceCmdPacket::APPLY_AT_COM) != 0; 

    success = physicsManipMgr->applyForce(
      applyForce->m_physicsEngineObjectID,
      forceMode,
      vector3fromVec3(applyForce->m_force),
      applyAtCom,
      vector3fromVec3(applyForce->m_localSpacePosition), 
      vector3fromVec3(applyForce->m_worldSpacePosition)
      ); 
  }
  replyPacket.m_result = success ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure;
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleBeginSceneObjectCmd(CmdPacketBase* cmdPacket)
{
  BeginSceneObjectCmdPacket* beginSceneObject = deserializeCommandPacket<BeginSceneObjectCmdPacket>(cmdPacket);

  SceneObjectManagementInterface* sceneObjectMgr = m_target->getSceneObjectManager();
  if (sceneObjectMgr && sceneObjectMgr->canCreateSceneObjects())
  {
    // There shouldn't be any pending scene object creation.
    NMP_ASSERT(m_objectsToSetup.find(getCurrentConnection()) == m_objectsToSetup.end());

    ObjectSetupHandler* objectToCreate = new ObjectSetupHandler(ObjectSetupHandler::kSceneObject, beginSceneObject->m_numAttributes);
    m_objectsToSetup.insert(
      std::pair<Connection*, ObjectSetupHandler*>(getCurrentConnection(), objectToCreate));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleEndSceneObjectCmd(CmdPacketBase* NMP_UNUSED(cmdPacket))
{
  SceneObjectManagementInterface* sceneObjectMgr = m_target->getSceneObjectManager();
  if (sceneObjectMgr && sceneObjectMgr->canCreateSceneObjects())
  {
    // Finally create the scene object or revert in case of error.
    ObjectSetupHandlers::iterator iter =  m_objectsToSetup.find(getCurrentConnection());
    NMP_ASSERT(iter != m_objectsToSetup.end());

    ObjectSetupHandler* handler = iter->second;
    NMP_ASSERT(handler->m_totalNumAttributes == handler->m_numAttributes); // Something wrong, missing attributes?

    SceneObject* characterControllerObject = NULL;
    SceneObject* const sceneObject = sceneObjectMgr->createSceneObject(
                                       handler->m_numAttributes,
                                       handler->m_attributes,
                                       getCurrentConnection(),
                                       characterControllerObject);

    // Once the object is created, the handler is not needed anymore, so delete it and erase it from the map.
    delete handler;
    m_objectsToSetup.erase(iter);

    if (sceneObject)
    {
      // Send back the new object description. Send it manually to the current connection because the broadcast
      // won't do it if the session is not running yet.
      if (!getCurrentConnection()->isSessionRunning())
      {
        m_coreModule->describeSceneObject(sceneObject, getCurrentConnection());
        if (characterControllerObject)
        {
          m_coreModule->describeSceneObject(characterControllerObject, getCurrentConnection());
        }
      }

      m_coreModule->synchronizeSceneObjects();
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: EndSceneObject command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSceneObjectAttributeCmd(CmdPacketBase* cmdPacket)
{
  // Packet's attribute data must be copied, for later treatment.
  AttributeCmdPacket* attrPacket = deserializeCommandPacket<AttributeCmdPacket>(cmdPacket);

  SceneObjectManagementInterface* sceneObjectMgr = m_target->getSceneObjectManager();
  if (sceneObjectMgr && sceneObjectMgr->canCreateSceneObjects())
  {
    ObjectSetupHandlers::iterator iter = m_objectsToSetup.find(getCurrentConnection());
    NMP_ASSERT(iter != m_objectsToSetup.end());

    ObjectSetupHandler* handler = iter->second;

    Attribute* attrCopy = Attribute::create(attrPacket->getAttributeDescriptor(), attrPacket->getAttributeData());
    handler->addAttribute(attrCopy);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetAttributeCmd(CmdPacketBase* cmdPacket)
{
  SetAttributeCmdPacket* setObjAttrPacket = deserializeCommandPacket<SetAttributeCmdPacket>(cmdPacket);

  // In legacy code if no object was specified we assumed that the attribute belonged to the environment. This has
  // changed with introducing the SetEnvironmentAttribute command (see handleSetEnvironmentAttributeCmd()).
  NMP_ASSERT_MSG((setObjAttrPacket->m_sceneObjectId != 0xFFFFFFFF), "MorphemeComms: valid object ID expected!");

  SceneObjectManagementInterface* sceneObjectMgr = m_target->getSceneObjectManager();
  if (sceneObjectMgr && sceneObjectMgr->canEditSceneObjects())
  {
    SceneObject* const targetObject = sceneObjectMgr->getSceneObject(setObjAttrPacket->m_sceneObjectId);
    if (targetObject)
    {
      // Create temporary attribute from packet data.
      Attribute* const incomingAttribute = Attribute::createEmpty();
      incomingAttribute->init(*setObjAttrPacket->getAttributeDescriptor(), setObjAttrPacket->getAttributeData());

      // Find attribute on object.
      Attribute* const existingAttribute = targetObject->getAttribute(incomingAttribute->getSemantic());
      if (existingAttribute)
      {
        // Transfer incoming attribute to the existing one
        if (!existingAttribute->copyDataFrom(incomingAttribute))
        {
          NMP_MSG(
            "MorphemeComms: Error: Scene object attribute mismatch (id=%u, semantic=%u)!",
            targetObject->getSceneObjectID(), incomingAttribute->getSemantic());
        }
        // If this scene object has a physics engine id notify the physicsManipManager of the change
        if(targetObject->getAttribute(MCOMMS::Attribute::SEMANTIC_PHYSICS_OBJECT_ID))
        {
          MCOMMS::Attribute* physicsIdAttribute = targetObject->getAttribute(MCOMMS::Attribute::SEMANTIC_PHYSICS_OBJECT_ID);
          if(physicsIdAttribute)
          {
            uint32_t physicsId = *(uint32_t*)physicsIdAttribute->getData();
            PhysicsManipulationInterface* physicsManipMgr = m_target->getPhysicsManipManager(); 
            if(physicsManipMgr && physicsManipMgr->canSetAttributes())
            {
              physicsManipMgr->setPhysicsObjectAttribute(physicsId, incomingAttribute);
            }
          }
        }
        else
        {
          NMP_MSG(
            "MorphemeComms: Error: Scene object attribute not found (id=%u, semantic=%u)!",
            targetObject->getSceneObjectID(), incomingAttribute->getSemantic());
        }
      }

      // Destroy temporary attribute.
      Attribute::destroy(incomingAttribute);
    }
    else
    {
      NMP_MSG("MorphemeComms: Error: Scene object not found (id=%u)!", setObjAttrPacket->m_sceneObjectId);
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: SetAttribute command not supported on scene objects.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleSetEnvironmentAttributeCmd(CmdPacketBase* cmdPacket)
{
  SetEnvironmentAttributeCmdPacket* setEnvAttrPacket =
    deserializeCommandPacket<SetEnvironmentAttributeCmdPacket>(cmdPacket);

  EnvironmentManagementInterface* environmentMgr = m_target->getEnvironmentManager();
  if (environmentMgr && environmentMgr->canEditEnvironmentAttributes())
  {
    Attribute* const attribute = Attribute::createEmpty();
    attribute->init(*setEnvAttrPacket->getAttributeDescriptor(), setEnvAttrPacket->getAttributeData());

    const bool succeeded = environmentMgr->setEnvironmentAttribute(attribute);
    if (!succeeded)
    {
      NMP_MSG("MorphemeComms: Error: SetEnvironmentAttribute failed!");
    }

    Attribute::destroy(attribute);
  }
  else
  {
    NMP_MSG("MorphemeComms: SetEnvironmentAttribute command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleDestroySceneObjectCmd(CmdPacketBase* cmdPacket)
{
  DestroySceneObjectCmdPacket* destroyObjectPacket = deserializeCommandPacket<DestroySceneObjectCmdPacket>(cmdPacket);
  SceneObjectManagementInterface* sceneObjectMgr = m_target->getSceneObjectManager();
  if (sceneObjectMgr && sceneObjectMgr->canCreateSceneObjects())
  {
    static const SceneObjectID invalidSceneObjectID = 0xFFFFFFFF;
    SceneObjectID destroyedCharacterControllerID = invalidSceneObjectID;
    bool res =
      sceneObjectMgr->destroySceneObject(
        destroyObjectPacket->m_sceneObjectId,
        destroyedCharacterControllerID);

    if (res)
    {
      // Send a communication about the fact the object has been destroyed.
      SceneObjectID sceneObjectID = destroyObjectPacket->m_sceneObjectId;

      if (!getCurrentConnection()->isSessionRunning())
      {
        m_coreModule->sendSceneObjectDestruction(sceneObjectID, getCurrentConnection());
        if (destroyedCharacterControllerID != invalidSceneObjectID)
        {
          m_coreModule->sendSceneObjectDestruction(destroyedCharacterControllerID, getCurrentConnection());
        }
      }

      m_coreModule->synchronizeSceneObjects();
    }
  }
  else
  {
    NMP_MSG("MorphemeComms: DestroySceneObject command not supported.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoreCommandsHandler::handleClearCachedDataCmd(CmdPacketBase* cmdPacket)
{
  ClearCachedDataCmdPacket* clearCachedDataPacket = deserializeCommandPacket<ClearCachedDataCmdPacket>(cmdPacket);

  NetworkDefLifecycleInterface* networkDefLifecycle = m_target->getNetworkDefLifecycle();
  ReplyPacket replyPacket(clearCachedDataPacket->m_requestId, pk_ClearCachedDataCmd);
  if (networkDefLifecycle)
  {
    bool res = networkDefLifecycle->clearCachedData();
    replyPacket.m_result = res ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure;
  }
  mcommsSendDataPacket(replyPacket, getCurrentConnection());
}

} // namespace MCOMMS
//----------------------------------------------------------------------------------------------------------------------
