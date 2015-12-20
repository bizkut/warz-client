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
#ifndef MCOMMS_EUPHORIAPACKETS_INL
#define MCOMMS_EUPHORIAPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaPersistentDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaPersistentDataPacket* EuphoriaPersistentDataPacket::create(
  NetworkDataBuffer*          buffer,
  ER::EuphoriaObjectDataTypes euphoriaType,
  uint32_t                    dataLength)
{
  uint32_t pktLen = sizeof(EuphoriaPersistentDataPacket) + dataLength;

  EuphoriaPersistentDataPacket* pkt = buffer->reserveAlignedMemory<EuphoriaPersistentDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_EuphoriaPersistentData, pktLen);

  pkt->m_euphoriaType = euphoriaType;
  pkt->m_dataLength = dataLength;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaPersistentDataPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  NMP::netEndianSwap(m_euphoriaType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaPersistentDataPacket::deserialize()
{
  NMP::netEndianSwap(m_euphoriaType);
  NMP::netEndianSwap(m_dataLength);

  if (m_dataLength == 0)
  {
    return;
  }

  // Find out what type of def data it is then swap it.
  void* data = getData();
  switch (m_euphoriaType)
  {
  case ER::kEuphoriaModuleDataType:
    ER::EuphoriaModulePersistentData::deserialiseTx(data);
    break;
  case ER::kEuphoriaBodyDataType:
    ER::EuphoriaBodyPersistentData::deserialiseTx(data);
    break;
  case ER::kEuphoriaLimbDataType:
    ER::EuphoriaLimbPersistentData::deserialiseTx(data);
    break;
  case ER::kEuphoriaDebugControlDataType:
    ER::EuphoriaDebugControlPersistentData::deserialiseTx(data);
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void* EuphoriaPersistentDataPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  return this + 1;
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaFrameDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaFrameDataPacket* EuphoriaFrameDataPacket::create(
  NetworkDataBuffer*          buffer,
  ER::EuphoriaObjectDataTypes  euphoriaType,
  uint32_t                    dataLength)
{
  uint32_t pktLen = sizeof(EuphoriaFrameDataPacket) + dataLength;

  EuphoriaFrameDataPacket* pkt = buffer->reserveAlignedMemory<EuphoriaFrameDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_EuphoriaFrameData, pktLen);

  pkt->m_euphoriaType = euphoriaType;
  pkt->m_dataLength = dataLength;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaFrameDataPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  NMP::netEndianSwap(m_euphoriaType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaFrameDataPacket::deserialize()
{
  NMP::netEndianSwap(m_euphoriaType);
  NMP::netEndianSwap(m_dataLength);

  if (m_dataLength == 0)
  {
    return;
  }

  // Find out what type of def data it is then swap it.
  void* data = getData();
  switch (m_euphoriaType)
  {
  case ER::kEuphoriaModuleDataType:
    ER::EuphoriaModuleFrameData::deserialiseTx(data);
    break;
  case ER::kEuphoriaBodyDataType:
  case ER::kEuphoriaLimbDataType:
    break;
  case ER::kEuphoriaDebugControlDataType:
    ER::EuphoriaDebugControlFrameData::deserialiseTx(data);
    break;
  case ER::kEuphoriaLimbControlAmountsDataType:
    ER::EuphoriaLimbControlAmountsFrameData::deserialiseTx(data);
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void* EuphoriaFrameDataPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  return this + 1;
}

//----------------------------------------------------------------------------------------------------------------------
/// EnableModuleDebugCmdPacket
//----------------------------------------------------------------------------------------------------------------------
inline EnableModuleDebugCmdPacket::EnableModuleDebugCmdPacket() :
   CmdPacketBase()
{
  PK_HEADER_CMD(EnableModuleDebugCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EnableModuleDebugCmdPacket::deserialize()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void EnableModuleDebugCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void EnableModuleDebugCmdPacket::endianSwap()
{
  NMP::netEndianSwap(m_instanceID);
  NMP::netEndianSwap(m_moduleIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlCmdPacket::EuphoriaDebugControlCmdPacket()
{
  PK_HEADER_CMD(EuphoriaDebugControlCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaDebugControlCmdPacket::deserialize()
{
  NMP::netEndianSwap(m_instanceID);
  ER::EuphoriaDebugControlFrameData::deserialiseTx(&m_controlData);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaDebugControlCmdPacket::serialize(
  InstanceID instanceID,
  uint8_t    controlIndex,
  uint8_t    limbIndex,
  bool       value)
{
  m_instanceID = instanceID;
  NMP::netEndianSwap(m_instanceID);
  ER::EuphoriaDebugControlFrameData::serialiseTx(&m_controlData, controlIndex, limbIndex, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaDebugControlCmdPacket::serialize(
  InstanceID instanceID,
  uint8_t    controlIndex,
  uint8_t    limbIndex,
  int32_t    value)
{
  m_instanceID = instanceID;
  NMP::netEndianSwap(m_instanceID);
  ER::EuphoriaDebugControlFrameData::serialiseTx(&m_controlData, controlIndex, limbIndex, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaDebugControlCmdPacket::serialize(
  InstanceID instanceID,
  uint8_t    controlIndex,
  uint8_t    limbIndex,
  uint32_t   value)
{
  m_instanceID = instanceID;
  NMP::netEndianSwap(m_instanceID);
  ER::EuphoriaDebugControlFrameData::serialiseTx(&m_controlData, controlIndex, limbIndex, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EuphoriaDebugControlCmdPacket::serialize(
  InstanceID instanceID,
  uint8_t    controlIndex,
  uint8_t    limbIndex,
  float      value)
{
  m_instanceID = instanceID;
  NMP::netEndianSwap(m_instanceID);
  ER::EuphoriaDebugControlFrameData::serialiseTx(&m_controlData, controlIndex, limbIndex, value);
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_EUPHORIAPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------
