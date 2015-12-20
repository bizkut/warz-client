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
#include "defaultAssetMgr.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/mcomms.h"
#include "comms/packet.h"
#include "comms/connection.h"
#include "comms/simpleBundleUtils.h"
#include "comms/networkManagementUtils.h"
#include "iPhysicsMgr.h"
#include "simpleBundle/simpleBundle.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include "NMPlatform/NMFile.h"
#include "defaultControllerMgr.h"
#include <algorithm>
#include "NMPlatform/NMHash.h"

#include "defaultDataManager.h"
#include "networkDefRecordManager.h"
#include "networkInstanceRecordManager.h"
#include "runtimeTargetContext.h"

#include "runtimeTargetLogger.h"
//----------------------------------------------------------------------------------------------------------------------

#define DISABLE_NETWORK_DEF_CACHINGx
#define MAX_UNREFERENCED_NETWORK_DEFINITIONS (5)


//----------------------------------------------------------------------------------------------------------------------
MR::AnimSourceBase* DefaultAssetMgr::networkAnimLoader(const MR::RuntimeAnimAssetID animAssetID, void* userdata)
{
  AnimLoadingInfo* info = (AnimLoadingInfo*)userdata;
  NMP_ASSERT(info);

  static char networkPath[128];
  static char completeFilePath[128];
  networkPath[0] = 0;

  size_t currentNetworkStrLen = NMP_STRLEN(info->m_currentNetworkName);

  size_t pathDividePoint = currentNetworkStrLen -1;
  while (pathDividePoint < currentNetworkStrLen)
  {
    if (info->m_currentNetworkName[pathDividePoint] == '\\' || info->m_currentNetworkName[pathDividePoint] == '/')
    {
      pathDividePoint++;
      break;
    }
    else
    {
      pathDividePoint--;
    }
  }

  if(pathDividePoint > 0 && pathDividePoint < 127)
  {
    memcpy(networkPath, info->m_currentNetworkName, sizeof(char) * pathDividePoint); 
    networkPath[pathDividePoint+1] = 0;
  }

  // Get the animation file name
  const char* shortFileName = info->m_currentNetDefSpecificAnimIDToFilename->getFilename(animAssetID);

  NMP_SPRINTF(completeFilePath, 128, "%s%s", networkPath, shortFileName);

  const MR::UTILS::SimpleAnimRegistry::Entry* e = info->m_currentAnimRegistry->getEntry(completeFilePath);
  if (e)
  {
    info->m_currentAnimRegistry->addReferenceToEntry(const_cast<MR::UTILS::SimpleAnimRegistry::Entry*>(e));
    return e->getAnim();
  }

  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "    Loading animation: '%s'\n", completeFilePath);

  // determine file size
  uint32_t fileSize = info->m_currentConnection->getFileSize(completeFilePath);
  if (fileSize == 0)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER, "    ERROR - Loading anim file %s: size is 0.\n", completeFilePath);
    return 0;
  }

  // Allocate buffer
  void* fileBuffer = NMPMemoryAllocAligned(fileSize, NMP_VECTOR_ALIGNMENT);
  if (fileBuffer == 0)
  {
    NM_LOG_ERROR_MESSAGE(
      RTT_LOGGER,
      "    ERROR - Loading anim file: Failed to allocate file buffer, size %d, for anim file %s\n",
      fileSize,
      completeFilePath);
    return 0;
  }

  // Load file via sm_currentConnection
  bool loaded = info->m_currentConnection->getFile(fileBuffer, fileSize, completeFilePath);
  if (!loaded)
  {
    NM_LOG_ERROR_MESSAGE(
      RTT_LOGGER,
      "    ERROR - Loading anim file: Failed to retrieve file %s\n",
      completeFilePath);
    NMP::Memory::memFree(fileBuffer);
    return 0;
  }

  // Locate the animation in memory
  MR::AnimSourceBase* animSource = (MR::AnimSourceBase*) fileBuffer;

  // just a small verification that the format matches the defined format during compile time
  // this can't be done in code because core is not even knowing of the actual files that are represented
  // by runtime ids.
#ifdef NMP_ENABLE_ASSERTS
  MR::AnimType animType = animSource->getType();
  NMP::endianSwap(animType);
  const char* definedTypeStr = info->m_currentNetDefSpecificAnimIDToFilename->getFormatType(animAssetID);
  const char* currentTypeStr = MR::Manager::getInstance().getInstance().findAnimationFormatRegistryEntry(animType)->m_animationFormatString;
  NMP_VERIFY_MSG(strcmp(currentTypeStr, definedTypeStr) == 0, "Anim type Mismatch for File %s!", completeFilePath);

  uint32_t fileCrc32 = info->m_currentNetDefSpecificAnimIDToFilename->getCRC(animAssetID);
  uint32_t crc32 = NMP::hashCRC32(fileBuffer, fileSize);
  NMP_ASSERT_MSG(crc32 == fileCrc32, "Animation data for %s corrupt! Please rebuild the asset!", completeFilePath);
  NMP::endianSwap(fileCrc32);
  NMP_ASSERT_MSG(crc32 == fileCrc32, "Animation data for %s corrupt! Please rebuild the asset!", completeFilePath);
#endif

  // Register the anim - the registry will now manage this memory.
  info->m_currentAnimRegistry->addEntry(completeFilePath, animSource);

  return animSource;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::networkAnimUnLoader(
  const MR::RuntimeAnimAssetID  animAssetID,
  MR::AnimSourceBase*           loadedAnimation,
  void*                         userdata)
{
  AnimLoadingInfo* info = (AnimLoadingInfo*)userdata;
  NMP_ASSERT(info);

  if (loadedAnimation)
  {
    // Do not unload an animation whos registry ref count is not zero.
    const MR::UTILS::SimpleAnimRegistry::Entry* e = info->m_currentAnimRegistry->getEntry(loadedAnimation);
    if (e)
    {
      NMP_ASSERT(loadedAnimation == e->getAnim());
      if (info->m_currentAnimRegistry->removeReferenceFromEntry(const_cast<MR::UTILS::SimpleAnimRegistry::Entry*>(e)))
      {
        // Free the anim itself.
        NMP::Memory::memFree(loadedAnimation);
      }
    }
    else
    {
      NM_LOG_ERROR_MESSAGE(
        RTT_LOGGER,
        "  Error - Attempting to unload an anim that does not exist in the anim registry. RuntimeAnimAssetID: '%i'\n",
        animAssetID);
      NMP_ASSERT_FAIL();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
DefaultAssetMgr::DefaultAssetMgr(RuntimeTargetContext* const context, MCOMMS::IPluginValidator* validatePluginList) :
  MCOMMS::NetworkDefLifecycleInterface(),
  MCOMMS::NetworkLifecycleInterface(),
  m_networkDefManager(context->getNetworkDefManager()),
  m_networkInstanceManager(context->getNetworkInstanceManager()),
  m_physicsMgr(NULL),
  m_pluginValidator(validatePluginList)
{
  m_numRefCountIncCallbacks = 0;
  m_numRefCountDecCallbacks = 0;
  m_numDeleteOrphanedAssetCallbacks = 0;
}

//----------------------------------------------------------------------------------------------------------------------
DefaultAssetMgr::~DefaultAssetMgr()
{
  clearCachedData();
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::loadNetworkDefinition(
  const MCOMMS::GUID&                guid,
  const char*                        networkDefName,
  MCOMMS::Connection*                connection)
{
  // Avoid loading the same NetDef twice
  NetworkDefRecord* networkDefRecord = m_networkDefManager->findNetworkDefRecord(guid);
  if (networkDefRecord != 0)
  {
    return true;  // NetworkDef already exists don't load it again.
  }

  static char filename[128];
  NMP_SPRINTF(filename, 128, "%s.nmb", networkDefName);

  // File size
  uint32_t fileSize = connection->getFileSize(filename);
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "------------------\n");
  NM_LOG_MESSAGE(
    RTT_LOGGER,
    RTT_MESSAGE_PRIORITY,
    "Loading Network Def: File '%s' has size %u bytes\n",
    filename,
    fileSize);
  if (fileSize == 0)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER, "Network Def file load failed: File size = 0\n");
    return false;
  }

  // Allocate buffer for file and transfer it
  void* fileBuffer = NMPMemoryAllocAligned(fileSize, NMP_VECTOR_ALIGNMENT);
  if (fileBuffer == 0)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER, "Network Def file load failed: Memory allocation failed\n");
    return false;
  }

  bool fileValid = connection->getFile(fileBuffer, fileSize, filename);
  if (!fileValid)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER,  "Network Def file load failed: File not valid\n");
    NMP::Memory::memFree(fileBuffer);
    return false;
  }

  // Setup the current connection so that the animation loader can find it
  m_connection = connection;

  MCOMMS::GUID GUIDFromFile;
  bool successfulLoad = loadNetworkDefinitionFromBuffer(
                          fileBuffer,
                          fileSize,
                          networkDefRecord,
                          GUIDFromFile,
                          networkDefName);
  if (successfulLoad == true)
  {
    // Check that the guid passed in matches the GUID we found in the file.
    NMP_ASSERT(GUIDFromFile == guid);

    m_connection = NULL; // reset current connection
  }

  // Free the main memory block
  NMP::Memory::memFree(fileBuffer);
  return successfulLoad;

}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::loadNetworkDefinitionFromBuffer(
  void*               fileBuffer,
  size_t              fileSize,
  NetworkDefRecord*   networkDefRecord,
  MCOMMS::GUID&       guid,
  const char*         networkDefName)
{
  NMP_ASSERT(NMP_IS_ALIGNED(fileBuffer, NMP_VECTOR_ALIGNMENT));
  MR::NetworkPredictionDef* netPredictionDef;
  MR::NetworkDef* networkDef = MCOMMS::readNetworkFromSimpleBundle(fileBuffer, fileSize, guid, m_pluginValidator, netPredictionDef);
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* netDefSpecificAnimIDtoFilename =
    MCOMMS::readAnimIDToFilenameFromSimpleBundle(fileBuffer, fileSize);

  //--------------------------
  // If all the net def and the anim ID lookup table have been loaded then pre-load all the anims
  if (networkDef && netDefSpecificAnimIDtoFilename)
  {
    // register the connection and registry for the asset loading (which is a static function)
    AnimLoadingInfo info;
    info.m_currentConnection = m_connection;
    info.m_currentAnimRegistry = &m_animRegistry;
    info.m_currentNetDefSpecificAnimIDToFilename = netDefSpecificAnimIDtoFilename;
    info.m_currentNetworkName = networkDefName;
    MR::Manager::getInstance().setAnimFileHandlingFunctions(networkAnimLoader, networkAnimUnLoader);
    for (MR::AnimSetIndex i = 0; i < networkDef->getNumAnimSets(); ++i)
    {
      // Fixup the animations and rigToAnimMaps.
      NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "  ------------------\n");
      NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "  Loading anims for set %i\n", i);
      networkDef->loadAnimations(i, (void*)&info);
    }
  }

  NMP::Memory::memFree(netDefSpecificAnimIDtoFilename);

  // New NetworkDefRecord.
  networkDefRecord = new NetworkDefRecord(guid, networkDefName, networkDef, netPredictionDef);
  m_networkDefManager->addNetworkDefRecord(networkDefRecord);

  return networkDef != 0 && netDefSpecificAnimIDtoFilename != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::canLoadNetworkDefinition() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::canCreateNetworkInstance() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::InstanceID DefaultAssetMgr::createNetworkInstance(
  const MCOMMS::GUID& guid,
  const char*         instanceName,
  uint32_t            startingAnimSetIndex,
  MCOMMS::Connection* connection,
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation)
{
  if (connection->isMaster() && m_physicsMgr)
  {
    m_physicsMgr->initializeScene();
  }

  // The NetDef should already exist
  NetworkDefRecord* defRecord = m_networkDefManager->findNetworkDefRecord(guid);
  if (defRecord == 0)
  {
    return MCOMMS::INVALID_INSTANCE_ID;
  }
  NMP_ASSERT(guid == defRecord->getGUID());

  // Create instance records and add to list
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(NetworkInstanceRecord), NMP_VECTOR_ALIGNMENT);
  NetworkInstanceRecord* instanceRecord = new(alignedMemory) NetworkInstanceRecord(
    getUnusedInstanceID(),
    defRecord,
    connection,
    instanceName);

  instanceRecord->init(
    startingAnimSetIndex,
    characterStartPosition,
    characterStartRotation); // This calls network update and so depends on the character controller existing.

  if (!instanceRecord->getNetwork())
  {
    delete instanceRecord;
    return 0xFFFFFFFF;
  }

  // Increase the reference count on the NetworkDef (and all sub-assets) we are using.
  incNetworkDefRefCounts(defRecord->getNetDef());

  // Add to the map
  m_networkInstanceManager->addNetworkInstanceRecord(instanceRecord);

  return instanceRecord->getInstanceID();
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::destroyNetworkInstance(MCOMMS::InstanceID id)
{
  if (m_physicsMgr != NULL)
  {
    m_physicsMgr->getControllerManager()->destroyControllerRecord(id);
  }

  resetMapCache();

  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "------------------\n");
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "Destroying Network Instance Record with ID: %u\n", id);
  NetworkInstanceRecord* instanceRecord = m_networkInstanceManager->findInstanceRecord(id);
  if (instanceRecord == 0)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER, "  Warning - request for destruction of non-existant network instance.");
    return false;
  }

  // Decrease the reference count on the def we are using.
  NetworkDefRecord* defRecord = instanceRecord->getNetworkDefRecord();
  if (defRecord == 0)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER, "  Cannot decrement reference to networkDef because the Network instance we are destroying has no Def record.");
  }
  else
  {
    // Decrease the reference count on the NetworkDef (and all sub-assets) we are using.
    decNetworkDefRefCounts(defRecord->getNetDef());
  }

  m_networkInstanceManager->removeNetworkInstanceRecord(id);
  // Must remove corresponding physics record as well?
  instanceRecord->~NetworkInstanceRecord();
  NMP::Memory::memFree(instanceRecord);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::destroyNetworkDefinition(const MCOMMS::GUID& guid)
{
  resetMapCache();

  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "------------------\n");
#ifdef NM_DEBUG
  const char* name = MCOMMS::guidToString(&guid);
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "Starting deletion of NetworkDef - GUID: %s, AssetID: %p.\n", name);
#endif

  // Get the record.
  NetworkDefRecord* defRecord = m_networkDefManager->findNetworkDefRecord(guid);
  if (defRecord == 0)
  {
    NM_LOG_ERROR_MESSAGE(RTT_LOGGER, "  Warning - request for destruction of non-existant network definition.");
    return false;
  }

  // add this record to this list of definitions that are not referenced.  If a non 0 network is
  // returned it means the returned record should be deleted.
#ifdef DISABLE_NETWORK_DEF_CACHING
  NetworkDefRecord* recordToDelete = defRecord;
#else
  NetworkDefRecord* recordToDelete = addToUnreferencedNetworkDefinitionList(defRecord);
#endif

  if (recordToDelete == 0)
  {
    // Network is added to cache of net defs, nothing more to do
    return true;
  }

  return destroyNetworkDefRecord(recordToDelete);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::destroyNetworkDefRecord(NetworkDefRecord* recordToDelete)
{
#ifdef NMP_ENABLE_ASSERTS
  // TODO: Make sure that this def is in the unreffed net def list
  // TODO: Make sure that this def has no other references
#endif
  // Get the def.
  MR::NetworkDef* networkDefToDelete = recordToDelete->getNetDef();

  // Unload this NetworDefs referenced animations.
  // register the connection and registry for the asset loading (which is a static function)
  AnimLoadingInfo info;
  info.m_currentConnection = NULL;
  info.m_currentAnimRegistry = &m_animRegistry;
  info.m_currentNetDefSpecificAnimIDToFilename = NULL;
  for (MR::AnimSetIndex i = 0; i < networkDefToDelete->getNumAnimSets(); ++i)
  {
    networkDefToDelete->unloadAnimations(i, (void*)&info);
  }
  // register the connection and registry for the asset loading (which is a static function)

  // Destroy the networkDef.
  deleteOrphanedNetworkDefAssets(networkDefToDelete);

  // Remove DefRecord from DefaultDataManager's list
  m_networkDefManager->removeNetworkDefRecord(recordToDelete->getGUID());
  delete recordToDelete;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefRecord* DefaultAssetMgr::addToUnreferencedNetworkDefinitionList(NetworkDefRecord* record)
{
  // The record should never exist on the list if it is being unreferenced
  for (size_t i = 0; i < m_unreferencedDefList.size(); ++i)
  {
    if (record == m_unreferencedDefList.at(i))
    {
      return 0;
    }
  }

  // Make sure that we have space on the unused def list
  NetworkDefRecord* defintionToDelete = 0;
  if (m_unreferencedDefList.size() >= MAX_UNREFERENCED_NETWORK_DEFINITIONS)
  {
    // we need to remove the top item of the list
    defintionToDelete = *(m_unreferencedDefList.begin());
#ifdef NMP_ENABLE_ASSERTS
    bool success =
#endif
      removeFromUnreferencedNetworkDefinitionList(defintionToDelete);
    NMP_ASSERT(success);
  }

  // We now have space for this definition
  m_unreferencedDefList.push_back(record);
  // Increment the reference count as the unreferenced network definition list now references the NetworkDef.
  incNetworkDefRefCounts(record->getNetDef());

  // Return a reference to the def that was on the unused list but now can be fully cleaned up
  return defintionToDelete;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::removeFromUnreferencedNetworkDefinitionList(NetworkDefRecord* record)
{
  NetworkDefList::iterator it;
  for (it = m_unreferencedDefList.begin(); it != m_unreferencedDefList.end(); ++it)
  {
    if (record == *it)
    {
      // Found the record so remove it
      m_unreferencedDefList.erase(it);
      decNetworkDefRefCounts(record->getNetDef());
      return true;
    }
  }

  // Unable to find this record in the list
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultAssetMgr::clearCachedData()
{
  if (m_physicsMgr)
  {
    m_physicsMgr->clearScene();
  }

  // Destroy all network instances
  NetworkInstanceMap& instMap = m_networkInstanceManager->getNetworkInstanceMap();
  while (instMap.size() > 0)
  {
    MCOMMS::InstanceID id = (*instMap.begin()).second->getInstanceID();
    destroyNetworkInstance(id);
  }

  // Clear the cache of unused net defs
  while (m_unreferencedDefList.size() > 0)
  {
    NetworkDefRecord* defintionToDelete = *(m_unreferencedDefList.begin());

#ifdef NMP_ENABLE_ASSERTS
    bool success =
#endif
      removeFromUnreferencedNetworkDefinitionList(defintionToDelete);
    NMP_ASSERT(success);

    // No longer in the list, now just clear it up
    destroyNetworkDefRecord(defintionToDelete);
  }

  // Final check to make sure that all defs are removed
  NetworkDefMap& defMap = m_networkDefManager->getNetworkDefMap();
  while (defMap.size() > 0)
  {
    NetworkDefRecord* defintionToDelete = (*defMap.begin()).second;
    destroyNetworkDefRecord(defintionToDelete);
  }

  // Make sure everything is cleared up
  NMP_ASSERT(m_networkDefManager->getNetworkDefMap().size() == 0);
  NMP_ASSERT(m_networkInstanceManager->getNetworkInstanceMap().size() == 0);
  NMP_ASSERT(m_unreferencedDefList.size() == 0);

  // Make sure no animations are still cached.
  NMP_ASSERT(m_animRegistry.getNumEntries() == 0);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::InstanceID DefaultAssetMgr::getUnusedInstanceID()
{
  static uint32_t nextID = 0;
  return nextID++;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Reference counting of NetworkDefs is based on the number of Networks that have been instanced from a particular Def.
// On instancing a Network we increment the reference count of the NetworkDef and any asset Defs that are referenced
// by this NetworkDef.
// Note: NetworkDef and all sub assets must already have been registered with the manager.
void DefaultAssetMgr::incNetworkDefRefCounts(MR::NetworkDef* networkDef)
{
  uint32_t refCount = 0;

  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "------------------\n");

  // NetworkDef itself.
  refCount = MR::Manager::incObjectRefCount(networkDef);
  NM_LOG_MESSAGE(
    RTT_LOGGER,
    RTT_MESSAGE_PRIORITY,
    "Increasing RefCount - NetworkDef AssetID: %p, RefCount: %d\n",
    MR::Manager::getInstance().getObjectIDFromObjectPtr(networkDef),
    refCount);

  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSetIdx = 0; animSetIdx < numAnimSets; ++animSetIdx)
  {
    // Anim rigs.
    MR::AnimRigDef* animRig = networkDef->getRig(animSetIdx);
    NMP_ASSERT(animRig);
    refCount = MR::Manager::incObjectRefCount(animRig);
    NM_LOG_MESSAGE(
      RTT_LOGGER,
      RTT_MESSAGE_PRIORITY,
      "  Increasing RefCount - AnimRigDef AssetID: %p, RefCount: %d\n",
      MR::Manager::getInstance().getObjectIDFromObjectPtr(animRig),
      refCount);

    // Sample character controllers.
    MR::CharacterControllerDef* characterController = networkDef->getCharacterControllerDef(animSetIdx);
    if (characterController)
    {
      refCount = MR::Manager::incObjectRefCount(characterController);
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "  Increasing RefCount - CharacterControllerDef AssetID: %p, RefCount: %d\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(characterController),
        refCount);
    }

    for (uint16_t nodeIndex = 0; nodeIndex < networkDef->getNumNodeDefs(); nodeIndex++)
    {
      // RigToAnimMaps.
      MR::AttribDataSourceAnim* sourceAnimAttr = (MR::AttribDataSourceAnim*)
        networkDef->getOptionalAttribData(
          MR::ATTRIB_SEMANTIC_SOURCE_ANIM,
          nodeIndex,
          animSetIdx); // Because we don't have attrib data for anim sets that this node cannot be in (e.g. through use a retarget node)
                       // We call the optional version and handle the return.
      if(sourceAnimAttr == NULL)
      {
        continue;
      }

      const MR::NodeDef * nodeDef = networkDef->getNodeDef(nodeIndex);
      if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_ANIM_SOURCE)
      {
        MR::RigToAnimMap* rigAnimMap = sourceAnimAttr->m_rigToAnimMap;
        refCount = MR::Manager::incObjectRefCount(rigAnimMap);
        NM_LOG_MESSAGE(
          RTT_LOGGER,
          RTT_MESSAGE_PRIORITY,
          "    Increasing RefCount - RigToAnimMap AssetID: %p, RefCount: %d\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(rigAnimMap),
          refCount);

        // Event tracks.
        // Discrete event tracks.
        MR::AttribDataSourceEventTrackSet* sourceDiscreteEventTracks =
            networkDef->getAttribData<MR::AttribDataSourceEventTrackSet>(
              MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,
              nodeIndex,
              animSetIdx);
        MR::EventTrackDefDiscrete** discreteTracks = sourceDiscreteEventTracks->m_sourceDiscreteEventTracks;
        for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numDiscreteEventTracks; ++i)
        {
          if (discreteTracks[i] != NULL)
          {
            refCount = MR::Manager::incObjectRefCount(discreteTracks[i]);
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "    Increasing RefCount - EventTrackDefDiscrete AssetID: %p, RefCount: %d\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(discreteTracks[i]),
              refCount);
          }
        }

        // Curve event tracks.
        MR::EventTrackDefCurve** curveTracks = sourceDiscreteEventTracks->m_sourceCurveEventTracks;
        for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numCurveEventTracks; ++i)
        {
          if (curveTracks[i] != NULL)
          {
            refCount = MR::Manager::incObjectRefCount(curveTracks[i]);
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "    Increasing RefCount - EventTrackDefCurve AssetID: %p, RefCount: %d\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(curveTracks[i]),
              refCount);
          }
        }

        // Duration event tracks.
        MR::EventTrackDefDuration** durationTracks = sourceDiscreteEventTracks->m_sourceDurEventTracks;
        for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numDurEventTracks; ++i)
        {
          if (durationTracks[i] != NULL)
          {
            refCount = MR::Manager::incObjectRefCount(durationTracks[i]);
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "    Increasing RefCount - EventTrackDefDuration AssetID: %p, RefCount: %d\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(durationTracks[i]),
              refCount);
          }
        }
      }
    }
  }

  // Call any registered callbacks to increment reference counts on any plug-in assets.
  invokeRefCountIncCallbacks(networkDef);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::decNetworkDefRefCounts(MR::NetworkDef* networkDef)
{
  uint32_t refCount = 0;

  // NetworkDef itself.
  refCount = MR::Manager::decObjectRefCount(networkDef);
  NM_LOG_MESSAGE(
    RTT_LOGGER,
    RTT_MESSAGE_PRIORITY,
    "  Decreasing RefCount - NetworkDef AssetID: %p, RefCount: %d\n",
    MR::Manager::getInstance().getObjectIDFromObjectPtr(networkDef),
    refCount);

  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSetIdx = 0; animSetIdx < numAnimSets; ++animSetIdx)
  {
    // Anim rigs.
    MR::AnimRigDef* animRig = networkDef->getRig(animSetIdx);
    NMP_ASSERT(animRig);
    refCount = MR::Manager::decObjectRefCount(animRig);
    NM_LOG_MESSAGE(
      RTT_LOGGER,
      RTT_MESSAGE_PRIORITY,
      "    Decreasing RefCount - AnimRigDef AssetID: %p, RefCount: %d\n",
      MR::Manager::getInstance().getObjectIDFromObjectPtr(animRig),
      refCount);

    // Sample character controllers.
    MR::CharacterControllerDef* characterController = networkDef->getCharacterControllerDef(animSetIdx);
    if (characterController)
    {
      refCount = MR::Manager::decObjectRefCount(characterController);
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "    Decreasing RefCount - CharacterControllerDef AssetID: %p, RefCount: %d\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(characterController),
        refCount);
    }

    for (uint16_t nodeIndex = 0; nodeIndex < networkDef->getNumNodeDefs(); nodeIndex++)
    {
      // RigToAnimMaps.
      MR::AttribDataSourceAnim* sourceAnimAttr = (MR::AttribDataSourceAnim*)
        networkDef->getOptionalAttribData(
          MR::ATTRIB_SEMANTIC_SOURCE_ANIM,
          nodeIndex,
          animSetIdx);// Because we don't have attrib data for anim sets that this node cannot be in (e.g. through use a retarget node)
                      // We call the optional version and handle the return.
      if(sourceAnimAttr == NULL)
      {
        continue;
      }

      const MR::NodeDef * nodeDef = networkDef->getNodeDef(nodeIndex);
      if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_ANIM_SOURCE)
      {
        MR::RigToAnimMap* rigAnimMap = sourceAnimAttr->m_rigToAnimMap;
        refCount = MR::Manager::decObjectRefCount(rigAnimMap);
        NM_LOG_MESSAGE(
          RTT_LOGGER,
          RTT_MESSAGE_PRIORITY,
          "      Decreasing RefCount - RigToAnimMap AssetID: %p, RefCount: %d\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(rigAnimMap),
          refCount);

        // Event tracks.
        // Discrete event tracks.
        MR::AttribDataSourceEventTrackSet* sourceDiscreteEventTracks =
            networkDef->getAttribData<MR::AttribDataSourceEventTrackSet>(
              MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,
              nodeIndex,
              animSetIdx);
        MR::EventTrackDefDiscrete** discreteTracks = sourceDiscreteEventTracks->m_sourceDiscreteEventTracks;
        for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numDiscreteEventTracks; i++)
        {
          if (discreteTracks[i] != NULL)
          {
            refCount = MR::Manager::decObjectRefCount(discreteTracks[i]);
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "      Decreasing RefCount - EventTrackDefDiscrete AssetID: %p, RefCount: %d\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(discreteTracks[i]),
              refCount);
          }
        }

        // Curve event tracks.
        MR::EventTrackDefCurve** curveTracks = sourceDiscreteEventTracks->m_sourceCurveEventTracks;
        for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numCurveEventTracks; i++)
        {
          if (curveTracks[i] != NULL)
          {
            refCount = MR::Manager::decObjectRefCount(curveTracks[i]);
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "      Decreasing RefCount - EventTrackDefCurve AssetID: %p, RefCount: %d\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(curveTracks[i]),
              refCount);
          }
        }

        // Duration event tracks.
        MR::EventTrackDefDuration** durationTracks = sourceDiscreteEventTracks->m_sourceDurEventTracks;
        for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numDurEventTracks; i++)
        {
          if (durationTracks[i] != NULL)
          {
            refCount = MR::Manager::decObjectRefCount(durationTracks[i]);
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "      Decreasing RefCount - EventTrackDefDuration AssetID: %p, RefCount: %d\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(durationTracks[i]),
              refCount);
          }
        }
      }
    }
  }

  // Call any registered callbacks to decrement reference counts on any plug-in assets.
  invokeRefCountDecCallbacks(networkDef);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::deleteOrphanedNetworkDefAssets(MR::NetworkDef* networkDef)
{
  if (MR::Manager::getInstance().getObjectRefCount(networkDef) == 0)
  {
    // Delete any plugin assets that have hit zero references.
    invokeDeleteOrphanedAssetCallbacks(networkDef);

    uint32_t numAnimSets = networkDef->getNumAnimSets();
    for (uint16_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
    {
      for (uint16_t nodeIndex = 0; nodeIndex < networkDef->getNumNodeDefs(); nodeIndex++)
      {
        // RigToAnimMaps.
        MR::AttribDataSourceAnim* sourceAnimAttr = (MR::AttribDataSourceAnim*)
          networkDef->getOptionalAttribData(
            MR::ATTRIB_SEMANTIC_SOURCE_ANIM,
            nodeIndex,
            animSetIndex);// Because we don't have attrib data for anim sets that this node cannot be in (e.g. through use a retarget node)
                          // We call the optional version and handle the return.
        if(sourceAnimAttr == NULL)
        {
          continue;
        }       

        const MR::NodeDef * nodeDef = networkDef->getNodeDef(nodeIndex);
        if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_ANIM_SOURCE)
        {
          MR::RigToAnimMap* rigAnimMap = sourceAnimAttr->m_rigToAnimMap;
          if (MR::Manager::getInstance().getObjectRefCount(rigAnimMap) == 0)
          {
            NM_LOG_MESSAGE(
              RTT_LOGGER,
              RTT_MESSAGE_PRIORITY,
              "  Deleting referenced RigToAnimMap - AssetID: %p.\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(rigAnimMap));
            MR::Manager::getInstance().unregisterObject(rigAnimMap);
            NMP::Memory::memFree(rigAnimMap);
          }

          //----------
          // Decrement ref counts for all the event tracks used in this animation set.

          // Discrete event tracks.
          deleteOrphanDiscreteTracks(networkDef, nodeIndex, animSetIndex);

          // Curve event tracks.
          deleteOrphanCurveTracks(networkDef, nodeIndex, animSetIndex);

          // Duration events tracks.
          deleteOrphanDurationTracks(networkDef, nodeIndex, animSetIndex);
        }
      }

      //----------
      // Sample character controller.
      MR::CharacterControllerDef* characterControllerDef = networkDef->getCharacterControllerDef(animSetIndex);
      if (characterControllerDef && (MR::Manager::getInstance().getObjectRefCount(characterControllerDef) == 0))
      {
        NM_LOG_MESSAGE(
          RTT_LOGGER,
          RTT_MESSAGE_PRIORITY,
          "  Deleting referenced CharacterControllerDef - AssetID: %p.\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(characterControllerDef));
        MR::Manager::getInstance().unregisterObject(characterControllerDef);
        NMP::Memory::memFree(characterControllerDef);
      }

      //----------
      // Anim Rig.
      MR::AnimRigDef* rig = networkDef->getRig(animSetIndex);
      if (rig && (MR::Manager::getInstance().getObjectRefCount(rig) == 0))
      {
        NM_LOG_MESSAGE(
          RTT_LOGGER,
          RTT_MESSAGE_PRIORITY,
          "  Deleting referenced AnimRigDef - AssetID: %p.\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(rig));
        MR::Manager::getInstance().unregisterObject(rig);
        NMP::Memory::memFree(rig);
      }
    }

    // Network Def.
    NM_LOG_MESSAGE(
      RTT_LOGGER,
      RTT_MESSAGE_PRIORITY,
      "Finished deletion of NetworkDef - AssetID: %p.\n",
      MR::Manager::getInstance().getObjectIDFromObjectPtr(networkDef));
    MR::Manager::getInstance().unregisterObject(networkDef);
    NMP::Memory::memFree(networkDef);
  }
  else
  {
    NM_LOG_MESSAGE(
      RTT_LOGGER,
      RTT_MESSAGE_PRIORITY,
      "Not deleting NetworkDef as ref count not 0 - AssetID: %p.\n",
      MR::Manager::getInstance().getObjectIDFromObjectPtr(networkDef));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::deleteOrphanDiscreteTracks(
  MR::NetworkDef*      networkDef,
  MR::NodeID           nodeIndex,
  uint16_t             animSetIndex)
{
  MCOMMS::deleteOrphanDiscreteTracks(networkDef, nodeIndex, animSetIndex, RTT_LOGGER, RTT_MESSAGE_PRIORITY);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::deleteOrphanCurveTracks(
  MR::NetworkDef*      networkDef,
  MR::NodeID           nodeIndex,
  uint16_t             animSetIndex)
{
  MCOMMS::deleteOrphanCurveTracks(networkDef, nodeIndex, animSetIndex, RTT_LOGGER, RTT_MESSAGE_PRIORITY);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::deleteOrphanDurationTracks(
  MR::NetworkDef*      networkDef,
  MR::NodeID           nodeIndex,
  uint16_t             animSetIndex)
{
  MCOMMS::deleteOrphanDurationTracks(networkDef, nodeIndex, animSetIndex, RTT_LOGGER, RTT_MESSAGE_PRIORITY);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::invokeRefCountIncCallbacks(MR::NetworkDef* networkDef)
{
  for (uint32_t i = 0; i < m_numRefCountIncCallbacks; i++)
  {
    m_refCountIncCallbacks[i](networkDef);
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::invokeRefCountDecCallbacks(MR::NetworkDef* networkDef)
{
  for (uint32_t i = 0; i < m_numRefCountDecCallbacks; i++)
  {
    m_refCountDecCallbacks[i](networkDef);
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::invokeDeleteOrphanedAssetCallbacks(MR::NetworkDef* networkDef)
{
  for (uint32_t i = 0; i < m_numDeleteOrphanedAssetCallbacks; i++)
  {
    m_deleteOrphanedAssetCallbacks[i](networkDef);
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::registerRefCountIncCallback(DefaultAssetMgr::AssetHandlerCB* incRefCountCB)
{
  NMP_ASSERT_MSG(m_numRefCountIncCallbacks < MAX_PLUGINS, "Too many asset manager plugins registered");
  m_refCountIncCallbacks[m_numRefCountIncCallbacks++] = incRefCountCB;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::registerRefCountDecCallback(DefaultAssetMgr::AssetHandlerCB* decRefCountCB)
{
  NMP_ASSERT_MSG(m_numRefCountDecCallbacks < MAX_PLUGINS, "Too many asset manager plugins registered");
  m_refCountDecCallbacks[m_numRefCountDecCallbacks++] = decRefCountCB;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::registerDeleteOrphanedAssetCallback(DefaultAssetMgr::AssetHandlerCB* deleteAssetCB)
{
  NMP_ASSERT_MSG(m_numDeleteOrphanedAssetCallbacks < MAX_PLUGINS, "Too many asset manager plugins registered");
  m_deleteOrphanedAssetCallbacks[m_numDeleteOrphanedAssetCallbacks++] = deleteAssetCB;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultAssetMgr::onConnectionClosed(MCOMMS::Connection* connection)
{
  const NetworkInstanceMap& networkInstances = m_networkInstanceManager->getNetworkInstanceMap();
  const NetworkInstanceMap::const_iterator end = networkInstances.end();

  // Find all network instances owned by the given connection ...
  std::vector<MCOMMS::InstanceID> instances;
  {
    instances.reserve(networkInstances.size());

    for (NetworkInstanceMap::const_iterator it = networkInstances.begin(); it != end; ++it)
    {
      if (it->second->getOwner() == connection)
      {
        const MCOMMS::InstanceID instanceID = it->second->getInstanceID();
        instances.push_back(instanceID);
      }
    }
  }

  // .. then destroy them
  for (std::vector<MCOMMS::InstanceID>::iterator it = instances.begin(); it != instances.end(); ++it)
  {
    destroyNetworkInstance((*it));
  }
}
