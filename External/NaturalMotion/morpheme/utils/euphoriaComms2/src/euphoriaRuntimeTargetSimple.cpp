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
#include "comms/euphoriaRuntimeTargetSimple.h"

#include "sharedDefines/mEuphoriaDebugInterface.h"

#include "physics/mrPhysics.h"

#include "euphoria/erAttribData.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erDebugControls.h"
#include "euphoria/erLimbInterface.h"
#include "euphoria/erLimbDef.h"
#include "euphoria/erModule.h"

#include "comms/connection.h"
#include "comms/corePackets.h"
#include "comms/debugPackets.h"
#include "comms/physicsPackets.h"
#include "comms/commsServer.h"

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
// SimpleEuphoriaPhysicsDataManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SimpleEuphoriaPhysicsDataManager::SimpleEuphoriaPhysicsDataManager() :
  SimplePhysicsDataManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaPhysicsDataManager::serializeTxPhysicsRigFrameData(InstanceID instanceID, Connection* connection)
{
  if (!SimplePhysicsDataManager::serializeTxPhysicsRigFrameData(instanceID, connection))
  {
    return false;
  }

#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return false;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (euphoriaCharacter != 0)
  {
    BeginInstanceSectionPacket beginInstanceData(MCOMMS::kPhysicsDataSectionType);
    mcommsBufferDataPacket(beginInstanceData, connection);

    MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();

    // there is no additional data needed with this packet
    MCOMMS::FrameDataPacket* euphoriaNetworkDataPacket = MCOMMS::FrameDataPacket::create(
      networkDataBuffer,
      instanceID,
      MR::kEuphoriaNetworkFrameData,
      0);
    mcommsSerializeDataPacket(*euphoriaNetworkDataPacket);

    MCOMMS::EndInstanceSectionPacket endInstanceData(MCOMMS::kPhysicsDataSectionType);
    mcommsBufferDataPacket(endInstanceData, connection);

    return true;
  }

  return false;
#else // defined(MR_OUTPUT_DEBUGGING)
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
// SimpleEuphoriaDataManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SimpleEuphoriaDataManager::SimpleEuphoriaDataManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEuphoriaDataManager::getModuleCount(InstanceID MR_OUTPUT_DEBUG_ARG(instanceID)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    return 0;
  }
  MR::InstanceDebugInterface* debugInterface = euphoriaCharacter->getDebugInterface();
  if (!debugInterface)
  {
    return 0;
  }

  return debugInterface->totalModules();
#else // defined(MR_OUTPUT_DEBUGGING)
  return 0;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleEuphoriaDataManager::getModuleParentName(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t    MR_OUTPUT_DEBUG_ARG(moduleIndex)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return NULL;
  }

  const ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return NULL;
  }

  MR::InstanceDebugInterface* debugInterface = euphoriaCharacter->getDebugInterface();
  if (!debugInterface)
  {
    NMP_DEBUG_MSG("error: Instance has no MR::InstanceDebugInterface!");
    return NULL;
  }

  if (moduleIndex >= debugInterface->totalModules())
  {
    NMP_DEBUG_MSG("error: Valid module index expected!");
    return NULL;
  }

  return debugInterface->getModuleParentName(moduleIndex);
#else // defined(MR_OUTPUT_DEBUGGING)
  return NULL;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleEuphoriaDataManager::getModuleName(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t    MR_OUTPUT_DEBUG_ARG(moduleIndex)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return "Unknown";
  }

  const ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return "Unknown";
  }

  MR::InstanceDebugInterface* debugInterface = euphoriaCharacter->getDebugInterface();
  if (!debugInterface)
  {
    NMP_DEBUG_MSG("error: Instance has no MR::InstanceDebugInterface!");
    return "Unknown";
  }

  if (moduleIndex >= debugInterface->totalModules())
  {
    NMP_DEBUG_MSG("error: Valid module index expected!");
    return "Unknown";
  }

  const char* moduleName = debugInterface->getModuleName(moduleIndex);
  if (moduleName == 0)
  {
    moduleName = "Unknown";
  }
  return moduleName;
#else // defined(MR_OUTPUT_DEBUGGING)
  return "Unknown";
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::isModuleDebugEnabled(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t    MR_OUTPUT_DEBUG_ARG(moduleIndex)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return false;
  }

  MR::Dispatcher* dispatcher = network->getDispatcher();
  MR::InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();

  bool result = debugInterface->isModuleDebugEnabled(moduleIndex);
  return result;
#else // defined(MR_OUTPUT_DEBUGGING)
  return false;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
const ER::LimbControlAmounts* SimpleEuphoriaDataManager::getLimbControlAmounts(
  InstanceID MR_OUTPUT_DEBUG_ARG(instanceID), 
  uint8_t MR_OUTPUT_DEBUG_ARG(limbIndex),
  float& stiffnessFraction) const
{
  stiffnessFraction = 0.0f;
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  const ER::LimbInterface& limbInterface = euphoriaCharacter->getBody().getLimbInterface(limbIndex);
  return &limbInterface.getDebugControlAmounts(stiffnessFraction);
#else // defined(MR_OUTPUT_DEBUGGING)
  return 0;
#endif // defined(MR_OUTPUT_DEBUGGING)
}


//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::enableModuleDebug(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t    MR_OUTPUT_DEBUG_ARG(moduleIndex),
  bool        MR_OUTPUT_DEBUG_ARG(enable))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return "Unknown";
  }

  MR::Dispatcher* dispatcher = network->getDispatcher();
  MR::InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();

  debugInterface->enableModuleDebug(moduleIndex, enable);

  return true;
#else // defined(MR_OUTPUT_DEBUGGING)
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEuphoriaDataManager::getDebugControlCount(InstanceID NMP_UNUSED(id))
{
#if defined(MR_OUTPUT_DEBUGGING)
  uint32_t count = ER::getEuphoriaDebugControlCount();
  return count;
#else
  return 0;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::getDebugControlInfo(
  InstanceID                NMP_UNUSED(id),
  uint32_t                  MR_OUTPUT_DEBUG_ARG(index),
  const char**              MR_OUTPUT_DEBUG_ARG(controlName),
  ER::DebugControlID*       MR_OUTPUT_DEBUG_ARG(controlId),
  ER::DebugControlFlags*    MR_OUTPUT_DEBUG_ARG(controlFlags),
  ER::DebugControlDataType* MR_OUTPUT_DEBUG_ARG(controlDataType))
{
#if defined(MR_OUTPUT_DEBUGGING)
  bool result = ER::getEuphoriaDebugControlInfo(index, controlName, controlId, controlFlags, controlDataType);
  return result;
#else
  return false;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEuphoriaDataManager::getNumControlTypes(InstanceID NMP_UNUSED(id)) const
{
  return ER::debugControlMax;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleEuphoriaDataManager::getControlTypeName(InstanceID NMP_UNUSED(id), uint32_t controlTypeIndex)
{
  return ER::getDebugControlTypeName(controlTypeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::setDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  bool               MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::setEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::getDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  bool*              MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::getEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::setDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  int32_t            MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::setEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::getDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  int32_t*           MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::getEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::setDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  uint32_t           MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::setEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::getDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  uint32_t*          MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::getEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::setDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  float              MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::setEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEuphoriaDataManager::getDebugControl(
  InstanceID         MR_OUTPUT_DEBUG_ARG(instanceID),
  ER::DebugControlID MR_OUTPUT_DEBUG_ARG(controlId),
  uint8_t            MR_OUTPUT_DEBUG_ARG(limbIndex),
  float*             MR_OUTPUT_DEBUG_ARG(value))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  bool result = ER::getEuphoriaDebugControl(euphoriaCharacter, controlId, limbIndex, value);
  return result;
#else
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEuphoriaDataManager::serializeBodyTx(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  void*       MR_OUTPUT_DEBUG_ARG(outputBuffer),
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  uint32_t dataSize = sizeof(ER::EuphoriaBodyPersistentData);
  if (outputBuffer)
  {
    MR::Network* network = findNetworkByInstanceID(instanceID);
    if (!network)
    {
      NMP_DEBUG_MSG("error: Valid instance ID expected!");
      return 0;
    }

    const ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
    if (!euphoriaCharacter)
    {
      NMP_DEBUG_MSG("error: Instance has no ER::Character!");
      return 0;
    }

    const ER::Body& body = euphoriaCharacter->getBody();

    NMP_ASSERT(outputBufferSize >= dataSize);
    ER::EuphoriaBodyPersistentData* persistentData = (ER::EuphoriaBodyPersistentData*)outputBuffer;

    persistentData->m_rootLimbIndex = body.getRootLimbIndex();
    persistentData->m_numLimbs = body.getNumLimbs();
    persistentData->m_numArms = body.getNumArms();
    persistentData->m_numHeads = body.getNumHeads();
    persistentData->m_numLegs = body.getNumLegs();
    persistentData->m_numSpines = body.getNumSpines();
    persistentData->m_mass = body.getMass();
    persistentData->m_selfAvoidanceRadius = body.getSelfAvoidanceRadius();

    ER::EuphoriaBodyPersistentData::endianSwap(persistentData);
  }
  return dataSize;
#else
  NMP_ASSERT(outputBufferSize >= sizeof(ER::EuphoriaBodyPersistentData));
  return 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEuphoriaDataManager::getLimbCount(InstanceID MR_OUTPUT_DEBUG_ARG(instanceID)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return 0;
  }

  ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
  if (!euphoriaCharacter)
  {
    NMP_DEBUG_MSG("error: Instance has no ER::Character!");
    return 0;
  }

  ER::Body& body = euphoriaCharacter->getBody();
  return body.getNumLimbs();
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEuphoriaDataManager::serializeLimbTx(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t MR_OUTPUT_DEBUG_ARG(limbIndex),
  void* MR_OUTPUT_DEBUG_ARG(outputBuffer),
  uint32_t MR_OUTPUT_DEBUG_ARG(outputBufferSize)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  // only used in asserts when MR_OUTPUT_DEBUGGING is defined, there is no macro for that so we have to reference it.
  (void)outputBufferSize;

  uint32_t dataSize = sizeof(ER::EuphoriaLimbPersistentData);
  if (outputBuffer)
  {
    MR::Network* network = findNetworkByInstanceID(instanceID);
    if (!network)
    {
      NMP_DEBUG_MSG("error: Valid instance ID expected!");
      return 0;
    }

    const ER::Character* euphoriaCharacter = ER::networkGetCharacter(network);
    if (!euphoriaCharacter)
    {
      NMP_DEBUG_MSG("error: Instance has no ER::Character!");
      return 0;
    }

    const ER::Body& body = euphoriaCharacter->getBody();

    if (limbIndex >= (uint32_t)body.getNumLimbs())
    {
      NMP_DEBUG_MSG("error: Valid limb index expected!");
      return 0;
    }

    const ER::LimbInterface& limb = body.getLimbInterface(limbIndex);

    NMP_ASSERT(outputBufferSize >= dataSize);
    ER::EuphoriaLimbPersistentData* persistentData = (ER::EuphoriaLimbPersistentData*)outputBuffer;

    persistentData->m_name = MCOMMS::CommsServer::getInstance()->getTokenForString(limb.getName());
    persistentData->m_type = limb.getType();
    if (limb.isLeftLimb())
    {
      persistentData->m_limbSide = ER::kEuphoriaLeftLimb; 
    }
    else if (limb.isRightLimb())
    {
      persistentData->m_limbSide = ER::kEuphoriaRightLimb; 
    } 
    else
    {
      persistentData->m_limbSide = ER::kEuphoriaCentreLimb;
    }
    persistentData->m_rootIndex = limb.getRootIndex();
    persistentData->m_endIndex = limb.getEndIndex();
    persistentData->m_baseIndex = limb.getBaseIndex();
    persistentData->m_isRootLimb = limb.getIsRootLimb();
    persistentData->m_endEffectorlocalPose = limb.getDefinition()->m_endOffset;
    persistentData->m_rootEffectorlocalPose = limb.getDefinition()->m_rootOffset;

    ER::EuphoriaLimbPersistentData::endianSwap(persistentData);
  }
  return dataSize;
#else // defined(MR_OUTPUT_DEBUGGING)
  return 0;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

} // namespace COMMS
