// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "comms/physicsCommsServerModule.h"

#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRigDef.h"

#include "comms/runtimeTargetInterface.h"

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
PhysicsCommsServerModule::PhysicsCommsServerModule() :
  CommsServerModule()
{
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCommsServerModule::~PhysicsCommsServerModule()
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::sendDefData(
  const GUID& MR_OUTPUT_DEBUG_ARG(networkGUID),
  Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  RuntimeTargetInterface* runtimeTarget = getRuntimeTarget();
  PhysicsDataManagementInterface* physicsDataManager = runtimeTarget->getPhysicsDataManager();

  if (physicsDataManager != 0)
  {
    MR::AnimSetIndex count = physicsDataManager->getPhysicsRigDefCount(networkGUID);
    for (MR::AnimSetIndex index = 0; index != count; ++index)
    {
      physicsDataManager->serializeTxPhysicsRigDefData(networkGUID, index, connection);
   }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::sendInstanceStaticData(
  const GUID& MR_OUTPUT_DEBUG_ARG(networkGUID),
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  RuntimeTargetInterface* runtimeTarget = getRuntimeTarget();
  PhysicsDataManagementInterface* physicsDataManager = runtimeTarget->getPhysicsDataManager();

  if (physicsDataManager != 0)
  {
    MR::AnimSetIndex count = physicsDataManager->getPhysicsRigDefCount(networkGUID);
    for (MR::AnimSetIndex index = 0; index != count; ++index)
    {
      physicsDataManager->serializeTxPhysicsRigPersistentData(instanceID, index, connection);
    }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::sendInstanceFrameData(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  RuntimeTargetInterface* runtimeTarget = getRuntimeTarget();
  PhysicsDataManagementInterface* physicsDataManager = runtimeTarget->getPhysicsDataManager();

  if (physicsDataManager != 0)
  {
    physicsDataManager->serializeTxPhysicsRigFrameData(instanceID, connection);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::sendFrameData(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::preFrameUpdate()
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::postFrameUpdate()
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::onStartSession(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::onStopSession(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsCommsServerModule::onConnectionClosed(Connection* NMP_UNUSED(connection))
{
}

} // namespace MCOMMS
