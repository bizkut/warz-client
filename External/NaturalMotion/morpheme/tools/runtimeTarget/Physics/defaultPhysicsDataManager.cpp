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
#include "defaultPhysicsDataManager.h"

#include "defaultPhysicsMgr.h"
#include "../networkDefRecordManager.h"
#include "../networkInstanceRecordManager.h"
#include "../runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// DefaultPhysicsDataManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsDataManager::DefaultPhysicsDataManager(RuntimeTargetContext* context, DefaultPhysicsMgr* physicsMgr) :
  SimplePhysicsDataManager(),
  m_physicsManager(physicsMgr),
  m_networkDefManager(context->getNetworkDefManager()),
  m_networkInstanceManager(context->getNetworkInstanceManager())
{
}

//--------------------------------------------------------------------------------------------------------------------
MR::Network* DefaultPhysicsDataManager::findNetworkByInstanceID(MCOMMS::InstanceID id) const
{
  NetworkInstanceRecord* networkRecord = m_networkInstanceManager->findInstanceRecord(id);
  if (networkRecord != 0)
  {
    MR::Network* network = networkRecord->getNetwork();
    return network;
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* DefaultPhysicsDataManager::findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const
{
  NetworkDefRecord* networkDefRecord = m_networkDefManager->findNetworkDefRecord(networkGUID);
  if (networkDefRecord != 0)
  {
    MR::NetworkDef* networkDef = networkDefRecord->getNetDef();
    return networkDef;
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsDataManager::findPhysicsObjectIDForPart(const MR::PhysicsRig::Part* part) const
{
  return m_physicsManager->getPhysicsIDForPart(part);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsDataManager::getPhysicsEngineID(char* buffer, uint32_t bufferLength) const
{
  m_physicsManager->getPhysicsEngineID(buffer, bufferLength);
}

//--------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsDataManager::onEnvironmentVariableChange(const MCOMMS::Attribute* attribute)
{
  // When the asset scale changes the physics manager should be notified in case it needs to re-create SDKs.
  if (attribute->getSemantic() == MCOMMS::Attribute::SEMANTIC_ASSET_SCALE)
  {
    MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();

    float assetScale = 1.0f;
    MCOMMS::Attribute* assetScaleAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_ASSET_SCALE);

    NMP_ASSERT(assetScaleAttr->getDataType() == MCOMMS::Attribute::DATATYPE_FLOAT);

    assetScale = *((float*)(assetScaleAttr->getData()));
    m_physicsManager->onAssetScaleChanged(assetScale);
  }
}
