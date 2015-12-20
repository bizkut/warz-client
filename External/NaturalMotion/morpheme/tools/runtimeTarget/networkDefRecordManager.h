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
#ifndef NM_RTT_NETWORKDEFRECORDMANAGER_H
#define NM_RTT_NETWORKDEFRECORDMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
#include <map>
#include <string>
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class NetworkDef;
class NetworkPredictionDef;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkDefRecordManager
/// \brief Decorates a network definition with additional information as required by the runtime target to operate.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefRecord
{
public:

  NetworkDefRecord(
    const MCOMMS::GUID&       guid,
    const char*               name,
    MR::NetworkDef*           netDef,
    MR::NetworkPredictionDef* netPredictionDef);
  virtual ~NetworkDefRecord();

  const MCOMMS::GUID& getGUID() const { return m_guid; }
  const char* getName() const { return m_name.c_str(); }
  MR::NetworkDef* getNetDef() const { return m_netDef; }
  MR::NetworkPredictionDef* getNetPredictionDef() const { return m_netPredictionDef; }

private:

  const MCOMMS::GUID              m_guid;
  const std::string               m_name;
  MR::NetworkDef* const           m_netDef;
  MR::NetworkPredictionDef* const m_netPredictionDef;

  NetworkDefRecord(const NetworkDefRecord&);
  NetworkDefRecord& operator=(const NetworkDefRecord&);
};

typedef std::map<MCOMMS::GUID, NetworkDefRecord*> NetworkDefMap;
typedef std::pair<MCOMMS::GUID, NetworkDefRecord*> NetworkDefMapEntry;

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkDefRecordManager
/// \brief Keeps track of the set of network definitions loaded by the runtime target.
/// \ingroup RuntimeTarget
///
/// The actual loading/unloading of network definitions for the runtime target is handled by DefaultAssetMgr. This class
/// simply keeps track of the available set of network definitions for use across the various runtime target components.
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefRecordManager
{
public:

  NetworkDefRecordManager();
  ~NetworkDefRecordManager();

  /// \brief Returns the network definition record indicated by the GUID or NULL if such a record does not exist.
  NetworkDefRecord* findNetworkDefRecord(const MCOMMS::GUID& guid) const;

  /// \brief Adds a network definition record to the map.
  void addNetworkDefRecord(NetworkDefRecord* record);

  /// \brief Removes the network definition record indicated by the given GUID.
  void removeNetworkDefRecord(const MCOMMS::GUID& guid);

  /// \brief Returns the actual network definition record map.
  NetworkDefMap& getNetworkDefMap() { return m_networkDefs; }

  /// \brief Returns the actual network definition record map.
  const NetworkDefMap& getNetworkDefMap() const { return m_networkDefs; }

private:

  NetworkDefMap                 m_networkDefs;
  mutable NetworkDefMapEntry    m_cachedNetworkDef;

  void resetCache();
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RTT_NETWORKDEFRECORDMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
