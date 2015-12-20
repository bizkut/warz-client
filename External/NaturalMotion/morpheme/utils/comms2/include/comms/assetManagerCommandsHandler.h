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
#ifndef MCOMMS_ASSETMANAGERCOMMANDSHANDLER_H
#define MCOMMS_ASSETMANAGERCOMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
#include "comms/commandsHandler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
class Connection;
class CoreCommsServerModule;

//----------------------------------------------------------------------------------------------------------------------
///
class AssetManagerCommandsHandler : public CommandsHandler
{
public:
  AssetManagerCommandsHandler(RuntimeTargetInterface* target, CoreCommsServerModule* coreModule);
  virtual ~AssetManagerCommandsHandler();

  virtual void clear() NM_OVERRIDE;
  virtual void onConnectionClosed(Connection* connection) NM_OVERRIDE;

private:
  CoreCommsServerModule* m_coreModule;

  virtual bool doHandleCommand(CmdPacketBase* cmdPacket) NM_OVERRIDE;

  void handleSetAnimBrowserAnimCmd(CmdPacketBase* cmdPacket);
  void handleSetAnimBrowserNetworkCmd(CmdPacketBase* cmdPacket);
  void handleLoadAnimBrowserDataCmd(CmdPacketBase* cmdPacket);
  void handleSetAnimBrowserTimeCmd(CmdPacketBase* cmdPacket);

  void handleDownloadAnimBrowserEventDetectionDataCmd(CmdPacketBase* cmdPacket);
};

} //namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_ASSETMANAGERCOMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------
