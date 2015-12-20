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
#ifndef MCOMMS_CORECOMMSSERVER_MODULE_H
#define MCOMMS_CORECOMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/commsServerModule.h"
#include "comms/sceneObject.h"

namespace MCOMMS
{

class Connection;
struct CmdPacketBase;
class AssetManagerCommandsHandler;
class CoreCommandsHandler;

class CoreCommsServerModule : public CommsServerModule
{
public:
  friend class CoreCommandsHandler;

  CoreCommsServerModule();
  virtual ~CoreCommsServerModule();

  virtual void sendDefData(const GUID& networkGUID, Connection* connection);
  virtual void sendInstanceFrameData(InstanceID instanceID, Connection* connection);
  virtual void sendFrameData(Connection* connection);
  virtual void postFrameUpdate();
  virtual void preFrameUpdate();
  virtual void onStartSession(Connection* connection);
  virtual void onStopSession(Connection* connection);

  virtual void onConnectionClosed(Connection* connection);

protected:
  CoreCommandsHandler*  m_coreCmdHandler; /// < Always have the core cmd handler.
  AssetManagerCommandsHandler* m_assetManagerCmdHandler; /// < Always have the asset manager cmd handler.

  void sendSceneObjectUpdate(Connection* connection, const SceneObject* sceneObject);
  void sendSceneObjectsUpdate(Connection* connection);

  void broadcastSceneObjectDescription(const SceneObject* sceneObject);
  void describeSceneObject(const SceneObject* sceneObject, Connection* connection);
  void describeSceneObjects(Connection* connection);

  void broadcastSceneObjectDestruction(SceneObjectID objectID);
  void sendSceneObjectDestruction(SceneObjectID objectID, Connection* connection);

public:
  void synchronizeSceneObjects();
  void initializeSceneObjectsSyncCache();

protected:
  uint32_t m_sizeOfSynchronizedObjectsCache;
  uint32_t m_numSceneObjectsAtLastSynch;
  SceneObjectID* m_sceneObjectsAtLastSynch;
  bool findSceneObject(const SceneObjectID* sceneObjectIDs, uint32_t numSceneObjects, SceneObjectID objectID) const;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_CORECOMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
