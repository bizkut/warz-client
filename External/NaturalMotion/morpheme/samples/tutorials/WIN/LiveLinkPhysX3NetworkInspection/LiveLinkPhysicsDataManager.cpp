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
#include "GameManagement/GameCharacter.h"
#include "LiveLinkPhysicsDataManager.h"
#include "comms/liveLinkNetworkManager.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
LiveLinkPhysicsDataManager::LiveLinkPhysicsDataManager(MCOMMS::LiveLinkNetworkManager* networkManager) :
  m_networkManager(networkManager)
{
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkPhysicsDataManager::getPhysicsEngineID(char* buffer, uint32_t bufferLength) const
{
  NMP_STRNCPY_S(buffer, bufferLength, "PhysX3");
}

//----------------------------------------------------------------------------------------------------------------------
MR::Network* LiveLinkPhysicsDataManager::findNetworkByInstanceID(MCOMMS::InstanceID id) const
{
  const char* networkName = 0;
  MR::Network* network = m_networkManager->findNetworkByInstanceID(id, networkName);
  return network;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* LiveLinkPhysicsDataManager::findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const
{
  MR::NetworkDef* networkDef = m_networkManager->findNetworkDefByGuid(networkGUID);
  return networkDef;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID LiveLinkPhysicsDataManager::findPhysicsObjectIDForPart(const MR::PhysicsRig::Part* NMP_UNUSED(part)) const
{
  // This particular feature is not implemented, yet. The object id is currently used for more advanced stuff like
  // creating constraints or sending forces to a physics body, but it's not necessary for the simple purpose of
  // visualizing the rigid bodies in connect.
  return (MR::PhysicsObjectID)-1;
}

//----------------------------------------------------------------------------------------------------------------------
