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
#ifndef MCOMMS_COMMSSERVER_MODULE_H
#define MCOMMS_COMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/mcomms.h"

namespace MCOMMS
{

class Connection;
struct CmdPacketBase;
class CommandsHandler;

/// When sending the frame data, the comms server asks each module to send its specific data.
/// A module can send data about a character instance (started and ended by the server), plus
/// some other specific data (ex: the core has debug draw data and scene object data, another module
/// might have data for the AI system).
class CommsServerModule
{
public:
  CommsServerModule();
  virtual ~CommsServerModule();

  virtual void sendDefData(const GUID& NMP_UNUSED(networkGUID), Connection* NMP_UNUSED(connection)) { }
  virtual void sendInstanceStaticData(const GUID& NMP_UNUSED(networkGUID), InstanceID NMP_UNUSED(instanceID), Connection* NMP_UNUSED(connection)) { }
  virtual void sendInstanceFrameData(InstanceID NMP_UNUSED(instanceID), Connection* NMP_UNUSED(connection)) { }
  virtual void sendFrameData(Connection* NMP_UNUSED(connection)) {}
  virtual bool handleCommand(CmdPacketBase* cmdPacket, Connection* connection);
  virtual void onStartSession(Connection* NMP_UNUSED(connection)) {}
  virtual void onStopSession(Connection* NMP_UNUSED(connection)) {}
  virtual void onNewConnection(Connection* NMP_UNUSED(connection)) {}
  virtual void onConnectionClosed(Connection* NMP_UNUSED(connection)) {}
  virtual void preFrameUpdate() {}
  virtual void postFrameUpdate() {}

protected:
  void addCommandsHandler(CommandsHandler* handler);
  void removeCommandsHandler(CommandsHandler* handler);
private:
  static const uint8_t  MAX_NUM_CMD_HANDLERS = 8;
  uint8_t               m_numCmdHandlers;
  CommandsHandler*      m_cmdHandlers[MAX_NUM_CMD_HANDLERS];
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_COMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
