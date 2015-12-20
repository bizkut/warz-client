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
#ifndef MCOMMS_LIVE_LINK_NETWORK_MANAGER_H
#define MCOMMS_LIVE_LINK_NETWORK_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
#include "NMPlatform/NMMapContainer.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkNetworkManager:
//
// COMMS uses this interface to inform morpheme:connect of available network definitions and network instances. This
// implementation maintains a simple list of both. Network definitions and instance must be published through this
// manager to be visible to COMMS and so the LiveLink capabilities.
//
// Other components in the tutorials use this manager to access the list of networks and network definitions as well,
// as such this class is the central instance for LiveLink related features.
//----------------------------------------------------------------------------------------------------------------------
class LiveLinkNetworkManager :
  public MCOMMS::NetworkManagementInterface
{
public:

  LiveLinkNetworkManager(uint32_t maxNumCharacterDefs, uint32_t maxNumCharacters);
  virtual ~LiveLinkNetworkManager();

  static LiveLinkNetworkManager* getInstance() { return sm_instance; }

  //----------------------------
  // Network (definition) publishing.
  //
  // Network definitions and network instances must be published to be visible to COMMS. This manager is globally
  // accessible (see getInstance()) so it can be reached from anywhere in the code base.
public:

  //----------------------------
  // Publishes the given network definition.
  bool publishNetworkDef(MR::NetworkDef* gameCharacterDef, const uint8_t* guid);

  //----------------------------
  // Revokes the given network definition.
  bool revokeNetworkDef(MR::NetworkDef* gameCharacterDef);

  //----------------------------
  // Checks if a network definition with the given GUID is known to the manager.
  bool hasNetworkDef(const MCOMMS::GUID& guid) const;

  //----------------------------
  // Returns the network definition known by the given GUID or NULL if the GUID is unknown.
  MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& guid) const;

  //----------------------------
  // Publishes the given network instance under the given name.
  // The respective network definition is also published if thats not already the case.
  bool publishNetwork(MR::Network* gameCharacter, const char* name, MCOMMS::InstanceID& instanceId);

  //----------------------------
  // Revokes the given network from the list of published networks.
  // The respective network definition will stay published until explicitly revoked.
  bool revokeNetwork(MR::Network* gameCharacter);

  //----------------------------
  // Returns the network instance known by the given ID or NULL if the ID is unknonw.
  // In addition the name the instance has been published under is returned as well.
  MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id, const char *&name) const;

  //----------------------------
  // This section implements MCOMMS::NetworkManagementInterface using the containers held by this class.
public:

  virtual uint32_t getNetworkDefCount() const NM_OVERRIDE;
  virtual uint32_t getNetworkDefGUIDs(MCOMMS::GUID* guids, uint32_t maxGuids) const NM_OVERRIDE;
  virtual bool isNetworkDefinitionLoaded(const MCOMMS::GUID& guid) const NM_OVERRIDE;

  virtual uint32_t getNetworkInstanceCount() const NM_OVERRIDE;
  virtual uint32_t getNetworkInstanceIDs(MCOMMS::InstanceID* instanceIDs, uint32_t maxIntanceIDs) const NM_OVERRIDE;
  virtual const char* getNetworkInstanceName(MCOMMS::InstanceID id) const NM_OVERRIDE;
  virtual bool getNetworkInstanceDefinitionGUID(MCOMMS::InstanceID id, MCOMMS::GUID& guid) const NM_OVERRIDE;

protected:

  class NetworkEntry
  {
  public:
    static const uint32_t MAX_CHARACTERNAME_LENGTH = 32;
    char              m_name[MAX_CHARACTERNAME_LENGTH];
    MR::Network*   m_gameCharacter;

    NetworkEntry() :
      m_gameCharacter(0)
    {
      m_name[0] = '\0';
    }

      NetworkEntry(MR::Network* gameCharacter, const char* name)
    {
      m_gameCharacter = gameCharacter;
      NMP_STRNCPY_S(m_name, MAX_CHARACTERNAME_LENGTH, name);
    }

    NetworkEntry& operator=(const NetworkEntry& other)
    {
      m_gameCharacter = other.m_gameCharacter;
      NMP_STRNCPY_S(m_name, MAX_CHARACTERNAME_LENGTH, other.m_name);
      return (*this);
    }
  };

  typedef NMP::MapContainer<MCOMMS::GUID, MR::NetworkDef*> GameCharacterDefMap;
  typedef NMP::MapContainer<MCOMMS::InstanceID, NetworkEntry> GameCharacterMap;

protected:
  static LiveLinkNetworkManager*  sm_instance;
  static MCOMMS::InstanceID       sm_nextInstanceId;

  GameCharacterDefMap*          m_characterDefs;    // A map of published game character definitions
  GameCharacterMap*             m_charactersData;   // A map of published game characters

  static MCOMMS::InstanceID getNextInstanceId() { return sm_nextInstanceId++; }
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_LIVE_LINK_NETWORK_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
