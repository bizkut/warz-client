// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "defaultERAssetMgr.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/mcomms.h"
#include "comms/connection.h"
#include "comms/simpleBundleUtils.h"
#include "erLiveLinkData.h"
#include "defaultControllerMgr.h"
#include "physics/mrPhysicsRigDef.h"
#include "euphoria/erAttribData.h"
#include "physics/mrPhysics.h"
#include "iPhysicsMgr.h"

#include "runtimeTargetLogger.h"
//----------------------------------------------------------------------------------------------------------------------
// Local declarations
void destroyEuphoriaNetworkDefInstance(MR::NetworkDef* networkDef);
void euphoriaRefCountInc(MR::NetworkDef* networkDef);
void euphoriaRefCountDec(MR::NetworkDef* networkDef);

//----------------------------------------------------------------------------------------------------------------------
// Behaviour defs supported by our library.  This is a temporary home until the behaviour library can expose the
//  supported behaviours automatically.
//----------------------------------------------------------------------------------------------------------------------
DefaultERAssetMgr::DefaultERAssetMgr(RuntimeTargetContext* const context, MCOMMS::IPluginValidator* validatePluginList) :
  DefaultAssetMgr(context, validatePluginList)
{
  registerDeleteOrphanedAssetCallback(&destroyEuphoriaNetworkDefInstance);
  registerRefCountIncCallback(&euphoriaRefCountInc);
  registerRefCountDecCallback(&euphoriaRefCountDec);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultERAssetMgr::loadNetworkDefinitionFromBuffer(
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
    // TODO: These statics will be removed in a later revision and replaced with a userdata to the loadAnimations() callback
    AnimLoadingInfo info;
    info.m_currentConnection = m_connection;
    info.m_currentAnimRegistry = &m_animRegistry;
    info.m_currentNetDefSpecificAnimIDToFilename = netDefSpecificAnimIDtoFilename;
    info.m_currentNetworkName = networkDefName;
    MR::Manager::getInstance().setAnimFileHandlingFunctions(networkAnimLoader, networkAnimUnLoader);
    for (MR::AnimSetIndex i = 0; i < networkDef->getNumAnimSets(); i++)
    {
      // Fixup the animations and rigToAnimMaps.
      NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "  ------------------\n");
      NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "  Loading anims for set %i\n", i);
      networkDef->loadAnimations(i, (void*)&info);
    }
  }

  NMP::Memory::memFree(netDefSpecificAnimIDtoFilename);

  //--------------------------
  // If all assets including NetworkDef have been loaded, located and registered with the Manager;
  // Update the LiveLink reference count for all assets.
  // The count of each asset is incremented for each time that it is referenced within the NetworkDef.
  networkDefRecord = new ERNetworkDefRecord(guid, networkDefName, networkDef, netPredictionDef);
  m_networkDefManager->addNetworkDefRecord(networkDefRecord);

  return networkDef != 0 && netDefSpecificAnimIDtoFilename != 0;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::InstanceID DefaultERAssetMgr::createNetworkInstance(
  const MCOMMS::GUID&                guid,
  const char*                        instanceName,
  uint32_t                           animSetIndex,
  MCOMMS::Connection*                connection,
  const NMP::Vector3&                characterStartPosition,
  const NMP::Quat&                   characterStartRotation)
{
  if (connection->isMaster() && m_physicsMgr)
  {
    m_physicsMgr->initializeScene();
  }

  // The NetDef should already exist
  NetworkDefRecord* defRecord = m_networkDefManager->findNetworkDefRecord(guid);
  if (defRecord == 0)
  {
    char guidBuffer[64];
    guid.toStringGUID(guidBuffer);
    return MCOMMS::INVALID_INSTANCE_ID;
  }
  NMP_ASSERT(guid == defRecord->getGUID());

  // Create instance records and add to list
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ERNetworkInstanceRecord), NMP_VECTOR_ALIGNMENT);
  NetworkInstanceRecord* instanceRecord = new(alignedMemory) ERNetworkInstanceRecord(
    getUnusedInstanceID(),
    defRecord,
    connection,
    instanceName);

  instanceRecord->init(
    animSetIndex,
    characterStartPosition,
    characterStartRotation); // This calls network update and so depends on the character controller existing.

  if (!instanceRecord->getNetwork())
  {
    NMP::Memory::memFree(instanceRecord);
    return MCOMMS::INVALID_INSTANCE_ID;
  }

  // Increase the reference count on the NetworkDef (and all sub-assets) we are using.
  incNetworkDefRefCounts(defRecord->getNetDef());

  // Add to the map
  m_networkInstanceManager->addNetworkInstanceRecord(instanceRecord);
  return instanceRecord->getInstanceID();
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultERAssetMgr::destroyNetworkInstance(MCOMMS::InstanceID id)
{
  NetworkInstanceRecord* const instanceRecord = m_networkInstanceManager->findInstanceRecord(id);
  ERNetworkInstanceRecord* erInstanceRecord = (ERNetworkInstanceRecord*)instanceRecord;
  NMP_ASSERT(erInstanceRecord);
  erInstanceRecord->term(); // Destroy the euphoria data.

  return DefaultAssetMgr::destroyNetworkInstance(id);
}

//----------------------------------------------------------------------------------------------------------------------
void destroyEuphoriaNetworkDefInstance(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (uint16_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
  {
    // Destroy the body def.  Not all networks will have body defs.
    MR::AttribDataHandle* handle = networkDef->getOptionalAttribDataHandle(
      ER::ATTRIB_SEMANTIC_BODY_DEF, MR::NETWORK_NODE_ID, animSetIndex);
    if (handle)
    {
      ER::AttribDataBodyDef* bodyDefDefAttrib = (ER::AttribDataBodyDef*)handle->m_attribData;
      if(bodyDefDefAttrib)
      {
        ER::BodyDef* bodyDef = bodyDefDefAttrib->m_bodyDef;
        if (bodyDef && (MR::Manager::getInstance().getObjectRefCount(bodyDef) == 0))
        {
          NM_LOG_MESSAGE(
            RTT_LOGGER,
            RTT_MESSAGE_PRIORITY,
            "  Deleting referenced BodyDef - AssetID: %p.\n",
            MR::Manager::getInstance().getObjectIDFromObjectPtr(bodyDef));
          MR::Manager::getInstance().unregisterObject(bodyDef);
          NMP::Memory::memFree(bodyDef);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void euphoriaRefCountInc(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (uint16_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
  {
    // Increase the ref count on body defs
    MR::AttribDataHandle* handle = networkDef->getOptionalAttribDataHandle(
      ER::ATTRIB_SEMANTIC_BODY_DEF, MR::NETWORK_NODE_ID, animSetIndex);
    if (handle)
    {
      ER::AttribDataBodyDef* bodyDefDefAttrib = (ER::AttribDataBodyDef*)handle->m_attribData;
      if(bodyDefDefAttrib)
      {
        ER::BodyDef* bodyDef = bodyDefDefAttrib->m_bodyDef;
        if (bodyDef)
        {
          MR::Manager::getInstance().incObjectRefCount(bodyDef);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void euphoriaRefCountDec(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (uint16_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
  {
    // Decrease the ref count on body defs
    MR::AttribDataHandle* handle = networkDef->getOptionalAttribDataHandle(
      ER::ATTRIB_SEMANTIC_BODY_DEF, MR::NETWORK_NODE_ID, animSetIndex);
    if (handle)
    {
      ER::AttribDataBodyDef* bodyDefDefAttrib = (ER::AttribDataBodyDef*)handle->m_attribData;
      if(bodyDefDefAttrib)
      {
        ER::BodyDef* bodyDef = bodyDefDefAttrib->m_bodyDef;
        if (bodyDef)
        {
          MR::Manager::getInstance().decObjectRefCount(bodyDef);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
