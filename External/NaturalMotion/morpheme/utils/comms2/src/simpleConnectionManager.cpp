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
#include "comms/simpleConnectionManager.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
// SimpleConnectionManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SimpleConnectionManager::SimpleConnectionManager() :
  ConnectionManagementInterface(),
  m_numConnections(0),
  m_connectionsReducedToZero(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
SimpleConnectionManager::~SimpleConnectionManager()
{
  NMP_ASSERT(m_numConnections == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleConnectionManager::onNewConnection(Connection* NMP_UNUSED(connection))
{
  ++m_numConnections;
  m_connectionsReducedToZero = false;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleConnectionManager::onConnectionClosed(Connection* NMP_UNUSED(connection))
{
  NMP_ASSERT(m_numConnections > 0);
  --m_numConnections;
  m_connectionsReducedToZero = (m_numConnections == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleConnectionManager::startDebuggingNetworkInstance(Connection* NMP_UNUSED(connection), InstanceID NMP_UNUSED(id))
{
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleConnectionManager::stopDebuggingNetworkInstance(Connection* NMP_UNUSED(connection), InstanceID NMP_UNUSED(id))
{
}

} // namespace COMMS
