// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#ifndef LIVE_LINK_PHYSICS_DATA_MANAGER_H
#define LIVE_LINK_PHYSICS_DATA_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/simpleDataManager.h"
#include "comms/physicsRuntimeTargetSimple.h"
#include "comms/euphoriaRuntimeTargetSimple.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
  class LiveLinkNetworkManager;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class LiveLinkPhysicsDataManager
/// \brief Implements MCOMMS::SimplePhysicsDataManager.
///
//----------------------------------------------------------------------------------------------------------------------
class LiveLinkPhysicsDataManager
  : public MCOMMS::SimplePhysicsDataManager
{
public:
  LiveLinkPhysicsDataManager(MCOMMS::LiveLinkNetworkManager* networkManager);

  virtual void getPhysicsEngineID(char* buffer, uint32_t bufferLength) const NM_OVERRIDE;

  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const NM_OVERRIDE;

  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const NM_OVERRIDE;

  virtual MR::PhysicsObjectID findPhysicsObjectIDForPart(const MR::PhysicsRig::Part* NMP_UNUSED(part)) const NM_OVERRIDE;

private:
  MCOMMS::LiveLinkNetworkManager* m_networkManager;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class LiveLinkEuphoriaDataManager
/// \brief Implements MCOMMS::SimpleEuphoriaDataManager.
//----------------------------------------------------------------------------------------------------------------------
class LiveLinkEuphoriaDataManager :
  public MCOMMS::SimpleEuphoriaDataManager
{
public:

  LiveLinkEuphoriaDataManager(MCOMMS::LiveLinkNetworkManager* networkManager);

  virtual ~LiveLinkEuphoriaDataManager() {}

  //---------------------------
  // [DataManagementInterface]
public:

  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const NM_OVERRIDE;
  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const NM_OVERRIDE;

protected:
  MCOMMS::LiveLinkNetworkManager* m_networkManager;

private:
  // disabled assignment operator to disable warning C4512
  //const LiveLinkEuphoriaDataManager& operator = (const LiveLinkEuphoriaDataManager& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // LIVE_LINK_PHYSICS_DATA_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
