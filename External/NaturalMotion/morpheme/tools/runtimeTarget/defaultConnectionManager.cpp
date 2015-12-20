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
#include "defaultConnectionManager.h"
#include "networkInstanceRecordManager.h"
#include "runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
DefaultConnectionManager::DefaultConnectionManager(RuntimeTargetContext* context) :
  MCOMMS::SimpleConnectionManager(),
  m_context(context)
{
  NMP_ASSERT(context);
}

//----------------------------------------------------------------------------------------------------------------------
DefaultConnectionManager::~DefaultConnectionManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultConnectionManager::startDebuggingNetworkInstance(
  MCOMMS::Connection* NMP_UNUSED(connection),
  MCOMMS::InstanceID  NMP_UNUSED(instanceID))
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultConnectionManager::stopDebuggingNetworkInstance(
  MCOMMS::Connection* NMP_UNUSED(connection),
  MCOMMS::InstanceID  NMP_UNUSED(instanceID))
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultConnectionManager::onConnectionClosed(
  MCOMMS::Connection* connection)
{
  MCOMMS::SimpleConnectionManager::onConnectionClosed(connection);
}

//----------------------------------------------------------------------------------------------------------------------
