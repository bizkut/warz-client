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
#ifndef MCOMMS_SIMPLECONNECTIONMANAGER_H
#define MCOMMS_SIMPLECONNECTIONMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleConnectionManager
/// \brief Implements ConnectionManagementInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the connection management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class SimpleConnectionManager :
  public ConnectionManagementInterface
{
  // Construction/destruction
public:

  SimpleConnectionManager();
  virtual ~SimpleConnectionManager();

  uint32_t getNumConnections() const { return m_numConnections; };
  bool connectionsReducedToZero() const { return m_connectionsReducedToZero; }

  // [ConnectionManagementInterface]
public:

  virtual void onNewConnection(Connection* connection) NM_OVERRIDE;
  virtual void onConnectionClosed(Connection* connection) NM_OVERRIDE;
  virtual void startDebuggingNetworkInstance(Connection* connection, InstanceID id) NM_OVERRIDE;
  virtual void stopDebuggingNetworkInstance(Connection* connection, InstanceID id) NM_OVERRIDE;

private:

  uint32_t m_numConnections;
  bool     m_connectionsReducedToZero;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_SIMPLECONNECTIONMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
