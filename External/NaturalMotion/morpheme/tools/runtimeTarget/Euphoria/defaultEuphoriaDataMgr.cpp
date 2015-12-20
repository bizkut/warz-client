// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "defaultEuphoriaDataMgr.h"

#include "networkDefRecordManager.h"
#include "networkInstanceRecordManager.h"
#include "runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// DefaultEuphoriaDataManager
//----------------------------------------------------------------------------------------------------------------------
DefaultEuphoriaDataManager::DefaultEuphoriaDataManager(RuntimeTargetContext* context) :
  MCOMMS::SimpleEuphoriaDataManager(),
  m_networkDefManager(context->getNetworkDefManager()),
  m_networkInstanceManager(context->getNetworkInstanceManager())
{
}

//----------------------------------------------------------------------------------------------------------------------
MR::Network* DefaultEuphoriaDataManager::findNetworkByInstanceID(MCOMMS::InstanceID id) const
{
  NetworkInstanceRecord* networkRecord = m_networkInstanceManager->findInstanceRecord(id);
  if (networkRecord != 0)
  {
    MR::Network* network = networkRecord->getNetwork();
    return network;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* DefaultEuphoriaDataManager::findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const
{
  NetworkDefRecord* networkDefRecord = m_networkDefManager->findNetworkDefRecord(networkGUID);
  if (networkDefRecord != 0)
  {
    MR::NetworkDef* networkDef = networkDefRecord->getNetDef();
    return networkDef;
  }
  return 0;
}
