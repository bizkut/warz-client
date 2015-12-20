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
#ifndef LIVE_LINK_NETWORK_DEF_LIFECYCLE_H
#define LIVE_LINK_NETWORK_DEF_LIFECYCLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"

#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
//----------------------------------------------------------------------------------------------------------------------
#include <map>
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkNetworkDefLifecycle
// Implements MCOMMS::NetworkDefLifecycleInterface.
//----------------------------------------------------------------------------------------------------------------------
class LiveLinkNetworkDefLifecycle
  : public MCOMMS::NetworkDefLifecycleInterface
{
public:

   LiveLinkNetworkDefLifecycle(Game::CharacterManagerNoPhysics* characterManager);
  virtual ~LiveLinkNetworkDefLifecycle();

public:

  virtual bool canLoadNetworkDefinition() const { return true; }

  virtual bool loadNetworkDefinition(
    const MCOMMS::GUID& guid,
    const char*         networkDefName,
    MCOMMS::Connection* connection);

  virtual bool destroyNetworkDefinition(const MCOMMS::GUID& guid);
  virtual bool clearCachedData();

private:

  typedef std::map<MCOMMS::GUID, Game::CharacterDef*> CharacterDefMap;

  Game::CharacterManagerNoPhysics* m_characterManager; // Reference to an externally created character manager that holds all our Network defs etc.

  CharacterDefMap   m_characterDefs;

  LiveLinkNetworkDefLifecycle(const LiveLinkNetworkDefLifecycle&);
  LiveLinkNetworkDefLifecycle& operator=(const LiveLinkNetworkDefLifecycle&);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // LIVE_LINK_NETWORK_DEF_LIFECYCLE_H
//----------------------------------------------------------------------------------------------------------------------
