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
#include "comms/simpleAnimBrowserManager.h"

#include "NMPlatform/NMPrioritiesLogger.h"

#ifdef NMP_MEMORY_LOGGING
#include "NMPlatform/NMLoggingMemoryAllocator.h"
#endif // NMP_MEMORY_LOGGING

#include "morpheme/mrNetwork.h"

#include "comms/connection.h"
#include "comms/scopedMemory.h"
#include "comms/simpleBundleUtils.h"
#include "comms/networkManagementUtils.h"
#include "morpheme/mrDebugManager.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/mrManager.h"
#include "sharedDefines/mSharedDefines.h"

#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include "simpleBundle/simpleAnimRegistry.h"

#include <list>

#ifdef NM_HOST_CELL_PPU
  #include "morpheme/mrDispatcherPS3.h"

  ///< When building for PS3 the SPU optimized dispatcher is used by default.
  ///< To use the basic dispatcher (PPU only) comment out this define.
  ///< *** ONLY DO THIS TEMPORARILY WHILST DEBUGGING ***
  ///< Otherwise it is easy to forget it and continue to add functionality that you will not be testing for execution on SPU.
  #define NM_USE_SPU_DISPATCHER_ON_PS3
#endif
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

#if defined(MR_ATTRIB_DEBUG_BUFFERING)

/// \def ANIM_BROWSER_INSTANCE_ID
///
/// The debug interfaces require an instance ID, but the animation browser network does not make use of instance ids 
/// because there is only a single network.  Use a default value where required.
static const MCOMMS::InstanceID ANIM_BROWSER_INSTANCE_ID = 0;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Per connection data holding the previewed animation browser network.
//----------------------------------------------------------------------------------------------------------------------
class SimpleAnimBrowserManager::ConnectionData
{
public:

  explicit ConnectionData(
    Connection* const                     connection,
    NMP::PrioritiesLogger&                logger,
    int32_t                               loggerPriority
    );

  ~ConnectionData();

  Connection* getConnection() const;

  std::list<MR::AnimSourceBase*> *getLoadedAnimations();

  MR::NetworkDef* getNetworkDef() const;
  bool setNetworkDef(MR::NetworkDef* networkDef, const MCOMMS::GUID& networkGuid);
  bool createNetworkInstance(); 
  bool hasValidNetworkInstance() const;

  void updateToZero();
  float setAnimationBrowserTime(float time);
  float getAnimationBrowserTime() const; 

  float getAnimationBrowserDuration();

  uint32_t getDataSourceSize(AnimationBrowserInterface::AnimationDataSources dataSource); 

  bool serializeDataSource(
    AnimationBrowserInterface::AnimationDataSources dataSource,
    MR::AttribTransformBufferOutputData* data);

  const char* getAnimBrowserFileServerPath() const;
  bool setAnimBrowserFileServerPath(const char* animBrowserPath);

  const char* getAnimBrowserNetwork() const;
  bool setAnimationBrowserNetwork(
    const char* animBrowserNetwork,
    SimpleAnimBrowserManager::NetworkDescriptor networkDescriptor);

  void freeNetworkDef();
  void freeNetworkInstance();
  void freeLoadedAnimSources();

  bool broadcastAnimationBrowserMessage(const MR::Message& message);

  bool sendAnimationBrowserMessage(commsNodeID nodeID, const MR::Message& message);

  bool setActiveAnimationSet(MR::AnimSetIndex index);

  bool setAnimationBrowserControlParameter(
    commsNodeID            nodeID,
    MR::NodeOutputDataType type,
    const void*            cparamData);

  void setAnimSourceBase(MR::AnimSourceBase* anim);

private:
  void deleteOrphanedNetworkDefAssets(MR::NetworkDef* networkDef);

  void initDebugInterfaces();
  void updateNetwork();
  void loadAnimSourceBase();

  void findEntryAndSerializeTxFn(
    const MR::NodeID targetNodeID,
    MR::AttribDataSemantic targetSemantic,
    const MR::NodeBinEntry** outEntry,
    MR::AttribSerializeTxFn& serializeFn);

  Connection* const m_connection;
  MR::NetworkDef* m_networkDef;
  MR::Network* m_network;
  std::list<MR::AnimSourceBase*> m_loadedAnimations;
  MR::AnimSourceBase* m_animSourceBase;
  void* m_animFileBuffer;

  bool  m_validNetworkInstance;
  MCOMMS::GUID m_networkDefGuid;

  NMP::PrioritiesLogger&  m_logger;
  const int32_t           m_loggerPriority;

  char m_animBrowserFileServerPath[260]; // max path in windows is 260 bytes
  char m_animBrowserNetwork[260];
  char m_dataSourceNodeNames[SimpleAnimBrowserManager::kNumDataSources][256];
  
  MR::NodeID m_dataSourceNodeIDs[SimpleAnimBrowserManager::kNumDataSources];

  NMP::FastHeapAllocator*          m_tempAllocator;
  NMP::MemoryAllocator*            m_persistantAllocator;
  MR::InstanceDebugInterface        m_debugInterface;    // Per network debug output interface. Stores useful debugging info 

  float m_animBrowserTime;
  ConnectionData(const ConnectionData&);
  ConnectionData& operator = (const ConnectionData&);
};

//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  MR::AnimSourceBase**             m_lastLoadedAnim;
  std::list<MR::AnimSourceBase*>*   m_loadedAnims;
  MCOMMS::Connection*              m_currentConnection;
  const char*                      m_currentNetworkName;
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* m_currentNetDefSpecificAnimIDToFilename;
} AnimBrowserAnimLoadingInfo;

//----------------------------------------------------------------------------------------------------------------------
static MR::AnimSourceBase* animBrowserAnimFileLoader(
  const MR::RuntimeAnimAssetID animAssetID,
  void* userdata)
{
  AnimBrowserAnimLoadingInfo* info = (AnimBrowserAnimLoadingInfo*)userdata;
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

  // determine file size
  uint32_t fileSize = info->m_currentConnection->getFileSize(completeFilePath);
  if (fileSize == 0)
  {
    return 0;
  }

  // Allocate buffer
  void* fileBuffer = NMPMemoryAllocAligned(fileSize, NMP_VECTOR_ALIGNMENT);
  if (fileBuffer == 0)
  {
    return 0;
  }

  // Load file via sm_currentConnection
  bool loaded = info->m_currentConnection->getFile(fileBuffer, fileSize, completeFilePath);
  if (!loaded)
  {
    return 0;
  }

  // Locate the animation in memory
  *info->m_lastLoadedAnim = (MR::AnimSourceBase*) fileBuffer;

  info->m_loadedAnims->push_back((MR::AnimSourceBase*) fileBuffer);

  return *info->m_lastLoadedAnim;
}


//----------------------------------------------------------------------------------------------------------------------
static void animBrowserAnimFileUnloader(
                                const MR::RuntimeAnimAssetID NMP_UNUSED(animAssetID),
                                MR::AnimSourceBase*          NMP_UNUSED(loadedAnimation),
                                void*                        NMP_UNUSED(userdata))
{
}

//----------------------------------------------------------------------------------------------------------------------
// SimpleAnimBrowserManager::ConnectionData
//----------------------------------------------------------------------------------------------------------------------

SimpleAnimBrowserManager::ConnectionData::ConnectionData(
  Connection* const connection, 
  NMP::PrioritiesLogger& logger,
  int32_t loggerPriority)
  : m_connection(connection),
  m_networkDef(NULL),
  m_validNetworkInstance(false),
  m_logger(logger),
  m_loggerPriority(loggerPriority),
  m_animBrowserTime(0.0f)
{
  m_animBrowserFileServerPath[0] = '\0';
  m_animBrowserNetwork[0] = '\0';
  for (uint32_t i = 0; i != SimpleAnimBrowserManager::kNumDataSources; ++i)
  {
    m_dataSourceNodeNames[i][0] = '\0';
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline SimpleAnimBrowserManager::ConnectionData::~ConnectionData()
{
  if (m_networkDef)
  {
    freeNetworkDef();
  }
}

//--------------------------------------------------------------------------------------------------------------------
inline Connection* SimpleAnimBrowserManager::ConnectionData::getConnection() const
{
  return m_connection;
}

//--------------------------------------------------------------------------------------------------------------------
inline std::list<MR::AnimSourceBase*>* SimpleAnimBrowserManager::ConnectionData::getLoadedAnimations()
{
  return &m_loadedAnimations;
}

//----------------------------------------------------------------------------------------------------------------------
inline MR::NetworkDef* SimpleAnimBrowserManager::ConnectionData::getNetworkDef() const
{
  return m_networkDef;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SimpleAnimBrowserManager::ConnectionData::setNetworkDef(MR::NetworkDef* networkDef, const MCOMMS::GUID& networkDefGuid)
{
  NMP_ASSERT(m_networkDef == 0); 
  m_networkDef = networkDef;
  m_networkDefGuid = networkDefGuid; 
  if (m_networkDef)
  {
    MR::Manager::getInstance().incObjectRefCount(m_networkDef);

    const NMP::IDMappedStringTable* nodeIDNames = (const NMP::IDMappedStringTable*)m_networkDef->getNodeIDNamesTable();

    for (uint32_t i = 0; i != AnimationBrowserInterface::kNumDataSources; ++i)
    {
      m_dataSourceNodeIDs[i] = (MR::NodeID)nodeIDNames->getIDForString(m_dataSourceNodeNames[i]);
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline const char* SimpleAnimBrowserManager::ConnectionData::getAnimBrowserFileServerPath() const
{
  return m_animBrowserFileServerPath;
}


//----------------------------------------------------------------------------------------------------------------------
inline void SimpleAnimBrowserManager::ConnectionData::setAnimSourceBase(MR::AnimSourceBase* anim)
{
  m_animSourceBase = anim;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::setAnimBrowserFileServerPath(const char* animBrowserAnim)
{
  if (!animBrowserAnim)
  {
    m_animBrowserFileServerPath[0] = '\0';
    return false;
  }
  else
  {
    NMP_STRNCPY_S(m_animBrowserFileServerPath, sizeof(m_animBrowserFileServerPath), animBrowserAnim);
    return true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline const char* SimpleAnimBrowserManager::ConnectionData::getAnimBrowserNetwork() const
{
  return m_animBrowserNetwork;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::setAnimationBrowserNetwork(
  const char* animBrowserNetwork,
  SimpleAnimBrowserManager::NetworkDescriptor networkDescriptor)
{
  if (!animBrowserNetwork)
  {
    m_animBrowserNetwork[0] = '\0';
    for (uint32_t i = 0; i != SimpleAnimBrowserManager::kNumDataSources; ++i)
    {
      m_dataSourceNodeNames[i][0] = '\0';
    }
    return false;
  }
  else
  {
    NMP_STRNCPY_S(m_animBrowserNetwork, sizeof(m_animBrowserNetwork), animBrowserNetwork);
    for (uint32_t i = 0; i != SimpleAnimBrowserManager::kNumDataSources; ++i)
    {
      NMP_STRNCPY_S(m_dataSourceNodeNames[i], sizeof(m_dataSourceNodeNames[i]), networkDescriptor.m_dataSourceNodeName[i]);
    }
    return true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::freeNetworkDef()
{
  if (m_networkDef)
  {
    freeNetworkInstance();
    MR::Manager::getInstance().decObjectRefCount(m_networkDef);
    deleteOrphanedNetworkDefAssets(m_networkDef);
    m_networkDef = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::deleteOrphanedNetworkDefAssets(MR::NetworkDef* networkDef)
{
  // TODO - copied from defaultAssetMgr.cpp
  if (MR::Manager::getInstance().getObjectRefCount(networkDef) == 0)
  {
    // Delete any plug-in assets that have hit zero references.
    // can't handle orphaned asset callbacks here

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
              m_logger,
              m_loggerPriority,
              "  Deleting referenced RigToAnimMap - AssetID: %p.\n",
              MR::Manager::getInstance().getObjectIDFromObjectPtr(rigAnimMap));
            MR::Manager::getInstance().unregisterObject(rigAnimMap);
            NMP::Memory::memFree(rigAnimMap);
          }

          //----------
          // Decrement ref counts for all the event tracks used in this animation set.

          // Discrete event tracks.
          MCOMMS::deleteOrphanDiscreteTracks(networkDef, nodeIndex, animSetIndex, m_logger, m_loggerPriority);

          // Duration events tracks.
          MCOMMS::deleteOrphanDurationTracks(networkDef, nodeIndex, animSetIndex, m_logger, m_loggerPriority);
        }
      }

      //----------
      // Sample character controller.
      MR::CharacterControllerDef* characterControllerDef = networkDef->getCharacterControllerDef(animSetIndex);
      if (characterControllerDef && (MR::Manager::getInstance().getObjectRefCount(characterControllerDef) == 0))
      {
        NM_LOG_MESSAGE(
          m_logger,
          m_loggerPriority,
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
          m_logger,
          m_loggerPriority,
          "  Deleting referenced AnimRigDef - AssetID: %p.\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(rig));
        MR::Manager::getInstance().unregisterObject(rig);
        NMP::Memory::memFree(rig);
      }
    }

    // Network Def.
    NM_LOG_MESSAGE(
      m_logger,
      m_loggerPriority,
      "Finished deletion of NetworkDef - AssetID: %p.\n",
      MR::Manager::getInstance().getObjectIDFromObjectPtr(networkDef));
    MR::Manager::getInstance().unregisterObject(networkDef);
    NMP::Memory::memFree(networkDef);
  }
  else
  {
    NM_LOG_MESSAGE(
      m_logger,
      m_loggerPriority,
      "Not deleting NetworkDef as ref count not 0 - AssetID: %p.\n",
      MR::Manager::getInstance().getObjectIDFromObjectPtr(networkDef));
  }
}


//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::freeNetworkInstance()
{
  if(m_network)
  {
    m_network->getDispatcher()->releaseAndDestroy();
    m_network->releaseAndDestroy();
    // Deallocate any memory from the per character debug interface.
    m_debugInterface.term();
    m_validNetworkInstance = false;
    freeLoadedAnimSources();

    MCOMMS::termFastHeapAllocator(&m_tempAllocator);
    MCOMMS::termMemoryAllocator(&m_persistantAllocator);

    m_network = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::freeLoadedAnimSources()
{
  std::list<MR::AnimSourceBase*>::const_iterator it = m_loadedAnimations.begin();
  std::list<MR::AnimSourceBase*>::const_iterator end = m_loadedAnimations.end();
  while (it != end)
  {
    NMP::Memory::memFree(*it);
    it++;
  }
  m_loadedAnimations.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::setActiveAnimationSet(MR::AnimSetIndex index)
{
  if(!m_network || !m_networkDef)
  {
    return false;
  }

  // in the standard case set 1 is for retargeting, 0 is the source animation set
  if(index < m_networkDef->getNumAnimSets())
  {
    bool result = m_network->setActiveAnimSetIndex(index);
    if(result)
    {
      updateToZero();
    }
    return result;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::createNetworkInstance()
{
  if(m_networkDef)
  {
    // Set the temporary memory size to 1024KB. This should be large enough to cope with most networks for now but to
    // accurately determine the temp memory size used, see FastHeapAllocator::PrintAllocations().
    static const size_t tempDataSize = 1024 * 1024;
    m_tempAllocator = MCOMMS::initFastHeapAllocator(tempDataSize, NMP_VECTOR_ALIGNMENT);
    m_persistantAllocator = MCOMMS::initMemoryAllocator();

    //----------------------
    // Create dispatcher
#if defined(NM_HOST_CELL_PPU) && defined(NM_USE_SPU_DISPATCHER_ON_PS3)
    MR::DispatcherPS3* const dispatcher = MR::DispatcherPS3::createAndInit();
#else
    MR::DispatcherBasic* const dispatcher = MR::DispatcherBasic::createAndInit();
#endif // NM_HOST_CELL_PPU
    if (!dispatcher)
    {
      return false;
    }

    m_network = MR::Network::createAndInit(m_networkDef, dispatcher, m_tempAllocator, m_persistantAllocator, 0);
    if (!m_network)
    {
      return 0;
    }

    m_validNetworkInstance = true; 

    initDebugInterfaces();

    dispatcher->setDebugInterface(&m_debugInterface);

    updateToZero();
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::initDebugInterfaces()
{
  // Initialise debugging interface.
  m_debugInterface.init(ANIM_BROWSER_INSTANCE_ID, m_network, MR::DebugManager::getInstance());
  m_debugInterface.debugOutputsTurnOn(MR::DEBUG_OUTPUT_TREE_NODES);
  for (uint32_t i = 0; i != AnimationBrowserInterface::kNumDataSources; ++i)
  {
    MR::NodeID targetNodeID = m_dataSourceNodeIDs[i];

    if(targetNodeID != MR::INVALID_NODE_ID)
    {
      MR::NodeBin* nodeBin = m_network->getNodeBin(targetNodeID);
      nodeBin->setDebugOutputAllSemantics(true);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::updateNetwork()
{
  NET_LOG_ENTER_FUNC();

  MR::Task* task = NULL;
  MR::ExecuteResult execResult;
  do
  {
    execResult = m_network->update(task);
  } while (execResult != MR::EXECUTE_RESULT_COMPLETE);

  // The allocator is passed to MR::Network::createAndInit
  // and must be reset by the owner (the app). Look at NetworkInstanceRecord::init.
  NMP::TempMemoryAllocator* tempAllocator = m_network->getTempMemoryAllocator();
  tempAllocator->reset();

}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::hasValidNetworkInstance() const
{
  return m_validNetworkInstance; 
}


//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::updateToZero()
{
  if(m_network)
  {
    m_network->startUpdate(0.0f, true, true);
    updateNetwork();
  }
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleAnimBrowserManager::ConnectionData::setAnimationBrowserTime(float time)
{
  if(m_network)
  {
    m_animBrowserTime = 0.0f;
    if(time >= 0.0)
    {
      m_animBrowserTime = time;
    }
    // The first update is required to reset the sample position to zero.  Animation browser time
    // can be set to any value and the absolute location of the output should still be correct. 
    updateToZero();

    // step the network to the provided time to ensure the delta trajectory is correct
    m_network->startUpdate(m_animBrowserTime, false, true);
    updateNetwork();
  }
  else
  {
    m_animBrowserTime = 0.0f;
  }
  return m_animBrowserTime;
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleAnimBrowserManager::ConnectionData::getAnimationBrowserTime() const
{
  return m_animBrowserTime;
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleAnimBrowserManager::ConnectionData::getAnimationBrowserDuration()
{

  if(m_animSourceBase)
  {
    return m_animSourceBase->animGetDuration();
  }
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::loadAnimSourceBase()
{
  MR::NodeID animSourceID = m_dataSourceNodeIDs[SimpleAnimBrowserManager::kAnimationDataSource];
  if(m_animSourceBase == 0 && animSourceID != MR::INVALID_NODE_ID)
  {
    MR::Manager& manager = MR::Manager::getInstance();
    // Check if the network def is partially built
    MR::NodeDef* nodeDef = m_networkDef->getNodeDef(animSourceID);
    if (!nodeDef)
      return;

    //--------------------------
    // Load source animation
    MR::AttribDataHandle* attribHandleAnim = m_networkDef->getAttribDataHandle(
      MR::ATTRIB_SEMANTIC_SOURCE_ANIM,
      animSourceID,
      0);
    if (!attribHandleAnim)
      return;

    MR::AttribDataSourceAnim* sourceAnim = (MR::AttribDataSourceAnim*)attribHandleAnim->m_attribData;
    NMP_ASSERT(sourceAnim);
    m_animSourceBase = manager.requestAnimation(sourceAnim->m_animAssetID);
    NMP_ASSERT_MSG(m_animSourceBase != NULL, "Unable to load animation!");
    if (!m_animSourceBase->isLocated())
    {
      MR::AnimType animType = m_animSourceBase->getType();
      NMP::endianSwap(animType);
      const MR::Manager::AnimationFormatRegistryEntry* animFormatRegistryEntry =
        MR::Manager::getInstance().getInstance().findAnimationFormatRegistryEntry(animType);
      NMP_ASSERT_MSG(animFormatRegistryEntry, "Unable to get AnimationFormatRegistryEntry entry for animation type %d!", animType);
      animFormatRegistryEntry->m_locateAnimFormatFn(m_animSourceBase);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::ConnectionData::findEntryAndSerializeTxFn(
  const MR::NodeID targetNodeID,
  MR::AttribDataSemantic targetSemantic,
  const MR::NodeBinEntry** outEntry,
  MR::AttribSerializeTxFn& serializeFn)
{
  *outEntry = 0; 
  serializeFn = NULL; 
  if(targetNodeID != MR::INVALID_NODE_ID)
  {
    MR::NodeDef* nodeDef = m_networkDef->getNodeDef(targetNodeID);
    MR::NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
    if ((!nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) &&
        !nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE) &&
        m_network->nodeIsActive(targetNodeID))     // Only send back active node data.
      )
    {
      MR::NodeBin* nodeBin = m_network->getNodeBin(targetNodeID);
      const MR::NodeBinEntry* entry = nodeBin->getEntries();
      while (entry)
      {
        MR::AttribDataSemantic semantic = entry->m_address.m_semantic;
        if (semantic == targetSemantic  )
        {
          MR::Manager& manager = MR::Manager::getInstance();
          // Find the required buffer size for serialising this attribute in to.
          MR::AttribData* attribData = entry->m_attribDataHandle.m_attribData;
          serializeFn = manager.getAttribSerializeTxFn(attribData->getType());
          *outEntry = entry; 
          return;
        }
        entry = entry->m_next;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleAnimBrowserManager::ConnectionData::getDataSourceSize(
  AnimationBrowserInterface::AnimationDataSources dataSource)
{
  if(dataSource < AnimationBrowserInterface::kNumDataSources && m_network)
  {
    MR::NodeID targetNodeID = m_dataSourceNodeIDs[dataSource];
    const MR::NodeBinEntry* outEntry;
    MR::AttribSerializeTxFn serializeFn;
    findEntryAndSerializeTxFn(targetNodeID, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, &outEntry, serializeFn);
    if (serializeFn != NULL && outEntry != NULL)
    {
      uint32_t outputSize = serializeFn(m_network,
        outEntry->m_address.m_owningNodeID,
        outEntry->m_attribDataHandle.m_attribData,
        0,
        0);
      return outputSize;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::serializeDataSource(
  AnimationBrowserInterface::AnimationDataSources dataSource,
  MR::AttribTransformBufferOutputData* transformData)
{
  if(dataSource < AnimationBrowserInterface::kNumDataSources && m_network)
  {
    MR::NodeID targetNodeID = m_dataSourceNodeIDs[dataSource];
    const MR::NodeBinEntry* outTransformEntry;
    MR::AttribSerializeTxFn serializeTransformFn;
    findEntryAndSerializeTxFn(targetNodeID, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, &outTransformEntry, serializeTransformFn);
    if (serializeTransformFn != NULL && outTransformEntry != NULL)
    {
      uint32_t dataSourceSize = getDataSourceSize(dataSource); 
      uint32_t outputSize = serializeTransformFn(m_network,
        outTransformEntry->m_address.m_owningNodeID,
        outTransformEntry->m_attribDataHandle.m_attribData,
        transformData,
        dataSourceSize);

      // At this point the transform data is missing the trajectory information in channel 0.

      // First get the trajectory delta from the relevant attrib data.
      const MR::NodeBinEntry* outTrajectoryEntry;
      MR::AttribSerializeTxFn serializeTrajectoryFn;
      findEntryAndSerializeTxFn(targetNodeID, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, &outTrajectoryEntry, serializeTrajectoryFn);
      if(outTrajectoryEntry != NULL && serializeTrajectoryFn != NULL)
      {
        MR::AttribTrajectoryDeltaTransformOutputData trajectoryData;
        serializeTrajectoryFn(m_network,
          outTrajectoryEntry->m_address.m_owningNodeID,
          outTrajectoryEntry->m_attribDataHandle.m_attribData,
          &trajectoryData,
          (uint32_t)sizeof(MR::AttribTrajectoryDeltaTransformOutputData));

        // Now add the trajectory information to the transform buffer.

        // Both the trajectory and transform buffer data are endian swapped for network transmission so take this
        // in to account before combining the data.

        // Find the start of the transforms array.
        uint32_t transformsPtrOffset = transformData->m_transformsPtrOffset;
        NMP::netEndianSwap(transformsPtrOffset);
        MR::AttribTransformBufferOutputData::Transform* transformDataTransforms =
          ((MR::AttribTransformBufferOutputData::Transform *)(((char*)transformData) + transformsPtrOffset));

        // Copy the trajectory information in to the transform buffer,
        // they're both endian swapped so no point swapping back just to copy.
        transformDataTransforms[0].m_pos = trajectoryData.m_deltaPos;
        transformDataTransforms[0].m_quat = trajectoryData.m_deltaAtt;
      }

      return outputSize == dataSourceSize;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::broadcastAnimationBrowserMessage(const MR::Message& message)
{
  if(m_network)
  {
    m_network->broadcastMessage(message);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::sendAnimationBrowserMessage(commsNodeID nodeID, const MR::Message& message)
{
  if(m_network)
  {
    m_network->sendMessage(nodeID, message);
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::ConnectionData::setAnimationBrowserControlParameter(
  commsNodeID            nodeID,
  MR::NodeOutputDataType type,
  const void*            cparamData)
{
  if(m_network)
  {
    return setNetworkControlParameter(m_network, nodeID, type, cparamData);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// SimpleAnimBrowserManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimBrowserManager::SimpleAnimBrowserManager(
  uint32_t               maxSimulataneousConncetions,
  NMP::PrioritiesLogger& logger,
  int32_t                loggerPriority,
  IPluginValidator*     validatePluginList) :
  AnimationBrowserInterface(),
  m_maxConnections(NMP::maximum((uint32_t)1, maxSimulataneousConncetions)),
  m_numConnections(0),
  m_logger(logger),
  m_loggerPriority(loggerPriority),
  m_validatePluginList(validatePluginList)
{
  const size_t connectionDataSize = m_maxConnections * sizeof(ConnectionData*);
  m_connectionData = (ConnectionData**)NMPMemoryAlloc(connectionDataSize);
  NMP_ASSERT(m_connectionData);
  memset(m_connectionData, 0, connectionDataSize);
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimBrowserManager::~SimpleAnimBrowserManager()
{
  for (uint32_t i = 0; i < m_maxConnections; ++i)
  {
    if (m_connectionData[i])
    {
      ConnectionData* const connectionData = m_connectionData[i];

      connectionData->~ConnectionData();
      NMP::Memory::memFree(connectionData);

      --m_numConnections;
    }
  }
  NMP_ASSERT(m_numConnections == 0);
  NMP::Memory::memFree(m_connectionData);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::setAnimationBrowserAnim(const char* animBrowserAnim, Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData && !addConnection(connectionData, connection))
  {
    return false;
  }
  else
  {
    return connectionData->setAnimBrowserFileServerPath(animBrowserAnim);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::setAnimationBrowserNetwork(
  const char* animBrowserNetwork,
  NetworkDescriptor animBrowserNetworkDesc,
  Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData && !addConnection(connectionData, connection))
  {
    return false;
  }

  return connectionData->setAnimationBrowserNetwork(animBrowserNetwork, animBrowserNetworkDesc);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::loadAnimationBrowserData(const char* compressionTypeString, Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return false;
  }

  connectionData->freeNetworkDef(); 

  char temporaryAnimBuffer[256];
  char temporaryNetBuffer[256];
  // Determine the animation browser file extension.
  NMP_STRNCPY_S(temporaryAnimBuffer, sizeof(temporaryAnimBuffer), connectionData->getAnimBrowserFileServerPath());
  NMP_STRNCAT_S(temporaryAnimBuffer, sizeof(temporaryAnimBuffer), ANIM_BROWSER_ANIM_FILE_NAME);
  NMP_STRNCAT_S(temporaryAnimBuffer, sizeof(temporaryAnimBuffer), ".");
  NMP_STRNCAT_S(temporaryAnimBuffer, sizeof(temporaryAnimBuffer), compressionTypeString);

  // Determine animation/bundle sizes.
  const uint32_t animSize = connection->getFileSize(temporaryAnimBuffer);
  if (animSize == 0)
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Valid animation (name=%s, size=%u) expected!\n",
      connectionData->getAnimBrowserFileServerPath(), animSize);

    return false;
  }

  NMP_STRNCPY_S(temporaryNetBuffer, sizeof(temporaryNetBuffer), connectionData->getAnimBrowserFileServerPath());
  NMP_STRNCAT_S(temporaryNetBuffer, sizeof(temporaryNetBuffer), ANIM_BROWSER_BUNDLE_FILE_NAME);
  const uint32_t networkSize = connection->getFileSize(temporaryNetBuffer);
  if (networkSize == 0)
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Valid network (name=%s, size=%u) expected!\n",
      connectionData->getAnimBrowserNetwork(), networkSize);
  }

  // Allocate temporary animation/bundle buffers (scoped memory frees memory as it goes out of scope for convenience).
  const ScopedMemory animBuffer(NMPMemoryAllocAligned(animSize, NMP_VECTOR_ALIGNMENT));
  if ((animBuffer == NULL))
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Memory allocation failed for animation (size=%u)!\n",
      animSize);

    return false;
  }

  const ScopedMemory networkBuffer(NMPMemoryAllocAligned(networkSize, NMP_VECTOR_ALIGNMENT));
  if ((networkBuffer == NULL))
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Memory allocation failed for network (size=%u)!\n",
      networkSize);

    return false;
  }

  // Get animation over connection.
  if (!connection->getFile(animBuffer, animSize, temporaryAnimBuffer))
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Failed to load animation over connection (name=%s)!\n",
      connectionData->getAnimBrowserFileServerPath());

    return false;
  }

  // Get network over connection.
  if (!connection->getFile(networkBuffer, networkSize, temporaryNetBuffer))
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Failed to load network over connection (name=%s)!\n",
      connectionData->getAnimBrowserNetwork());

    return false;
  }

  if (connectionData->getNetworkDef())
  {
    connectionData->freeNetworkDef();
  }

  MCOMMS::GUID networkGuid;
  MR::NetworkPredictionDef* netPredictionDef;
  MR::NetworkDef* networkDef = readNetworkFromSimpleBundle(networkBuffer, networkSize, networkGuid, m_validatePluginList, netPredictionDef);
  if (networkDef == 0)
  {
    NM_LOG_ERROR_MESSAGE(
      m_logger,
      "SimpleAnimBrowserManager: Error: Failed to read simple bundle for network (name=%s)!\n",
      connectionData->getAnimBrowserNetwork());

    return false;
  }

  if(connectionData->setNetworkDef(networkDef, networkGuid))
  {
    // The animation browser works by briefly redirecting the loading functions to a simple global that
    // stores the last loaded animation.  This technique is not thread safe and will need more complex handling
    // if  animation loading is done on multiple threads. 
    MR::Manager::RequestAnimFn* existingRequestFn = MR::Manager::getInstance().getRequestAnimFn();
    MR::Manager::ReleaseAnimFn* existingReleaseFn = MR::Manager::getInstance().getReleaseAnimFn();
    MR::Manager::getInstance().setAnimFileHandlingFunctions(animBrowserAnimFileLoader, animBrowserAnimFileUnloader);
   
    MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup *netDefSpecificAnimIDtoFilename =
      MCOMMS::readAnimIDToFilenameFromSimpleBundle(networkBuffer, networkSize);

    if(netDefSpecificAnimIDtoFilename)
    {
      MR::AnimSourceBase* lastLoaded = 0;
      AnimBrowserAnimLoadingInfo info;
      info.m_lastLoadedAnim = &lastLoaded;
      info.m_loadedAnims = connectionData->getLoadedAnimations();
      info.m_currentConnection = connection;
      info.m_currentNetDefSpecificAnimIDToFilename = netDefSpecificAnimIDtoFilename;
      info.m_currentNetworkName = temporaryNetBuffer;

      for (MR::AnimSetIndex i = 0; i < networkDef->getNumAnimSets(); ++i)
      {
        networkDef->loadAnimations(i, &info);
      }

      // Clean up the SimpleAnimRuntimeIDtoFilenameLookup allocated by MCOMMS::readAnimIDToFilenameFromSimpleBundle()
      NMP::Memory::memFree(info.m_currentNetDefSpecificAnimIDToFilename);

      connectionData->setAnimSourceBase(lastLoaded);
    }

    bool createdNetworkInstance = false; 
    if(connectionData->createNetworkInstance())
    {
      NM_LOG_MESSAGE(
        m_logger, m_loggerPriority,
        "SimpleAnimBrowserManager: network %s loaded.\n",
        connectionData->getAnimBrowserNetwork());
      createdNetworkInstance = true; 
    }
    
    MR::Manager::getInstance().setAnimFileHandlingFunctions(existingRequestFn, existingReleaseFn);
    if(!createdNetworkInstance)
    {
      NM_LOG_MESSAGE(
        m_logger, m_loggerPriority,
        "SimpleAnimBrowserManager:Failed to create network instance for network (name=%s)\n",
        connectionData->getAnimBrowserNetwork());
      return false;
    }

  }
  else
  {
    NM_LOG_MESSAGE(
      m_logger, m_loggerPriority,
      "SimpleAnimBrowserManager:Error: Failed to load network def for network (name=%s)\n",
      connectionData->getAnimBrowserNetwork());
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleAnimBrowserManager::setAnimationBrowserTime(float time, Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return 0.0f;
  }
  else
  {
    return connectionData->setAnimationBrowserTime(time);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleAnimBrowserManager::getAnimationBrowserTime(Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);

  if (!connectionData)
  {
    return 0.0f;
  }
  else
  {
    return connectionData->getAnimationBrowserTime();
  }
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleAnimBrowserManager::getAnimationBrowserDuration(Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return 0.0f;
  }
  else
  {
    return connectionData->getAnimationBrowserDuration();
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleAnimBrowserManager::getAnimationBrowserTransformDataSourceSize(
  AnimationDataSources dataSource,
  Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return 0;
  }
  else
  {
    return connectionData->getDataSourceSize(dataSource);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::serializeAnimationBrowserDataSource(
  AnimationDataSources dataSource,
  MR::AttribTransformBufferOutputData* data,
  Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return false;
  }
  else
  {
    return connectionData->serializeDataSource(dataSource, data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::broadcastAnimationBrowserMessage(const MR::Message& message, Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return false;
  }
  else
  {
    return connectionData->broadcastAnimationBrowserMessage(message);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::sendAnimationBrowserMessage(commsNodeID nodeID, const MR::Message& message,Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return false;
  }
  else
  {
    return connectionData->sendAnimationBrowserMessage(nodeID, message);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::setAnimationBrowserActiveAnimationSet(MR::AnimSetIndex setIndex, Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return false;
  }
  else
  {
    return connectionData->setActiveAnimationSet(setIndex);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::setAnimationBrowserControlParameter(
  commsNodeID            nodeID,
  MR::NodeOutputDataType type,
  const void*            cparamData,
  Connection* connection)
{
  ConnectionData* connectionData = findConnectionData(connection);
  if (!connectionData)
  {
    return false;
  }
  else
  {
    return connectionData->setAnimationBrowserControlParameter(nodeID, type, cparamData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::onConnectionClosed(Connection* connection)
{
  removeConnection(connection);
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimBrowserManager::ConnectionData* SimpleAnimBrowserManager::findConnectionData(Connection* connection)
{
  for (uint32_t i = 0; i < m_maxConnections; ++i)
  {
    if (m_connectionData[i] && (m_connectionData[i]->getConnection() == connection))
    {
      return m_connectionData[i];
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimBrowserManager::addConnection(ConnectionData *& connectionData, Connection* connection)
{
  for (uint32_t i = 0; i < m_maxConnections; ++i)
  {
    if (!m_connectionData[i])
    {
      void* connectionDataMemPtr = NMPMemoryAlloc(sizeof(ConnectionData));
      NMP_ASSERT(connectionDataMemPtr);
      connectionData = new(connectionDataMemPtr) ConnectionData(connection, m_logger, m_loggerPriority);

      m_connectionData[i] = connectionData;
      ++m_numConnections;

      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimBrowserManager::removeConnection(Connection* connection)
{
  for (uint32_t i = 0; i < m_maxConnections; ++i)
  {
    if (m_connectionData[i] && m_connectionData[i]->getConnection() == connection)
    {
      ConnectionData* const connectionData = m_connectionData[i];

      connectionData->~ConnectionData();
      NMP::Memory::memFree(connectionData);

      m_connectionData[i] = NULL;
      --m_numConnections;
    }
  }
}



#endif // defined(MR_ATTRIB_DEBUG_BUFFERING)

} // namespace COMMS
