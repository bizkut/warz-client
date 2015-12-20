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
#ifndef LIVE_LINK_NETWORK_LIFECYCLE_H
#define LIVE_LINK_NETWORK_LIFECYCLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"

#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"

#include <map>
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkNetworkLifecycle
// Implements MCOMMS::NetworkLifecycleInterface.
//----------------------------------------------------------------------------------------------------------------------
class LiveLinkNetworkLifecycle
  : public MCOMMS::NetworkLifecycleInterface
{
public:

  class CharacterEntry
  {
  public:

    MCOMMS::Connection*       m_owningConnection;
    Game::CharacterNoPhysics* m_character;
  };

  typedef std::map<MCOMMS::InstanceID, CharacterEntry> CharacterMap;

public:

  LiveLinkNetworkLifecycle(Game::CharacterManagerNoPhysics* characterManager);
  virtual ~LiveLinkNetworkLifecycle();

  //----------------------------
  // Returns the list of characters created by this managers.
  const CharacterMap& getCharacters() const { return m_characters; }

public:

  virtual bool canCreateNetworkInstance() const { return true; }

  //----------------------------
  // Create a character using the given network definition.
  //
  // The network instance is tracked in the local character list and considered owned by the COMMS connection it was
  // requested for. The character will be destroyed when requested by COMMS.
  virtual MCOMMS::InstanceID createNetworkInstance(
    const MCOMMS::GUID& guid,
    const char*         instanceName,
    uint32_t            animSetIndex,
    MCOMMS::Connection* connection,
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation);

  //----------------------------
  // Destroys the character holding the network instance with the given ID.
  virtual bool destroyNetworkInstance(MCOMMS::InstanceID id);

  //----------------------------
  // Tracks connections to destroy any character owned by the connection.
  virtual void onConnectionClosed(MCOMMS::Connection* connection);

private:

  Game::CharacterManagerNoPhysics* m_characterManager; // Reference to an externally created character manager that holds all our Network defs etc.

  CharacterMap m_characters;

  LiveLinkNetworkLifecycle(const LiveLinkNetworkLifecycle&);
  LiveLinkNetworkLifecycle& operator=(const LiveLinkNetworkLifecycle&);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // LIVE_LINK_NETWORK_LIFECYCLE_H
//----------------------------------------------------------------------------------------------------------------------
