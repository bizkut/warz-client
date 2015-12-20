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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_LL_DEFAULTASSETMANAGER_H
#define NM_LL_DEFAULTASSETMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "morpheme/mrNetworkDef.h"
#include "simpleBundle/simpleAnimRegistry.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include "comms/runtimeTargetInterface.h"
#include "comms/simpleBundleUtils.h"
//----------------------------------------------------------------------------------------------------------------------

class IPhysicsMgr;
class NetworkDefRecordManager;
class NetworkDefRecord;
class NetworkInstanceRecordManager;
class RuntimeTargetContext;

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultAssetMgr
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultAssetMgr :
  public MCOMMS::NetworkDefLifecycleInterface,
  public MCOMMS::NetworkLifecycleInterface
{
public:

  DefaultAssetMgr(RuntimeTargetContext* const context, MCOMMS::IPluginValidator* validatePluginList);
  virtual ~DefaultAssetMgr();

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultAssetMgr & operator= (const DefaultAssetMgr & other) {*this = other; return *this;}

  void setPhysicsManager(IPhysicsMgr* const physicsMgr)
  {
    NMP_ASSERT(!m_physicsMgr || !physicsMgr);
    m_physicsMgr = physicsMgr;
  }

  // [NetworkDefLifecycleInterface]
public:

  virtual bool canLoadNetworkDefinition() const NM_OVERRIDE;

  virtual bool loadNetworkDefinition(
    const MCOMMS::GUID& guid,
    const char*         networkDefName,
    MCOMMS::Connection* connection) NM_OVERRIDE;

  virtual bool destroyNetworkDefinition(const MCOMMS::GUID& guid) NM_OVERRIDE;
  virtual bool clearCachedData() NM_OVERRIDE;

  // [NetworkLifecycleInterface]
public:

  virtual bool canCreateNetworkInstance() const NM_OVERRIDE;

  virtual MCOMMS::InstanceID createNetworkInstance(
    const MCOMMS::GUID& guid,
    const char*         instanceName,
    uint32_t            startingAnimSetIndex,
    MCOMMS::Connection* connection,
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation) NM_OVERRIDE;

  virtual bool destroyNetworkInstance(MCOMMS::InstanceID instance) NM_OVERRIDE;
  virtual void onConnectionClosed(MCOMMS::Connection* connection) NM_OVERRIDE;

  /// \brief Typedef for reference count callback
  typedef void (AssetHandlerCB)(MR::NetworkDef* network);
  /// \brief Call any registered callbacks to increment reference counts on any plug-in assets.
  void invokeRefCountIncCallbacks(MR::NetworkDef* networkDef);
  /// \brief Call any registered callbacks to decrement reference counts on any plug-in assets.
  void invokeRefCountDecCallbacks(MR::NetworkDef* networkDef);
  /// \brief Call any registered callbacks to delete any plug-in assets that have hit 0 references.
  void invokeDeleteOrphanedAssetCallbacks(MR::NetworkDef* networkDef);
  /// \brief Register a new callback to be invoked when a network def has it's ref count increased
  void registerRefCountIncCallback(AssetHandlerCB* incRefCountCB);
  /// \brief Register a new callback to be invoked when a network def has it's ref count decreased
  void registerRefCountDecCallback(AssetHandlerCB* decRefCountCB);
  /// \brief Register a new callback to be invoked when a network def is deleted.
  void registerDeleteOrphanedAssetCallback(AssetHandlerCB* deleteAssetCB);

protected:

  bool destroyNetworkDefRecord(NetworkDefRecord* recordToDelete);

  /// Add this network to this list of networks that are no longer referenced but should be kept in memory to save on export time from connect.
  /// If more than an internal limit of definitions are stored this will return the oldest networkDefRecord that should be freed.
  NetworkDefRecord* addToUnreferencedNetworkDefinitionList(NetworkDefRecord* record);
  /// Remove this network from the list of networks that are no longer referenced but should be kept in memory to save on export time from connect.
  bool removeFromUnreferencedNetworkDefinitionList(NetworkDefRecord* record);

  typedef struct
  {
    MR::UTILS::SimpleAnimRegistry* m_currentAnimRegistry;
    MCOMMS::Connection*            m_currentConnection;
    const char*                    m_currentNetworkName;
    MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* m_currentNetDefSpecificAnimIDToFilename;
  } AnimLoadingInfo;

  static MR::AnimSourceBase* networkAnimLoader(
    const MR::RuntimeAnimAssetID animAssetID,
    void* userdata);

  static void networkAnimUnLoader(
    const MR::RuntimeAnimAssetID animAssetID,
    MR::AnimSourceBase*          loadedAnimation,
    void*                        userdata);

  virtual bool loadNetworkDefinitionFromBuffer(
    void*                  filebuffer,
    size_t                 fileSize,
    NetworkDefRecord*      networkDefRecord,
    MCOMMS::GUID&          guid,
    const char*            networkDefName);

  /// \brief Increase the ref count on a NetworkDef and all its referenced object/assets.
  ///
  /// Reference counting of NetworkDefs is based on the number of Networks that have been instanced from a particular Def.
  /// On instancing a Network we increment the reference count of the NetworkDef and any asset Defs that are referenced
  /// by this NetworkDef.
  /// Note: NetworkDef and all sub assets must already have been registered with the manager.
  void incNetworkDefRefCounts(MR::NetworkDef* networkDef);

  /// \brief Decrease the ref count on a NetworkDef and all its referenced object/assets.
  void decNetworkDefRefCounts(MR::NetworkDef* networkDef);

  void deleteOrphanedNetworkDefAssets(MR::NetworkDef* networkDef);

  void deleteOrphanDiscreteTracks(
    MR::NetworkDef*      networkDef,
    MR::NodeID           nodeIndex,
    uint16_t             animSetIndex);

  void deleteOrphanCurveTracks(
    MR::NetworkDef*      networkDef,
    MR::NodeID           nodeIndex,
    uint16_t             animSetIndex);

  void deleteOrphanDurationTracks(
    MR::NetworkDef*      networkDef,
    MR::NodeID           nodeIndex,
    uint16_t             animSetIndex);

  MCOMMS::InstanceID getUnusedInstanceID();

  MR::UTILS::SimpleAnimRegistry m_animRegistry;

  typedef std::vector<NetworkDefRecord*> NetworkDefList;

  // Network Defs that have been requested for deletion, but are being stored for quick caching
  NetworkDefList m_unreferencedDefList;

  // These plugin callbacks allow the asset manager to handle asset types it know nothing about
  static const uint32_t MAX_PLUGINS = 8;

  uint32_t        m_numRefCountIncCallbacks;
  AssetHandlerCB* m_refCountIncCallbacks[MAX_PLUGINS];
  uint32_t        m_numRefCountDecCallbacks;
  AssetHandlerCB* m_refCountDecCallbacks[MAX_PLUGINS];
  uint32_t        m_numDeleteOrphanedAssetCallbacks;
  AssetHandlerCB* m_deleteOrphanedAssetCallbacks[MAX_PLUGINS];

  NetworkDefRecordManager* const      m_networkDefManager;
  NetworkInstanceRecordManager* const m_networkInstanceManager;
  IPhysicsMgr*                        m_physicsMgr;

  MCOMMS::Connection* m_connection;
  MCOMMS::IPluginValidator* m_pluginValidator;

  void resetMapCache() {}
}; // class DefaultAssetMgr

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_LL_DEFAULTASSETMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
