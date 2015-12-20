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
#ifndef MCOMMS_PHYSICSPACKETS_INL
#define MCOMMS_PHYSICSPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPersistentDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPersistentDataPacket* PhysicsPersistentDataPacket::create(
  NetworkDataBuffer*          buffer,
  MR::PhysicsObjectDataTypes  physicsType,
  uint32_t                    dataLength)
{
  uint32_t pktLen = sizeof(PhysicsPersistentDataPacket) + dataLength;

  PhysicsPersistentDataPacket* pkt = buffer->reserveAlignedMemory<PhysicsPersistentDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_PhysicsPersistentData, pktLen);

  pkt->m_physicsType = physicsType;
  pkt->m_dataLength = dataLength;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsPersistentDataPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  NMP::netEndianSwap(m_physicsType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsPersistentDataPacket::deserialize()
{
  NMP::netEndianSwap(m_physicsType);
  NMP::netEndianSwap(m_dataLength);

  if (m_dataLength == 0)
  {
    return;
  }

  // Find out what type of def data it is then swap it.
  void *data = getData();
  switch (m_physicsType)
  {
  case MR::kPhysicsPartDataType:
    MR::PhysicsPartPersistentData::deserialiseTx(data);
    break;
  case MR::kPhysicsJointDataType:
    {
      MR::PhysicsJointPersistentData::JointTypes jointType = static_cast<MR::PhysicsJointPersistentData*>(data)->m_jointType;
      NMP::netEndianSwap(jointType);
      switch (jointType)
      {
      case MR::PhysicsJointPersistentData::JOINT_TYPE_SIX_DOF:
        MR::PhysicsSixDOFJointPersistentData::deserialiseTx(data);
        break;
      case MR::PhysicsJointPersistentData::JOINT_TYPE_RAGDOLL:
        MR::PhysicsRagdollJointPersistentData::deserialiseTx(data);
        break;
      }
      break;
    }
  case MR::kPhysicsSoftLimitDataType:
    MR::PhysicsSoftLimitPersistentData::deserialiseTx(data);
    break;
  case MR::kPhysicsPartOverrideColourDataType:
    MR::PhysicsPartOverrideColour::deserialiseTx(data);
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void *PhysicsPersistentDataPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  return this + 1;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsFrameDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsFrameDataPacket* PhysicsFrameDataPacket::create(
  NetworkDataBuffer*          buffer,
  MR::PhysicsObjectDataTypes  physicsType,
  uint32_t                    dataLength)
{
  uint32_t pktLen = sizeof(PhysicsFrameDataPacket) + dataLength;

  PhysicsFrameDataPacket* pkt = buffer->reserveAlignedMemory<PhysicsFrameDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_PhysicsFrameData, pktLen);

  pkt->m_physicsType = physicsType;
  pkt->m_dataLength = dataLength;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsFrameDataPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  NMP::netEndianSwap(m_physicsType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsFrameDataPacket::deserialize()
{
  NMP::netEndianSwap(m_physicsType);
  NMP::netEndianSwap(m_dataLength);

  if (m_dataLength == 0)
  {
    return;
  }

  // Find out what type of def data it is then swap it.
  void *data = getData();
  switch (m_physicsType)
  {
  case MR::kPhysicsPartDataType:
    MR::PhysicsPartFrameData::deserialiseTx(data);
    break;
  case MR::kPhysicsJointDataType:
    {
      MR::PhysicsJointFrameData::JointTypes jointType = static_cast<MR::PhysicsJointFrameData*>(data)->m_jointType;
      NMP::netEndianSwap(jointType);
      switch (jointType)
      {
      case MR::PhysicsJointFrameData::JOINT_TYPE_SIX_DOF:
        MR::PhysicsSixDOFJointFrameData::deserialiseTx(data);
        break;
      case MR::PhysicsJointFrameData::JOINT_TYPE_RAGDOLL:
        MR::PhysicsRagdollJointFrameData::deserialiseTx(data);
        break;
      }
      break;
    }
  case MR::kPhysicsSoftLimitDataType:
    break;
  case MR::kPhysicsPartOverrideColourDataType:
    MR::PhysicsPartOverrideColour::deserialiseTx(data);
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void *PhysicsFrameDataPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  return this + 1;
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_PHYSICSPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------
