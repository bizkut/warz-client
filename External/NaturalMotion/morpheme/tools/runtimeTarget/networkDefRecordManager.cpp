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
#include "networkDefRecordManager.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefRecord
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NetworkDefRecord::NetworkDefRecord(
  const MCOMMS::GUID&       guid,
  const char*               name,
  MR::NetworkDef*           netDef,
  MR::NetworkPredictionDef* netPredictionDef) :
    m_guid(guid),
    m_name(name),
    m_netDef(netDef),
    m_netPredictionDef(netPredictionDef)
{
  NMP_ASSERT(name && netDef);
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefRecord::~NetworkDefRecord()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefRecordManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NetworkDefRecordManager::NetworkDefRecordManager() :
  m_networkDefs(),
  m_cachedNetworkDef()
{
  resetCache();
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefRecordManager::~NetworkDefRecordManager()
{
  NMP_ASSERT(m_networkDefs.empty());
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefRecord* NetworkDefRecordManager::findNetworkDefRecord(const MCOMMS::GUID& guid) const
{
  // Check cached (MRU) value first ...
  if (m_cachedNetworkDef.first == guid)
  {
    return m_cachedNetworkDef.second;
  }

  // ... then search whole map
  NetworkDefMap::const_iterator it = m_networkDefs.find(guid);

  if (it == m_networkDefs.end())
  {
    return 0;
  }
  else
  {
    m_cachedNetworkDef.first = guid;
    m_cachedNetworkDef.second = it->second;

    return it->second;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefRecordManager::addNetworkDefRecord(NetworkDefRecord* record)
{
  if (!findNetworkDefRecord(record->getGUID()))
  {
    m_networkDefs.insert(NetworkDefMapEntry(record->getGUID(), record));
  }
  else
  {
    NMP_MSG("Warning: Network definition record already added!");
    NMP_ASSERT_MSG((findNetworkDefRecord(record->getGUID()) == record), "Error: Network definition ID conflict!");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefRecordManager::removeNetworkDefRecord(const MCOMMS::GUID& guid)
{
  if (m_cachedNetworkDef.first == guid)
  {
    resetCache();
  }

  NetworkDefMap::iterator it = m_networkDefs.find(guid);

  if (it != m_networkDefs.end())
  {
    m_networkDefs.erase(it);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefRecordManager::resetCache()
{
  MCOMMS::GUID& guid = m_cachedNetworkDef.first;
  m_cachedNetworkDef.second = NULL;

  // Set to 'blank' GUID.
  for (int i = 0; i < 16; ++i) { guid.value[i] = 254; }
}
