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
#ifndef MCOMMS_COMMANDSHANDLER_H
#define MCOMMS_COMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

class RuntimeTargetInterface;
class Connection;
struct CmdPacketBase;

//----------------------------------------------------------------------------------------------------------------------
///
class CommandsHandler
{
public:
  CommandsHandler(RuntimeTargetInterface* target) :
    m_target(target),
    m_currentConnection(0)
  {
  }

  virtual ~CommandsHandler()
  {
  }

  bool handleCommand(CmdPacketBase* cmdPkt, Connection* connection)
  {
    setCurrentConnection(connection);
    bool ret = doHandleCommand(cmdPkt);
    resetCurrentConnection();
    return ret;
  }

  inline void setCurrentConnection(Connection* connection) { m_currentConnection = connection; }
  inline Connection* getCurrentConnection() const { return m_currentConnection; }
  inline void resetCurrentConnection() { setCurrentConnection(0); }

  virtual void clear() = 0;
  virtual void onConnectionClosed(Connection* NMP_UNUSED(connection)) {};

private:
  virtual bool doHandleCommand(CmdPacketBase* cmdPkt) = 0;
protected:

  RuntimeTargetInterface*  m_target;
  Connection*              m_currentConnection; /// < Avoid passing around the pointer to the connection.
};

} //namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_COMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------
