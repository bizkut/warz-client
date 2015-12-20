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
#include "comms/euphoriaCommandsHandler.h"
#include "comms/runtimeTargetInterface.h"
#include "comms/mcomms.h"
#include "comms/connection.h"
#include "comms/commsServer.h"

#include "comms/euphoriaCommsServerModule.h"
#include "comms/euphoriaDataManagementInterface.h"
#include "comms/euphoriaPackets.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
EuphoriaCommandsHandler::EuphoriaCommandsHandler(RuntimeTargetInterface* target) :
  CommandsHandler(target)
{
}

//----------------------------------------------------------------------------------------------------------------------
EuphoriaCommandsHandler::~EuphoriaCommandsHandler()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaCommandsHandler::doHandleCommand(CmdPacketBase* commandPacket)
{
  uint16_t pktId = commandPacket->hdr.m_id;
  NMP::netEndianSwap(pktId);

  switch (pktId)
  {
  case pk_EnableModuleDebugCmd:
    handleEnableModuleDebug(commandPacket);
    return true;
  case pk_EuphoriaDebugControlCmd:
    handleSetEuphoriaDebugControl(commandPacket);
    return true;
  default:
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaCommandsHandler::clear()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaCommandsHandler::handleEnableModuleDebug(CmdPacketBase* baseCommandPacket)
{
  EnableModuleDebugCmdPacket* commandPacket = static_cast<EnableModuleDebugCmdPacket*>(baseCommandPacket);
  commandPacket->deserialize();

  EuphoriaDataManagementInterface* euphoriaDataManager = m_target->getEuphoriaDataManager();

  bool result = euphoriaDataManager->enableModuleDebug(
    commandPacket->m_instanceID,
    commandPacket->m_moduleIndex,
    commandPacket->m_enableDebug);

  NMP_ASSERT(result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaCommandsHandler::handleSetEuphoriaDebugControl(CmdPacketBase* baseCommandPacket)
{
  EuphoriaDebugControlCmdPacket* commandPacket = static_cast<EuphoriaDebugControlCmdPacket*>(baseCommandPacket);
  commandPacket->deserialize();

  EuphoriaDataManagementInterface* euphoriaDataManager = m_target->getEuphoriaDataManager();

  bool result = false;
  switch (commandPacket->m_controlData.m_controlDataType)
  {
  case ER::kBoolControlDataType:
    result = euphoriaDataManager->setDebugControl(
      commandPacket->m_instanceID,
      commandPacket->m_controlData.m_controlId,
      commandPacket->m_controlData.m_limbIndex,
      commandPacket->m_controlData.m_value.m_bool);
    break;
  case ER::kInt32ControlDataType:
    result = euphoriaDataManager->setDebugControl(
      commandPacket->m_instanceID,
      commandPacket->m_controlData.m_controlId,
      commandPacket->m_controlData.m_limbIndex,
      commandPacket->m_controlData.m_value.m_int32);
    break;
  case ER::kUInt32ControlDataType:
    result = euphoriaDataManager->setDebugControl(
      commandPacket->m_instanceID,
      commandPacket->m_controlData.m_controlId,
      commandPacket->m_controlData.m_limbIndex,
      commandPacket->m_controlData.m_value.m_uint32);
    break;
  case ER::kFloatControlDataType:
    result = euphoriaDataManager->setDebugControl(
      commandPacket->m_instanceID,
      commandPacket->m_controlData.m_controlId,
      commandPacket->m_controlData.m_limbIndex,
      commandPacket->m_controlData.m_value.m_float);
    break;
  case ER::kNumControlDataTypes:
  default:
    break;
  }

  NMP_ASSERT(result);
  return result;
}

} // namespace MCOMMS
