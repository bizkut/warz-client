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
#include "comms/physicsRuntimeTargetSimple.h"

#include "morpheme/mrNetworkDef.h"
#include "physics/mrPhysics.h"

#include "comms/connection.h"
#include "comms/corePackets.h"
#include "comms/commsServer.h"
#include "comms/debugPackets.h"
#include "comms/physicsPackets.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
// SimplePhysicsDataManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SimplePhysicsDataManager::SimplePhysicsDataManager() :
  PhysicsDataManagementInterface()
{
}

//----------------------------------------------------------------------------------------------------------------------
MR::AnimSetIndex SimplePhysicsDataManager::getPhysicsRigDefCount(const GUID& guid)
{
  MR::NetworkDef* networkDef = findNetworkDefByGuid(guid);
  if (!networkDef)
  {
    NMP_DEBUG_MSG("error: Valid network GUID expected!");
    return 0;
  }

  return networkDef->getNumAnimSets();
}

//----------------------------------------------------------------------------------------------------------------------
bool SimplePhysicsDataManager::serializeTxPhysicsRigDefData(
  const GUID&      MR_OUTPUT_DEBUG_ARG(networkGUID),
  MR::AnimSetIndex MR_OUTPUT_DEBUG_ARG(index),
  Connection*      MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::NetworkDef* networkDef = findNetworkDefByGuid(networkGUID);
  if (!networkDef)
  {
    NMP_DEBUG_MSG("error: Valid network GUID expected!");
    return false;
  }

  MCOMMS::CommsServer* commsServer = MCOMMS::CommsServer::getInstance();

  MR::PhysicsRigDef* physicsRigDef = MR::getPhysicsRigDef(networkDef, index);
  if (physicsRigDef)
  {
    uint32_t rigDefDataSize = physicsRigDef->serializeTx(index, 0, 0);

    if (rigDefDataSize > 0)
    {
      MCOMMS::BeginPersistentPacket beginPersistentData(MR::kPhysicsRigDefPersistentData);
      mcommsBufferDataPacket(beginPersistentData, connection);

      MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();
      MCOMMS::PersistentDataPacket* physicsRigDefPacket = MCOMMS::PersistentDataPacket::create(
        networkDataBuffer,
        networkGUID,
        MR::kPhysicsRigDefPersistentData,
        rigDefDataSize);

      void* physicsRigData = physicsRigDefPacket->getData();

      NMP_USED_FOR_ASSERTS(uint32_t physicsRigUsedSize =)
        physicsRigDef->serializeTx(index, physicsRigData, rigDefDataSize);
      NMP_ASSERT(physicsRigUsedSize <= rigDefDataSize);

      mcommsSerializeDataPacket(*physicsRigDefPacket);

      uint32_t numParts = physicsRigDef->getNumParts();

      for (uint32_t i = 0; i != numParts; ++i)
      {
        MR::StringToken nameToken = commsServer->getTokenForString(physicsRigDef->m_parts[i].name);
        int32_t parentIndex = physicsRigDef->getParentPart(i);

        uint32_t partDataSize = physicsRigDef->m_parts[i].serializeTx(parentIndex, nameToken, 0, 0);

        if (partDataSize > 0)
        {
          MCOMMS::PhysicsPersistentDataPacket* partPacket = MCOMMS::PhysicsPersistentDataPacket::create(
            networkDataBuffer,
            MR::kPhysicsPartDataType,
            partDataSize);

          void* partData = partPacket->getData();

          NMP_USED_FOR_ASSERTS(uint32_t partDataUsedSize =)
            physicsRigDef->m_parts[i].serializeTx(parentIndex, nameToken, partData, partDataSize);
          NMP_ASSERT(partDataUsedSize <= partDataSize);

          mcommsSerializeDataPacket(*partPacket);
        }
      }

      uint32_t numJoints = physicsRigDef->getNumJoints();

      for (uint32_t i = 0; i != numJoints; ++i)
      {
        MR::StringToken nameToken = commsServer->getTokenForString(physicsRigDef->m_joints[i]->m_name);

        switch (physicsRigDef->m_joints[i]->m_jointType)
        {
        case MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF:
          {
            MR::PhysicsSixDOFJointDef* joint = static_cast<MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[i]);

            // kPhysicsJointDataType
            uint32_t dataSize = joint->serializeTx(nameToken, 0, 0);
            if (dataSize > 0)
            {
              MCOMMS::PhysicsPersistentDataPacket* jointPacket = MCOMMS::PhysicsPersistentDataPacket::create(
                networkDataBuffer,
                MR::kPhysicsJointDataType,
                dataSize);

              void* data = jointPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
              uint32_t usedSize =
#endif
                joint->serializeTx(nameToken, data, dataSize);
              NMP_ASSERT(usedSize <= dataSize);

              mcommsSerializeDataPacket(*jointPacket);
            }

            // kPhysicsSoftLimitDataType
            if (joint->hasSoftLimit())
            {
              dataSize = joint->serializeSoftLimitTx(nameToken, 0, 0);
              if (dataSize > 0)
              {
                MCOMMS::PhysicsPersistentDataPacket* jointPacket = MCOMMS::PhysicsPersistentDataPacket::create(
                  networkDataBuffer,
                  MR::kPhysicsSoftLimitDataType,
                  dataSize);

                void* data = jointPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
                uint32_t usedSize =
#endif
                  joint->serializeSoftLimitTx(nameToken, data, dataSize);
                NMP_ASSERT(usedSize <= dataSize);

                mcommsSerializeDataPacket(*jointPacket);
              }
            }
          }
          break;
        case MR::PhysicsJointDef::JOINT_TYPE_RAGDOLL:
          {
            MR::PhysicsRagdollJointDef* joint = static_cast<MR::PhysicsRagdollJointDef*>(physicsRigDef->m_joints[i]);

            // kPhysicsJointDataType
            uint32_t dataSize = joint->serializeTx(nameToken, 0, 0);
            if (dataSize > 0)
            {
              MCOMMS::PhysicsPersistentDataPacket* jointPacket = MCOMMS::PhysicsPersistentDataPacket::create(
                networkDataBuffer,
                MR::kPhysicsJointDataType,
                dataSize);

              void* data = jointPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
              uint32_t usedSize =
#endif
                joint->serializeTx(nameToken, data, dataSize);
              NMP_ASSERT(usedSize <= dataSize);

              mcommsSerializeDataPacket(*jointPacket);
            }
          }
          break;
        default:
          NMP_ASSERT_FAIL_MSG("Encountered unknown joint type %d", physicsRigDef->m_joints[i]->m_jointType);
          break;
        }
      }

      MCOMMS::EndPersistentPacket endPersistentData;
      mcommsBufferDataPacket(endPersistentData, connection);

      return true;
    }
  }

  return false;
#else // defined(MR_OUTPUT_DEBUGGING)
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimplePhysicsDataManager::serializeTxPhysicsRigPersistentData(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  uint32_t    MR_OUTPUT_DEBUG_ARG(index),
  Connection *MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return false;
  }

  MCOMMS::CommsServer* commsServer = MCOMMS::CommsServer::getInstance();
  NMP_ASSERT(commsServer);

  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(network, static_cast<MR::AnimSetIndex>(index));
  if (physicsRig)
  {
    // get size
    MR::PhysicsRigDef* physicsRigDef = physicsRig->getPhysicsRigDef();
    uint32_t physicsRigDataSize = physicsRig->serializeTxPersistentData(0, 0, 0);

    if (physicsRigDataSize > 0)
    {
      MCOMMS::BeginPersistentPacket beginPersistent(MR::kPhysicsRigPersistentData);
      mcommsBufferDataPacket(beginPersistent, connection);

      MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();
      MCOMMS::PersistentDataPacket* physicsRigDataPacket = MCOMMS::PersistentDataPacket::create(
        networkDataBuffer,
        instanceID,
        MR::kPhysicsRigPersistentData,
        physicsRigDataSize);

      void* physicsRigData = physicsRigDataPacket->getData();

      NMP_USED_FOR_ASSERTS(uint32_t physicsRigUsedSize =)
      physicsRig->serializeTxPersistentData(static_cast<MR::AnimSetIndex>(index), physicsRigData, physicsRigDataSize);
      NMP_ASSERT(physicsRigUsedSize <= physicsRigDataSize);

      mcommsSerializeDataPacket(*physicsRigDataPacket);

      uint32_t numParts = physicsRig->getNumParts();

      for (uint32_t i = 0; i != numParts; ++i)
      {
        const MR::PhysicsRig::Part* part = physicsRig->getPart(i);
        NMP_ASSERT(part);

        const char* partName = physicsRigDef->getPartName(i);
        MR::StringToken nameToken = commsServer->getTokenForString(partName);

        MR::PhysicsObjectID objectID = findPhysicsObjectIDForPart(part);

        uint32_t dataSize = part->serializeTxPersistentData(nameToken, objectID, 0, 0);

        if (dataSize > 0)
        {
          MCOMMS::PhysicsPersistentDataPacket* partPacket = MCOMMS::PhysicsPersistentDataPacket::create(
            networkDataBuffer,
            MR::kPhysicsPartDataType,
            dataSize);

          void* data = partPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
          uint32_t usedSize =
#endif
            part->serializeTxPersistentData(nameToken, objectID, data, dataSize);
          NMP_ASSERT(usedSize <= dataSize);

          mcommsSerializeDataPacket(*partPacket);

          NMP::Colour overrideColour;
          if (calculatePartCollisionSetColour(physicsRig, i, &overrideColour))
          {
            PhysicsPersistentDataPacket* overridePacket = PhysicsPersistentDataPacket::create(
              networkDataBuffer,
              MR::kPhysicsPartOverrideColourDataType,
              sizeof(MR::PhysicsPartOverrideColour));

            MR::PhysicsPartOverrideColour* overrideData = (MR::PhysicsPartOverrideColour*)(overridePacket->getData());

            overrideData->m_overrideColour = overrideColour;
            overrideData->m_physicsObjectID = objectID;
            overrideData->m_overrideTypeID = MR::PhysicsPartOverrideColour::kCollisionSetOverrideTypeID;

            NMP::netEndianSwap(overrideData->m_physicsObjectID);
            NMP::netEndianSwap(overrideData->m_overrideTypeID);

            mcommsSerializeDataPacket(*overridePacket);
          }
        }
      }

      uint32_t numJoints = physicsRig->getNumJoints();

      for (uint32_t i = 0; i != numJoints; ++i)
      {
        const MR::PhysicsRig::Joint* joint = physicsRig->getJoint(i);
        const MR::PhysicsJointDef* jointDef = physicsRigDef->m_joints[i];
        MR::StringToken nameToken = commsServer->getTokenForString(jointDef->m_name);

        // kPhysicsJointDataType
        uint32_t dataSize = joint->serializeTxPersistentData(jointDef, nameToken, 0, 0);
        if (dataSize > 0)
        {
          MCOMMS::PhysicsPersistentDataPacket* jointPacket = MCOMMS::PhysicsPersistentDataPacket::create(
            networkDataBuffer,
            MR::kPhysicsJointDataType,
            dataSize);

          void* data = jointPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
          uint32_t usedSize =
#endif
            joint->serializeTxPersistentData(jointDef, nameToken, data, dataSize);
          NMP_ASSERT(usedSize <= dataSize);

          mcommsSerializeDataPacket(*jointPacket);
        }

        if (jointDef->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF)
        {
          const MR::PhysicsSixDOFJointDef* sixDOFDef =
            static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[i]);

          // kPhysicsSoftLimitDataType
          if (sixDOFDef->hasSoftLimit())
          {
            dataSize = sixDOFDef->serializeSoftLimitTx(nameToken, 0, 0);
            if (dataSize > 0)
            {
              MCOMMS::PhysicsPersistentDataPacket* limitPacket = MCOMMS::PhysicsPersistentDataPacket::create(
                networkDataBuffer,
                MR::kPhysicsSoftLimitDataType,
                dataSize);

              void* data = limitPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
              uint32_t usedSize =
#endif
                sixDOFDef->serializeSoftLimitTx(nameToken, data, dataSize);
              NMP_ASSERT(usedSize <= dataSize);

              mcommsSerializeDataPacket(*limitPacket);
            }
          }
        }
      }

      MCOMMS::EndPersistentPacket endPersistent;
      mcommsBufferDataPacket(endPersistent, connection);

      return true;
    }
  }

  return false;
#else // defined(MR_OUTPUT_DEBUGGING)
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
bool SimplePhysicsDataManager::serializeTxPhysicsRigFrameData(
  InstanceID  MR_OUTPUT_DEBUG_ARG(instanceID),
  Connection *MR_OUTPUT_DEBUG_ARG(connection))
{
#if defined(MR_OUTPUT_DEBUGGING)
  MR::Network* network = findNetworkByInstanceID(instanceID);
  NMP_ASSERT(network);
  if (!network)
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected!");
    return false;
  }
  uint32_t index = network->getActiveAnimSetIndex();

  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(network);
  if (physicsRig != 0)
  {
    // get size
    uint32_t physicsRigDataSize = physicsRig->serializeTxFrameData(0, 0, 0);

    if (physicsRigDataSize > 0)
    {
      BeginInstanceSectionPacket beginInstanceData(MCOMMS::kPhysicsDataSectionType);
      mcommsBufferDataPacket(beginInstanceData, connection);

      MCOMMS::NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();
      MCOMMS::FrameDataPacket* physicsRigDataPacket = MCOMMS::FrameDataPacket::create(
        networkDataBuffer,
        instanceID,
        MR::kPhysicsRigFrameData,
        physicsRigDataSize);

      void* physicsRigData = physicsRigDataPacket->getData();

      NMP_USED_FOR_ASSERTS(uint32_t physicsRigUsedSize =)
      physicsRig->serializeTxFrameData(static_cast<MR::AnimSetIndex>(index), physicsRigData, physicsRigDataSize);
      NMP_ASSERT(physicsRigUsedSize <= physicsRigDataSize);

      mcommsSerializeDataPacket(*physicsRigDataPacket);

      uint32_t numParts = physicsRig->getNumParts();
      for (uint32_t i = 0; i != numParts; ++i)
      {
        const MR::PhysicsRig::Part* part = physicsRig->getPart(i);
        NMP_ASSERT(part != 0);

        uint32_t dataSize = part->serializeTxFrameData(0, 0);
        if (dataSize != 0)
        {
          MCOMMS::PhysicsFrameDataPacket* partPacket = MCOMMS::PhysicsFrameDataPacket::create(
            networkDataBuffer,
            MR::kPhysicsPartDataType,
            dataSize);

          void* data = partPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
          uint32_t usedSize =
#endif
            part->serializeTxFrameData(data, dataSize);
          NMP_ASSERT(usedSize <= dataSize);

          mcommsSerializeDataPacket(*partPacket);
        }
      }

      uint32_t numJoints = physicsRig->getNumJoints();
      for (uint32_t i = 0; i != numJoints; ++i)
      {
        const MR::PhysicsRig::Joint* joint = physicsRig->getJoint(i);
        NMP_ASSERT(joint != 0);

        uint32_t dataSize = joint->serializeTxFrameData(0, 0);

        if (dataSize != 0)
        {
          MCOMMS::PhysicsFrameDataPacket* jointPacket = MCOMMS::PhysicsFrameDataPacket::create(
            networkDataBuffer,
            MR::kPhysicsJointDataType,
            dataSize);

          void* data = jointPacket->getData();
#if defined(NMP_ENABLE_ASSERTS)
          uint32_t usedSize =
#endif
            joint->serializeTxFrameData(data, dataSize);
          NMP_ASSERT(usedSize <= dataSize);

          mcommsSerializeDataPacket(*jointPacket);
        }
      }

      MCOMMS::EndInstanceSectionPacket endInstanceData(MCOMMS::kPhysicsDataSectionType);
      mcommsBufferDataPacket(endInstanceData, connection);

      return true;
    }
  }

  return false;
#else // defined(MR_OUTPUT_DEBUGGING)
  return true;
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void SimplePhysicsDataManager::onEnvironmentVariableChange(const Attribute* NMP_UNUSED(attribute))
{
}

//----------------------------------------------------------------------------------------------------------------------
bool SimplePhysicsDataManager::calculatePartCollisionSetColour(
  MR::PhysicsRig* physicsRig,
  uint32_t partIndex,
  NMP::Colour* setColour) const
{
  MR::PhysicsRigDef* physicsRigDef = physicsRig->getPhysicsRigDef();

  if (physicsRigDef->m_numCollisionGroups > 0)
  {
    float blendedHue = 0.0f;
    int32_t numCollisionSets = 0; // number of collision sets this part belongs to
    for (int32_t i = 0; i < physicsRigDef->m_numCollisionGroups; ++i)
    {
      float setHue = (float)i / (float)physicsRigDef->m_numCollisionGroups;

      MR::PhysicsRigDef::CollisionGroup* set = &physicsRigDef->m_collisionGroups[i];
      for (int32_t j = 0; j != set->numIndices; ++j)
      {
        if (set->indices[j] == (int32_t)partIndex)
        {
          blendedHue += setHue;
          ++numCollisionSets;
          break;
        }
      }
    }

    if (numCollisionSets > 0)
    {
      // take an average of all the set hues the part belonged to.
      blendedHue = blendedHue / (float)numCollisionSets;
      setColour->setFromHSV(blendedHue, 0.8f, 0.8f);
      setColour->setA(255);
      return true;
    }
  }
  
  return false;
}

} // namespace COMMS
