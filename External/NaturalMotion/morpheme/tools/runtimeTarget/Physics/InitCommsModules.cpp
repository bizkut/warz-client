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
#include "InitCommsModules.h"

// Asset Processor Modules
#include "comms/physicsCommsServerModule.h"

//----------------------------------------------------------------------------------------------------------------------

MCOMMS::PhysicsCommsServerModule* g_physicsCommsServerModule = 0;

//----------------------------------------------------------------------------------------------------------------------
bool initCommsServerModules(MCOMMS::CommsServer *commsServer)
{
  NMP_ASSERT(commsServer != 0);
  NMP_ASSERT(g_physicsCommsServerModule == 0);

  g_physicsCommsServerModule = new MCOMMS::PhysicsCommsServerModule();
  commsServer->registerModule(g_physicsCommsServerModule);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool termCommsServerModules(MCOMMS::CommsServer *commsServer)
{
  NMP_ASSERT(commsServer != 0);
  NMP_ASSERT(g_physicsCommsServerModule != 0);

  commsServer->unregisterModule(g_physicsCommsServerModule);
  delete g_physicsCommsServerModule;
  g_physicsCommsServerModule = 0;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
