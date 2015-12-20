// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "NMPlatform/NMPlatform.h"

#include "comms/MCOMMS.h"
#include "comms/commsServer.h"
#include "comms/packet.h"
#include "comms/corePackets.h"
#include "comms/debugPackets.h"
#include "comms/connection.h"
#include "comms/commsServerModule.h"
#include "comms/runtimeTargetInterface.h"
#include "comms/coreCommsServerModule.h"

#include "NMPlatform/NMSocketWrapper.h"
#include "NMPlatform/NMFastHeapAllocator.h"

#include "morpheme/mrTask.h"

// Optionally broadcast the transmission of runtime debug info through CoreDebugInterfaceComms rather than sending to
// a single cached connection.
#define BROADCAST_SCRATCH_PAD_DATAx

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
CommsServer* CommsServer::sm_instance = 0;

//----------------------------------------------------------------------------------------------------------------------
CommsServer::CommsServer(RuntimeTargetInterface* target, int mainPortNumber) :
  m_inited(false),
  m_networkInited(false),
  m_stringTokenIndex(0),
  m_connectionManager(0),
  m_target(target),
  m_mainPortNumber(mainPortNumber),
  m_numModules(0),
  m_lastNetworkError(NETWORK_NO_ERROR)
{
  NMP_ASSERT(sm_instance == 0);
  sm_instance = this;

  m_stringMap = new StringMap(32);

  m_stringCacheMMgr = new ChunkMemory(0, NM_STRINGCACHE_CHUNK);

  m_frameTicker = 0;

  for (uint8_t i = 0; i < MAX_NUM_MODULES; ++i)
  {
    m_modules[i] = 0;
  }

  m_coreModule = new CoreCommsServerModule();
  registerModule(m_coreModule);
}

//----------------------------------------------------------------------------------------------------------------------
CommsServer::~CommsServer()
{
  NMP_ASSERT(sm_instance == this);
  sm_instance = 0;

  delete m_stringCacheMMgr;
  m_stringCacheMMgr = 0;

  delete m_stringMap;

  delete m_connectionManager;
  m_connectionManager = 0;

  unregisterModule(m_coreModule);
  delete m_coreModule;
  m_coreModule = 0;

  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i] = 0;
  }
  m_numModules = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::registerModule(CommsServerModule* module)
{
  NMP_ASSERT(m_numModules < MAX_NUM_MODULES);
  m_modules[m_numModules] = module;
  ++m_numModules;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::unregisterModule(CommsServerModule* module)
{
  uint8_t foundIndex = (uint8_t) - 1;
  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    if (module == m_modules[i])
    {
      foundIndex = i;
      break;
    }
  }

  if (foundIndex != (uint8_t) - 1)
  {
    // Found. Remove it.
    --m_numModules;
  }
}

//----------------------------------------------------------------------------------------------------------------------
CommsServer* CommsServer::getInstance()
{
  return sm_instance;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::init(bool initNetworking)
{
  NMP_ASSERT(!m_inited);

  if (initNetworking)
  {
    if (!NMP::NetworkInitialize())
    {
      return false;
    }

    m_networkInited = true;
  }

  m_inited = true;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::startServer(bool initNetworking)
{
  m_lastNetworkError = NETWORK_NO_ERROR;

  if (!init(initNetworking))
  {
    NMP_MSG("CommsServer: Failed to initialize");
    return false;
  }

  NMP_ASSERT(m_inited); // Should be already initialized.

  if (!initConnectionManager())
  {
    NMP_MSG("CommsServer: Failed to initialize the connection manager");
    term();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::initConnectionManager()
{
  NMP_ASSERT(m_connectionManager == 0);

  m_connectionManager = new ConnectionManager();
  return (m_connectionManager->init(m_mainPortNumber) == ConnectionManager::kNoError);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::termConnectionManager()
{
  if (m_connectionManager)
  {
    // Notify the target of each connection that will be closed.
    const uint32_t connectionCount = m_connectionManager->getNumConnections();
    for (uint32_t i = 0; i < connectionCount; ++i)
    {
      m_target->broadcastOnConnectionClosed(m_connectionManager->getConnection(i));
    }

    m_connectionManager->term();
    delete m_connectionManager;
    m_connectionManager = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::stopServer()
{
  termConnectionManager();
  term();

  m_lastNetworkError = NETWORK_NO_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t CommsServer::getNumConnections() const
{
  NMP_ASSERT(m_connectionManager);
  return m_connectionManager->getNumConnections();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t CommsServer::getNumRunningConnections() const
{
  uint32_t result = 0;
  for (uint32_t i = 0; i < getNumConnections(); ++i)
  {
    if (getConnection(i)->isSessionRunning())
    {
      ++result;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Connection* CommsServer::getConnection(uint32_t connIndex) const
{
  NMP_ASSERT(m_connectionManager);
  return m_connectionManager->getConnection(connIndex);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::handleCmdPacket(CmdPacketBase* cmdPacket, Connection* connection, bool& gotStep)
{
  logPacket(cmdPacket);

  uint16_t pktId = cmdPacket->hdr.m_id;
  NMP::netEndianSwap(pktId);

  // Use the step command to retrieve the delta time only.
  if (pktId == MCOMMS::pk_StepCmd)
  {
    // Unlock the comms update if in synchronous mode.
    if (connection->isMaster())
    {
      gotStep = true;

      StepCmdPacket stepCmdPacket = *static_cast<StepCmdPacket*>(cmdPacket);
      stepCmdPacket.deserialize();

      RuntimeTargetInterface* target = getRuntimeTarget();
      PlaybackManagementInterface* playbackMgr = target->getPlaybackManager();
      DataManagementInterface* dataMgr = target->getDataManager();
      if (dataMgr && playbackMgr && playbackMgr->canChangeDeltaTime())
      {
        float dt = stepCmdPacket.m_deltaTime;
        dataMgr->setStepInterval(dt);
      }
      scheduleStep(true);
      m_lastMasterStepCmd = stepCmdPacket;
    }
  }

  // Look up for a proper handler for this command.
  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    if (m_modules[i]->handleCommand(cmdPacket, connection))
      return true;
  }

  // Couldn't find a proper handler for this command.
#ifdef MCOMMS_VERBOSE
  NMP_MSG("MorphemeComms: unknown command %d\n", pktId);
#endif
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::update()
{
  bool gotStep;
  bool keepLooping = false;

  do
  {
    gotStep = false;
    bool res = m_connectionManager->update(gotStep);
    if (gotStep)
    {
      break;
    }

    keepLooping = (getNumConnections() > 0);
    if (!res)
    {
      NMP_MSG("MorphemeComms: an error occurred while updating the CommsServer.\n");
    }
  } while (keepLooping);

  return gotStep;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::shouldSendFrameDataToConnection(const Connection* connection) const
{
  // Frame data should be sent to a connection if the session is running and:
  // - the connection is not in pause AND the asynchronous mode is set
  // OR
  // - a step was scheduled (either a single step, or a continuos playback)
  //    AND
  //      - the connection is either the master one OR the connection is not in pause.
  // Notes: the first condition covers the asynchronous stepping mode. It's the easiest case, as basically the only
  // for not sending the frame data is that the connection is paused.
  // The second condition catches the cases where a master step was received. In order
  // to send the frame data, the connection must be master, or, if it's not, must be not in pause.
  // This second part is very important to make the single stepping system work when the session is paused.
  if (
      connection->isSessionRunning() &&
      (
       (connection->isAsynchronousSteppingEnabled() && !connection->isSessionPaused()) ||
       (isStepScheduled() && (connection->isMaster() || !connection->isSessionPaused()))
      )
     )
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::sendDefData(const GUID& networkGUID, Connection* connection)
{
  // Request all the modules to send the def data.
  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i]->sendDefData(networkGUID, connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::sendInstanceStaticData(const GUID& networkGUID, InstanceID id, Connection* connection)
{
  // Request all the modules to send the def data.
  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i]->sendInstanceStaticData(networkGUID, id, connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::beginFrame(float deltaTime)
{
  if (isStepScheduled())
  {
    for (uint8_t i = 0; i < m_numModules; ++i)
    {
      m_modules[i]->preFrameUpdate();
    }

    m_frameTicker++;

    for (uint32_t i = 0; i < m_connectionManager->getNumConnections(); ++i)
    {
      Connection* connection = m_connectionManager->getConnection(i);
      if (shouldSendFrameDataToConnection(connection))
      {
        beginUpdateFrameData(connection, deltaTime);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::endFrame(float deltaTime)
{
  m_coreModule->synchronizeSceneObjects();

  if (isStepScheduled())
  {
    for (uint32_t i = 0; i < m_connectionManager->getNumConnections(); ++i)
    {
      Connection* connection = m_connectionManager->getConnection(i);
      if (shouldSendFrameDataToConnection(connection))
      {
        endUpdateFrameData(connection, deltaTime);
      }

      // Also mark the execution of a step command on the master connection.
      if (connection->isMaster() && isStepScheduled())
      {
        ReplyPacket replyPacket(m_lastMasterStepCmd.m_requestId, pk_StepCmd);
        mcommsSendDataPacket(replyPacket, connection);
      }
    }

    for (uint8_t i = 0; i < m_numModules; ++i)
    {
      m_modules[i]->postFrameUpdate();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::beginUpdateFrameData(Connection* connection, float deltaTime)
{
  BeginFramePacket bfp(deltaTime);
  bfp.m_frameId = m_frameTicker;
  mcommsBufferDataPacket(bfp, connection);

  connection->flushDataBuffer();
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::endUpdateFrameData(Connection* connection, float)
{
  NetworkManagementInterface* networkMgr = m_target->getNetworkManager();
  NMP_ASSERT(networkMgr);

  const uint32_t numInstances = networkMgr->getNetworkInstanceCount();
  if (numInstances > 0)
  {
    BeginFrameSegmentPacket beginInstancesSegment(MCOMMS::FRAMESEGTYPE_NETWORKINSTANCES);
    mcommsBufferDataPacket(beginInstancesSegment, connection);

    NMP::TempMemoryAllocator* tempAllocator = connection->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();
    connection->setTempAllocator(childAllocator);

    InstanceID* instanceIDs = (InstanceID*)NMPAllocatorMemAlloc(childAllocator, sizeof(InstanceID) * numInstances, NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(instanceIDs);

    //// Send a list of active network instances.
    ActiveInstancesPacket* activeInstances =
      ActiveInstancesPacket::create(connection->getDataBuffer(), numInstances);

    const uint32_t numInstancesAvailable = networkMgr->getNetworkInstanceIDs(instanceIDs, numInstances);
    NMP_ASSERT(numInstancesAvailable == numInstances);

    InstanceID* instancesData = activeInstances->getInstances();
    for (uint32_t i = 0; i < numInstancesAvailable; ++i)
    {
      instancesData[i] = instanceIDs[i];
    }

    mcommsSerializeDataPacket(*activeInstances);

    // For each instance, request each module to send its specific instance data.
    for (uint32_t instIndex = 0; instIndex < numInstancesAvailable; ++instIndex)
    {
      InstanceID instanceID = instanceIDs[instIndex];
      if (connection->isDebuggingNetwork(instanceID))
      {
        BeginInstanceDataPacket beginInstanceData(instanceID);
        mcommsBufferDataPacket(beginInstanceData, connection);

        for (uint8_t i = 0; i < m_numModules; ++i)
        {
          m_modules[i]->sendInstanceFrameData(instanceID, connection);
        }

        EndInstanceDataPacket endInstanceData(instanceID);
        mcommsBufferDataPacket(endInstanceData, connection);
      }
    }

    childAllocator->memFree(instanceIDs);
    tempAllocator->destroyChildAllocator(childAllocator);
    connection->setTempAllocator(tempAllocator);


    EndFrameSegmentPacket endInstancesSegment(MCOMMS::FRAMESEGTYPE_NETWORKINSTANCES);
    mcommsBufferDataPacket(endInstancesSegment, connection);
  }

  // Request all the modules to send the other segments (eg: scene objects, debug draw, etc...)
  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i]->sendFrameData(connection);
  }

  EndFramePacket efp;

  mcommsBufferDataPacket(efp, connection);
  connection->flushDataBuffer();
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::sendFrameData(Connection* connection)
{
  beginUpdateFrameData(connection, 0);
  endUpdateFrameData(connection, 0);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::onStartSession(Connection* connection)
{
  connection->startSession();

  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i]->onStartSession(connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::onStopSession(Connection* connection)
{
  broadcastTargetStatus();

  connection->stopSession();

  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i]->onStopSession(connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::term()
{
  if (m_inited)
  {
    if (m_networkInited)
    {
      NMP::NetworkShutdown();

      m_networkInited = false;
    }

    m_inited = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::broadcastPacket(PacketBase& pkt)
{
  // Send the packet through the connections. Each connection has its own buffer for sending.
  return m_connectionManager->broadcastPacket(pkt);
}

//----------------------------------------------------------------------------------------------------------------------
StringToken CommsServer::getTokenForString(const char* str)
{
  if (str == NULL)
  {
    return MCOMMS_INVALID_STRINGTOKEN;
  }

  unsigned int stringLen = 0;
  int stringHash = utils::hashString(str, stringLen);
  StringHashed nHashed;

  // go see if we already have this key
  bool found = m_stringMap->find(stringHash, &nHashed);

  // not in the hash map already?
  if (!found)
  {
    // the string tokens are u16 values
    NMP_ASSERT(m_stringTokenIndex < 0xFFFF);

    nHashed.m_stringLength = stringLen;
    nHashed.m_token   = (StringToken)m_stringTokenIndex;

    // copy the string locally so we have a permanent reference to it
    char* copiedString = (char*)m_stringCacheMMgr->alloc(nHashed.m_stringLength + 1);

    NMP_STRNCPY_S(copiedString, nHashed.m_stringLength + 1, str);
    copiedString[nHashed.m_stringLength] = '\0';

    nHashed.m_string  = copiedString;

    // log the new string in the hashmap
    m_stringMap->insert(stringHash, nHashed);

    // increment our 'unique ID' counter
    ++m_stringTokenIndex;

    // if we are connected, send the string straight away;
    // on a new connection, any strings cached before the connection was made
    // are sent automatically (eg. strings encountered during deserialization)

    // send the string data the first time it's encountered
    AddStringToCachePacket astc(nHashed.m_string, nHashed.m_token);
    mcommsBroadcastPacket(astc);

    return nHashed.m_token;
  }

  // otherwise just return the already-inserted value
  return (StringToken)nHashed.m_token;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::sendStringMap()
{
  if (m_stringMap->getNumUsedSlots() == 0)
    return;
  StringMap::const_value_walker stringMapValues = m_stringMap->constWalker();
  while (stringMapValues.next())
  {
    StringHashed nHashed = stringMapValues();
    AddStringToCachePacket astc(nHashed.m_string, nHashed.m_token);
    mcommsBroadcastPacket(astc);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::sendTargetStatus(Connection* connection)
{
  // Experimental: send the target status when receiving a ping.
  RuntimeTargetInterface* target = getRuntimeTarget();
  NetworkManagementInterface* networkMgr = target->getNetworkManager();
  size_t numNetInstances = 0;
  size_t numNetDefinitions = 0;

  if (networkMgr)
  {
    numNetInstances = networkMgr->getNetworkInstanceCount();
    numNetDefinitions = networkMgr->getNetworkDefCount();
  }

  MCOMMS::TargetStatusCtrlPacket* targetStatusPkt = TargetStatusCtrlPacket::create(
    connection->getDataBuffer(), (uint32_t)numNetInstances, (uint32_t)numNetDefinitions);

  if (networkMgr)
  {
    NMP::TempMemoryAllocator* tempAllocator = connection->getTempAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

    InstanceID* instanceIDs = (InstanceID*)NMPAllocatorMemAlloc(childAllocator, sizeof(InstanceID) * numNetInstances, 4);
    NMP_ASSERT(instanceIDs);

    GUID* guids = (GUID*)NMPAllocatorMemAlloc(childAllocator, sizeof(GUID) * numNetDefinitions, 4);
    NMP_ASSERT(guids);

    networkMgr->getNetworkInstanceIDs(instanceIDs, (uint32_t)numNetInstances);
    networkMgr->getNetworkDefGUIDs(guids, (uint32_t)numNetDefinitions);

    TargetStatusCtrlPacket::ActiveInstance* instancesData = targetStatusPkt->getInstances();
    for (uint32_t i = 0; i < numNetInstances; ++i)
    {
      instancesData[i].m_id = instanceIDs[i];
      networkMgr->getNetworkInstanceDefinitionGUID(instanceIDs[i], instancesData[i].m_guid);
      NMP_STRNCPY_S(
        instancesData[i].m_name,
        TargetStatusCtrlPacket::ActiveInstance::kMaxInstanceNameLen,
        networkMgr->getNetworkInstanceName(instanceIDs[i]));
    }

    memcpy(targetStatusPkt->getGUIDs(), guids, sizeof(GUID) * numNetDefinitions);

    childAllocator->memFree(instanceIDs);
    tempAllocator->destroyChildAllocator(childAllocator);
  }

  mcommsSerializeAndSendDataPacket(*targetStatusPkt, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::broadcastTargetStatus()
{
  uint32_t numConnections = getNumConnections();
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    sendTargetStatus(getConnection(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::identify(IdentificationReplyPacket& identityPacket) const
{
  RuntimeTargetInterface* target = getRuntimeTargetInterface();

  // Start with no support, then query the RTT Interface for specific behaviour.
  identityPacket.m_protocolVersion = NM_PROTOCOL_VER;
  identityPacket.m_animBrowserSupportFlags = IdentificationReplyPacket::ANIMBROWSER_NO_SUPPORT;
  identityPacket.m_networkManagementSupportFlags = IdentificationReplyPacket::NETWORKMGMT_NO_SUPPORT;
  identityPacket.m_playbackControlSupportFlags = IdentificationReplyPacket::PLAYBACKCTRL_NO_SUPPORT;
  identityPacket.m_sceneObjectsSupportFlags = IdentificationReplyPacket::SCENEOBJECTS_NO_SUPPORT;

  identityPacket.m_maxInstances = 1024;
  identityPacket.m_maxNetworkDefs = 32;

  identityPacket.m_rttID[0] = 'M';
  identityPacket.m_rttID[1] = 'R';
  identityPacket.m_rttID[2] = 'T';
  identityPacket.m_rttID[3] = 'T';

  PlaybackManagementInterface* playbackMgr = target->getPlaybackManager();
  if (playbackMgr)
  {
    if (playbackMgr->canChangeDeltaTime())
    {
      identityPacket.m_playbackControlSupportFlags.setFlag(IdentificationReplyPacket::PLAYBACKCTRL_VARIABLE_TIME_STEP);
    }
    if (playbackMgr->canPause())
    {
      identityPacket.m_playbackControlSupportFlags.setFlag(IdentificationReplyPacket::PLAYBACKCTRL_PAUSE);
    }
    if (playbackMgr->canStepInSynch())
    {
      identityPacket.m_playbackControlSupportFlags.setFlag(IdentificationReplyPacket::PLAYBACKCTRL_SYNCH_STEPPING);
    }
  }

  NetworkDefLifecycleInterface* const networkDefLifecycle = target->getNetworkDefLifecycle();
  if (networkDefLifecycle)
  {
    if (networkDefLifecycle->canLoadNetworkDefinition())
    {
      identityPacket.m_networkManagementSupportFlags.setFlag(IdentificationReplyPacket::NETWORKMGMT_LOAD_UNLOAD_DEFINITION);
    }
  }

  NetworkLifecycleInterface* const networkLifecycle = target->getNetworkLifecycle();
  if (networkLifecycle)
  {
    if (networkLifecycle->canCreateNetworkInstance())
    {
      identityPacket.m_networkManagementSupportFlags.setFlag(IdentificationReplyPacket::NETWORKMGMT_CREATE_DESTROY_INSTANCE);
    }
  }

  DataManagementInterface* const dataMgr = target->getDataManager();
  if (dataMgr)
  {
    if (dataMgr->canSetControlParameters())
    {
      identityPacket.m_networkManagementSupportFlags.setFlag(IdentificationReplyPacket::NETWORKMGMT_CONTROL_PARAMETERS);
    }
    if (dataMgr->canSendStateMachineRequests())
    {
      identityPacket.m_networkManagementSupportFlags.setFlag(IdentificationReplyPacket::NETWORKMGMT_STATE_MACHINE_REQUESTS);
    }
    if (dataMgr->canSetNetworkAnimSet())
    {
      identityPacket.m_networkManagementSupportFlags.setFlag(IdentificationReplyPacket::NETWORKMGMT_SET_NETWORK_ANIMSET);
    }
  }

  PhysicsDataManagementInterface* const physicsDataMgr = target->getPhysicsDataManager();
  NMP_STRNCPY_S(identityPacket.m_physicsEngineID, IdentificationReplyPacket::kPhysicEngineIDLen, "NoPhysics");
  if (physicsDataMgr)
  {
    physicsDataMgr->getPhysicsEngineID(identityPacket.m_physicsEngineID, IdentificationReplyPacket::kPhysicEngineIDLen);
  }

  PhysicsManipulationInterface* physicsManipMgr = target->getPhysicsManipManager(); 
  if(physicsManipMgr)
  {
    if(physicsManipMgr->canCreateAndEditConstraints())
    {
      identityPacket.m_physicsManipulationSupportFlags.setFlag(IdentificationReplyPacket::PHYSICSMANIP_CREATE_EDIT_CONSTRAINTS);
    }
    if(physicsManipMgr->canApplyForces())
    {
      identityPacket.m_physicsManipulationSupportFlags.setFlag(IdentificationReplyPacket::PHYSICSMANIP_APPLY_FORCES);
    }
  }

  AnimationBrowserInterface* animBrowserMgr = target->getAnimationBrowser();
  if (animBrowserMgr)
  {
    if (animBrowserMgr->canLoadAnimSource())
    {
      identityPacket.m_animBrowserSupportFlags.setFlag(IdentificationReplyPacket::ANIMBROWSER_LOAD_ANIM_SOURCE);
      identityPacket.m_animBrowserSupportFlags.setFlag(IdentificationReplyPacket::ANIMBROWSER_SET_ANIM_TIME);
    }
  }

  SceneObjectManagementInterface* sceneObjectMgr = target->getSceneObjectManager();
  if (sceneObjectMgr)
  {
    identityPacket.m_sceneObjectsSupportFlags.setFlag(IdentificationReplyPacket::SCENEOBJECTS_READ_OBJECTS);

    if (sceneObjectMgr->canCreateSceneObjects())
    {
      identityPacket.m_sceneObjectsSupportFlags.setFlag(IdentificationReplyPacket::SCENEOBJECTS_CREATE_DESTROY);
    }
    if (sceneObjectMgr->canEditSceneObjects())
    {
      identityPacket.m_sceneObjectsSupportFlags.setFlag(IdentificationReplyPacket::SCENEOBJECTS_EDIT_ATTRIBUTES);
    }
  }

  // Check for platform endianness
#if NM_HOST_BIGENDIAN
  identityPacket.m_platformCapabilitiesFlags.setFlag(IdentificationReplyPacket::PLATFORM_USES_BIG_ENDIAN);
#else
  identityPacket.m_platformCapabilitiesFlags = 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::onNewConnection(Connection* newConnection)
{
  ConnectionManagementInterface* connectionMgr = m_target->getConnectionManager();
  if (connectionMgr)
  {
    connectionMgr->onNewConnection(newConnection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServer::onConnectionClosed(Connection* closedConnection)
{
  scheduleStep(false);
  for (uint8_t i = 0; i < m_numModules; ++i)
  {
    m_modules[i]->onConnectionClosed(closedConnection);
  }
  m_target->broadcastOnConnectionClosed(closedConnection);

  // Force scene objects synchronization, as the connection that was just closed has likely destroyed the owned
  // scene objects, so we need to inform all the other connections.
  m_coreModule->synchronizeSceneObjects();
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServer::isAsynchronousSteppingEnabled() const
{
  NMP_ASSERT(m_connectionManager);
  Connection* masterConnection = m_connectionManager->getMasterConnection();
  if (masterConnection)
  {
    return masterConnection->isAsynchronousSteppingEnabled();
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
ChunkMemory::ChunkMemory(size_t initialSize, size_t reallocSize) :
  m_heapIndex(0),
  m_currentHeap(0),
  m_heapWalkIdx(0),
  m_allocCount(0),
  m_deallocCount(0),
  m_subHeapSize(reallocSize),
  m_totalAllocSize(0)
{
  if (initialSize != 0)
    newSubHeap(initialSize);
}

//----------------------------------------------------------------------------------------------------------------------
ChunkMemory::~ChunkMemory()
{
  logStats();

  for (unsigned int i = 0; i < m_heapIndex; ++i)
  {
    NMP::Memory::memFree(m_heapList[i].m_heap);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChunkMemory::logStats()
{
#ifdef NM_CHUNKMEM_LOG_DTOR

  const int dbgBufSize = 256;
  char dbg[dbgBufSize];

  _snprintf(dbg, dbgBufSize, "ChunkMemory: reset/dtor [  alloc: %u = %u KB  |  dealloc: %u  ]\n", m_allocCount, m_totalAllocSize / 1024, m_deallocCount);
  OutputDebugStringA(dbg);

  _snprintf(dbg, dbgBufSize, "  /-- %i heaps :\n", m_heapIndex);
  OutputDebugStringA(dbg);

  for (unsigned int i = 0; i < m_heapIndex; i++)
  {
    _snprintf(dbg, dbgBufSize, "  + heap sz: %u  max: %u \n", m_heapList[i].m_heapUse, m_heapList[i].m_heapSize);
    OutputDebugStringA(dbg);
  }
#endif // #ifdef NM_CHUNKMEM_LOG_DTOR
}

//----------------------------------------------------------------------------------------------------------------------
void ChunkMemory::resetForReuse()
{
  logStats();

  if (m_heapIndex > 0)
  {
    for (unsigned int i = 0; i < m_heapIndex; ++i)
    {
      m_heapList[i].m_heapUse = 0;
    }
    m_currentHeap = &m_heapList[0];
  }
  else
    m_currentHeap = 0;

  m_allocCount = 0;
  m_deallocCount = 0;
  m_totalAllocSize = 0;
  m_heapWalkIdx = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ChunkMemory::newSubHeap(size_t sz)
{
  Heap* tmpHeap = 0;
  for (unsigned int i = m_heapWalkIdx; i < m_heapIndex; i++)
  {
    tmpHeap = &m_heapList[i];

    if (tmpHeap->m_heapUse + sz <= tmpHeap->m_heapSize)
    {
      m_currentHeap = tmpHeap;
      m_heapWalkIdx = i;
      return;
    }
  }

  Heap* h = &m_heapList[m_heapIndex];
  m_heapIndex ++;

  // we run outta heaps
  NMP_ASSERT(m_heapIndex < 64);

  h->m_heap = (unsigned char*)NMPMemoryAllocAligned(sz, 16);
  NMP_ASSERT(h->m_heap);
  h->m_heapSize = sz;
  h->m_heapUse = 0;

  m_currentHeap = h;
  m_heapWalkIdx ++;
}

//----------------------------------------------------------------------------------------------------------------------
void* ChunkMemory::alloc(size_t size)
{
  // optionally we can store a heap to pop back to using
  // after the function has returned a pointer; this is used
  // if the size requested is > the sub-heap size (see comments below)
  Heap* pushedHeap = 0;
  unsigned int pushedWalkIdx = 0;

  // new heap required?
  if (m_currentHeap == 0 ||
      (m_currentHeap->m_heapUse + size) > m_currentHeap->m_heapSize)
  {
    // ensure the next sub-heap can contain the allocation
    if (size <= m_subHeapSize)
      newSubHeap(m_subHeapSize);
    else
    {
      // otherwise build a heap to contain this size of allocation,
      // do the work of returning the pointer, then rewind back to the
      // other heap to use it up
      pushedHeap = m_currentHeap;
      pushedWalkIdx = m_heapWalkIdx;
      newSubHeap(size);

      // we could up m_subHeapSize to 'size' here...
    }
  }

  // return memory block to use
  unsigned char* ptr = m_currentHeap->m_heap + m_currentHeap->m_heapUse;
  m_currentHeap->m_heapUse += size;

  // update class-local stats
  m_allocCount ++;
  m_totalAllocSize += size;

  // pop back to a heap recorded above
  if (pushedHeap)
  {
    m_currentHeap = pushedHeap;
    m_heapWalkIdx = pushedWalkIdx;
  }

  return (void*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void ChunkMemory::dealloc(void*)
{
  m_deallocCount ++;
}

} // namespace MCOMMS
//----------------------------------------------------------------------------------------------------------------------
