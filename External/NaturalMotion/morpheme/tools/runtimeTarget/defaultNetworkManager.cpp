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
#include "defaultNetworkManager.h"

#include "networkDefRecordManager.h"
#include "networkInstanceRecordManager.h"
#include "runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
DefaultNetworkManager::DefaultNetworkManager(RuntimeTargetContext* context) :
  MCOMMS::NetworkManagementInterface(),
  m_context(context)
{
  NMP_ASSERT(context);
}

//----------------------------------------------------------------------------------------------------------------------
DefaultNetworkManager::~DefaultNetworkManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultNetworkManager::getNetworkDefCount() const
{
  return (uint32_t)m_context->getNetworkDefManager()->getNetworkDefMap().size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultNetworkManager::getNetworkDefGUIDs(
  MCOMMS::GUID* guids,
  uint32_t      maxGuids) const
{
  const NetworkDefMap& networkDefs = m_context->getNetworkDefManager()->getNetworkDefMap();
  const NetworkDefMap::const_iterator end = networkDefs.end();
  NetworkDefMap::const_iterator it = networkDefs.begin();

  for (uint32_t i = 0; (it != end) && (i < maxGuids) ; ++it)
  {
    guids[i++] = (*it).first;
  }

  return (uint32_t)networkDefs.size();
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultNetworkManager::isNetworkDefinitionLoaded(const MCOMMS::GUID& guid) const
{
  const NetworkDefMap& networkDefs = m_context->getNetworkDefManager()->getNetworkDefMap();
  NetworkDefMap::const_iterator i = networkDefs.find(guid);

  return (i != networkDefs.end());
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultNetworkManager::getNetworkInstanceCount() const
{
  return (uint32_t)m_context->getNetworkInstanceManager()->getNetworkInstanceMap().size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultNetworkManager::getNetworkInstanceIDs(MCOMMS::InstanceID* instanceIDs, uint32_t maxInstanceIDs) const
{
  const NetworkInstanceMap& networkInstances = m_context->getNetworkInstanceManager()->getNetworkInstanceMap();
  const NetworkInstanceMap::const_iterator end = networkInstances.end();
  NetworkInstanceMap::const_iterator it = networkInstances.begin();

  for (uint32_t i = 0; (it != end) && (i < maxInstanceIDs); ++it)
  {
    instanceIDs[i++] = (*it).first;
  }

  return (uint32_t)networkInstances.size();
}

//----------------------------------------------------------------------------------------------------------------------
const char* DefaultNetworkManager::getNetworkInstanceName(MCOMMS::InstanceID id) const
{
  NetworkInstanceRecord* const instanceRecord = m_context->getNetworkInstanceManager()->findInstanceRecord(id);

  if (!instanceRecord)
  {
    return "";
  }
  else
  {
    return instanceRecord->getName();
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultNetworkManager::getNetworkInstanceDefinitionGUID(MCOMMS::InstanceID id, MCOMMS::GUID& guid) const
{
  NetworkInstanceRecord* const instanceRecord = m_context->getNetworkInstanceManager()->findInstanceRecord(id);

  if (!instanceRecord)
  {
    return false;
  }
  else
  {
    guid = instanceRecord->getNetworkDefRecord()->getGUID();
    return true;
  }
}
