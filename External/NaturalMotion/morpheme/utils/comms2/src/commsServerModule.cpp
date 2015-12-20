// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "comms/commsServerModule.h"
#include "comms/commandsHandler.h"

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
CommsServerModule::CommsServerModule() :
  m_numCmdHandlers(0)
{
  for (uint8_t i = 0; i < MAX_NUM_CMD_HANDLERS; ++i)
  {
    m_cmdHandlers[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
CommsServerModule::~CommsServerModule()
{
  for (uint8_t i = 0; i < m_numCmdHandlers; ++i)
  {
    m_cmdHandlers[i] = 0;
  }
  m_numCmdHandlers = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServerModule::addCommandsHandler(CommandsHandler* handler)
{
  NMP_ASSERT(m_numCmdHandlers < MAX_NUM_CMD_HANDLERS);
  m_cmdHandlers[m_numCmdHandlers] = handler;
  ++m_numCmdHandlers;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsServerModule::removeCommandsHandler(CommandsHandler* handler)
{
  uint8_t foundIndex = (uint8_t) - 1;
  for (uint8_t i = 0; i < m_numCmdHandlers; ++i)
  {
    if (handler == m_cmdHandlers[i])
    {
      foundIndex = i;
      break;
    }
  }

  if (foundIndex != (uint8_t) - 1)
  {
    // Found. Remove it.
    --m_numCmdHandlers;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool CommsServerModule::handleCommand(CmdPacketBase* cmdPacket, Connection* connection)
{
  // Standard implementation of the command handling, just scan the available handlers for a good one.
  for (uint8_t i = 0; i < m_numCmdHandlers; ++i)
  {
    if (m_cmdHandlers[i]->handleCommand(cmdPacket, connection))
    {
      return true;
    }
  }

  return false;
}

} // namespace MCOMMS
