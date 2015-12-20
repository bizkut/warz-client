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
#ifndef NM_DEFAULTCONNECTIONMANAGER_H
#define NM_DEFAULTCONNECTIONMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/simpleConnectionManager.h"
//----------------------------------------------------------------------------------------------------------------------

class RuntimeTargetContext;

//----------------------------------------------------------------------------------------------------------------------
class DefaultConnectionManager :
  public MCOMMS::SimpleConnectionManager
{
  // Construction/destruction
public:

  DefaultConnectionManager(RuntimeTargetContext* context);
  virtual ~DefaultConnectionManager();

  //--------------------------------------------------------------------------------------------------------------------
  // [ConnectionManagementInterface]
public:
  virtual void startDebuggingNetworkInstance(MCOMMS::Connection* connection, MCOMMS::InstanceID instanceID) NM_OVERRIDE;
  virtual void stopDebuggingNetworkInstance(MCOMMS::Connection* connection, MCOMMS::InstanceID instanceID) NM_OVERRIDE;
  virtual void onConnectionClosed(MCOMMS::Connection* connection) NM_OVERRIDE;

private:

  RuntimeTargetContext* const m_context;

  DefaultConnectionManager(const DefaultConnectionManager&);
  DefaultConnectionManager& operator=(const DefaultConnectionManager&);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTNETWORKMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
