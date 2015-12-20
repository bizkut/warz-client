// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "comms/euphoriaCommsServerModule.h"

#include "sharedDefines/mEuphoriaDebugInterface.h"

#include "comms/packet.h"
#include "comms/debugPackets.h"
#include "comms/euphoriaPackets.h"

#include "comms/commsServer.h"
#include "comms/euphoriaDataManagementInterface.h"
#include "comms/runtimeTargetInterface.h"

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
EuphoriaCommsServerModule::EuphoriaCommsServerModule() :
 CommsServerModule(),
  m_commandsHandler(getRuntimeTarget())
{
  addCommandsHandler(&m_commandsHandler);
}

//----------------------------------------------------------------------------------------------------------------------
EuphoriaCommsServerModule::~EuphoriaCommsServerModule()
{
  removeCommandsHandler(&m_commandsHandler);
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::sendDefData(const GUID& NMP_UNUSED(networkGUID), Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::sendInstanceStaticData(
  const GUID& NMP_UNUSED(networkGUID),
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  RuntimeTargetInterface* runtimeTarget = getRuntimeTarget();
  EuphoriaDataManagementInterface* euphoriaDataManager = runtimeTarget->getEuphoriaDataManager();

  MCOMMS::CommsServer* commsServer = MCOMMS::CommsServer::getInstance();

  if (euphoriaDataManager != 0)
  {
    uint32_t numModules = euphoriaDataManager->getModuleCount(instanceID);
    if (numModules > 0)
    {
      MCOMMS::BeginPersistentPacket beginPersistentData(MR::kEuphoriaNetworkPersistentData);
      mcommsBufferDataPacket(beginPersistentData, connection);

      MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();
      MCOMMS::PersistentDataPacket* euphoriaNetworkPacket = MCOMMS::PersistentDataPacket::create(
        networkDataBuffer,
        instanceID,
        MR::kEuphoriaNetworkPersistentData,
        sizeof(ER::EuphoriaNetworkPersistentData));

      ER::EuphoriaNetworkPersistentData* euphoriaNetwork =
        static_cast<ER::EuphoriaNetworkPersistentData*>(euphoriaNetworkPacket->getData());
      euphoriaNetwork->m_numModules = numModules;

      uint32_t numControlTypes = euphoriaDataManager->getNumControlTypes(instanceID);
      euphoriaNetwork->m_numControlTypes = numControlTypes;

      for (uint32_t iControlType = 0 ; iControlType != numControlTypes ; ++iControlType)
      {
        if (iControlType < numControlTypes)
        {
          const char* controlTypeName = euphoriaDataManager->getControlTypeName(instanceID, iControlType);
          euphoriaNetwork->m_controlTypeName[iControlType] = commsServer->getTokenForString(controlTypeName);
        }
        else
        {
          euphoriaNetwork->m_controlTypeName[iControlType] = MCOMMS_INVALID_STRINGTOKEN;
        }
      }

      ER::EuphoriaNetworkPersistentData::endianSwap(euphoriaNetwork);
      mcommsSerializeDataPacket(*euphoriaNetworkPacket);

      // kEuphoriaDebugControlDataType
      uint32_t debugControlCount = euphoriaDataManager->getDebugControlCount(instanceID);
      if (debugControlCount > 0)
      {
        ER::DebugControlID controlID;
        const char* controlName;
        ER::DebugControlFlags controlFlags;
        ER::DebugControlDataType controlDataType;
        for (uint32_t i = 0; i != debugControlCount; ++i)
        {
          bool result = euphoriaDataManager->getDebugControlInfo(
            instanceID,
            i,
            &controlName,
            &controlID,
            &controlFlags,
            &controlDataType);

          if (result)
          {
            MCOMMS::EuphoriaPersistentDataPacket* packet = MCOMMS::EuphoriaPersistentDataPacket::create(
              networkDataBuffer,
              ER::kEuphoriaDebugControlDataType,
              sizeof(ER::EuphoriaDebugControlPersistentData));

            MR::StringToken controlNameToken = commsServer->getTokenForString(controlName);
            ER::EuphoriaDebugControlPersistentData::serialiseTx(
              packet->getData(),
              controlNameToken,
              controlID,
              controlFlags,
              controlDataType);

            mcommsSerializeDataPacket(*packet);
          }
        }
      }

      // kEuphoriaBodyDataType
      uint32_t dataSize = euphoriaDataManager->serializeBodyTx(instanceID, 0, 0);
      if (dataSize)
      {
        MCOMMS::EuphoriaPersistentDataPacket* packet = MCOMMS::EuphoriaPersistentDataPacket::create(
          networkDataBuffer,
          ER::kEuphoriaBodyDataType,
          dataSize);

        void* data = packet->getData();
  #if defined(NMP_ENABLE_ASSERTS)
        uint32_t usedSize =
  #endif
          euphoriaDataManager->serializeBodyTx(instanceID, data, dataSize);
        NMP_ASSERT(usedSize <= dataSize);

        mcommsSerializeDataPacket(*packet);
      }

      // kEuphoriaLimbDataType
      uint32_t numLimbs = euphoriaDataManager->getLimbCount(instanceID);
      for (uint32_t limbIndex = 0; limbIndex != numLimbs; ++limbIndex)
      {
        uint32_t limbDataSize = euphoriaDataManager->serializeLimbTx(instanceID, limbIndex, 0, 0);
        if (limbDataSize)
        {
          MCOMMS::EuphoriaPersistentDataPacket* packet = MCOMMS::EuphoriaPersistentDataPacket::create(
            networkDataBuffer,
            ER::kEuphoriaLimbDataType,
            limbDataSize);

          void* data = packet->getData();
  #if defined(NMP_ENABLE_ASSERTS)
          uint32_t usedSize =
  #endif
            euphoriaDataManager->serializeLimbTx(instanceID, limbIndex, data, limbDataSize);
          NMP_ASSERT(usedSize <= limbDataSize);

          mcommsSerializeDataPacket(*packet);
        }
      }

      // kEuphoriaModuleDataType
      for (uint32_t moduleIndex = 0; moduleIndex != numModules; ++moduleIndex)
      {
        MCOMMS::EuphoriaPersistentDataPacket* modulePacket = MCOMMS::EuphoriaPersistentDataPacket::create(
          networkDataBuffer,
          ER::kEuphoriaModuleDataType,
          sizeof(ER::EuphoriaModulePersistentData));

        ER::EuphoriaModulePersistentData* persistentData = static_cast<ER::EuphoriaModulePersistentData*>(modulePacket->getData());

        const char* moduleName = euphoriaDataManager->getModuleName(instanceID, moduleIndex);
        persistentData->m_moduleName = commsServer->getTokenForString(moduleName);

        const char* moduleParentName = euphoriaDataManager->getModuleParentName(instanceID, moduleIndex);
        persistentData->m_parentModuleName = moduleParentName == NULL ? MCOMMS_INVALID_STRINGTOKEN : commsServer->getTokenForString(moduleParentName);

        ER::EuphoriaModulePersistentData::endianSwap(persistentData);
        mcommsSerializeDataPacket(*modulePacket);
      }

      MCOMMS::EndPersistentPacket endPersistentData;
      mcommsBufferDataPacket(endPersistentData, connection);
    }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::sendInstanceFrameData(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  Connection* MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  RuntimeTargetInterface* runtimeTarget = getRuntimeTarget();
  EuphoriaDataManagementInterface* euphoriaDataManager = runtimeTarget->getEuphoriaDataManager();

  if (euphoriaDataManager != 0)
  {
    uint32_t numModules = euphoriaDataManager->getModuleCount(instanceID);
    if (numModules > 0)
    {
      BeginInstanceSectionPacket beginInstanceData(MCOMMS::kEuphoriaDataSectionType);
      mcommsBufferDataPacket(beginInstanceData, connection);

      MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();

      uint8_t limbCount = (uint8_t) euphoriaDataManager->getLimbCount(instanceID);

      // kEuphoriaDebugControlDataType
      //
      uint32_t debugControlCount = euphoriaDataManager->getDebugControlCount(instanceID);
      if (debugControlCount > 0)
      {
        ER::DebugControlID controlID;
        const char* controlName;
        ER::DebugControlFlags controlFlags;
        ER::DebugControlDataType controlDataType;

        for (uint32_t i = 0; i != debugControlCount; ++i)
        {
          bool result = euphoriaDataManager->getDebugControlInfo(
            instanceID,
            i,
            &controlName,
            &controlID,
            &controlFlags,
            &controlDataType);

          if (result)
          {
            if ((controlFlags & ER::kPerLimbControlFlag) == ER::kPerLimbControlFlag)
            {
              // have to send a frame data packet for each limb
              for (uint8_t limbIndex = 0; limbIndex != limbCount; ++limbIndex)
              {
                serializeEuphoriaControlFrameData(
                  networkDataBuffer,
                  euphoriaDataManager,
                  instanceID,
                  controlID,
                  controlDataType,
                  limbIndex);
              }
            }
            else
            {
              serializeEuphoriaControlFrameData(
                networkDataBuffer,
                euphoriaDataManager,
                instanceID,
                controlID,
                controlDataType,
                ER::INVALID_LIMB_INDEX);
            }
          }
        }
      }

      // kEuphoriaModuleDataType
      //
      for (uint32_t moduleIndex = 0; moduleIndex != numModules; ++moduleIndex)
      {
        MCOMMS::EuphoriaFrameDataPacket* modulePacket = MCOMMS::EuphoriaFrameDataPacket::create(
          networkDataBuffer,
          ER::kEuphoriaModuleDataType,
          sizeof(ER::EuphoriaModuleFrameData));

        ER::EuphoriaModuleFrameData* frameData = static_cast<ER::EuphoriaModuleFrameData*>(modulePacket->getData());

        frameData->m_moduleDebugEnabled = euphoriaDataManager->isModuleDebugEnabled(instanceID, moduleIndex);

        ER::EuphoriaModuleFrameData::endianSwap(frameData);
        mcommsSerializeDataPacket(*modulePacket);
      }

      // kEuphoriaLimbControlAmountsDataType
      for (uint8_t limbIndex = 0; limbIndex != limbCount; ++limbIndex)
      {
        float limbStiffnessFraction = 0.0f;
        const ER::LimbControlAmounts* limbControlAmounts = euphoriaDataManager->getLimbControlAmounts(
          instanceID, limbIndex, limbStiffnessFraction);
        if (limbControlAmounts)
        {
          MCOMMS::EuphoriaFrameDataPacket* packet = MCOMMS::EuphoriaFrameDataPacket::create(
            networkDataBuffer,
            ER::kEuphoriaLimbControlAmountsDataType,
            sizeof(ER::EuphoriaLimbControlAmountsFrameData));

          ER::EuphoriaLimbControlAmountsFrameData* frameData = 
            static_cast<ER::EuphoriaLimbControlAmountsFrameData*>(packet->getData());

          frameData->m_limbControlAmounts = *limbControlAmounts;
          frameData->m_limbStiffnessFraction = limbStiffnessFraction;
          frameData->m_limbIndex = limbIndex;

          ER::EuphoriaLimbControlAmountsFrameData::endianSwap(frameData);
          mcommsSerializeDataPacket(*packet);
        }
      }

      EndInstanceSectionPacket endInstanceData(MCOMMS::kEuphoriaDataSectionType);
      mcommsBufferDataPacket(endInstanceData, connection);
    }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::sendFrameData(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::preFrameUpdate()
{
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::postFrameUpdate()
{
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::onStartSession(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::onStopSession(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommsServerModule::onConnectionClosed(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaCommsServerModule::serializeEuphoriaControlFrameData(
  MCOMMS::NetworkDataBuffer* networkDataBuffer,
  EuphoriaDataManagementInterface* euphoriaDataManager,
  InstanceID instanceID,
  ER::DebugControlID controlID,
  ER::DebugControlDataType controlDataType,
  uint8_t limbIndex)
{
  bool result;
  MCOMMS::EuphoriaFrameDataPacket* packet = MCOMMS::EuphoriaFrameDataPacket::create(
    networkDataBuffer,
    ER::kEuphoriaDebugControlDataType,
    sizeof(ER::EuphoriaDebugControlFrameData));

  switch (controlDataType)
  {
  case ER::kBoolControlDataType:
    {
      bool value;
      result = euphoriaDataManager->getDebugControl(instanceID, controlID, limbIndex, &value);
      NMP_ASSERT(result);
      ER::EuphoriaDebugControlFrameData::serialiseTx(packet->getData(), controlID, limbIndex, value);
    }
    break;
  case ER::kInt32ControlDataType:
    {
      int32_t value;
      result = euphoriaDataManager->getDebugControl(instanceID, controlID, limbIndex, &value);
      NMP_ASSERT(result);
      ER::EuphoriaDebugControlFrameData::serialiseTx(packet->getData(), controlID, limbIndex, value);
    }
    break;
  case ER::kUInt32ControlDataType:
    {
      uint32_t value;
      result = euphoriaDataManager->getDebugControl(instanceID, controlID, limbIndex, &value);
      NMP_ASSERT(result);
      ER::EuphoriaDebugControlFrameData::serialiseTx(packet->getData(), controlID, limbIndex, value);
    }
    break;
  case ER::kFloatControlDataType:
    {
      float value;
      result = euphoriaDataManager->getDebugControl(instanceID, controlID, limbIndex, &value);
      NMP_ASSERT(result);
      ER::EuphoriaDebugControlFrameData::serialiseTx(packet->getData(), controlID, limbIndex, value);
    }
    break;
  default:
    NMP_ASSERT_FAIL();
    result = false;
    break;
  }

  mcommsSerializeDataPacket(*packet);
  return result;
}

} // namespace MCOMMS
