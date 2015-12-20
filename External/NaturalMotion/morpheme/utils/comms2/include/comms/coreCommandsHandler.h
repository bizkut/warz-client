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
#ifndef MCOMMS_CORECOMMANDSHANDLER_H
#define MCOMMS_CORECOMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/commandsHandler.h"
#include <map>

namespace MCOMMS
{
class Connection;
class Attribute;
class CoreCommsServerModule;

//----------------------------------------------------------------------------------------------------------------------
class ObjectSetupHandler
{
public:
  enum
  {
    kSceneObject,
    numObjectTypes
  };

  ObjectSetupHandler(uint32_t objType, uint32_t numAttributes);
  ~ObjectSetupHandler();

  /// ObjectSetupHandler will take ownership of the attribute and delete it and its data on destruction.
  void addAttribute(Attribute* attribute);

  Attribute** m_attributes;
  uint32_t    m_numAttributes;
  uint32_t    m_totalNumAttributes;
  uint32_t    m_objType;
};

//----------------------------------------------------------------------------------------------------------------------
///
class CoreCommandsHandler : public CommandsHandler
{
public:
  CoreCommandsHandler(RuntimeTargetInterface* target, CoreCommsServerModule* coreModule);
  virtual ~CoreCommandsHandler();

  virtual void clear();
  virtual void onConnectionClosed(Connection* connection);
private:
  virtual bool doHandleCommand(CmdPacketBase* cmdPacket);

private:
  void handleIdentificationCmd(CmdPacketBase* cmdPacket);
  void handleSetControlParamCmd(CmdPacketBase* cmdPacket);
  void handleSetRootTransformCmd(CmdPacketBase* cmdPacket);
  void handleSetAnimationSetCmd(CmdPacketBase* cmdPacket);
  void handleSetAnimBrowserAnimationSetCmd(CmdPacketBase *cmdPacket);
  void handleSendRequestCmd(CmdPacketBase* cmdPacket);
  void handleBroadcastRequestCmd(CmdPacketBase* cmdPacket);
  void handleMessageCmd(CmdPacketBase* cmdPacket);
  void handleSetCurrentStateCmd(CmdPacketBase* cmdPacket);
  void handleExecuteCommandCmd(CmdPacketBase* cmdPacket);
  void handleEnableOutputDataCmd(CmdPacketBase* cmdPacket);
  void handleSetDebugOutputFlagsCmd(CmdPacketBase* cmdPacket);
  void handleSetDebugOutputOnNodesCmd(CmdPacketBase* cmdPacket);
  void handleLoadNetworkCmd(CmdPacketBase* cmdPacket);
  void handleDownloadSceneObjectsCmd(CmdPacketBase* cmdPacket);
  void handleDownloadFrameDataCmd(CmdPacketBase* cmdPacket);
  void handleDownloadGlobalDataCmd(CmdPacketBase* cmdPacket);
  void handleDownloadNetworkDefinitionCmd(CmdPacketBase* cmdPacket);
  void handleCreateNetworkInstanceCmd(CmdPacketBase* cmdPacket);
  void handleReferenceNetworkInstanceCmd(CmdPacketBase* cmdPacket);
  void handleUnreferenceNetworkInstanceCmd(CmdPacketBase* cmdPacket);
  void handleDestroyNetworkDefinitionCmd(CmdPacketBase* cmdPacket);
  void handleDestroyNetworkInstanceCmd(CmdPacketBase* cmdPacket);
  void handleStepCmd(CmdPacketBase* cmdPacket);
  void handleStepModeCmd(CmdPacketBase* cmdPacket);
  void handleSetChannelsCmd(CmdPacketBase* cmdPacket);
  void handleStartSessionCmd(CmdPacketBase* cmdPacket);
  void handleStopSessionCmd(CmdPacketBase* cmdPacket);
  void handlePauseSessionCmd(CmdPacketBase* cmdPacket);

  void handleCreateConstraintCmd(CmdPacketBase* cmdPacket);
  void handleMoveConstraintCmd(CmdPacketBase* cmdPacket);
  void handleRemoveConstraintCmd(CmdPacketBase* cmdPacket);
  void handleApplyForceCmd(CmdPacketBase* cmdPacket);

  void handleBeginSceneObjectCmd(CmdPacketBase* cmdPacket);
  void handleEndSceneObjectCmd(CmdPacketBase* cmdPacket);
  void handleSceneObjectAttributeCmd(CmdPacketBase* cmdPacket);
  void handleSetAttributeCmd(CmdPacketBase* cmdPacket);
  void handleSetEnvironmentAttributeCmd(CmdPacketBase* cmdPacket);
  void handleDestroySceneObjectCmd(CmdPacketBase* cmdPacket);

  void handleClearCachedDataCmd(CmdPacketBase* cmdPacket);

private:
  // The process to create a scene object is not immediate.
  // A scene object is described by a sequence of packets:
  // - BeginSceneObject : type, number of attributes
  // - An AttributePacket for each attribute of the object.
  // - EndSceneObject.
  // When the initial BeginSceneObject packet is received, a "pending" scene object creation is initialized.
  // Subsequent attribute packets are collected, until the EndSceneObject packet is received.
  // An additional complication here is given by the fact that the command handler is shared between connections,
  // so scene object creations from different connections could interleave. In order to avoid that, the only way
  // is to keep an open creation handler per connection.
  typedef std::map<const Connection*, ObjectSetupHandler*> ObjectSetupHandlers;

  ObjectSetupHandlers    m_objectsToSetup;
  CoreCommsServerModule* m_coreModule;
};

} //namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_CORECOMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------
